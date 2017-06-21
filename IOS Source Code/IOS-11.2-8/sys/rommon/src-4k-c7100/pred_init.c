/* $Id: pred_init.c,v 3.2.58.7 1996/07/18 17:48:56 mbeesley Exp $
 * $Source: /release/112/cvs/Xsys/rommon/src-4k-c7100/pred_init.c,v $
 *------------------------------------------------------------------
 * pred_init.c -- Predator Rom Monitor init code
 *
 * July 1995, Michael Beesley
 *
 * Copyright (c) 1996-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: pred_init.c,v $
 * Revision 3.2.58.7  1996/07/18  17:48:56  mbeesley
 * CSCdi63193:  Need support for rev2 gt64010CSCdi63193: Teach the rom
 * monitor
 * about the incompatibilities between rev1 and rev2 gt64010s :
 *         o Addressing flip for bytes accessed thru PCI IO space
 *         o Internal PCI config register addressing differences
 * Branch: California_branch
 *
 * Revision 3.2.58.6  1996/06/01  02:46:15  clev
 * CSCdi55487:  Add support for AMD 008 and 016 flash SIMMs
 * Branch: California_branch
 *
 * Fujitsu chips too.
 *
 * Revision 3.2.58.5  1996/04/20  01:10:59  szhang
 * CSCdi55174:  Reset Reason Reg is not read correctly
 * Branch: California_branch
 *
 * Revision 3.2.58.4  1996/04/09  23:09:40  szhang
 * CSCdi53689:  Open() failed in PCMCIA monlib
 * Branch: California_branch
 * Monitor shouldn't reset the PCMCIA controller after the MONLIB
 * is launched.
 *
 * Revision 3.2.58.3  1996/04/06  02:05:35  ssangiah
 * CSCdi53822:  Need to deal with product name change.
 * Make changes to deal with the change in the product name for Predator
 * from c7100 to c7200.
 * Branch: California_branch
 *
 * Revision 3.2.58.2  1996/04/04  17:05:56  szhang
 * CSCdi52979:  Bootloader hang after OIR HW change
 * Branch: California_branch
 *
 * Revision 3.2.58.1  1996/03/21  23:31:37  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.2.2.10  1996/03/07  00:51:53  szhang
 * CSCdi50915:  ROM monitor hang after reload
 * Branch: ELC_branch
 * The cause is that the port adaptors are not
 * shut down.
 *
 * Revision 3.2.2.9  1996/02/28  01:00:38  szhang
 * CSCdi50105:  C7100 ROMMON code cleanup
 * Branch: ELC_branch
 *
 * Revision 3.2.2.8  1996/02/13  00:43:09  mbeesley
 * CSCdi48770:  Make reset_io() work, add support for fault history
 * Branch: ELC_branch
 *
 * Revision 3.2.2.7  1996/01/20  01:22:54  szhang
 * Branch: ELC_branch
 * Make the parity detection code work on the Predator platform.
 *
 * Revision 3.2.2.6  1995/12/11  19:14:58  mbeesley
 * Branch: ELC_branch
 * Sync ELC_branch to ELC_baseline_120995.
 *
 * Revision 3.2.2.5  1995/12/08  05:00:03  mbeesley
 * Branch: ELC_branch
 * Setup first 5 TLB entiries, init PCI, PCMCIA and ralib. Clean up
 * exception/interrupt handling, fix NVRAM handling.
 *
 * Revision 3.2.2.4  1995/12/05  23:12:27  mbeesley
 * Branch: ELC_branch
 * Add complete support for discontigous NVRAM.
 *
 * Revision 3.2.2.3  1995/12/01  21:31:33  mbeesley
 * Branch: ELC_branch
 * Fix LED register handling, add EMT call support and memory sizing.
 *
 * Revision 3.2.2.2  1995/12/01  04:23:51  mbeesley
 * Branch: ELC_branch
 * Reorganize Rom monitor header files to make it easier to port
 * to new platforms.
 *
 * Revision 3.2.2.1  1995/11/23  19:14:40  szhang
 * Branch: ELC_branch
 * Initial source code commit to ELC branch for Predator (C7100) ROM
 * monitor.
 *
 * Revision 3.2  1995/11/17  18:46:10  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:08:40  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "monitor.h"
#include "proto.h"
#include "console.h"
#include "confreg.h"
#include "signal_4k.h"
#include "mon_defs.h"
#include "cpu_mips.h"
#include "extern.h"
#include "cookie.h"
#include "ralib.h"
#include "flash.h"
#include "dev.h"
#include "mon_boot.h"

#include "../../../boot/cpu.h"
#include "../../src-4k/gt64010.h"
#include "../../src-4k/vspace_4k.h"

/*
 * Hardware resource pointers
 */
struct nvram *nvram_ptr = ((struct nvram *)(KSEG1_ADRSPC_NVRAM));
gt64010_regs_t *gt_ptr = (gt64010_regs_t *)KSEG1_ADRSPC_GT64010_BASE;

/*
 * Some variables
 */
char 		*errlog_start = ((char *)(MON_FREE_MEM));
char 		*platform_string = "C7200";
char 		*default_monlib_string = "%c7200-fslib-m";
unsigned char 	reason_reg;
int 		cpu_type;

ralib_vector_table_t *ralib_vector_table_ptr;

volatile unsigned char intimerint;  /* 0: never got into timer int handler
				       !0: got into timer int handler */

static volatile int alarm_setting;  /* What to set the alarm to */
static volatile int alarm_count;    /* Number of milliseconds left */
static volatile int reset_type_sav;

/* 
 * Forward declarations 
 */
static void 	init_soft_reset(int reset_type, unsigned long *frame, 
				uchar gt64010_rev);
void 	        detect_parity(void);
void 	        config_dram_parity(void);
void 		reset_io(void);
void		flush_all_wb(void);

struct mtestvals {
    long val1, val2;
    char *name;
} mtestvals[] = {
    0x0, 0x0, "data equals address",
    0x55555555, 0xaaaaaaaa, "checkerboard",
    0xaaaaaaaa, 0x55555555, "inverse checkerboard",
};

#define NUM_MTEST_VALS   				\
    (sizeof(mtestvals)/sizeof(struct mtestvals))

#define STACK_LEN 0x2000
#define RAM_TEST_START 		(long *)(KSEG1_ADRSPC_RAM)
#define RAM_TEST_END   		(long *)(MON_FREE_MEM - STACK_LEN)

/*
 * Device ID strings
 */
char mon_main_mem_id[] = DEV_KMEM1_ID;
char default_bootdev_id[] = DEV_BOOTFLASH_ID;
char default_boothelper_id[] = DEV_BOOTFLASH_ID;

/*
 * Device Table
 * 
 * The first item in the devstruct table is the default
 * search area when a device specifier is not used or known.
 * The network device(s) must be the only ones with a NULL
 * devsizroutine.
 *
 * This is the initial structure in EPROM, it is copied into
 * a RAM structure at runtime so fields can be updated
 */
#define ONE_MEG 0x100000

struct devstruct devtblinit[] = {

    /*
     * Network
     */
    DEV_NETWORK_ID, "network", (char *)0, 0,
    (char *)0, (unsigned)0, (int (*)())0,
    FILE_ACCESS_UNKNOWN,  

    /*
     * Boot flash SIMM
     */
    default_bootdev_id, "boot flash", (char *)KSEG1_ADRSPC_FLASH_BASE, 0,
    (char *)0, (unsigned)DEV_FS_CAPABLE, (int (*)())bflash_siz,
    FILE_ACCESS_FLASH, 

    /*
     * PCMCIA Slot 0
     */
    DEV_SLOT0, "PCMCIA slot 0", (char *)ADRSPC_PCMCIA_WIN0_BASE, 0, 
    (char *)0, (unsigned)DEV_FS_CAPABLE, slot0_size,
    FILE_ACCESS_SLOT0,

    /*
     * PCMCIA Slot 1
     */
    DEV_SLOT1, "PCMCIA slot 1", (char *)ADRSPC_PCMCIA_WIN1_BASE, 0,
    (char *)0, (unsigned)DEV_FS_CAPABLE, slot1_size,
    FILE_ACCESS_SLOT1, 

    /*
     * EPROM
     */
    DEV_EPROM_ID, "eprom", (char *)&banner_string, 0,
    (char *)(KSEG1_ADRSPC_PROM_END), (unsigned)DEV_FS_CAPABLE, promscan,
    FILE_ACCESS_ROM, 

    /*
     * Main Memory (KSeg1)
     */
    mon_main_mem_id, "main memory (kseg1)", (char *)ADRSPC_K1BASE, 0,
    (char *)0, (unsigned)DEV_IS_MEMORY, (int (*)())sizemainmem,
    FILE_ACCESS_UNKNOWN, 

    /*
     * Main Memory (KSeg0)
     */
    DEV_KMEM0_ID, "main memory (kseg0)", (char *)ADRSPC_K0BASE, 0,
    (char *)0, (unsigned)DEV_IS_MEMORY, (int (*)())sizemainmem,
    FILE_ACCESS_UNKNOWN, 

    /*
     * Main Memory (kuseg)
     */
    DEV_KMEMU_ID, "main memory (kuseg)", (char *)ADRSPC_KUBASE, 0,
    (char *)0, (unsigned)DEV_IS_MEMORY, (int (*)())sizemainmem,
    FILE_ACCESS_UNKNOWN, 
};

#define NUMDEVSTRUCTS (sizeof(devtblinit)/sizeof(struct devstruct))

#define HALF_MEG      0x00080000

/*
 * Virtual Addres space table
 *
 * Predator ROM monitor and IOS share some of the TLB entries to 
 * allow the monitor to access PCI SRAM and the PCMCIA controller.
 * The following is the TLB entries for the monitor
 */
vspace_tbl_t vspace_4k_c7100[] = 
{

/* First half of PCI SRAM, uncached view, used for pcimem */

{ ADRSPC_PCISRAM, PHY_ADRSPC_PCISRAM, ADDR_BIT32_ONE,
  TLB_PAGESIZE_256K, TLB_256K_SHIFT, 
  TLB_UNCACHED, 1, 1, 1 },

/* Second half of PCI SRAM, uncached view, used for pcimem */

{ ADRSPC_PCISRAM+HALF_MEG, PHY_ADRSPC_PCISRAM+HALF_MEG, ADDR_BIT32_ONE,
  TLB_PAGESIZE_256K, TLB_256K_SHIFT, 
  TLB_UNCACHED, 1, 1, 1 },

/* PCMCIA Controller registers (PCI IO space, so set bit 32 */

{ ADRSPC_PCMCIA_REGS, PHY_ADRSPC_PCMCIA_REGS, ADDR_BIT32_ONE,
  TLB_PAGESIZE_4K, TLB_4K_SHIFT, 
  TLB_UNCACHED, 1, 1, 1 },

/* PCMCIA Slot 0 windows (8meg total, uncached) */

{ ADRSPC_PCMCIA_SLOT0, ADRSPC_PCMCIA_SLOT0, ADDR_BIT32_ZERO,
  TLB_PAGESIZE_16M, TLB_16M_SHIFT, 
  TLB_UNCACHED, 1, 1, 1 },

/* PCMCIA Slot 1 windows (8meg total, uncached) */

{ ADRSPC_PCMCIA_SLOT1, ADRSPC_PCMCIA_SLOT1, ADDR_BIT32_ZERO,
  TLB_PAGESIZE_16M, TLB_16M_SHIFT, 
  TLB_UNCACHED, 1, 1, 1 }

};

/* 
 * mask to turn main memory address into a physical address 
 */
unsigned long phymemmask = 0x1fffffff;

/* 
 * our working copy of the device table in RAM 
 */
struct devstruct devtbl[NUMDEVSTRUCTS];
struct devstruct *memdevp;
int 		 numdevstructs = NUMDEVSTRUCTS;

/*
 * pred_tlb_init:
 * Initialize the required TLB entries
 */
static void pred_tlb_init (void)
{
    vspace_tbl_t *vspace;
    struct tlb_t tlb_e;
    ulong paddr, i;

    vspace = vspace_4k_c7100; 
    for (i=0; i<5; i++, vspace++) {

	/*
	 * Setup page size and vaddr
	 */
	tlb_e.pmask.mask = vspace->page_mask;
	tlb_e.hi.vpn2 = (vspace->vaddr >> TLB_HI_VPN2SHIFT);

	/*
	 * Setup paddr
	 */
	paddr = (vspace->paddr >> (TLB_HI_VPN2SHIFT - 1 + vspace->pshift));
	tlb_e.lo0.pfn = paddr << vspace->pshift;
	tlb_e.lo1.pfn = (paddr + 1) << vspace->pshift;
	
	/*
	 * Or in the settings for paddr [35..32]
	 */
	tlb_e.lo0.pfn |= (vspace->paddr_a35_a32 << ADDR_BIT35_32_SHIFT);
	tlb_e.lo1.pfn |= (vspace->paddr_a35_a32 << ADDR_BIT35_32_SHIFT);

	/*
	 * Setup page characteristics
	 */
	tlb_e.lo0.c = tlb_e.lo1.c = vspace->cache;
	tlb_e.lo0.d = tlb_e.lo1.d = vspace->dirty;
	tlb_e.lo0.v = tlb_e.lo1.v = vspace->valid;
	tlb_e.lo0.g = tlb_e.lo1.g = vspace->global;
	settlb(i, &tlb_e);
    }
}

/*
 * pci_config:
 * Perform a PCI configuration write cycle. Only bus 0.
 */
static void pci_config (uchar dev, uchar reg, ulong value, uchar rev_level)
{
    int internal = 0;
    ulong temp = 0;

    temp |= GT64010_CONFIG_ENABLE;
    temp |= ((dev & 0x1f) << GT64010_DEVNUM_SHIFT);
    if (!dev) {
	/*
	 * This is an access to the gt64010. Work arround bug where by
	 * the register offset needs to be shifted by 2. Also mark internal.
	 */
	if (rev_level == GT64010_REV1) {
	    temp |= (reg << (GT64010_REGNUM_SHIFT+2));
	} else {
	    temp |= (reg << GT64010_REGNUM_SHIFT);
	}
	internal = 1;
    } else {
	/*
	 * This is an access to an external device
	 */
	temp |= (reg << GT64010_REGNUM_SHIFT);
    }

    /*
     * Write the config register
     */
    gt_ptr->pci_conf_addr = temp;
    flush_all_wb();

    /*
     * Write the data. If it is not internal, then we must byte swap the 
     * data as the GT64010 will byte swap the data going out the PCI bus.
     */
    if (!internal) {
	value = (((value >> 24) & 0x000000ff) | 
		 ((value >> 8) & 0x0000ff00) |
		 ((value << 8) & 0x00ff0000) |
		 ((value << 24) & 0xff000000));
	gt_ptr->pci_conf_data = value;
    } else {
	gt_ptr->pci_conf_data = value;
    }
    flush_all_wb();
}

/*
 * pred_pci_init:
 * Init the GT64010's PCI IO memory range and configure up the 
 * PCMCIA controller to respond to its IO mapped PCI IO address.
 */
static void pred_pci_init (uchar gt64010_rev)
{
    /*
     * Reset the PCMCIA device
     */
    KSEG1_PRED_FPGA->io_ctrl_reg &= ~PCMCIA_RESET;
    delay_10us(2);

    /*
     * Program the GT64010 to accept addresses 0x1,00000000 to
     * 0x1,0000FFFF as PCI IO addresses.
     */
    gt_ptr->pci_io_low = GT64010_PCI_IO_LOW_INIT;
    gt_ptr->pci_io_high = GT64010_PCI_IO_HIGH_INIT;

    /*
     * Program the pci_cmd register in the GT64010
     */
    gt_ptr->pci_cmd = GT64010_PCI_CMD_INIT;

    /*
     * Program the GT64010 to be a PCI master
     */
    pci_config(0, GT64010_STAT_CMD_REG_OFFSET, 0x04000000, gt64010_rev);

    /*
     * Config the PCMCIA controller to respond to its address
     * (IO mapped PCI bus IO cycle at PCMCIA_REGS_OFFSET).
     */
    pci_config(PCI_PCMCIA_DEV_NUM, PCI_COMMAND_REG_OFFSET,
	       CLPD6279_COMMAND_SETTINGS, gt64010_rev);
    pci_config(PCI_PCMCIA_DEV_NUM, PCI_IO_BASE_OFFSET, 
	       PCMCIA_REGS_OFFSET, gt64010_rev);
}

/*
 * Initialize the hardware
 *
 */
void init (int reset_type, unsigned long *frame)
{
    int i, conf;
    long *mptr, expected, readval, val1, val2;
    uchar gt64010_rev;
    char *cp;

    /* Sync up different voltages */
    delay_10us(100000);

    /*
     * Enable the UART and Show the LEDs
     */
    KSEG1_PRED_FPGA->led_reg &= LEDS_ON;
    delay_10us(100);
    KSEG1_PRED_FPGA->led_reg |= LEDS_OFF;

    /*
     * Initialize the timer control register and confirm.
     * Disable watchdog timer.
     */
    KSEG1_PRED_FPGA->led_reg = (~5 & LEDS_OFF);	/* stage 5 of testing */
    gt_ptr->counter_ctrl = 0;
    if (gt_ptr->counter_ctrl) {
	asm("    li    $4,5");            	/* error 5 */
	asm("    j     lit_errorLEDs");
    }
    KSEG1_PRED_FPGA->led_reg = (~6 & LEDS_OFF); /* stage 6 */

    /*
     * Check the revision level of the gt64010
     */
    gt64010_rev = ralib_check_gt64010_rev();
    if (gt64010_rev != GT64010_REV1) {
	
 	/*
 	 * We are on a rev 2 gt64010. See if we can run packed NVRAM
 	 */
 	if ((KSEG1_PRED_FPGA->flash_simm_reg & HWREV_MASK) >=
 	    IOCARD_PACKED_NVRAM_REV) {
 	    
 	    /*
 	     * The IO card fpga rev supports packed NVRAM
 	     */
  	    KSEG1_PRED_FPGA->io_ctrl_reg |= NVRAM_PACKED;
 	    
 	    /*
 	     * Reprogram the device bank 2 param to 8 bit wide device
 	     */
 	    gt_ptr->device_bank2_param = DEVICE_BANK2_PARAM_INIT;
  	}
    }

    /*
     * If NVRAM is valid get our virtual config register
     * else use the default
     */
    if (NV_VALID_CONFREG)
	conf = nv_read_config();
    else 
	conf = DEFAULT_CONFIG;

    /*
     * Initialize the UART
     */
    i = ((conf & CONSOLE_MASK) >> CONSOLE_SHIFT); 
    uartaddr->a.aux2 = 0;	/* Turn off interrupts (IMR=0) */
    uartaddr->a.command = RESETMRPTR;
    uartaddr->a.mode = CHARERROR | PRTYOFF | DBITS8;
    uartaddr->a.mode = MODENORMAL | STOP16X16;
    uartaddr->a.aux1 = BRGSET1 | TIMXTAL;
    uartaddr->a.state = (RXCLOCK * UARTspeeds[i]) | (TXCLOCK * UARTspeeds[i]);
    uartaddr->a.command = RESETDBREAK|STP_TXENABLE|STP_RXENABLE;

    uartaddr->b.aux2 = 0;	/* All outputs are driven by OPR */
    uartaddr->b.aux3 = CDA;	/* Set Carrier detect on Console port */
    initaux(); 

    KSEG1_PRED_FPGA->led_reg = LEDS_OFF;      /* Reset all LEDs to OFF */

    /*
     * See comment about reset_type at beginning of this routine.
     */
    if (reset_type == RESET_COLD || reset_type == RESET_WARM) {
	PUTSTRING(cp, banner_string);

    } else {

	/*
	 * We got rebooted by the watchdog, so memory is OK
	 * We pass reset_type to this subroutine even though
	 * it is not used there.  This is done to keep the 
	 * compiler from moving reset_type and frame around
	 * in registers. See comment about reset_type at
	 * beginning of this routine.
	 */
	init_soft_reset(reset_type, frame, gt64010_rev);
	goto exit_init;
    }

    /*
     * Now run a memory test of lower RAM.
     *
     * data = address : Check address aliasing
     * AA's/55's : Checkerboard
     * 55's/AA's : Inverse checkerboard
     * Note, memory range must be divisable by 8
     */
    for(i=0; i<NUM_MTEST_VALS; i++) {
	if (conf & DIAG_MASK) {
	    PUTSTRING(cp,"\r\nTesting lower main memory - ");
	    PUTSTRING(cp, mtestvals[i].name);
	}
	val1 = mtestvals[i].val1;
	val2 = mtestvals[i].val2;
	for( mptr = RAM_TEST_START; mptr < RAM_TEST_END; ) {
	    if(i == 0) {  /* test 0 is "data = address" test */
		*mptr++ = (long)mptr;
	    } else {
		*mptr++ = val1;
		*mptr++ = val2;
	    }
	}
	for( mptr = RAM_TEST_START; mptr < RAM_TEST_END; mptr++) {
	    if( i == 0 )  {
		/* test 0 is "data = address" */
		expected = (long)mptr;
	    } else {
		if((long)mptr & 4)
		    expected = val2;
		else 
		    expected = val1;
	    }
	    readval = *mptr;
	    if( readval != expected ) {
		PUTSTRING(cp,"\r\nBad RAM at location 0x");
		PUTHEXNUM((long)mptr);
		PUTSTRING(cp,": wrote 0x");
		PUTHEXNUM(expected);
		PUTSTRING(cp,", read 0x");
		PUTHEXNUM(readval);
		asm("    li   $4,7");  /* error number 7 */
		asm("    j     lit_errorLEDs");
	    }
	}
    }

    /* 
     * Clear the lower 16K for cache initialization.
     */
    if (conf & DIAG_MASK) {
	PUTSTRING(cp, "\r\nClearing lower 16K memory for cache "
		  "initialization");
    }
    val1 = 0;
    for (mptr = (long *)KSEG1_ADRSPC_RAM; 
	 mptr < (long *)(KSEG1_ADRSPC_RAM + CACHE_SIZE); ) {
	*mptr++ = val1;
    }

    if (conf & DIAG_MASK) {
	PUTSTRING(cp,"\r\nClearing bss ");
    }
    val1 = 0;
    for(mptr = (long *)&_fbss; mptr < (long *)&end; ) {
	*mptr++ = val1;
    }

    /*
     * Save the reset type
     */
    reset_type_sav = reset_type;  /* save this info in .bss */

    /*
     * If this is a cold boot, reset our autoboot
     * state machine to zero in NVRAM.
     */
    if (reset_type == RESET_COLD) {
	clr_autoboot_state();
    }

    reset_io();

    /*
     * It is OK to use the stack & bss now!
     * (don't need to use reset_type any more,
     * use reset_type_sav in .bss)
     * Get nvmonvars from NVRAM
     */
    if (conf & DIAG_MASK) {
	puts("\nReading monitor variables from NVRAM ");
    }
    nvraminit();
    envflag = INPONCT;

    if (reset_type_sav == RESET_COLD) {
	log_sys_ret(EXC_RESET, (r4k_context_t *)0);
    }

    /*
     * Must initialize the processor
     */
    if (conf & DIAG_MASK) {
	puts("\nEnabling interrupts ");
    }
    sys_state = MONITOR_RUNNING;
    init_exceptions();

    if (conf & DIAG_MASK) {
	puts("\nInitializing TLB ");
    }
    init_tlb();

    if (conf & DIAG_MASK) {
	puts("\nInitializing cache ");
    }
    init_cache();

    if (conf & DIAG_MASK) {
	puts("\nInitializing required TLB entries");
    }
    pred_tlb_init();
    
    if (conf & DIAG_MASK) {
	puts("\nInitializing PCI subsystem");
    }
    pred_pci_init(gt64010_rev);

    /*
     * Initzilaze cpu type
     */
    cpu_type = CPU_C7100;

    /*
     * Initialize main memory
     */
    if (conf & DIAG_MASK) {
	printf("\nInitializing main memory");
    }
    sizemainmem();

    if (conf & DIAG_MASK) {
	puts("\nConfig DRAM parity check");
    }
    detect_parity();
    config_dram_parity();

    /* enable cache exception after determining dram parity */
    setcp0_sreg(getcp0_sreg() & ~SREG_DE);

    /*
     * Initialize shared packet SRAM
     */
    if (conf & DIAG_MASK) {
	printf("\nInitializing shared memory");
    }
    sizeshmem();

    /*
     * Size NVRAM
     */
    if (conf & DIAG_MASK) {
	printf("\nSizing NVRAM");
    }
    size_nvram();

    /*
     * Initialize the PCMCIA flash card support
     */
    if (conf & DIAG_MASK) {
	printf("\nInitializing PCMCIA controller");
    }
    ralib_vector_table_ptr = ralib_init();

    /*
     * We are done
     */
    if (conf & DIAG_MASK) {
	puts("\nExiting init\n");
    }

exit_init:
    /*
     * Return to __init_done in pred_start.S
     */

}

/*
 * init_soft_reset : Set up context after a watch dog soft reset
 * Note that reset type is passed to this routine even though it
 * is not used here.  See relevant comments in the routine init
 * (above). Do not remove reset_type parameter even though it is not used.
 */
static void init_soft_reset (int reset_type, unsigned long *frame,
			     uchar gt64010_rev)
{
    r4k_context_t *ptr = (r4k_context_t *)frame;
    unsigned char reset_reason;
    int reason;

    reset_io();

    reset_reason = ~get_reset_reason();

    if (reset_reason & SW_NMI) {
	printf("\n*** Software NMI ***\n");
	reason = EXC_SW_NMI;
    } else if (reset_reason & PUSH_BUTTON_NMI) {
	printf("\n*** PUSH BUTTON NMI ***\n");
	reason = EXC_RESET;
    } else if (reset_reason & SW_WATCHDOG_RST) {
	printf("\n*** Watch Dog Timeout ***\n");
	reason = EXC_WATCHDOG;
    } else if (reset_type == RESET_WARM) {
	printf("\n*** Reset or Reload Command ***\n");
	reason = EXC_RESET;
    } else {
	printf("\n*** Unknown Soft Reset ***\n");
	reason = EXC_UNKNOWN_RESET;
    }

    if (frame) {
	printf("PC = 0x%.8x, SP = 0x%.8x\n", ptr->err_epc, 
	       ptr->regs[SP_REGNUM]);
    }

    /*
     * Re-init the system
     */
    init_exceptions();
    init_tlb();
    init_cache();
    pred_tlb_init();
    pred_pci_init(gt64010_rev);

    if (frame) {
	copy_context(frame, 1);
	/*
	 * Log the info for future use
	 */
	log_sys_ret(reason, frame);
    }
    sys_state = MONITOR_RUNNING;
    detect_parity();
    config_dram_parity();

    /* enable cache exception after determining dram parity */
    setcp0_sreg(getcp0_sreg() & ~SREG_DE);
}

/*
 * flush_io_wb:
 * Flush the IO write buffer
 */
int flush_io_wb (void)
{
    return (*(int *)(KSEG1_ADRSPC_PROM));
}

/*
 * flush_all_wb:
 * Flush all write buffers. 
 */
void flush_all_wb (void)
{
    flush_io_wb();
}

/*
 * reset_io:
 * Reset all I/O subsystem to a quiescent state.
 */
void reset_io (void)
{
    volatile ulong *ptr;

    /*
     * Power off all port adaptors and mask all interrupts
     */
    KSEG1_C7100_MP_FPGA->pkt_int_enbl_lsw = 0;
    flush_all_wb();
    KSEG1_C7100_MP_FPGA->mgmt_int_enbl_lsw = 0;
    flush_all_wb();
    KSEG1_C7100_MP_FPGA->err_int_enbl_lsw = 0;
    flush_all_wb();
    KSEG1_C7100_MP_FPGA->pa_ctrl_lsw = 0;
    flush_all_wb();

    /*
     * Reset MB0
     */
    ptr = (volatile ulong *)(ADRSPC_MB0_RESET);
    *ptr = 0;			        /* assert reset */
    delay_10us(10);
    *ptr = 0;				/* deassert reset */
    delay_10us(10);

}

/*
 * save_reset_reason:
 * Save the reset reason register into a global variable 
 */
void save_reset_reason (void)
{
    reason_reg = KSEG1_PRED_FPGA->reset_reason_reg;
}

/*
 * get_reset_reason:
 * Return the value of reset reason register
 */
unsigned char get_reset_reason (void)
{
    return(reason_reg);
}

/*
 ***********************************************************************
 * Timer routines
 ***********************************************************************
 */

/*
 * timer_calibrate:
 * Calibrate the timer. Just return the parameter.
 */
long timer_calibrate (long t) 
{
    return(t);
}

/*
 * time_it:
 * Just return the parameter.
 */
int time_it (int cnt)
{
    return(cnt);
}

/*
 * wastetime:
 * Waste away n microseconds
 */
void wastetime (long usec)
{
    unsigned long before, after, time;

    before = getcp0_count();
    for (;;) {
	after = getcp0_count();
	if (after < before) {
	    time = (0xffffffff - before) + after;
	} else {
	    time = after - before;
	}
	time = (time >> 6);
	if (time >= usec) {
	    return;
	}
    }
}

/*
 * tcal:
 */
int tcal (int argc, char *argv[])
{
    return(0);
}

/*
 * disable_watchdog:
 * Turn watchdog off
 */
void disable_watchdog (void)
{
    KSEG1_PRED_FPGA->led_reg &= ~WATCHDOG_ENABLE;
    flush_io_wb();
}
    
/*
 * test_watchdog: 
 * Test the watchdog reboot mechanism
 */
int test_watchdog(int argc, char *argv[])
{
    long delaycnt, i;

    if (argc != 1) {
	printf("usage: %s\n", argv[0]);
	return(1);
    }

    delaycnt = timer_calibrate(1000);	/* 1ms calibrated time */

    /*
     * Initialize the watchdog and let it run out, Should reboot us.
     */
    printf("Starting the watchdog ... should reboot in 32ms\n");
    KSEG1_PRED_FPGA->led_reg |= WATCHDOG_ENABLE;
    flush_io_wb();

    /*
     * waste aways 100ms
     */
    for (i=0; i<100; i++) {
	wastetime(delaycnt);
    }
    
    /*
     * we should never get here
     */
    printf("%s: error, the watchdog did not reboot us, ct0= %d\n",
	   argv[0], i);
    return(1);
}

/*
 * abort_alarm : Stub to jump back to the monitor when
 * an un-caught alarm signal occurs. longjmps with a value of 4.
 */
static void abort_alarm (void)
{
    setcp0_sreg( (getcp0_sreg() & ~(SREG_ERL|SREG_EXL)) | SREG_IE );
    sys_state = MONITOR_RUNNING;
    cache_flush(0, -1);
    if (monjmpptr) 
	longjmp(monjmpptr,4);
}

/*
 * timerint:
 * Timer interrupt handler
 */
long timerint (r4k_context_t *frame)
{
    extern int (*sigalrmfcn)();

    /*
     * Write the Clear Timer Interrupt Register to ack the interrupt
     */
    flush_io_wb();
    intimerint++;

    if (!alarm_setting)
	return(0);                 /* Alarm function not initialized */
    alarm_count--;
    if (alarm_count == 0) {   	   /* Time to send alarm */
	alarm_setting = 0;         /* clear the alarm */
	t_disable();
   	switch ( (int)sigalrmfcn ) {
	case SIG_DFL:
	    break;
	case SIG_IGN:
	    return(0);
	default:
	    if ((*sigalrmfcn)(frame) == (int)SIG_DFL) 
		break;
	    return(0);
	}
	/* default action terminates program */
	if (monjmpptr) {
	    frame->epc = (unsigned long)abort_alarm;
	    frame->epc_hi = KSEG_MSB_ADDR;
	}
    }
    return(0);
}

/*
 * scopetimer:
 * Timer scope loop
 */
void scopetimer (void)
{
    unsigned long ticks1 = 40000;
    unsigned long ticks2 = 40000;

    printf("scope triggers every %d and %d usecs.\n",ticks1,ticks2);
    ticks1 = timer_calibrate(ticks1);
    ticks2 = timer_calibrate(ticks2);
    while(1) {
	wastetime(ticks1);
	wastetime(ticks2);
    }
}

/*
 * disable timer and return possible count
 */
int t_disable (void)
{
    int count = 0;

    flush_io_wb();
    return(count);
}
 
/*
 * t_enable:
 * enable timer 2 and load with count
 */
void t_enable (unsigned short count)
{
    flush_io_wb();
}

/*
 * alarm : Setup the alarm mechanism
 * If cnt == 0, disable the alarm mechanism, else initialize the mechanism
 * and set alarm_setting to cnt. cnt is the number of 1ms intervals that
 * pass before the sigalrmfcn signal will be called. Return the amount of
 * time left before the alarm would go off.
 */
int alarm (int cnt)
{
    int tmp = alarm_count;

    t_disable();              /* Ensure mutual exclusion on variables */
    alarm_setting = cnt;      /* Initialize the alarm settings */
    alarm_count = cnt;
    if (cnt) {
	t_enable(ALARM_FREQUENCY);  /* start the timer */
    }
    return(tmp);
}

/*
 * pause - stop until alarm signal or a user interrupt
 */
int pause(void)
{
    if (alarm_count) {
	while(alarm_count)
	    ;
    } else {
	while(1)
	    ;
    }
    return(-1);  /* as per man pages */
}

/*
 * sleep - do nothing for the specified number of milliseconds
 * This routine tries to do the right thing if the alarm is
 * already set.
 */
void sleep (int msecs)
{
    int tmp;
    SIG_PF sigsav;

    tmp = t_disable();     /* ensure mutual exclusion on variables */
    if (alarm_count && alarm_count < msecs) {
	/* alarm is currently in use */
	msecs -= alarm_count;
	t_enable(tmp ? tmp : 1);
	pause();
    }
    sigsav = signal(SIGALRM, SIG_IGN);
    tmp = alarm(msecs);
    pause();
    signal(SIGALRM, sigsav);
    if (tmp > msecs) {
	alarm(tmp - msecs);
    } else {
	alarm(0);
    }
}

/*
 ***********************************************************************
 * LED routines
 ***********************************************************************
 */

/*
 * testleds:
 */
testleds(int argc, char *argv[])
{
    long value;
    jmp_buf local_buf, *savjmp;

    if (argc != 2) {
	printf("usage: %s value\n",argv[0]);
	return(1);
    }
    if (getnum(argv[1],10,&value) == 0) 
	return(1);
    printf("Calling errleds() with value %d\n"
	   "enter <break> to exit\n",value);
    savjmp = monjmpptr;
    if (!setjmp(local_buf)) {
	monjmpptr = &local_buf;
	errleds(value);
    } else {
	monjmpptr = savjmp;

	/*
	 * make sure runlamp is off
	 */
    }
    return(0);
}

/*
 ***********************************************************************
 * Flash routines
 ***********************************************************************
 */

#define FLASH_VPP_ON()     						\
    (KSEG1_PRED_FPGA->io_ctrl_reg &= ~FLASH_PROG)

#define FLASH_VPP_OFF()    						\
    (KSEG1_PRED_FPGA->io_ctrl_reg |= FLASH_PROG)

extern long tenmsecs;

/*
 * flash_prog_en:
 * Turn on flash programming voltage
 */
void flash_prog_en (void)
{
    int i;

    FLASH_VPP_ON();
    flush_io_wb();
    /*
     * Wait 100 ms
     */
    for (i=0; i<10; i++) 
	wastetime(tenmsecs);
}

/*
 * flash_prog_dis:
 * Turn off flash programming volatge
 */
void flash_prog_dis (void)
{
    int i;

    FLASH_VPP_OFF();
    flush_io_wb();
    /*
     * Wait 100 ms
     */
    for (i=0; i<10; i++) 
	wastetime(tenmsecs);
}

/*
 * bootflash_addr:
 * Set the address of boot flash and return one or zero if it does
 * not exist on this platform.
 */
int bootflash_addr (long *addrptr, int *fcc_install)
{
    *fcc_install = 1;
    *addrptr = (long)KSEG1_ADRSPC_FLASH_BASE;
    return(1);
}

/*
 * bflash_siz:
 * Return the size of bootflash
 */
long bflash_siz (void)
{
    int fcc_install;
    struct flash_dvr_info fdi[MAXFLASHSETS];
    
    if(!bootflash_addr(&fdi[0].flash_base, &fcc_install)) {
	printf("boot flash not available\n");
	return(0);
    }
    return(flash_siz(fdi));
}

/*
 * get_flash_base:
 * Return the base address of system flash - none
 */
unsigned get_flash_base (void)
{
    return(0xffffffff);
}

/*
 ***********************************************************************
 * Cookie routines
 ***********************************************************************
 */
static struct cookie virtual_cookie;

struct cookie *get_cookie (void) 
{
    strcpy((char *)&virtual_cookie, "I am fine!");
    return(&virtual_cookie);
}

/* End of Module */
