/* $Id: sizemem.c,v 3.2.58.1 1996/03/21 23:29:23 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/rommon/src-36-c1000/sizemem.c,v $
 *------------------------------------------------------------------
 * sizemem.c
 *
 * November 1994, Rob Clevenger
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Memory sizing routines.
 *------------------------------------------------------------------
 * $Log: sizemem.c,v $
 * Revision 3.2.58.1  1996/03/21  23:29:23  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.2.2.1  1995/12/01  04:22:23  mbeesley
 * Branch: ELC_branch
 * Reorganize Rom monitor header files to make it easier to port
 * to new platforms.
 *
 * Revision 3.2  1995/11/17  18:44:31  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:07:39  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:39:36  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#include "monitor.h"
#include "signal_4k.h"
#include "error.h"
#include "testmem.h"
#include "mon_defs.h"
#include "quicc.h"
#include "console_36.h"

#define EIGHT_K     (8*1024)
#define THIRTYTWO_K (32*1024)
#define HALFMEG     0x0080000
#define ONEMEG      0x0100000
#define TWOMEGS     0x0200000
#define FOURMEGS    0x0400000
#define EIGHTMEGS   0x0800000
#define SIXTEENMEGS 0x1000000

long memsize;              /* size of main memory */
long shmemsize;            /* size of shared memory */
unsigned long *shmemaddr;  /* address of share memory */

#define TEST_PAT1 0x55aa55aa
#define TEST_PAT2 0xaaa555aa
#define TEST_PAT3 0xa55aa55a

/*
 *	probing dram
 *
 *	There are 3 simm sizes supported
 *
 *------------------------------------
 *       total      chip
 *        dram      size    banks
 *		(megs)    (megs)
 *------------------------------------
 *  1.    16         4        1
 *  2.     8         1        2
 *  3.     4         1        1
 *------------------------------------
 *
 */

static int
probe_dram(probe_size)
int probe_size;
{
    volatile unsigned long *base, *baseplus4m, *baseplus8m;
    volatile unsigned long pat1, pat2, pat3;

    base = (unsigned long *)ADRSPC_RAM;
    baseplus4m = (unsigned long *)(ADRSPC_RAM + FOURMEGS);
    baseplus8m = (unsigned long *)(ADRSPC_RAM + EIGHTMEGS);

    pat1 = TEST_PAT1;
    pat2 = TEST_PAT2;

    *base = pat1;
    *baseplus4m = pat2;

    switch (probe_size) {
    case SIXTEENMEGS:
    	pat3 = TEST_PAT3;
    	*baseplus8m = pat3;
    	if(*base == pat1 && *baseplus4m == pat2 && *baseplus8m == pat3)
	    return(SIXTEENMEGS);  /* no values get clobbered */
	break;
	
    case EIGHTMEGS:
    	if(*base == pat1  && *baseplus4m == pat2)
	    return(EIGHTMEGS);
	break;
    }
    return(FOURMEGS);  /* default */
}

int
memsizing(void) 
{
    unsigned long testval, readval, *membase;
    int totalmem;
    int temp;
    SIG_PF sigsav;
    quicc_internal_regs_t *qir_p = (quicc_internal_regs_t *)ADRSPC_QUICC_REGB;

    /* Size main memory: 4, 8, 16M */

    testval = TEST_PAT1;
    membase = (unsigned long *)ADRSPC_RAM;

    /*
    ** First check for 16m SIMM using 4m DRAM size, one bank (CS1)
    */

    qir_p->memc_gmr = QUICC_GMR_4M_VAL;      /* 4m pagesize */

    qir_p->memc_or1 = QUICC_OR1_16MEG_VAL;   /* Initial values */
    qir_p->memc_br1 = QUICC_BR1_VAL;
    qir_p->memc_br2 = 0;		     /* invalild */

    totalmem = probe_dram(SIXTEENMEGS);

    if(totalmem != SIXTEENMEGS) {
	/*
	** Next check for 8megs with 2 banks (CS1 and CS2) of 1m DRAMs
	*/
	qir_p->memc_gmr = QUICC_GMR_1M_VAL;     /* 1m pagesize */
	qir_p->memc_or1 = QUICC_OR1_4MEG_VAL;

	qir_p->memc_or2 = QUICC_OR2_VAL;        /* 4mb second bank */
	qir_p->memc_br2 = QUICC_BR2_VAL;        /* 4mb second bank */

	totalmem = probe_dram(EIGHTMEGS);
	if(totalmem != EIGHTMEGS) {
	    /*
	    ** Default to 4 megs
	    */
	    totalmem = FOURMEGS;
	    qir_p->memc_br2 = 0;		/* no second bank */
	}
    }
    
    /*
    **	enable odd parity on dram
    */
    qir_p->memc_br1 |= QUICC_BR_PAR_EN;
    qir_p->memc_br2 |= QUICC_BR_PAR_EN;
    qir_p->memc_gmr |= (QUICC_GMR_PBEE | QUICC_GMR_OPAR);

    return(totalmem);    /* successful completion */
}

/*
** The following routines are convenience tools provided for use
** by other diagnostics.
** The two sizing routines return the size of the memory or zero.
** The start routines return the starting address or 0xffffffff.
** Both routines will size the memory if it has not already been.
*/

long
sizemainmem()
{
    int retval;

    if(memsize) return(memsize);
    DIAGFLAG |= D_NESTED;
    memsize = memsizing();
    DIAGFLAG &= ~D_NESTED;
    return(memsize);
}

long
sizeshmem()
{
    /* there is no shared memory on this platform */
    return((long)0 );
}

long
mainmemstart()
{
    return(sizemainmem() == 0 ? (long)-1 : (long)ADRSPC_RAM);
}

long
shmemstart()
{
    /* there is no shared memory on this platform */
    return((long)-1);
}

long freememstart;

long
getfreememstart(void)
{
    if(!freememstart)
	freememstart = (long)(errlog_start+ERRLOG_SIZE);
    return(freememstart);
}


int
size_nvram(void)
{
    unsigned long testbuf, readval;
    char *nverrmsg = "... cannot size NVRAM\n";
    unsigned long *base_ptr = &(NVRAM_SAV)->testarea;
    unsigned long *baseplus8k_ptr = (ulong *)((long)&(NVRAM_SAV)->testarea + EIGHT_K);
    quicc_internal_regs_t *qir_p = (quicc_internal_regs_t *)ADRSPC_QUICC_REGB;

    qir_p->memc_or7 = QUICC_OR7_32K_VAL;  /* Initial value */
    qir_p->memc_br7 = QUICC_BR7_VAL;

    testbuf = TEST_PAT1;
    if(nvwrite((uchar *)&testbuf, (uchar *)base_ptr, sizeof(testbuf)) < 0) {
write_err:
	printf("Error writing NVRAM %s", nverrmsg);
size_err:
	/*
	** We could invalidate the NVRAM chip select here but than all
	** accesses to this area would cause a bus error.  I doubt we
	** want this behaviour.
	**
	** qir_p->memc_br7 = 0;
	*/
	return(0);
    }
    readval = *base_ptr;
    if(readval != testbuf) {
	printf("First location in NVRAM fails %s", nverrmsg);
	goto size_err;
    }
    readval = *baseplus8k_ptr;  /* save this for later */
    testbuf = TEST_PAT2;        /* new unique value */
    if(nvwrite((uchar *)&testbuf, (uchar *)base_ptr, sizeof(testbuf)) < 0) {
	goto write_err;
    }

#ifdef DEBUG
printf("size_nvram: base = %#x, *base = %#x, base+8k = %#x, "
       "*base+8k = %#x readval = %#x\n", base_ptr, *base_ptr,
       baseplus8k_ptr, *baseplus8k_ptr, readval);
#endif


    if(*base_ptr == testbuf && *baseplus8k_ptr == testbuf &&
       readval == TEST_PAT1) {	/* we have 8k of nvram */
	qir_p->memc_or7 = QUICC_OR7_8K_VAL;  /* set to 8k */
#ifdef FIXME_BUSERR_SSW
	/*
	** make sure we buserr at >8k both on read and write
	*/
	if(chkberr(baseplus8k_ptr, BW_8BITS, 0) != 3) {
	    printf("Bus error does not occur at top of NVRAM\n");
	}
#endif
	return(EIGHT_K);
    }
    return(THIRTYTWO_K);  /* we must have 32k */
}

/* 
** Test main memory.
**
** Returns :
**    0    : test complete
**    1    : failed
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
    tmemptr->start = (unsigned *)(MONITOR_FREE_MEM_BASE + ERRLOG_SIZE);
    tmemptr->length = msize - ((int)tmemptr->start - ADRSPC_RAM);
    tmemptr->flag = INDIAGS;
    tmemptr->passcount = 1;  /* do complete mem test once */

    if(testmem(tmemptr) < 0) return(1);
    return(0);
}


/*
** display the memory info for the user
*/

extern int netboot_memorysize;
extern int netflashbooted;
extern end;

int
meminfo(int argc, char *argv[])
{
    long msize = sizemainmem();

    printf("\nMain memory size: %d MB. NVRAM size: %d KB",
	   msize>>20, size_nvram()>>10);
    printf("\nAvailable main memory starts at %#x, size %#x\n", 
	   MONITOR_FREE_MEM_BASE, (msize + ADRSPC_RAM) - MONITOR_FREE_MEM_BASE);
    return(0);
}

/* end of module */
