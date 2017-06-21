/* $Id: sizemem.c,v 1.1.68.1 1996/06/16 21:18:59 vnguyen Exp $
 * $Source: /release/112/cvs/Xsys/rommon/src-68-as5200/sizemem.c,v $
 *------------------------------------------------------------------
 * sizemem.c -- Brasil memory sizing routines.
 *
 * October 1995, Joe Turner
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: sizemem.c,v $
 * Revision 1.1.68.1  1996/06/16  21:18:59  vnguyen
 * Finally committing Brasil to California_branch
 *
 * Revision 1.1  1995/12/03  21:18:44  jturner
 * Placeholder for Brasil.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Includes
 */


#include "monitor.h"
#include "signal_4k.h"
#include "error.h"
#include "testmem.h"
#include "mon_defs.h"
#include "confreg.h"
#include "console.h"

/*
 * Defines
 */
#define ONE_K	    1024
#define EIGHT_K     (8*1024)
#define THIRTYTWO_K (32*1024)
#define HALFMEG     0x0080000
#define ONEMEG      0x0100000
#define TWOMEGS     0x0200000
#define FOURMEGS    0x0400000
#define EIGHTMEGS   0x0800000
#define SIXTEENMEGS   0x1000000

#define TEST_PAT1 0x55aa55aa
#define TEST_PAT2 0xaaa555aa
#define TEST_PAT3 0xa55aa55a

/*
 * Misc declarations
 */
long memsize;              /* size of CPU memory */
long shmemsize;            /* size of Packet memory */
unsigned long *shmemaddr;  /* address of share memory */

/*
 * int memsizing(void)
 */
int
memsizing(void) 
{
    int test_pat = 0xdeadbeef, num_pages;
    unsigned long *membase, *memptr;
    SIG_PF sigsav;
    short control2;
 
    if ((NVRAM_SAV)->confreg & DIAG_MASK)
	printf("\r\nSizing CPU RAM... ");
    /*
     * Ignore bus errors
     */
    sigsav = signal(SIGBUS,SIG_IGN);
    hkeepflags &= ~H_BUSERR;

    /*
     * Set control register for max size, 16MB,
     * then scan memory looking for wraparounds.
     */
    control2 = *(volatile ushort *)ADRSPC_CONTROL2;
    control2 |= (CONTROL2_MSIZE_16MB << CONTROL2_MSIZE_SHIFT);
    *(volatile ushort *)ADRSPC_CONTROL2 = control2;

    /*
     * Test the first location
     */
    membase = (unsigned long *)ADRSPC_RAM;
    *membase = test_pat;
    if (*membase != test_pat || hkeepflags & H_BUSERR) {
        cterr('f',0,"No memory found. Wrote first memory
		location expect %8x, receive %8x ", test_pat, *membase);
        signal(SIGBUS,sigsav);   /* set back to original value */
        return(0);                     /* zero memory is found */
    }

    /*
     * Test main CPU memory: 4, 8, 16M
     * Write current addresss at current address
     * to detect wraparounds.
     */
    num_pages = 0;
    for (memptr = (unsigned long *)ADRSPC_RAM;
        (unsigned long *)memptr < (unsigned long *)(SIXTEENMEGS);
        memptr += (HALFMEG/4)) {

            *memptr = (unsigned long)memptr;

            /*
             * If a later write has overwritten the zero that we first
             * wrote to location zero, we have wrapped around, and now
             * know the end of memory:
             */
            if (*(long *)ADRSPC_RAM != ADRSPC_RAM)
		break; 
	    num_pages++;
    }

    signal(SIGBUS,sigsav);   /* set back to original value */
    switch(num_pages) {
        case 8:		/* We found 4 MB of CPU RAM */
    	    control2 |=
                (CONTROL2_MSIZE_4MB << CONTROL2_MSIZE_SHIFT);
            break;
        case 16:	/* We found 8 MB of CPU RAM */
    	    control2 |=
                (CONTROL2_MSIZE_8MB << CONTROL2_MSIZE_SHIFT);
            break;
        case 32:	/* We found 16 MB of CPU RAM */
    	    control2 |=
                (CONTROL2_MSIZE_16MB << CONTROL2_MSIZE_SHIFT);
            break;
        default:
            printf("Found invalid number of 512 KB pages= %d ",num_pages);
            return(0);
    }
    /*
     * Write it out...
     */
    *(volatile ushort *)ADRSPC_CONTROL2 = control2;

    if ((NVRAM_SAV)->confreg & DIAG_MASK)
	printf("Found %dMB CPU RAM... ", (num_pages/2));
    return((num_pages*HALFMEG));
}

/*
 * The following routines are convenience tools provided for use
 * by other diagnostics.
 * The two sizing routines return the size of the memory or zero.
 * The start routines return the starting address or 0xffffffff.
 * Both routines will size the memory if it has not already been.
 */

long
sizemainmem()
{
    if(memsize) return(memsize);
    memsize = memsizing();
    return(memsize);
}

long
sizeshmem()
{
    if(shmemsize) return(shmemsize);
    shmemsize = shmemsizing();
    return(shmemsize);
}

/*
 * int shmemsizing(void)
 */
int
shmemsizing()
{
    int test_pat = 0xdeadbeef, num_pages;
    unsigned long *membase, *memptr;
    SIG_PF sigsav;
 
    if ((NVRAM_SAV)->confreg & DIAG_MASK)
	printf("\r\nSizing Packet RAM... ");
    /*
     * Ignore bus errors
     */
    sigsav = signal(SIGBUS,SIG_IGN);
    hkeepflags &= ~H_BUSERR;

    /*
     * Set control register for max size, 16MB,
     * then scan memory looking for wraparounds.
     */
    *(ushort *)ADRSPC_BRASIL_MB_CTRL1_REG =
		(PKT_MEM_SIXTEENMEGS<<PKT_MEM_SHIFT);

    /*
     * Test the first location
     */
    membase = (unsigned long *)ADRSPC_BRASIL_PKT_MEM;
    *membase = test_pat;
    if (*membase != test_pat || hkeepflags & H_BUSERR) {
        cterr('f',0,"No packet memory found. Wrote first packet memory
		location expect %8x, receive %8x ", test_pat, *membase);
        signal(SIGBUS,sigsav);   /* set back to original value */
        return(0);                     /* zero memory is found */
    }

    /*
     * Test packet memory: 4, 8, 16M
     * Write current addresss at current address
     * to detect wraparounds.
     */
    num_pages = 0;
    for (memptr = (ulong *)ADRSPC_BRASIL_PKT_MEM;
        (ulong *)memptr < (ulong *)(ADRSPC_BRASIL_PKT_MEM +SIXTEENMEGS);
        memptr += (HALFMEG/4)) {

            *memptr = (unsigned long)memptr;

            /*
             * If a later write has overwritten the zero that we first
             * wrote to location zero, we have wrapped around, and now
             * know the end of memory:
             */
            if (*(long *)ADRSPC_BRASIL_PKT_MEM != ADRSPC_BRASIL_PKT_MEM)
		break; 
	    num_pages++;
    }

    signal(SIGBUS,sigsav);   /* set back to original value */
    switch(num_pages) {
        case 8:		/* We found 4 MB of packet DRAM */
    	    *(volatile ushort *)ADRSPC_BRASIL_MB_CTRL1_REG =
			(PKT_MEM_FOURMEGS<<PKT_MEM_SHIFT);
            break;
        case 16:	/* We found 8 MB of packet DRAM */
    	    *(volatile ushort *)ADRSPC_BRASIL_MB_CTRL1_REG =
			(PKT_MEM_EIGHTMEGS<<PKT_MEM_SHIFT);
            break;
        case 32:	/* We found 16 MB of packet DRAM */
    	    *(volatile ushort *)ADRSPC_BRASIL_MB_CTRL1_REG =
			(PKT_MEM_SIXTEENMEGS<<PKT_MEM_SHIFT);
            break;
        default:
            printf("Found invalid number of 512 KB pages= %d ",num_pages);
            return(0);
    }
    if ((NVRAM_SAV)->confreg & DIAG_MASK)
	printf("Found %dMB Packet RAM... ", (num_pages/2));
    return((num_pages*HALFMEG));
}

long
mainmemstart()
{
    return(sizemainmem() == 0 ? (long)-1 : (long)ADRSPC_RAM);
}

long
shmemstart()
{
    /* Return the size of shared (aka Packet) memory */
    return(sizeshmem() == 0 ? (long)-1 : (long)ADRSPC_BRASIL_PKT_MEM);
}

long freememstart;

long
getfreememstart(void)
{
    if(!freememstart)
	freememstart = (long)(errlog_start+ERRLOG_SIZE);
    return(freememstart);
}


long
size_nvram(void)
{
    unsigned long testbuf, readval;
    char *nverrmsg = "... cannot size NVRAM\n";
    unsigned long *base_ptr = &(NVRAM_SAV)->testarea;
 
    testbuf = TEST_PAT1;
    if(nvwrite((uchar *)&testbuf, (uchar *)base_ptr, sizeof(testbuf)) < 0) {
write_err:
        printf("Error writing NVRAM %s", nverrmsg);
size_err:
        /*
         * We could invalidate the NVRAM chip select here but than all
         * accesses to this area would cause a bus error.  I doubt we
         * want this behaviour.
         *
         */
        return(0);
    }
    readval = *base_ptr;
    if(readval != testbuf) {
        printf("First location in NVRAM fails %s", nverrmsg);
        goto size_err;
    }
    return(128*ONE_K);  /* brasil has 128k */
}

/* 
 * Test main memory.
 *
 * Returns :
 *    0    : test complete
 *    1    : failed
 */
int
sysmemtest(void)
{
    struct testmem tmem;
    register struct testmem *tmemptr=&tmem;
    long msize;
 
    msize = sizemainmem();
    testname("main memory");
    if(!msize) {
        cterr('w',0,"cannot size main memory - aborting test");
        return(1);
    }
    tmemptr->start = (unsigned *)(MONITOR_STACK + ERRLOG_SIZE);
    tmemptr->length = msize - ((int)tmemptr->start - ADRSPC_RAM);
    tmemptr->flag = INDIAGS;
    tmemptr->passcount = 1;  /* do complete mem test once */
 
    if(testmem(tmemptr) < 0) return(1);
    return(0);
}


/*
 * display the memory info for the user
 */
int
meminfo(int argc, char *argv[])
{
    long msize = sizemainmem();
    long shmsize = sizeshmem();
 
    printf("\nMain memory size: %d MB. NVRAM size: %d KB",
           msize>>20, size_nvram()>>10);
    printf("\nAvailable main memory starts at %#x, size %#x",
           MONITOR_FREE_MEM_BASE, (msize + ADRSPC_RAM) - MONITOR_FREE_MEM_BASE);
    printf("\nPacket memory size: %d MB.\n\n", shmsize>>20);
    return(0);
}

/* end of module */
