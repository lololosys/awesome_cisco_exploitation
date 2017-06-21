/* $Id: second_cache.c,v 3.1.60.1 1996/03/21 23:30:59 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/rommon/src-4k-c4000/second_cache.c,v $
 *------------------------------------------------------------------
 * second_cache.c
 *
 * June 1995, Anh Dang
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Secondary cache routines for R4000/Orion CPU.
 *------------------------------------------------------------------
 * $Log: second_cache.c,v $
 * Revision 3.1.60.1  1996/03/21  23:30:59  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.1.4.1  1995/12/01  04:23:26  mbeesley
 * Branch: ELC_branch
 * Reorganize Rom monitor header files to make it easier to port
 * to new platforms.
 *
 * Revision 3.1  1995/11/09  13:08:23  shaker
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
#include "cpu_mips.h"
#include "mon_defs.h"
#include "queryflags.h"
#include "signal_4k.h"
#include "error.h"
#include "proto.h"

#define LEVEL2_CACHE_512K 0x80000  /* 512K bytes */
#define CACHE_SIZE        0x04000  /* 16K bytes */
#define DATA_1 0x12345678
#define DATA_2 0xabcdefab
#define PATTERN_A5   (ulong) 0xA5A5A5A5
#define PATTERN_5A   (ulong) 0x5A5A5A5A
#define PATTERN_DB6C (ulong) 0xDB6CDB6C
#define CHUNK1      1
#define CHUNK2      2
#define CHUNK3      3
#define CHUNK4      4
#define PAT_A5      5
#define PAT_5A      6
#define RIPPLE_ONE  7
#define RIPPLE_ZERO 8
#define PAT_ADDR    9
#define PHY_PAT_ADDR 10
#define ZERO         11
#define DATA_INC     12
#define PAT_DB6C     13

char * chunk_data[5] = {"","PAT_A5","RIPPLE_ONE","PAT_5A","RIPPLE_ZERO"};
char test_one_chunk[]= "Level2 cache test on 512K bytes:\n";
char test_more_chunk[]= "Level2 cache test on multiple 512K bytes:\n";

/*
 *********************************************************************************
 * Function: level2_cache_detect
 *  The algorithm for detecting the level2 cache is as follows:
 *  - flush P_cache
 *  - write to uncache memory data = addr, readback verify memory is good
 *  - write to P_cache pattern1, verify P_cache is OK
 *  - flush P_cache, pattern1 will be in both level2_cache (if present) and memory 
 *  - write to uncache memory with pattern2
 *  - read P_cache. if level2_cache present then we get pattern1
 *                  if level2_cache not present then we get pattern2 
 *
 * Return: 1 if level2_cache present
 *         0 if level2_cache not present
 *********************************************************************************
 */

int
level2_cache_detect (void)
{
    volatile long *c_addrptr, *u_addrptr, dataval, dataval1;
    volatile ulong c_startaddr, u_startaddr, c_endaddr, u_endaddr;
    ulong phy_chunk_begin, phy_chunk_len, phy_addr_end;
    struct nvram *nvptr = NVRAM;


    phy_chunk_begin = nvptr->start;
    phy_addr_end = phy_chunk_begin + nvptr->length;
#ifdef DEBUG
    printf("\nphy_chunk_begin: %lx, phy_addr_end: %lx",phy_chunk_begin, phy_addr_end);
#endif

    dcache_flush();     
    level2_cache_flush();

    u_startaddr = PHY_TO_KSEG1(phy_chunk_begin);  /* Uncached virtual address */
    u_endaddr   = PHY_TO_KSEG1(phy_addr_end);     /* Uncached virtual address */
    fill_data (u_startaddr, u_endaddr, ZERO);     /* Initialize the memory */
    u_addrptr   = (volatile long *)u_startaddr;
    *u_addrptr  = (volatile long)u_addrptr;       /* write: data = addr */
    dataval     = *(volatile long *)u_addrptr;
    if (dataval != (volatile long)u_addrptr)
	cterr('w',0,"uncached readback fails at location: 0x%0.8x"
	      "data expect: 0x%0.8x, data rec: 0x%0.8x", 
	      u_addrptr, u_addrptr, dataval);
    /* 
     * read cache, read miss, fill S_cache and P_cache with data = addr from memory 
     * write to data1 cache, this data only stay in P_cache
     * Verify data in cache 
     */

    c_startaddr = PHY_TO_KSEG0(phy_chunk_begin);  /* Cached virtual address */
    c_addrptr   = (volatile long *)c_startaddr;
    dataval     = *((volatile long *)c_addrptr);  /* read miss, read: data = addr */
    *((volatile long *)c_addrptr)  = (long)DATA_1;/* write new new data_1*/
    dataval     = *((volatile long *)c_addrptr);  /* read: data = data_1 */
    if (dataval != (long)DATA_1)
	cterr('w',0,"primary cached readback fails at location: 0x%0.8x"
	      "data expect: 0x%0.8x, data rec: 0x%0.8x",
	      c_addrptr, DATA_1, dataval);

    /* 
     * flush the P_cache, write hit,
     * pattern1 will go to memory and level2_cache if present
     * because this write is a write hit.
     * change data in uncache to pattern2.
     * read P_cache if data = pattern2 then only P_cache
     * read P_cache if data = pattern1 then level2_cache present
     */    

    dcache_flush(); 
    dataval    = *(volatile long *)u_addrptr;    /* read: data = data_1 */

    *(volatile long *)u_addrptr  = (long)DATA_2; /* write: data = data_2 */
    dataval     = *(volatile long *)c_addrptr;

    if (dataval == DATA_2)
	return(0); /* only P_cache */     

     return(1);    /* level2_cache present */

} /* END level2_cache_detect */


/* 
 ******************************************************************************
 * Function: test_level2_cache
 * Each chunk of memory is 512K bytes. Four chunks of memory is used for testing
 * - Flush the P_cache (primary)
 * - Map to uncache(Kseg1) Test memory: all four chunks of memory
 *******************************************************************************
 */
test_level2_cache()
{
    long *addrptr, dataval, memlen;
    ulong c_startaddr, u_startaddr, c_endaddr, u_endaddr;
    ulong phy_chunk_begin, phy_chunk_len, phy_addr_end;
    struct nvram *nvptr = NVRAM;
    int phase, retval;
    uchar *byteptr;
    int i, j, pattern, data_inc, data_save;

   if ((memlen = sizemainmem()) == 0) {
	cterr('f',0,"problem sizing main memory");
	return (1);
    }

    nvptr->start = memlen - 6*LEVEL2_CACHE_512K;
    nvptr->length = 4*LEVEL2_CACHE_512K;

    dcache_flush();
	
    /* Initialize to test in LEVEL2_CACHE_SIZE chunks. */
    phy_chunk_begin = nvptr->start;
    phy_addr_end = phy_chunk_begin + nvptr->length;
#ifdef DEBUG
    printf("\nTest all four chunks of memory before using, addr: 0x%.8x len: 0x%.8x",
	   phy_chunk_begin, phy_addr_end);
#endif
    /*
     * via uncached segment, for the given range write
     * to each cell its address.  Read it back uncached again to check that the
     * the block is OK in memory.
     */
    u_startaddr = PHY_TO_KSEG1(phy_chunk_begin);  /* Uncached virtual address */
    u_endaddr   = PHY_TO_KSEG1(phy_addr_end);

    fill_data(u_startaddr, u_endaddr, PAT_ADDR);
    if (verify_data(u_startaddr, u_endaddr, PAT_ADDR))
	return(1);

    if (level2_cache_test_one_chunk())
	return(1);

    if (level2_cache_test_multi_chunk())
	return(1);
    
    return(0);

}
/*
 *****************************************************************************
 * Function: level2_cache_test_one_chunk
 *           Only 512K of memory is used for testing
 *****************************************************************************
 */

level2_cache_test_one_chunk()
{
    char err_msg[60];
    char cache_msg[80];
    long *addrptr, dataval, memlen, dataexpt;
    ulong c_startaddr, u_startaddr, c_endaddr, u_endaddr;
    ulong phy_chunk_begin, phy_chunk_len, phy_addr_end;
    struct nvram *nvptr = NVRAM;
    int phase, retval;
    uchar *byteptr;
    int i, j, pattern ,data_inc, data_save;

    phy_chunk_begin = nvptr->start;
    phy_addr_end    = phy_chunk_begin + LEVEL2_CACHE_512K;
    u_startaddr     = PHY_TO_KSEG1(phy_chunk_begin);  /* Uncached virtual address */
    u_endaddr       = PHY_TO_KSEG1(phy_addr_end);
    c_startaddr     = PHY_TO_KSEG0(phy_chunk_begin);  /* cached virtual address */
    c_endaddr       = PHY_TO_KSEG0(phy_addr_end);

#ifdef DEBUG
    printf("\nUncache start:%0.8x, end: %0.8x, Cache start:%0.8x, end:%0.8x",
	   u_startaddr, u_endaddr, c_startaddr, c_endaddr);
#endif
    phase = 1;
    sprintf(cache_msg,"512K Secondary");
    prpass(testpass, "phase %d, %s", phase++, cache_msg);  /* phase 1 */
    
    dcache_flush();
    level2_cache_flush();

    /* 
     * write uncache with data = addr, this only in memory, verify
     * read cache, read miss, data fill in S_cache and P_cache, verify
     * Verify uncache data again after read miss, data = addr
     */

    fill_data (u_startaddr, u_endaddr, PAT_ADDR);

    if (verify_data(u_startaddr, u_endaddr, PAT_ADDR, err_msg)){
	cterr('f',0,"%s"
	      "Verify uncache memory data = addr failed\n"
	      "%s",test_one_chunk, err_msg);
	return(1);
    }
    
    if (verify_data(c_startaddr, c_endaddr, PHY_PAT_ADDR, err_msg)){
	cterr('f',0,"%s"
	      "Cache READ MISS to fill level2 cache failed\n"
	      "%s",test_one_chunk, err_msg);
	return(1);
    }

    if (verify_data(u_startaddr, u_endaddr, PAT_ADDR, err_msg)){
	cterr('f',0,"%s"
	      "Cache READ MISS has disturbed uncache memory data = addr failed\n"
	      "%s",test_one_chunk, err_msg);
	return(1);
    }

    /*
     * Change data in memory to increment data,
     * Verify data in memory data = increment
     * read cache, data in cache should not change, should be data = addr
     * Verify data in cache data = addr
     * Verify data in memory again data = increment
     */

    fill_data (u_startaddr, u_endaddr, DATA_INC);

    if (verify_data(u_startaddr, u_endaddr, DATA_INC, err_msg)){
	cterr('f',0,"%s"
	      "Verify uncache memory change to data = increment failed\n"
	      "%s", test_one_chunk, err_msg);
	return(1);
    }
	
    if (verify_data(c_startaddr, c_endaddr, PHY_PAT_ADDR, err_msg)){
	cterr('f',0,"%s"
	      "Alter data in uncache memory to data = increment\n"
	      "has disturbed level2 cache READ HIT\n"
	      "%s", test_one_chunk, err_msg);
	return(1);
    }

    if (verify_data(u_startaddr, u_endaddr, DATA_INC, err_msg)){
	cterr('f',0,"%s"
	      "Cache READ HIT disturbed uncache memory data = increment\n"
	      "%s", test_one_chunk, err_msg);
	return(1);
    }
	
    /* 
     * write data a5 in cache, write hit, expect data write both mem and S_cache
     * Verify data in memory = a5
     * Clear data in memory = 0
     * verify cache read hit, data = a5
     * write data db6cdb6c in cache, write hit, expect data write both mem and S_cache
     * verify data in memory = db6cdb6c
     * Fill memory with = 5a
     * verify cache read hit again, data = db6cdb6c
     * Flush the second level cache
     * read cache again, read miss, verify data = 5a
     */
    prpass(testpass, "phase %d, %s", phase++, cache_msg);  /* phase 2 */

    fill_data (c_startaddr, c_endaddr, PAT_A5);
    
    dcache_flush();  /* let the last 16K from P_cache flush into S_cache and mem*/

    if (verify_data(u_startaddr, u_endaddr, PAT_A5, err_msg)){
	cterr('f',0,"%s"
	      "WRITE HIT pattern A5,uncached readback fails\n"
	      "%s", test_one_chunk, err_msg);
	return(1);
    }
 
    fill_data (u_startaddr, u_endaddr, ZERO);

    if (verify_data(c_startaddr, c_endaddr, PAT_A5, err_msg)){
	cterr('f',0,"%s"
	      "READ HIT after WRITE HIT pattern A5,cached readback fails\n"
	      "%s", test_one_chunk, err_msg);
	return(1);
    }

    prpass(testpass, "phase %d, %s", phase++, cache_msg);  /* phase 3 */
 
    fill_data (c_startaddr, c_endaddr, PAT_DB6C);

    dcache_flush();  /* let the last 16K from P_cache flush into S_cache and mem*/

    pattern = PATTERN_DB6C;

    if (verify_data(u_startaddr, u_endaddr, PAT_DB6C, err_msg)){
	cterr('f',0,"%s"
	      "WRITE HIT pattern DB6C, uncached readback fails\n"
	      "%s", test_one_chunk, err_msg);
	return(1);
    }

    fill_data (u_startaddr, u_endaddr, PAT_5A);

    if (verify_data(c_startaddr, c_endaddr, PAT_DB6C, err_msg)){
	cterr('f',0,"%s"
	      "READ HIT after WRITE HIT pattern DB6C,cached readback fails\n"
	      "%s", test_one_chunk, err_msg);
	return(1);
    }

    prpass(testpass, "phase %d, %s", phase++, cache_msg);  /* phase 4 */

    level2_cache_flush(); /* flush S_cache, read cache again will be miss */

    if (verify_data(c_startaddr, c_endaddr, PAT_5A, err_msg)){
	cterr('f',0,"%s"
	      "READ MISS after level2 cache flush,cached readback fails\n"
	      "%s", test_one_chunk, err_msg);
	return(1);
    }

     return(0);
}


/*
 ******************************************************************************
 * Function: level2_cache_test_multi_chunk
 *           Five chunks of memory (512K each) are used for testing
 ******************************************************************************
 */

level2_cache_test_multi_chunk()
{
    char err_msg[60];
    char cache_msg[80];
    long *addrptr, dataval, memlen;
    ulong c_startaddr, u_startaddr, c_endaddr, u_endaddr;
    ulong c_start_next, u_start_next, c_end_next, u_end_next;
    ulong phy_chunk_begin, phy_chunk_len, phy_addr_end;
    struct nvram *nvptr = NVRAM;
    int phase, retval;
    uchar *byteptr;
    int i, j, pattern, chunk, chunk_index,data_inc, data_save;

    /*
     * Flush P_cache
     * Flush S_cache
     * Initialize all four chunks of uncache memory again with data = addr
     * For loop of four chunks
     *  - Read cache, READ MISS, verify data = addr
     *  - Change uncache memory new_data in that chunk
     *  - Read cache, READ HIT, verify data = addr
     *  - Flush S_cache
     *  - Read cache, READ MISS, verify data = new_data, beware the last 16K
     *    in this chunk will have data write back from P_cache with data_addr
     *    may need to experiment with invalidate P_cache.
     *   
     */

    sprintf (cache_msg,"Multiple 512K Secondary");
    prpass(testpass, "%s", cache_msg);  

    dcache_flush();
    level2_cache_flush();

    phy_chunk_begin = nvptr->start;
    phy_addr_end = phy_chunk_begin + nvptr->length + LEVEL2_CACHE_512K;
#ifdef DEBUG
    printf("\nTest all four chunks of memory before using, addr: 0x%.8x len: 0x%.8x",
	   phy_chunk_begin, phy_addr_end);
#endif
    /*
     * via uncached segment, for the given range write
     * to each cell its address.  Read it back uncached again to check that the
     * the block is OK in memory.
     */
    u_startaddr = PHY_TO_KSEG1(phy_chunk_begin);  /* Uncached virtual address */
    u_endaddr   = PHY_TO_KSEG1(phy_addr_end);

    fill_data(u_startaddr, u_endaddr, PAT_ADDR);
    if (verify_data(u_startaddr, u_endaddr, PAT_ADDR))
	return(1);

    for (chunk_index=1; chunk_index<=4;chunk_index++){
	u_startaddr = PHY_TO_KSEG1(phy_chunk_begin);  /* Uncached virtual address */
	u_endaddr   = u_startaddr + LEVEL2_CACHE_512K;
	c_startaddr = PHY_TO_KSEG0(phy_chunk_begin);  /* cached virtual address */
	c_endaddr   = c_startaddr + LEVEL2_CACHE_512K;

	phase = 1;
	sprintf(cache_msg,"At:0x%0.8x Multiple 512K Secondary", phy_chunk_begin);

	prpass(testpass, "phase %d, %s", phase++, cache_msg); /* phase 1 */

	/*
	 * Since all cache flush, the next cache read will be READ MISS
	 * Verify data = address, only verify physical addr since write in KSEG1
	 * and read in KSEG0
	 */

	if (verify_data(c_startaddr, c_endaddr, PHY_PAT_ADDR, err_msg)){
	    cterr('f',0,"%s READ MISS verify cache data = addr\n"
		  "%s",test_more_chunk,err_msg);
	    return(1);
	}

	/* Change uncache memory new_data */
	fill_data(u_startaddr, u_endaddr, chunk_index);

	/*
	 * Read cache again, this is READ HIT with data = addr, verify
	 */

	if (verify_data(c_startaddr, c_endaddr, PHY_PAT_ADDR, err_msg)){
	    cterr('f',0,"%s READ HIT verify cache data = addr\n"
		  "%s",test_more_chunk,err_msg);
	    return(1);
	}

	/*
	 * Flush S_cache, read cache again will be MISS with new_data, verify
	 */
	level2_cache_flush();

	if (verify_data(c_startaddr, c_endaddr, chunk_index, err_msg)){
	    cterr('f',0,"%s READ MISS verify cache data = %s\n"
		  "%s",test_more_chunk,chunk_data[chunk_index],err_msg);
	    return(1);
	}

	/*
	 * Clear data in uncache data
	 * write data = addr in the next cache chunk, expect WRITE MISS, this write
	 * to both level2 cache and memory
	 * Verify cache data in previous chunk, READ MISS with data update in cache
	 * Verify uncache data in previous chunk is clear
	 * Verify uncache data in next chunk with data = addr
	 */
	prpass(testpass, "phase %d, %s", phase++, cache_msg); /* phase 2 */

	fill_data(u_startaddr, u_endaddr, ZERO);

	if (verify_data(c_startaddr, c_endaddr, chunk_index, err_msg)){
	    cterr('f',0,"%s READ HIT verify cache data = %s\n"
		  "%s",test_more_chunk,chunk_data[chunk_index],err_msg);
	    return(1);
	}

	u_start_next = u_endaddr;   /* Uncached virtual address */
	u_end_next   = u_start_next + LEVEL2_CACHE_512K;
	c_start_next = c_endaddr;   /* Cached virtual address */
	c_end_next   = c_start_next + LEVEL2_CACHE_512K;
#ifdef DEBUG
	printf("\nnext chunk start:%lx, next chunk end: %lx",
	       c_start_next, c_end_next);
#endif
	fill_data(c_start_next, c_end_next, PAT_ADDR); /* write MISS  */

	/*
	 * Read cache again in the previous chunk, expect the cache READ MISS 
	 * this will get data from memory which has been clear
	 */

	if (verify_data(c_startaddr, c_endaddr, ZERO, err_msg)){
	    cterr('f',0,"%sREAD MISS following WRITE MISS\n"
		  "%s",test_more_chunk,err_msg);
	    return(1);
	}
	if (verify_data(u_startaddr, u_endaddr, ZERO, err_msg)){
	    cterr('f',0,"%sREAD MISS following WRITE MISS, disturbed uncache data\n"
		  "%s",test_more_chunk,err_msg);
	    return(1);
	}
	if (verify_data(u_start_next, u_end_next, PHY_PAT_ADDR, err_msg)){
	    cterr('f',0,"%sWRITE MISS, did not write to memory\n"
		  "%s",test_more_chunk,err_msg);
	    return(1);
	}
	
	
	phy_chunk_begin += LEVEL2_CACHE_512K; /* bump for the next chunk */
    } /* end for loop */

    return(0);

} /* END level2_cache_test_multi_chunk */
/*
 ***************************************************************************
 * Function: fill_data
 * Input: start addr, end addr, pattern
 *        pattern: PAT_ADDR            data = addr
 *                 DATA_INC            data = increment data pattern
 *                 PAT_A5, CHUNK1      data = checker board A5A5A5A5A5
 *                 PAT_5A, CHUNK3      data = checkerboard  5A5A5A5A5A
 *                 PAT_RIPPLE_ZERO, CHUNK4 data = 01,02,04...
 *                 PAT_RIPPLE_ONE, CHUNK2  data = 0xfffffffe...
 **************************************************************************
 */
fill_data(ulong startaddr, ulong endaddr, int pattern_index)
{
    long *addrptr, dataval, memlen;
    int pattern;
    
    switch (pattern_index){
    case PAT_ADDR:  /* data = addr */
	for (addrptr=(long *)startaddr; addrptr < (long *)endaddr ;){
	    *addrptr++ = (long)addrptr;   /* write : data = address */
	}
	break;

    case CHUNK1:
    case PAT_A5: /* checker board */
	pattern = PATTERN_A5;
	for (addrptr=(long *)startaddr; addrptr < (long *)endaddr ;) {
	    *addrptr++ = (long)pattern;   /* write : data = checker board */
	    pattern =  ~pattern;
	}
	break;

    case CHUNK2:
    case RIPPLE_ONE:
	pattern = 0x1;
	for (addrptr=(long *)startaddr; addrptr < (long *)endaddr ;) {
	    *addrptr++ = (long)pattern;   /* write : data = ripple one */
	    pattern =  pattern<<1;
	    if (!pattern) 
		pattern = 0x1;
	}
	break;

    case CHUNK3:	
    case PAT_5A: /* checker board */
	pattern = PATTERN_5A;
	for (addrptr=(long *)startaddr; addrptr < (long *)endaddr ;) {
	    *addrptr++ = (long)pattern;   /* write : data = checker board */
	    pattern =  ~pattern;
	}
	break;

    case CHUNK4:
    case RIPPLE_ZERO:
	pattern = 0xfffffffe;
	for (addrptr=(long *)startaddr; addrptr < (long *)endaddr ;) {
	    *addrptr++ = (long)pattern;   /* write : data = ripple zero */
	    pattern =  (pattern<<1)| 1;
	    if (pattern == 0xffffffff)
		pattern = 0xfffffffe;
	    }
	break;

    case ZERO:
	for (addrptr=(long *)startaddr; addrptr < (long *)endaddr ;) 
	    *addrptr++ = (long)0;   /* write : data = zero */
	break;

    case DATA_INC:
	pattern = 0;
	for (addrptr=(long *)startaddr; addrptr < (long *)endaddr ;) 
	    *addrptr++ = pattern++;   /* write : data = increment */
	break;

    case PAT_DB6C: /* checker board */
	pattern = PATTERN_DB6C;
	for (addrptr=(long *)startaddr; addrptr < (long *)endaddr ;) {
	    *addrptr++ = (long)pattern;   /* write : data = checker board */
	    pattern =  ~pattern;
	}
	break;

    default:
	printf("\nPattern_index to fill is not valid");
	break;

    } /* END switch */
	
} /* END fill_data */

/*
 ***************************************************************************
 * Function: verify_data
 * Input: start addr, end addr, pattern
 *        pattern: PAT_ADDR  data = addr
 *                 DATA_INC            data = increment data pattern
 *                 PAT_A5, CHUNK1      data = checker board A5A5A5A5A5
 *                 PAT_5A, CHUNK3      data = checkerboard  5A5A5A5A5A
 *                 PAT_RIPPLE_ZERO, CHUNK4 data = 01,02,04...
 *                 PAT_RIPPLE_ONE, CHUNK2  data = 0xfffffffe...
 **************************************************************************
 */
verify_data(ulong startaddr, ulong endaddr, int pattern_index, char *err_msg)
{
    long *addrptr, dataval, memlen;
    int pattern;
    
    switch (pattern_index){
    case PAT_ADDR:  /* data = addr */
	for (addrptr=(long *)startaddr; addrptr < (long *)endaddr ; addrptr++){
	    dataval = *addrptr;
	    if (dataval != (long)addrptr) {
		sprintf(err_msg,"at  0x%0.8x: got 0x%0.8x, expected 0x%0.8x",
			addrptr, dataval, addrptr);
		return(1);
	    }
	}
	break;

    case PHY_PAT_ADDR:  /* data = physical addr */
	for (addrptr=(long *)startaddr; addrptr < (long *)endaddr ; addrptr++){
	    dataval = PHY_ADDR(*addrptr);
	    if (dataval != PHY_ADDR((long)addrptr)) {
		sprintf(err_msg,"at  0x%0.8x: got 0x%0.8x, expected 0x%0.8x",
			addrptr, dataval, addrptr);
		return(1);
	    }
	}
	break;

    case CHUNK1:
    case PAT_A5: /* checker board */
	pattern = PATTERN_A5;
	for (addrptr=(long *)startaddr; addrptr < (long *)endaddr ; addrptr++) {
	    dataval = *addrptr;
	    if (dataval != pattern) {
		sprintf(err_msg,"at  0x%0.8x: got 0x%0.8x, expected 0x%0.8x",
			addrptr, dataval, pattern);
		return(1);
		}
	    pattern =  ~pattern;
	}
	break;

    case CHUNK2:
    case RIPPLE_ONE:
	pattern = 0x1;
	for (addrptr=(long *)startaddr; addrptr < (long *)endaddr ; addrptr++) {
	    dataval = *addrptr;
	    if (dataval != pattern) {
		sprintf(err_msg,"at  0x%0.8x: got 0x%0.8x, expected 0x%0.8x",
		      addrptr, dataval, pattern);
		return(1);
		}
	    pattern =  pattern<<1;
	    if (!pattern) 
		pattern = 0x1;
	}
	break;
	
    case CHUNK3:
    case PAT_5A: /* checker board */
	pattern = PATTERN_5A;
	for (addrptr=(long *)startaddr; addrptr < (long *)endaddr ; addrptr++) {
	    dataval = *addrptr;
	    if (dataval != pattern) {
		sprintf(err_msg,"at  0x%0.8x: got 0x%0.8x, expected 0x%0.8x",
			addrptr, dataval, pattern);
		return(1);
		}
	    pattern =  ~pattern;
	}
	break;

    case CHUNK4:
    case RIPPLE_ZERO:
	pattern = 0xfffffffe;
	for (addrptr=(long *)startaddr; addrptr < (long *)endaddr ; addrptr++) {
	    dataval = *addrptr;
	    if (dataval != pattern) {
		sprintf(err_msg,"at  0x%0.8x: got 0x%0.8x, expected 0x%0.8x",
			addrptr, dataval, pattern);
		return(1);
		}
	    pattern =  (pattern<<1)| 1;
	    if (pattern == 0xffffffff)
		pattern = 0xfffffffe;
	    }
	break;

    case ZERO:
	for (addrptr=(long *)startaddr; addrptr < (long *)endaddr ; addrptr++) {
	    dataval = *addrptr;
	    if (dataval) {
		sprintf(err_msg,"at  0x%0.8x: got 0x%0.8x, expected 0x%0.8x",
			addrptr, dataval, 0x0);
		return(1);
		}
	}
	break;

    case DATA_INC:
	pattern = 0;
	for (addrptr=(long *)startaddr; addrptr < (long *)endaddr ; addrptr++) {
	    dataval = *addrptr;
	    if (dataval != pattern) {
		sprintf(err_msg,"at  0x%0.8x: got 0x%0.8x, expected 0x%0.8x",
			addrptr, dataval, pattern);
		return(1);
		}
	    pattern++;
	}
	break;
	
    case PAT_DB6C: /* checker board */
	pattern = PATTERN_DB6C;
	for (addrptr=(long *)startaddr; addrptr < (long *)endaddr ; addrptr++) {
	    dataval = *addrptr;
	    if (dataval != pattern) {
		sprintf(err_msg,"at  0x%0.8x: got 0x%0.8x, expected 0x%0.8x",
			addrptr, dataval, pattern);
		return(1);
		}
	    pattern =  ~pattern;
	}
	break;

    default:
	printf("\nPattern_index to verify is not valid");
	break;


    } /* END switch */
	
} /* END verify_data */

/*
 ****************************************************************
 * Function: display_data
 *           handy for debug purpose
 *****************************************************************
 */

display_data(ulong startaddr, ulong len)
{
    long *addrptr, dataval, memlen;
    ulong endaddr;

    endaddr = startaddr + len;
    printf("\n display data start = %0.8x, end = %0.8x\n", startaddr, endaddr);
    for (addrptr=(long *)startaddr; addrptr < (long *)endaddr ; addrptr++)
	printf("\n    addr:%0.8x data = %0.8x  ",addrptr, *addrptr);
}

