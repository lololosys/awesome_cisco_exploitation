/* $Id: svip_sizemem.c,v 1.1.6.1 1996/07/08 22:43:11 mwu Exp $
 * $Source: /release/112/cvs/Xsys/rommon/src-4k-svip/svip_sizemem.c,v $
 *------------------------------------------------------------------
 * svip_sizemem.c 
 *
 * Mar, 96. Paul Greenfield
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: svip_sizemem.c,v $
 * Revision 1.1.6.1  1996/07/08  22:43:11  mwu
 * CSCdi60922:  11.2 Core Features Commit
 * Branch: California_branch
 *
 * Revision 1.1  1996/03/23  01:23:05  pgreenfi
 * Placeholder files for src-4k-svip.
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

typedef unsigned long long u64;

#include "monitor.h"
#include "../../src-svip/svip_pma.h"
#include "../../src-svip/svip_asic.h"
#include "signal_4k.h"
#include "error.h"
#include "testmem.h"
#include "mon_defs.h"
#include "cpu_mips.h"

#define DIM(a)   (sizeof((a))/sizeof((a[0])))

#define S_128KB     0x20000  /* 128 K bytes */
#define S_256KB     0x40000  /* 256 K bytes */
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
#define SRAM_INDEX  1
#define NVRAM_INDEX 2

#define ORG_TEST_DATA     0xcafebabe
#define NEW_TEST_DATA     0xdeadface

#define MEM_GOOD           0
#define NO_MEM_ERROR       1
#define MEM_ERROR          2

long memsize;             /* size of main memory */
long srammemsize;         /* size of sram */  
long shmemsize;           /* size of shared memory */

static long nvsize;

struct meminfo {
    char  *memstr;
    unsigned long *memoffset;
    long  *memsizptr;          /* point to memory size global variable */
};

struct meminfo memtype[] = {
    "main",
    (unsigned long *)(KSEG1_ADRSPC_RAM), 
    &memsize,

    "SRAM",
    (unsigned long *)(KSEG1_ADRSPC_PCIMEM), 
    &srammemsize,
    
    "nvram",
    (unsigned long *)(KSEG1_ADRSPC_PROM),
    &nvsize
};

/* base for dram sizing */
#define BASE_ADDR	KSEG1_ADRSPC_RAM

struct sramtestdata {
    long    banksize;           /* value to put in PMA config register */
    long    sramsize;           /* value to put in PMA config register */
    long    ctrl1_sramsize;     /* value to put in control register 1 */
    long    size;               /* memory size (also used as offset) */
};

static struct sramtestdata sramtest[] = {
    CR1L_SRAM_BANK_256K, 0, CR1_SRAM_SZ_256KB, S_256KB,
    CR1L_SRAM_BANK_512K, CR1L_SRAM_SIZE_512K, CR1_SRAM_SZ_512KB, S_HALFMEG,
    CR1L_SRAM_BANK_1M, CR1L_SRAM_SIZE_1M, CR1_SRAM_SZ_1MB, S_ONEMEG,
    CR1L_SRAM_BANK_2M, CR1L_SRAM_SIZE_2M, CR1_SRAM_SZ_2MB, S_TWOMEG,
    CR1L_SRAM_BANK_4M, CR1L_SRAM_SIZE_4M, CR1_SRAM_SZ_4MB, S_FOURMEG,
    CR1L_SRAM_BANK_8M, CR1L_SRAM_SIZE_8M, CR1_SRAM_SZ_8MB, S_EIGHTMEG,
    CR1L_SRAM_BANK_16M, CR1L_SRAM_SIZE_16M, CR1_SRAM_SZ_16MB, S_16MEG
};

/* this table is organized by the DRAM size bits in CR1 register */
static dramsize[] = {S_64MEG, S_32MEG, S_16MEG, S_EIGHTMEG};

/*
 * function prototype 
 */

long sizesram(void);
long sizing_memd();
long sizing_nvram();
long sizemainmem(void);
long sizeshmem(void);
long size_nvram(void);

/*
 * use move64 for aligned accesses
 */
extern inline void
move64 (u64 *src, u64 *dst)
{
    asm volatile(".set noreorder\n"
	".set noat\n"
	".set mips3\n"
	"ld $1, 0(%0)\n"
	"nop\n"
	"sd $1, 0(%1)\n"
	".set at\n"
	".set reorder\n"
	".set mips2"	: : "r" (src), "r" (dst) : "$1");
}


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
 * svip_setup_hardware_registers :
 */
void svip_setup_PMA_registers (void)

{
    u64  temp64;

#if 0
    /* reset entire chip before anything else */
    temp64 = CR1L_RESET_PMA;
    move64(&temp64, (u64 *)&KSEG1_PMA_ASIC->config_reg1);
#endif

    KSEG1_PMA_ASIC->pci_status_cmd = PMA_PCI_STAT_CMD_DEFAULT;

    /* Clear the pma asic error register */
    temp64 = 0;
    move64(&temp64, (u64 *)&KSEG1_PMA_ASIC->err_reg); 

    /*
     * Initialize Memory sector for PCI target accesses
     */
    KSEG1_PMA_ASIC->pci_mem_loc = PHY_ADRSPC_PCIMEM; 	
    /*
     * Initialize I/O sector for PCI target accesses
     */
    KSEG1_PMA_ASIC->pci_io_loc = PHY_ADRSPC_PCI_IO;
    /* 
     * Initialize configuration register 1
     * Sets Sram size and bank size to 16M for memory sizing
     */
    temp64 = PMA_CR1U_DEFAULT;
    temp64 = (temp64 << 32 )| CR1L_SRAM_SIZE_16M | CR1L_SRAM_BANK_16M;
    move64(&temp64, (u64 *)&KSEG1_PMA_ASIC->config_reg1);

    /* doubleword read must be done before singleword read in order to 
       insure valid parity */
    move64((u64 *)&KSEG1_PMA_ASIC->err_reg, &temp64);
}

/*
 * Name: long sizing_memd()
 */
long
sizing_memd()
{
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


/*
 * Name: long sizemainmem()
 * Desc: DRAM sizing was actually done right after we got reset.  This is
 *       necessary because some value were stored in memory and if the
 *       sizing bits got changed the values were at a different address.
 *       In particular the value in monitor_stk got messed up.
 *
 *       So all we do here is get the size of memory from the configuration
 *       register and zero the DRAM.
 */
long
sizemainmem()
{
    unsigned long sreg;
    ushort cr1value;

    if(memsize) return(memsize);
    sreg = getcp0_sreg();              /* disable parity exception */
    setcp0_sreg(sreg | SREG_DE);
    cr1value = KSEG1_IO_ASIC->cr1;
    memsize = dramsize[(cr1value & CR1_DRAM_SZ_MASK) >> 10];
#define KSEG0_MON_FREE_MEM (ADRSPC_K0BASE + 0x6000)
    if (memsize != 0) {
/*	bzero(MON_FREE_MEM, memsize - (MON_FREE_MEM - KSEG1_ADRSPC_RAM)); */
        bzero(KSEG0_MON_FREE_MEM, memsize - (KSEG0_MON_FREE_MEM - ADRSPC_K0BASE));
        cache_flush(0, -1);
    }
    setcp0_sreg(sreg);                 /* restore status register */
    return(memsize);
}

/*
 * Name: long sizesram()
 * Desc: checks the memory size of both SRAM banks using aliasing technique
 */
long
sizesram()
{
    struct meminfo *meminfoptr;
    unsigned long membase, *memptr, *membaseptr, sreg, testval, readval;
    unsigned long bank1size, pmareg;
    ushort regvalue;
    int mem_status, bank1index, bank2index;
    u64  temp64;

    if(srammemsize) return(srammemsize);
    mem_status = MEM_GOOD;
    meminfoptr   = &(memtype[SRAM_INDEX]);  /* point to the info array */
    membaseptr    = meminfoptr->memoffset;
    membase = (unsigned long)membaseptr;
    testval = ORG_TEST_DATA;

    /* set control register 1 to 16MB */
    regvalue = KSEG1_IO_ASIC->cr1;
    regvalue &= ~CR1_SRAM_SZ_MASK;
    KSEG1_IO_ASIC->cr1 = regvalue | CR1_SRAM_SZ_16MB; 

    sreg = getcp0_sreg();              /* disable parity exception */
    setcp0_sreg(sreg | SREG_DE);

    /*
     * check whether there is any memory at all
     */
    *membaseptr = testval;              /* write word to the first location */
    readval = *(volatile unsigned long *)membaseptr; /* read the first location  */

    if (readval != testval){
	*meminfoptr->memsizptr = 0;     /* zero memory is found       */
        setcp0_sreg(sreg);              /* restore status register */
	return (0);
    }

    /* test all sizes of memory, we don't test for 16M because */
    /* if we have more than 8M it must be 16M */
    testval = NEW_TEST_DATA;
    for(bank1index = 0; bank1index < (DIM(sramtest)-1); bank1index++)
        {
        memptr = (unsigned long *)(membase + sramtest[bank1index].size);
        *memptr = testval;
        readval = *(volatile unsigned long *)membaseptr;
        if(readval == NEW_TEST_DATA)
            break;
        }
    /* set the PMA configuration register for bank 1 */
    move64((u64 *)&KSEG1_PMA_ASIC->config_reg1, &temp64);
    temp64 &= ~(CR1L_SRAM_BANK_SIZE_MASK);
    temp64 |= sramtest[bank1index].banksize;
    pmareg = sramtest[bank1index].banksize;
    move64(&temp64, (u64 *)&KSEG1_PMA_ASIC->config_reg1);
    bank1size = sramtest[bank1index].size;

    /* check if there is a bank 2 */
    bank2index = -1;
    if(bank1size != S_16MEG){
        membase += bank1size;
        membaseptr = (unsigned long *)membase;
        *membaseptr = ORG_TEST_DATA;      /* write word to the first location */
        readval = *(volatile unsigned long *)membaseptr; /* read the first location  */

        if (readval == ORG_TEST_DATA){
            /* have something in bank 2 so size it */
            testval = NEW_TEST_DATA;
            for(bank2index = 0; bank2index < (DIM(sramtest)-1); bank2index++)
                {
                memptr = (unsigned long *)(membase + sramtest[bank2index].size);
                *memptr = testval;
                readval = *(volatile unsigned long *)membaseptr;
                if(readval == NEW_TEST_DATA)
                    break;
                }
            /* bank 1 must be same size as bank 2 */
            if(bank1index != bank2index) {
                bank2index = -1;
                mem_status = MEM_ERROR;
            }
        }
    }
    /* if there is a bank 2 then total size is twice the */
    /* bank 1 size, else the same as bank 1 size */
    if(bank2index != -1)
        bank1index++;
    srammemsize = sramtest[bank1index].size;
    KSEG1_IO_ASIC->cr1 = regvalue | sramtest[bank1index].ctrl1_sramsize;
    temp64 &= ~(CR1L_SRAM_SIZE_MASK);
    temp64 |= sramtest[bank1index].sramsize;
    move64(&temp64, (u64 *)&KSEG1_PMA_ASIC->config_reg1);
    pmareg |= sramtest[bank1index].sramsize;
    /* clear the sram */
    bzero((char *)meminfoptr->memoffset, srammemsize);
    setcp0_sreg(sreg);                 /* restore status register */
    return(srammemsize);
}


long
sizeshmem(void)
{
    unsigned long sreg;

    if(shmemsize) return(shmemsize);

    sreg = getcp0_sreg();              /* disable parity exception */
    setcp0_sreg(sreg | SREG_DE);

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
