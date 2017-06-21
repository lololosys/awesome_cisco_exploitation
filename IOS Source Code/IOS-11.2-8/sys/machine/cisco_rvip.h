/* $Id: cisco_rvip.h,v 3.5.20.3 1996/06/04 22:55:16 mbeesley Exp $
 * $Source: /release/112/cvs/Xsys/machine/cisco_rvip.h,v $
 *------------------------------------------------------------------
 * cisco_rvip.h - Definitions for the Checkers VIP
 *
 * January 1995, Michael Beesley
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cisco_rvip.h,v $
 * Revision 3.5.20.3  1996/06/04  22:55:16  mbeesley
 * CSCdi59380:  Optimize buffer code for traditional platforms
 * Branch: California_branch
 *
 * Revision 3.5.20.2  1996/05/23  03:58:31  kao
 * CSCdi58288:  inconsistent ip cache in VIP after mic rel with traffic
 * Branch: California_branch
 *
 * Revision 3.5.20.1  1996/03/21  22:57:17  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.2.2.3  1996/02/02  06:10:13  mbeesley
 * Branch: ELC_branch
 * Mini-sync ... pull in buffer manager tweaks.
 *
 * Revision 3.2.2.2  1996/01/15  23:54:47  dcarroll
 * Branch: ELC_branch
 * Sync ELC_branch between ELC_baseline_120995 and ELC_baseline_960110.
 *
 * Revision 3.2.2.1  1995/12/08  05:33:18  mbeesley
 * Branch: ELC_branch
 * Mini-sync ... get correct ID fields, and parse chains.
 *
 * Revision 3.4  1995/12/23  13:10:13  hampton
 * Fix a timing window where interfaces can be mistakenly throttled.
 * [CSCdi44951]
 *
 * Revision 3.3  1995/12/21  16:29:14  hampton
 * Switch the 68K version of DELAY() to call the existing usecdelay
 * routine which uses the timer chip.  For all other platforms, move a
 * routine or two between files.  [CSCdi45965]
 *
 * Revision 3.2  1995/11/17  17:50:09  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:40:17  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:15:51  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * The RVIP uses ELF images
 */
#include "image_elf.h"

/*
 * Interrupt level assignments for system software
 *
 *	1 - Port Adaptor Network IO		(direct)
 *	2 - DMA Engine				(direct)
 *	3 - Port Adaptor Management Interrupt	(nevada)
 *	4 - Errors, Hot Swap etc		(nevada)
 *	5 - Timer (msclock)			(direct)
 *	6 - DBUS Interrupt			(direct)
 *	7 - MALU Attention Interrupt		(direct)
 */

/*
 * Interrupt levels
 */
#define LEVEL_CBUS	 	7 /* Malu Attention level */
#define LEVEL_DBUS		6 /* Dbus level */
#define NMI_LEVEL		5 /* Timer level (msclock) */
#define LEVEL_ERROR		4 /* Errors, Hot Swap */
#define LEVEL_DMA		2 /* DMA Engine */
#define LEVEL_CONSOLE		3 /* Console - no hw */
#define LEVEL_PA_MANAGEMENT	3 /* Port Adaptor Hi IRQ */
#define LEVEL_PA_NETWORK	1 /* Network IO level */

#define WASTETIME(usecs) 	wastetime(usecs)
#define DELAY(ms)        	msecdelay(ms)

/*
 * Define hw board
 */
#define CHECKERS		TRUE

/* 
 * Max possible lines for RVIP
 */
#define PLATFORM_MAXLINES	6

/*
 * Highest numbered PA slot (zero-based)
 */
#define MAX_IF_SLOT		1

/*
 * Base of Memory
 */
#define RAMBASE       		0x64000000
#define PHY_RAMBASE		0x04000000

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


#define REFRESHTIME          	4	/* 4ms per tick */                 
#define NMI_FREQ_NUMERATOR 	250	/* Numerator of NMI frequency */
#define NMI_FREQ_DENOMINATOR 	1   	/* Denominator of NMI frequency */

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
#define R4K_SREG_SETUP          0x34410000
#define R4K_CONFIG_SETUP        0x00000033

/*
 * Write buffer flush routines
 */
#ifndef ASMINCLUDE

extern int flush_io_wbuffer(void);
extern int flush_shmem_wbuffer(void);
extern void flush_all_wbuffers(void);

/*
 * Support protected writes to the dbus registers.
 * These are stubs as the RVIP can never take a bus error
 * from a read to the dbus (bus interface is on the same card)
 */
typedef uchar dbus_protect_t;

#define dbus_protect(statep)	(*(statep) = 0)
#define dbus_restore(statep)

/*
 * RVIP does not cache packets in shared memory
 */
#define pak_set_dirty(x)

/*
 * RVIP never ignores the input holdqueue count
 */
#define pak_ignore_inputq(pak)	FALSE

/*
 * RVIP deals with scattered packets
 */
#define pak_has_particles(pak)	platform_pak_has_particles_inline(pak)

#endif /* ASMINCLUDE */

/*
 * Common interrupt levels
 *
 * Note : ALL_DISABLE does not mask the DMA interrupt or PA Management 
 * interrupt.
 */
#define PLATFORM_ALL_DISABLE   	1  /* disable all interrupts */
#define PLATFORM_TTY_DISABLE   	1  /* priority to shut off all types of TTY */
#define PLATFORM_NETS_DISABLE  	1  /* highest priority of a network device */
#define PLATFORM_ALL_ENABLE    	0  /* enable all interrupts */

#define	PLATFORM_FAMTYPE 	FAMILY_VIP

/*
 * Nonstandard buffer pool sizes
 */
#define RVIP_VERYBIG_MIN        3
#define RVIP_VERYBIG_MAX        10
#define RVIP_VERYBIG_INIT       3

#define RVIP_LARGE_MIN          3
#define RVIP_LARGE_MAX          10
#define RVIP_LARGE_INIT         3

#define RVIP_HUGE_MIN           1
#define RVIP_HUGE_MAX           5
#define RVIP_HUGE_INIT          1

/* end of file */
