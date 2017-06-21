/* $Id: init.c,v 3.2.50.2 1996/06/17 08:44:51 jhernand Exp $
 * $Source: /release/112/cvs/Xboot/src-68-c3000/init.c,v $
 *------------------------------------------------------------------
 * init.c -- system hardware initialization
 *
 * August 1986, Greg Satz
 * Modified from csc2/init.c for Pancake, Chris Shaker, September 12, 1989
 *
 * Copyright (c) 1986-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: init.c,v $
 * Revision 3.2.50.2  1996/06/17  08:44:51  jhernand
 * Commit Modular Cobra to California_branch
 *
 * Revision 3.2.50.1  1996/06/13  17:18:14  etrehus
 * Commit boa to California Branch
 *
 * Revision 3.2  1995/12/03  17:55:20  thille
 * CSCdi44905:  ciscopro targets not in 11.1
 * Put them in.
 *
 * Revision 3.1  1995/11/09  09:09:12  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/09/22  23:57:17  samuell
 * CSCdi33515:  ROM monitor not reset HD64570 on C2500 platform
 *
 * Revision 2.1  1995/06/07  19:15:38  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 * Revision 1.6  1994/10/30 21:59:15  abaindur
 * CSCdi25240:  Need to support dfb on low end systems
 * Flash Load Helper and Dual Flash Bank port.
 *
 * Revision 1.5.14.2  1994/10/14  18:40:34  enf
 * Branch: Dfb_branch
 * put dfb into boot tree
 *
 * Revision 1.5.14.1  1994/10/07  23:31:02  enf
 * Branch: Dfb_branch
 * flh support for ROMMON
 *
 * Revision 1.5  1994/07/13  18:45:10  sjay
 * CSCdi22006:  Revise code for new Cancun memory config
 * Incorporate code-review suggestions in code to support new Cancun memory
 * configuration (originally added with CSCdi21144).
 *
 * Revision 1.4  1994/06/28  18:41:09  sjay
 * CSCdi20865:  Boot changes to support Brut II: more memory; jump to
 * diagnostics
 *
 * Support for Brut II:
 * 1.  Brut II has 8 MB DRAM.  Split it properly into IO & Processor (2/6)
 *     by using same code as for Cancun.
 * 2.  Support diagnostic code in high ROM by jumping to it if
 *     "manufacturing mode" hardware status is pulled low during
 *     initialization.
 *
 * Revision 1.3  1994/06/28  18:12:43  sjay
 * CSCdi21144:  Boot code problem for new Cancun memory config (w/o 2MB
 * soldered-on)
 *
 * Some partner derivatives of Cancun don't have "on-board" memory...only
 * SIMM (4 MB or 16 MB).  Fix code to test for presence of 2 MB IO memory
 * when 16 MB SIMM is installed, set iomem_base, iomem_size, and memsize
 * based on result of test.
 *
 * This code was done by John Kuzma (jkuzma).
 *
 * Revision 1.2  1993/12/21  06:25:45  enf
 * Cancun boot support
 *
 * Revision 1.1  1993/09/24  16:33:26  widmer
 * Rename boot obj directories to include CPU type and product family
 *
 * Revision 1.19  1993/08/20  06:06:21  hampton
 * Blast from the past.  Missing change from 9.21(0.14) update.
 *
 * CSCdi11972:  New proms don't set cookie correctly
 * 
 * If not a serial NVRAM cookie, set cookie pointer to the cookie PROM.
 *
 * Revision 1.18  1993/08/19  03:51:23  hampton
 * Bring modular system to Aries 9.21(0.18) patch level.
 *
 * CSCdi12370:  Invalid memory check for 8 Mb of main memory
 * 
 * Check to make sure we're a cristal before checking for a second 4MB
 * bank.
 *
 * CSCdi09946:  Protect stack_whyreload() from bad boot_reason values
 * 
 * Doing ROM monitor changes required to support EMT_SET_CACR on all
 * 68020/68030/68040 platforms. Share setcacr() and getcacr() on all of
 * these platforms. Move all defines for bits in CACR registers to the
 * pcmap.h files for each processor so that process level code does not
 * have to have its own definition of the CACR bits.
 * 
 * Add better comments to explain memory sizing code in Merlot (IGS/TR)
 * ROM monitor.
 * 
 * Add new EMT call, EMT_WRITE_BAD_MAIN_MEMORY_PARITY, used by the new
 * 'test crash' menu in the system image. This EMT call writes a 32 bit
 * word of information into main memory with bad parity.
 * 
 * Fix EMT number collision between Clean and 9.1 for new EMT call to make
 * life easier for the 9.17 release.  Also, the numbers for all new EMT
 * calls currently defined in the 9.21 release are now also defined in the
 * 9.1 release as placeholders to prevent such EMT collision problems in
 * the future.
 *
 * Revision 1.17  1993/06/25  05:09:01  smackie
 * Bring modular system to Aries 9.21(0.10) patch level.
 *
 * CSCdi11223:  merge 9.14 low end code into aries
 *
 * Revision 1.16  1993/02/28  22:59:49  schaefer
 * Chassis MIB
 *
 * Revision 1.15  1993/02/03  04:21:37  gstovall
 * Removed support for the cristal low-end platform.
 *
 * Revision 1.14  1993/01/30  01:11:24  whp4
 * Add RP1 support.
 *
 * Revision 1.13  1992/10/09  20:57:54  clev
 * Add CPU_BASS for Bass and Pinot platforms.
 *
 * Revision 1.12  1992/09/26  00:41:08  gstovall
 * Added support for the serial NVRAM cookie used on cristal.  Copied kph's
 * driver down to the rom monitor level.
 *
 * Revision 1.11  1992/09/23  17:12:50  clev
 * Add improved cache command for pan.
 * Add support for Bass.
 *
 * Revision 1.10  1992/09/19  21:54:33  gstovall
 * Added changes to support the Cristal low-end router.  Added new processor
 * type and converted a few if-else to switch statements.
 *
 * Revision 1.9  1992/08/31  20:37:58  dcoli
 * Changes for Brut.
 *
 * Revision 1.8  1992/07/08  08:15:17  schaefer
 * remove cookie-based software capability bits
 *
 * Revision 1.7  1992/06/24  00:43:45  clev
 * Set flash size for Merlot (with 16bit boot proms) to 2mb.
 *
 * Revision 1.6  1992/06/08  23:55:37  shaker
 * CSCdi05317, "default config reg setting should have NOCONFIG set".
 *
 * Revision 1.5  1992/05/12  22:20:49  clev
 * - Eliminate support for the physical configuration register on Pan.
 *
 * - Add support for the monitor command "o/r" for Pan and XX.
 *
 * - Break enable during boot on Pan.
 *
 * - Flash support added for Pan.
 *
 * Revision 1.4  1992/05/11  05:58:37  schaefer
 * zero capabilities structure
 *
 * Revision 1.3  1992/04/17  23:48:55  schaefer
 * fix typo in .cvsignore
 * ROM monitor changes to read image capability bits from cookie PROM
 *
 * Revision 1.2  1992/04/08  18:16:09  kph
 * Lemonade cleanup:
 *
 * Flash OK LED at different rates for ROM monitor, netbooting, and running
 * the system. Honor the reset flag from the Emulex diagnostics.
 *
 * Revision 1.1  1992/03/07  23:57:36  hampton
 * Initial conversion from RCS to CVS.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "../defs.h"
/* #include "../types.h" */
#include "../reason.h"
#include "../m68vectors.h"
/* #include "confreg.h" */
/* #include "console.h" */
#include "../i8254.h"
#include "../prom2.h"
#include "../extern.h"
#include "../stdio.h"
#include "../cpu.h"
#include "../test.h"
#include "../caps.h"
/* #include "cookie.h" */
#include "pcmap.h"
#include "nonvol_x2444.h"
#include "../flh.h"
#include "../flh_log.h"
extern u_long etext;
extern char int_vecs[];
extern volatile short buserrcnt; /* Bus error count */
extern u_short flags;

void parity_checker_handler();
void reset_io();

struct regpair *console = (struct regpair *) &uartaddr->a; /* UART registers */
    cookie_t *cookie; /* Address of cookie */
    cookie_t *cookie_boot = (cookie_t *)ADRSPC_COOKIE; /* Address of cookie */
    
    uchar nv_cookie[sizeof(cookie_t)];/* Storage for info in serial NVRAM cookie */
    
    uchar *syn_leds = (uchar *)ADRSPC_SYNOPTICS_LEDS;
    volatile u_short cancun_scr;	/* snapshot of cancun SCR 1 */
    volatile ulong tmp_parity_test_location; /* memory location to do parity */
    /* tests on */
    
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
    extern void buserror();

    register long startup;	/* MAGIC - see asm code below */
    /* startup *must* be a register variable */
    u_short conf;		/* config parameter from config nvram */
    /* conf  *must* be register variable */
    u_short control;
    u_short rev;
    register long i, j;
    register long *et;		/* this *must* be register variable */
    register char *cp;		/* variable used to help print strings */
    /* this *must* be register variable */
#ifdef PAN_ORIGINAL
    u_char phys_conf;
#endif /* PAN_ORIGINAL */
    uchar syn_state = SYNOPTICS_LED_ALL;
    /* make sure these variables are not on stack and not register vars */
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
    asm("	.globl init_reset_int");
    asm("init_reset_int:");
    
#define SYNOPTICS_LEDS(a,b) \
    if (NORMAL_COOKIE_PROM_PRESENT) { \
	if (cookie_boot->vendor == VENDOR_SYNOPTICS) { \
	    CLR_SYNOPTICS_LEDS(syn_leds, SYNOPTICS_LED_STEP_ ## a); \
	    SET_SYNOPTICS_LEDS(syn_leds, SYNOPTICS_LED_STEP_ ## b); \
	} \
    }
    
    
    if (NORMAL_COOKIE_PROM_PRESENT) {
	if (cookie_boot->vendor == VENDOR_SYNOPTICS) {
	    CLR_SYNOPTICS_LEDS(syn_leds, SYNOPTICS_LED_ALL);
	    SET_SYNOPTICS_LEDS(syn_leds, SYNOPTICS_LED_BOOT); /* Step 0 */
	}
    }
    
    /* 
     * Case on processor type to use the correct reset register.
     * Set the bits to reset the interface, give the devices 
     * time to respond then clear the reset bits 
     */
    if (NORMAL_COOKIE_PROM_PRESENT) {
	switch (cookie_boot->processor) {
	case PROCESSOR_CRISTAL:
	case PROCESSOR_CANCUN:
	    *(volatile u_short *)ADRSPC_RESET |= ALL_NETWORK_RESET;
	    asm("nop");
	    asm("nop");                     /* Can't call delay() yet! */
	    asm("nop");
	    asm("nop");
	    *(volatile u_short *)ADRSPC_RESET &= ~ALL_NETWORK_RESET;
	    break;
	    
	case PROCESSOR_BRUT:
	    
            /*
             * Support for diagnostics in high (upper 1 MB of 2 MB) boot ROM:
             *
             * If this is a Brut II (ASIC rev > 1), and the "manufacturing
             * mode" bit (DUART I3 pin, which is connected to backplane) is
             * low, and the right "magic number" is found in high ROM, jump
             * to the diagnostic.  Check for Brut II is needed because Brut I
             * has only 1 MB ROM space, so check for magic number might cause
             * bus error.  Jump is used instead of call to avoid use of stack
             * (DRAM).
             * Jump is via helper routine in locore.S to avoid declaring
             * another register variable here, which might disrupt the very
             * delicate assignments already used.
             */
	    
            if ((*(u_char *)ADRSPC_BRUT_ASIC_REV >= BRUT_2M_BOOT_REV) &&
                !(uartaddr->b.aux2 & BRUT_DUART_MFG_MODE) &&
                (*(u_short *)BRUT_DIAG_MAGIC_ADDR == BRUT_DIAG_MAGIC_VALUE)) {
		
		asm(".globl brut_diag_jump");
		asm("jmp brut_diag_jump");
		
            }
	    
	    *(volatile u_short *)ADRSPC_CONTROL3 |= (BRUT_RESET_NETWORK_A | 
						     BRUT_RESET_NETWORK_B | 
						     BRUT_RESET_NETWORK_C);
	    asm("nop");
	    asm("nop");			/* Can't call delay() yet! */
	    asm("nop");
	    asm("nop");
	    asm("nop");
	    asm("nop");
	    *(volatile u_short *)ADRSPC_CONTROL3 &= ~(BRUT_RESET_NETWORK_A | 
						      BRUT_RESET_NETWORK_B | 
						      BRUT_RESET_NETWORK_C);
	    break;
	    
	default:
	    *(volatile u_short *)ADRSPC_CONTROL |= (CONTROL_RESET_NETWORK_A | 
						    CONTROL_RESET_NETWORK_B);
	    asm("nop");
	    asm("nop");			/* Can't call delay() yet! */
	    asm("nop");
	    asm("nop");
	    asm("nop");
	    asm("nop");
	    *(volatile u_short *)ADRSPC_CONTROL &= ~(CONTROL_RESET_NETWORK_A | 
						     CONTROL_RESET_NETWORK_B);
	    break;
	}
    }
    
    /*
     * Update the parts of the NVRAM config register that are controlled
     * by the physical configuration register:
     */
    if (NORMAL_COOKIE_PROM_PRESENT) {
	if (cookie_boot->vendor == VENDOR_SYNOPTICS)
	    SET_SYNOPTICS_LEDS(syn_leds, SYNOPTICS_LED_STEP_1);
    }
    
    asm("	.globl init_phys_config");
    asm("init_phys_config:");
#ifdef PAN_ORIGINAL
    phys_conf = *(u_char *)phys_confreg_ptr;
#endif
    
    /*
     * Is the configuration magic value in NVRAM ok?
     */
    conf = *(u_short *)config_magic_ptr;
    if (conf == CONFREG_MAGIC)
	conf = *(u_short *)config_ptr; /* Yes, use config value from NVRAM */
    else
	conf = DEFAULT_CONFIG;	       /* No, use default config value */
    
#ifdef PAN_ORIGINAL
    /*
     * Now fold in the bits from the physical configuration register:
     */
    if (phys_conf & PHYS_DIAG_MASK)
	conf |= DIAG_MASK;
    else
	conf &= ~DIAG_MASK;
    if (phys_conf & PHYS_WATCHDOG_MASK)
	conf |= WATCHDOG_MASK;
    else
	conf &= ~WATCHDOG_MASK;
    conf &= ~CONSOLE_MASK;	/* Update baud rate bits */
    conf |= ((phys_conf & PHYS_CONSOLE_MASK) << PHYS_TO_VIRT_CONSOLE_SHIFT);
    conf &= ~BOOT_FIELD;	/* Update boot bits */
    conf |= (phys_conf & BOOT_FIELD);
#endif
    /* 
     * Assume that if we get NO_COOKIE_PROM_CANCUN that we must be running 
     * cancun (may not be true in the future ... )
     * On cancun, we'll save the conf value later beacuse of the hassle 
     * with writing eeprom config device.
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

    SYNOPTICS_LEDS(1,2);
    
    asm("	.globl init_uart");
    asm("init_uart:");
    i = ((~conf & CONSOLE_MASK) >> CONSOLE_SHIFT); 
    uartaddr->a.aux2 = 0;	/* Turn off interrupts (IMR=0) */
    uartaddr->a.command = RESETMRPTR;
    uartaddr->a.mode = CHARERROR | PRTYOFF | DBITS8;
    uartaddr->a.mode = MODENORMAL | STOP16X16;
    uartaddr->a.aux1 = BRGSET1 | TIMXTAL;
    uartaddr->a.state = (RXCLOCK * UARTspeeds[i]) | (TXCLOCK * UARTspeeds[i]);
    uartaddr->a.command = STP_TXENABLE|STP_RXENABLE;
    uartaddr->b.aux2 = 0;	/* All outputs are driven by OPR */
    uartaddr->b.aux3 = CDA;	/* Set Carrier detect on Console port */
    
    SYNOPTICS_LEDS(2,3);
    asm("	.globl init_version");
    asm("init_version:");
    PUTSTRING(cp,(~conf & OEM_MASK) ? oemversion : ciscoversion);
    
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
    SYNOPTICS_LEDS(3,4);
    PUT_DIAG_STR("Exiting boot state - setting bus error vector...");
    asm("	.globl init_exit_boot");
    asm("init_exit_boot:");
    *(volatile u_short *)ADRSPC_CONTROL &= ~CONTROL_BOOT;
    
    if (NORMAL_COOKIE_PROM_PRESENT) {
	if (cookie_boot->processor == PROCESSOR_BRUT) {
	    /* Set Control Reg 4 as follows for the Brut box:
	     *   Cookie    : 0 wait states
	     *   Flash     : 0 wait state reads, 1 wait state writes
	     *   NVRAM     : 2 wait states
	     *   Boot PROM : 2 wait states
	     *   DRAM      : 20 Mhz
	     * Note: the hardware defaults to all 1's for these wait state
	     * bits, so we need to clear the register before ORing in the
	     * values for the various fields.
	     */
	    
	    *(ushort *)ADRSPC_CONTROL4 = 0;
	    *(ushort *)ADRSPC_CONTROL4 = BRUT_2_WS_COOKIE | BRUT_0_WS_FLASH |
		BRUT_2_WS_NVRAM  | BRUT_2_WS_PROM  |
		    BRUT_0_WS_DRAM   | BRUT_5_WS_UART  |
			BRUT_20MHZ_CPU   | BRUT_IO_PRIORITY;
	    
	    /* Set control register 2 (EPROM size+width and Flash size+width)
	     * from flags value massaged in by the bytek program.
	     * To account for a bug in bytek, OR this value with 0x02
	     * to force flash size to atleast 4Mbytes.
	     */
	    
	    *(volatile u_short *)ADRSPC_CONTROL2 = (u_short)flags | 0x02;
	}
    }
    
    /*
     * If we are a Merlot, set up the system CONTROL2 register for the
     * correct memory size, ROM size, and flash size.
     *
     * We also do this if the cookie prom is missing.  Missing prom means
     * we're a Cristal/Cancun.
     */
    asm("	.globl init_processor");
    asm("init_processor:");
    if ((!NORMAL_COOKIE_PROM_PRESENT)  ||
	(cookie_boot->processor == PROCESSOR_CRISTAL) ||
	(cookie_boot->processor == PROCESSOR_CANCUN) ||
	(cookie_boot->processor == PROCESSOR_MERLOT)) {
	register uint got;
	register ushort control2 = 0; /* *must* be register variable */
	
	/*
	 * How wide is our EPROM data bus?  This answer
	 * determines our Merlot memory map)
	 *
	 * flags is set up by the bytek when the eproms are burned
 	 * choices are 16 bit - boot proms
 	 *             32 bit - main system proms (run from prom)
 	 * in the former case, ROMSIZE must be set to
 	 * at least the size of flash memory
	 *
	 * NB - the bits which control this are set by
	 *      the bytek and stuffed into rom.
	 *      See pcmap.h for defs.
	 */
	
 	PUT_DIAG_STR("[OK]\r\nSizing RAM and ROM...");
	
 	switch (flags & FLAGS_BUS_MASK) {
	    
	case FLAGS_BUS_16:      /* boot proms */
	    PUT_DIAG_STR("16 bit wide EPROM...");
	    
	    switch(flags & FLAGS_ROMSIZE_MASK) {
	    case FLAGS_ROMSIZE_16M:
		control2 = CONTROL2_BROMSIZE_4MB;
		PUT_DIAG_STR("16 Mbit EPROMs...");
		break;
	    case FLAGS_ROMSIZE_8M:
		control2 = CONTROL2_BROMSIZE_2MB;
		PUT_DIAG_STR("8 Mbit EPROMs...");
		break;
	    case FLAGS_ROMSIZE_4M:
		control2 = CONTROL2_BROMSIZE_1MB;
		PUT_DIAG_STR("4 Mbit EPROMs...");
		break;
	    case FLAGS_ROMSIZE_2M:
		control2 = CONTROL2_BROMSIZE_512KB;
		PUT_DIAG_STR("2 Mbit EPROMs...");
		break;
	    case FLAGS_ROMSIZE_1M:
	    default:
		control2 = CONTROL2_BROMSIZE_256KB;
		PUT_DIAG_STR("1 Mbit EPROMs...");
	    }
	    
	    control2 <<= CONTROL2_BROMSIZE_SHIFT; /* correct bit pos */
	    /* set Merlot Flash at 2mb, 
	     * not relevant for cristal/cancun 
	     */
	    control2 |= CONTROL2_FLASHSIZE_2MB;
	    break;
	    
	case FLAGS_BUS_32:       /* Main memory proms */
	    PUT_DIAG_STR("32 bit wide EPROM...");
	    
	    switch(flags & FLAGS_ROMSIZE_MASK) {
		
	    case FLAGS_ROMSIZE_8M:
		PUTSTRING(cp,"BAD Flash/EPROM ROMSIZE = 8Mbit");
		asm("    jmp lights");
	    case FLAGS_ROMSIZE_4M:
		control2 = CONTROL2_FLASHSIZE_4MB;
		PUT_DIAG_STR("4 Mbit EPROMs...");
		break;
	    case FLAGS_ROMSIZE_2M:
		control2 = CONTROL2_FLASHSIZE_2MB;
		PUT_DIAG_STR("2 Mbit EPROMs...");
		break;
	    case FLAGS_ROMSIZE_1M:
	    default:
		control2 = CONTROL2_FLASHSIZE_1MB;
		PUT_DIAG_STR("1 Mbit EPROMs...");
	    }
	    break;
	    
	default:
	    control2 = CONTROL2_FLASHSIZE_2MB;
	    PUT_DIAG_STR("Invalid EPROM bus size code...");
	    
	}
	
	/*
	 * Initially set up RAM size bits for the maximum possible, 16 MB:
	 */

	control2 |= (CONTROL2_MSIZE_16MB << CONTROL2_MSIZE_SHIFT);
	*(volatile u_short *)ADRSPC_CONTROL2 = control2;
	
	/*
	 * See if our CPU RAM is 16 bits wide, or 32 bits wide.
	 * Write a 32 bit value to memory location 0, write the word
	 * after it and see if the first word reads back ok.
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
	*(ulong *)0 = (ulong)0xa5fa0fc5;
	*(ulong *)4 = (ulong)0xffffffff;
	got = *(volatile ulong *)0;
	if (got != (ulong)0xa5fa0fc5)
	    goto RAM_is_16_bits_wide;
	*(ulong *)0 = (ulong)0x5a05f03a;
	*(ulong *)4 = (ulong)0xffffffff;
	got = *(volatile ulong *)0;
	if (got == (ulong)0x5a05f03a) {
	    PUT_DIAG_STR("32 bit wide CPU RAM...");
	    
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
	    i = 0;
	    for (et = (long *)0;
		 et < (long *)(RAMPAGES * PAGESIZE);
		 et += (HALF_MEG/4)) {
		*et = (ulong)et;
		
		/*
		 * If a later write has overwritten the zero that we first
		 * wrote to location zero, we have wrapped around, and now
		 * know the end of memory:
		 */
		if (*(long *)0 != 0)
		    break;
		i++;
	    }
	    
	    /*
	     * CRISTAL HACK ALERT!!!
	     * Cristal supports 8 meg of DRAM.  This looks like two 4 meg
	     * banks, which means wrap around will occur at 4 meg also.
	     * Write into the upper 4 meg and see if we get anything.
	     */
	    
	    if (!NORMAL_COOKIE_PROM_PRESENT) {
		if (i == 8) {
		    *(volatile u_short *)ADRSPC_CONTROL2 |= 
			(CONTROL2_MSIZE_8MB << CONTROL2_MSIZE_SHIFT);
		    *et = (ulong)0xdeadbeef;
		    *(et+4) = (ulong)0xaa55aa55;
		    if ((*et == (ulong)0xdeadbeef) && 
			(*(et+4) == (ulong)0xaa55aa55)) {
			et = 0;
			if ((*et != (ulong)0xdeadbeef) || 
			    (*(et+4) != (ulong)0xaa55aa55)) {
			    i = 16;
			}
		    }
		}
	    }
	    
	    asm("	.globl init_found_wrap");
	    asm("init_found_wrap:");
	    control2 &= ~CONTROL2_MSIZE_MASK; /* Clear MSIZE bits */
	    switch(i) {
		
	    case 1:
		/*
		 * We found only 512 KB of CPU RAM:
		 */
		goto RAM_is_16_bits_wide;
		
	    case 2:
		/*
		 * We found 1 MB of CPU RAM:
		 */
		control2 |= (CONTROL2_MSIZE_1MB << CONTROL2_MSIZE_SHIFT);
		PUT_DIAG_STR("Found 1 MB CPU RAM...");
		break;
	    case 4:
		/*
		 * We found 2 MB of CPU RAM:
		 */
		control2 |= (CONTROL2_MSIZE_2MB << CONTROL2_MSIZE_SHIFT);
		PUT_DIAG_STR("Found 2 MB CPU RAM...");
		break;
	    case 8:
		/*
		 * We found 4 MB of CPU RAM:
		 */
		control2 |= (CONTROL2_MSIZE_4MB << CONTROL2_MSIZE_SHIFT);
		PUT_DIAG_STR("Found 4 MB CPU RAM...");
		break;
	    case 16:
		/*
		 * We found 8 MB of CPU RAM:
		 */
		control2 |= (CONTROL2_MSIZE_8MB << CONTROL2_MSIZE_SHIFT);
		PUT_DIAG_STR("Found 8 MB CPU RAM...");
		break;
	    case 32:
		/*
		 * We found 16 MB of CPU RAM:
		 */
		control2 |= (CONTROL2_MSIZE_16MB << CONTROL2_MSIZE_SHIFT);
		PUT_DIAG_STR("Found 16 MB CPU RAM...");
		break;
	    default:
		PUTSTRING(cp,"Found invalid number of 512 KB pages=");
		PUTHEXNUM(i);
		asm("    jmp lights");
	    }
	} else {
	RAM_is_16_bits_wide:
	    PUT_DIAG_STR("16 bit wide CPU RAM...");
	    /*
	     * We know we must have only 512 KB of 16 bit memory:
	     */
	    control2 &= ~CONTROL2_MSIZE_MASK; /* Clear MSIZE bits */
	    control2 |= (CONTROL2_MSIZE_512KB << CONTROL2_MSIZE_SHIFT);
	}
	*(volatile u_short *)ADRSPC_CONTROL2 = control2;
    }
    
    /*
     * Try to set the bus error vector to ignore bus errors.
     * This is the first time we touch main memory.
     */
    SYNOPTICS_LEDS(4,5);
    asm("	.globl init_bev");
    asm("init_bev:");
    
    /*
     * First, set up the other vectors in RAM also, since we will
     * be changing the VBR to point into RAM instead of ROM:
     */
    for (et = &evec; et < &evece; et++)
	*(long *)((u_long)et - (u_long)&evec) = *et;
    
    /*
     * Point the bus error vector at a routine that will simply return
     * from the exception, and update the vector base register to point
     * to the new exception table in RAM:
     */
    *EVEC_BUSERR = (long)catchbe;
    asm ("movc %0, vbr" : : "r" (ADRSPC_RAM));	/* Set VBR */
    
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
    SYNOPTICS_LEDS(5,6);
    PUT_DIAG_STR("Testing low memory...0's...");
    
    asm("	.globl init_zeros");
    asm("init_zeros:");
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
    SYNOPTICS_LEDS(6,7);
    PUT_DIAG_STR("1's...");
    asm("	.globl init_ones");
    asm("init_ones:");
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
    SYNOPTICS_LEDS(7,8);
    PUT_DIAG_STR("address+1...");
    asm("	.globl init_address");
    asm("init_address:");
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
    SYNOPTICS_LEDS(8,9);
    PUT_DIAG_STR("[OK]\r\nSizing memory...");	
    asm("	.globl init_sizing_memory");
    asm("init_sizing_memory:");
    i = icacheon();		/* turn on instruction cache to gain speed */
    memsize = meminit(~conf & DIAG_MASK) << PAGESHIFT;
    if (~conf & DIAG_MASK) {
        PUTSTRING(cp,"memsize() returned 0x");
        PUTHEXNUM(memsize);
    }
    cacheoff();

    start_flh = (struct _flhlog_begin *)FLHLOGMAGIC1; /* init start_flh to */
 				                      /* a known val */
    
    /*
     * Set up the cookie pointer.  It will either point at the cookie prom or
     * the data structure we will fill up from the serial NVRAM
     */
    if (!NORMAL_COOKIE_PROM_PRESENT) {
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
	cookie = (cookie_t *)&nv_cookie;
	ciscopro_flag = cookie->processor & CISCOPRO_FLAG;
	cookie->processor &= ~CISCOPRO_FLAG;
						/* unmask the real processor type.
						   (see cookie.h) */
    } else {
	cookie = cookie_boot;
	ciscopro_flag = 0;
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
	*(volatile u_short *)ADRSPC_RESET |= ALL_NETWORK_RESET;
	asm("nop");
	asm("nop");                     /* Can't call delay() yet! */
	asm("nop");
	asm("nop");
	*(volatile u_short *)ADRSPC_RESET &= ~ALL_NETWORK_RESET;
	break;
	
    case PROCESSOR_BRUT:
	*(volatile u_short *)ADRSPC_CONTROL3 |= (BRUT_RESET_NETWORK_A | 
						 BRUT_RESET_NETWORK_B | 
						 BRUT_RESET_NETWORK_C);
	asm("nop");
	asm("nop");			/* Can't call delay() yet! */
	asm("nop");
	asm("nop");
	asm("nop");
	asm("nop");
	*(volatile u_short *)ADRSPC_CONTROL3 &= ~(BRUT_RESET_NETWORK_A | 
						  BRUT_RESET_NETWORK_B | 
						  BRUT_RESET_NETWORK_C);
	break;
	
    default:
	*(volatile u_short *)ADRSPC_CONTROL |= (CONTROL_RESET_NETWORK_A | 
						CONTROL_RESET_NETWORK_B);
	asm("nop");
	asm("nop");			/* Can't call delay() yet! */
	asm("nop");
	asm("nop");
	asm("nop");
	asm("nop");
	*(volatile u_short *)ADRSPC_CONTROL &= ~(CONTROL_RESET_NETWORK_A | 
						 CONTROL_RESET_NETWORK_B);
	break;
    }
    
    /*
     * Write all of memory outside page 0
     * in order to initialize parity bits.
     * This can't be a "clr", since that instruction reads(!)
     * before writing.  The use of 0xFFFFFFFF means that random
     * jumps will immediately cause Line 1111 Emulator traps.
     */

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
    if (~conf & DIAG_MASK)
	puts("[OK]\nSetting memory to all ones...");
    asm("	.globl init_parity");
    asm("init_parity:");
    
    /*
     * Copy over ourselves to reset parity
     */
    i = icacheon();		/* turn on instruction cache to gain speed */
    if (startup) {
	/*
	 * To avoid clr.x instruction,
	 * need stupid compiler!
	 */
	i = 0;
	for (et = LOWCORE1; et < LOWCORE1 + sizeof(bootstuff); et++)
	    *et = i;
    }
    /* 
     * do two loops so that we can skip flhboot buffer if present 
     * flhbuf_start and flhbuf_end are set above as per system mem size
     * if the buffer is present.  Otherwise they are set to 'memsize'
     */
    
    for (et = LOWCORE1 + sizeof(bootstuff); et < (long *)flhbuf_start; et++)
	*et = 0xFFFFFFFF;  /* two "unimplemented 1111's" */
    for (et = (long *)flhbuf_end; et < (long *)memsize; et++)
	*et = 0xFFFFFFFF;  /* two "unimplemented 1111's" */
    cacheoff();
    if (~conf & DIAG_MASK)
	puts("[OK]\n");
    
    /*
     * Copy exception table from the data area to the exception area.
     */
    SYNOPTICS_LEDS(10,11);
    if (~conf & DIAG_MASK)
	puts("Setting up exception vectors...");
    asm("	.globl init_vectors");
    asm("init_vectors:");
    buserrhandler = 0;		/* Report bus errors */
    for (et = &evec; et < &evece; et++)
	*(long *)((u_long)et - (u_long)&evec) = *et;
    /* kludge to install a unique timer interrupt service routine */
    /* for the brut project. brut timers are accessed differently */
    /* and the registers are at different addressses. to prevent */
    /* run time checks in timer isr's, a seperate wrapper and isr*/
    /* is installed for the brut box */
    if (cookie->processor == PROCESSOR_BRUT || 
	cookie->processor == PROCESSOR_CANCUN)
	*EVEC_LEVEL7 = (long)(&refreshe_brut);
    
    /*
     * Enable the processor data cache, now that we've tested memory:
     * (initc() in lowcore.S turned on the instruction cache before)
     */
    if (~conf & DIAG_MASK)
	puts("[OK]\nEnabling processor caches...");
    asm("	.globl init_enable_caches");
    asm("init_enable_caches:");
    if(cookie->processor == PROCESSOR_BASS) {
        /* set up the access control unit */
        setacu(0,0x02008507);  /* cache all but the second 16mb segment */
    }
    i = cacheon();		/* Enable both caches */
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
    default:
        cpu_type = CPU_UNKNOWN;
	break;
    }
    
    if (~conf & DIAG_MASK)
	puts(cputypes[cpu_type]);
    
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
    /*
     * Initialize the counter/timers:
     */
    SYNOPTICS_LEDS(11,12);
    if (~conf & DIAG_MASK)
	puts("[OK]\nEnabling timers...");
    asm("	.globl init_timers");
    asm("init_timers:");
    /*
     * Initialize Counter 2 for our 4 ms real time clock interrupt:
     */
    clock = 0;			/* zero the ms clock */
    hclock = 0;			/* zero the upper ms clock */
    /* must check for a BRUT/CANCUN box here. These boxes have moved the
     * counter registers to a different location. Also, the
     * setting of the mode is now done in hardware. */ 
    if (cpu_type == CPU_IGS_BRUT || cpu_type == CPU_CANCUN) {
	if(cpu_type == CPU_IGS_BRUT) {
	    /* clear the reset line for the timer chip */
	    *(short *)ADRSPC_CONTROL3 &= ~(BRUT_RESET_COUNTER);
	}
        /* this op can be done 16 bits at a time on the brut */
        /* box, other boxes can only handle 8 bits at a time */
        *(short *)ADRSPC_BRUT_TIMER2 = REFRFREQUENCY;
    } else { /* not a BRUT box */
        /* set the mode */
        ctaddr->control = SEL_COUNTER2 | RW_LSBMSB | CT_MODE2;
        asm("nop");		/* 68030 on Merlot is too fast! */
        asm("nop");
        ctaddr->counter2 = REFRFREQUENCY & 0xFF;
        asm("nop");		/* 68030 on Merlot is too fast! */
        asm("nop");
        ctaddr->counter2 = (REFRFREQUENCY >> 8) & 0xFF;
        asm("nop");		/* 68030 on Merlot is too fast! */
        asm("nop");
    }
    
    /*
     * Initialize Counter 0 as watchdog -- 4 times refresh rate
     */
    /* must check for a BRUT/CANCUN box here. These boxes have moved the
     * counter registers to a different location. Also, the
     * setting of the mode is now done in hardware. */ 
    if (cpu_type == CPU_IGS_BRUT || cpu_type == CPU_CANCUN) {
        /* this op can be done 16 bits at a time on the brut
         * box, other boxes can only handle 8 bits at a time */
        *(short *)ADRSPC_BRUT_TIMER0 = REFRFREQUENCY * 4;
    } else { /* not a BRUT box */
        ctaddr->control = SEL_COUNTER0 | RW_LSBMSB | CT_MODE0;
        asm("nop");		/* 68030 on Merlot is too fast! */
        asm("nop");
        ctaddr->counter0 = (REFRFREQUENCY*4) & 0xFF;
        asm("nop");		/* 68030 on Merlot is too fast! */
        asm("nop");
        ctaddr->counter0 = ((REFRFREQUENCY*4) >> 8) & 0xFF;
        asm("nop");		/* 68030 on Merlot is too fast! */
        asm("nop");
    }
    
    /*
     * The following write clears any existing IRQ from the i8254
     * so that the timers will start interrupting us:
     */
    *(volatile u_char *)ADRSPC_TCONTROL = 0;
    
    /*
     * Perform a quick go/no go test on the 4 ms real time clock:
     */
    SYNOPTICS_LEDS(12,13);
    asm("	.globl init_tclock");
    asm("init_tclock:");
    delay(4);			/* Wait at least 4 ms */
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
    SYNOPTICS_LEDS(13,14);
    if (~conf & WATCHDOG_MASK) {
	if (~conf & DIAG_MASK)
	    puts("Watchdog timer disabled...");
    } else {
	if (~conf & DIAG_MASK)
	    puts("Enabling Watchdog timer...");
	*(volatile u_char *)ADRSPC_TCONTROL |= TCONTROL_WE;
    }
    
    /*
     * Size NVRAM via Bus error detection. We don't want to size memory via
     * detecting on a write, because we don't want to be left holding the bag
     * if we lose power before restoring the NVRAM location...
     */
    SYNOPTICS_LEDS(14,15);
    if (~conf & DIAG_MASK)
	puts("[OK]\nSizing NVRAM...");
    asm("	.globl init_size_nvram");
    asm("init_size_nvram:");
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
    SYNOPTICS_LEDS(15,16);
    if (~conf & DIAG_MASK)
	puts("[OK]\nSearching for system code...");
    asm("	.globl init_find_system");
    asm("init_find_system:");
    buserrcnt = 0;		/* No bus error yet */
    buserrhandler = buserrnote; /* Catch and count bus errors */
    prom2 = (u_short *)NULL;
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
    buserrhandler = 0;		/* Report bus errors again */
    if (~conf & DIAG_MASK)
	if (prom2 != NULL)
	    printf("found at %x, magic number = %x...",prom2, *prom2);
	else
	    puts("no system image found...");
    
    /*
     * Variable initializations
     */
    SYNOPTICS_LEDS(16,17);
    if (~conf & DIAG_MASK)
	puts("[OK]\nSetting up monitor variables...");
    asm("	.globl init_variables");
    asm("init_variables:");
    
    switch (cookie->processor) {
    case PROCESSOR_BASS:
        /* the bass box has 2 Mbytes of IO RAM. the first 1 Mbyte is
         * used by the CPU and the top 1 Mbyte is used for IO memory. 
         * set the base of IO memory and then adjust the amount of
         * memory to show only the cpu memory. cpu and i/o memory
         * are contiguous.
         */
	
        if (memsize != TWO_MEG)
            printf("\nERROR: Expected 2048K of memory, found %dK", 
		   memsize/1024);
	
        iomem_size = ONE_MEG;
        iomem_base = memsize - iomem_size;
        memsize -= iomem_size;
	break;
	
    case PROCESSOR_CANCUN:
    case PROCESSOR_BRUT:
	
        /* 
	 * Brut can have 2 MB (Brut I) or 8 MB (Brut II).  In either case,
	 * the following code for Cancun gets the right answer.
	 *
	 * Cancun may have from 2Mbg to 18Mb of system/IO memory.  
	 * If no expansion memory is inserted into the box then the first 
	 * meg is used by the CPU and the second is used for IO memory. 
	 * 
	 * If any memory is added to the box then the IO memory space is 
	 * expanded to 2 Meg.
	 * 
	 * By special hardware arrangement, if 16 meg of RAM is installed 
	 * then the IO RAM is mapped into address ADDRSPC_MERLOT_IOMEM 
	 * (and is still 2 meg in length)
	 */
	
	if (memsize <= TWO_MEG) { /* No expansion simms installed */
	    iomem_size = ONE_MEG;
	    iomem_base = memsize - iomem_size;
	    memsize -= iomem_size;
	} else if (memsize < SIXTEEN_MEG) { /* 16 Meg > memory size > 2 Meg */
	    iomem_size = TWO_MEG;
	    iomem_base = memsize - iomem_size;
	    memsize -= iomem_size;
	} else {
            /*
             * May be either 16MB + 2MB soldered-on, or only 16MB. Do a quickie
             * test to see if the 2MB exists, and if so, perform a more 
             * extensive test. If it doesn't exist, set up the variables for
             * 16MB only.
             */
	    iomem_base = ADRSPC_MERLOT_IOMEM;
	    iomem_size = TWO_MEG;

            buserrcnt = 0;
            buserrhandler = buserrnote; /* Catch and count bus errors */
	    *(ulong *) iomem_base = (ulong) 0x03131964; /*A magic date indeed!*/

            if (buserrcnt) {
                PUT_DIAG_STR("No 2MB soldered-on...16MB only...");
	        iomem_base = memsize - iomem_size;
	        memsize -= iomem_size;
            } else {
                PUT_DIAG_STR("16MB + 2MB DRAM...");
	        memsize = SIXTEEN_MEG;
	        /* 
	         * now need to go and verify that iomem is good since we've only
	         * tested the 16 Meg simms.
	         */
                PUT_DIAG_STR("I/O -- Memory 1's...");
	        asm("	.globl init_io_ones");
	        asm("init_io_ones:");
	        for (et = (long *)iomem_base; 
		     et <(long *)(iomem_base + iomem_size); et++) {
		    *et = 0xFFFFFFFF;
	        }
	        for (et = (long *)iomem_base; 
		     et < (long *)(iomem_base + iomem_size); et++) {
		    i = *et;
		    if (i != 0xFFFFFFFF) {
		        PUTSTRING(cp,"\r\nBad I/O memory - wrote all ones, read 0x");				
		        PUTHEXNUM(i);
		        PUTSTRING(cp," at location 0x");
		        PUTHEXNUM((unsigned)et);
		        asm("    jmp lights");
		    }
	        }
                PUT_DIAG_STR("I/O -- address+1...");
	        asm("	.globl init_io_address");
	        asm("init_io_address:");
	        for (et = (long *)iomem_base; 
		     et < (long *)(iomem_base + iomem_size); et++) {
		    *et = (long) (et + 1);
	        }
	        for (et = (long *)iomem_base; 
		     et < (long *)(iomem_base + iomem_size); et++) {
		    j = (long) (et + 1);
		    i = *et;
		    if (i != (long)j) {
		        PUTSTRING(cp,"\r\nBad I/O memory - wrote 0x");
		        PUTHEXNUM(j);
		        PUTSTRING(cp,", read 0x");
		        PUTHEXNUM(i);
		        PUTSTRING(cp," at location 0x");
		        PUTHEXNUM((unsigned)et);
		        asm("    jmp lights");
		    }
	        }
	    }
            buserrhandler = 0;		/* Report bus errors again */
	}
	break;
	
    case PROCESSOR_CRISTAL:
	iomem_base = ADRSPC_MERLOT_IOMEM;
	/*
	 * Size IOMEM via Bus error detection.
	 */
	if (~conf & DIAG_MASK)
	    puts("[OK]\nSizing IOMEM...");
	asm("	.globl init_size_iomem");
	asm("init_size_iomem:");
	buserrcnt = 0;		/* No bus error yet */
	buserrhandler = buserrnote; /* Catch and count bus errors */
	for (cp = (char *)ADRSPC_MERLOT_IOMEM;
	     (cp < (char *)ADRSPC_MERLOT_IOMEM_END) && (buserrcnt == 0);
	     cp += 1024) {
#if FALSE
	    i = *(volatile u_char *)cp;
#endif
	    i = 0xFF;
	    *cp = i;
	}
	iomem_size = (uint)(cp - ADRSPC_MERLOT_IOMEM);
	buserrhandler = 0;		/* Report bus errors again */
	if (~conf & DIAG_MASK)
	    printf("found %dK bytes of IOMEM", iomem_size/1024);
	
	asm("	.globl init_iomem_parity");
	asm("init_iomem_parity:");
	/*
	 * Init iomem to all 1's for parity
	 */
	if (~conf & DIAG_MASK)
	    puts("\nSetting IO memory to all ones...");
	for (et = (long *)iomem_base; et < (long *)(iomem_base + iomem_size); 
	     et++)
	    *et = 0xFFFFFFFF;
	if (~conf & DIAG_MASK)
	    puts("[OK]\n");
	break;
    default:
        /* Compute the start of IO memory. If we have less than 
         * 16 MB of RAM, the IO memory is contiguous with the
         * CPU RAM. Adjust the amount of memory to show only the
         * cpu memory. If we have 16 MB of RAM, IO memory moves
         * to a different location and cpu mem is not adjusted. */
        iomem_size = HALF_MEG;
        if (memsize < SIXTEEN_MEG) {
	    iomem_base = memsize - iomem_size;
            memsize -= iomem_size;
        }
        else
	    iomem_base = ADRSPC_MERLOT_IOMEM;
	break;
    }
    
    if (~conf & DIAG_MASK)
	printf("\nfound %d total Kbytes of DRAM, iomem_base set to %x", 
	       memsize/1024, iomem_base);
    
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
    SYNOPTICS_LEDS(17,18);
    if (~conf & DIAG_MASK)
	puts("[OK]\nReading hardware revision register...");
    asm("	.globl init_rev");
    asm("init_rev:");
    control = *(volatile u_short *)ADRSPC_CONTROL;
    rev = *(volatile u_short *)ADRSPC_HW_REVISION;
    control = *(volatile u_short *)ADRSPC_CONTROL;
    if ((cookie->processor == PROCESSOR_PAN) && (rev == control)) {
	
	/*
	 * We have a rev I or rev II Pancake, so set the revision to 0:
	 */
	rev = 0;
    }
    
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
    if (cookie->vendor == VENDOR_SYNOPTICS) {
	CLR_SYNOPTICS_LEDS(syn_leds, SYNOPTICS_LED_STEP_18);
	SET_SYNOPTICS_LEDS(syn_leds, SYNOPTICS_LED_STEP_19);
    }
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
    SYNOPTICS_LEDS(19,20);
    asm("	.globl init_test_parity");
    asm("init_test_parity:");
    if (cpu_type == CPU_CANCUN) {
	register ulong dummy;	/* *needs* to be register variable */
	
	/* 
	 * now get disable nmi's.  YES you can do this on cancun. By 
	 * writing a one (0x01) to counter 2 it sets up a mode where the
	 * timer will never generate an interrupt -- It will continually
	 * reload a 0x01 -- as the interrrupt is generated because of the
	 * transisition from 2 to 1.
	 */
	*(short *)ADRSPC_BRUT_TIMER2 = 0x01;
	
	/* need to disable watchdog */
	
	if (!(~conf & WATCHDOG_MASK)) {
	    *(volatile u_char *)ADRSPC_TCONTROL &= ~TCONTROL_WE;
	}
	delay(1);		/* delay to make sure timers loaded */
	
	/* 
	 * we can really generate parity errors with cancun so generate
	 * a couple of errors and make sure the parity logic works correctly
	 */
	cacheoff();
	if (~conf & DIAG_MASK)
	    puts("\n\rParity Logic tests...");
	cancun_scr = 0;
	buserrhandler = parity_checker_handler;
	/* 
	 * enable EVEN parity 
	 */
	
	*((ushort *)ADRSPC_CONTROL) |= (CANCUN_EVEN_PARITY); 
	
	/* 
	 * access to real memory should generate a parity error 
	 */
	
	tmp_parity_test_location = 0;
	
	/* 
	 * now restore normal parity checking and read location
	 * this should cause a parity error 
	 */
	
	/* 
	 * enable odd parity 
	 */
        *((ushort *)ADRSPC_CONTROL) &= ~(CANCUN_EVEN_PARITY); 
	*((ushort *)ADRSPC_CONTROL) |= CANCUN_PARITY_ENABLE;/* enable parity */
	
	dummy = tmp_parity_test_location;
	
	buserrhandler = 0;	/* remove the bus handler */
	
	/* 
	 * set up nmi's to run again
	 */
        *(short *)ADRSPC_BRUT_TIMER2 = REFRFREQUENCY;
	if (~conf & WATCHDOG_MASK) {
	} else {
	    *(volatile u_char *)ADRSPC_TCONTROL |= TCONTROL_WE;
	}
	
	/* check results of accessing memory */
	
	asm("	.globl init_test_parity1");
	asm("init_test_parity1:");
	cancun_scr &= (CONTROL_PARITY0 | CONTROL_PARITY1 | 
		       CONTROL_PARITY_ERROR);
	if (cancun_scr != (CONTROL_PARITY0 | CONTROL_PARITY1 | 
			   CONTROL_PARITY_ERROR) ) {
	    PUTSTRING(cp,"\r\n failed, SCR read 0x");
	    PUTHEXNUM(cancun_scr);
	    PUTSTRING(cp,"(masked), expected 0x");
	    PUTHEXNUM((CONTROL_PARITY0 | CONTROL_PARITY1 | 
		       CONTROL_PARITY_ERROR));
	    cacheon();
	    asm("    jmp lights");
	} else {
	    if (~conf & DIAG_MASK)
		puts("[OK]");
	}	    
	cacheon();
    } else {
	for (i = 0; i < BUFSIZ; i++) {
	    bufu.buf[i] = (char)i;
	    j = bufu.buf[i];
	}
    }    
    SYNOPTICS_LEDS(20,21);
    
    /*
     * Light the run lamp:
     */
    *(volatile u_short *)ADRSPC_CONTROL |= CONTROL_RUNLAMP;
    
    SYNOPTICS_LEDS(21,22);
    
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
	*(volatile u_short *)ADRSPC_RESET |= ALL_NETWORK_RESET;
	delay(5);
	*(volatile u_short *)ADRSPC_RESET &= ~ALL_NETWORK_RESET;
        delay(5);

        /*
         * reset SCR3 RST T bit does not clear modem control interrupts.
         * So, we need to specifically do it on Serial channel 1 & 2
         * device registers.
         */
        *(ushort *)ADRSPC_DEVICE_SERIAL_0 &= HD64570_ALL_INT_CLEAR;
        *(ushort *)ADRSPC_DEVICE_SERIAL_1 &= HD64570_ALL_INT_CLEAR;

        /*
         * For the Cobra, the reset bits to reset the low-speed serial
         * interfaces are in the LSS reset register. We need to reset
         * the CD2430 controllers here using those bits.
         */
        if ((cookie->interfaces == INTERFACE_1E2T1B2AS) ||
            (cookie->interfaces == INTERFACE_1R2T1B2AS)) {
            (*(ushort *)ADRSPC_LSS_RESET_ASEL) &= DEVICE_LSS_RESET_MASK;
            delay(5);
            (*(ushort *)ADRSPC_LSS_RESET_ASEL) |= ~DEVICE_LSS_RESET_MASK;
        }

        /*
         * 2500 Derivative specific stuff
         */
        switch (cookie->interfaces) {
            case INTERFACE_1E1B2W:
            case INTERFACE_1R1B2W:
                /* Prevent CSU/DSUs from generating interrupts */
                BOA_MASK_ALM_RDY_INTERRUPT((uchar *) ADRSPC_BOA_WAN0_MISC);
                BOA_MASK_ALM_RDY_INTERRUPT((uchar *) ADRSPC_BOA_WAN1_MISC);
                break;
            default:
                break;
        }

	break;
	
    case PROCESSOR_BRUT:
	*(volatile u_short *)ADRSPC_CONTROL3 |= (BRUT_RESET_NETWORK_A | 
						 BRUT_RESET_NETWORK_B | 
						 BRUT_RESET_NETWORK_C);
	delay(5);
	*(volatile u_short *)ADRSPC_CONTROL3 &= ~(BRUT_RESET_NETWORK_A | 
						  BRUT_RESET_NETWORK_B | 
						  BRUT_RESET_NETWORK_C);
	break;
	
    default:
	*(volatile u_short *)ADRSPC_CONTROL |= (CONTROL_RESET_NETWORK_A | 
						CONTROL_RESET_NETWORK_B);
	delay(5);
	*(volatile u_short *)ADRSPC_CONTROL &= ~(CONTROL_RESET_NETWORK_A | 
						 CONTROL_RESET_NETWORK_B);
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
    tmp_parity_test_location = 1; /* indicate that we have  bus execption */
    /* and get parity correct at that addr */
    cancun_scr = *((ushort *)ADRSPC_CONTROL);/* get status register and hold */
    /* in cpu register. This also */
    /* clears int parity error */
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
    register uint   *reg_dptr;
    register uint   reg_d;
    volatile ushort *control_ptr;
    uint            old_cacr;
    ushort          level;
    
    /*
     * Disable all maskable interrupts during this code:
     */
    level = sil(7);
    
    /*
     * Set CACR to disable instruction and data caches while we are
     * in this subroutine:
     */
    old_cacr = setcacr(CLEAR_CACHES);
    
    /*
     * Get data pointer and data in registers so that we don't
     * reference the stack while parity is set wrong:
     */
    reg_dptr = location;
    reg_d = data;
    
    /*
     * Write data to location with odd parity to make compiler
     * put pointer and data into registers:
     */
    *reg_dptr = reg_d;
    *reg_dptr = ~reg_d;
    
    /*
     * Set control register for even parity in main memory
     * instead of the usual odd parity:
     */
    control_ptr = (volatile ushort *)ADRSPC_CONTROL;
    switch (cpu_type) {
    case CPU_CANCUN:
	*control_ptr |= (CANCUN_EVEN_PARITY); 
	break;
    default:
	*control_ptr |= CONTROL_EVENPAR;
	break;
    }
    /*
     * Write data to location with even parity using the pointer
     * and data squirreled away in registers:
     */
    *reg_dptr = reg_d;
    
    /*
     * Set control register back to usual odd parity in main memory
     * to generate and check odd parity before returning:
     */
    switch (cpu_type) {
    case CPU_CANCUN:
	*control_ptr &= ~(CANCUN_EVEN_PARITY); 
	break;
    default:
	*control_ptr &= ~CONTROL_EVENPAR;
	break;
    }
    
    /*
     * Let write to control register happen while we execute code from ROM:
     */
    asm("	nop");
    
    /*
     * Restore the cache states:
     */
    setcacr(old_cacr);
    
    /*
     * Restore the previous interrupt level:
     */
    (void)sil(level);
    
    return(0);
}
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
