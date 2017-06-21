/* $Id: exec_debug_lane.h,v 3.2.58.1 1996/05/09 14:32:05 rbadri Exp $
 * $Source: /release/112/cvs/Xsys/lane/exec_debug_lane.h,v $
 *------------------------------------------------------------------
 * parser/exec_debug_lane.h: DEBUG command parsing code
 *
 * October, 1994, Chris Parrott
 *
 * Copyright (c) 1994-1996, 1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: exec_debug_lane.h,v $
 * Revision 3.2.58.1  1996/05/09  14:32:05  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.2.86.1  1996/04/27  07:06:00  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.2.76.2  1996/04/15  09:06:37  shj
 * CSCdi52532:  LANE invalidates protocol fast caches too often
 * Branch: LE_Cal_V112_0_2_branch
 * Don't do bump_cache_version for dying LE-ARP's unless data-direct path
 * was actually in use.  Also add some debugging to see dying LE-ARP's.
 *
 * Revision 3.2.76.1  1996/04/08  01:56:27  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.2.64.1  1996/03/22  22:53:41  rlowe
 * Non-sync of Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.2.48.2  1996/03/05  06:42:28  rlowe
 * Apply LANE content to merge branch.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 3.2.48.1  1996/03/03  21:23:21  rlowe
 * Apply SYNALC content to merge branch.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 3.2.36.1  1996/02/27  21:09:49  cakyol
 * non sync sync of LE_Cal_V111_0_16_branch to V111_1_0_3 yielding
 * LE_Cal_V111_1_0_3_branch
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 3.2.14.2  1996/02/22  09:05:23  cakyol
 * the new LECS config command syntax enabled provisionally
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 3.2.14.1  1996/01/11  01:45:57  cakyol
 * non sync sync of LE_Calif_branch to V111_0_16 yielding
 *     LE_Cal_V111_0_16_branch
 *
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 3.2.10.2  1996/01/03  22:52:36  cakyol
 * - divided up debug categories to: all, events and packets
 *   for the LECS.  There were too many debugging messages
 *   for a single debug flag.
 *
 * - updated some of the outdated comments.
 *
 * Branch: LE_Calif_branch
 *
 * Revision 3.2.10.1  1995/12/29  01:06:20  cakyol
 * Initial commits for:
 *
 * LANE Fault Tolerant LECS Servers
 * LANE Fault Tolerant LES Servers
 * HSRP over LANE Client
 *
 * Branch: LE_Calif_branch
 *
 * Revision 3.2  1995/11/17  17:49:54  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:22:31  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2.56.1  1996/02/03  07:06:59  rlowe
 * Non-sync sync of Synalc_catchup_branch to V111_0_5.
 * Branch: Synalc_catchup_pretal_branch
 *
 * Revision 2.2.54.1  1996/01/19  21:56:43  sgarcia
 * CSCdi47203:  sync 11.1 catchup branch to 11.0 ATMFLASH_V11_951214 tag
 * of Synalc2_
 * Branch: Synalc_catchup_branch
 * Patch 11.0 Synalc2_LE2_V110_1_0_3_branch diffs between tags
 * Synalc2_LE2_V110_1_0_3_arizona_arkansas_split_951103 and
 * ATMFLASH_V11_951214
 * into 11.1 branch.
 *
 * Revision 2.2.36.1  1995/12/01  16:01:12  bbenson
 * CSCdi44768:  TOPO change debug message should output on toggle
 * Branch: Synalc2_LE2_V110_1_0_3_branch
 * Added new debug switch to the client for topo debugging.
 *
 * Revision 2.2  1995/06/19  14:46:35  widmer
 * CSCdi36074:  Clean up debug parse chain alternates
 * Add keyword_debug macro to assign debug array
 *
 * Revision 2.1  1995/06/07  21:18:35  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * debug lane signaling
 */
EOLS	(debug_lane_signaling_eol, debug_command, DEBUG_LANE_SIGNAL);
KEYWORD	(debug_lane_signaling, debug_lane_signaling_eol, no_alt,
	"signaling", "LAN Emulation ATM signaling", PRIV_OPR);

/******************************************************************
 * debug lane nsap-parsing
 */
EOLS	(debug_lane_nsap_eol, debug_command, DEBUG_LANE_NSAP);
KEYWORD	(debug_lane_nsap, debug_lane_nsap_eol, debug_lane_signaling,
	"nsap-parsing", "masked nsap parsing", PRIV_HIDDEN);

/******************************************************************
 * debug lane server
 */
EOLS	(debug_lane_les_eol, debug_command, DEBUG_LANE_LSV);
KEYWORD	(debug_lane_les, debug_lane_les_eol, debug_lane_nsap,
	"server", "LAN Emulation Server", PRIV_OPR);

/******************************************************************
 * debug lane lecsfinder
 */
EOLS	(debug_lane_lecsf_eol, debug_command, DEBUG_LANE_LECSF);
KEYWORD (debug_lane_lecsf, debug_lane_lecsf_eol, debug_lane_les,
	 "finder", "LAN Emulation LECS Finder", PRIV_OPR);

/******************************************************************
 * debug lane config (LECS)
 */

/* packets */
EOLS    (debug_lane_lecs_p_eol, debug_command, DEBUG_LANE_LECS_PACKETS);
KEYWORD (debug_lane_lecs_packets, debug_lane_lecs_p_eol, no_alt,
	 "packets", "LAN Emulation Config Server (LECS) Packets", PRIV_OPR);

/* events */
EOLS    (debug_lane_lecs_ev_eol, debug_command, DEBUG_LANE_LECS);
KEYWORD (debug_lane_lecs_events, debug_lane_lecs_ev_eol, debug_lane_lecs_packets,
	 "events", "LAN Emulation Config Server (LECS) Events", PRIV_OPR);

/* all */
EOLS    (debug_lane_lecs_all_eol, lecs_exec_debug_all, DEBUG_LANE_LECS_ALL);
KEYWORD (debug_lane_lecs_all, debug_lane_lecs_all_eol, debug_lane_lecs_events,
	 "all", "Enable all LAN Emulation Config Server (LECS) debugging", PRIV_OPR);

KEYWORD (debug_lane_lecs, debug_lane_lecs_all, debug_lane_lecsf,
	 "config", "LAN Emulation Config Server (LECS)", PRIV_OPR);

/******************************************************************
 * debug lane client topology
 */
EOLS	(debug_lane_lec_topo_eol, debug_command, DEBUG_LANE_LEC_TOPO);
KEYWORD	(debug_lane_lec_topo, debug_lane_lec_topo_eol, no_alt,
	"topology", "Client topology activity", PRIV_OPR);

/******************************************************************
 * debug lane client le-arp
 */
EOLS	(debug_lane_lec_le_arp_eol, debug_command, DEBUG_LANE_LEC_LE_ARP);
KEYWORD	(debug_lane_lec_le_arp, debug_lane_lec_le_arp_eol, debug_lane_lec_topo,
	"le-arp", "Client LE_ARP activity", PRIV_OPR);

/******************************************************************
 * debug lane client packet
 */
EOLS	(debug_lane_lec_packet_eol, debug_command, DEBUG_LANE_LEC_PACKET);
KEYWORD	(debug_lane_lec_packet, debug_lane_lec_packet_eol,
         debug_lane_lec_le_arp, "packet", "Client packet activity", PRIV_OPR);

/******************************************************************
 * debug lane client state
 */
EOLS	(debug_lane_lec_state_eol, debug_command, DEBUG_LANE_LEC_STATE);
KEYWORD	(debug_lane_lec_state, debug_lane_lec_state_eol, debug_lane_lec_packet,
	"state", "Client state machine", PRIV_OPR);

/******************************************************************
 * debug lane client signaling
 */
EOLS	(debug_lane_lec_signaling_eol, debug_command, DEBUG_LANE_LEC_SIGNAL);
KEYWORD	(debug_lane_lec_signaling, debug_lane_lec_signaling_eol, debug_lane_lec_state,
	"signaling", "Client signaling", PRIV_OPR);

/******************************************************************
 * debug lane client all
 */
EOLS	(debug_lane_lec_all_eol, lec_exec_debug_all, DEBUG_LANE_LEC_ALL);
KEYWORD	(debug_lane_lec_all, debug_lane_lec_all_eol, debug_lane_lec_signaling,
	"all", "Enable all Client debugging", PRIV_OPR);

/******************************************************************
 * debug lane client
 */
KEYWORD (debug_lane_lec, debug_lane_lec_all, debug_lane_lecs,
	 "client", "LAN Emulation Client", PRIV_OPR);

/******************************************************************
 * debug lane
 *
 */
KEYWORD_DEBUG(debug_lane, debug_lane_lec, ALTERNATE,
	      OBJ(pdb,1), lane_debug_arr,
	      "lane", "LAN Emulation", PRIV_OPR);

#undef	ALTERNATE
#define	ALTERNATE	debug_lane
