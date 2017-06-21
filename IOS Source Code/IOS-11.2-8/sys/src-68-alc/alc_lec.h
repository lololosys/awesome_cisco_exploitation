/* $Id: alc_lec.h,v 3.2.62.3 1996/07/02 23:44:27 sakumar Exp $
 * $Source: /release/112/cvs/Xsys/src-68-alc/alc_lec.h,v $
 *------------------------------------------------------------------
 * Synergy specific LANE Client includes
 *
 * May 1995, Brad Benson
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: alc_lec.h,v $
 * Revision 3.2.62.3  1996/07/02  23:44:27  sakumar
 * CSCdi60668:  PVC (RFC1483) support needs to be added to Cat5K ATM Module
 * Branch: California_branch
 *
 * Incorporated CLI change requests for PVC config and throttle config.
 *
 * Revision 3.2.62.2  1996/06/18  16:43:42  sakumar
 * CSCdi60668:  PVC (RFC1483) support needs to be added to Cat5K ATM Module
 * Branch: California_branch
 *
 * Revision 3.2.62.1  1996/05/09  14:41:00  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.2.92.1  1996/04/27  07:25:14  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.2.82.2  1996/04/10  06:40:45  rlowe
 * Fix up ifdefs; clean-out old, obsolete code.
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 3.2.82.1  1996/04/08  02:08:48  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.2.68.1  1996/03/22  09:43:33  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.2.52.2  1996/03/08  01:14:04  rbadri
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 * Fix Synergy/Router conflicts for LANE.
 *
 * Revision 3.2.52.1  1996/03/03  21:24:47  rlowe
 * Apply SYNALC content to merge branch.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 3.2.50.1  1996/03/02  01:21:00  rlowe
 * Non-sync sync to mainline tag V111_1_0_3.
 * Branch: Synalc3_Cal_V111_1_0_3_branch
 *
 * Revision 3.2.34.1  1996/02/29  07:50:10  rlowe
 * First pass non-sync sync for Synergy ATM line card to V111_0_16.
 * Branch: Synalc3_Cal_V111_0_16_branch
 *
 * Revision 3.2  1995/11/17  18:42:53  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:24:46  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1.66.1  1996/02/03  07:08:27  rlowe
 * Non-sync sync of Synalc_catchup_branch to V111_0_5.
 * Branch: Synalc_catchup_pretal_branch
 *
 * Revision 2.1.48.2  1995/09/28  00:47:11  bbenson
 * Branch: Synalc2_LE2_V110_1_0_3_branch
 * Remove lec_ipc_inputQ_enabled.
 *
 * Revision 2.1.48.1  1995/09/11  01:57:04  rlowe
 * Non-sync sync of Synalc2_branch to LE2_V110_1_0_3_branch,
 * yielding Synalc2_LE2_V110_1_0_3_branch.
 *
 * Branch: Synalc2_LE2_V110_1_0_3_branch
 *
 * Revision 2.1.32.2  1995/08/31  14:05:12  rlowe
 * Carryover useful code from old Synergy_ATMlc_branch (10.3) to new
 * Synalc2_branch (11.0).
 *
 * Branch: Synalc2_branch
 *
 * Revision 2.1.32.1  1995/08/22  17:51:47  lcheung
 * move file from Synergy_atmlc_branch to Synalc2_branch
 * Branch: Synalc2_branch
 *
 * Revision 2.1  1995/06/07  22:54:20  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#ifndef _ALC_LEC_H_
#define	_ALC_LEC_H_

#ifndef LANGUAGE_C
#define LANGUAGE_C
#endif


/*
 * Extern function declarations.
 */
extern void lec_alc_init(void);
extern void lec_process_sar_messages(void);
extern void lec_process_ipc_messages(void);
extern void lec_sar_enqueue(void *msg);

extern void cat5k_interface_commands(parseinfo *csb);
extern void cat5k_show_commands(parseinfo *csb);
extern void lec_cfg_lec(parseinfo *);

#define ATM_CMD_BIND_PVC_TO_VLAN		1
#define ATM_CMD_REMOVE_VLAN			2
#define SHOW_ATM_VLAN				3
#define ATM_CMD_THROTTLE_TSAR			4

#endif /* _ALC_LEC_H_ */
