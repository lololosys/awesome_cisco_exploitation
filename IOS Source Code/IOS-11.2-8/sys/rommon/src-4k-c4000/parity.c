/* $Id: parity.c,v 3.2.58.1 1996/03/21 23:30:55 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/rommon/src-4k-c4000/parity.c,v $
 *------------------------------------------------------------------
 * parity.c
 *
 * May 1993, Michael Beesley
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Parity tests for Sierra Shared Memory.  Ported from the DiagMon
 * test for XX.
 *------------------------------------------------------------------
 * $Log: parity.c,v $
 * Revision 3.2.58.1  1996/03/21  23:30:55  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.2.2.1  1995/12/01  04:23:23  mbeesley
 * Branch: ELC_branch
 * Reorganize Rom monitor header files to make it easier to port
 * to new platforms.
 *
 * Revision 3.2  1995/11/17  18:45:43  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:08:21  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:40:31  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/*
 * Ported from the parity test for XX in DiagMon.
 * 
 * Generic parity test.
 * Works on shared memory only.
 * Does a 32 X 6 phase test on the location - 
 * writing and reading with bad parity.
 * Location is checked for longword boundary and valid memory space.
 * Location is passed as argument to routine.
 * Returns: 0 for invalid argument, -1 for failure, and 1 for success.
 * Subtests 0-2 write location with bad parity.
 * Subtests 3-5 read location with bad parity.
 */

#include "monitor.h"
#include "signal_4k.h"
#include "error.h"
#include "confreg.h"
#include "cpu_mips.h"

#define NUM_PTESTS 6    /* 6 basic tests to be performed */

/* 
 * array of expected status values for each of the subtests
 */
unsigned char pstat_vals[NUM_PTESTS] = {
    0xef,	/* stat reg sh mem rd, byte 0-3 error */
    0xe3,	/* stat reg sh mem rd, byte 0-1 error */
    0xec,	/* stat reg sh mem rd, byte 2-3 error */

    0xef,	/* stat reg sh mem wr, byte 0-3 error */
    0xef,	/* stat reg sh mem wr, byte 0-1 error */
    0xef	/* stat reg sh mem wr, byte 2-3 error */
};

/*
 * external variables
 */
extern volatile unsigned char sh_statreg_snapshot, in_parity_test;
extern volatile int lev6count;
extern long shmemsize;

/*
 * local variables
 */
unsigned long sav_val;
unsigned char *pstatvalptr;
char phase;

/*
 * test_parity:
 * Test a location for correct parity. indiag == 1 in diagnostic
 * mode, else it is 0
 */
int
test_parity(volatile unsigned long location, int indiag)
{
    unsigned long writeval;
    char subtest;
    unsigned long readval;
    SIG_PF savfcn;

    if((location >= KSEG1_ADRSPC_SHAREDMEM) && 
       (location < (KSEG1_ADRSPC_SHAREDMEM + shmemsize))) {	/* shared memory */
	testname("shared memory parity");
	KSEG1_IO_ASIC->shmem_ctl |= SHMEM_PERR_CLEAR;	/* enable parity */
    } else {
	cterr('f',0,"%#x is not in shared memory...aborting",
	      location);
	return(2);
    }

    if (location & 0x03) {
	cterr('f',0,"Test location (%#x) must be on long boundary...aborting",
	       location);
	return(1);
    }

    if (indiag) 
	prpass(testpass, "test location %#x, ", location);

    writeval = 0x00000000;  		   /* writeval for first test phase */
    *(unsigned long *)location = writeval; /* initialize the location */

    /* 
     * 33 phase test - even and odd write values,
     * zeros and ones in all bits
     */
    for(phase=0; phase<33; phase++, writeval = ((writeval << 1) +1)) {

	/* 
	 * point to first pstatval for memory we are testing
	 */
	pstatvalptr = &pstat_vals[0];

	/* 
	 * do the specified range of tests
	 */
	for(subtest=0; subtest < NUM_PTESTS; subtest++, pstatvalptr++) {
	    
	    /*
	     * save original value at test location. Level 6 interrupts
	     * are already enabled
	     */
	    sav_val = *(unsigned long *)location;
	    if (subtest < (NUM_PTESTS>>1)) {      
		
		/*
		 * Set even parity for this access
		 */
		KSEG1_IO_ASIC->shmem_ctl &= ~SHMEM_PARITY_ODD;
		flush_io_wb();

		switch(subtest) {       /* "specific" write the location */
		case 0: 
		    *(unsigned long *)location = writeval; 
		    break;
		case 1: 
		    *(unsigned short *)location = writeval; 
		    break;
		case 2: 
		    *(unsigned short *)(location+2) = writeval; 
		    break;
		}
		
		/*
		 * Return parity to odd
		 */
		KSEG1_IO_ASIC->shmem_ctl |= SHMEM_PARITY_ODD;
		flush_io_wb();

	    } else {
		*(unsigned long *)location = writeval;
	    }

	    sh_statreg_snapshot = 0;
	    lev6count = 0;		      /* init the count */
	    in_parity_test = 1;               /* yes, i am in parity test */
	    readval = 0;                      /* done to make compiler happy */

	    savfcn = signal(SIGBUS,SIG_IGN);  /* set to ignore bus errors */
	    hkeepflags &= ~H_BUSERR;          /* initialize flag */
	   
	    if (subtest >= (NUM_PTESTS>>1)) { /* for the "read" parity tests */
					      /* make parity even */
		KSEG1_IO_ASIC->shmem_ctl &= ~SHMEM_PARITY_ODD;
		flush_io_wb();
	    }
	    
	    /*
	     * "specific" read the location to generate parity error
	     */
	    switch(subtest) {
	    case 0: case 3: 
		readval = *(unsigned long *)location; 
		break;
	    case 1: case 4: 
		(unsigned short)readval = *(unsigned short *)location; 
		break;
	    case 2: case 5: 
		(unsigned short)readval = *(unsigned short *)(location+2); 
		break;
	    }

	    /*
	     * Set parity back to ODD
	     */
	    KSEG1_IO_ASIC->shmem_ctl |= SHMEM_PARITY_ODD;
	    flush_io_wb();
	    signal(SIGBUS,savfcn);

	    /* 
	     * rewrite location with original value and correct parity
	     */
	    *(unsigned long *)location = sav_val;
	    if(lev6count != 1) {
		cterr('w',0,"level 6 count on sh mem parity err wrong\n\
expected: 1, got: %d\n", lev6count);
		goto abort;
	    }

	    if(sh_statreg_snapshot != *pstatvalptr) {
		cterr('w',0,"bad status on shared memory parity error\n\
expected: %#x, got: %#x\n",
		      *pstatvalptr, sh_statreg_snapshot);
		goto abort;
	    }
	    if(hkeepflags & H_BUSERR) {
		cterr('w',0,"bus error generated on sh. memory parity error");
abort:
		cterr('f',0,"location %#x, phase %d, subtest %d, test: %s\n\
wrote %#x,  read %#x ... aborting\n",
		      location, phase, subtest, 
		      subtest < (NUM_PTESTS/2) ? "write" : "read",
		      writeval, readval);
	
		in_parity_test = 0;  /* getting out of parity test */
		return(1);
	    }
	}
    }
    return(0);  /* successful completion */
}

/*
 * syspartest:
 * Shared Memory Parity test
 */
int
syspartest(void)
{
    unsigned long location;  		/* memory location */
    unsigned long end;
    int temp, size;

    if (shmemsize == 0) {
	if(memsizing(2) != 0) {  	/* perform memory sizing test */
	    cterr('f',0,"unable to size memory for testing");
	    return(1);
	}
    }
    location = KSEG1_ADRSPC_SHAREDMEM;  	/* first location in shared memory */
    end = location + shmemsize;
    size = shmemsize;

    size >>= 6;
    for(; location < end; location += size) {
	temp = test_parity(location, 1);
	if (temp != 0) 
	    return(temp);
    }
    prcomplete(testpass,errcount, 0);
    return(0);
}



/* End of Module */
