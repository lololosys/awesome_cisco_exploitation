/* $Id: init.c,v 3.1.58.1 1996/05/09 13:54:24 rbadri Exp $
 * $Source: /release/112/cvs/Xboot/src-68-alc/init.c,v $
 *------------------------------------------------------------------
 * init.c -- Synergy ATM line card IOS ROM monitor hardware initialization
 *	     (based on c3000)
 *
 * April 13, 1995, Rick Lowe
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: init.c,v $
 * Revision 3.1.58.1  1996/05/09  13:54:24  rbadri
 * Branch: California_branch
 * Synergy features to the mainline.
 *
 * Revision 3.1.88.1  1996/04/27  06:13:22  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.1.78.1  1996/04/08  01:34:53  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.1.64.2  1996/03/23  04:43:14  rlowe
 * Fix includes to look inside wbu top-level directory.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.1.64.1  1996/03/22  09:07:18  rlowe
 * Non-sync of Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.1.50.1  1996/03/03  21:19:18  rlowe
 * Apply SYNALC content to merge branch.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 3.1.48.1  1996/03/02  01:07:16  rlowe
 * Non-sync sync to mainline tag V111_1_0_3.
 * Branch: Synalc3_Cal_V111_1_0_3_branch
 *
 * Revision 3.1.32.1  1996/02/29  07:42:18  rlowe
 * First pass non-sync sync for Synergy ATM line card to V111_0_16.
 * Branch: Synalc3_Cal_V111_0_16_branch
 *
 * Revision 3.1  1995/11/09  09:08:51  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1.62.1  1996/02/03  06:11:55  rlowe
 * Non-sync sync of Synalc_catchup_branch to V111_0_5.
 * Branch: Synalc_catchup_pretal_branch
 *
 * Revision 2.1.44.4  1995/10/20  05:43:44  rlowe
 * Use green/off when testing link leds.
 *
 * Branch: Synalc2_LE2_V110_1_0_3_branch
 *
 * Revision 2.1.44.3  1995/10/20  03:51:21  rlowe
 * Clean up kernel image loading now that BCS checksums are in place.
 * Branch: Synalc2_LE2_V110_1_0_3_branch
 *
 * Revision 2.1.44.2  1995/10/13  05:24:22  rlowe
 * Enable and use BCS checksums in PROM Boot.
 * Branch: Synalc2_LE2_V110_1_0_3_branch
 *
 * Revision 2.1.44.1  1995/09/11  01:39:51  rlowe
 * Non-sync sync of Synalc2_branch to LE2_V110_1_0_3_branch,
 * yielding Synalc2_LE2_V110_1_0_3_branch.
 *
 * Branch: Synalc2_LE2_V110_1_0_3_branch
 *
 * Revision 2.1.28.2  1995/09/02  00:01:56  rlowe
 * Fix includes so that makefile can give search paths, rather than
 * hardcoding them.
 *
 * Branch: Synalc2_branch
 *
 * Revision 2.1.28.1  1995/08/22  18:18:45  lcheung
 * move files from synergy_atmlc_branch to synalc2_branch
 * Branch: Synalc2_branch
 *
 * Revision 2.1  1995/06/07  19:15:14  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 * Revision 1.1.2.4  1995/06/15  09:36:15  rlowe
 * Move boot download functionality to boot-1.  Condition PMON to SARs
 * on MFG jumper instead of DBG jumper.  Condition boot download on
 * SPARE jumper until checksums are done.  Cleanup makefiles.
 * Lock s-bus when resetting BIGA/SARs to avoid hung frame buffer.
 * Explicitly hold/release reset on BIGA.  Force LCP reset whenever
 * LCP driver re-initialized.
 *
 * Branch: Synergy_ATMlc_branch
 *
 * Revision 1.1.2.3  1995/06/09  09:19:49  rlowe
 * Port and integrate NMP (CBI) flash driver (Intel, AMD flash simms) to
 * ACP.
 * Branch: Synergy_ATMlc_branch
 *
 * Revision 1.1.2.2  1995/05/17  00:27:35  rlowe
 * Support downloading to Rev 2 ATM line cards.  Move downloaded code base
 * from $40001000 to $40004000.
 * Branch: Synergy_ATMlc_branch
 *
 * Revision 1.1.2.1  1995/04/19  02:28:49  rlowe
 * Merge Synergy ATM line card code (from 10.2) into IOS 10.3.
 *
 * Branch: Synergy_ATMlc_branch
 *
 * Revision 1.1  1995/04/14 05:22:06  rlowe
 * Placeholders for IOS port to Synergy ATM line card.
 *
 *------------------------------------------------------------------
 * Revision 1.2  1995/03/03  08:00:24  rlowe
 * Change to use "DEBUG" jumper, rather than "FASTBOOT" jumper, to decide
 * whether to autoboot the IOS Kernel or fall into the IOS Boot monitor.
 *
 * Revision 1.1.1.1  1995/02/21  19:33:46  kchristi
 * creating new ios directory
 *
 * Revision 1.5  1995/02/18  13:18:22  rlowe
 * ACP memory carving of DRAM has 3.5 Mb of processor RAM and 128 Kb IOMEM
 * plus 384 Kb for other stuff.  Keep IOS Boot from puking because 3.5 Mb
 * is a weird (non-even, non-power-of-2) number.
 *
 * Revision 1.4  1995/02/18  12:05:37  rlowe
 * Have IOS IOMEM be in ACP DRAM area, between processor ram chunk and
 * NTC DMA area.  Keep ACP Frame Buffer area hidden from IOS.
 *
 * Revision 1.3  1995/02/14  07:15:38  rlowe
 * Add some new BCS service requests to extract base address / size info
 * from BCS headers.  Also adjust IOS Kernel image entry point address to
 * reflect IOS Boot deleting a.out header (32 = $20 bytes).
 * Make more use of IOS Boot support for a.out images.
 *
 * Revision 1.2  1995/02/02  02:08:37  rlowe
 * Latest merge changes from old code tree to current 10.2/nac1.
 * Lots of new #defines and macro usage.
 *
 * Revision 1.1  1995/02/01  10:59:55  rlowe
 * Ram's initial merge of ALC IOS-Boot code (from older 10.2 tree).
 *
 * Revision 1.3  1995/01/27  09:29:24  rlowe
 * More test/changes to try getting an initial (auto)booting prom.
 *
 * Revision 1.2  1995/01/26  12:35:02  rlowe
 * Add some hacks to try out integration with BCS (for looking up IOS kernel
 * code in flash).
 *
 * Revision 1.1  1995/01/11  23:04:36  rlowe
 * Initial port of IOS Boot to ACP.  Got Monitor up.
 *
 * Revision 1.1.1.1  1994/08/31  22:16:50  rahuja
 * IPS ver 10.2 snapshot
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "../defs.h"
#include "../reason.h"
#include "../m68vectors.h"
#ifndef	SYNALC
#include "../i8254.h"
#endif
#include "../prom2.h"
#include "../extern.h"
#include "../stdio.h"
#include "../cpu.h"
#include "../test.h"
#include "../caps.h"
#include "pcmap.h"         /* SYNALC_ADD */
#include "nonvol_x2444.h"

#include "../flh.h"	   /* SYNALC_ADD */ 
#include "../flh_log.h"

#include "../../wbu/cat5k/alc_include/mfp_acp_defs.h"
#include "../../wbu/cat5k/alc_include/bcs_api.h"
#ifdef	RKL
#include "../../wbu/cat5k/alc_include/bdg_if.h"
#endif

#define	RAM_VECTOR_TBL	(EVEC_RESETSSP)

extern long etext;
extern char int_vecs[];
extern volatile short buserrcnt; /* Bus error count */
extern u_short flags;

void parity_checker_handler();
void reset_io();

struct regpair *console = (struct regpair *) &uartaddr->a; /* UART registers */
cookie_t *cookie; /* Address of cookie */
#ifdef	SYNALC
const cookie_t	ACP_cookie = { INTERFACE_1E1Tatmswitch,
				VENDOR_CISCO,
				{0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
				PROCESSOR_SYNALC,
				{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
				{0x00, 0x00, 0x00, 0x00},
				{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
				{COOKIE_MAGIC_0, COOKIE_MAGIC_1},
				{0x00, 0x00},
				1
				} ;
const cookie_t *cookie_boot = & ACP_cookie ;	/* Address of cookie */
#else
cookie_t *cookie_boot = (cookie_t *)ADRSPC_COOKIE; /* Address of cookie */
#endif

uchar nv_cookie[sizeof(cookie_t)];/* Storage for info in serial NVRAM cookie */

#ifdef	SYNALC
#undef	PUNCH_WD	/* Re-use normal definition. */
#define	PUNCH_WD	/* Punch HW watchdog,  always enabled for SYNALC. */ \
	*(volatile u_char *)(ACP_HW_IOREG_BASE + ACP_WRRG_WATCHDOG) = 1 ; \
	*(volatile u_char *)(ACP_HW_IOREG_BASE + ACP_WRRG_WATCHDOG) = 0 ;

#elif	! defined (PUNCH_WD)
#define	PUNCH_WD
#endif


/* eeprom support needed for cancun */
void eeprom_nvwritebyte (byte, base, offset)
     uchar byte;
     uchar *base;
     int offset;
{
    int time;
    uchar *ptr;

    ptr = base + offset;
    *ptr = byte;
    time = 1000;

    while (*ptr != byte) {
	delay(1);
	if (--time <= 0) {
	    printf("\nEEPROM write timeout");
	    break;
	}
    }
}

uchar eeprom_nvreadbyte (base, offset)
     uchar *base;
     int offset;
{
    uchar *ptr;
    uchar data;

    ptr = base + offset;
    data = *ptr;

    return(data);
}


/*
 * This routine is invoked first. It disables all
 * timers, sets up memory mapping, exits from the
 * boot state, and sizes and configures memory. Also
 * obtains a copy of the configuration register, sets up
 * the UARTs and exception vectors, and reenables the timers.
 */

/* SYNALC_ADD */

#define PUT_DIAG_STR(str) \
    if (~conf & DIAG_MASK) { \
       PUTSTRING(cp,str); \
       }


/* 
 * Beware of the cookie monster!!!  This code supports both the cookie prom
 * and the serial NVRAM cookie used by Cristal/Cancun.  
 * The cookie prom address is read to see if it exists.  The value
 * NO_COOKIE_PROM (0xF1), for Cristal, and NO_COOKIE_PROM_CANCUN (0xff), for
 * Cancun, will be returned by the hardware if the prom is missing.  
 * Because we're in boot mode, we put off setting up the cookie
 * structure out of the serial NVRAM until we exit boot mode and test memory.
 * Until we've set up the cookie structure, we must keep checking to 
 * see if the cookie prom is there before we access it.
 */

void init ()
{
    extern char *oemversion, *ciscoversion;
    extern long evec, evece, refreshe_brut;
    extern void buserror(); /* SYNALC_ADD */

    register long startup;	/* MAGIC - see asm code below */
				/* startup *must* be a register variable */
    u_short conf;		/* config parameter from config nvram */
				/* conf  *must* be register variable */
    u_short control;
    u_short rev;
    register long h, i, j, k ;
    register long *et;		/* this *must* be register variable */
    register char *cp;		/* variable used to help print strings */
				/* this *must* be register variable */
    
/* SYNALC_ADD make sure these variables are not on stack and not register 
   vars */

    static volatile ulong flhbuf_start;
    static volatile ulong flhbuf_end;
    /* make sure these vars on not on the stack and are not reg vars */
    static volatile struct _flhlog_end *flhend_ptr;
    static volatile struct _flhlog_begin *flhbegin_ptr;


#ifdef lint
    startup = NULL;
#else
    asm("	.globl sinit");
    asm("sinit:");
    /*
     * Intialize startup from d0, which is initialized by _start() and
     * _restart() in locore.s before entering _sinit().
     */
    asm("	movl	d0,%0" : "=g" (startup)); /* Initialize startup */
#endif

    /* Read the control register to clear any errors and reset all 
     * interface devices. Setting the bit in the control register 
     * results in assertion of the reset input to the chip.
     * Note that the reset must be active for at least 200ns for 
     * the Lance. */
    /*
     * SYNALC reset is done by Primary Boot (not IOS Boot).  Force
     * a hard reset to restart in Primary Boot (see asm.S).  Basic
     * grooming of HW is also done in the Primary Boot, not here.
     */
#ifndef	RKL
    asm("	.globl init_data_bss");
    asm("init_data_bss:");
    PUNCH_WD ;
    {	/* Hack to copy in our C data section, and clear our BSS section. */
	extern long _end, edata, bdata ;

	for (cp = (char *) & etext, et = & bdata ; et < & edata ; ++ et)
	{
		*et = * (long *) cp ;	/* for data */
		cp += 4 ;
	}
	for (; et < & _end ; ++ et)
	{
		*et = 0 ;		/* for bss */
	}
    }
#endif

    asm("	.globl init_reset_int");
    asm("init_reset_int:");

/* SYNCALC_ADD */

#define SYNOPTICS_LEDS(a,b) \
    if (NORMAL_COOKIE_PROM_PRESENT) { \
        if (cookie_boot->vendor == VENDOR_SYNOPTICS) { \
            CLR_SYNOPTICS_LEDS(syn_leds, SYNOPTICS_LED_STEP_ ## a); \
            SET_SYNOPTICS_LEDS(syn_leds, SYNOPTICS_LED_STEP_ ## b); \
        } \
    }


	PUNCH_WD ;

		/* Light some LEDs to indicate life.  Only LEDs visible to  */
		/* a user are on the PMD.  This also serves as a LED check. */
		/* RKL:  do not forget to read/clear the reset reason reg.  */
	*(volatile u_short *)(ACP_HW_IOREG_BASE + ACP_WRRG_PMD_STAT_LED) =
							ACP_PMD_STAT_LED_YEL ;

	*(volatile u_short *)(ACP_HW_IOREG_BASE + ACP_WRRG_PMD_TOP_LED) =
							ACP_PMD_LEVL_LED_GRN ;
	*(volatile u_short *)(ACP_HW_IOREG_BASE + ACP_WRRG_PMD_MID_LED) =
							ACP_PMD_LEVL_LED_GRN ;
	*(volatile u_short *)(ACP_HW_IOREG_BASE + ACP_WRRG_PMD_BOT_LED) =
							ACP_PMD_LEVL_LED_GRN ;


    /* 
     * Case on processor type to use the correct reset register.
     * Set the bits to reset the interface, give the devices 
     * time to respond then clear the reset bits 
     */
    if (NORMAL_COOKIE_PROM_PRESENT) {
	switch (cookie_boot->processor) {
	  case PROCESSOR_CRISTAL:
	  case PROCESSOR_CANCUN:
#ifndef	SYNALC
	    *(volatile u_short *)ADRSPC_RESET |= ALL_NETWORK_RESET;
	    asm("nop");
	    asm("nop");                     /* Can't call delay() yet! */
	    asm("nop");
	    asm("nop");
	    *(volatile u_short *)ADRSPC_RESET &= ~ALL_NETWORK_RESET;
#endif	/* SYNALC */
	    break;
	    
	  case PROCESSOR_SYNALC:
	  default:
	    asm("nop");
	    asm("nop");			/* Can't call delay() yet! */
	    asm("nop");
	    asm("nop");
	    asm("nop");
	    asm("nop");
	    break;
	}
    }
    
    /*
     * Update the parts of the NVRAM config register that are controlled
     * by the physical configuration register:
     */

    asm("	.globl init_phys_config");
    asm("init_phys_config:");

    /*
     * Is the configuration magic value in NVRAM ok?
     */
    conf = *(u_short *)config_magic_ptr;
    if (conf == CONFREG_MAGIC)
	conf = *(u_short *)config_ptr; /* Yes, use config value from NVRAM */
    else
	conf = DEFAULT_CONFIG;	       /* No, use default config value */

    /*
     * Now fold in bits from hardware jumpers:
     *	SYNALC jumpers are active LOW.  Accept either MFG-DIAGS or DEBUG
     *	jumpers to indicate diags mode inside this code.
     */
     if ((! (*(volatile u_char *)(ACP_HW_MFP_BASE + MFP_RO_GPDR) & (MFP_GPI_MFGJMP_)))
#ifndef	SYNALC		/* RKL */
    								 ||
	 (! (*(volatile u_char *)(ACP_HW_IOREG_BASE + ACP_RDRG_DEBUG)
						& (1 << ACP_RRB_JMP_DEBUG_)))
#endif
						)
	conf |= DIAG_MASK;
    else
	conf &= ~DIAG_MASK;

    conf &= ~(WATCHDOG_MASK);		/* SYNALC watchdog always enabled. */

    conf &= ~CONSOLE_MASK;		/* Fix SYNALC dbg console @ 9600 Baud.*/
    conf |= ((BAUDRATE_9600) <<
			(PHYS_CONSOLE_SHIFT + PHYS_TO_VIRT_CONSOLE_SHIFT)) ;

    conf &= ~BOOT_FIELD;		/* Boot bits reflect jumper settings. */
#ifdef	SYNALC
    if (! (*(volatile u_char *)(ACP_HW_IOREG_BASE + ACP_RDRG_DEBUG)
						& (1 << ACP_RRB_JMP_DEBUG_)))
	conf |= 0 ;			/* Indicate to ROM Monitor code. */
    else
#endif
	conf |= 1 ;			/* Indicate to default system code. */

    /* 
     * Assume that if we get NO_COOKIE_PROM_CANCUN in reading from the 
     * cookie_boot pointer that we must be running a cancun platform
     * (CRISTAL returns NO_COOKIE_PROM value here and CANCUN returns
     * NO_COOKIE_PROM_CANCUN.  All other platforms return the contents
     * of the first byte of the cookie prom.
     * Note: This assumption may not be true in the future with other
     * systems that use serial cookies.
     *
     * On cancun, we'll save the conf value and write it later into
     * the config EEPROM.
     */
    if (*(uchar *)cookie_boot != NO_COOKIE_PROM_CANCUN)  {
	*(u_short *)config_ptr = conf;
	*config_magic_ptr = CONFREG_MAGIC;
    }
    conf = ~conf;
    
    /*
     * Now initialize the UARTs themselves and print the version string
     * Appearance of the version string on the console indicates that
     * the UART and timer chips are functional.
     */

#ifndef SYNALC
 SYNOPTICS_LEDS(1,2);
#endif
    /*
     * For SYNALC, UART init is done with timer/MFP init -- see below.
     * It is done through a subroutine call, so wait until later when
     * DRAM has been checked out.  Meanwhile, assume that earlier
     * Boot stages have done some UART/MFP init.
     */
    asm("	.globl init_uart");
    asm("init_uart:");

    asm("	.globl init_version");
    asm("init_version:");
    PUTSTRING(cp,(~conf & OEM_MASK) ? oemversion : ciscoversion);

#ifndef SYNALC
SYNOPTICS_LEDS(2,3);
#endif
    /*
     * Boot State:
     *
     * In boot state (the state of the system after reset) the address
     *	0XXXXXX		reads and executes from address 1XXXXXX
     *			writes onto address 0XXXXXX,
     * in this way it is possible to initialize RAM just after reset.
     * Also, all interrupts including normally non-maskable ones are
     * disabled.
     *
     * Note that CLR instructions should not be used on RAM space in
     * boot state; this is because the CLR instructions does a read cycle
     * before writing.
     */
    /*
     * For SYNALC, Boot code (prom/flash) is always starting at $00xxxxxx
     * while DRAM starts at $40xxxxxx (packet buf mem at $b0xxxxxx).
     * Since we have no DRAM parity protection, don't worry about mem reads.
     * Besides, the early SYNALC Boot has already checked out DRAM for us.
     *								RKL
     */

#ifndef SYNALC
 SYNOPTICS_LEDS(3,4);
#endif

    PUT_DIAG_STR("Exiting boot state - setting bus error vector...");
    asm("	.globl init_exit_boot");
    asm("init_exit_boot:");

    /*
     * If we are a Merlot, set up the system CONTROL2 register for the
     * correct memory size, ROM size, and flash size.
     *
     * We also do this if the cookie prom is missing.  Missing prom means
     * we're a Cristal/Cancun.
     */
    asm("	.globl init_processor");
    asm("init_processor:");

    {
	/*
	 * Initially set up RAM size bits for the maximum possible, 16 MB:
	 */

	/*
	 * See if our CPU RAM is 16 bits wide, or 32 bits wide.
	 * Write a 32 bit value to memory location 0, write the word
	 * after it and see if the first word reads backc ok.
	 * If it does, try writing the complement value, write the 
	 * word after it, then read back the first word, and test again.
	 * Note that this code has to be ugly so that it doesn't touch
	 * DRAM except whatever locations it is using to test memory.
	 * 
	 * At one time delayed here with nop's.  This was to give the data
	 * lines time to float back up to ffffffff's before reading back
	 * the bus.  A better algorithm (and what we use now) is to just
	 * drive the data lines to ffffffff with a specific write.  The 
	 * advantage to this is that it's also non time critical.
	 */
	asm("	.globl init_bus_width");
	asm("init_bus_width:");

	{
	    if (~conf & DIAG_MASK) {
		PUTSTRING(cp,"32 bit wide CPU RAM...");
	    }

	    /*
	     * Since we have set CONTROL2 for 16 MB of RAM, we will not
	     * get a bus error unless there is broken hardware. We will
	     * detect wrap around on smaller memory sizes:
	     */

	    /*
	     * Now, size this 32 bit DRAM, so we can set the size bits
	     * for the address decoding circuit. Compute the number of
	     * 512 KB memory chunks:
	     */
	    asm("	.globl init_wrap");
	    asm("init_wrap:");

	    PUNCH_WD ;

	    i = 0;
	    for (et = (long *)(ADRSPC_RAM) ;
		 et < (long *)(ADRSPC_RAM + (RAMPAGES * PAGESIZE));
		 et += (HALF_MEG/4)) {
		*et = (ulong)et;

		/*
		 * If a later write has overwritten the zero that we first
		 * wrote to location zero, we have wrapped around, and now
		 * know the end of memory:
		 */
		if (*(long *)(ADRSPC_RAM) != ADRSPC_RAM)
		    break;
		i++;
	    }

	    asm("	.globl init_found_wrap");
	    asm("init_found_wrap:");

#ifdef	SYNALC
	    if (~conf & DIAG_MASK)
	    {
		PUTSTRING (cp,"CPU RAM number of 512 KB pages = ");
		PUTHEXNUM (i);
	    }
#else
	    if (~conf & DIAG_MASK)
	     switch(i) {
	      case 2:	/* We found 1 MB of CPU RAM: */
		PUTSTRING(cp,"Found 1 MB CPU RAM...");
		break;
	      case 4:	/* We found 2 MB of CPU RAM: */
		PUTSTRING(cp,"Found 2 MB CPU RAM...");
		break;
	      case 8:	/* We found 4 MB of CPU RAM: */
		PUTSTRING(cp,"Found 4 MB CPU RAM...");
		break;
	      case 16:	/* We found 8 MB of CPU RAM: */
		PUTSTRING(cp,"Found 8 MB CPU RAM...");
		break;
	      case 32:	/* We found 16 MB of CPU RAM: */
		PUTSTRING(cp,"Found 16 MB CPU RAM...");
		break;
	      default:
		PUTSTRING(cp,"Found invalid number of 512 KB pages=");
		PUTHEXNUM(i);
		asm("    jmp lights");
		break;
	     }
#endif
	}
    }

    /*
     * Try to set the bus error vector to ignore bus errors.
     * This is the first time we touch main memory.
     */
#ifndef SYNALC
SYNOPTICS_LEDS(4,5);
#endif

    asm("	.globl init_bev");
    asm("init_bev:");

    PUNCH_WD ;

    /*
     * First, set up the other vectors in RAM also, since we will
     * be changing the VBR to point into RAM instead of ROM:
     */
    buserrhandler = 0;		/* Report bus errors */
    for (et = &evec; et < &evece; et++)
	(RAM_VECTOR_TBL) [et - & evec] = *et ;

    /*
     * Point the bus error vector at a routine that will simply return
     * from the exception, and update the vector base register to point
     * to the new exception table in RAM:
     */
    *EVEC_BUSERR = (long)catchbe;
    asm ("movc %0, vbr" : : "r" ((u_long)(RAM_VECTOR_TBL)));	/* Set VBR */

    /*
     * Make sure that the bus error vector write worked:
     */
    if (*EVEC_BUSERR != (long) catchbe) {
	PUTSTRING(cp,"\r\nBad memory - unable to write low core");
	asm("    jmp lights");
    }

    PUT_DIAG_STR("[OK]\r\n");
    
    /*
     * Now run a more extensive memory test of page zero.
     * Do this only on a hardware reset/powerup.
     * First write all zeros. Never use a clr.l instruction
     * at this point.
     */

#ifndef SYNALC
SYNOPTICS_LEDS(5,6);
#endif

    PUT_DIAG_STR("Testing low memory...0's...");
    asm("	.globl init_zeros");
    asm("init_zeros:");

    PUNCH_WD ;

    i = 0;	/*hope compiler stays dumb*/
    for (et = LOWCORE0; et < LOWCORE1; et++)
	*et = i;
    for (et = LOWCORE0; et < LOWCORE1; et++) {
	i = *et;
	if (i != 0) {
	    PUTSTRING(cp,"\r\nBad memory - wrote zero, read 0x");
	    PUTHEXNUM(i);
	    PUTSTRING(cp," at location 0x");
	    PUTHEXNUM((unsigned)et);
	    asm("    jmp lights");
	}
    }
    
    /*
     * Test all ones
     */

#ifndef SYNALC
 SYNOPTICS_LEDS(6,7);
#endif
    PUT_DIAG_STR("1's...");
    asm("	.globl init_ones");
    asm("init_ones:");

    PUNCH_WD ;

    for (et = LOWCORE0; et < LOWCORE1; et++)
	*et = 0xFFFFFFFF;
    for (et = LOWCORE0; et < LOWCORE1; et++) {
	i = *et;
	if (i != 0xFFFFFFFF) {
	    PUTSTRING(cp,"\r\nBad memory - wrote all ones, read 0x");				
	    PUTHEXNUM(i);
	    PUTSTRING(cp," at location 0x");
	    PUTHEXNUM((unsigned)et);
	    asm("    jmp lights");
	}
    }
    
    /*
     * Test address patterns
     */

#ifndef SYNALC

    SYNOPTICS_LEDS(7,8);

#endif
    PUT_DIAG_STR("address+1...");
    asm("	.globl init_address");
    asm("init_address:");

    PUNCH_WD ;

    for (et = LOWCORE0; et < LOWCORE1; et++)
	*et = (long) (et + 1);
    for (et = LOWCORE0; et < LOWCORE1; et++) {
	j = (long) (et + 1);
	i = *et;
	if (i != (long)j) {
	    PUTSTRING(cp,"\r\nBad memory - wrote 0x");
	    PUTHEXNUM(j);
	    PUTSTRING(cp,", read 0x");
	    PUTHEXNUM(i);
	    PUTSTRING(cp," at location 0x");
	    PUTHEXNUM((unsigned)et);
	    asm("    jmp lights");
	}
    }
    
    /*
     * Size and initialize memory on powerup or hard reset.
     * Note we are using the stack for the first time.
     */

#ifndef SYNALC

    SYNOPTICS_LEDS(8,9);

#endif
    PUT_DIAG_STR("[OK]\r\nSizing memory...");
    asm("	.globl init_sizing_memory");
    asm("init_sizing_memory:");

    PUNCH_WD ;

    if (~conf & BOOT_FIELD)		/* Indicate NOT to ROM Monitor code. */
    	i = icacheon();		/* turn on instruction cache to gain speed */
    memsize = meminit(~conf & DIAG_MASK) << PAGESHIFT;
#ifdef	SYNALC
	/* Account for all of physical DRAM, not just processor ram. */
    true_memsize = memsize + ADRSPC_SYNALC_TRU_RAM_TAIL ;
#endif
    if (~conf & DIAG_MASK) {
        PUTSTRING(cp,"meminit() returned 0x");
        PUTHEXNUM(memsize);
    }
#ifndef	SYNALC
    (void)icacheoff();
#endif

    /*
     * Set up the cookie pointer.  It will either point at the cookie prom or
     * the data structure we will fill up from the serial NVRAM
     */
    if (~conf & DIAG_MASK) {
	PUTSTRING(cp,"[OK]\r\nSetting cookie...");	
    }

    PUNCH_WD ;

    if (!NORMAL_COOKIE_PROM_PRESENT) {
#ifndef	SYNALC
        PUT_DIAG_STR("[OK]\r\nSetting cookie...");
 	cpu_type = CPU_CRISTAL;	/* don't know what our cpu type is */
				/* so just use CRISTAL cpu type (used for  */
				/* delay call in x2444_io() call below */
	if (*(uchar *)cookie_boot == NO_COOKIE_PROM_CANCUN)  {
	    cpu_type = CPU_CANCUN; /* if we know we arent cristal, use cancun*/
	}
	(void)x2444_io(X2444_CMD_RCL, 8, 0); /* Recall data into RAM */
	delay(1);
	/*
	 * Read bits from the serial NVRAM and fill up the cookie structure, a
	 * byte at a time.
	 */
	for (i = 0; i < 16; i++) {
	    ushort value;
	    value = (ushort)x2444_io(X2444_CMD_READ + 
				     (i * 2), 8, 16); /* Read 16 bits */
	    nv_cookie[(i * 2) + 1] = value;
	    nv_cookie[(i * 2)] = value >> 8;
	}
#else		/* RKL */
	for (i = 0 ; i < sizeof (cookie_t) ; ++ i)
	{
		nv_cookie [i] = ((u_char *) cookie_boot) [i] ;
	}
#endif
	cookie = (cookie_t *)&nv_cookie [0];
    } else {
	cookie = (cookie_t *) cookie_boot;
    }

    /* 
     * Do the reset dance after we find the cookie
     * Case on processor type to use the correct reset register.
     * Set the bits to reset the interface, give the devices 
     * time to respond then clear the reset bits 
     */
    switch (cookie->processor) {
      case PROCESSOR_CRISTAL:
      case PROCESSOR_CANCUN:
      case PROCESSOR_BRUT:
#ifndef	SYNALC
	*(volatile u_short *)ADRSPC_RESET |= ALL_NETWORK_RESET;
	asm("nop");
	asm("nop");                     /* Can't call delay() yet! */
	asm("nop");
	asm("nop");
	*(volatile u_short *)ADRSPC_RESET &= ~ALL_NETWORK_RESET;
#endif	/* SYNALC */
	break;
	
      case PROCESSOR_SYNALC:
      default:
	asm("nop");
	asm("nop");			/* Can't call delay() yet! */
	asm("nop");
	asm("nop");
	asm("nop");
	asm("nop");
	break;
    }
    
    /*
     * Write all of memory outside page 0
     * in order to initialize parity bits.
     * This can't be a "clr", since that instruction reads(!)
     * before writing.  The use of 0xFFFFFFFF means that random
     * jumps will immediately cause Line 1111 Emulator traps.
     */

#ifndef SYNALC

    SYNOPTICS_LEDS(9,10);

    /* 
     * set up buserror (locore.S)  to catch our buserrrs.
     */

    *EVEC_BUSERR = (long)buserror;

    /* 
     * check for the presence of the flh buffer.  We need to first
     * find the end of system memory then look for the magic numbers in the
     * structs that define the buffer to be valid
     */
    flhbuf_start = memsize;
    flhbuf_end = memsize;
    flhend_ptr = 0;
    switch (cookie->processor) {
      case PROCESSOR_CANCUN:
      case PROCESSOR_BRUT:
	if (memsize <= TWO_MEG) { /* No expansion simms installed */
	    flhend_ptr = 
		(struct _flhlog_end *) (ONE_MEG - 
					sizeof (struct _flhlog_end));
	} else if (memsize < SIXTEEN_MEG) { /* 16 Meg > mem size > 2 Meg */
	    flhend_ptr = 
		(struct _flhlog_end *) (memsize - TWO_MEG - 
					sizeof (struct _flhlog_end));
	} else {		/* 2 Meg base + 16 Meg installed */
				/* or 0 Meg base + 16 Meg installed */
	    flhend_ptr = 
		(struct _flhlog_end *) (memsize - 
					sizeof (struct _flhlog_end));
	    /*
	     * now check to see if we have any on board memory and
	     * decrement flhend_ptr if on board 2Mb not present
	     */
	    buserrcnt = 0;
	    buserrhandler = buserrnote; /* Catch and count bus errors */
	    *(volatile ulong *)ADRSPC_MERLOT_IOMEM = 0xADAD2BEE;
	    if ( buserrcnt != 0 ) {
		flhend_ptr = (struct _flhlog_end *)((long) flhend_ptr - 
						    TWO_MEG);
	    }
	    buserrhandler = 0;		/* Report bus errors again */
	}
	break;
      case PROCESSOR_CRISTAL:
	flhend_ptr = 
	    (struct _flhlog_end *) (memsize - sizeof (struct _flhlog_end));
	break;
      case PROCESSOR_BASS:
	flhend_ptr = (struct _flhlog_end *) (memsize - ONE_MEG - 
					     sizeof (struct _flhlog_end));
	break;
      default:	
	/* Moosehead, Merlot ... */
	flhend_ptr = (struct _flhlog_end *) (memsize - HALF_MEG - 
					     sizeof (struct _flhlog_end));
	break;
    }
    if (flhend_ptr) { /* should probably always be true but .... */
	if (flhend_ptr->magic1 == FLHLOGMAGIC1) {
	    if ((flhend_ptr->length) > 0 && 
		(flhend_ptr->length < (2 * DEFAULT_FLHLOG_SIZE) + 1)) {
		flhbegin_ptr = (struct _flhlog_begin *) ((uint) flhend_ptr - 
							 flhend_ptr->length + 
							 sizeof(struct _flhlog_end));
		if (flhbegin_ptr->magic2 == FLHLOGMAGIC2) { /* we got a buffer! */
		    flhbuf_start = (ulong) flhbegin_ptr;
		    flhbuf_end = (ulong) ((ulong)flhend_ptr + 
					  sizeof (struct _flhlog_end));
		}
	    }
	}
    }
    if(cookie->processor == PROCESSOR_CANCUN) {
	/* need to set parity type and enable parity */
	/* before we initialize it */
	*((u_short *)ADRSPC_CONTROL) |= CANCUN_PARITY_ENABLE; /* enable odd */
	/* parity  */
    }

    asm("     .globl check_flh_buf");
    asm("check_flh_buf:");
    cancun_scr = 0;          /* No bus error yet */
    buserrhandler = parity_checker_handler; /* Catch and count bus errors */
    for (et = (long *)flhbuf_start;et < (long *)flhbuf_end; et++) {
	static uint dummy;
	dummy = *et;
	if (cancun_scr) {
	    flhbuf_start = flhbuf_end = memsize;
	    break;
	}
    }
    buserrhandler = 0;
#endif

    if (~conf & DIAG_MASK)
	puts("[OK]\nSetting memory to all ones...");
    asm("	.globl init_parity");
    asm("init_parity:");

	*(volatile u_short *)(ACP_HW_IOREG_BASE + ACP_WRRG_PMD_TOP_LED) =
							ACP_PMD_LEVL_LED_OFF ;

	PUNCH_WD ;

    /*
     * Copy over ourselves to reset parity
     */
    if (~conf & BOOT_FIELD)		/* Indicate NOT to ROM Monitor code. */
    	i = icacheon();		/* turn on instruction cache to gain speed */

    j = ((long) (LOWCORE1)) + sizeof(bootstuff) ;	/* Put in register. */
    if (startup) {
	/*
	 * To avoid clr.x instruction,
	 * need stupid compiler!
	 */
	i = 0;
	for (et = LOWCORE1; et < (long *) j ; ++ et)
	    *et = i;
    }

    PUNCH_WD ;
    et = (long *) j ;
    j = memsize + (ADRSPC_RAM) ;		/* Put in register. */
    i = 0xFFFFFFFF ;				/* Two "unimplemented 1111's" */
    k = (QUART_MEG - 1) & ~03 ;			/* Inter-watchdog period. */
    for ( ; et < (long *) j ; ++ et)
    {
	*et = i ;
	if (! (((long) et) & k))
	{
		PUNCH_WD ;
	}
    }

    PUNCH_WD ;

#ifndef	SYNALC
    (void)icacheoff();
#endif
    if (~conf & DIAG_MASK)
	puts("[OK]\n");

	PUNCH_WD ;

	*(volatile u_short *)(ACP_HW_IOREG_BASE + ACP_WRRG_PMD_MID_LED) =
							ACP_PMD_LEVL_LED_OFF ;

    /*
     * Copy exception table from the data area to the exception area.
     */

#ifndef SYNALC
 SYNOPTICS_LEDS(10,11);
#endif
    if (~conf & DIAG_MASK)
	puts("Setting up exception vectors...");
    asm("	.globl init_vectors");
    asm("init_vectors:");
    buserrhandler = 0;		/* Report bus errors */
    for (et = &evec; et < &evece; et++)
	(RAM_VECTOR_TBL) [et - & evec] = *et ;

    /*
     * Enable the processor data cache, now that we've tested memory:
     * (initc() in lowcore.S turned on the instruction cache before)
     */
    if (~conf & DIAG_MASK)
	puts("[OK]\nEnabling processor caches...");
    asm("	.globl init_enable_caches");
    asm("init_enable_caches:");
#ifndef	SYNALC
    if(cookie->processor == PROCESSOR_BASS) {
        /* set up the access control unit */
        setacu(0,0x02008507);  /* cache all but the second 16mb segment */
    }
#endif
    if (~conf & BOOT_FIELD)		/* Indicate NOT to ROM Monitor code. */
    	i = cacheon();			/* Enable both caches */
    else
    	i = cacheoff();			/* Disable both caches */

    if (~conf & DIAG_MASK)
	printf("set CACR to %#x...", i);
    
    /*
     * Which processor are we? Delay() needs this.
     */
    if (~conf & DIAG_MASK)
	puts("[OK]\nProcessor type is: ");
    asm("	.globl init_cpu_type");
    asm("init_cpu_type:");
    switch (cookie->processor) {
      case PROCESSOR_PAN:
	cpu_type = CPU_PAN;
	break;
      case PROCESSOR_MERLOT:
	cpu_type = CPU_MERLOT;
	break;
      case PROCESSOR_BRUT:
	cpu_type = CPU_IGS_BRUT;
	break;
      case PROCESSOR_CRISTAL:
 	cpu_type = CPU_CRISTAL;
	break;
      case PROCESSOR_BASS:
	cpu_type = CPU_BASS;
	break;
      case PROCESSOR_CANCUN:
	cpu_type = CPU_CANCUN;
	break;
      case PROCESSOR_SYNALC:
	cpu_type = CPU_SYNALC;
	break;
      default:
        cpu_type = CPU_UNKNOWN;
	break;
    }

    if (~conf & DIAG_MASK)
	puts(cputypes[cpu_type]);

#ifndef	SYNALC
    if (cpu_type == CPU_CANCUN) {
	/*
	 * we'll now write the config ram using special routines
	 * to help us access the eeprom nvram. All writes must be
	 * done a byte at a time.
	 */
	eeprom_nvwritebyte((~conf) >> 8, (u_char *)config_ptr,0);
	eeprom_nvwritebyte((~conf) & 0xff, (u_char *)config_ptr,1);

	eeprom_nvwritebyte(CONFREG_MAGIC >> 8, (u_char *)config_magic_ptr,0);
	eeprom_nvwritebyte(CONFREG_MAGIC & 0xff, (u_char *)config_magic_ptr,1);
    }
#endif
    /*
     * Initialize the counter/timers:
     */

#ifndef SYNALC
SYNOPTICS_LEDS(11,12);
#endif
    if (~conf & DIAG_MASK)
	puts("[OK]\nEnabling timers...");
    asm("	.globl init_timers");
    asm("init_timers:");
    /*
     * Initialize Counter 2 for our 4 ms real time clock interrupt:
     */
    /*
     * For SYNALC, low-level Boot has already initialized our MFP timers.
     * All we need to do is unmask the interrupt (see below).
     */
    clock = 0;			/* zero the ms clock */
    hclock = 0;			/* zero the upper ms clock */
    clk_cor_cnt = MFP_4MS_TMR_CORREC_CNT ;
    clk_cor_adj = MFP_4MS_TMR_CORREC_ADJ ;

		/* Read and clear the Reset Reasons register. */
	resetrsns = *(volatile u_char *)(ACP_HW_IOREG_BASE + ACP_RDRG_RST_REASONS) ;
	*(volatile u_char *)(ACP_HW_IOREG_BASE + ACP_WRRG_CLR_RST_REASNS) = ON  ;
	asm("nop");
	asm("nop");
	asm("nop");
	asm("nop");
	*(volatile u_char *)(ACP_HW_IOREG_BASE + ACP_WRRG_CLR_RST_REASNS) = OFF ;

    /*
     * Initialize Counter 0 as watchdog -- 4 times refresh rate
     */
    /*
     * For SYNALC, watchdog is always running and is tied to NMI.
     */

    /*
     * The following write clears any existing IRQ from the i8254
     * so that the timers will start interrupting us:
     */
    /*
     * For SYNALC, timers driven by 68901 MFP.  Watch for spurious interrupts !
     * Since lower-level Boots may (or may not) have enabled UART interrupts,
     * re-groom the MFP for both Timers and UARTs.
     */
    PUNCH_WD ;
    MFP_InitAll (FALSE, NULL, MFP_BASE_VECTOR, FALSE, FALSE, TRUE, TRUE, TRUE) ;
    
    /*
     * SYNALC:
     * Perform a quick go/no go test on the 4 ms real time clock:
     * we have to init the brk/abort/abortstate/config variables here as
     * they will be referenced by the timer ISR ("refreshe"), monitor, etc.
     * Other cisco boxes use NMI for timer (and thus BREAK detect).
     * SYNALC uses MFP for timer intr, but don't worry about BREAK
     * detect at this time since our console is Debug-only
     * (ie. not available in field).
     */

    asm("	.globl init_tclock");
    asm("init_tclock:");

    PUNCH_WD ;

    brk = 0;				/* no breaks */
    abort = FALSE;			/* no abort state */
    abortstate = ABORT_IGNORE;		/* aborts are internal */
    *(u_short *) & config = conf;	/* config register at startup */

    delay(4);				/* Wait at least 4 ms */
    sil (0) ;				/* AFTER timer, SYNALC not use NMI. */
    sil (7) ;

    PUNCH_WD ;
    if (clock) {
	if (~conf & DIAG_MASK)
	    puts("[OK]\n");
    } else {
	puts("real time clock FAILED to increment\n");
	asm("    jmp lights");
    }

    /*
     * Enable or disable the watchdog timer:
     * Initialize the counter timers *BEFORE* doing this. Otherwise, we are
     * vulnerable to a momentary glitch in WE (watchdog enable) which will
     * reset the box if the 8254 comes up in a funny state with Counter 0
     * out already asserted.
     */

#ifndef SYNALC
SYNOPTICS_LEDS(13,14);
#endif
    if (~conf & WATCHDOG_MASK) {
	if (~conf & DIAG_MASK)
	    puts("Watchdog timer disabled...");
    } else {
	if (~conf & DIAG_MASK)
	    puts("Watchdog timer running...");
    }

    /*
     * Size NVRAM via Bus error detection. We don't want to size memory via
     * detecting on a write, because we don't want to be left holding the bag
     * if we lose power before restoring the NVRAM location...
     */

#ifndef SYNALC
SYNOPTICS_LEDS(14,15);
#endif
    if (~conf & DIAG_MASK)
	puts("[OK]\nSizing NVRAM...");
    asm("	.globl init_size_nvram");
    asm("init_size_nvram:");
    PUNCH_WD ;

    buserrcnt = 0;		/* No bus error yet */
    buserrhandler = buserrnote; /* Catch and count bus errors */
    for (cp = (char *)ADRSPC_NVRAM;
	 (cp < (char *)ADRSPC_NVRAM_END) && (buserrcnt == 0);
	 cp += 1024)
	i = *(volatile u_char *)cp;
    nvram_size = (uint)(cp - ADRSPC_NVRAM - 1024);
    buserrhandler = 0;		/* Report bus errors again */
    if (~conf & DIAG_MASK)
	printf("found %dK bytes of NVRAM", nvram_size/1024);
    
    /*
     * Determine if a system image is present by searching for 'magic'
     * patterns in ROM address space after the end of the ROM monitor.
     * Search for the start of the system image until:
     *  o We find a system image, set prom2 to point to it, or
     *	o We encounter a bus error, or
     *	o We reach the end of the 16 MB ROM address region
     */
    /*
     * For SYNALC, we use the Boot Code Service (BCS) to find IOS run-time
     * image in flash.  If found, set prom2 to point to it.	RKL.
     */

#ifndef SYNALC
SYNOPTICS_LEDS(15,16);
#endif
    if (~conf & DIAG_MASK)
	puts("[OK]\nSearching for system code...");
    asm("	.globl init_find_system");
    asm("init_find_system:");
    PUNCH_WD ;

    buserrcnt = 0;		/* No bus error yet */
    buserrhandler = buserrnote; /* Catch and count bus errors */
    prom2 = (u_short *)NULL;
#ifdef	SYNALC			/* RKL */
	/* Get BCS Service entry point.  Don't checksum its   */
	/* code as it is already available for us to be here. */
    do {
	PUNCH_WD ;
	i = (long) BCS_FindSvc (BCS_API_REV_0, TRUE, BCSL_BF_NONE) ;
	if (i == (long)(BCSSR_FAILURE))
		break ;

	PUNCH_WD ;
	et = (long *) ((* (p_void (*)()) i)
				(BCS_API_REV_0, TRUE, BCSSR_GET_FLASH_BASE,
				 BCS_IH_TYPE_IOSKR, BCSL_BF_NONE)) ;

	if (et == (long *)(BCSSR_FAILURE))
		break ;

#ifdef	RKL
	else  if ((*(u_short *)et == (PROM2_MAGIC1)) ||
		  (*(u_short *)et == (PROM2_MAGIC2)) ||
		  (*(u_short *)et == (PROM2_MAGIC3)))
#endif
		prom2 = (u_short *) et ;

    } while (FALSE) ;
    PUNCH_WD ;
#else
    for (et = (long *)&etext;
       ((et < (long *)ADRSPC_PROM_END) && (buserrcnt == 0));
	(u_char *)et += 2) {
	if (*(u_short *)et == PROM2_MAGIC1 ||
	    *(u_short *)et == PROM2_MAGIC2 ||
	    *(u_short *)et == PROM2_MAGIC3) {
	    prom2 = (u_short *)et;
	    break;
	}
    }
#endif	/* SYNALC */
    buserrhandler = 0;		/* Report bus errors again */
    if (~conf & DIAG_MASK)
        if (prom2 != NULL)
           printf("found at %x, magic number = %x...",prom2, *prom2);
        else
           puts("no system image found...");

    PUNCH_WD ;

    *(volatile u_short *)(ACP_HW_IOREG_BASE + ACP_WRRG_PMD_BOT_LED) =
							ACP_PMD_LEVL_LED_OFF ;

    /*
     * Variable initializations
     */

#ifndef SYNALC
SYNOPTICS_LEDS(16,17);
#endif
    if (~conf & DIAG_MASK)
	puts("[OK]\nSetting up monitor variables...");
    asm("	.globl init_variables");
    asm("init_variables:");

    switch (cookie->processor) {
      case PROCESSOR_BASS:
      case PROCESSOR_CANCUN:
      case PROCESSOR_BRUT:
      case PROCESSOR_CRISTAL:
	break;

      case PROCESSOR_SYNALC:
      default:
        /*
	 * SYNALC physical DRAM is sectioned into processor ram + reserved
	 * area.  Latter includes NTC DMA area, lower-level Boot areas, and
	 * IOS iomem area.  ACP Frame Buffer is kept hidden from IOS.
	 * In fact, IOS processor ram and ioram are placed contiguously here.
	 * Diags on ALL physical ram areas have been run by low-level Boot.
	 */
        iomem_base = ADRSPC_SYNALC_IOMEM ;
        iomem_size = ADRSPC_SYNALC_IOMEM_SIZ ;

	if (! (*(volatile u_char *)(ACP_HW_IOREG_BASE + ACP_RDRG_DEBUG)
						& (1 << ACP_RRB_JMP_FASTBOOT_)))
	{
		if (~conf & DIAG_MASK) {
			PUTSTRING (cp, "skipping I/O ram tests...") ;
		}
		PUNCH_WD ;
		break ;
	}

	/* 
	 * Go and verify that iomem ram is good since
	 * we've only tested the processor ram.
	 */
	if (~conf & DIAG_MASK) {
		PUTSTRING(cp,"I/O -- Memory 1's...");
	}
	asm("	.globl init_io_ones");
	asm("init_io_ones:");

    	i = cacheoff();			/* Disable both caches, esp. data. */

	if (~conf & BOOT_FIELD)		/* Indicate NOT to ROM Monitor code. */
		i = icacheon();		/* Turn on instr-cache to gain speed */

	PUNCH_WD ;
	i = 0xFFFFFFFF ;
	j = (long) (iomem_base + iomem_size) ;		/* Put in register. */
	k = ((QUART_MEG - 1) & ~03) ;			/* Inter-wdog period. */
	for (et = (long *) iomem_base ; et < (long *) j ; ++ et) {
		*et = i ;
		if (! (((long) et) & k))
		{
			PUNCH_WD ;
		}
	}

	PUNCH_WD ;
	h = i ;						/* Put in register. */
	for (et = (long *) iomem_base ; et < (long *) j ; ++ et) {
		if (! (((long) et) & k))
		{
			PUNCH_WD ;
		}

		i = *et;
		if (i != h) {
			PUTSTRING(cp,"\r\nBad I/O memory - wrote all ones, read 0x");				
			PUTHEXNUM(i);
			PUTSTRING(cp," at location 0x");
			PUTHEXNUM((unsigned) et);
			asm("    jmp lights");
		}
	}

	PUNCH_WD ;
	if (~conf & DIAG_MASK) {
		PUTSTRING(cp,"I/O -- address+1...");
	}
	asm("	.globl init_io_address");
	asm("init_io_address:");

	PUNCH_WD ;
	j = (long) (iomem_base + iomem_size) ;		/* Put in register. */
	k = ((QUART_MEG - 1) & ~03) ;			/* Inter-wdog period. */
	for (et = (long *) iomem_base ; et < (long *) j ; ++ et) {
		*et = (long) (et + 1) ;
		if (! (((long) et) & k))
		{
			PUNCH_WD ;
		}
	}

	PUNCH_WD ;
	for (et = (long *) iomem_base ; et < (long *) j ; ++ et) {
		if (! (((long) et) & k))
		{
			PUNCH_WD ;
		}

		h = (long) (et + 1) ;
		i = *et ;
		if (i != h) {
			PUTSTRING(cp,"\r\nBad I/O memory - wrote 0x");
			PUTHEXNUM(h);
			PUTSTRING(cp,", read 0x");
			PUTHEXNUM(i);
			PUTSTRING(cp," at location 0x");
			PUTHEXNUM((unsigned) et);
			asm("    jmp lights");
		}
	}

	PUNCH_WD ;
	if (~conf & BOOT_FIELD)		/* Indicate NOT to ROM Monitor code. */
    		i = cacheon();		/* Enable both caches */
	else
    		i = cacheoff();		/* Disable both caches */
	break;
    }

#ifdef	SYNALC
    if (~conf & DIAG_MASK)
        printf("\nfound %d total Kb DRAM, %d Kb processor ram, iomem_base $%x, size %d Kb",
               true_memsize/1024, memsize/1024, iomem_base, iomem_size/1024);
#else
    if (~conf & DIAG_MASK)
        printf("\nfound %d total Kbytes cpu DRAM, iomem_base $%x, size %d Kb",
               memsize/1024, iomem_base, iomem_size/1024);
#endif

    *(u_short *)&config = conf;	/* config register at startup */
    echoing = TRUE;		/* we want to echo characters */
    brk = 0;			/* no breaks */
    abort = FALSE;		/* no abort state */
    abortstate = ABORT_IGNORE;	/* aborts are internal */
    buserrhandler = 0;		/* routine to handle bus errors */
    breakf = FALSE;		/* not inst. break state */
    breakaddr = 0;		/* first value */
    breakvalue = 0;		/* to be consistant with above */
    lastloc = 0;		/* last dep/ex. location */
    lastsize = 'L';		/* last dep/ex. size */
    lastswap = FALSE;		/* last dep/ex. byte swap action */
    cache = TRUE;		/* cache default to on */
    baseaddr = LOWCORE1;	/* default net load address */
    system_state = SYSTATE_MONITOR; /* Note we are in the monitor */

    vsr_set = vstring_sys_rom[0] = vstring_sys[0] = '\0';

    /*
     * Read the hardware revision register:
     */

#ifndef SYNALC
SYNOPTICS_LEDS(17,18);
#endif
    if (~conf & DIAG_MASK)
	puts("[OK]\nReading hardware revision register...");
    asm("	.globl init_rev");
    asm("init_rev:");
    rev = *(volatile u_short *)ADRSPC_HW_REVISION;

    /*
     * Get the hardware revision number:
     */
    revision = rev >> 8;
    if (~conf & DIAG_MASK)
	printf("\n%s processor, hardware revision = %d",
	       cputypes[cpu_type], revision);

    /*
     * Save information from restart() in bootstuff[]:
     */

    asm("	.globl init_bootstuff");
    asm("init_bootstuff:");
    et = LOWCORE1;
    bootstuff[BOOT_COUNT] = BOOTDATA;	/* Length of bootstuff */
    for (i = BOOT_REASON; i <= BOOTDATA; i++)
	bootstuff[i] = *et++;		/* Copy into bootstuff */
    
    /*
     * Clear out the cross-boot information area
     */

    bzero(boot_information, BOOTINFOSIZE);

    /*
     * Try to generate a possible parity error:
     */
    /*
     * This is not possible on SYNALC, as parity is NOT supported.
     */
#ifndef SYNALC
SYNOPTICS_LEDS(19,20);
#endif

    asm("	.globl init_test_parity");
    asm("init_test_parity:");

	for (i = 0; i < BUFSIZ; i++) {
	    bufu.buf[i] = (char)i;
	    j = bufu.buf[i];
	}
    
    /*
     * Light the run lamp:
     */
	*(volatile u_short *)(ACP_HW_IOREG_BASE + ACP_WRRG_PMD_STAT_LED) =
							ACP_PMD_STAT_LED_GRN ;

	PUNCH_WD ;

#ifndef SYNALC
SYNOPTICS_LEDS(21,22);
#endif

    bzero((char*) &caps, sizeof(caps));
    caps.version = SYSCAP_VERSION;
    caps.length = SYSCAP_LENGTH;

    if (~conf & DIAG_MASK)
	puts("\nExiting to monitor command level\n\n");

    /*
     * Return
     */
    asm("	.globl initc");
    asm("	jmp initc");
}

/*
 * Reset all IO devices to a quiescent state:
 * Check for the number of ports then set the bits in the control 
 * register. This results in assertion of the reset input to the
 * chip. Note that the reset must be active for at least 5 msec
 * for the TMS380.
 */

void reset_io ()
{

    /* 
     * Case on processor type to use the correct reset register.
     * Set the bits to reset the interface, give the devices 
     * time to respond then clear the reset bits 
     */
    switch (cookie->processor) {
      case PROCESSOR_CRISTAL:
      case PROCESSOR_CANCUN:
      case PROCESSOR_BRUT:
#ifndef	SYNALC
	*(volatile u_short *)ADRSPC_RESET |= ALL_NETWORK_RESET;
	delay(5);
	*(volatile u_short *)ADRSPC_RESET &= ~ALL_NETWORK_RESET;
#endif	/* SYNALC */
	break;

      case PROCESSOR_SYNALC:
      default:
	delay(5);
	break;
    }
} /* reset_io() */

/*
** Reset the virtual configuration register to default value
*/

void reset_confreg ()
{
    if(cpu_type == CPU_CANCUN) {
	/* 
	 * for cancun we need to do byte writes using eeprom_nvwritebyte()
	 * routine.  Write the bytes in BIG endian format.
	 */
	u_short tmpconf = DEFAULT_CONFIG | IGNORE_CONFIG;
	eeprom_nvwritebyte((tmpconf) >> 8, (u_char *)config_ptr,0);
	eeprom_nvwritebyte((tmpconf) & 0xff, (u_char *)config_ptr,1);
    } else {
	*(u_short *)config_ptr = DEFAULT_CONFIG | IGNORE_CONFIG;
    }
}


void parity_checker_handler ()
{
}


/*
 * write_bad_main_memory_parity
 * Write the given data to the specified location in main memory with
 * incorrect parity, so that reading from this location will cause a
 * parity error. Do this from ROM so that we don't have to worry about
 * opcode fetches while the main memory parity setting is set wrong:
 */

int write_bad_main_memory_parity (data, location)
    uint data;
    uint *location;
{
#ifdef	lint
	data = 1 ;
	location = & data ;
#endif

    return(0);
}
#ifndef SYNALC
disable_nmi()
{
    if (cpu_type == CPU_CANCUN) {
        /*
         * now get disable nmi's.  YES you can do this on cancun. By
         * writing a one (0x01) to counter 2 it sets up a mode where the
         * timer will never generate an interrupt -- It will continually
         * reload a 0x01 -- as the interrrupt is generated because of the
         * transisition from 2 to 1.
         */
        *(short *)ADRSPC_BRUT_TIMER2 = 0x01;

    }
}
#endif

#ifdef	SYNALC

/*
 * ALC_LoadAcpCode
 * Load (or download and burn to flash) a SYNALC ACP image.
 * Also pre-load the SAR images.
 * Returns length of ACP image, or 0 on any error.
 */

u_long ALC_LoadAcpCode (u_char * startaddr, u_char * endaddr, u_long boot_flgs)
{
	u_long	size ;
	p_void	(*pFcn) () ;
	p_void	Val ;


#ifdef	DEBUG
	buginf ("\n\tALC_LoadAcpCode:  entering...\n") ;
#endif

		/* Get BCS Service entry point.  Don't checksum its	*/
		/* code as it is already available for us to be here.	*/

		/* Check for the following flags to be set on calls	*/
		/* to "bootload":  BOOTERASEFLASH.			*/

	size = 0 ;
	pFcn = NULL ;
	Val  = NULL ;

	do {
		PUNCH_WD ;
		pFcn = BCS_FindSvc (BCS_API_REV_0, TRUE, BCSL_BF_NONE) ;
		if (pFcn == (p_void)(BCSSR_FAILURE))
			break ;

		/* Flash downloads now done by Boot-1. */
#ifdef	DEBUG
		PUNCH_WD ;
		buginf("\n\tALC_LoadAcpCode:  loading from flash...\n");
#endif

		PUNCH_WD ;
		Val = (*pFcn) (BCS_API_REV_0, TRUE, BCSSR_COPY_IMAGE,
				BCS_IH_TYPE_IOSKR, BCSL_BF_NONE) ;
		if (Val == (p_void)(BCSSR_FAILURE))
			break ;

		PUNCH_WD ;
		Val = (*pFcn) (BCS_API_REV_0, TRUE, BCSSR_GET_RAM_SIZE,
				BCS_IH_TYPE_IOSKR, BCSL_BF_NONE) ;
		if (Val == (p_void)(BCSSR_FAILURE))
			break ;

		PUNCH_WD ;
		size = (u_long) Val ;

		/* SARs should have been loaded by Boot-2. */

	} while (FALSE) ;

#ifdef	DEBUG
	PUNCH_WD ;
	buginf ("\n\tALC_LoadAcpCode:  size %u\n", size) ;
#endif

	PUNCH_WD ;
	return (size) ;

}	/* ALC_LoadAcpCode */
#endif	/* SYNALC */
