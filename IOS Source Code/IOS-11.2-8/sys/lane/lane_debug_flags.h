/* $Id: lane_debug_flags.h,v 3.2.58.1 1996/05/09 14:32:24 rbadri Exp $
 * $Source: /release/112/cvs/Xsys/lane/lane_debug_flags.h,v $
 *------------------------------------------------------------------
 * lane_debug.h: LANE DEBUG command specific vars. & constants
 *
 * October, 1994, Chris Parrott
 *
 * Copyright (c) 1994-1996, 1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: lane_debug_flags.h,v $
 * Revision 3.2.58.1  1996/05/09  14:32:24  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.2.86.1  1996/04/27  07:06:18  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.2.76.2  1996/04/15  09:06:40  shj
 * CSCdi52532:  LANE invalidates protocol fast caches too often
 * Branch: LE_Cal_V112_0_2_branch
 * Don't do bump_cache_version for dying LE-ARP's unless data-direct path
 * was actually in use.  Also add some debugging to see dying LE-ARP's.
 *
 * Revision 3.2.76.1  1996/04/08  01:56:45  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.2.64.1  1996/03/22  22:54:09  rlowe
 * Non-sync of Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.2.48.2  1996/03/05  06:42:47  rlowe
 * Apply LANE content to merge branch.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 3.2.48.1  1996/03/03  21:23:25  rlowe
 * Apply SYNALC content to merge branch.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 3.2.36.1  1996/02/27  21:10:08  cakyol
 * non sync sync of LE_Cal_V111_0_16_branch to V111_1_0_3 yielding
 * LE_Cal_V111_1_0_3_branch
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 3.2.14.1  1996/01/11  01:46:12  cakyol
 * non sync sync of LE_Calif_branch to V111_0_16 yielding
 *     LE_Cal_V111_0_16_branch
 *
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 3.2.10.2  1996/01/03  22:52:41  cakyol
 * - divided up debug categories to: all, events and packets
 *   for the LECS.  There were too many debugging messages
 *   for a single debug flag.
 *
 * - updated some of the outdated comments.
 *
 * Branch: LE_Calif_branch
 *
 * Revision 3.2.10.1  1995/12/29  01:06:33  cakyol
 * Initial commits for:
 *
 * LANE Fault Tolerant LECS Servers
 * LANE Fault Tolerant LES Servers
 * HSRP over LANE Client
 *
 * Branch: LE_Calif_branch
 *
 * Revision 3.2  1995/11/17  17:50:22  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:22:53  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2.48.1  1996/02/03  07:07:02  rlowe
 * Non-sync sync of Synalc_catchup_branch to V111_0_5.
 * Branch: Synalc_catchup_pretal_branch
 *
 * Revision 2.2.46.1  1996/01/19  21:56:45  sgarcia
 * CSCdi47203:  sync 11.1 catchup branch to 11.0 ATMFLASH_V11_951214 tag
 * of Synalc2_
 * Branch: Synalc_catchup_branch
 * Patch 11.0 Synalc2_LE2_V110_1_0_3_branch diffs between tags
 * Synalc2_LE2_V110_1_0_3_arizona_arkansas_split_951103 and
 * ATMFLASH_V11_951214
 * into 11.1 branch.
 *
 * Revision 2.2.28.1  1995/12/01  16:01:14  bbenson
 * CSCdi44768:  TOPO change debug message should output on toggle
 * Branch: Synalc2_LE2_V110_1_0_3_branch
 * Added new debug switch to the client for topo debugging.
 *
 * Revision 2.2  1995/07/12  01:47:48  nfinn
 * CSCdi36959:  LECS access attempts to connect to ATM address 0
 *
 * Revision 2.1  1995/06/07  21:19:16  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "../ui/debug_macros.h"

/* Now define the actual flags and the array that points to them */
DEBUG_ARRDECL(lane_debug_arr)

/* LEC */
DEBUG_FLAG(lec_packet_debug, DEBUG_LANE_LEC_PACKET, "LANE Client packet")
DEBUG_FLAG(lec_state_debug, DEBUG_LANE_LEC_STATE, "LANE Client state")
DEBUG_FLAG(lec_topo_debug, DEBUG_LANE_LEC_TOPO, "LANE Client topology")
DEBUG_FLAG(lec_le_arp_debug, DEBUG_LANE_LEC_LE_ARP, "LANE Client le-arp")
DEBUG_FLAG(lec_signal_debug, DEBUG_LANE_LEC_SIGNAL, "LANE Client signaling")
DEBUG_FLAG(lec_all_debug, DEBUG_LANE_LEC_ALL, "All LANE Client debugging")

/* LECS */
DEBUG_FLAG(lane_lecs_debug, DEBUG_LANE_LECS, "LANE Config Server Events")
DEBUG_FLAG(lane_lecsPacket_debug, DEBUG_LANE_LECS_PACKETS, "LANE Config Server Packets")
DEBUG_FLAG(lecs_all_debug, DEBUG_LANE_LECS_ALL, "All LANE Config Server debugging")

/* LECS Finder */
DEBUG_FLAG(lane_lecsFinder_debug, DEBUG_LANE_LECSF, "LANE LECS Finder")

/* LES */
DEBUG_FLAG(lane_lsv_debug, DEBUG_LANE_LSV, "LANE Server")

/* other */
DEBUG_FLAG(lane_nsap_debug, DEBUG_LANE_NSAP, "ATM nsap parsing")
DEBUG_FLAG(lane_signal_debug, DEBUG_LANE_SIGNAL, "LANE ATM signaling")

DEBUG_ARRDONE
