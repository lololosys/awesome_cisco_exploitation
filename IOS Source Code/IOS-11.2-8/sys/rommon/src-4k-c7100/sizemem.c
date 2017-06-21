/* $Id: sizemem.c,v 3.1.68.3 1996/04/30 17:25:16 szhang Exp $
 * $Source: /release/112/cvs/Xsys/rommon/src-4k-c7100/sizemem.c,v $
 *------------------------------------------------------------------
 * sizemem.c - Predator Rom monitor memory sizing
 *
 * November 1995, Michael Beesley
 *
 * Copyright (c) 1996-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: sizemem.c,v $
 * Revision 3.1.68.3  1996/04/30  17:25:16  szhang
 * CSCdi55355:  fault tolerant boot strategy causes boot cycle
 * Branch: California_branch
 *
 * Revision 3.1.68.2  1996/04/04  17:06:02  szhang
 * CSCdi52979:  Bootloader hang after OIR HW change
 * Branch: California_branch
 *
 * Revision 3.1.68.1  1996/03/21  23:31:48  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.1.2.7  1996/02/21  18:28:22  szhang
 * CSCdi49458:  Problem with sizing 512K NVRAM
 * Branch: ELC_branch
 * NVRAM size bit in I/O control register should
 * be set to zero when sizing NVRAM.
 *
 * Revision 3.1.2.6  1996/02/12  19:12:42  szhang
 * Branch: ELC_branch
 * Move the GT64010 register initial values to pred_plat_defs.h.
 * Add the pcmap display into sizemem.c
 *
 * Revision 3.1.2.5  1996/01/20  01:22:59  szhang
 * Branch: ELC_branch
 * Make the parity detection code work on the Predator platform.
 *
 * Revision 3.1.2.4  1996/01/17  05:54:52  szhang
 * Branch: ELC_branch
 * Fixed the problem with chip select to mid-plane eeprom.
 * NVRAM size is also set in the FPGA I/O control register.
 *
 * Revision 3.1.2.3  1996/01/05  23:19:27  mansonw
 * Branch: ELC_branch
 *
 * Add memory sizing code.
 *
 * Revision 3.1.2.2  1995/12/08  05:00:07  mbeesley
 * Branch: ELC_branch
 * Setup first 5 TLB entiries, init PCI, PCMCIA and ralib. Clean up
 * exception/interrupt handling, fix NVRAM handling.
 *
 * Revision 3.1.2.1  1995/12/01  21:31:35  mbeesley
 * Branch: ELC_branch
 * Fix LED register handling, add EMT call support and memory sizing.
 *
 * Revision 3.1  1995/12/01  18:27:42  mbeesley
 * Add placeholder files for new development.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "monitor.h"
#include "signal_4k.h"
#include "error.h"
#include "testmem.h"
#include "mon_defs.h"
#include "cpu_mips.h"
#include "gt64010.h"
#include "pred_plat_defs.h"

#define MMOFFSET    0x80000  	/* half meg */

#define S_128KB     0x20000  	/* 128 K bytes */
#define S_HALFMEG   0x80000  	/* half meg */
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
#define NVRAM_INDEX 1

#define UORG_TEST_DATA     0xcafebabe
#define LORG_TEST_DATA     0xfeedbeef
#define UNEW_TEST_DATA     0xdeadface
#define LNEW_TEST_DATA     0xabeeafee

#define NO_MEM_ERROR       0
#define MEM_ERROR          1
#define MAXBANK            4

#define ENABLE_PARITY()  \
    setcp0_sreg(getcp0_sreg() & ~SREG_DE);\
    KSEG1_GT64010->dram_bank0_param |= DRAM_BANK_PARITY;\
    KSEG1_GT64010->dram_bank1_param |= DRAM_BANK_PARITY;\
    KSEG1_GT64010->dram_bank2_param |= DRAM_BANK_PARITY;\
    KSEG1_GT64010->dram_bank3_param |= DRAM_BANK_PARITY;

#define DISABLE_PARITY()  \
    KSEG1_GT64010->dram_bank0_param &= ~DRAM_BANK_PARITY;\
    KSEG1_GT64010->dram_bank1_param &= ~DRAM_BANK_PARITY;\
    KSEG1_GT64010->dram_bank2_param &= ~DRAM_BANK_PARITY;\
    KSEG1_GT64010->dram_bank3_param &= ~DRAM_BANK_PARITY;\
    setcp0_sreg(getcp0_sreg() | SREG_DE);

        


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

    "nvram",
    (unsigned long *)(KSEG1_ADRSPC_NVRAM),
    &nvsize
};
    

/*
 * function prototype 
 */
long sizing_sram();
long sizing_nvram();
long sizemainmem(void);
long sizeshmem(void);
long size_nvram(void);
ulong size_and_pack_mem(void);
void get_gal_sim_para(ulong Bank, struct simParm *sim_pt);
void print_mem_config(void);

/*
 * memsizing:
 */
int memsizing (int testindex)
{
    long tmp;
    
    if (testindex == 1) {
	tmp = sizemainmem();
    } else if (testindex == 2) {
	tmp = sizeshmem();
    } else {
	printf("Test Index Invalid %d\n", testindex);
	return(1);
    }

    if (tmp == 0) {
	printf("Memory can't be sized\n");
	return(1);
    } else {
	return(0);
    }
}

/*
 * sizing_sram:
 * Size packet SRAM
 */
long sizing_sram (void)
{
    return(S_ONEMEG);
}

/*
 * sizing_nvram:
 * Get the size of NVRAM
 */
long sizing_nvram()
{
    struct meminfo *meminfoptr;
    unsigned long *memoffset, testval, readval0, readval,  sreg;
    SIG_PF sigsav, sigint;
    unsigned long sav_val1, sav_val2;      /* used to save the values in NVRAM */
    long nvram_size_t;         
    
    meminfoptr = &(memtype[NVRAM_INDEX]);
    memoffset  = meminfoptr->memoffset;

    sav_val1 = *memoffset;
    testval = 0xcafebabe;

    sigsav = signal(SIGBUS,SIG_IGN);   /* set to ignore bus errors */
    sigint = signal(SIGINT,SIG_IGN);   /* ignore user interrupts   */
    hkeepflags &= ~H_BUSERR;           /* initialize flag          */
    sreg = getcp0_sreg();	       /* disable parity exception */
    setcp0_sreg(sreg | SREG_DE);

    *memoffset = testval;              /* write the first location */
    readval = *(volatile unsigned long *)memoffset;   /* read the first 
							 location  */

    if (((readval & 0xff) != (testval & 0xff)) || hkeepflags & H_BUSERR) {
	printf("Warning: NVRAM size is 0 \n");
	setcp0_sreg(sreg);             /* restore status register */
	signal(SIGBUS,sigsav);         /* set back to original value */
	signal(SIGINT,sigint);	       /* restore user interrupts    */
	return(0);                     /* zero memory is found */
    }

    KSEG1_PRED_FPGA->io_ctrl_reg &= ~NVRAM_SIZE;
    testval = 0xdeadface;
/*
 * It is unlikely that it will generate bus error as the CS2 Decode Address
 * range is 1M mininum. So, check the test value wrap-around instead.
 * Also, using (memoffset + S_128KB) to take care of the holes in the NVRAM read
 */
    sav_val2 = *(volatile unsigned long *)(memoffset + S_128KB);
    *(volatile unsigned long *)(memoffset + S_128KB) = testval;
    readval = *(volatile unsigned long *)(memoffset + S_128KB);
    readval0 = *(volatile unsigned long *)memoffset;

    if ((hkeepflags & H_BUSERR) || 
	((readval & 0xff) != (testval & 0xff)) ||
	((readval0 & 0xff) == (testval & 0xff))) {
	nvram_size_t = S_128KB;
	KSEG1_PRED_FPGA->io_ctrl_reg |= NVRAM_SIZE;
    }
    else {
	nvram_size_t = S_128KB * 4;
	*(volatile unsigned long *)(S_128KB + (unsigned long)memoffset) = sav_val2;
	KSEG1_GT64010->cs2_high = 0xe1000000; /* With 512KB NVRAM, the cs2_high */
	                                      /* needed to be set to 1e1fffff */
    }

    *memoffset = sav_val1;
    
    /*
     * Return signal to original value, print out
     * and return success
     */
    setcp0_sreg(sreg);		/* restore status register */
    signal(SIGINT,sigint);	/* restore user interrupt  */
    signal(SIGBUS,sigsav);	/* restore bus errors      */

    return (nvram_size_t);
}


ulong mem_size;
struct simParm simBank[MAXBANK];

/*------------------------------------------------------------------
 *
 * This function scans the DRAM SIMs on board and explores their parameters.
 * It configures the GT64010 registers according to the actual DRAM SIMs on board.
 * The function also maps all DRAM banks to one continous memory block.
 *
 *------------------------------------------------------------------*/
ulong size_and_pack_mem()
{
    ulong i;
    ulong BankTopAdd;
    
    if (mem_size)       /* This packing process can be done only once */
	return(memsize);

#ifdef DEBUG
    print_mem_config();
#endif

    /* For other Banks explore Bank parameters now */
    get_gal_sim_para(0, &simBank[0]);
    get_gal_sim_para(1, &simBank[1]);
    get_gal_sim_para(2, &simBank[2]);
    get_gal_sim_para(3, &simBank[3]);

    /* Change DRAM Mapping so that all the DRAM memory would be */
    /* a continouse memory block                                */

    /* Place Bank 0 at the beginning of memory block */
    KSEG1_GT64010->ras10_low = 0;
    KSEG1_GT64010->ras0_low = 0;
    BankTopAdd = simBank[0].size;
    KSEG1_GT64010->ras0_high = vaxorder(((BankTopAdd - 4) & 0xff00000) >> 20);
    /* printf("BankTopAdd0 = %x\n", BankTopAdd); */

    /* Place Bank 1 continously after Bank 0 */
    KSEG1_GT64010->ras1_low = vaxorder((BankTopAdd & 0xff00000) >> 20);
    BankTopAdd = BankTopAdd + simBank[1].size;
    KSEG1_GT64010->ras1_high = vaxorder(((BankTopAdd  - 4) & 0xff00000) >> 20);
    KSEG1_GT64010->ras10_high = vaxorder(((BankTopAdd - 4) & 0xfe00000) >> 21);
    /* printf("BankTopAdd1 = %x\n", BankTopAdd); */

    /* Place Bank 2 continously after Bank 1 */
    KSEG1_GT64010->ras32_low = vaxorder((BankTopAdd & 0xffe00000) >> 21);
    KSEG1_GT64010->ras2_low = vaxorder((BankTopAdd & 0xff00000) >> 20);
    BankTopAdd = BankTopAdd + simBank[2].size;
    KSEG1_GT64010->ras2_high = vaxorder(((BankTopAdd - 4) & 0xff00000) >> 20);
    /* printf("BankTopAdd2 = %x\n", BankTopAdd); */

    /* Place Bank 3 continously after Bank 2 */
    KSEG1_GT64010->ras3_low = vaxorder((BankTopAdd & 0xff00000) >> 20);
    BankTopAdd = BankTopAdd + simBank[3].size;
    KSEG1_GT64010->ras3_high = vaxorder(((BankTopAdd - 4) & 0xff00000) >> 20);
    KSEG1_GT64010->ras32_high = vaxorder(((BankTopAdd - 4) & 0xfe00000) >> 21);
    /* printf("BankTopAdd3 = %x\n", BankTopAdd); */

    return(mem_size = BankTopAdd); 
}

/*---------------------------------------------------------------------------*
 * This function explores the SIM actual parameters                         
 * It checks:
 *
 * 1. DRAM Bank row size: 9,10,11,12 bits.
 * 2. Word Width: 32/64 bit Bank.
 * 3. Size of the Bank
 *
 *---------------------------------------------------------------------------*/
#define DRAM_BANK_BANKWDTH_MASK  ~DRAM_BANK_BANKWDTH_64BIT
#define DRAM_BANK_XKRF_MASK ~DRAM_BANK_XKRF_4K

void get_gal_sim_para(ulong bank, struct simParm *sim_pt)
{
    ulong *BaseAdd;
    ulong *TopAdd;
    ulong *add;
    ulong i, top, LastTop;
    ulong SizeIn32;
    ulong temp;
    volatile ulong *ParAddr;
    
#ifdef DEBUG
    printf("Sizing Bank %d:\n", bank);
#endif
    /* Extract the current mapping of the DRAM banks from the GT64010 registers */
    switch (bank)
	{
	case 0:
	    BaseAdd = (ulong*)(((vaxorder(KSEG1_GT64010->ras10_low) & 0x7f80) << 21) |
			       (vaxorder(KSEG1_GT64010->ras0_low) << 20) | 0xa0000000);
	    TopAdd = (ulong*)(((vaxorder(KSEG1_GT64010->ras10_low) & 0x7f80) << 21) |
			      (vaxorder(KSEG1_GT64010->ras0_high) << 20) | 0xa00ffffc);
	    ParAddr = &KSEG1_GT64010->dram_bank0_param;
	    break;
	case 1:
	    BaseAdd = (ulong*)(((vaxorder(KSEG1_GT64010->ras10_low) & 0x7f80) << 21) |
			       (vaxorder(KSEG1_GT64010->ras1_low) << 20) | 0xa0000000);
	    TopAdd = (ulong*)(((KSEG1_GT64010->ras10_low & 0x7f80) << 21) |
			      (vaxorder(KSEG1_GT64010->ras1_high) << 20) | 0xa00ffffc);
	    ParAddr = &KSEG1_GT64010->dram_bank1_param;
	    break;
	case 2:
	    BaseAdd = (ulong*)(((vaxorder(KSEG1_GT64010->ras32_low) & 0x7f80) << 21) |
			       (vaxorder(KSEG1_GT64010->ras2_low) << 20) | 0xa0000000);
	    TopAdd = (ulong*)(((vaxorder(KSEG1_GT64010->ras32_low) & 0x7f80) << 21) |
			       (vaxorder(KSEG1_GT64010->ras2_high) << 20) | 0xa00ffffc);
	    ParAddr = &KSEG1_GT64010->dram_bank2_param;
	    break;
	case 3:
	    BaseAdd = (ulong*)(((vaxorder(KSEG1_GT64010->ras32_low) & 0x7f80) << 21) |
			       (vaxorder(KSEG1_GT64010->ras3_low) << 20) | 0xa0000000);
	    TopAdd = (ulong*)(((vaxorder(KSEG1_GT64010->ras32_low) & 0x7f80) << 21) |
			      (vaxorder(KSEG1_GT64010->ras3_high) << 20) | 0xa00ffffc);
	    ParAddr = &KSEG1_GT64010->dram_bank3_param;
	    break;
	}

    /* Check Row Size and Word size */
    temp = *BaseAdd;                     /* Save current value */
    *BaseAdd = (ulong)BaseAdd + 1;       /* Place a new value into DRAM base address */
    
    if (*BaseAdd == (ulong)BaseAdd + 1)  /* If Memory exists in this bank */
	{
	    /* Check 32/64 bit */
	    *ParAddr = *ParAddr | DRAM_BANK_BANKWDTH_64BIT;   /* Set 64 bit mode */
	    BaseAdd[0] = (ulong)(BaseAdd + 3);
	    BaseAdd[1] = (ulong)(BaseAdd + 4);
	    if ((BaseAdd[0] != (ulong)(BaseAdd + 3)) ||
		(BaseAdd[1] != (ulong)(BaseAdd + 4)))
		{
		    sim_pt->wordwidth = 64;
		    *ParAddr = *ParAddr & DRAM_BANK_BANKWDTH_MASK | 
			DRAM_BANK_BANKWDTH_64BIT; /* Set to 64 bits anyway */
#ifdef DEBUG
		    printf("Config Violation!!!\nSet bank size to 0.\n");
#endif

		    sim_pt->size = 0;
		    return;
	    } else {
		*ParAddr = *ParAddr & DRAM_BANK_BANKWDTH_MASK |
		    DRAM_BANK_BANKWDTH_64BIT;     /* Set to 64 bits */
		sim_pt->wordwidth = 64;
#ifdef DEBUG
		printf("Config set to 64 bit.\n");
#endif
	    }
	    
	    /* Check Row size */
	    /* Set Bank to 4K refresh  12bit Row size */
	    *ParAddr = *ParAddr & DRAM_BANK_XKRF_MASK | DRAM_BANK_XKRF_4K;

	    /* Check row size */
	    BaseAdd[0] = 0x55555555;
	    for (i = 14; i > 10; i--)
		if (BaseAdd[1 << i] != 0x55555555)    /* Set address bit 16..13 */
		    break;                            /* to check 4K..1/2K refresh */

	    *ParAddr = *ParAddr & DRAM_BANK_XKRF_MASK | 
		(DRAM_BANK_XKRF_4K - DRAM_BANK_XKRF_1K * (14 - i));

#ifdef DEBUG
	    printf("*ParAddr = %x while i = %d\n", *ParAddr, i);
#endif
	    sim_pt->rowsize = i - 2;
	    
	    /* Check Bank Size */
	    BaseAdd[0] = 0x13243546;
	    for (i = 2; i < 32; i++)
		if (&BaseAdd[1<<i] > TopAdd) {
#ifdef DEBUG
		    printf(".....MAX SIZE FOUND\n");
#endif
		    break;
		} else if (BaseAdd[1<<i] == 0x13243546) {
#ifdef DEBUG
		    printf(".....ADDR WRAP FOUND\n");
#endif
         	    break;
		}

	    sim_pt->size = (1<<i)*4;
#ifdef DEBUG
	    printf("i = %d while RowSize = %d and size installed = %x\n",
		   i, sim_pt->rowsize, sim_pt->size);
#endif
	} else {
	    sim_pt->size = 0;               /* No SIM on this BANK */
#ifdef DEBUG
	    printf("No SIM Installed.\n");
#endif
	}

    *BaseAdd = temp;             /* Return to original value */
}


void print_mem_config(void)
{
    printf("DRAM SIM configuration:\n");
    printf("ras10_low = %x\n",KSEG1_GT64010->ras10_low);
    printf("ras10_high = %x\n",KSEG1_GT64010->ras10_high);
    printf("ras32_low = %x\n",KSEG1_GT64010->ras32_low);
    printf("ras32_high = %x\n",KSEG1_GT64010->ras32_high);

    printf("ras0_low = %x\n",KSEG1_GT64010->ras0_low);
    printf("ras0_high = %x\n",KSEG1_GT64010->ras0_high);
    printf("ras1_low = %x\n",KSEG1_GT64010->ras1_low);
    printf("ras1_high = %x\n",KSEG1_GT64010->ras1_high);
    printf("ras2_low = %x\n",KSEG1_GT64010->ras2_low);
    printf("ras2_high = %x\n",KSEG1_GT64010->ras2_high);
    printf("ras3_low = %x\n",KSEG1_GT64010->ras3_low);
    printf("ras3_high = %x\n",KSEG1_GT64010->ras3_high);

    printf("dram_bank0_param = %x\n",KSEG1_GT64010->dram_bank0_param);
    printf("dram_bank1_param = %x\n",KSEG1_GT64010->dram_bank1_param);
    printf("dram_bank2_param = %x\n",KSEG1_GT64010->dram_bank2_param);
    printf("dram_bank3_param = %x\n",KSEG1_GT64010->dram_bank3_param);

    printf("RowSize[0]  =%d RowSize[1]  =%d RowSize[2]  =%d RowSize[3]  =%d\n",
	   simBank[0].rowsize, simBank[1].rowsize,
	   simBank[2].rowsize, simBank[3].rowsize);
    printf("WordWidth[0]=%d WordWidth[1]=%d WordWidth[2]=%d WordWidth[3]=%d\n",
	   simBank[0].wordwidth, simBank[1].wordwidth,
	   simBank[2].wordwidth, simBank[3].wordwidth);
    printf("Size[0]     =%x Size[1]     =%x Size[2]     =%x Size[3]     =%x\n",
	   simBank[0].size, simBank[1].size, simBank[2].size, simBank[3].size);
}


/*
 * The following routines are convenience tools provided for use
 * by other diagnostics.
 * The two sizing routines return the size of the memory or zero.
 * The start routines return the starting address or 0xffffffff.
 * Both routines will size the memory if it has not already been.
 */
long sizemainmem (void)
{
    unsigned long sreg;
    unsigned long size;

    if (memsize) 
	return(memsize);

    /* 
     * sizing the memory and initialize those memory to zero
     */
    sreg = getcp0_sreg();               /* disable parity exception */
    setcp0_sreg(sreg | SREG_DE);

#ifdef DEBUG
    printf("Performing memory sizing...\n");
#endif
    memsize = size_and_pack_mem();
#ifdef DEBUG
    print_mem_config();
#endif
    if (memsize != 0) {
	bzero(MON_FREE_MEM, memsize - (MON_FREE_MEM - KSEG1_ADRSPC_RAM));
    }

    setcp0_sreg(sreg);                 /* restore status register */
    return (memsize);
}


/*
 * sizeshmem:
 * Size packet SRAM
 */
long sizeshmem (void)
{
    unsigned long sreg;

    if (shmemsize) 
	return(shmemsize);

    sreg = getcp0_sreg();              /* disable parity exception */
    setcp0_sreg(sreg | SREG_DE);

    shmemsize = sizing_sram();
    setcp0_sreg(sreg);                 /* restor status register */
    return(shmemsize);
}

/*
 * mainmemstart:
 */
long mainmemstart (void)
{
    return(sizemainmem() == 0 ? (long)-1 : KSEG1_ADRSPC_RAM);
}

/*
 * shmemstart:
 */
long shmemstart (void)
{
    return(sizeshmem() == 0 ? (long)-1 : ADRSPC_PCISRAM);
}

/*
 * ismainmemaddr:
 */
int ismainmemaddr (long addr)
{
    long phymemstart;
    long phyaddr;

    if (sizemainmem()) {
	phymemstart = PHY_ADRSPC_RAM;
	phyaddr = PHY_ADDR(addr);
	if(phyaddr >= phymemstart &&
	   phyaddr < (phymemstart + memsize)) {
	    return(1);
	}
    }
    return(0);
}

/* 
 * Test either main or shared memory.
 *
 * Returns :
 *    0    : test complete
 *    1    : failed
 */
int sysmemtest (int testindex)
    /* 1: main memory  2: shared */
{
    struct meminfo {
	char  *memstr;
	unsigned *memaddr;
	unsigned *startadd;
	long *memsizeptr;
    } memtype[] = {
	"main"  , (unsigned *)KSEG1_ADRSPC_RAM, (unsigned *)MON_FREE_MEM, 
	&memsize,
    };
    struct meminfo *meminfoptr;
    struct testmem tmem;
    register struct testmem *tmemptr=&tmem;
    int tmp=0;

    if (testindex != 1) {
	printf("Error - can only test main memory\n");
	return(1);
    }
    meminfoptr = &memtype[testindex-1];
    if (*meminfoptr->memsizeptr == 0)    /* see if memory has been sized 
					    already */
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

    if (testmem(tmemptr) < 0) 
	return(1);

    testname("%s memory refresh",meminfoptr->memstr);
    if (memrefresh(tmemptr) < 0) 
	return(1);
    return(0);
}

long freememstart;

long getfreememstart (void)
{
    if(!freememstart) {
	freememstart = (long)(errlog_start+ERRLOG_SIZE);
    }
    return(freememstart);
}

/*
 * size_nvram:
 */
long size_nvram (void)
{
    unsigned long temp;

    if (!nvsize) {
	nvsize = sizing_nvram();
    }
    return(nvsize);
}


#define PATTERN_1 0xaaaaaaaa
#define PATTERN_2 0x55555555
int simple_mem_test(ulong *memstart, ulong size)
{
    ulong testsize;
    ulong *tempptr, *endptr;

    testsize = size/sizeof(long);
    endptr = memstart + testsize;
    
    printf("memstart = %x endptr = %x testsize = %x\n", memstart, endptr, testsize);
    printf("Zero Memory ... \n");
    bzero(memstart, size);
    
    printf("Check memory with 0 and fill with pattern %x\n", PATTERN_1);
    for (tempptr = memstart; tempptr < endptr ; tempptr++)
	if (*tempptr)
	    {
		printf("Memory Test failed at %x with %x test...result = %x\n",
		       tempptr, 0, *tempptr);
		return(1);
	    }
	else
	    *tempptr = PATTERN_1;

    printf("Check memory with pattern %x and fill with pattern %x\n",
	   PATTERN_1, PATTERN_2);
    for (tempptr = memstart; tempptr < endptr ; tempptr++)
	if (*tempptr != PATTERN_1)
	    {
		printf("Memory Test failed at %x with %x test...result = %x\n",
		       tempptr, PATTERN_1, *tempptr);
		return(1);
	    }
	else
	    *tempptr = PATTERN_2;
    
    printf("Check memory with pattern %x and fill with zero\n", PATTERN_2);
    for (tempptr = memstart; tempptr < endptr ; tempptr++)
	if (*tempptr != PATTERN_2)
	    {
		printf("Memory Test failed at %x with %x test...result = %x\n",
		       tempptr, PATTERN_2, *tempptr);
		return(1);
	    }
	else
	    *tempptr = 0;
    
    return(0);
}
 

/*
 * Print out some memory information
 */
int meminfo(int argc, char *argv[])
{
    long memsiz, availmem;
    int temp;
    int ix;

    memsiz =  sizemainmem();
    availmem = getfreememstart();
    printf("\nMain memory size: %d MB.\n",memsiz>>20);
    printf("Packet memory size: %d MB\n", sizeshmem()>>20);

    printf("Main memory size: 0x%x\n", memsiz);
    printf("Available main memory starts at %#x, size %#x\n",
	   availmem, (mainmemstart() + memsiz) - availmem);
    printf("NVRAM size: %#x\n", size_nvram());

#ifdef DEBUG
    printf("\nDRAM Parameter Info\n");
    for (ix = 0; ix < MAXBANK; ix++) {
	printf("rowsize = %x, wordwidth = %x, size = %x, simma_parity = %d, simmb_parity = %d\n",
	        simBank[ix].rowsize, simBank[ix].wordwidth, simBank[ix].size,
	        simBank[ix].simma_parity, simBank[ix].simmb_parity);
    }
#endif

    printf("\nParity Map for the DRAM Banks\n");
    for (ix = 0; ix < MAXBANK; ix++) {
	if (simBank[ix].size > 0) {
	    if (simBank[ix].simma_parity == PARITY_YES) {
		if (ix < 2) {
		    printf("Socket 0 in Bank %d Has Parity\n", ix);
		} else {
		    printf("Socket 2 in Bank %d Has Parity\n", ix);
		}
	    } else {
		if (ix < 2) {
		    printf("Socket 0 in Bank %d Has No Parity\n", ix);
		} else {
		    printf("Socket 2 in Bank %d Has No Parity\n", ix);
		}
	    }
	    
	    if (simBank[ix].simmb_parity == PARITY_YES) {
		if (ix < 2) {
		    printf("Socket 1 in Bank %d Has Parity\n", ix);
		} else {
		    printf("Socket 3 in Bank %d Has Parity\n", ix);
		}
	    } else {
		if (ix < 2) {
		    printf("Socket 1 in Bank %d Has No Parity\n", ix);
		} else {
		    printf("Socket 3 in Bank %d Has No Parity\n", ix);
		}
	    }
	}
    }

    printf("\n");       /* insert a blank line */
	

    if (hkeepflags & H_PRIVILEGED) {
	printf("Physical Address  : \n");
	printf("ROM               : 0x%.8x\n", PHY_ADRSPC_PROM);
	printf("NVRAM             : 0x%.8x\n", PHY_ADRSPC_NVRAM);
	printf("Boot Flash        : 0x%.8x\n", PHY_ADRSPC_BFLASH);
	printf("Packet SRAM       : 0x%.8x\n", PHY_ADRSPC_PCISRAM);
	printf("IO Registers      : 0x%.8x\n", PHY_ADRSPC_PRED_FPGA);
	printf("MP Registers      : 0x%.8x\n", PHY_ADRSPC_MP_FPGA);
	printf("GT64010 Registers : 0x%.8x\n", PHY_ADRSPC_GT64010_BASE);
	printf("Bay 0, IO Card    : 0x%.8x\n", PHY_ADRSPC_BAY0);
	printf("Bay 1, PA 1       : 0x%.8x\n", PHY_ADRSPC_BAY1);
	printf("Bay 2, PA 2       : 0x%.8x\n", PHY_ADRSPC_BAY2);
	printf("Bay 3, PA 3       : 0x%.8x\n", PHY_ADRSPC_BAY3);
	printf("Bay 4, PA 4       : 0x%.8x\n", PHY_ADRSPC_BAY4);
	printf("Bay 5, PA 5       : 0x%.8x\n", PHY_ADRSPC_BAY5);
	printf("Bay 6, PA 6       : 0x%.8x\n", PHY_ADRSPC_BAY6);
    } 

#ifdef DEBUG
    printf("Memory Test ...\n");
    temp = simple_mem_test((ulong *)availmem, (mainmemstart() + memsiz) - availmem);
    printf("Memory Test ... %s\n", temp ? "FAILED" : "PASSED");
#endif

    return(0);
}

/*
 ***************************************************************
 * PCMCIA Flash socket sizes
 ***************************************************************
 */

int slot0_size (void)
{
    return(0x1400000);
}

int slot1_size (void)
{
    return(0x1400000);
}


/*
 * detect_parity:
 * Return TRUE when the DRAM parity is supported.
 */

#define ODD_ONE_PATTERN       0xEFEFEFEF
#define EVEN_ONE_PATTERN      0xFFEEFFEE

unsigned char bank_parity;    /* used to store parity flags for IOS */

void detect_parity (void)
{
    SIG_PF sigsav, sigint;
    volatile unsigned long *testloc, testval, utestval;
    int ix;                    /* index to bank */

    setcp0_sreg(getcp0_sreg() & ~SREG_IE); /* disable interrupts to be safe */

    sigsav = signal(SIGBUS,SIG_IGN);   /* set to ignore bus errors */
    sigint = signal(SIGINT,SIG_IGN);   /* ignore user interrupts   */
    hkeepflags &= ~H_BUSERR;           /* initialize flag          */

    testloc = (volatile unsigned long *)KSEG1_ADRSPC_RAM;

    /* Flush primary and secondary caches and move stack to kseg1 to avoid
       cache parity exception from a cached stack */
    cache_flush(0, 1);

    for (ix=0; ix < MAXBANK; ix++) {
	if (simBank[ix].size > 0) {             /* if bank size bigger than 0 */
	    *testloc = ODD_ONE_PATTERN;
	    *(testloc + 1) = EVEN_ONE_PATTERN;

	    ENABLE_PARITY();
	    lw_instr((unsigned long *)testloc);
	    DISABLE_PARITY();

	    if (hkeepflags & H_BUSERR) {
		simBank[ix].simmb_parity = PARITY_NO;
		hkeepflags &= ~H_BUSERR;
	    } else {
		simBank[ix].simmb_parity = PARITY_YES;
	    }

	    *testloc = EVEN_ONE_PATTERN;
	    *(testloc + 1) = ODD_ONE_PATTERN;

	    ENABLE_PARITY();
	    lw_instr((unsigned long *)testloc);
	    DISABLE_PARITY();

	    if (hkeepflags & H_BUSERR) {
		simBank[ix].simma_parity = PARITY_NO;
		hkeepflags &= ~H_BUSERR;
	    } else {
		simBank[ix].simma_parity = PARITY_YES;
	    }
	}
	testloc += (simBank[ix].size / sizeof(unsigned long));
    }
    

    bank_parity = 0; /* parity info for system image */
    for (ix = MAXBANK - 1; ix >= 0; ix--) {
	bank_parity = (bank_parity << 1) | simBank[ix].simmb_parity;
	bank_parity = (bank_parity << 1) | simBank[ix].simma_parity;
    }

    signal(SIGBUS, sigsav);
    signal(SIGINT, sigint);

    setcp0_sreg(getcp0_sreg() | SREG_IE); /* re-enable interrupts */
 
    /* Flush primary and secondary caches to make sure no bad data are
       left in caches, and move stack back to kseg0 */
    cache_flush(0, 0);
}

/*
 * config_dram_parity:
 * Setup hardware to support parity or not
 */
void config_dram_parity (void)
{

    /* 
     * config parity for bank 0
     */
    if (simBank[0].simma_parity != simBank[0].simmb_parity)
	KSEG1_GT64010->dram_bank0_param &= ~DRAM_BANK_PARITY;
    else if (simBank[0].simma_parity == PARITY_YES)
	KSEG1_GT64010->dram_bank0_param |= DRAM_BANK_PARITY;
    else
	KSEG1_GT64010->dram_bank0_param &= ~DRAM_BANK_PARITY;

    /*
     * config parity for bank 1
     */
    if (simBank[1].simma_parity != simBank[1].simmb_parity)
	KSEG1_GT64010->dram_bank1_param &= ~DRAM_BANK_PARITY;
    else if (simBank[1].simma_parity == PARITY_YES)
	KSEG1_GT64010->dram_bank1_param |= DRAM_BANK_PARITY;
    else
	KSEG1_GT64010->dram_bank1_param &= ~DRAM_BANK_PARITY;

    /*
     * config parity for bank 2
     */
    if (simBank[2].simma_parity != simBank[2].simmb_parity)
	KSEG1_GT64010->dram_bank2_param &= ~DRAM_BANK_PARITY;
    else if (simBank[2].simma_parity == PARITY_YES)
	KSEG1_GT64010->dram_bank2_param |= DRAM_BANK_PARITY;
    else
	KSEG1_GT64010->dram_bank2_param &= ~DRAM_BANK_PARITY;

     /*
      * config parity for bank 3
      */
    if (simBank[3].simma_parity != simBank[3].simmb_parity)
	KSEG1_GT64010->dram_bank3_param &= ~DRAM_BANK_PARITY;
    else if (simBank[3].simma_parity == PARITY_YES)
	KSEG1_GT64010->dram_bank3_param |= DRAM_BANK_PARITY;
    else
	KSEG1_GT64010->dram_bank3_param &= ~DRAM_BANK_PARITY;
    
}

int 
check_dram_config()
{
    if (simBank[1].size == 0) {
	if ((simBank[0].size >= simBank[2].size) &&
	    (simBank[0].size >= simBank[3].size))
	    return(0);
    } else {
	if ((simBank[0].size >= simBank[2].size) &&
	    (simBank[0].size >= simBank[3].size) &&
	    (simBank[1].size >= simBank[2].size) &&
	    (simBank[1].size >= simBank[3].size))
	    return(0);
    }

    return(1);
}

/* End of file */
