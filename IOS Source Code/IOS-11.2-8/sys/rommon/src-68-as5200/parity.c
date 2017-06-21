/* $Id: parity.c,v 1.1.68.1 1996/06/16 21:18:58 vnguyen Exp $
 * $Source: /release/112/cvs/Xsys/rommon/src-68-as5200/parity.c,v $
 *------------------------------------------------------------------
 * parity.c -- Main memory parity test routines for Brasil.
 *
 * October 1995, Joe Turner
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: parity.c,v $
 * Revision 1.1.68.1  1996/06/16  21:18:58  vnguyen
 * Finally committing Brasil to California_branch
 *
 * Revision 1.1  1995/12/03  21:18:43  jturner
 * Placeholder for Brasil.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "setjmp.h"
#include "nvmonvars.h"
#include "error.h"
#include "monitor.h"
#include "signal_4k.h"
#include "pcmap.h"
#include "as5200.h"
#include "m68vectors.h"
#include "confreg.h"
#include "mon_defs.h"
#include "queryflags.h"

extern unsigned short mstat_snapshot;
extern unsigned long end;
extern long memsize,shmemsize;
extern unsigned long *shmemaddr;
extern int optind;

unsigned long sav_val;
char memidx, phase;
static unsigned long gmr_save;
volatile ulong parity_test_loc; /* target memory location of parity tests */
volatile ushort scr; /* SCR1 */
static int tmp_berr(void);
static int cleanup(void);
static SIG_PF sav_intfcn;

/*
 * Generic parity test.
 * Location is passed as argument to routine.
 * Returns: -1 for failure, and 1 for success.
 * Register allocation and instruction sequence is critical for portions of
 * this routine - see comments embedded in code.
 */
/*
 * int test_parity(unsigned long loc, int indiag)
 */
int
test_parity ( unsigned long loc, int indiag )
{
    register ulong dummy;   /* *MUST* be a register variable */
    SIG_PF savfcn;
 
    parity_test_loc = loc;
 
    testname("main memory parity");
    /*
     * Disable timers
     * Make sure cache is off...
     */ 
    if(indiag) prpass(testpass, "test location %#x, ", parity_test_loc);
    timer_disable();
    setcacr(0); /* Disable caches */
    setcacr(CLEAR_CACHES); /* Clear caches */

    scr = 0;
    savfcn = signal(SIGBUS,(SIG_PF)tmp_berr);  /* set to dummy handler */
    hkeepflags &= ~H_BUSERR;            /* initialize flag */

    /*
     * enable EVEN parity
     */
    *((ushort *)ADRSPC_CONTROL) |= (CANCUN_EVEN_PARITY);
 
    /*
     * This access to real memory should generate a parity error.
     */
    parity_test_loc = 0;
 
    /*
     * Now restore normal (ODD) parity checking and read location.
     * This should cause a parity error.
     */
    *((ushort *)ADRSPC_CONTROL) &= ~(CANCUN_EVEN_PARITY);
    *((ushort *)ADRSPC_CONTROL) |= CANCUN_PARITY_ENABLE;/* enable parity */
    dummy = parity_test_loc;

    /*
     * Restore berr handler back to original condition
     */
    signal(SIGBUS,savfcn);

    /*
     * Check results of accessing memory
     */
    scr &= (CONTROL_PARITY0 | CONTROL_PARITY1 | CONTROL_PARITY_ERROR);
    if (scr != (CONTROL_PARITY0 | CONTROL_PARITY1 | CONTROL_PARITY_ERROR)) {
        printf("Parity test failed, SCR read 0x%x", scr);
        printf("(masked), expected 0x%x\r\n", (CONTROL_PARITY0 |
		CONTROL_PARITY1 | CONTROL_PARITY_ERROR));
        cleanup();
	return(-1);
    }
    cleanup();
    puts("Parity test passed.\r\n");
    return(1); /* Test passed! */
}

static int cleanup(void)
{
    timer_enable();
    setcacr(CLEAR_CACHES); /* Clear the caches */
    setcacr(ENABLE_CACHES); /* Enable both caches */
    signal(SIGINT,sav_intfcn);
    return((int)SIG_DFL);
}

static int tmp_berr(void)
{
    /*
     * Indicate that we have a bus error
     * and correct parity at that addr
     */
    parity_test_loc = 1;

    /*
     * Get status register and hold in cpu register.
     * This also clears int parity error.
     */
    scr = *((ushort *)ADRSPC_CONTROL);
}

int
parityset(int argc, char *argv[])
{

    unsigned short confreg;
    int i, shift;
    long temp;
    char buffer[16];

    if(argc > 2) {
usage:
	printf("usage: %s <even|odd>\n");
	return(1);
    }
    if(argc == 2) {  /* value specified on command line */
	i = getnum(argv[1], 16, &temp);
	if(i == 0) {
	    printf("illegal argument\n");
	    goto usage;
	}
	confreg = temp;
    } else {
	while(1) {
	    puts("\nParityset utility:");
	    puts("\nWARNING: Power-on default is parity disabled.");
	    if(getc_answer("\nDo you wish to enable main memory parity? y/n",
			   "yn", 'n')  == 'n')
		break;
	    if(getc_answer("Enable even parity?", "yn", 'n') == 'y') {
		*((unsigned short *)ADRSPC_CONTROL) |= CANCUN_PARITY_ENABLE;
		puts("Even parity enabled.\n");
		break;
	    }
	    if(getc_answer("Enable odd parity?", "yn", 'n') == 'y') {
		*((ushort *)ADRSPC_CONTROL) &= ~(CANCUN_EVEN_PARITY);
		*((ushort *)ADRSPC_CONTROL) |= CANCUN_PARITY_ENABLE;
		puts("Odd parity enabled.\n");
		break;
	    }
	    puts("No parity enabled.\n");
	    break;
	}
    }
    puts("\n");
    return(0);
}
