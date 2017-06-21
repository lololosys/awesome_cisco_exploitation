/* $Id: cisco_alc.h,v 3.2.62.2 1996/06/04 22:55:04 mbeesley Exp $
 * $Source: /release/112/cvs/Xsys/machine/cisco_alc.h,v $
 *------------------------------------------------------------------
 * cisco_alc.h -- Definitions for the Synergy ATM line card.
 * Based on cisco_c3000.h - Definitions for the 25XX, 3XXX and IGS
 *
 * April 13, 1995, Rick Lowe
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cisco_alc.h,v $
 * Revision 3.2.62.2  1996/06/04  22:55:04  mbeesley
 * CSCdi59380:  Optimize buffer code for traditional platforms
 * Branch: California_branch
 *
 * Revision 3.2.62.1  1996/05/09  14:34:19  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.2.92.1  1996/04/27  07:16:00  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.2.82.1  1996/04/08  01:59:12  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.2.68.2  1996/03/22  23:27:23  rlowe
 * Give relative path to wbu include files.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.2.68.1  1996/03/22  09:40:49  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.2.52.1  1996/03/03  21:24:10  rlowe
 * Apply SYNALC content to merge branch.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 3.2.50.2  1996/03/02  07:36:56  rlowe
 * Platform-specific hooks needed for sync to V111_1_0_3.
 * Branch: Synalc3_Cal_V111_1_0_3_branch
 *
 * Revision 3.2.50.1  1996/03/02  01:20:15  rlowe
 * Non-sync sync to mainline tag V111_1_0_3.
 * Branch: Synalc3_Cal_V111_1_0_3_branch
 *
 * Revision 3.2.34.1  1996/02/29  07:49:31  rlowe
 * First pass non-sync sync for Synergy ATM line card to V111_0_16.
 * Branch: Synalc3_Cal_V111_0_16_branch
 *
 * Revision 3.2  1995/11/17  17:49:55  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:40:04  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1.62.1  1996/02/03  07:07:47  rlowe
 * Non-sync sync of Synalc_catchup_branch to V111_0_5.
 * Branch: Synalc_catchup_pretal_branch
 *
 * Revision 2.1.44.2  1995/09/20  03:29:38  rlowe
 * Allow, but do not require, system timer interrupt vector to be
 * different from NMI vector.  For 68k-related platforms.
 *
 * Branch: Synalc2_LE2_V110_1_0_3_branch
 *
 * Revision 2.1.44.1  1995/09/11  01:56:42  rlowe
 * Non-sync sync of Synalc2_branch to LE2_V110_1_0_3_branch,
 * yielding Synalc2_LE2_V110_1_0_3_branch.
 *
 * Branch: Synalc2_LE2_V110_1_0_3_branch
 *
 * Revision 2.1.28.2  1995/09/08  03:22:54  rlowe
 * SynALC prom resides at addresses $0 through $80000.  Undef
 * TRAP_PAGE_ZERO.
 * Branch: Synalc2_branch
 *
 * Revision 2.1.28.1  1995/08/31  13:00:41  rlowe
 * Carryover applicable code from old Synergy_ATMlc_branch (10.3)
 * to Synalc2_branch (11.0).  Add SYNALC-specific entries besides
 * other platform-specific entries.
 *
 * Branch: Synalc2_branch
 *
 * Revision 2.1  1995/06/07  21:43:02  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */



/*
 * The SYNALC uses a.out images
 */
#include "image_aout.h"

#include "../../boot/src-68-alc/pcmap.h"
#include "../../boot/src-68-alc/defs_alc.h"
#include "../../wbu/cat5k/alc_include/mfp_acp_defs.h"


/*
 * Interrupt level assignments for system software
 *
 * SYNALC uses 68901 MPF @ level 2 for all interrupts except watchdog NMI.
 * This includes the system timer (4 ms) tick, which is NOT NMI on SYNALC.
 * Instead of assuming autovectored interrupts, supply the interrupt vector.
 *
 */

#define LEVEL2_DISABLE		(LEVEL2)	/* Mask all MFP interrupts. */

#define	SAR_INTLEVEL	(LEVEL2_DISABLE)	/* For either Tx/Rx SARs. */
#define	NTC_INTLEVEL	(LEVEL2_DISABLE)	/* For NTC interface.     */
#define	LCP_INTLEVEL	(LEVEL2_DISABLE)	/* For LCP rx/tx-done.    */
#define	BRG_INTLEVEL	(LEVEL2_DISABLE)	/* For UART BRG (Tx) intr.*/

#define LEVEL_CONSOLE		(LEVEL2_DISABLE)
				/* console UART */
#define ETHER_DISABLE		(LEVEL2_DISABLE)
				/* priority level to shut off Ethernet */
#define ETHER_INTLEVEL		(LEVEL2_DISABLE)
				/* another name */

#define REFRESHTIME		4	/* 4ms per tick */                 
#define NMI_FREQ_NUMERATOR	250	/* Numerator of NMI frequency */
#define NMI_FREQ_DENOMINATOR	1	/* Denominator of NMI frequency */

#define WASTETIME(usecs)	wastetime(usecs)
#define DELAY(ms)		msecdelay(ms)

/*
 * Define interrupt vectors of interest.
 */
#define	MIN_USR_VECTOR		(MFP_BASE_VECTOR)
#define	TXSAR_IVECTOR		(MFP_BASE_VECTOR + MFP_IINDX_TSAR)
#define	RXSAR_IVECTOR		(MFP_BASE_VECTOR + MFP_IINDX_RSAR)
#define	LCPTX_IVECTOR		(MFP_BASE_VECTOR + MFP_IINDX_LCPTP)
#define	LCPRX_IVECTOR		(MFP_BASE_VECTOR + MFP_IINDX_LCPRD)
#define	UARTBRG_IVECTOR		(MFP_BASE_VECTOR + MFP_IINDX_BRG)
#define	NTC_IVECTOR		(MFP_BASE_VECTOR + MFP_IINDX_NTC)
#define	CONSOLE_IVECTOR		(MFP_BASE_VECTOR + MFP_IINDX_TXER)
#define	SYSTIMR_IVECTOR		(MFP_BASE_VECTOR + MFP_IINDX_TIC_OS)
#define	MAX_VECS_IN_TBL		(MFP_BASE_VECTOR + MFP_TOT_VECS_USED)

#define	NBR_CONSOLE_IVECS	(MFP_NBR_UART_IVECS)

#define	TIMER_VECTOR		(SYSTIMR_IVECTOR)

/*
 * Base of memory
 */

#define	RAMBASE			0x40000000L	/* Base of RAM */
#undef	TRAP_PAGE_ZERO		/* SynALC addresses 0x00 - 0xFF are in prom. */

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

/*
 * cancun NV EEPROM support
 */
#define NVADDRESS(base, offset) (base + offset)

#define NVBASE			ADRSPC_NVRAM_START



/*
 * SYNALC does not cache packets in shared memory
 */
#define pak_set_dirty(x)

/*
 * SYNALC never ignores the input holdqueue count
 */
#define pak_ignore_inputq(pak)	FALSE

/*
 * SYNALC never deals with scattered packets
 */
#define pak_has_particles(pak)	FALSE

/*
 * Max possible lines for c5kalc platform.
 */
#define PLATFORM_MAXLINES	16


/*
 * Common interrupt levels
 */

#define PLATFORM_ALL_DISABLE	(LEVEL2_DISABLE)
				/* disable all interrupts */
#define PLATFORM_TTY_DISABLE	(LEVEL2_DISABLE)
				/* priority to shut off all types of TTY */
#define PLATFORM_NETS_DISABLE	(LEVEL2_DISABLE)
				/* highest priority of a network device */
#define PLATFORM_ALL_ENABLE	(0)
				/* enable all interrupts */

#define	PLATFORM_FAMTYPE	(FAMILY_SYNALC)

