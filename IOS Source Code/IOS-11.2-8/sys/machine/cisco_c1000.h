/* $Id: cisco_c1000.h,v 3.6.12.3 1996/06/12 19:56:15 jng Exp $
 * $Source: /release/112/cvs/Xsys/machine/cisco_c1000.h,v $
 *------------------------------------------------------------------
 * cisco_c1000.h - Definitions for the 1XXX
 *
 * December 1994, Greg Stovall
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cisco_c1000.h,v $
 * Revision 3.6.12.3  1996/06/12  19:56:15  jng
 * CSCdi60260:  Async catwoman commit
 * Branch: California_branch
 * Async Catwoman commit into California
 *
 * Revision 3.6.12.2  1996/06/04  22:55:06  mbeesley
 * CSCdi59380:  Optimize buffer code for traditional platforms
 * Branch: California_branch
 *
 * Revision 3.6.12.1  1996/05/09  14:34:20  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.6.38.1  1996/04/27  07:16:01  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.6.28.1  1996/04/08  01:59:13  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.6.18.1  1996/03/22  09:40:50  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.6  1996/02/13  20:38:50  hampton
 * Move buffer initialization to the platform_xxx.c files to eliminate
 * platform dependencies from common code.  [CSCdi48844]
 *
 * Revision 3.5.8.1  1996/03/03  21:24:11  rlowe
 * Apply SYNALC content to merge branch.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 3.5.6.1  1996/03/02  01:20:17  rlowe
 * Non-sync sync to mainline tag V111_1_0_3.
 * Branch: Synalc3_Cal_V111_1_0_3_branch
 *
 * Revision 3.5  1996/02/02  04:29:14  mbeesley
 * CSCdi48003:  Buffer manager not optimal on all boxes
 * Make pak_set_dirty() and pak_ignore_inputq() platform specific.
 *
 * Revision 3.4.18.1  1996/02/29  07:49:32  rlowe
 * First pass non-sync sync for Synergy ATM line card to V111_0_16.
 * Branch: Synalc3_Cal_V111_0_16_branch
 *
 * Revision 3.4  1995/12/23  13:10:05  hampton
 * Fix a timing window where interfaces can be mistakenly throttled.
 * [CSCdi44951]
 *
 * Revision 3.3  1995/12/21  16:59:40  hampton
 * Move a couple of externs between files.  [CSCdi45965]
 *
 * Revision 3.2  1995/11/17  17:49:57  hampton
 * Remove old entries from the RCS header logs.
 * Revision 2.1.60.1  1996/02/03  07:07:49  rlowe
 * Non-sync sync of Synalc_catchup_branch to V111_0_5.
 * Branch: Synalc_catchup_pretal_branch
 *
 * Revision 2.1.44.1  1995/09/20  03:29:39  rlowe
 * Allow, but do not require, system timer interrupt vector to be
 * different from NMI vector.  For 68k-related platforms.
 *
 * Branch: Synalc2_LE2_V110_1_0_3_branch
 *
 * Revision 2.1  1995/06/07  21:43:04  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 * Revision 3.1  1995/11/09  12:40:06  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/10/15  18:53:56  eschaffe
 * CSCdi40294:  Increase the number of Protocol Translation sessions
 *
 * Revision 2.1  1995/06/07  21:43:04  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/*
 * The C1XXX use a.out images
 */
#include "image_aout.h"

#include "../src-36-c1000/pcmap.h" /* Needed for ADRSPC_RAM definition */


/*
 * Everything is at level 4, the QUICC has internal priorities
 */
#define LEVEL_CPIC (4)		/* All CPIC-handled interrupts */
#define LEVEL_CONSOLE (LEVEL_CPIC) /* console UART */
#define ETHER_DISABLE (LEVEL_CPIC)  /* priority level to shut off Ethernet */
#define ETHER_INTLEVEL (LEVEL_CPIC)	/* another name */
#define SERIAL_DELTA_INTLEVEL (LEVEL_CPIC)
				 /* Serial interface state changed */

#define CPIC_INTERRUPT_BASE (96) 	/* CPIC interrupts start at 96 */

#define WASTETIME(usecs) quicc_usec_delay(usecs)
#define DELAY(ms)        quicc_millisecond_delay(ms)

#define	TIMER_VECTOR	NMIVector

#define REFRESHTIME             4        /* time in milliseconds for refresh */
#define REFRFREQUENCY    (REFRESHTIME*1000)
#define PIT_XTAL_FREQ	     4000000	 /* PIT Xtal frequency */
#define PIT_XTAL_DIVIDER     512	 /* PIT Xtal divider */
#define PIT_COMMON_FACTOR    256	 /* GCF in divider and frequency */
#define PIT_COUNT ((REFRESHTIME * PIT_XTAL_FREQ) / (PIT_XTAL_DIVIDER * 1000))
#define NMI_FREQ_NUMERATOR   (PIT_XTAL_FREQ/PIT_COMMON_FACTOR)
					 /* Numerator of NMI frequency */
#define NMI_FREQ_DENOMINATOR ((PIT_XTAL_DIVIDER*PIT_COUNT)/PIT_COMMON_FACTOR)
                                         /* Denominator of NMI frequency */
#define NTP_PRECISION 7			 /* A 252 Hz clock */


/*
 * Base of memory
 */
#define RAMBASE	ADRSPC_RAM	/* Base of RAM */

/*
 * Define the address of the pointer to the crash info data structure
 * This pointer (and data structure) are used by GDB as a starting point
 * for analyzing a crash dump.  This location does not need to survive a
 * reload, it just needs to appear in the crash dump.  If you change
 * this value, you must coordinate with Cisco's GDB maintainers.
 */
#define CRASH_INFO (*((crashinfo **)(RAMBASE + 0xFFC)))

/*
 * Main NVRAM define
 */
#define NVBCOPY(s, d, len)	bcopy(s, d, len)
#define NVBASE                  ADRSPC_NVRAM
/* for cancun and sapphire NV EEPROM support -- */
#define NVADDRESS(base, offset) (base + offset)


/*
 * Max possible lines for c1000 platform.
 */
#define PLATFORM_MAXLINES 7

/*
 * Common interrupt levels
 */

#define PLATFORM_ALL_DISABLE  LEVEL_CPIC  /* disable all interrupts */
#define PLATFORM_TTY_DISABLE  LEVEL_CPIC  /* priority to shut off all types of TTY */
#define PLATFORM_NETS_DISABLE LEVEL_CPIC  /* highest priority of a network device */
#define PLATFORM_ALL_ENABLE   0	    /* enable all interrupts */

#define	PLATFORM_FAMTYPE FAMILY_C1000

/*
 * Nonstandard buffer pool sizes
 */
#define C1000_VERYBIG_INIT2 0

/*
 * Prototypes
 */
#ifndef ASMINCLUDE

void quicc_usec_delay(unsigned);
void quicc_millisecond_delay(uint);

#define usecdelay quicc_usec_delay
#define msecdelay quicc_millisecond_delay

/*
 * C1000 does not cache packets in shared memory
 */
#define pak_set_dirty(x)

/*
 * C1000 never ignores the input holdqueue count
 */
#define pak_ignore_inputq(pak)	FALSE

/*
 * C1000 never deals with scattered packets
 */
#define pak_has_particles(pak)	FALSE

#endif
