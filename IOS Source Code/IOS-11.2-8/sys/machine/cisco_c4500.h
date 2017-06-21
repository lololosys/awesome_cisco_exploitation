/* $Id: cisco_c4500.h,v 3.6.12.1 1996/06/04 22:55:10 mbeesley Exp $
 * $Source: /release/112/cvs/Xsys/machine/cisco_c4500.h,v $
 *------------------------------------------------------------------
 * cisco_c4500.h - Definitions for the R4600 based C4500
 *
 * October 1993, Lele Nardin
 * December 1994, Scott Mackie (moved and altered from sys/h)
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cisco_c4500.h,v $
 * Revision 3.6.12.1  1996/06/04  22:55:10  mbeesley
 * CSCdi59380:  Optimize buffer code for traditional platforms
 * Branch: California_branch
 *
 * Revision 3.6  1996/02/13  20:38:55  hampton
 * Move buffer initialization to the platform_xxx.c files to eliminate
 * platform dependencies from common code.  [CSCdi48844]
 *
 * Revision 3.5  1996/02/02  04:29:18  mbeesley
 * CSCdi48003:  Buffer manager not optimal on all boxes
 * Make pak_set_dirty() and pak_ignore_inputq() platform specific.
 *
 * Revision 3.4  1995/12/23  13:10:09  hampton
 * Fix a timing window where interfaces can be mistakenly throttled.
 * [CSCdi44951]
 *
 * Revision 3.3  1995/12/21  16:29:09  hampton
 * Switch the 68K version of DELAY() to call the existing usecdelay
 * routine which uses the timer chip.  For all other platforms, move a
 * routine or two between files.  [CSCdi45965]
 *
 * Revision 3.2  1995/11/17  17:50:02  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:40:10  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/11/08  21:15:48  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.2  1995/10/15  18:53:59  eschaffe
 * CSCdi40294:  Increase the number of Protocol Translation sessions
 *
 * Revision 2.1  1995/06/07  21:43:13  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * The c4500 family supports variable speed r4k cpus. The
 * c4500 has a 100Mhz processor. The c4700 has a 150 Mhz processor
 */
#define MULTIPLE_R4K_SPEEDS


/*
 * The C4500 uses ELF images
 */
#include "image_elf.h"


/*
 * Interrupt level assignments for system software
 *
 *	2 - Serial modem control state
 *	2 - Lance and MK5025
 *	4 - Console UART
 *	7 - NMI Tick (internal timer)
 */

/*
 * Interrupt levels
 */
#define NMI_LEVEL 7		/* NMI Timer level */
#define SHMEM_PARLEVEL 6	/* Sh. memory parity error level */
#define LEVEL_CONSOLE 4		/* console UART */
#define SMT_INTLEVEL 3		/* SMT fddi level */
#define ETHER_DISABLE 2	        /* priority level to shut off Ethernet */
#define ETHER_INTLEVEL 2	/* another name */
#define SERIAL_DELTA_INTLEVEL 2 /* High speed serial interface state changed */

#define WASTETIME(usecs) wastetime(usecs)
#define DELAY(ms)        msecdelay(ms)

/*
 * Base of Memory
 */
#define RAMBASE       	0x60000000
#define PHY_RAMBASE	RAMBASE

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

#define REFRESHTIME          4          /* 4ms per tick */                 
#define NMI_FREQ_NUMERATOR   250        /* Numerator of NMI frequency   */
#define NMI_FREQ_DENOMINATOR 1	        /* Denominator of NMI frequency */

/*
 * Address of UART registers
 */
#define UARTDEVBASEADDR	0x3E820800

/*
 * R4K CPU Config and Status register initialization
 * Status Register : 	Coprocessor 0,1 usable
 *			32 FPU registers
 *			Bootstrap exceptions
 *			KX,SX,UX all 32 bit
 *			ERL,EXL reset to 0
 *
 * Config Register :	K0 set to cached non-coherent
 */
		
#define R4K_SREG_SETUP          0x34400000
#define R4K_CONFIG_SETUP        0x00000033

#ifndef ASMINCLUDE
/*
 * Main NVRAM define and prototype
 */
extern void bcopy_32(void const *, void *, int);
#endif

#define NVBCOPY(s, d, l)	bcopy_32(s, d, l)

/* for cancun NV EEPROM support -- */
#define NVADDRESS(base, offset) (base + offset)

#define NVBASE			ADRSPC_NVRAM_START

/*
 * Write buffer flush routines
 */
#ifndef ASMINCLUDE

extern int flush_io_wbuffer(void);
extern int flush_dram_wbuffer(void);
extern int flush_shmem_wbuffer(void);
extern void flush_all_wbuffers(void);

/*
 * C4500 does not cache packets in shared memory
 */
#define pak_set_dirty(x)

/*
 * C4500 never ignores the input holdqueue count
 */
#define pak_ignore_inputq(pak)	FALSE

/*
 * C4500 never deals with scattered packets
 */
#define pak_has_particles(pak)	FALSE

#endif


/*
 * Max possible lines for c4500 platform.
 */
#define PLATFORM_MAXLINES 1002

/*
 * Common interrupt levels
 */

#define PLATFORM_ALL_DISABLE   5  /* disable all interrupts */
#define PLATFORM_TTY_DISABLE   4  /* priority to shut off all types of TTY */
#define PLATFORM_NETS_DISABLE  3  /* highest priority of a network device */
#define PLATFORM_ALL_ENABLE    0  /* enable all interrupts */

#define	PLATFORM_FAMTYPE FAMILY_SIERRA

/*
 * Nonstandard buffer pool sizes
 */
#define C4500_BIG_MAX1 60
