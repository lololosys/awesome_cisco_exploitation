/* $Id: cisco_c4000.h,v 3.6.12.2 1996/06/04 22:55:09 mbeesley Exp $
 * $Source: /release/112/cvs/Xsys/machine/cisco_c4000.h,v $
 *------------------------------------------------------------------
 * cisco_c4000.h - Definitions for the 68EC030 based C4000
 *
 * November 1993, Lele Nardin
 * December 1994, Scott Mackie (moved and altered from sys/h)
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cisco_c4000.h,v $
 * Revision 3.6.12.2  1996/06/04  22:55:09  mbeesley
 * CSCdi59380:  Optimize buffer code for traditional platforms
 * Branch: California_branch
 *
 * Revision 3.6.12.1  1996/05/09  14:34:23  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.6.38.1  1996/04/27  07:16:03  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.6.28.1  1996/04/08  01:59:15  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.6.18.1  1996/03/22  09:40:53  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.6  1996/02/13  20:38:53  hampton
 * Move buffer initialization to the platform_xxx.c files to eliminate
 * platform dependencies from common code.  [CSCdi48844]
 *
 * Revision 3.5.8.1  1996/03/03  21:24:14  rlowe
 * Apply SYNALC content to merge branch.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 3.5.6.1  1996/03/02  01:20:20  rlowe
 * Non-sync sync to mainline tag V111_1_0_3.
 * Branch: Synalc3_Cal_V111_1_0_3_branch
 *
 * Revision 3.5  1996/02/02  04:29:17  mbeesley
 * CSCdi48003:  Buffer manager not optimal on all boxes
 * Make pak_set_dirty() and pak_ignore_inputq() platform specific.
 *
 * Revision 3.4.18.1  1996/02/29  07:49:35  rlowe
 * First pass non-sync sync for Synergy ATM line card to V111_0_16.
 * Branch: Synalc3_Cal_V111_0_16_branch
 *
 * Revision 3.4  1995/12/23  13:10:08  hampton
 * Fix a timing window where interfaces can be mistakenly throttled.
 * [CSCdi44951]
 *
 * Revision 3.3  1995/12/21  16:29:08  hampton
 * Switch the 68K version of DELAY() to call the existing usecdelay
 * routine which uses the timer chip.  For all other platforms, move a
 * routine or two between files.  [CSCdi45965]
 *
 * Revision 3.2  1995/11/17  17:50:00  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:40:09  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1.60.1  1996/02/03  07:07:51  rlowe
 * Non-sync sync of Synalc_catchup_branch to V111_0_5.
 * Branch: Synalc_catchup_pretal_branch
 *
 * Revision 2.1.44.1  1995/09/20  03:29:41  rlowe
 * Allow, but do not require, system timer interrupt vector to be
 * different from NMI vector.  For 68k-related platforms.
 *
 * Branch: Synalc2_LE2_V110_1_0_3_branch
 *
 * Revision 2.1  1995/06/07  21:43:10  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */



/*
 * The C4000 uses a.out images
 */
#include "image_aout.h"


/*
 * Interrupt level assignments for system software
 *
 *	3 - Network Traffic
 *	4 - High Priority Network Interrupts
 *	5 - Console UART
 */

#define LEVEL_CONSOLE 5		/* console UART */
#define ETHER_DISABLE 3	        /* priority level to shut off Ethernet */
#define ETHER_INTLEVEL 3	/* another name */
#define SERIAL_DELTA_INTLEVEL 3 /* High speed serial interface state changed */

#define	TIMER_VECTOR	NMIVector

#define REFRESHTIME          4          /* 4ms per tick */                 
#define NMI_FREQ_NUMERATOR   250        /* Numerator of NMI frequency */
#define NMI_FREQ_DENOMINATOR 1	        /* Denominator of NMI frequency */

#define WASTETIME(usecs) wastetime(usecs)
#define DELAY(ms)        msecdelay(ms)

/*
 * Base of memory
 */

#define RAMBASE	0x00000000	/* Base of RAM */
#define TRAP_PAGE_ZERO		/* Catch writes to addresses 0x0 - 0xFF */

/*
 * Define the address of the pointer to the crash info data structure
 * This pointer (and data structure) are used by GDB as a starting point
 * for analyzing a crash dump.  This location does not need to survive a
 * reload, it just needs to appear in the crash dump.  If you change
 * this value, you must coordinate with Cisco's GDB maintainers.
 */
#define CRASH_INFO (*((crashinfo **)(RAMBASE + 0xFFC)))

/*
 * Address of 8254 registers
 */

#define CTDEVBASEADDR	0x2120000

/*
 * Address of UART registers
 */
#define UARTDEVBASEADDR	0x02120100

/*
 * Serial NVRAM stuff
 */
#define X2444_PORT *(uchar *)0x02110060        /* Data register for X2444 */

#define X2444_CE 0x08           /* CE: Chip enable, also seen as CS */
#define X2444_SK 0x04           /* SK: Serial clock in */
#define X2444_DI 0x02           /* DI: Serial data in  */
#define X2444_DO 0x01           /* DO: Serial data out */

/*
 * Main NVRAM define
 */
#define NVBCOPY(s, d, len)	bcopy(s, d, len)

/*
 * cancun NV EEPROM support
 */
#define NVADDRESS(base, offset) (base + offset)

#define NVBASE			ADRSPC_NVRAM_START


/*
 * C4000 does not cache packets in shared memory
 */
#define pak_set_dirty(x)

/*
 * C4000 never ignores the input holdqueue count
 */
#define pak_ignore_inputq(pak)	FALSE

/*
 * C4000 never deals with scattered packets
 */
#define pak_has_particles(pak)	FALSE

/*
 * Max possible lines for c4000 platform.
 */
#define PLATFORM_MAXLINES 200

/*
 * Common interrupt levels
 */

#define PLATFORM_ALL_DISABLE  6	    /* disable all interrupts */
#define PLATFORM_TTY_DISABLE  5	    /* priority to shut off all types of TTY */
#define PLATFORM_NETS_DISABLE 4	    /* highest priority of a network device */
#define PLATFORM_ALL_ENABLE   0	    /* enable all interrupts */

#define	PLATFORM_FAMTYPE FAMILY_XX

/*
 * Nonstandard buffer pool sizes
 */
#define C4000_BIG_MAX1 60
