/* $Id: pri_cache.c,v 3.1.60.1 1996/03/21 23:30:57 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/rommon/src-4k-c4000/pri_cache.c,v $
 *------------------------------------------------------------------
 * pri_cache.c
 *
 * June 1995, Anh Dang
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Primary cache test routines for R4000/Orion CPU.
 *------------------------------------------------------------------
 * $Log: pri_cache.c,v $
 * Revision 3.1.60.1  1996/03/21  23:30:57  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.1.4.1  1995/12/01  04:23:25  mbeesley
 * Branch: ELC_branch
 * Reorganize Rom monitor header files to make it easier to port
 * to new platforms.
 *
 * Revision 3.1  1995/11/09  13:08:22  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/17  01:01:16  clev
 * CSCdi35421:  Add cache tests to ROM monitor core board diagnostics
 *
 *
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "monitor.h"
#include "signal_4k.h"
#include "cpu_mips.h"
#include "mon_defs.h"
#include "queryflags.h"
#include "error.h"
#include "extern.h"
#include "proto.h"

SIG_PF sigsav;
ulong sreg_sav;
ulong sys_clk_ratio;
int  level2_cache_present;
  
#define CACHE_SIZE     0x4000         /* 16KB cache */

/*
 * dcache_flush : Flush the Data Cache
 */
void
dcache_flush (void)
{
    unsigned int index, lines;

    lines = 1 << (CSIZE_SHIFT - CLINE_SHIFT); /* lines = 512, CLINE_SHIFT = 5 */

    for (index = 0; index < lines; index++)
	dcache_wback( PHY_TO_KSEG0((index << CLINE_SHIFT)) );
}
  
/*
 * when the test start, the stack has changed from KSEG0 to KSEG1 so
 * before exit the test, need to change it back from KSEG1 to KSEG0
 */
   
static 
SIG_PF cleanup_cachetest ()
{
    long old_sp, new_sp;

    setcp0_sreg(sreg_sav);
    old_sp = getcpu_sp();            /* get the stack pointer */
    new_sp = KSEG1_TO_KSEG0(old_sp); /* change the stack ptr back to cache */
    setcpu_sp(new_sp);               /* store the stack ptr back */
    signal(SIGINT, (SIG_PF)sigsav);
    return((SIG_PF)SIG_DFL);
}

/*
 *  Function: pretest_orioncache 
 *   Called from the main menu, calls test_orioncache after supplying a default
 *   of DRAM base and length of memsize.  If that test succeeds, it proceeds to
 *   call test_orionparity.  The sequence thus tests the data cache first for
 *   correct operation, and then tests whether induced parity errors result in 
 *   the correct cache error exceptions.
 */


int pretest_orioncache (int one)
{
    struct nvram *nvptr = NVRAM;
    int retval;
    long memlen; 

    if ((memlen = sizemainmem()) == 0) {
	cterr('f',0,"problem sizing main memory");
	return (1);
    }

    testname ("cache");
    nvptr->start = memlen - 16*CACHE_SIZE;
    nvptr->length = 4*CACHE_SIZE;

    sreg_sav = getcp0_sreg();
    sigsav = signal(SIGINT, (SIG_PF)cleanup_cachetest);

    cache_info(1);  /* get cache information */

    if (!sys_clk_ratio && level2_cache_present)
	cterr ('w',0,"!!! Secondary Cache is detected\n"
	       "and the processor CLK frequency is only 100MHz !!!");
    if (sys_clk_ratio && !level2_cache_present)
	cterr ('w',0,"!!! The processor CLK frequency is 150MHz\n"
	       "and Secondary Cache is not detected !!!");

    cache_test();

    cleanup_cachetest();    
    prcomplete(testpass, errcount, 0);

} /* END pretest_orioncache */
/* 
 * Function: cache_test
 * - test the primary cache
 * - test the second level cache if present
 *
 */

int
cache_test(void)
{

    if (test_orioncache())        /* test Primary cache */
	return(1);

    if (level2_cache_present)
	if (test_level2_cache())  /* test Secondary cache */
	    return(1);

    return(0);
    
} /* END cache_test */

/*
 *  Function: test_orioncache 
 *   Tests the cache for the range of physical addresses given in NVRAM.  Makes 
 *   accesses alternately uncached and cached with stores and loads to test the
 *   integrity of both memory and the data cache.  Test for the extent of the
 *   range by subdividing it into CACHE_SIZE chunks.
 */
int test_orioncache (void)
{

#define   PATTERN_B   (ulong) 0xAAAAAAAA
#define   PATTERN_C   (ulong) 0x55555555
#define   PATTERN_A5  (ulong) 0xA5A5A5A5
#define   PATTERN_5A  (ulong) 0x5A5A5A5A
#define   CACHE_READBACK_DELAY  4000  /* 4s call to sleep */
    char cache_msg[80];
    int test_cacherefill (long **addrptr_ctrl); /* Tests refill alternation betw sets */
    long *addrptr, dataval;
    ulong c_startaddr, u_startaddr, c_endaddr, u_endaddr;
    ulong phy_chunk_begin, phy_chunk_len, phy_addr_end;
    struct nvram *nvptr = NVRAM;
    int phase, retval;
    uchar *byteptr;
    int i, j, pattern;
    long old_sp, new_sp;

    /*
     * This is rom monitor environment, the stack is set in cache KSEG0.
     * This will interfere with the test since this test the cache, so 
     * need to move the stack from KSEG0 to KSEG1. First of need to flush
     * the cache so that all stack info get updated in memory then switch
     * the KSEG. Since we change the KSEG here, in the cleanup() need to
     * switch it back.
     */ 

    dcache_flush();                  /* all stack info get transfer to mem */
    old_sp = getcpu_sp();            /* get the stack pointer */
    new_sp = KSEG0_TO_KSEG1(old_sp); /* change the stack ptr to non cache */
    setcpu_sp(new_sp);
    
    /* Initialize to test in CACHE_SIZE chunks. */
    phy_chunk_begin = nvptr->start;
    phy_chunk_begin += CACHE_SIZE;
    phy_addr_end = phy_chunk_begin + nvptr->length;

    testname("data cache");  /* print initial message */    
    sprintf(cache_msg,"Primary");
    phase = 1;               /* Each phase corresponds to chunk */

    while (phy_chunk_begin < phy_addr_end) {
	phy_chunk_len = ((phy_chunk_begin + CACHE_SIZE) > phy_addr_end) ?
	    phy_addr_end - phy_chunk_begin : CACHE_SIZE;

	prpass(testpass, "phase %d, %s", phase++, cache_msg);

#ifdef DEBUG
	printf("\nTest cache of addr: 0x%.8x len: 0x%.8x",
	       phy_chunk_begin, phy_chunk_len);
#endif /* DEBUG */

	/*
	 * First, flush the cache, and via uncached segment, for the given range write
	 * to each cell its address.  Read it back uncached again to check that the
	 * the block is OK in memory.
	 */
       
	dcache_flush();
	level2_cache_flush();

	u_startaddr = PHY_TO_KSEG1(phy_chunk_begin);  /* Uncached virtual address */
	u_endaddr   = u_startaddr + phy_chunk_len;

	for (addrptr=(long *)u_startaddr; addrptr < (long *)u_endaddr ;) {
	    *addrptr++ = (long)addrptr;   /* write : data = address */
	}
	for (addrptr=(long *)u_startaddr ; addrptr < (long *)u_endaddr ; addrptr++) {
	    dataval = *addrptr;
	    if (dataval != (long)addrptr) {  /* read : data = address */
		cterr('f',0,"uncached readback fails at location: 0x%x data: 0x%x",
		      addrptr, dataval);
		return(1);
	    }
	}

	/*
	 * Now write a different pattern to same cells via cached segment.  Then read
	 * back uncached again as well as cached to check that this pattern is only in 
	 * the cache, and not yet in memory.
	 */
	c_startaddr = PHY_TO_KSEG0(phy_chunk_begin);  /* Cached virtual address */
	c_endaddr   = c_startaddr + phy_chunk_len;

	for (addrptr=(long *)c_startaddr; addrptr < (long *)c_endaddr ;)
	    *addrptr++ = PATTERN_B;

	/* Delay before readback to test for cell loss in cache */
	sleep(CACHE_READBACK_DELAY);

	for (addrptr=(long *)u_startaddr ; addrptr < (long *)u_endaddr ; addrptr++) {
	    dataval = *addrptr;
	    if (dataval != (long)addrptr) {  /* read : data = address */
		cterr('f',0,"cached write disturbed memory at location: 0x%x data: 0x%x",
			  addrptr, dataval);
		return(1);
	    }
	}

	for (addrptr=(long *)c_startaddr ; addrptr < (long *)c_endaddr ; addrptr++) {
	    dataval = *addrptr;
	    if (dataval != PATTERN_B) {  /* check cache */
		cterr('f',0,"cache contents wrong at location: 0x%x data: 0x%x",
		      addrptr, dataval);
		return(1);
	    }
	}

	/* Now read back from memory again to see if cache read disturbed it */
	for (addrptr=(long *)u_startaddr ; addrptr < (long *)u_endaddr ; addrptr++) {
	    dataval = *addrptr;
	    if (dataval != (long)addrptr) {  /* read : data = address */
		cterr('f',0,"cached read disturbed memory at location: 0x%x data: 0x%x",
		      addrptr, dataval);
		return(1);
	    }
	}

	/* Next series tests further for cache cell loss after delay */

	/* First write 0..0xFF repeating pattern in successive bytes */
	byteptr = (uchar *)c_startaddr;
	for (i = 0; i < DCACHE_SIZE; i++)
	    *byteptr++ = i % 256;

	sleep(CACHE_READBACK_DELAY);

	byteptr = (uchar *)c_startaddr;
	for (i = 0; i < DCACHE_SIZE; i++) {
	    j = *byteptr++;
	    if (j != i % 256) {
		cterr('f',0,"cache loss at 0x%0.8x: got 0x%0.2x, expected 0x%0.2x",
		      --byteptr, j, i % 256);
		return(1);
	    }
	}

	dcache_flush();

	/* Next write PATTERN_A5 / PATTERN_5A in successive ulongs */
	pattern = PATTERN_A5;
	for (addrptr=(long *)c_startaddr; addrptr < (long *)c_endaddr ;) {
	    *addrptr++ = pattern;
	    pattern = ~pattern;
	}

	sleep(CACHE_READBACK_DELAY);

	pattern = PATTERN_A5;
	for (addrptr=(long *)c_startaddr; addrptr < (long *)c_endaddr; addrptr++) {
	    dataval = *addrptr;
	    if (dataval != pattern) {
		cterr('f',0,"cache loss (A5...) at 0x%0.8x: got 0x%0.8x, expected 0x%0.8x",
		      addrptr, dataval, pattern);
		return(1);
	    }
	    pattern = ~pattern;
	}

	dcache_flush();

	/* Finally write PATTERN_5A / PATTERN_A5 in successive ulongs */
	pattern = PATTERN_5A;
	for (addrptr=(long *)c_startaddr; addrptr < (long *)c_endaddr ;) {
	    *addrptr++ = pattern;
	    pattern = ~pattern;
	}

	sleep(CACHE_READBACK_DELAY);

	pattern = PATTERN_5A;
	for (addrptr=(long *)c_startaddr; addrptr < (long *)c_endaddr; addrptr++) {
	    dataval = *addrptr;
	    if (dataval != pattern) {
		cterr('f',0,"cache loss (5A...) at 0x%0.8x: got 0x%0.8x, expected 0x%0.8x",
		      addrptr, dataval, pattern);
		return(1);
	    }
	    pattern = ~pattern;
	}

	/* The following section may not work if the level2_cache is present
	 * so the following code need to be in condition. Since the cache write back invalidate
	 * put data into level2 cache and also in memory. The read from cache will be a
	 * read HIT and data will come from level2_cache, not memory.
	 */

	/*
	 * Execute the CACHE opcode Hit Writeback Invalidate on the range to effect
	 * writeback of the latest pattern in cache into memory.  Check that memory has been
	 * changed.  Then write uncached another pattern to memory, and read back cached.
	 * This last cached read should return the new pattern in memory as the cache
	 * is refilled because of the previous invalidation of all the entries.
	 */
	for (addrptr=(long *)c_startaddr; addrptr < (long *)c_endaddr; addrptr++) 
	    hit_wb_inval((long)addrptr);
	pattern = PATTERN_5A;
	for (addrptr=(long *)u_startaddr ; addrptr < (long *)u_endaddr ; addrptr++) {
	    dataval = *addrptr;
	    if (dataval != pattern) {
		cterr('f',0,"writeback to memory failed at location: 0x%x data: 0x%x",
		      addrptr, dataval);
		return(1);
	    }
	    pattern = ~pattern;
	}

	if (!level2_cache_present){
	    for (addrptr=(long *)u_startaddr; addrptr < (long *)u_endaddr ;)
		*addrptr++ = PATTERN_C;
	    for (addrptr=(long *)c_startaddr ; addrptr < (long *)c_endaddr ; addrptr++) {
		dataval = *addrptr;
		if (dataval != PATTERN_C) {
		    cterr('f',0,"cache refill from memory failed at location: 0x%x data: 0x%x",
			  addrptr, dataval);
		    return(1);
		}
	    }
	}
	/*
	 * Finally, test for correct set alternation on cache refills.  Disable
	 * cache exception errors during the test because of reads from unwritten
	 * memory.
	 */
	setcp0_sreg(sreg_sav | SREG_DE);
	for (addrptr=(long *)c_startaddr; addrptr < (long *)c_endaddr; ) {
	    retval = test_cacherefill(&addrptr);  /* addrptr gets bumped to next line */
	    if (retval != 0)
		/* Error detected, cterr() already called */
		return(retval);
	}
	setcp0_sreg(sreg_sav);
	phy_chunk_begin += CACHE_SIZE;   /* bump for the next chunk */
    }

    return(0);

} /* END test_orioncache */


/*
 *  Function: test_cacherefill
 *   Starting with the cached address given in *addrptr_ctrl, this function tests
 *   that refills occurring upon cache miss from a load, alternate properly between
 *   the two 8K sets within the orion data cache.  It does this by inspecting the
 *   cache tag lines in both sets at the indices corresponding to the address.
 *
 *   It tests for each long within the current cache line.  If an error occurs, this
 *   function calls cterr() with a message, and does return(1).  Otherwise, it returns
 *   zero after bumping the given address to the start of the next cache line.
 *
 *   NOTE> Rule for next set refill: if F bits are the same in both sets, select lo-index
 *         set; if different, choose hi-index set.  In any case, complement F-bit of 
 *         selected set.
 */

int test_cacherefill (long **addrptr_ctrl)
{
#define  BIT13  (long)0x00002000
#define  DISPL  (long)0x00004000     /* cache size (16K) */
    volatile long *addrptr;
    struct {
	int lo_set_bit;
	int hi_set_bit;
    } fbit_xrslt[4] = { 1, 0,  /* FIFO replacement bit progression from init 0,0 */
			1, 1,
		        0, 1,
		        0, 0 };
    volatile long *rd_addr1, *rd_addr2, *rd_addr_tmp;
    long displ, rd_tmp, ptag_rslt1, ptag_rslt2, ptag_xrslt1, ptag_xrslt2;
    int i;

    addrptr = *addrptr_ctrl;
    displ = (KSEG0_TO_PHY((long)addrptr) + 3*DISPL >= PHY_ADRSPC_RAM + memsize) ?
	-DISPL : DISPL;  /* Use negative displacement for reads if at end of mem */
    do {
	/* Initialize read addrs & tag lines */
	rd_addr1 = addrptr;
	setcp0_taglo(((PHY_ADDR((long)rd_addr1) >> 4) & TAGLO_PTAGLO) |
		     (3 << TAGLO_PSTATE_SHIFT));  /* pstate valid, F-bit 0 */
	xdstore_tag((long)rd_addr1 & ~BIT13);  /* low index */
 
	rd_addr2 = (long *)(((long)addrptr & ~BIT13) | /* in other set */
			    (~(long)addrptr & BIT13));
	setcp0_taglo(((PHY_ADDR((long)rd_addr2) >> 4) & TAGLO_PTAGLO) |
		     (3 << TAGLO_PSTATE_SHIFT));  /* pstate valid, F-bit 0 */
	xdstore_tag((long)rd_addr2 | BIT13);   /* high index */

	for (i = 0; i < 4; i++) {
	    /* Execute reads in both sets four times to cycle thru F-bit combos */
	    rd_addr_tmp = rd_addr1;
	    rd_addr1 = rd_addr2;             /* case of address already in line */
	    rd_addr2 = (long *)((uchar *)rd_addr_tmp + displ);  /* cause refill */
	    rd_tmp = *rd_addr1;
	    rd_tmp = *rd_addr2;
	    if (i & 1) {
		ptag_xrslt1 = (long)rd_addr1;
		ptag_xrslt2 = (long)rd_addr2;
	    } else {
		ptag_xrslt1 = (long)rd_addr2;
		ptag_xrslt2 = (long)rd_addr1;
	    }
	    ptag_xrslt1 = ((PHY_ADDR(ptag_xrslt1) >> 4) & TAGLO_PTAGLO) |
		(3 << TAGLO_PSTATE_SHIFT) | 
		    (fbit_xrslt[i].lo_set_bit << TAGLO_FBIT_SHIFT);
	    ptag_xrslt2 = ((PHY_ADDR(ptag_xrslt2) >> 4) & TAGLO_PTAGLO) |
		(3 << TAGLO_PSTATE_SHIFT) | 
		    (fbit_xrslt[i].hi_set_bit << TAGLO_FBIT_SHIFT);

	    /* In each cycle, check tag line in both sets */
	    dload_tag((long)rd_addr1 & ~BIT13);
	    ptag_rslt1 = getcp0_taglo();
	    dload_tag(((long)rd_addr1 | BIT13));
	    ptag_rslt2 = getcp0_taglo();
	    if (((ptag_rslt1 & ~TAGLO_PARITY) != ptag_xrslt1) ||
		((ptag_rslt2 & ~TAGLO_PARITY) != ptag_xrslt2)) {
		cterr('f',0,"discrepancy in cache line refill @ 0x%0.8x\nlo-index Taglo reg =\
 0x%0.8x  expected = 0x%0.8x\nhi-index Taglo reg = 0x%0.8x  expected = 0x%0.8x",
		      addrptr,
		      ptag_rslt1 & ~TAGLO_PARITY, ptag_xrslt1,
		      ptag_rslt2 & ~TAGLO_PARITY, ptag_xrslt2);
#ifdef DEBUG
		printf("\nrd1 = 0x%0.8x; rd2 = 0x%0.8x\n", rd_addr1, rd_addr2);
#endif DEBUG
		return(1);
	    }
	}

	addrptr++;
    } while (((long)addrptr & 0x1F) != 0);  /* Until next tag line */

    *addrptr_ctrl = (long *)addrptr;
    return(0);


} /* END test_cacherefill */

int
cache_info(int one)
{
    ulong config;

    config = getcp0_config();
    sys_clk_ratio = config >> 28;    /* 0 : processor clk frequency divided by 2
					1 : ---------------------------------- 3 */

    /* level2_cache_present is a global variable, 
     * will be set if level2_cache is present 
     */

    level2_cache_present = 0;
    level2_cache_present = level2_cache_detect();

    if (!one) {
	if (sys_clk_ratio)
	    printf("\nThe processor CLK frequency is 150MHz");
	else
	    printf("\nThe processor CLK frequency is 100MHz");

	if (level2_cache_present)
	    printf("\nSecondary cache present\n");
	else
	    printf("\nOnly Primary cache\n");
    }
} /* END cache_info */

/* End of Module */


