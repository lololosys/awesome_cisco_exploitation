/* $Id: sizemem.c,v 3.5.4.1 1996/03/21 23:32:31 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/rommon/src-4k-rsp/sizemem.c,v $
 *------------------------------------------------------------------
 * sizemem.c 
 *
 * Mar 1995, Steve J. Zhang
 *
 * Copyright (c) 1995 - 1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Size main/shared memory for RSP
 *------------------------------------------------------------------
 * $Log: sizemem.c,v $
 * Revision 3.5.4.1  1996/03/21  23:32:31  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.2.2.3  1996/03/19  01:01:41  mbeesley
 * Branch: ELC_branch
 * Sync to V111_1_3
 *
 * Revision 3.2.2.2  1996/02/08  08:59:09  gstovall
 * Branch: ELC_branch
 * Sync to ELC_baseline_960205.
 *
 * Revision 3.2.2.1  1995/12/01  04:24:23  mbeesley
 * Branch: ELC_branch
 * Reorganize Rom monitor header files to make it easier to port
 * to new platforms.
 *
 * Revision 3.5  1996/03/11  06:00:33  tkam
 * CSCdi51218:  Need to implement fault history mechanism
 * Added fault history support and misc. cleanup.
 *
 * Revision 3.4  1996/02/01  02:27:24  mansonw
 * CSCdi47569:  HSA slave refuses to switch to master state
 * Fixed the HSA slave to gain master during hot insertion/removal
 *
 * Revision 3.3  1996/01/17  23:31:51  mansonw
 * CSCdi47058:  Fix the broken ROMMON build
 *
 * Revision 3.2  1995/11/17  18:47:00  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:09:08  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:41:23  hampton
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
#include "error.h"
#include "testmem.h"
#include "mon_defs.h"
#include "cpu_mips.h"

#include "dclasic.h"
#include "memdasic.h"

#define MMOFFSET  0x80000  /* half meg */
#define SHMOFFSET 0x2000   /* first 0x2000 are for QA */

#define S_128KB     0x20000  /* 128 K bytes */
#define S_HALFMEG   0x80000  /* half meg */
#define S_ONEMEG    0x100000
#define S_TWOMEG    0x200000
#define S_FOURMEG   0x400000
#define S_EIGHTMEG  0x800000
#define S_16MEG     0x1000000
#define S_32MEG     0x2000000
#define S_64MEG     0x4000000
#define S_128MEG    0x8000000
#define S_256MEG    0x10000000

#define DRAM_INDEX  0
#define MEMD_INDEX  1
#define NVRAM_INDEX 2

#define UORG_TEST_DATA     0xcafebabe
#define LORG_TEST_DATA     0xfeedbeef
#define UNEW_TEST_DATA     0xdeadface
#define LNEW_TEST_DATA     0xabeeafee

#define NO_MEM_ERROR       0
#define MEM_ERROR          1

long memsize;             /* size of main memory */
long shmemsize;           /* size of shared memory */

static long nvsize;

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
    (unsigned long *)(KSEG1_ADRSPC_MEMD+SHMOFFSET), 
    &shmemsize,

    "nvram",
    (unsigned long *)(KSEG1_ADRSPC_NVRAM),
    &nvsize
};


/*
 * function prototype 
 */

long sizing_bank1(unsigned long *bank1_size);
long sizing_bank2(unsigned long bank1_size, unsigned long *bank2_size);
long sizing_memd();
long sizing_nvram();
long sizemainmem(void);
long sizeshmem(void);
long size_nvram(void);


int memsizing(int testindex)
{
    long tmp;
    
    if (testindex == 1)
	tmp = sizemainmem();
    else if (testindex == 2)
	tmp = sizeshmem();
    else {
	printf("Test Index Invalid %d\n", testindex);
	return(1);
    }

    if (tmp == 0) {
	printf("Memory can't be sized\n");
	return(1);
    }
    else 
	return(0);
} /* memsizing */

/*
 * Name: long sizing_bank1(bank1_size)
 * Desc: return the memory size in bank1 using aliasing technique
 */
long
sizing_bank1(unsigned long *bank1_size)
{
    struct meminfo *meminfoptr;
    unsigned long *memoffset, sreg, utestval, ltestval, ureadval, lreadval;
    int mem_status;

    mem_status = NO_MEM_ERROR;
    meminfoptr   = &(memtype[DRAM_INDEX]);  /* point to the info array */
    memoffset    = meminfoptr->memoffset;
#ifdef DEBUG
    printf("Sizing the DRAM bank 1 ... \n");
#endif
    utestval = UORG_TEST_DATA;
    ltestval = LORG_TEST_DATA;

    /*
     * check whether there is any memory at all
     */
    DCL_ASIC->bank_config = BK1_64M;   /* initialize bank to 64M */
    DCL_ASIC->dram_config &= ~nBanks;  /* initialize to 1 bank only */
    *memoffset = utestval;             /* write upper word to the first location */
    *(memoffset + 1) = ltestval;       /* write lower word to the first location */
    ureadval = *(volatile unsigned long *)memoffset; /* read the first location  */
    lreadval = *(volatile unsigned long *)(memoffset + 1);

    if (ureadval != utestval || lreadval != ltestval){
	DCL_ASIC->bank_config = 0;
	printf("Warning: There is no memory in the system !\n");
	*bank1_size = 0;                     /* zero memory is found       */
	return (NO_MEM_ERROR);
    }

    /* 
     * check the first bank for 64M
     */
    *(unsigned long *)(S_32MEG + (unsigned long)memoffset) = UNEW_TEST_DATA;
    *(unsigned long *)(S_32MEG + (unsigned long)memoffset + 4) = LNEW_TEST_DATA;
    ureadval = *(volatile unsigned long *)(S_32MEG + (unsigned long)memoffset);
    lreadval = *(volatile unsigned long *)(S_32MEG + (unsigned long)memoffset + 4);
    if (ureadval == UNEW_TEST_DATA && lreadval == LNEW_TEST_DATA) {
	*bank1_size = S_64MEG;
	return (NO_MEM_ERROR);
    } else if (ureadval == UNEW_TEST_DATA) {
	printf("Warning: Slot 2 in Bank 1 has less memory than Slot 1 above 32M\n");
	mem_status = MEM_ERROR;
    } else if (lreadval == LNEW_TEST_DATA) {
	printf("Warning: Slot 1 in Bank 1 has less memory than Slot 2 above 32M\n");
	mem_status = MEM_ERROR;
    }
	

    /*
     * check the first bank for 8M
     */
    DCL_ASIC->bank_config = BK1_32M;
    *(unsigned long *)(S_EIGHTMEG + (unsigned long)memoffset) = UNEW_TEST_DATA;
    *(unsigned long *)(S_EIGHTMEG + (unsigned long)memoffset + 4) = LNEW_TEST_DATA;
    ureadval = *(volatile unsigned long *)memoffset;
    lreadval = *(volatile unsigned long *)(memoffset + 1);
    if (ureadval == UNEW_TEST_DATA || lreadval == LNEW_TEST_DATA) {
	DCL_ASIC->bank_config = BK1_8M;
	*bank1_size = S_EIGHTMEG;
	if (ureadval != UNEW_TEST_DATA) {
	    printf("Warning: Slot 2 in Bank 1 has less memory than Slot 1 under 32M\n");
	    return(MEM_ERROR);
	}
	else if (lreadval != LNEW_TEST_DATA) {
	    printf("Warning: Slot 1 in Bank 1 has less memory than Slot 2 under 32M\n");
	    return(MEM_ERROR);
	}
	else 
	    return(mem_status);
    } /* if */


    /* 
     * check the first bank for 16M
     */
    *(unsigned long *)(S_16MEG + (unsigned long)memoffset) = UNEW_TEST_DATA;
    *(unsigned long *)(S_16MEG + (unsigned long)memoffset + 4) = LNEW_TEST_DATA;
    ureadval = *(volatile unsigned long *)memoffset;
    lreadval = *(volatile unsigned long *)(memoffset + 1);
    if (ureadval == UNEW_TEST_DATA || lreadval == LNEW_TEST_DATA) {
	DCL_ASIC->bank_config = BK1_16M;
	*bank1_size = S_16MEG;
	if (ureadval != UNEW_TEST_DATA) {
	    printf("Warning: Slot 2 in Bank 1 has less memory than Slot 1\n");
	    return(MEM_ERROR);
	}
	else if (lreadval != LNEW_TEST_DATA) {
	    printf("Warning: Slot 1 in Bank 1 has less memory than Slot 2\n");
	    return(MEM_ERROR);
	}
	else
	    return(mem_status);
    }
 /* if */
    
    *bank1_size = S_32MEG;
    return(mem_status);

} /* sizing_bank1 */



/*
 * Name: long sizing_bank2(bank1_size, bank2_size)
 * Desc: return the memory size in bank2 using aliasing technique
 */
long
sizing_bank2(unsigned long bank1_size, unsigned long *bank2_size)
{
    struct meminfo *meminfoptr;
    unsigned long *memoffset, sreg, utestval, ltestval, ureadval, lreadval;
    int mem_status;

    mem_status = NO_MEM_ERROR;
    meminfoptr   = &(memtype[DRAM_INDEX]);  /* point to the info array */
    memoffset    = meminfoptr->memoffset;
#ifdef DEBUG
    printf("Sizing the DRAM bank 2 ... \n");
#endif
    /*
     * check whether there is any memory at all
     */
    DCL_ASIC->bank_config |= BK2_64M;   /* initialize bank 2 to 64M */
    DCL_ASIC->dram_config |= nBanks;    /* initialize to 2 banks */

    /* Write to one location outside the bank1 and read back */
    *(unsigned long *)(bank1_size + (unsigned long)memoffset) = UORG_TEST_DATA;
    *(unsigned long *)(bank1_size + (unsigned long)memoffset + 4) = LORG_TEST_DATA;
    ureadval = *(volatile unsigned long *)(bank1_size + (unsigned long)memoffset); 
    lreadval = *(volatile unsigned long *)(bank1_size + (unsigned long)memoffset + 4); 

    /*
     * Can't read the value back or wrap arond happen.
     */
    if (ureadval != UORG_TEST_DATA || lreadval != LORG_TEST_DATA) {
	DCL_ASIC->bank_config &= 0xF0;  /* mask out the last nibble */
	DCL_ASIC->dram_config &= ~nBanks;  /* set back to one bank */
#ifdef DEBUG
	printf("Warning: There is no memory in bank 2 !\n");
#endif
	*bank2_size = 0;
	return(NO_MEM_ERROR);                     /* zero memory is found       */
    }


    /* 
     * check the second bank for 64M
     */
    if (bank1_size == S_64MEG) {
	*(unsigned long *)(bank1_size + S_32MEG + (unsigned long)memoffset) = UNEW_TEST_DATA;
	*(unsigned long *)(bank1_size + S_32MEG + (unsigned long)memoffset + 4) = LNEW_TEST_DATA;
	ureadval = *(volatile unsigned long *)(bank1_size + S_32MEG + (unsigned long)memoffset);
	lreadval = *(volatile unsigned long *)(bank1_size + S_32MEG + (unsigned long)memoffset + 4);
	if (ureadval == UNEW_TEST_DATA && lreadval == LNEW_TEST_DATA) {
	    *bank2_size = S_64MEG;
	    return (NO_MEM_ERROR);
	} else if (ureadval == UNEW_TEST_DATA) {
	    printf("Warning: Slot 2 in Bank 1 has less memory than Slot 1 above 32M\n");
	    mem_status = MEM_ERROR;
	} else if (lreadval == LNEW_TEST_DATA) {
	    printf("Warning: Slot 1 in Bank 1 has less memory than Slot 2 above 32M\n");
	    mem_status = MEM_ERROR;
	}
    }

    /*
     * check the second bank for 8M
     */
    DCL_ASIC->bank_config = (DCL_ASIC->bank_config & 0xF0) | BK2_32M;

    /*
     * If there is memory and bank1 size = 8MEG, bank2 can only has 8MEG
     * since bank2 <= bank1.
     */
    if (bank1_size == S_EIGHTMEG) {
	DCL_ASIC->bank_config = (DCL_ASIC->bank_config & 0xF0) | BK2_8M;
	*bank2_size = S_EIGHTMEG;
	return (NO_MEM_ERROR);
    } else {
	*(unsigned long *)(bank1_size + S_EIGHTMEG + (unsigned long)memoffset) = UNEW_TEST_DATA;
	*(unsigned long *)(bank1_size + S_EIGHTMEG + (unsigned long)memoffset + 4) = LNEW_TEST_DATA;
	ureadval = *(volatile unsigned long *)(bank1_size + (unsigned long)memoffset);
	lreadval = *(volatile unsigned long *)(bank1_size + (unsigned long)memoffset + 4);
	if (ureadval == UNEW_TEST_DATA || lreadval == LNEW_TEST_DATA) {  /* check for wrap around */
	    DCL_ASIC->bank_config = (DCL_ASIC->bank_config & 0xF0) | BK2_8M;
	    *bank2_size = S_EIGHTMEG;
	    if (ureadval != UNEW_TEST_DATA) {
		printf("Warning: Slot 2 in Bank 2 has less memory than Slot 1 under 32M\n");
		return(MEM_ERROR);
	    }
	    else if (lreadval != LNEW_TEST_DATA) {
		printf("Warning: Slot 1 in Bank 2 has less memory than Slot 2 under 32M\n");
		return(MEM_ERROR);
	    }
	    else
		return(mem_status);
	}
    }


    /* 
     * check the second bank for 16M
     */
    if (bank1_size == S_16MEG) {
	DCL_ASIC->bank_config = (DCL_ASIC->bank_config & 0xF0) | BK2_16M;
	*bank2_size = S_16MEG;
	return (NO_MEM_ERROR);
    } else {
	*(unsigned long *)(bank1_size + S_16MEG + (unsigned long)memoffset) = UNEW_TEST_DATA;
	*(unsigned long *)(bank1_size + S_16MEG + (unsigned long)memoffset + 4) = LNEW_TEST_DATA;
	ureadval = *(volatile unsigned long *)(bank1_size + (unsigned long)memoffset);
	lreadval = *(volatile unsigned long *)(bank1_size + (unsigned long)memoffset + 4);
	if (ureadval == UNEW_TEST_DATA || lreadval == LNEW_TEST_DATA) {
	    DCL_ASIC->bank_config = (DCL_ASIC->bank_config & 0xF0) |  BK2_16M;
	    *bank2_size = S_16MEG;
	    if (ureadval != UNEW_TEST_DATA) { 
		printf("Warning: Slot 2 in Bank 2 has less memory than Slot 1 under 32M\n");
		return(MEM_ERROR);
	    }
	    else if (lreadval != LNEW_TEST_DATA) {
		printf("Warning: Slot 1 in Bank 2 has less memory than Slot 2 under 32M\n");
		return(MEM_ERROR);
	    }
	    else
		return(mem_status);
	}
    }
   
    *bank2_size = S_32MEG;
    return(mem_status);

} /* sizing_bank2 */


/*
 * Name: long sizing_memd()
 */
long
sizing_memd()
{
    struct meminfo *meminfoptr;
    unsigned long *memoffset, testval, readval;

    meminfoptr = &(memtype[MEMD_INDEX]);  /* point to the info array */
    memoffset  = meminfoptr->memoffset;

    MEMD_ASIC->memdconfig_reg |= MEMD2M;      /* initialize to 2 MB */
   
    testval = 0xcafebabe; 
    
    *memoffset = testval;                 /* write the first location */
    readval = *memoffset;                 /* read the first location */
    if (readval != testval) {
	printf("There is no SRAM in MEMD\n");
	return (0);
    }

    *(unsigned long *)(S_HALFMEG + (unsigned long)memoffset) = 0xfeedface;
    readval = *memoffset;                  /* wraparound happen ? */

    if (readval == 0xfeedface) {
	MEMD_ASIC->memdconfig_reg &= ~MEMD2M;    /* set to 512k byte */
	return (S_HALFMEG);
    }
    else
	return (S_TWOMEG);

} /* sizing_memd */

/*
 * Name: long sizing_nvram()
 */
long sizing_nvram()
{
    struct meminfo *meminfoptr;
    unsigned long *memoffset, testval, readval, sreg;
    SIG_PF sigsav, sigint;
    unsigned long sav_val;      /* used to save the first value in NVRAM */
    long nvram_size_t;         
    
    meminfoptr = &(memtype[NVRAM_INDEX]);
    memoffset  = meminfoptr->memoffset;

    sav_val = *memoffset;

    testval = 0xcafebabe;

    sigsav = signal(SIGBUS,SIG_IGN);   /* set to ignore bus errors */
    sigint = signal(SIGINT,SIG_IGN);   /* ignore user interrupts   */
    hkeepflags &= ~H_BUSERR;           /* initialize flag          */
    sreg = getcp0_sreg();	       /* disable parity exception */
    setcp0_sreg(sreg | SREG_DE);

    *memoffset = testval;              /* write the first location */
    readval = *(volatile unsigned long *)memoffset;   /* read the first location  */

    if (readval != testval || hkeepflags & H_BUSERR) {
	printf("Warning: NVRAM size is 0 \n");
	setcp0_sreg(sreg);             /* restore status register */
	signal(SIGBUS,sigsav);         /* set back to original value */
	signal(SIGINT,sigint);	       /* restore user interrupts    */
	return(0);                     /* zero memory is found */
    }

    readval = *(volatile unsigned long *)(S_128KB + (unsigned long)memoffset);

    if (hkeepflags & H_BUSERR)
	nvram_size_t = S_128KB;
    else
	nvram_size_t = S_128KB * 4;
    
    *memoffset = sav_val;
    
    /*
     * Return signal to original value, print out
     * and return success
     */
    setcp0_sreg(sreg);		/* restore status register */
    signal(SIGINT,sigint);	/* restore user interrupt  */
    signal(SIGBUS,sigsav);	/* restore bus errors      */

    return (nvram_size_t);

} /* sizing_nvram */



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

    unsigned long sreg;
    unsigned long bank1_size, bank2_size;
    int bank1_status, bank2_status; 

    if(memsize) return(memsize);

    /* 
     * sizing the memory and initialize those memory to zero
     */
    sreg = getcp0_sreg();               /* disable parity exception */
    setcp0_sreg(sreg | SREG_DE);

    if (sizing_bank1(&bank1_size) == MEM_ERROR) {
	printf("Warning: There are errors in sizing bank1\n");
	printf("         Memory size is not accurate\n");
	memsize = bank1_size;
    } else if (sizing_bank2(bank1_size, &bank2_size) == MEM_ERROR) {
	printf("Warning: There are errors in sizing bank2\n");
	printf("         Memory size is not accurate\n");
	memsize = bank1_size;
    } else {
	memsize = bank1_size + bank2_size;
    }
#define KSEG0_MON_FREE_MEM (ADRSPC_K0BASE + 0x6000)
    if (memsize != 0) {
	bzero(KSEG0_MON_FREE_MEM, memsize - (KSEG0_MON_FREE_MEM - ADRSPC_K0BASE));
	cache_flush(0, -1);
    }

    setcp0_sreg(sreg);                 /* restore status register */

     return (memsize);

}


long
sizeshmem(void)
{
    unsigned long sreg;

    if(shmemsize) return(shmemsize);

    sreg = getcp0_sreg();              /* disable parity exception */
    setcp0_sreg(sreg | SREG_DE);

#ifdef RSP2
    DCL_ASIC->asic_reg |= BOB_MODE;
/* remap range registers */
    MEMD_ASIC->qa_status_range_reg = 0xf0;
    MEMD_ASIC->qa_range_reg = 0xf0;
    MEMD_ASIC->BH_range_reg = 0xf0;
    MEMD_ASIC->buff_range_reg = 0x20;
#endif

    shmemsize = sizing_memd();

    setcp0_sreg(sreg);                 /* restor status register */

    if (shmemsize != 0) {
	bzero((unsigned char *)0xa8002000, shmemsize - 0x2000);
    }
    return(shmemsize);
}


long
mainmemstart(void)
{
    return(sizemainmem() == 0 ? (long)-1 : KSEG1_ADRSPC_RAM);
}

long
shmemstart(void)
{
    return(sizeshmem() == 0 ? (long)-1 : KSEG1_ADRSPC_MEMD);
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
	"shared", (unsigned *)KSEG1_ADRSPC_MEMD, (unsigned *)KSEG1_ADRSPC_MEMD, &shmemsize,
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
** RSP can have either 128k or 512k of NVRAM.
** Return size in bytes to user.  Size by checking for valid
** memory above 128k.  Memory will alias if not 512k.
** The first long in NVRAM is dedicated to this test function.
*/

long
size_nvram(void)
{
    unsigned long temp;

    if(!nvsize) {
	nvsize = sizing_nvram();
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

    printf("\nMain memory size: 0x%x\n", memsiz);
    printf("\nAvailable main memory starts at %#x, size %#x\n",
	   availmem, (mainmemstart() + memsiz) - availmem);

    printf("\nPacket memory starts at %#x\n", shmemstart());

    printf("NVRAM size: %#x\n", size_nvram());


    return(0);

}

/* End of file */















