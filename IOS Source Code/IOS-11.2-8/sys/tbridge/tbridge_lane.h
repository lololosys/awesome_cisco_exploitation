/* $Id: tbridge_lane.h,v 3.3.20.2 1996/05/09 14:47:17 rbadri Exp $
 * $Source: /release/112/cvs/Xsys/tbridge/tbridge_lane.h,v $
 *------------------------------------------------------------------
 * tbridge/tbridge_lane.h
 *
 * Aug 1995, Badri Ramaswamy
 *
 * Functions to provide Lan Emulation Client -> Bridge table interactions
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: tbridge_lane.h,v $
 * Revision 3.3.20.2  1996/05/09  14:47:17  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.3.20.1.2.1  1996/04/27  07:26:52  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.3.20.1  1996/04/25  23:22:48  ronnie
 * CSCdi55766:  Hey!  This ATM and Frame Relay is no different than in 11.1
 * Feature commit for ATM and Frame Relay into 11.2
 * Branch: California_branch
 *
 * Revision 3.3.38.1  1996/04/08  02:10:08  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.3.26.1  1996/03/22  09:44:19  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.3.10.1  1996/03/08  01:12:52  rbadri
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 * Fix Synergy/Router conflicts.
 *
 * Revision 3.3  1996/02/05  18:05:47  rbadri
 * CSCdi47734:  LEC responds to LE_ARP requests even in blocked state.
 * Add another registry call for LE_ARP responses.
 *
 * Revision 3.2  1995/11/17  18:45:22  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:33:35  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:31:25  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/08/16  16:51:46  rbadri
 * Placeholder for bridging over Emulated LANs.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Registry functions to update and query the bridge table entry, called from
 * Lane Client code. 
 */
extern 
void tbridge_lane_bte_update(void *lec_info,  uchar *mac_addr,
			     vcd_t  vcd, ushort lec_state);
extern
boolean tbridge_lane_bte_exists(void *lec_info, uchar *mac);

extern
boolean tbridge_lane_bte_ready(void *lec_info, uchar *mac);

extern
boolean tbridge_lane_bte_flooding(void *lec_info, uchar *mac);

