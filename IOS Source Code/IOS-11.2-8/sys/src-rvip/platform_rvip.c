/* $Id: platform_rvip.c,v 3.13.6.14 1996/09/06 01:12:22 snyder Exp $
 * $Source: /release/112/cvs/Xsys/src-rvip/platform_rvip.c,v $
 *------------------------------------------------------------------
 * platform_rvip.c - Platform specific support for RVIP
 *
 * May 1995, Michael Beesley
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: platform_rvip.c,v $
 * Revision 3.13.6.14  1996/09/06  01:12:22  snyder
 * CSCdi68277:  make shr_core, shr_ukernel, sub_cxxxx sub_core_platform
 *              things const, save 712 out of data, 208 from image
 * Branch: California_branch
 *
 * Revision 3.13.6.13  1996/08/28  13:14:46  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.13.6.12  1996/06/27  18:11:13  avaz
 * CSCdi41715:  vip1 flash rom and vpld versions not right
 * Branch: California_branch
 *
 * Revision 3.13.6.11  1996/06/19  04:44:36  smackie
 * Buffered logging support gets a hot wax and shine. (CSCdi53462)
 *
 *   o Restructure buffered logging to be a independant device
 *   o Add support for "clear logging"
 *   o Enable buffered logging by default on most platforms
 *   o Start hiding logger structures in private include files
 *
 * Branch: California_branch
 *
 * Revision 3.13.6.10  1996/05/23  03:58:13  kao
 * CSCdi58288:  inconsistent ip cache in VIP after mic rel with traffic
 * Branch: California_branch
 *
 * Revision 3.13.6.9  1996/05/17  11:54:46  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.11.2.4  1996/05/05  23:54:04  ppearce
 * Sync to IbuMod_Calif_baseline_960504
 *
 * Revision 3.11.2.3  1996/04/26  07:57:51  dstine
 * Branch: IbuMod_Calif_branch
 * - commit of IBU modularity work, phase 1.
 *
 * Revision 3.11.2.2  1996/04/03  21:43:54  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.11.2.1  1996/03/17  18:31:34  ppearce
 * Sync IbuMod_Calif_branch to V111_1_3
 *
 * Revision 3.13.6.8  1996/05/01  13:56:23  dcarroll
 * CSCdi56037:  provide oir active check routine
 * Branch: California_branch
 * Need 'oir active check' routine.
 *
 * Revision 3.13.6.7  1996/04/12  17:43:31  kao
 * CSCdi53527:  Need EOIR support on VIP2
 * Branch: California_branch
 *
 * Revision 3.13.6.6  1996/04/05  06:30:41  dcarroll
 * CSCdi53603:  avoid hogging cpu in analyze path
 * Branch: California_branch
 * Add platform dependant delay routines.
 *
 * Revision 3.13.6.5  1996/04/05  05:33:07  dcarroll
 * CSCdi53595:  allow for multiple passes through pas idb init code
 * Branch: California_branch
 * Break up the init idb routines and keep track of idb init state.
 *
 * Revision 3.13.6.4  1996/04/04  19:45:39  tboyle
 * CSCdi51923:  Dynamic particle sizes for VIP2 based on SRAM size
 * Branch: California_branch
 *
 * Revision 3.13.6.3  1996/04/03  22:49:19  jwelder
 * CSCdi47348:  lingering references to rvip
 * Branch: California_branch
 *
 * Revision 3.13.6.2  1996/03/21  23:50:34  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.13.6.1  1996/03/18  22:03:12  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.2.11  1996/03/18  23:35:47  mbeesley
 * Branch: ELC_branch
 * Sync to V111_1_3
 *
 * Revision 3.3.2.10  1996/03/07  17:29:18  getchell
 * CSCdi50953:  viper dfs mods
 * Branch: ELC_branch
 *
 * Revision 3.3.2.9  1996/02/24  00:02:44  mbeesley
 * CSCdi49759:  VIP1 images do not boot on RSP, noisy on RP
 * Forgot to tell the RP/RSP that the VIP1 was ready.
 * Branch: ELC_branch
 *
 * Revision 3.3.2.8  1996/02/16  02:24:35  mbeesley
 * CSCdi48910:  Add storage of fault history for crashes
 * Branch: ELC_branch
 *
 * Revision 3.3.2.7  1996/02/10  00:18:53  mbeesley
 * CSCdi48581:  Add VIP2 support to ELC_branch
 * Branch: ELC_branch
 *
 * Revision 3.3.2.6  1996/02/08  09:30:51  gstovall
 * Branch: ELC_branch
 * Sync to ELC_baseline_960205.
 *
 * Revision 3.3.2.5  1996/01/16  02:36:05  dcarroll
 * Branch: ELC_branch
 * Sync ELC_branch between ELC_baseline_120995 and ELC_baseline_960110.
 *
 * Revision 3.3.2.4  1995/12/23  04:42:02  fsunaval
 * Branch: ELC_branch
 * 1.First pass at FDDI port adapter.
 *
 * Revision 3.3.2.3  1995/12/11  19:24:44  mbeesley
 * Branch: ELC_branch
 * Sync ELC_branch to ELC_baseline_120995.
 *
 * Revision 3.3.2.2  1995/12/05  06:36:20  mbeesley
 * Branch: ELC_branch
 * Safeguard static paktype headers from being freed.
 *
 * Revision 3.3.2.1  1995/11/30  02:09:55  dcarroll
 * Branch: ELC_branch
 *
 * Revision 3.13  1996/03/06  21:24:41  dlobete
 * CSCdi49840:  Support for 32 MB DRAM required for DFS on RVIP
 * Map the DRAM according to the size we dynamically sense on board.
 * - For 8 MB RVIPs we'll have 6 MB DRAM and 3 MB of PCI space.
 * - For 32 MB RVIPs we'll have 28 MB DRAM and 4 MB of PCI space.
 *
 * Revision 3.12  1996/03/04  07:14:23  kao
 * CSCdi49674:  While VIP is cofigured with DFS, OIR event will crash the
 * VIP;
 *
 * Revision 3.11  1996/02/23  20:44:40  jkuzma
 * CSCdi49730:  case label incorrect in error_interrupt_handler
 *
 * Revision 3.10  1996/02/21  02:49:28  kao
 * CSCdi45136:  EOIR Problems with VIP card; don't need to include cpu_4k.h
 *
 * Revision 3.9  1996/02/18  19:42:29  kao
 * CSCdi45136:  EOIR Problems with VIP card
 *
 * Revision 3.8  1996/02/13  20:39:42  hampton
 * Move buffer initialization to the platform_xxx.c files to eliminate
 * platform dependencies from common code.  [CSCdi48844]
 *
 * Revision 3.7  1996/01/12  21:53:05  jkuzma
 * CSCdi46358:  VIP1 PA mngmt and DMA interrupt stacks swapped
 *              General debug of PA management interrupts
 *
 * Revision 3.6  1995/12/19  07:50:00  hampton
 * CSCdi45824:  RVIP platform check is wrong
 *
 * Revision 3.5  1995/12/10  04:01:36  smackie
 * Add support for public particle pools. Make particle chunk growth less
 * aggressive for dynamic pools. (CSCdi45336)
 *
 * Revision 3.4  1995/11/28  03:28:15  getchell
 * Increase ring sizes to avoid holdq enqueing
 * CSCdi43906:  performance drops by 1/2 when traffic passes zero drops
 *
 * Revision 3.3  1995/11/17  18:50:23  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:33:37  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 * Revision 1.1.2.6  1995/11/16  18:03:30  getchell
 * Increase number of buffers so that particle_reparent is less likely to
 * fail.
 * CSCdi43906:  performance drops by 1/2 when traffic passes zero drops
 * Branch: VIP_branch
 *
 * Revision 1.1.2.5  1995/10/18  04:11:20  mbeesley
 * CSCdi42317:  Large Tx packets get corrupted.
 * Use multiple DMA requests for large Tx packets. Also grow some
 * larger buffers at init time to minimize buffer misses.
 * Branch: VIP_branch
 *
 * Revision 3.1  1995/11/09  13:29:08  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:29:05  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/06/07  22:58:59  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Include files
 */
#include "master.h"
#include "../../boot/cpu.h"
#include "interface_private.h"
#include "packet.h"
#include "logger.h"
#include "ttysrv.h"
#include "exception.h"
#include "stacks.h"
#include "sys_registry.h"
#include "subsys.h"

#include "../os/gdb.h"
#include "../os/free.h"
#include "../os/region.h"
#include "../os/hostname.h"
#include "../os/pool.h"
#include "../os/particle.h"
#include "../os/buffers.h"
#include "../os/buffers_inline.h"
#include "../if/network.h"

#include "../src-4k/util_4k.h"
#include "../dev/monitor1.h"
#include "../hes/dgbus.h"

#include "../pas/if_pas.h"
#include "../pas/msg_pas.c"		/* Not a typo, see logger.h */
#include "../src-vip/vip_dbus.h"
#include "../src-vip/vip_memd.h"
#include "rvip_dma.h"

/*
 * These variables are held in the vip-version.c file which
 * gets dynamically generated and compiled by the VIP makefile. They
 * hold the VIP VPLD version number, the VIP software version number, 
 * and the VIP rom monitor version number.
 */
extern ushort vip_sw_vint, vip_vpld_vint, vip_rom_vint;

extern union instr_r4k * check_load_inst(r4k_context_t *sc);
extern void r4k_set_cpu_reg(r4k_context_t *sc, int index, int value, boolean ext);
extern int branch_emulate(r4k_context_t *sc);


/*
 * Memory pools
 */
static mempool pmem_mempool;
static mempool sram_mempool;
static mempool pcimem_mempool;

/*
 * Memory regions
 */
static regiontype main_region;
static regiontype text_region;
static regiontype data_region;
static regiontype bss_region;
static regiontype pmem_region;
static regiontype sram_region;
static regiontype pcimem_region;

static regiontype k0_main_region;
static regiontype k1_main_region;

/*
 * Buffer pools
 */
static pooltype *particle_pool;
static pooltype *header_pool;

/*
 * Local strings, pointers
 */
static char proc_revision[80];
static char vip_ios_name[20];
static char hw_revision[20];
static char hw_serial[20];
static char *vip_ios_name_ptr = NULL;

/*
 * Freelists
 */
static const ulong freelist_pcimem_block_sizes[] = {
    FREESIZE(0), FREESIZE(1), FREESIZE(2), FREESIZE(3), FREESIZE(4),
    FREESIZE(5), FREESIZE(6), FREESIZE(7), 65536
};

static const ulong nfreelist_pcimem_block_sizes =
    sizeof(nfreelist_pcimem_block_sizes)/sizeof(ulong);

/*
 * Defines
 */
#define SRAM_MALLOC_START (uchar *)(ADRSPC_SRAM_HEAP)
#define SRAM_MALLOC_SIZE           (ADRSPC_SRAM_END - ADRSPC_SRAM_HEAP)

/*
 * platform_main :
 * Called from main() to allow platforms to initialize basic functions
 * such as LEDs.
 */
void platform_main (void)
{
    ushort hw_version;

    /*
     * Build the platform strings
     */
    sprintf(hw_serial, "%08x", 0);
    hw_version = mon_get_rev();
    sprintf(hw_revision, "0x%02x", hw_version);
    r4k_get_revision_string(proc_revision);
}

static boolean is_memd_read_addr (int addr, r4k_context_t *sc)
{
    return(is_memd_addr(addr) &&
	sc->epc >= (int)start_memd_read_handlers &&
	sc->epc <= (int)end_memd_read_handlers);
}

/*
 * if the bus error was caused by accessing MEMD, it could be
 * due to an on-going bus stall. If so, don't break the VIP
 */
static void read_bus_error_handler (int signal_no, int code, r4k_context_t *sc)
{
    union instr_r4k *op;
 
    op = check_load_inst(sc);
    if (op) {
	int addr;
	 
	addr = r4k_get_cpu_reg(sc, op->i_t.rs);
	addr += op->i_t.s_imd;
	if (is_memd_read_addr(addr, sc)) {
	    /*
	     * Decide the read bus error is caused by a temporary bus stall
	     * condition, set both destination register and regular return
	     * register v0 to zero. Then delay 1 ms to slow down a burst of
	     * such bus error (potentially) before return back to the 
	     * instruction after the failure one
	     */
	    vsys_vec.bus_stall_read_err++;
	    branch_emulate(sc);
	    r4k_set_cpu_reg(sc, R4K_V0_REG, 0, FALSE);
	    r4k_set_cpu_reg(sc, op->i_t.rt, 0, FALSE);
	    DELAY(1);
	    return;
	}
    }

    handle_exception(signal_no, code);
}

/*
 * error_interrupt_handler:
 * Handle all flavors of checkers hardware errors.
 */
static void error_interrupt_handler (r4k_context_t *ptr)
{
    uchar vector, pmac_error;
    volatile ulong err_addr;
    pmac_intrpt_status_t pmac_status;
    
    /*
     * Get some info from the hardware
     */
    vector = IO_ASIC->int_vect;
    pmac_status = PCI_ASIC->pmac_intrpt_status_reg;
    pmac_error = pmac_status.errors;


    /*
     * Decode the error in detail
     */
    switch (vector) {

    case NEV_DRAM_ERR_VEC :
	/*
	 * Print a generic message
	 */
	ttyprintf(CONTTY, "\nVIP Error Interrupt: Vector = 0x%x\n", vector);
	ttyprintf(CONTTY, "PMAC status register = 0x%08x\n\n", pmac_status);

	/*
	 * DRAM bus error. Clear the interrupt and read the address reg.
         */
	IO_ASIC->int_eoi = NEV_DRAM_ERR_INT;
	ttyprintf(CONTTY, "%%ERR-1-FATAL: DRAM write bus error interrupt\n");
	err_addr = *(volatile ulong *)(ADRSPC_DRAM_WBERR);
	ttyprintf(CONTTY, " Error address = 0x%08x, %s", err_addr,
		  (err_addr & 0x01) ? "(valid bit set)\n" : 
				    "(valid bit NOT set)\n");
	IO_ASIC->csb &= ~CSB_CLR_DRAM_WR_BERR;
	flush_io_wbuffer();    
	IO_ASIC->csb |= CSB_CLR_DRAM_WR_BERR;
	break;

    case NEV_IO_ERR_VEC:
	/*
	 * I/O bus error. Clear the interrupt and read the address reg.
	 * Prepare
         */
	IO_ASIC->int_eoi = NEV_IO_ERR_INT;
	IO_ASIC->csb = 0;
	IO_ASIC->csb = CSB_CLR_IO_WR_BERR;
	/*
	 * err_addr doesn't provide real MEMD address in write bus error
	 */
	err_addr = *(volatile ulong *)(ADRSPC_IO_WBERR);
	/*
	 * If Cbus stall, don't crash RVIP
	 */
	if (vip_probe_memd() == FALSE) {
	    vsys_vec.bus_stall_write_err++;
	    return;
	}
	/*
	 * Print a generic message
	 */
	ttyprintf(CONTTY, "\nVIP Error Interrupt: Vector = 0x%x\n", vector);
	ttyprintf(CONTTY, "PMAC status register = 0x%08x\n\n", pmac_status);
	ttyprintf(CONTTY, " Error address = %08x, %s", err_addr,
		(err_addr & 0x01) ? "(valid bit set)\n" : 
				    "(valid bit NOT set)\n");
	break;

    case NEV_PMAC_ERR_VEC:
	/*
	 * PMAC Error Interrupt. Clear the interrupt and decode error
	 */
	IO_ASIC->int_eoi = NEV_PMAC_ERR_INT;
	if (pmac_error & PMAC_IO_PARITY_ERROR)
	    ttyprintf(CONTTY, "%%PAR-1-FATAL: PMAC I/O parity error\n");
	if (pmac_error & PMAC_PM_FIFO_ERROR) 
	    ttyprintf(CONTTY,
		      "%%FIFO-1-FATAL: PMAC packet memory fifo error\n");
	if (pmac_error & PMAC_PM_CTRLR_ERROR) 
	    ttyprintf(CONTTY,
		      "%%CNTL-1-FATAL: PMAC pkt mem controller error\n");
	if (pmac_error & PMAC_PCI_PARITY_ERROR) 
	    ttyprintf(CONTTY, "%%PAR-1-FATAL: PMAC PCI parity error\n");
	if (pmac_error & PMAC_PCI_SYSTEM_ERROR) 
	    ttyprintf(CONTTY, "%%PCI-1-FATAL: PMAC PCI system error\n");
	break;

    default:
	ttyprintf(CONTTY, "%%ERR-1-UNKNOWN: Unknown error vector, %#x\n",
		  vector);
	break;
    } 

    /*
     * Flush the write buffers and reload
     */
    flush_io_wbuffer();
    rvip_reload(SIGRELOAD, 0, ptr);
}

/*
 * install_error_handler :
 * Install the Level 4 error handler. Note, interrupts are locked out
 */
static void install_error_handler (void)
{
    /* 
     * Create level 4 interrupt stack - Error handler
     */
    createlevel(LEVEL_ERROR, (void (*)(void))error_interrupt_handler, 
		"Error Interrupt");

    /* Set up the nevada valid interrupt mask register */
    IO_ASIC->int_mask = NEV_INT_MASK;

    /* Clear all Nevada interrupts */  
    IO_ASIC->int_eoi = NEV_INT_MASK;
}

/*
 * Setup special signal handler
 */
static void install_sig_handler(void)
{
    SIG_PF sigbus_save;

    sigbus_save = signal(SIGBUS, (SIG_PF)read_bus_error_handler);
}

/*
 * platform_exception_init :
 * Initialize basic interrupt handling for the platform
 */
void platform_exception_init (void)
{
    /*
     * Initialize the NMI handler
     */
    init_clocktick_handler();

    /*
     * Setup DBUS interrupt handler
     */
    install_rvip_dbus_handler();

    /*
     * Setup MALU Attention handler
     */
    install_cbus_handler();

    /*
     * Setup special signal handler
     */
    install_sig_handler();
    /*
     * Setup Error handler
     */
    install_error_handler();

    /*
     * Place holder for dynamic particle sizing, when implemented.
     * Currently used by vip_show_memd.
     */
    vsys_vec.vip_particle_size = VIP_PARTICLE_BYTES;

    /*
     * Setup DMA handler
     */
    install_dma_handler();

    /*
     * Restart count/compare mechanism due to the possibly 
     * really long time we have spent in here doing initialization
     * with the timer interrupt off.
     */
    restart_count_compare(0);
}

/*
 * platform_memory_init :
 * Define and declare all the memory regions and mempools for this platform
 */
void platform_memory_init (void)
{
    ulong memsize, iomemsize;

    /*
     * Find out how much main dram the Rom Monitor thinks we've got
     */
    memsize = mon_getmemsize();

    /*
     * Most of this main DRAM will be dedicated to code, data
     * and the global heap. But part of it will be used for IO memory.
     * It has already been mapped once into the virtual address
     * space. We need to map the IO memory part once more to get the
     * IO memory view.
     */
    iomemsize = mon_get_iosize();
    memsize -= iomemsize;

    /*
     * Map the IO DRAM memory into the address space.
     */
    if (!rvip_map_iomemory(memsize, iomemsize)) {
        crashdump(0);
    }
     
    /*
     * Add a region to describe all of main DRAM minus IO memory
     */
    region_create(&main_region, "main",
		  (void *)RAMBASE, memsize,
		  REGION_CLASS_LOCAL, REGION_FLAGS_DEFAULT);

    /*
     * Add regions to describe the loaded image
     */
    region_create(&text_region, "text",
		  TEXT_START, ((uint)TEXT_END - (uint)TEXT_START),
		  REGION_CLASS_IMAGETEXT, REGION_FLAGS_DEFAULT);
    region_create(&data_region, "data",
		  DATA_START, ((uint)DATA_END - (uint)DATA_START),
		  REGION_CLASS_IMAGEDATA, REGION_FLAGS_DEFAULT);
    region_create(&bss_region, "bss",
		  DATA_END, ((uint)_END - (uint)DATA_END),
		  REGION_CLASS_IMAGEBSS, REGION_FLAGS_DEFAULT);

    /*
     * Mark the text segment as read only as the R4600 TLB is set up
     * to protect this area
     */
    region_set_media(&text_region, REGION_MEDIA_READONLY);

    /*
     * Declare a region from the end of BSS to the end of the processor
     * portion of DRAM (IE not including the PCI memory) and create a 
     * "local" mempool based on it.
     */
    region_create(&pmem_region, "mainheap",
		  _END, (RAMBASE + memsize - (ulong)_END), 
		  REGION_CLASS_LOCAL, REGION_FLAGS_DEFAULT);

    mempool_create(&pmem_mempool, "Processor", &pmem_region, 0,
		   NULL, 0, MEMPOOL_CLASS_LOCAL);


    /*
     * Declare a region and mempool for PCI shared memory
     */
    region_create(&pcimem_region, "pcimem",
		  (void *)(PHY_RAMBASE + memsize), iomemsize,
		  REGION_CLASS_PCIMEM, REGION_FLAGS_DEFAULT);
    mempool_create(&pcimem_mempool, "PCI", &pcimem_region, 32,
		   freelist_pcimem_block_sizes, nfreelist_pcimem_block_sizes,
		   MEMPOOL_CLASS_PCIMEM);

    /*
     * Create the regions for K0 and K1 segments
     */
    region_create(&k0_main_region, "main_k0",
		  (void *)R4K_K0BASE, memsize,
		  REGION_CLASS_LOCAL, REGION_FLAGS_DEFAULT);
    region_create(&k1_main_region, "main_k1",
		  (void *)R4K_K1BASE, memsize,
		  REGION_CLASS_LOCAL, REGION_FLAGS_DEFAULT);

    region_add_alias(&main_region,  &k0_main_region);
    region_add_alias(&main_region,  &k1_main_region);

    /*
     * Declare a region and mempool for fast memory (which is really
     * a block of unused DRAM)
     */
    region_create(&sram_region, "sram",
		  SRAM_MALLOC_START, SRAM_MALLOC_SIZE,
		  REGION_CLASS_FAST, REGION_FLAGS_DEFAULT);
    mempool_create(&sram_mempool, "SRAM", &sram_region, 0,
		   NULL, 0, MEMPOOL_CLASS_FAST);

    /*
     * If SRAM runs out, give it someplace to go.
     */
    mempool_add_alternate_pool(&sram_mempool, &pmem_mempool);

    /*
     * Add aliases for mandatory mempools
     */
    mempool_add_alias_pool(MEMPOOL_CLASS_IOMEM, &pcimem_mempool);
    mempool_add_alias_pool(MEMPOOL_CLASS_ISTACK, &sram_mempool);
    mempool_add_alias_pool(MEMPOOL_CLASS_PSTACK, &sram_mempool);
}

/*
 * platform_buffer_init
 *
 * Define and declare all the buffer sizes for this platform
 * VIP is only called with PLATFORM_BUFFER_NORMAL
 */

void platform_buffer_init (platform_buffer_init_type type)
{
	pool_adjust(small, SMALL_MIN, SMALL_MAX1, SMALL_INIT1, TRUE);
	pool_adjust(middle, MID_MIN, MID_MAX1, MID_INIT1, TRUE);
	pool_adjust(big, BIG_MIN, BIG_MAX1, BIG_INIT1, TRUE);

    /*
     * Adjust the bigger buffer pools so that we always have
     * at least some buffers
     */
    pool_adjust(verybig, RVIP_VERYBIG_MIN, RVIP_VERYBIG_MAX, 
		RVIP_VERYBIG_INIT, TRUE);
    pool_adjust(large, RVIP_LARGE_MIN, RVIP_LARGE_MAX, RVIP_LARGE_INIT, TRUE);
    pool_adjust(huge, RVIP_HUGE_MIN, RVIP_HUGE_MAX, RVIP_HUGE_INIT, TRUE);
}

/*
 * platform_get_string :
 * Returns a variety of strings unique to the RVIP
 */
char *platform_get_string (platform_string_type type)
{
    char *value;

    switch (type) {

    case PLATFORM_STRING_NOM_DU_JOUR:
	value = "VIP";
	break;

    case PLATFORM_STRING_DEFAULT_HOSTNAME:
	if (!vip_ios_name_ptr) {
	    sprintf(vip_ios_name, "VIP-Slot%d", vip_logical_slot());
	    vip_ios_name_ptr = vip_ios_name;
	}
	value = vip_ios_name_ptr;
	break;

    case PLATFORM_STRING_PROCESSOR:
	value = "R4K";
	break;

    case PLATFORM_STRING_PROCESSOR_REVISION:
	value = proc_revision;
	break;

    case PLATFORM_STRING_VENDOR:
	value = "cisco";
	break;

    case PLATFORM_STRING_HARDWARE_REVISION:
	value = hw_revision;
	break;

    case PLATFORM_STRING_HARDWARE_SERIAL:
	value = hw_serial;
	break;

    default:
	value = NULL;
	break;
    }

    return(value);
}

/*
 * platform_get_value :
 * Returns a variety of integer values unique to the RVIP
 */
uint platform_get_value (platform_value_type type)
{
    uint value;

    switch (type) {
      
    case PLATFORM_VALUE_SERVICE_CONFIG:
	value = FALSE;
	break;

    case PLATFORM_VALUE_FEATURE_SET:
	value = 0x0000;
	break;

    case PLATFORM_VALUE_HARDWARE_REVISION:
	value = mon_get_rev();
	break;

    case PLATFORM_VALUE_HARDWARE_SERIAL:
	value = 0;
	break;

    case PLATFORM_VALUE_VENDOR:
	value = VENDOR_CISCO;
	break;

    case PLATFORM_VALUE_CPU_TYPE:
	value = CPU_VIP;
	break;

    case PLATFORM_VALUE_FAMILY_TYPE:
	value = (cpu_type == CPU_VIP) ? FAMILY_VIP : FAMILY_UNKNOWN;
	break;

    case PLATFORM_VALUE_REFRESH_TIME:
	value = REFRESHTIME;
	break;

    case PLATFORM_VALUE_LOG_BUFFER_SIZE:
    default:
	value = 0;
	break;
    }

    return(value);
}

/*
 * vip_system_configured:
 * VIP post-configuration callback
 */
static void vip_system_configured (void)
{
    /*
     * Disable netconfig_enable to stop spawning off config
     * probe processes. Also call whoami() to setup hostname.
     */
    netconfig_enable = FALSE;
    whoami();
}

/*
 * rvip_setup_hardware_registers :
 * Set Nevada Registers A, B and C as follows:
 *      A status
 *      B control
 *      C control
 */
void rvip_setup_hardware_registers (void)
{
    /*
     * 0 means status
     * 1 means control
     * Bit 0 - Reg A. Configured as status register
     * Bit 1 - Reg B. Configured as control register
     * Bit 2 - Reg C. Configured as control register
     *
     * Note: We initialize the control register(s) to their passive
     * state before configuring Nevada
     */
    IO_ASIC->csb = 0xff;
    IO_ASIC->csc = 0xff;
    IO_ASIC->asic_reg = ASIC_CONT_B|ASIC_CONT_C;
    flush_io_wbuffer();
}

/*
 * rvip_subsys_init_class:
 * Invoke pas_subsys_init_class and then tell RP/RSP we are ready
 */
static void rvip_subsys_init_class (ushort class)
{
    if (class == SUBSYS_CLASS_DRIVER) {
	pas_subsys_init_class(class);
	
	/*
	 * Let the RP/RSP know we are ready
	 */
	vip_kernel_ready();
    }
}

/*
 * vip_registry_init :
 * Register some VIP specific functions
 */
static void vip_registry_init (void)
{
    /*
     * Setup hardware registers
     */
    rvip_setup_hardware_registers();

    reg_add_subsys_init_class(rvip_subsys_init_class,
			      "rvip_subsys_init_class");
    /*
     * Setup a phony handler for autoaddr_okay() to stop config probes
     */
    reg_add_system_configured(vip_system_configured, "vip_system_configured");

    /*
     * Register the buffer returning
     */
    reg_add_onesec(vip_memd_return_periodic, "vip_memd_return_periodic");
}

/* 
 * platform_interface_init :
 * Initialize platform interface specific features
 */
void platform_interface_init (void)
{
    /*
     * Allocate a global pool of particles
     */
    particle_pool = particle_pool_create("Fast", 
					 POOL_GROUP_PUBLIC,
					 VIP_PARTICLE_BYTES, 
					 POOL_DEFAULT_FLAGS,
					 32,
					 &pcimem_mempool);
    if (particle_pool) {
	pool_adjust(particle_pool, 128, 1024, 512, TRUE);
	particle_pool_create_cache(particle_pool, 128);
	pool_adjust_cache(particle_pool, 128);
    }

    /*
     * Init the global header pool
     */
    header_pool = pak_pool_create("Header", POOL_GROUP_PUBLIC, 0,
				  POOL_DYNAMIC | POOL_PUBLIC_DUPLICATE,
				  NULL);
    if (header_pool) {
	pool_adjust(header_pool, HEADER_MIN1, RVIP_DMA_BUFFERS, 
		    RVIP_DMA_BUFFERS, TRUE);
    }

    /*
     * Init the pool of static paktypes for fastswitching
     */
    pas_init_fspak_pool();

    /*
     * Initialize the DMA particle pools
     */
    rvip_init_dma_pools();

    vip_registry_init();		/* setup VIP specific registry 	*/
    pas_init_jumptable();		/* initialize Net IO jumptable 	*/
    pas_init_network_level();		/* initialize network intr level*/
    pas_init_pa_nmi_handler();		/* initialize PA Hi IRQ stack */

    /*
     * Add a callback for interface final initialization
     */
    reg_add_if_final_init(pas_if_final_init, "pas_if_final_init");
}

boolean (*platform_interface_ordering) (hwidbtype *new_idb, hwidbtype *old_idb)

     = idb_slotted_ordering;

/*
 * platform_verify_config
 *
 * Validate hardware/software compatability.
 */

void platform_verify_config(void)
{
}

/*
 * platform_r4k_set_clock_params:
 * Analyze the config register and setup r4k_1ms_pipe_ticks
 * and r4k_4ms_pipe_ticks. Support divide by 2 (100Mhz) only for rvip.
 * In case, different CPU speed has to be supported in the future,
 * analyze the EC bits of the config reg.
 */
void platform_r4k_set_clock_params (ulong config_reg)
{
    /*
     * It is a 100mhz mipe (divide by 2), load the
     * appropriate values for pipe_speed and period in nanosecs.
     */
    r4k_1ms_pipe_ticks = R4K_1MS_AT_100MHZ;
    r4k_4ms_pipe_ticks = R4K_4MS_AT_100MHZ;
    r4k_pipe_speed = 100;
    r4k_count_period_ns = R4K_NS_PERIOD_AT_100MHZ;
}

/*
 * pci_memaddr_convert:
 * Convert a memory address to the correct cached view
 */
void *pci_memaddr_convert (void *addr, int attr)
{
    ulong taddr;
 
    taddr = (ulong) addr;
 
    /* force address to uncached address */
    taddr &= ~PCIMEM_CWB_OFS;
 
    switch (attr) {
      case TLB_CACHE_WT:
        taddr |= PCIMEM_CWT_OFS;
        break;
 
      case TLB_CACHE_NONCOHERENT:
        taddr |= PCIMEM_CWB_OFS;
        break;
 
      case TLB_UNCACHED:
        break;
 
      default:
        /* No other modes supported */
        break;
    }
 
    return((void *)taddr);
}

/*
 * pas_interface_fallback_pool:
 * Return a pointer to an interface's fallback pool
 */
pooltype *pas_interface_fallback_pool (hwidbtype *idb)
{
    return(particle_pool);
}

/*
 * pas_interface_header_pool:
 * Return a pointer to an interface's header pool
 */
pooltype *pas_interface_header_pool (hwidbtype *idb)
{
    return(header_pool);
}

/*
 * vip_dma_buffer_mempool:
 * Return a pointer to the pool to be used for a particle cache
 * by the DMA engine sucking Tx buffers from memd
 */
mempool *vip_dma_buffer_mempool (void)
{
    return(&pcimem_mempool);
}

/*
 * pas_buffer_mempool:
 * Return a pointer to the memory pool that an interface should use
 * for its buffer pool
 */
mempool *pas_buffer_mempool (hwidbtype *idb)
{
    return(&pcimem_mempool);
}

/*
 * pas_netio_interrupts:
 * Control the network IO interrupt enables for a pa bay. Must be 
 * called with interrupts masked.
 */
void pas_netio_interrupts (uint pa_bay, boolean onoff)
{
    volatile ulong *reg;

    reg = (volatile ulong *)&(PCI_ASIC->pmac_intrpt_mask_reg);
    if (pa_bay == PA0) {
	if (onoff) {
	    *reg |= PMAC_PA0_PKT_MASK;
	} else {
	    *reg &= ~PMAC_PA0_PKT_MASK;
	}
    } else if (pa_bay == PA1) {
	if (onoff) {
	    *reg |= PMAC_PA1_PKT_MASK;
	} else {
	    *reg &= ~PMAC_PA1_PKT_MASK;
	}
    } else {
	errmsg(&msgsym(BADPABAY, PA), pa_bay);
    }
}

/*
 * pas_mngmt_interrupts:
 * Control the PA management interrupt enables for a bay. Must be called with
 * interrupts masked.
 */
void pas_mngmt_interrupts (uint pa_bay, boolean onoff)
{
    volatile ulong *reg;

    reg = (volatile ulong *)&(PCI_ASIC->pmac_intrpt_mask_reg);
    if (pa_bay == PA0) {
        if (onoff) {
            *reg |= PMAC_PA0_NMI_MASK;
        } else {
            *reg &= ~PMAC_PA0_NMI_MASK;
        }
    } else if (pa_bay == PA1) {
        if (onoff) {
            *reg |= PMAC_PA1_NMI_MASK;
        } else {
            *reg &= ~PMAC_PA1_NMI_MASK;
        }
    } else {
        errmsg(&msgsym(BADPABAY, PA), pa_bay);
    }
}

/*
 * pas_platform_if_final_init: 
 * Gets called off of the if_final_init registry. We loop
 * thru all the interfaces, putting them into the admin down state.
 */
void pas_platform_if_final_init (void)
{
    hwidbtype *idb;

    FOR_ALL_HWIDBS(idb) {
	idb->oir_flags &= ~IDB_FINAL_INIT;
	net_cstate(idb, IDBS_ADMINDOWN);
	if (idb->shutdown && !(idb->status & IDB_NOHARDWARE))
	    (*idb->shutdown)(idb, IDBS_ADMINDOWN);
    }

    /*
     * Init the VIP ipc
     */
    vip_init_ipc();
}

/*
 * vip_sw_version:
 * Return the VIP IOS software version
 */
ushort vip_sw_version (void)
{
    return(vip_sw_vint);
}

/*
 * vip_vpld_version:
 * Return the VIP VPLD version. Note, should be a Rom Monitor EMT call
 * when the monitor supports such stuff
 */
ushort vip_vpld_version (void)
{
    return(vip_vpld_vint);
}

/*
 * vip_rom_version:
 * Return the VIP Rom version. Note, should be a Rom Monitor EMT call
 * when the monitor supports such stuff
 */
ushort vip_rom_version (void)
{
    return(vip_rom_vint);
}

/*
 * pas_activate_bay :
 * Update the slots structure. Will be moved to pas_controller.c
 */
void pas_activate_bay (uint pa_bay)
{
}

/*
 * pas_slots_init :
 * Reset slots flags. Will be moved to pas_controller.c
 */
void pas_slots_init (void)
{
}

/*
 * pas_get_pa_type :
 * Return pa controller type.
 */
ushort pas_get_pa_type (uint pa_bay)
{
    return(get_pa_type(pa_bay));
}

/*
 * pas_pa_exists :
 * Update slots that pa exists. Will be moved to pas_controller.c.
 */
void pas_pa_exists (uint pa_bay)
{
}

/*
 * pas_pa_analyzed :
 * Determine if this PA is already analyzed. Will be moved to pas_controller.c
 */
boolean pas_pa_analyzed (uint pa_bay)
{
    return(FALSE);
}

/*
 * pas_add_card_entry :
 * Add a Port Adapter to the chassis MIB. Will be moved to pas_controller.c.
 */
void pas_add_card_entry (uint pa_bay)
{
}

/*
 * pas_set_fci_type :
 * Set the idb fci type. Will be moved to pas_controller.c.
 */
void pas_set_fci_type (hwidbtype *idb, uint slot)
{
}

/*
 * pas_install_interrupt_status_reg:
 * Stub routine for VIP1.
 */
void pas_install_status_reg (hwidbtype *idb, void *addr,
			     boolean memory_mapped_io)
{
}

/*
 * pas_delay:
 * platform dependant delay routine.
 */
void pas_delay(uint time)
{
    DELAY(time);
}

/*
 * oir_active:
 * Test if OIR is active
 */ 
boolean pas_oir_active (void) {
    return (FALSE);
}
/* end of file */



