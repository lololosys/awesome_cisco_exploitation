/* $Id: les_console_alc.h,v 3.2.62.1 1996/05/09 14:34:12 rbadri Exp $
 * $Source: /release/112/cvs/Xsys/les/les_console_alc.h,v $
 *------------------------------------------------------------------
 * les_console_alc.h -- Definitions for LES console and aux port
 *
 * Copyright (c) 1989-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: les_console_alc.h,v $
 * Revision 3.2.62.1  1996/05/09  14:34:12  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.2.92.1  1996/04/27  07:15:33  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.2.82.1  1996/04/08  01:59:02  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.2.68.1  1996/03/22  09:40:41  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.2.52.1  1996/03/03  21:24:02  rlowe
 * Apply SYNALC content to merge branch.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 3.2.50.1  1996/03/02  01:20:06  rlowe
 * Non-sync sync to mainline tag V111_1_0_3.
 * Branch: Synalc3_Cal_V111_1_0_3_branch
 *
 * Revision 3.2.34.1  1996/02/29  07:49:23  rlowe
 * First pass non-sync sync for Synergy ATM line card to V111_0_16.
 * Branch: Synalc3_Cal_V111_0_16_branch
 *
 * Revision 3.2  1995/11/17  17:41:38  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:33:34  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1.32.1  1996/02/03  07:07:39  rlowe
 * Non-sync sync of Synalc_catchup_branch to V111_0_5.
 * Branch: Synalc_catchup_pretal_branch
 *
 * Revision 2.1.2.1  1995/08/25  19:47:37  lcheung
 * move file from synergy_atmlc branch to synalc2 branch
 * Branch: Synalc2_branch
 *
 * Revision 2.1  1995/08/25  19:41:38  lcheung
 * add placeholder file fro Synalc2_branch
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "../../boot/src-68-alc/console.h"

#define WHICHUART(linenum) uartaddr

/*
 * These defines attempt to mimic PAN definitions for SYNALC.
 */
typedef struct uartdevice	uartaddress ;

#define	dataA			a.data

#define	RXDISABLE		(STP_RXENABLE & ~(MFP_RSR_RX_ENABLE))
#define	TXDISABLE		(STP_TXENABLE & ~(MFP_TSR_TX_ENABLE))

#define	RCVBREAK		(MFP_RSR_BREAK_DETECT)
#define	OVRRNERROR		(MFP_RSR_OVERRUN_ERR)


/*
 * Prototypes
 */

extern void pan_console_interrupt(void);
extern void pan_tx_disable(tt_soc *);
