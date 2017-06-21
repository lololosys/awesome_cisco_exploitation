/* $Id: parity.c,v 3.2 1996/01/03 03:09:04 clev Exp $
 * $Source: /vws/aam/cherf.111/ios/boot/src-68-c4000/parity.c,v $
 *------------------------------------------------------------------
 * parity.c - parity test routine
 *
 * April 1992, Rob Clevenger
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: parity.c,v $
 * Revision 3.2  1996/01/03  03:09:04  clev
 * CSCdi46211:  Memory sizing broke on 4000 platform
 *
 * Fix old monitor code so it doesn't get optimized out with newer
 * versions of the compiler (gcc.94q2.68k).
 *
 * Revision 3.1  1995/11/09  09:09:44  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  19:16:20  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 * Revision 1.1  1993/09/24 16:34:03  widmer
 * Rename boot obj directories to include CPU type and product family
 *
 * Revision 1.1  1992/04/15  02:11:51  clev
 * Initial checkin for XX
 *
 * Revision 1.1  1992/03/09  08:38:08  hampton
 * Initial conversion from RCS to CVS.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Generic parity test.
 * Works on both main memory and shared memory, figures out which.
 * Does a 32 X 14 phase test on the location - writing and reading with bad parity.
 * Location is checked for longword boundary and valid memory space.
 * Location is passed as argument to routine.
 * Returns: 0 for invalid argument, -1 for failure, and 1 for success.
 * Subtests 0-6 write location with bad parity.
 * Subtests 7-13 read location with bad parity.
 * Register allocation and instruction sequence is critical for portions of this routine -
 * see comments imbedded in code.
 */
#include "../defs.h"
#include "../reason.h"
#include "../extern.h"
#include "../m68vectors.h"

#define NUM_PMEMS  2    /* main memory and shared memory */
#define NUM_PTESTS 14    /* 14 basic tests to be performed */

#define MAIN_MEM   0    /* flag which doubles as index into following array */
#define SHARED_MEM 1    /* flag which doubles as index into following array */
/* array of expected status values for each of the subtests */
uchar pstat_vals[NUM_PMEMS][NUM_PTESTS] = {
    0x0f, 0x0c, 0x03, 0x08, 0x04, 0x02, 0x01,    /* status register values for main memory write test */
    0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f,    /* status register values for main memory read test */
    0x1f, 0x1c, 0x13, 0x18, 0x14, 0x12, 0x11,    /* status register values for shared memory write test */
    0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f,    /* status register values for shared memory read test */
};

uchar statreg_snapshot, sh_statreg_snapshot;
void lev6phandler(), parberr();
extern void pbuserror(), plev6hand();
int lev6count;
extern ulong end;
extern volatile short buserrcnt;
ulong sav_val;
char memidx, phase;
uchar *pstatvalptr;
long sav_bvect, sav_l6vect;
extern int setcacr();
static int origcache;

parity_test(ulong location, int powerup)
{
    register ulong writeval, parodd;  /* must be in regs for main mem test */
    register char subtest;
    register volatile ulong *ctlreg;
    ulong readval;

    if(location == 0) location = memsize - 4;

    if(!powerup) printf("\nParity Test on location %#x ",location);

    if(location & 3) {
	printf("\nTest location (%#x) must be on longword boundary...aborting\n",
	       location);
	return(0);
    }
    if(location > ADRSPC_RAM && location < (ADRSPC_RAM + memsize)) {
	if(!powerup) puts("in main memory\n");
	memidx = MAIN_MEM;                       /* location is in main memory */
	ctlreg = (ulong *)ADRSPC_CONTROL;        /* get appropriate control register */
	parodd = CONTROL_PARITY_GEN;             /* bit to make parity odd */
	*ctlreg |= CONTROL_ENABLE_BERR;
    } else if(location >= ADRSPC_SHAREDMEM && location < (ADRSPC_SHAREDMEM + shmemsize)) {
	if(!powerup) puts("in shared memory\n");
	memidx = SHARED_MEM;                     /* location is in shared mem */
	ctlreg = (ulong *)ADRSPC_SHMEM_CONTROL;  /* get appropriate control register */
	parodd = SHMEM_PARITY_ODD;               /* bit to make parity odd */
	*ctlreg |= SHMEM_PERR_CLEAR;             /* make sure parity is enabled in hardware */
	sil(5);                             /* make sure we don't mask level 6 interrupts */  
    } else {
	printf("\n%#x is not a valid location for parity testing...aborting\n",location);
	return(0);
    }
    origcache = setcacr(CLEAR_CACHES);
    *(volatile ulong *)ADRSPC_CONTROL |= CONTROL_ENABLE_BERR;  /* bus errors enabled */
    writeval = 0x00000000;  /* writeval for first test phase */
    *(volatile ulong *)location = writeval;  /* initialize the location */
    /* 33 phase test - even and odd write values - zeros and ones in all bits */
    for(phase=0; phase<33; phase++, writeval = ((writeval << 1) +1)) {
	if(!powerup) printf("\rphase %d  ", phase);
	/* point to first pstatval for memory we are testing */
	pstatvalptr = &pstat_vals[memidx][0];
	/* do the specified range of tests */
	for(subtest=0; subtest < NUM_PTESTS; subtest++, pstatvalptr++) {
	    sav_val = *(volatile ulong *)location;  /* save original value at test location */
	    if( subtest < (NUM_PTESTS/2) ) {
		if(memidx == MAIN_MEM && !powerup) {
		    /* disable timer (stack access could cause later bus error) */
		    t_disable();
		    refresh();  /* because we are interrupting timer service */
		}
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
		*ctlreg &= ~parodd;     /* make parity even for this write */
		switch(subtest) {       /* "specific" write the location */
		case 0: *(volatile ulong *)location = writeval; break;
		case 1: *(volatile ushort *)location = writeval; break;
		case 2: *(volatile ushort *)(location+2) = writeval; break;
		case 3: *(volatile uchar *)location = writeval; break;
		case 4: *(volatile uchar *)(location+1) = writeval; break;
		case 5: *(volatile uchar *)(location+2) = writeval; break;
		case 6: *(volatile uchar *)(location+3) = writeval; break;
		}
		*ctlreg |= parodd;  /* make parity odd again */
		if(memidx == MAIN_MEM && !powerup) t_enable();  /* re-enable timer */
	    } else *(volatile ulong *)location = writeval;
	    buserrcnt = 0;
	    statreg_snapshot = 0;
	    lev6count = 0;
	    sh_statreg_snapshot = 0;
	    sav_l6vect = *EVEC_LEVEL6;  /* save original value */
	    *EVEC_LEVEL6 = (long)plev6hand;  /* set up for special handling of level 6 int */
	    readval = 0;  /* done to make compiler happy */
	    sav_bvect = *EVEC_BUSERR;
	    *EVEC_BUSERR = (long)pbuserror;  /* set up to handle possible bus error */
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
	    if( subtest >= (NUM_PTESTS/2) ) {  /* for the "read" parity error tests */
		if(memidx == MAIN_MEM) {
		    if(!powerup) {
			/* disable timer and bus errors (stack access will berr) */
			t_disable();
			refresh();  /* because we are interrupting timer service */
		    }
		    *(volatile ulong *)ADRSPC_CONTROL &= ~CONTROL_ENABLE_BERR;
		}
		*ctlreg &= ~parodd;     /* make parity even for this read */
	    }
	    switch(subtest) {    /* "specific" read the location to generate parity error */
	    case 0: case 7: readval = *(volatile ulong *)location; break;
	    case 1: case 8: (ushort)readval = *(volatile ushort *)location; break;
	    case 2: case 9: (ushort)readval = *(volatile ushort *)(location+2); break;
	    case 3: case 10: (uchar)readval = *(volatile uchar *)location; break;
	    case 4: case 11: (uchar)readval = *(volatile uchar *)(location+1); break;
	    case 5: case 12: (uchar)readval = *(volatile uchar *)(location+2); break;
	    case 6: case 13: (uchar)readval = *(volatile uchar *)(location+3); break;
	    }
	    asm("nop"); asm("nop");
	    *ctlreg |= parodd;  /* make sure parity is odd */

	    *EVEC_BUSERR = sav_bvect;  /* reset to report bus errors */
	    /* make sure bus errors are enabled */
	    *(volatile ulong *)ADRSPC_CONTROL |= CONTROL_ENABLE_BERR;
	    *EVEC_LEVEL6 = sav_l6vect;  /* restore original value */
	    /* rewrite location with original value and correct parity */
	    *(volatile ulong *)location = sav_val;
#ifdef XDEBUG
printf("location %#x, phase %d, subtest %d, test: %s\n  written %#x, read %#x\n",
		   location, phase, subtest, subtest < (NUM_PTESTS/2) ? "write" : "read",
		   writeval, readval);
#endif

	    if(memidx == MAIN_MEM) {
		if( subtest >= (NUM_PTESTS/2) ) {
		    parberr();  /* do this in place of actual berr */
		    if(!powerup) t_enable();  /* re-enable timer which was disabled above */
		}
		if(buserrcnt != 1) {
		    puts("bus error not generated on main memory parity error");
#ifdef DEBUG
		    printf(",\nbuserrcnt: %d",buserrcnt);
#endif
abort:
printf("\nlocation %#x, phase %d, subtest %d, test: %s\n  written %#x, read %#x ... aborting\n",
		   location, phase, subtest, subtest < (NUM_PTESTS/2) ? "write" : "read",
		   writeval, readval);
		    setcacr(origcache);
		    return(-1);
		}
		if(statreg_snapshot != *pstatvalptr) {
		    printf("bad status on main memory parity error, expected: %#x, is: %#x",
			   *pstatvalptr, statreg_snapshot);
		    goto abort;
		}
		if(lev6count) {
		    puts("level 6 interrupt generated on main memory parity error");
		    goto abort;
		}
	    } else {
		if(lev6count != 1) {
		    puts("level 6 interrupt not generated on shared memory parity error");
#ifdef DEBUG
		    printf(",\nlev6count: %d", lev6count);
#endif
		    goto abort;
		}
		if(sh_statreg_snapshot != *pstatvalptr) {
		    printf("bad status on shared memory parity error, expected: %#x, is: %#x",
			   *pstatvalptr, sh_statreg_snapshot);
		    goto abort;
		}
		if(buserrcnt) {
		    puts("bus error generated on shared memory parity error");
		    goto abort;
		}
	    }
	}
    }
    if(!powerup) putchar('\n');
    setcacr(origcache);
    return(1);  /* successful completion */
}

void parberr()
{
    register volatile ulong *ctlreg = (volatile ulong *)ADRSPC_CONTROL;
    register ulong status;

    status = *ctlreg;
    statreg_snapshot = ((status & CONTROL_PERR_MASK) >> 28);
    if(status & CONTROL_PERR_MASK) {  /* parity error reported; clear it */
	status &= ~CONTROL_PERR_MASK;  /* wipe out these bits */
	*ctlreg = status & ~CONTROL_PARITY_CLEAR;
	*ctlreg = status | CONTROL_PARITY_CLEAR;
    }
    buserrcnt++;
}

void lev6phandler()
{
    register volatile ulong *ctlreg = (ulong *)ADRSPC_SHMEM_CONTROL;
    register ulong status;

    status = *ctlreg;
    sh_statreg_snapshot = ((status & SHMEM_PARITY_MASK) >> 24);
    if(status & SHMEM_PARITY_MASK) {  /* parity error reported; clear it */
	status &= ~SHMEM_PARITY_MASK;  /* wipe out these bits */
	*ctlreg = status & ~SHMEM_PERR_CLEAR;
	*ctlreg = status | SHMEM_PERR_CLEAR;
    }
    lev6count++;
}
