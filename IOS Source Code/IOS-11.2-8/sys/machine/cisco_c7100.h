/* $Id: cisco_c7100.h,v 3.1.68.2 1996/06/04 22:55:11 mbeesley Exp $
 * $Source: /release/112/cvs/Xsys/machine/cisco_c7100.h,v $
 *------------------------------------------------------------------
 * cisco_c7100.h - Definitions for R4700 based Predator (c7100)
 *
 * October 1995, Michael Beesley
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cisco_c7100.h,v $
 * Revision 3.1.68.2  1996/06/04  22:55:11  mbeesley
 * CSCdi59380:  Optimize buffer code for traditional platforms
 * Branch: California_branch
 *
 * Revision 3.1.68.1  1996/03/21  22:57:16  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.1.2.8  1996/02/02  06:10:09  mbeesley
 * Branch: ELC_branch
 * Mini-sync ... pull in buffer manager tweaks.
 *
 * Revision 3.1.2.7  1996/01/15  23:54:38  dcarroll
 * Branch: ELC_branch
 * Sync ELC_branch between ELC_baseline_120995 and ELC_baseline_960110.
 *
 * Revision 3.1.2.6  1995/12/09  20:48:36  mbeesley
 * Branch: ELC_branch
 * Fix UART address, GT64010 Vendor/Dev ID and DRAM config registers.
 *
 * Revision 3.1.2.5  1995/12/08  05:33:16  mbeesley
 * Branch: ELC_branch
 * Mini-sync ... get correct ID fields, and parse chains.
 *
 * Revision 3.1.2.4  1995/12/05  12:51:07  dcarroll
 * Branch: ELC_branch
 *
 * Revision 3.1.2.3  1995/12/02  00:16:49  mbeesley
 * Branch: ELC_branch
 * Make the software know that the CPU is running at 150 Mhz.
 *
 * Revision 3.1.2.2  1995/11/30  01:25:08  dcarroll
 * Branch: ELC_branch
 *
 * Revision 3.1.2.1  1995/11/22  20:31:11  mbeesley
 * Initial commit of c7100 support.
 * Branch: ELC_branch
 *
 * Revision 3.1  1995/11/20  21:36:43  mbeesley
 * Add placeholder files for future development.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * The c7100 family runs at 150 Mhz. However, fixed speed boxes
 * are assumed to be 100 Mhz, so we define multiple speeds here.
 */
#define MULTIPLE_R4K_SPEEDS

/*
 * The C7100 uses ELF images
 */
#include "image_elf.h"

/*
 * Interrupt levels
 */
#define NMI_LEVEL 		7 /* NMI Timer level */
#define LEVEL_ERROR		5 /* Errors, Hot swap etc */
#define LEVEL_CONSOLE 		4 /* console UART */
#define LEVEL_PA_MANAGEMENT	3 /* Port Adaptor Hi IRQ */
#define LEVEL_DMA		2 /* Galileo DMA engine */
#define LEVEL_PA_NETWORK	1 /* Network IO level */

#define WASTETIME(usecs) 	wastetime(usecs)
#define DELAY(ms)        	msecdelay(ms)

/* 
 * Max possible lines for Predator
 */
#define PLATFORM_MAXLINES 1002

/*
 * Highest numbered PA slot (zero-based)
 */
#define MAX_IF_SLOT		6

/*
 * Maximum number of slots and interfaces
 */
#define MAX_SLOTS               7
#define MAX_CTRLR_INTERFACES    8

/*
 * Base of Memory
 */
#define RAMBASE       		0x60000000
#define PHY_RAMBASE		0x00000000

/*
 * Define the position of the pointer to the crashinfo structure in the
 * core file.  This pointer (and data structure) are used by GDB as a
 * starting point for analyzing a crash dump.  This location does not
 * need to survive a reload, it just needs to appear in the crash dump.
 * If you change this value, you must coordinate with Cisco's GDB
 * maintainers.
 *
 * Note: The offset into DRAM of the pointer to the crashinfo structure
 * is dependent on the exception handling code in asm-mips/asm_4k.S.  Do
 * not change it without changing the asm code.
 */
#define CRASH_INFO_OFFSET	0x250

#define CRASH_INFO	 	(*((crashinfo **)(R4K_K0BASE + 		\
						  CRASH_INFO_OFFSET)))


#define REFRESHTIME          	4   	/* 4ms per tick */                 
#define NMI_FREQ_NUMERATOR   	250  	/* Numerator of NMI frequency   */
#define NMI_FREQ_DENOMINATOR 	1      	/* Denominator of NMI frequency */

/*
 * Address of UART registers
 */
#define UARTDEVBASEADDR		0x1E840400

/*
 * R4K CPU Config and Status register initialization
 */
#define R4K_SREG_SETUP          0x34400000
#define R4K_CONFIG_SETUP        0x00000033

/*
 * Data Cache line size
 */
#define C7100_DCACHE_LINE_SIZE	32

/*
 * Base of NVRAM
 */
#define NVBASE			ADRSPC_NVRAM_START

#define NVBCOPY(s, d, l)	bcopy(s, d, l)

#ifndef ASMINCLUDE

/*
 * Write buffer flush routines
 */
extern int 	flush_io_wbuffer(void);
extern void 	flush_all_wbuffers(void);

/*
 * Scheduler loop end support
 */
extern void	c7100_sched_loop_end(boolean);

/*
 * Support protected writes to the dbus registers.
 * These need to be defined as the PA subsystem uses hes/dgbus.h
 * to get the format of the EEPROM on each PA.
 */
typedef uchar dbus_protect_t;

#define dbus_protect(statep)	(*(statep) = 0)
#define dbus_restore(statep)

/*
 * C7100 does not cache packets in shared memory
 */
#define pak_set_dirty(x)

/*
 * C7100 never ignores the input holdqueue count
 */
#define pak_ignore_inputq(pak)	FALSE

/*
 * C7100 deals with scattered packets
 */
#define pak_has_particles(pak)	platform_pak_has_particles_inline(pak)

#endif	/* ASMINCLUDE */

/*
 * Common interrupt levels
 */
#define PLATFORM_ALL_DISABLE   	4  /* disable all interrupts */
#define PLATFORM_TTY_DISABLE   	4  /* priority to shut off all types of TTY */
#define PLATFORM_NETS_DISABLE  	3  /* highest priority of a network device */
#define PLATFORM_ALL_ENABLE    	0  /* enable all interrupts */

#define	PLATFORM_FAMTYPE 	FAMILY_C7100

/* end of file */
