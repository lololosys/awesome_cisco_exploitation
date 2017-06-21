/* $Id: sizemem.c,v 3.4.10.1 1996/03/21 23:31:03 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/rommon/src-4k-c4000/sizemem.c,v $
 *------------------------------------------------------------------
 * sizemem.c
 *
 * July 1993, Michael Beesley
 *
 * Copyright (c) 1994 - 1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Size main/shared memory for Sierra.
 *------------------------------------------------------------------
 * $Log: sizemem.c,v $
 * Revision 3.4.10.1  1996/03/21  23:31:03  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.2.2.2  1996/03/19  01:01:31  mbeesley
 * Branch: ELC_branch
 * Sync to V111_1_3
 *
 * Revision 3.2.2.1  1995/12/01  04:23:30  mbeesley
 * Branch: ELC_branch
 * Reorganize Rom monitor header files to make it easier to port
 * to new platforms.
 *
 * Revision 3.4  1996/02/27  05:41:50  clev
 * CSCdi49964: 32m memory option with mem bank jumper misconfigured causes
 * crash
 *
 * Add code to further qualify memory configuration.
 *
 * Revision 3.3  1996/02/11  02:17:19  clev
 * CSCdi44945:  Support new memory options
 *
 * Add code to support the 64mb dual bank SIMM.
 *
 * Revision 3.2  1995/11/17  18:45:49  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:08:26  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:40:35  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/*
 * Ported from the sizemem routine for XX in DiagMon
 *
 * Size main/shared memory and initialize proper control register.
 * Sierra only supports 8, 32, and 128 megs of ram.
 * Sierra only supports 1, 4 and 16 megs of shared memory.
 * 
 * Note, during the sizing algorithm, a read is always done after
 * a write to ensure that the write gets completed, and that any
 * interrupts due to the write get processed without a delay
 *
 * Returns :
 *   0     : successful completion
 *   1     : error
 */

#include "monitor.h"
#include "signal_4k.h"
#include "nvmonvars.h"
#include "error.h"
#include "testmem.h"
#include "mon_defs.h"
#include "cpu_mips.h"

#define MMOFFSET  0x80000  /* half meg */
#define SHMOFFSET 0x0

#define S_ONEMEG    0x100000
#define S_FOURMEG   0x400000
#define S_EIGHTMEG  0x800000
#define S_16MEG     0x1000000
#define S_32MEG     0x2000000
#define S_64MEG     0x4000000
#define S_128MEG    0x8000000

long memsize;             /* size of main memory */
long shmemsize;           /* size of shared memory */

struct meminfo {
    char  *memstr;
    unsigned long *memoffset;
    long  *memsizptr;          /* point to memory size global variable */
};

struct meminfo memtype[] = {
    "main",
    (unsigned long *)(KSEG1_ADRSPC_RAM+MMOFFSET), 
    &memsize,
    
    "shared",
    (unsigned long *)(KSEG1_ADRSPC_SHAREDMEM+SHMOFFSET), 
    &shmemsize 
};

int 
memsizing(int testindex) 
    /* 1: main memory sizing,  2: shared memory sizing */
{
    struct meminfo *meminfoptr;
    volatile unsigned long *memoffset;
    unsigned long testval, readval, sreg;
    int totalmem;
    long memorysize;
    SIG_PF sigsav, sigint;

    meminfoptr   = &(memtype[testindex-1]);  /* point to the info array */
    memoffset    = meminfoptr->memoffset;
    testname("size %s memory",meminfoptr->memstr);

    if (testindex == 2) {
	reset_io();             /* unreset the NIMs for shmem sizing */
	totalmem = 16;          /* 16 Meg */
	memorysize = S_16MEG;
	KSEG1_IO_ASIC->shmem_ctl &= ~(SHMEM_CONFIG0 | SHMEM_CONFIG1);
	flush_io_wb();
    } else {
	totalmem = 128;         /* 128 Meg */
	memorysize = S_128MEG;
	KSEG1_IO_ASIC->sys_ctl &= ~CONTROL_MEM_MASK;
	flush_io_wb();
    }
    testval = 0xdeadbeef;
    sigsav = signal(SIGBUS,SIG_IGN);   /* set to ignore bus errors */
    sigint = signal(SIGINT,SIG_IGN);   /* ignore user interrupts   */
    hkeepflags &= ~H_BUSERR;           /* initialize flag          */
    sreg = getcp0_sreg();	       /* disable parity exception */
    setcp0_sreg(sreg | SREG_DE);

    *memoffset = testval;              /* write the first location */
    readval = *memoffset;              /* read the first location  */

    if (readval != testval || hkeepflags & H_BUSERR) {
	cterr('f',0,"No %s memory found.",meminfoptr->memstr);
sizerr:
	setcp0_sreg(sreg);	       /* restore status register    */
	signal(SIGBUS,sigsav);         /* set back to original value */
	signal(SIGINT,sigint);	       /* restore user interrupts    */
	return(1);                     /* zero memory is found       */
    }

    if (testindex == 2) {

	/*
	 * Shared memory is 1, 4 or 16 Meg
	 * New for rev >0 boards:
	 * 8m shared memory size
	 * 1m no longer valid
	 */
	*memoffset = testval;
	*(unsigned long *)(S_ONEMEG + (long)memoffset) = 0xfeedface;
	readval = *memoffset;
	if (readval != testval || hkeepflags & H_BUSERR) {
	    if((KSEG1_IO_ASIC->sys_stat0 & SS0_HWREV_BITS) > 0) {
		totalmem = 0;
		memorysize = 0;
		printf("1mb of shared memory is not supported on this platform\n");
		goto common;
	    }
	    KSEG1_IO_ASIC->shmem_ctl |= (SHMEM_1MEG << SHMEM_SIZ_SHIFT);
	    flush_io_wb();
	    totalmem     = 1;
	    memorysize   = S_ONEMEG;
	    goto common;
   	}

	/*
	 * Four Meg
	 */
	*(unsigned long *)(S_FOURMEG + (long)memoffset) = 0xcafebabe;
	readval = *memoffset;
	if (readval != testval || hkeepflags & H_BUSERR) {
size4meg:
	    KSEG1_IO_ASIC->shmem_ctl |= (SHMEM_4MEG << SHMEM_SIZ_SHIFT);
	    flush_io_wb();
	    totalmem = 4;
	    memorysize = S_FOURMEG;
	    goto common;
	}

	/*
	** Check for 8m, if hardware rev 0 use 4 of it, else use all
	** 8 of it.
	** Note that this magic pattern must generate the same parity
	** for each byte as the original pattern with some SIMMs otherwise
	** a parity error will result.
	*/
	*(unsigned long *)(S_EIGHTMEG + (long)memoffset) = 0xbeefface;
	readval = *memoffset;
	if (readval != testval || hkeepflags & H_BUSERR) {
	    if((KSEG1_IO_ASIC->sys_stat0 & SS0_HWREV_BITS) > 0) {
		KSEG1_IO_ASIC->shmem_ctl |= (SHMEM_8MEG << SHMEM_SIZ_SHIFT);
		flush_io_wb();
		totalmem = 8;
		memorysize = S_EIGHTMEG;
		goto common;
	    } else {  /* rev 0 Hardware */
		/*
		** This should never happen but handle
		** it in case it does.
		*/
		goto size4meg;
	    }
	}
	
	/*
	 * Leave it at 16 Meg
	 */
    } else {

	/*
	 * Main DRAM is 8, 32, 128 Meg.
	 * New for rev 1 hardware, a 16m option.
	 */
	*memoffset = testval;
	*(volatile unsigned long *)(S_EIGHTMEG + (long)memoffset) = 0xfeedface;
	readval = *memoffset;

	if (readval != testval || hkeepflags & H_BUSERR ||
	    *(volatile unsigned long *)(S_EIGHTMEG + (long)memoffset) != 0xfeedface) {
size8meg:
	    KSEG1_IO_ASIC->sys_ctl |= (CONTROL_8MEG_MEM << CONTROL_MEM_SHIFT);
	    flush_io_wb();
	    totalmem     = 8;
	    memorysize   = S_EIGHTMEG;
	    goto common;
   	}

	/*
	** Check for 16m, if hardware rev 0 use 8 of it, else use all
	** 16 of it
	*/
	*(volatile unsigned long *)(S_16MEG + (long)memoffset) = 0xbeefface;
	readval = *memoffset;
	if (readval != testval || hkeepflags & H_BUSERR ||
	    *(volatile unsigned long *)(S_16MEG + (long)memoffset) != 0xbeefface) {
	    if((KSEG1_IO_ASIC->sys_stat0 & SS0_HWREV_BITS) > 0) {
		KSEG1_IO_ASIC->sys_ctl |= (CONTROL_16MEG_MEM << CONTROL_MEM_SHIFT);
		flush_io_wb();
		totalmem = 16;
		memorysize = S_16MEG;
		goto common;
	    } else {  /* rev 0 Hardware */
		/* 
		** It would be nice to detect this case and explain to the user
		** that rev 0 does not support 8m double sided SIMMs, but we never
		** end up here due to rev 0 hardware design constraints.
		*/
		goto size8meg;
	    }
	}
	    
	/*
	 * Thirty two Meg
	 */
	*(volatile unsigned long *)(S_32MEG + (long)memoffset) = 0xcafebabe;
	readval = *memoffset;
	if (readval != testval || hkeepflags & H_BUSERR ||
	    *(volatile unsigned long *)(S_32MEG + (long)memoffset) != 0xcafebabe) {
mm32mb:
	    KSEG1_IO_ASIC->sys_ctl |= (CONTROL_32MEG_MEM << CONTROL_MEM_SHIFT);
	    flush_io_wb();
	    totalmem = 32;
	    memorysize = S_32MEG;
	    goto common;
	}
	
	/*
	 * New for rev 4 hardware
	 * Sixty Four Meg
	 */
	*(volatile unsigned long *)(S_64MEG + (long)memoffset) = 0xdeadface;
	readval = *memoffset;
	if (readval != testval || hkeepflags & H_BUSERR ||
	    *(volatile unsigned long *)(S_64MEG + (long)memoffset) != 0xdeadface) {
	    KSEG1_IO_ASIC->sys_ctl |= (CONTROL_64MEG_MEM << CONTROL_MEM_SHIFT);
	    flush_io_wb();
	    totalmem = 64;
	    memorysize = S_64MEG;
	    goto common;
	}
	/*
	 * Leave it at 128 Meg
	 * We can check to make sure that the bank select jumper is
	 * correctly positioned by testing if memory exists just above 32m.
	 */
	if(*(volatile unsigned long *)(S_32MEG + (long)memoffset) != 0xcafebabe) {
	    cterr('w',0,"Detected 128MB of memory but missing banks\n"
		  "The bank select jumper may be misconfigured\n"
		  "Reconfiguring for 32MB\n");
	    goto mm32mb;
	}
    }

common:
    if(hkeepflags & H_BUSERR) {
	hkeepflags &= ~H_BUSERR;  /* unset the bit */
	cterr('f',0,"unexpected bus error during sizing, aborting");
	goto sizerr;
    }
    *meminfoptr->memsizptr = memorysize;

    /*
     * Now that the memory has been sized and the control register has been
     * written with the size, test that a bus error occurs when we access above
     * the memory.  Note that this is a read bus error test only.  Do not do a
     * write bus error test with chkberr() as this will cause other memory spaces
     * to be read on Sierra.
     * Do not expect a bus error above 128m of main memory.  This is address
     * A8000000 which is the beginning of shared memory.
     */
    if(memorysize && (memorysize != S_128MEG)) {
	if(chkberr((volatile unsigned long *)(memorysize+(long)memoffset),
		   BW_32BITS, 1) != 1) {
	    cterr('w',0,"expected bus error does not occur above %d mb of %s memory"
		  ,totalmem,meminfoptr->memstr);
	    goto sizerr;
	}
    }

    /*
     * Return signal to original value, print out a message
     * and return success
     */
    setcp0_sreg(sreg);		/* restore status register */
    signal(SIGINT,sigint);	/* restore user interrupt  */
    signal(SIGBUS,sigsav);	/* restore bus errors      */

    prcomplete(testpass, errcount, "found %d mb", totalmem);
    return(0);
}

/*
** The following routines are convenience tools provided for use
** by other diagnostics.
** The two sizing routines return the size of the memory or zero.
** The start routines return the starting address or 0xffffffff.
** Both routines will size the memory if it has not already been.
*/

long
sizemainmem(void)
{
    int retval;

    if(memsize) return(memsize);
    (NVRAM)->diagflag |= D_NESTED;
    retval = memsizing(1);
    (NVRAM)->diagflag &= ~D_NESTED;
    if(retval == 0) {
	bzero(MON_FREE_MEM, memsize - (MON_FREE_MEM - KSEG1_ADRSPC_RAM));
	return(memsize);
    }
    return((long)0);
}

long
sizeshmem(void)
{
    int retval;

    if(shmemsize) return(shmemsize);
    (NVRAM)->diagflag |= D_NESTED;
    retval = memsizing(2);
    (NVRAM)->diagflag &= ~D_NESTED;
    if(retval == 0) {
	bzero(KSEG1_ADRSPC_SHAREDMEM, shmemsize);
	return(shmemsize);
    }
    return((long)0);
}

long
mainmemstart(void)
{
    return(sizemainmem() == 0 ? (long)-1 : KSEG1_ADRSPC_RAM);
}

long
shmemstart(void)
{
    return(sizeshmem() == 0 ? (long)-1 : KSEG1_ADRSPC_SHAREDMEM);
}

int
ismainmemaddr(long addr)
{
    long phymemstart;
    long phyaddr;

    if(sizemainmem()) {
	phymemstart = PHY_ADRSPC_RAM;
	phyaddr = PHY_ADDR(addr);
	if(phyaddr >= phymemstart &&
	   phyaddr < (phymemstart + memsize)) return(1);
    }
    return(0);
}


/* 
** Test either main or shared memory.
**
** Returns :
**    0    : test complete
**    1    : failed
*/

sysmemtest(int testindex)
    /* 1: main memory  2: shared */
{
    struct meminfo {
	char  *memstr;
	unsigned *memaddr;
	unsigned *startadd;
	long *memsizeptr;
    } memtype[] = {
	"main"  , (unsigned *)KSEG1_ADRSPC_RAM, (unsigned *)MON_FREE_MEM, &memsize,
	"shared", (unsigned *)KSEG1_ADRSPC_SHAREDMEM, (unsigned *)KSEG1_ADRSPC_SHAREDMEM, &shmemsize,
    };
    struct meminfo *meminfoptr;
    struct testmem tmem;
    register struct testmem *tmemptr=&tmem;
    int tmp=0;

    meminfoptr = &memtype[testindex-1];
    if (*meminfoptr->memsizeptr == 0)    /* see if memory has been sized already */
	tmp = memsizing(testindex);      /* perform memory sizing test */

    testname("%s memory",meminfoptr->memstr);
    if (tmp) {
	cterr('w',0,"cannot size %s memory - aborting test",
	      meminfoptr->memstr);
        return(1);
    } else tmemptr->length = *meminfoptr->memsizeptr -
	((unsigned)meminfoptr->startadd - (unsigned)meminfoptr->memaddr);

    tmemptr->start = meminfoptr->startadd;  
    tmemptr->flag = INDIAGS;
    tmemptr->passcount = 1;  /* do complete mem test once */

#ifdef DEBUG
printf("memtest params are: start %#x, size %#x\n",tmemptr->start,tmemptr->length);
printf("meminfo struct: memaddr %#x, startadd %#x, memsize %#x\n", 
       meminfoptr->memaddr, meminfoptr->startadd, *meminfoptr->memsizeptr);
#endif

    if(testmem(tmemptr) < 0) return(1);

    testname("%s memory refresh",meminfoptr->memstr);
    if(memrefresh(tmemptr) < 0) return(1);

    return(0);
}

long freememstart;

long
getfreememstart(void)
{
    if(!freememstart)
	freememstart = (long)(errlog_start+ERRLOG_SIZE);
    return(freememstart);
}

/*
** Sierra can have either 128k or 512k of NVRAM.
** Return size in bytes to user.  Size by checking for valid
** memory above 128k.  Memory will alias if not 512k.
** The first long in NVRAM is dedicated to this test function.
*/
static long nvsize;

long
size_nvram(void)
{
    volatile long *ptr1, *ptr2;
    long data1, data2, datasav, junk;

    if(!nvsize) {
	ptr1 = (long *)KSEG1_ADRSPC_NVRAM;
	ptr2 = (long *)(KSEG1_ADRSPC_NVRAM + (128 * 1024));
	datasav = *ptr1;      /* read and save first space in NVRAM */
	data1 = 0x5555aaaa;  /* unique patterns */
	data2 = 0xaaaa5555;
	*ptr1 = data1;                    /* write it */
	junk = *(ptr1 + 1);               /* junk read to clear bus */
	if(*ptr1 == data1) {              /* verify */
	    if(*ptr2 == data1) {          /* first indication of 128k */
		*ptr1 = data2;            /* different pattern */
		junk = *(ptr1 + 1);        /* junk read to clear bus */
		if(*ptr1 == data2) {      /* verify */
		    if(*ptr2 == data2) {  /* second indication of 128k */
			nvsize = (128 * 1024);
		    } else nvsize = (512 * 1024);
		}
	    } else nvsize = (512 * 1024);
	}
	*ptr1 = datasav;                   /* restore original data */
    }
    return(nvsize);
}

/*
** Print out some memory information
*/
int
meminfo(int argc, char *argv[])
{
    long memsiz, availmem;

    memsiz =  sizemainmem();

    availmem = getfreememstart();

    printf("\nMain memory size: %d MB. Packet memory size: %d MB", 
	   memsiz>>20, sizeshmem()>>20);
    printf("\nAvailable main memory starts at %#x, size %#x",
	   availmem, (mainmemstart() + memsiz) - availmem);
    printf("\nPacket memory starts at %#x\n", shmemstart());
    printf("NVRAM size: %#x\n", size_nvram());
    return(0);
}

/* End of file */
