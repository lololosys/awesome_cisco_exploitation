/* $Id: main_4k.c,v 3.3.6.1 1996/03/21 23:21:22 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/os/main_4k.c,v $
 *------------------------------------------------------------------
 * main_4k.c -- System entry point for MIPS R4k systems
 * 
 * Michael Beesley, November 1993
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: main_4k.c,v $
 * Revision 3.3.6.1  1996/03/21  23:21:22  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.2.2.4  1996/03/18  23:26:19  mbeesley
 * Branch: ELC_branch
 * Sync to V111_1_3
 *
 * Revision 3.2.2.3  1996/02/10  00:18:08  mbeesley
 * CSCdi48581:  Add VIP2 support to ELC_branch
 * Branch: ELC_branch
 *
 * Revision 3.2.2.2  1995/12/20  20:28:29  wfried
 * Branch: ELC_branch
 *
 * Revision 3.2.2.1  1995/11/22  20:31:33  mbeesley
 * Initial commit of c7100 support.
 * Branch: ELC_branch
 *
 * Revision 3.3  1996/03/06  21:26:17  dlobete
 * CSCdi49840:  Support for 32 MB DRAM required for DFS on RVIP
 * Change name pf pick_dram_page() to pick_tlb_page() for easier
 * synching for ELC.
 * Declare pick_tlb_page() non-static.
 *
 * Revision 3.2  1995/11/17  18:49:03  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:46:38  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/11/08  21:19:03  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.2  1995/07/25  17:45:04  mleelani
 * CSCdi37536:  Fix the C4700 Millisecond Ticks.
 * Add support for the 133.33 mhz pipe.
 *
 * Revision 2.1  1995/06/07  21:56:27  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../src-4k/vspace_4k.h"
#include "../dev/monitor1.h"
#include "../os/gdb.h"

/*
 * Define some literals for memory sizes
 */

#define MEM_32M           0x2000000
#define MEM_8M            0x800000
#define MEM_2M            0x200000
#define MEM_512K          0x80000
#define MEM_128K          0x20000
#define MEM_32K           0x8000
#define MEM_8K            0x2000

#define SMALLEST_PAGE     MEM_8K

#define BOOTLOAD	  2

/*
 * external and forward declarations 
 */
extern uchar _gp[];
extern void r4k_cpu_level_init(void);

static boolean call_entry_point(boolean (*)(int, boolean), int, boolean);

#ifdef DEBUG_CACHING
#define CPUMEM_CACHE_POLICY	TLB_UNCACHED
#else
#define CPUMEM_CACHE_POLICY	TLB_CACHE_NONCOHERENT
#endif

/*
 * __start : The world begins here for MIPS R4K platforms.
 *           This must be the first code in the text segment
 */
boolean 
__start (int code, boolean arg)
{
    ulong temp, paddr, vaddr;
    vspace_tbl_t *vspace;
    struct r4k_tlb_t tlb_e;
    long size, pmask, bytes, mem_size, pfn0, pfn1;

    /*
     * We are running on the Scheduler stack, through KSeg0/1.
     * We must initialize the virtual space definition, check that
     * the CPU status and config registers are to our liking and call
     * the common system entry point (_start). We ensure that BEV is set
     * making sure that exceptions go to the monitor through EPROM space.
     * init_exceptions() will setup our vectors and set the chip to normal
     * exception vectors.
     *
     * Note : Initially we'll be paranoid and flush the caches.
     */
    
    /*
     * program the gp register
     */
    asm volatile ("la $8,_gp; move $28,$8"
	: 				/* no output variables */
	: );				/* no input variables */

    /*
     * setcp0_config() will flush the caches to handle
     * the case where KSeg0 cache attribute is different 
     * between the ROM monitor and the system image.
     * The system image will honor the monitor's setting of DE
     * to allow the same image to run on boards with or without 
     * main DRAM parity.
     *
     * IE bit is not enabled
     */
    temp = getcp0_sreg();
    setcp0_sreg(R4K_SREG_SETUP | (temp & SREG_DE));
    setcp0_config(R4K_CONFIG_SETUP);

    /*
     * Get the DRAM size from the ROM Monitor
     */
    mem_size = mon_getmemsize();

    /*
     * Now program the virtual address space definition for
     * all hardware except the DRAM. Sierra and RSP have a one-to-one
     * direct map between physical and virtual space. Checkers and VIPER
     * do not.
     *
     * Start by initializing all TLB entries to be local, invalid
     */
    for (temp=0; temp<MAX_TLB_ENTRIES; temp++) {
	r4k_gettlb(&tlb_e, temp);
	tlb_e.pmask.mask = 0;
	tlb_e.hi.vpn2 = 0;
	tlb_e.hi.asid = 1;
	tlb_e.lo0.pfn = tlb_e.lo1.pfn = 0;
	tlb_e.lo0.v = tlb_e.lo1.v = 0;		/* Invalid 	*/
	tlb_e.lo0.g = tlb_e.lo1.g = 0;		/* Local	*/
	r4k_settlb(&tlb_e, temp, FALSE /* don't clear cache */);
    }

    vspace = vspace_table_addr(&size);  /* PIC magic to get the virtual
					   space table addr and size */
    if ( size > MAX_TLB_ENTRIES ) {     /* hosed, too many entries */
	return(0);
    }

    /*
     * Each TLB entry is actually a pair of entries, one mapping
     * the even page (lo0) and the other mapping the odd page (lo1)
     */
    for (temp=0; temp<size; temp++, vspace++) {

	/*
	 * Setup page size and vaddr
	 */
	tlb_e.pmask.mask = vspace->page_mask;
	tlb_e.hi.vpn2 = (vspace->vaddr >> TLB_HI_VPN2SHIFT);

	/*
	 * Setup paddr
	 */
	paddr = (vspace->paddr >> (TLB_HI_VPN2SHIFT - 1 + vspace->pshift));
	tlb_e.lo0.pfn = paddr << vspace->pshift;
	tlb_e.lo1.pfn = (paddr + 1) << vspace->pshift;
	
	/*
	 * Or in the settings for paddr [35..32]
	 */
	tlb_e.lo0.pfn |= (vspace->paddr_a35_a32 << ADDR_BIT35_32_SHIFT);
	tlb_e.lo1.pfn |= (vspace->paddr_a35_a32 << ADDR_BIT35_32_SHIFT);

	/*
	 * Setup page characteristics
	 */
	tlb_e.lo0.c = tlb_e.lo1.c = vspace->cache;
	tlb_e.lo0.d = tlb_e.lo1.d = vspace->dirty;
	tlb_e.lo0.v = tlb_e.lo1.v = vspace->valid;
	tlb_e.lo0.g = tlb_e.lo1.g = vspace->global;
	r4k_settlb(&tlb_e, temp, FALSE);
    }

    /*
     * The "text" segment is defined as starting at the start
     * of memory and continuing to the end of the text segment IE
     * it covers lowcore, fslib and the text segment, read only
     */
    tlb_e.lo0.c = tlb_e.lo1.c = CPUMEM_CACHE_POLICY;
    tlb_e.lo0.d = tlb_e.lo1.d = TLB_RO;
    tlb_e.lo0.v = tlb_e.lo1.v = TLB_VALID;
    tlb_e.lo0.g = tlb_e.lo1.g = TLB_GLOBAL;
    size = (ulong)TEXT_END - RAMBASE;
    if ( size > mem_size ) {
	return(0);
    }
    paddr = PHY_RAMBASE;
    vaddr = RAMBASE;

    /*
     * Loop until at least all the text segment is covered
     */
    while (size > 0) {

	if (temp >= MAX_TLB_ENTRIES) {       /* should never happen */
	    return(0);
	}
	
	/*
	 * Pick a page size for this entry
	 */
	pmask = pick_tlb_page(paddr, size, &pfn0, &pfn1, &bytes);
	if (pmask == -1) {
	    return(0);                       /* should never happen */
	}
	tlb_e.lo0.pfn = pfn0;
	tlb_e.lo1.pfn = pfn1;

	/*
	 * Control the upper address bits [35..32]
	 */
	tlb_e.lo0.pfn |= DRAM_ADDR33;
	tlb_e.lo1.pfn |= DRAM_ADDR33;
	
	tlb_e.pmask.mask = pmask;
	tlb_e.hi.vpn2 = (vaddr >> TLB_HI_VPN2SHIFT);
	r4k_settlb(&tlb_e, temp, FALSE /* don't clear cache */);
	paddr += bytes;
	vaddr += bytes;
	size -= bytes;
	temp += 1;
    }

    /*
     * Next job is to program the rest of main DRAM into 
     * the virtual address space definition using the remainder of the TLB
     */
    tlb_e.lo0.d = tlb_e.lo1.d = 1;          /* set read/write */
    size = mem_size - (paddr - PHY_RAMBASE);

    while ( size > 0 ) {

	if (temp >= MAX_TLB_ENTRIES) {      /* should never happen */
	    return(0);
	}

	/*
	 * Pick a page size for this entry
	 */
	pmask = pick_tlb_page(paddr, size, &pfn0, &pfn1, &bytes);
	if (pmask == -1) {
	    return(0);                       /* should never happen */
	}
	tlb_e.lo0.pfn = pfn0;
	tlb_e.lo1.pfn = pfn1;

	/*
	 * Control the upper address bits [35..32]
	 */
	tlb_e.lo0.pfn |= DRAM_ADDR33;
	tlb_e.lo1.pfn |= DRAM_ADDR33;

	tlb_e.pmask.mask = pmask;
	tlb_e.hi.vpn2 = (vaddr >> TLB_HI_VPN2SHIFT);
	r4k_settlb(&tlb_e, temp, FALSE /* don't clear cache */);
	paddr += bytes;
	vaddr += bytes;
	size -= bytes;
	temp += 1;
    }

    /*
     * Initialize ASID
     */
    r4k_setasid(0);

    /*
     * Flush the caches again, in preparation for the 
     * call via virtual address space to the common system start point
     */
    cache_flush();

    /*
     * Record the number of tlb entries used
     */
    tlb_entries_used = temp;

    /*
     * Call the common system entry point. We call a dummy routine with
     * the address of _start, which will in turn call _start.
     * This is to ensure that the compiler produces :
     *
     *         li   t0,_start
     *         jal  t0
     *
     * which will launch us into our desired virtual space
     */
    return( call_entry_point(TEXT_START, code, arg));
}

/*
 * pick_tlb_page :
 * Pick the largest page size that will cover the desired 
 * bytes, wasting no more than 8K and not violating alignment rules.
 */
int pick_tlb_page (long addr, long size, long *pfn0, long *pfn1, long *bytes)
{
    long pfn;

    /*
     * Check 16M page for fit
     */
    if ((MEM_32M - SMALLEST_PAGE) <= size) {       /* 32M OK */
	if ((addr % MEM_32M) == 0) {
	    pfn = addr >> (TLB_HI_VPN2SHIFT - 1 + TLB_16M_SHIFT);
	    *pfn0 = pfn << TLB_16M_SHIFT;
	    *pfn1 = (pfn + 1) << TLB_16M_SHIFT;
	    *bytes = MEM_32M;
	    return(TLB_PAGESIZE_16M);
	}
    }

    /*
     * Check 4M page for fit
     */
    if ((MEM_8M - SMALLEST_PAGE) <= size) {        /* 8M OK */
	if ((addr % MEM_8M) == 0) {
	    pfn = addr >> (TLB_HI_VPN2SHIFT - 1 + TLB_4M_SHIFT);
	    *pfn0 = pfn << TLB_4M_SHIFT;
	    *pfn1 = (pfn + 1) << TLB_4M_SHIFT;
	    *bytes = MEM_8M;
	    return(TLB_PAGESIZE_4M);
	}
    }

    /*
     * Check 1M page for fit
     */
    if ((MEM_2M - SMALLEST_PAGE) <= size) {        /* 2M OK */
	if ((addr % MEM_2M) == 0) {
	    pfn = addr >> (TLB_HI_VPN2SHIFT - 1 + TLB_1M_SHIFT);
	    *pfn0 = pfn << TLB_1M_SHIFT;
	    *pfn1 = (pfn + 1) << TLB_1M_SHIFT;
	    *bytes = MEM_2M;
	    return(TLB_PAGESIZE_1M);
	}
    }

    /*
     * Check 256K page for fit
     */
    if ((MEM_512K - SMALLEST_PAGE) <= size) {      /* 512K OK */
	if ((addr % MEM_512K) == 0) {
	    pfn = addr >> (TLB_HI_VPN2SHIFT - 1 + TLB_256K_SHIFT);
	    *pfn0 = pfn << TLB_256K_SHIFT;
	    *pfn1 = (pfn + 1) << TLB_256K_SHIFT;
	    *bytes = MEM_512K;
	    return(TLB_PAGESIZE_256K);
	}
    }

    /*
     * Check 64K page for fit
     */
    if ((MEM_128K - SMALLEST_PAGE) <= size) {      /* 128K OK */
	if ((addr % MEM_128K) == 0) {
	    pfn = addr >> (TLB_HI_VPN2SHIFT - 1 + TLB_64K_SHIFT);
	    *pfn0 = pfn << TLB_64K_SHIFT;
	    *pfn1 = (pfn + 1) << TLB_64K_SHIFT;
	    *bytes = MEM_128K;
	    return(TLB_PAGESIZE_64K);
	}
    }

    /*
     * Check 16K page for fit
     */
    if ((MEM_32K - SMALLEST_PAGE) <= size) {       /* 32K OK */
	if ((addr % MEM_32K) == 0) {
	    pfn = addr >> (TLB_HI_VPN2SHIFT - 1 + TLB_16K_SHIFT);
	    *pfn0 = pfn << TLB_16K_SHIFT;
	    *pfn1 = (pfn + 1) << TLB_16K_SHIFT;
	    *bytes = MEM_32K;
	    return(TLB_PAGESIZE_16K);
	}
    }

    /*
     * We are only left with 4K page. Check the alignment.
     */
    if ((addr % MEM_8K) == 0) {
	pfn = addr >> (TLB_HI_VPN2SHIFT - 1 + TLB_4K_SHIFT);
	*pfn0 = pfn << TLB_4K_SHIFT;
	*pfn1 = (pfn + 1) << TLB_4K_SHIFT;
	*bytes = MEM_8K;
	return(TLB_PAGESIZE_4K);
    } else {
	return(-1);     /* dead !!! */
    }
}

/*
 * call_entry_point :
 * Dummy function to call the main system entry point. This
 * will ensure that the code dereferences off of a register
 * resulting in us entering the desired virtual space. We also
 * call the cpu level initialization routine to get everything straight
 * there.
 */
static boolean
call_entry_point (boolean (*fnc)(int, boolean), int code, boolean arg)
{
    ulong sreg;
    boolean temp;

    /*
     * processor interrupts are still disabled via IE
     *
     * initialize interrupt mask bits in status register and
     * external interrupt mask register
     *
     * approximately equal to set_interrupt_level(LEVEL7);
     */
    r4k_cpu_level_init();
    
    /*
     * Analyze the config register and setup r4k_1ms_pipe_ticks
     * and r4k_4ms_pipe_ticks. Support divide by 2 (100Mhz), divide
     * by 3 (150Mhz), divide by 4 (200Mhz). Note, assuming that
     * the system bus is always 50 Mhz.
     */
    platform_r4k_set_clock_params(getcp0_config());
    
    /*
     * Look for a level 2 cache, and if it is there flush it
     */
    if (level2_cache_detect()) {
	level2_cache_flush();
    }

    /*
     * enable interrupts, but leave everything masked
     */
    sreg = getcp0_sreg();
    sreg |= SREG_IE;
    setcp0_sreg(sreg);

    /*
     * call the entry point. Below is kind of a hack. On r4k boxes
     * that have a rom monitor, PHY_RAMBASE equals RAMBASE. On
     * other r4k platforms that run an imbedded IOS, this is not true.
     * These platforms need hardcoded arguments to _start.
     */
#if defined(RVIP) || defined(SVIP)
	temp = (*fnc)(BOOTLOAD, 0);
#else
	temp = (*fnc)(code, arg);
#endif

    /*
     * We will return to the monitor. Disable interrupts and quieten
     * the watchdog
     */
    sreg = getcp0_sreg();
    disable_watchdog();
    setcp0_sreg(sreg & ~SREG_IE);
    return(temp);
}

/* End of File */
