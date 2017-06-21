/* $Id: parity.c,v 3.2.58.1 1996/03/21 23:29:13 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/rommon/src-36-c1000/parity.c,v $
 *------------------------------------------------------------------
 * parity.c
 *
 * January 1995, Rob Clevenger
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Main memory parity test routines for Sapphire
 *------------------------------------------------------------------
 * $Log: parity.c,v $
 * Revision 3.2.58.1  1996/03/21  23:29:13  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.2.2.1  1995/12/01  04:22:10  mbeesley
 * Branch: ELC_branch
 * Reorganize Rom monitor header files to make it easier to port
 * to new platforms.
 *
 * Revision 3.2  1995/11/17  18:44:13  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:07:30  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:39:28  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/*
 * Generic parity test.
 * Does a 32 X 14 phase test on the location - writing and reading with bad parity.
 * Location is checked for longword boundary and valid memory space.
 * Location is passed as argument to routine.
 * Returns: 0 for invalid argument, -1 for failure, and 1 for success.
 * Register allocation and instruction sequence is critical for portions of
 * this routine - see comments embedded in code.
 */
#include "monitor.h"
#include "error.h"
#include "signal_4k.h"
#include "confreg.h"
#include "mon_defs.h"
#include "queryflags.h"
#include "quicc.h"

#include "../../../boot/m68vectors.h"

#define FOURMEGS    0x0400000

#define NUM_PTESTS 7

extern unsigned short mstat_snapshot;
extern unsigned long end;
extern long memsize,shmemsize;
extern unsigned long *shmemaddr;
extern int optind;

unsigned long sav_val;
char memidx, phase;
static unsigned long gmr_save;
static int cleanup(void);
static SIG_PF sav_intfcn;

int
syspartest (void)
{
    unsigned long location, totalmem;  /* memory location */
    unsigned long msize, end;
    int temp, size;


    if ((msize = sizemainmem()) == 0) {
	cterr('f',0,"unable to size memory for testing");
	return(1);
    }

    location = MONITOR_FREE_MEM_BASE + ERRLOG_SIZE;   /* first RAM location in main memory */
    end = ADRSPC_RAM + msize;
    size = msize - ERRLOG_SIZE;

    size >>= 6;
    if (size == 0) {
	cterr('f',0,"invalid memory size");
	return(1);
    }
    for(; location < end; location += size) {
	temp = test_parity(location, 1);
	if(temp != 0) return(temp);
    }
    prcomplete(testpass, errcount, 0);
    return(0);
}

int
test_parity( unsigned long loc, int indiag)
{
    register unsigned long writeval;  /* must be in regs for main mem test */
    register char subtest;
    register quicc_internal_regs_t *qir_p = (quicc_internal_regs_t *)ADRSPC_QUICC_REGB;
    unsigned long readval;
    volatile unsigned char * location;
	unsigned short per_bank;
    SIG_PF savfcn;

    location = (unsigned char *)loc;

    testname("main memory parity");

    if((long)location & 3) {
	cterr('f',0,"Test location (%#x) must be on longword boundary...aborting",
	       location);
	return(1);
    }

    gmr_save = qir_p->memc_gmr;
    sav_intfcn = signal(SIGINT,(SIG_PF)cleanup);
    qir_p->memc_gmr |= (QUICC_GMR_PBEE | QUICC_GMR_OPAR); /* berr on odd parity error */
    qir_p->memc_br1 |= QUICC_BR_PAR_EN;		/* enable parity bank 1	*/
    qir_p->memc_br2 |= QUICC_BR_PAR_EN;		/* enable parity bank 2	*/

    if(indiag) prpass(testpass, "test location %#x, ", location);

    timer_disable();  /* we cannot handle timer ticks here!!! */
    writeval = 0x00000000;  /* writeval for first test phase */
    *(unsigned long *)location = writeval;  /* initialize the location */
    /* 33 phase test - even and odd write values - zeros and ones in all bits */
    for(phase=0; phase<33; phase++, writeval = ((writeval << 1) +1)) {
	/* do the specified range of tests */
	for(subtest=0; subtest < NUM_PTESTS; subtest++) {
	    sav_val = *(unsigned long *)location;  /* save original value at test location */

	    /*
	     * Special note:
	     * When this test is run on main memory the only access to main memory
	     * must be the specific write below.
	     * Any other reads or writes to main memory (this includes the stack)
	     * will cause this routine to break.
	     * For this reason some of the subroutines variables are external to
	     * ensure enough registers for "important" variables.
	     * The critical area is between the two parity commands to the
	     * control register.
	     */
	    qir_p->memc_gmr &= ~QUICC_GMR_OPAR;	/* make parity even for this write */

	    switch(subtest) {       /* "specific" write the location */
	    case 0: *(unsigned long *)location = writeval; break;
	    case 1: *(unsigned short *)location = writeval; break;
	    case 2: *(unsigned short *)(location+2) = writeval; break;
	    case 3: *(unsigned char *)location = writeval; break;
	    case 4: *(unsigned char *)(location+1) = writeval; break;
	    case 5: *(unsigned char *)(location+2) = writeval; break;
	    case 6: *(unsigned char *)(location+3) = writeval; break;
	    }
	    
	    qir_p->memc_gmr |= QUICC_GMR_OPAR;	/* make parity odd again */
	    
	    mstat_snapshot = 0;                 /* initialize this */
	    readval = 0;                        /* done to make compiler happy */
	    savfcn = signal(SIGBUS,SIG_IGN);    /* set to ignore bus errors */
	    hkeepflags &= ~H_BUSERR;            /* initialize flag */
	    
	    /*
	     * Special note:
	     * When this test is run on main memory the only access to main memory
	     * must be the specific read below.
	     * Any other reads or writes to main memory (this includes the stack)
	     * will cause this routine to break.
	     * For this reason some of the subroutines variables are external to
	     * ensure enough registers for "important" variables.
	     * The critical area is between the two parity commands to the
	     * control register.
	     */

	    switch(subtest) {    /* "specific" read the location to generate parity error */
	    case 0:  readval = *(volatile unsigned long *)location; break;
	    case 1:  (unsigned short)readval = *(volatile unsigned short *)location; break;
	    case 2:  (unsigned short)readval = *(volatile unsigned short *)(location+2); break;
	    case 3:  (unsigned char)readval = *(volatile unsigned char *)location; break;
	    case 4:  (unsigned char)readval = *(volatile unsigned char *)(location+1); break;
	    case 5:  (unsigned char)readval = *(volatile unsigned char *)(location+2); break;
	    case 6:  (unsigned char)readval = *(volatile unsigned char *)(location+3); break;
	    }
	    asm("nop"); asm("nop");

	    qir_p->memc_gmr |= QUICC_GMR_OPAR;	/* make sure parity is odd */

	    signal(SIGBUS,savfcn);  /* restore back to original condition */

	    /* rewrite location with original value and correct parity */
	    *(volatile unsigned long *)location = sav_val;
#ifdef DEBUG
	    printf("mstat %#x, hkeepflags %#x\n", mstat_snapshot, hkeepflags);
#endif
	    if(!(hkeepflags & H_BUSERR)) {
		cterr('w',0,"bus error not generated on main memory parity error");
abort:
		cterr('f',0,"location %#x, phase %d, subtest %d, test: %s\n  written %#x,  read %#x ... aborting\n",
		      location, phase, subtest, subtest < (NUM_PTESTS/2) ? "write" : "read",
		      writeval, readval);
		cleanup();
		return(1);
	    }
	    if((mstat_snapshot & QUICC_MSTAT_PER_MASK) == 0) {
		cterr('w',0,"no parity error bits set in MSTAT register");
		goto abort;
	    }

	    /*
	     * check for the correct parity error bank
	     *
	     * the 8 meg simm uses CS2 (bank 3) for its
	     * second 4 megs.
	     *
	     */

	    if ((loc >= (ADRSPC_RAM + FOURMEGS)) && (qir_p->memc_br2 & QUICC_BR_VALID))
		per_bank = QUICC_MSTAT_PER3;
	    else
		per_bank = QUICC_MSTAT_PER2;
	    
	    if ((per_bank & mstat_snapshot) == 0) {
		cterr('w',0,"wrong parity error bit set in MSTAT register");
		goto abort;
	    }
	}
    }
    cleanup();
    return(0);  /* successful completion */
}


static int cleanup(void)
{
    quicc_internal_regs_t *qir_p = (quicc_internal_regs_t *)ADRSPC_QUICC_REGB;

    qir_p->memc_gmr = gmr_save;
    timer_enable();
    signal(SIGINT,sav_intfcn);
    return((int)SIG_DFL);
}

/* end of module */
