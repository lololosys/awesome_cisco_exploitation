/* $Id: lec_parser.c,v 3.7.8.15 1996/09/05 21:03:42 ppearce Exp $
 * $Source: /release/112/cvs/Xsys/lane/lec_parser.c,v $
 *------------------------------------------------------------------
 * lec_parser.c -- LAN Emulation Client parser routines
 *
 * October 1994, Brad Benson
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: lec_parser.c,v $
 * Revision 3.7.8.15  1996/09/05  21:03:42  ppearce
 * CSCdi68216:  alc-a-m does not build due to undefined references to srbV
 * Branch: California_branch
 *   Access srbV from registry rather than directly
 *
 * Revision 3.7.8.14  1996/08/28  12:55:05  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.7.8.13  1996/08/26  15:09:44  ppearce
 * CSCdi66582:  Implement SRB vector table to cut size of SRB_CORE subsys
 * Branch: California_branch
 *
 * Revision 3.7.8.12  1996/08/16  23:00:29  rbadri
 * CSCdi66287:  LANE: Clean up IPX/OSPF releated initializaion
 * Branch: California_branch
 * Move IPX & OSPF related registry invocations to lec_parser.c
 *
 * Revision 3.7.8.11  1996/08/12  06:14:02  bbenson
 * CSCdi65719:  Rearrange deck chairs.  create/delete client belongs in
 * lec_parser.c
 * Branch: California_branch
 *
 * Revision 3.7.8.10  1996/07/31  16:07:17  bbenson
 * CSCdi45072:  Restarting a LEC causes it to lose VLAN identity
 * Branch: California_branch
 *
 * Revision 3.7.8.9  1996/07/29  05:27:01  bbenson
 * CSCdi64313:  Pull out dead code dealing with HSRP MAC addresses.
 * Branch: California_branch
 *
 * Revision 3.7.8.8  1996/06/06  18:39:11  michellw
 * CSCdi59652:  make generic function for creating/deleting LEC
 * Branch: California_branch
 * The functions, lec_create_client() and lec_delete_client(), are
 * created to do the job.
 *
 * Revision 3.7.8.7  1996/05/31  16:50:41  michellw
 * CSCdi46369:  LEC remembers ELAN name even if it was never specified
 * Branch: California_branch
 * Not to save elan_name to lane_info->name when processing the
 * join_rsp message.
 *
 * Revision 3.7.8.6  1996/05/30  21:33:09  rbadri
 * CSCdi59101:  LE-ARP entry should use 6 byte MAC address instead of
 * hwaddrtype
 * Branch: California_branch
 * Changed the le_arp structure and all the associated functions to use
 * 6 byte MAC address instead of using 24 byte hwaddrtype to store 6
 * bytes.
 *
 * Revision 3.7.8.5  1996/05/23  04:25:53  kjadams
 * CSCdi57919:  Elan Segment ID field in show lane client needs to go on a
 * new line
 * Branch: California_branch
 * elan segment moved to a line with the route descriptor - displayed for
 * TRLANE only.
 *
 * Revision 3.7.8.4  1996/05/21  09:54:19  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.7.8.3  1996/05/17  11:25:36  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.7.8.2  1996/05/09  14:32:56  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.7.8.1.18.2  1996/05/03  18:04:50  rbadri
 * CSCdi56594:  Need better formatting on <show lane client> command
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 * Format the VLANID display properly.
 *
 * Revision 3.7.8.1.18.1  1996/04/27  07:06:48  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.7.8.1.8.10  1996/04/26  00:46:09  michellw
 * Branch: LE_Cal_V112_0_2_branch
 * Support two new commands "lane client idle IDLE-TIME" and
 * "clear lane client join-cnt".  Print the LEC uptime and
 * lec join-attempt-count in the "show lan client" display.
 *
 * Revision 3.7.8.1.8.9  1996/04/21  01:17:15  rbadri
 * Branch: LE_Cal_V112_0_2_branch
 * Create a new sub-system for TRLANE.
 *
 * Revision 3.7.8.1.8.8  1996/04/19  03:01:08  rbadri
 * Branch: LE_Cal_V112_0_2_branch
 * Initialize TR-LANE from (lane)parser, if not already done.
 *
 * Revision 3.7.8.1.8.7  1996/04/17  23:14:55  rbadri
 * Branch: LE_Cal_V112_0_2_branch
 * Convert TokenRing LANE related functions to registries
 * Move contents of lec_srb* srt/srb_lane.[ch], to facilitate
 * creation of new sub-system.
 *
 * Revision 3.7.8.1.8.6  1996/04/16  02:44:51  cakyol
 * goodbye to PVC support for LANE.
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 3.7.8.1.8.5  1996/04/13  23:13:08  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * - Don't call tr_lane_start, llc2_start or start_srb_background unless
 *   tr-lane is configured.
 * - First shot at calling lec_set_maxmtu to set the tr-lane MTU.
 * - Remove lane_type from lec_muex_config.  It's not needed.
 * - Only do the tr-lane config of srb when tr-lane is configured.
 *
 * Revision 3.7.8.1.8.4  1996/04/11  23:58:13  rbadri
 * Branch: LE_Cal_V112_0_2_branch
 * When a lane client is removed, zero the vlanid field of lec_info.
 *
 * Revision 3.7.8.1.8.3  1996/04/11  05:34:12  ddecapit
 * Branch: LE_Cal_V112_0_2_branch
 *
 * - Correct botched lecs.c sync
 * - Modify indentation to appease the pedantic
 *
 * Revision 3.7.8.1.8.2  1996/04/08  14:52:07  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 3.7.8.1.8.1  1996/04/08  01:57:13  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.7.8.1  1996/03/18  20:32:54  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.7.12.7  1996/04/06  02:52:30  rbadri
 * Branch: LE_Syn_Cal_V111_1_3_branch
 * In LE-ARP requests, fill in the source MAC of the originator and not
 * the MAC of client.
 *
 * Revision 3.7.12.6  1996/04/05  03:20:31  cakyol
 * started using the %Cx Cisco extensions to printf.  These are:
 *
 *   j: uchar*, atm RAW bytes.
 *
 *   h: hwaddrtype*, where type is either STATION_IEEE48 or
 *      STATION_AIPNSAP.
 *
 *   w: MaskedAddress*, where type is either STATION_IEEE48 or
 *      STATION_AIPNSAP.
 *
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.7.12.5  1996/04/03  21:09:11  rbadri
 * Branch: LE_Syn_Cal_V111_1_3_branch
 * During NVGEN, write the vlanid for Synergy.
 *
 * Revision 3.7.12.4  1996/04/02  10:09:54  cakyol
 * - perform re-verifications ONLY on completely operational clients.
 * - tidy up initializations, they were scattered all over the place.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.7.12.3  1996/03/28  17:34:46  rbadri
 * Branch: LE_Syn_Cal_V111_1_3_branch
 * MTU support for sub-interfaces (used by ATM interfaces)
 *
 * Revision 3.7.12.2  1996/03/27  23:39:55  cyoung
 * Remove vlan id from lec parser
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.7.12.1  1996/03/22  22:55:09  rlowe
 * Non-sync of Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.7  1996/02/28  03:03:41  rbadri
 * CSCdi49394:  CDP can not be enabled on LANE interfaces
 * Enable CDP on ATM interfaces, when LANE is configured and
 * when the LANE client is operational.
 *
 * Revision 3.6.8.3  1996/03/08  01:10:29  rbadri
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 * Fix router/Synergy conflicts.
 *
 * Revision 3.6.8.2  1996/03/05  06:43:18  rlowe
 * Apply LANE content to merge branch.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 3.6.8.1  1996/03/03  21:23:37  rlowe
 * Apply SYNALC content to merge branch.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 3.6.2.1  1996/02/27  21:10:36  cakyol
 * non sync sync of LE_Cal_V111_0_16_branch to V111_1_0_3 yielding
 * LE_Cal_V111_1_0_3_branch
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 3.6  1996/02/07  16:13:39  widmer
 * CSCdi48285:  Remove function name from bad_parser_subcommand
 *
 * Revision 3.5.4.1  1996/02/29  07:48:58  rlowe
 * First pass non-sync sync for Synergy ATM line card to V111_0_16.
 * Branch: Synalc3_Cal_V111_0_16_branch
 *
 * Revision 3.5.2.4  1996/02/13  00:17:58  rbadri
 * Branch: LE_Cal_V111_0_16_branch
 * DECnet routing for LANE
 *
 * Revision 3.5.2.3  1996/01/24  01:19:20  cakyol
 * - to satisfy popular demand, suppress printing of HSRP
 *   controlled mac addresses for "show lane client"
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 3.5.2.2  1996/01/23  23:54:27  cakyol
 * HSRP over LANE Code review changes:
 * - "interface_up" now works properly with HSRP after
 *   Badri's network.c fix.
 * - additional HSRP mac addresses implemented much
 *   faster on the LANE client.
 * - starting an already running or stopping an already
 *   stopped LANE client now behaves properly.
 * - white space cleanup
 *
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 3.5.2.1  1996/01/11  01:46:36  cakyol
 * non sync sync of LE_Calif_branch to V111_0_16 yielding
 *     LE_Cal_V111_0_16_branch
 *
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 3.5  1996/01/05  04:56:16  hampton
 * Remove extraneous includes of h/timer.h.  [CSCdi46482]
 *
 * Revision 3.4.6.1  1995/12/29  01:06:54  cakyol
 * Initial commits for:
 *
 * LANE Fault Tolerant LECS Servers
 * LANE Fault Tolerant LES Servers
 * HSRP over LANE Client
 *
 * Branch: LE_Calif_branch
 *
 * Revision 3.4  1995/11/29  05:20:47  bbenson
 * CSCdi43720:  7K crashes while doing show lane
 * It was freeing and then accessing a pointer.
 *
 * Revision 3.3  1995/11/24  19:39:45  rbadri
 * CSCdi36121:  Line protocol is up, while LANE client is non-operational
 * subif_line_state variable will track the swidb's line state.
 * Use subif_line_statechange() to control the operational state of
 * the sub-interface. In LECS/LES use interface_admin_down instead of
 * interface up to determine the adminstrative nature of the sub-interface.
 *
 * Revision 3.2  1995/11/17  17:51:08  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:23:26  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.10.18.1  1996/02/03  07:07:14  rlowe
 * Non-sync sync of Synalc_catchup_branch to V111_0_5.
 * Branch: Synalc_catchup_pretal_branch
 *
 * Revision 2.10  1995/08/30  02:34:12  bbenson
 * CSCdi37992:  LANE packet count can go negative
 * - Change appropriate format chars from d to u.  AKA gratuitous commit
 *   to impress the boss.
 *
 * Revision 2.9.16.2.2.1  1996/01/19  21:56:53  sgarcia
 * CSCdi47203:  sync 11.1 catchup branch to 11.0 ATMFLASH_V11_951214 tag
 * of Synalc2_
 * Branch: Synalc_catchup_branch
 * Patch 11.0 Synalc2_LE2_V110_1_0_3_branch diffs between tags
 * Synalc2_LE2_V110_1_0_3_arizona_arkansas_split_951103 and
 * ATMFLASH_V11_951214
 * into 11.1 branch.
 *
 * Revision 2.9.16.3  1995/11/10  01:51:44  bbenson
 * CSCdi43720:  7K crashes while doing show lane
 * Branch: Synalc2_LE2_V110_1_0_3_branch
 * It was freeing and then accessing a pointer.  Bad dog.
 *
 * Revision 2.9.16.2  1995/10/26  02:35:20  bbenson
 * CSCdi39695:  <show lane client> should indicate VLAN number
 * Branch: Synalc2_LE2_V110_1_0_3_branch
 * Added display of the VLANID for "show lane client" on the C5000.
 *
 * Revision 2.9.16.1  1995/09/11  01:56:23  rlowe
 * Non-sync sync of Synalc2_branch to LE2_V110_1_0_3_branch,
 * yielding Synalc2_LE2_V110_1_0_3_branch.
 *
 * Branch: Synalc2_LE2_V110_1_0_3_branch
 *
 * Revision 2.9  1995/08/14  19:14:35  bbenson
 * CSCdi35018:  AS should be turned off when 1577 and LANE are on the same
 * AIP.
 *
 * Revision 2.8  1995/08/06  21:59:53  bbenson
 * CSCdi36725:  LEC needs to obtain LECS address from the switch.
 * - Locally implement LECS address determination (not using cfgacc).
 * - Use interface_admin_down instead of interface_up_simple.
 *
 * Revision 2.7  1995/07/30  01:15:53  bbenson
 * CSCdi37903:  Not necessary to invalidate caches immediately for LANE.
 * - No more calls to reg_invoke_bump_cache_version with an immediate
 *   request.
 * - Also collapsed numerous message pointers in the lec_info_t into a
 *   single union
 *
 * Revision 2.6.6.1  1995/09/02  01:58:05  bbenson
 * Branch: Synalc2_branch
 * Sync in LANE for the new improved Synalc branch.
 *
 * Revision 1.1.4.10.2.6  1995/07/25  06:02:23  bbenson
 * Branch: Synergy_ATMlc_branch
 * - Sync'd LANE code on Synergy_ATMlc_branch up to V110_0_12 on mainline
 *   from the head of the LE_branch (last time it was sync'd).  Rest of
 *   the Synergy branch is left as-is.
 *
 * Revision 1.1.4.10.2.5  1995/06/15  21:00:30  rahuja
 * Branch: Synergy_ATMlc_branch
 * Committing working version of Synergy LANE.
 *
 * Revision 1.1.4.10.2.4  1995/06/08  19:27:44  bbenson
 * Branch: Synergy_ATMlc_branch
 * Add EARL ipc.
 *
 * Revision 1.1.4.10.2.3  1995/06/01  22:46:11  sakumar
 * Changes made to support LANE Client on Synergy ATM card.
 * Branch: Synergy_ATMlc_branch
 *
 * Revision 1.1.4.10.2.2  1995/05/25  22:04:35  rahuja
 * Branch: Synergy_ATMlc_branch
 * Committing latest LE_branch (which just got folded into the
 * Arizona_branch)
 * into Synergy_ATMlc_branch.
 *
 * Revision 2.6  1995/07/12  07:28:35  bbenson
 * CSCdi36903:  LEC needs to align show lane output with other components.
 * CSCdi36945:  LEC doesn't handle RELEASE COMPLETE in idle state.
 *
 * Revision 2.5  1995/06/26  23:22:00  nfinn
 * CSCdi35235:  LANE config server access sequencing incorrect
 *
 * Revision 2.4  1995/06/22  01:28:22  bbenson
 * CSCdi35017:  Client should disallow rfc1577 and vice versa on subint.
 *
 * Revision 2.3  1995/06/19  21:12:44  bbenson
 * CSCdi36050:  show lane client indicates join attempt while client
 * operational
 * - Turn off showing seconds to next join when already operational
 *
 * Revision 2.2  1995/06/15  16:56:33  bbenson
 * CSCdi35803:  show lane client should display lecid and secs to Join
 *         - Add another line of output to the "sh lane cli" command
 *
 * Revision 2.1  1995/06/07  21:20:09  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include "../../boot/cpu.h"
#include <string.h>
#include "media_registry.h"
#include "mgd_timers.h"
#include "interface_private.h"
#include "packet.h"
#include "address.h"
#include "name.h"
#include "config.h"
#include "parser.h"
#include "ttysrv.h"
#include "logger.h"
#include "../lane/msg_lane.c"		/* Not a typo, see logger.h */
#include "../ui/debug.h"
#include "parser_defs_lane.h"
#include "sys_registry.h"
#include "subsys.h"
#include "../if/atm.h"
#include "../if/ether.h"
#include "../if/tring.h"
#include "../if/network.h"
#include "../atm/atmsig_api.h"
#include "../atm/ilmi_api.h"

#include "lane.h"
#include "lane_private.h"
#include "lec_private.h"
#include "lec_fsm.h"
#include "lec_pdu.h"
#include "lane_debug.h"
#include "lane_registry.h"

#include "../ip/ip_registry.h"
#include "../xns/novell.h"
#include "../clns/clns_route.h"
#include "../clns/isis.h"
#include "../xns/ipx_registry.h"

#include "../srt/rsrb.h"
#include "../srt/srb.h"
#include "../srt/srb_core.h"
#include "../srt/srb_core_registry.h"
#include "../srt/lack_externs.h"
#include "../llc2/llc2.h"



/*
 * lec_create_client
 *
 * This is a generic routine to create a LEC on a sub-interface.
 *
 * Virtual ring number (vr_rn) and bridge number (srb_bn)
 * are only applicable for Token Ring LEC.
 * Virtual lane id (vlan_id) is for synergy box.
 */
boolean lec_create_client (idbtype 	*swidb, 
			   lane_caller_t lane_caller,
			   int 		lane_type, 
			   uchar 	*elan_name, 
			   int 		vr_rn, 
			   int 		srb_bn, 
			   int 		vlan_id)
{
    hwidbtype 		*hwptr;
    lane_info_t		*lane_info;
    lec_info_t		*lec_info;
    int			name_size;
    int			sub_mtu;

    hwptr = swidb->hwptr;
    lane_info = swidb->lane_info;

    /*
     * Create an LEC.  If there is no lane_info or lec_info
     * struct, the call to lec_init_idb will create them.
     */
    if (!lane_info || (lane_info && !lane_info->lec_info)){
	lec_info = lec_init_idb(swidb);
	if (!lec_info)
	    return FALSE;
	lane_info = lec_info->lane_info;
    } else {
	lec_info = lane_info->lec_info;
    }

    /* 
     * if a LEC is already running here, get out 
     */
    if (lane_info->lec_enabled) {
	if (lane_caller == LANE_CALLER_CLI)
	    printf("\n%% An LEC is already running here.");
	return FALSE;
    }

    /*
     * Get type.  Convert to internal values.
     * Set the MTU of the AIP if 1577 isn't already on it.  If
     * there was something configured that conflicts, the
     * lec_muex_config will catch it.
     */
    if (lec_muex_config(swidb)) {
	return FALSE;
    }

    switch (lane_type) {
    case LANE_TYPE_ETHERNET:
	lane_type = LANE_TYPE_802_3;
	break;
    case LANE_TYPE_TOKEN_RING:
	lane_type = LANE_TYPE_802_5;

	lec_info->lec_is_srb = TRUE;
	lec_info->vr_rn = vr_rn;
	lec_info->srb_bn = srb_bn;

	/* set the mcast send/forward correctly since they defaulted to EN */ 
	lec_info->mcast_send.blli = &lec_blli_token_ring_mcast;
	lec_info->mcast_send.lane_vc_type = LANE_LEC_TR_SEND;
	lec_info->mcast_forward.blli = &lec_blli_token_ring_mcast;
	lec_info->mcast_forward.lane_vc_type = LANE_LEC_TR_FORWARD;

	if (!reg_invoke_tr_lane_vrfind_ringgroup(lec_info->vr_rn) ) {
	    if (lane_caller == LANE_CALLER_CLI) {
		printf("\n%%virtual ring %d must be the same as a ring-group",
		       lec_info->vr_rn);
		printf("\nas configured by the source-bridge ring-group command");
	    }
	    return FALSE;
	}
	break;
    default:
	return FALSE;
    }

    /*
     * If this reg call exists, we're on Synergy.  If the
     * vlanid is 0, then we've failed to configure it.
     */
    if (reg_invoke_lec_cfg_vlanid(swidb, vlan_id) && !lec_info->vlanid) {
	return FALSE;
    }

    /*
     * Check legality of name and type.
     */
    name_size = strlen(elan_name);
    if (name_size > MAX_ELAN_NAME_LENGTH) {
	if (lane_caller == LANE_CALLER_CLI)
	    printf("\n%% ELAN name must be 1-32 characters long");
	return FALSE;
    }

    /* 
     * At this point, there is no LEC running on 
     * this subinterface. So we only need to check
     * the LES configuraiton.
     */
    if (lane_info->name_from_les &&
	name_size &&
	(lane_info->name_size != name_size  ||
	 memcmp(lane_info->name, elan_name, name_size))) {
	if (lane_caller == LANE_CALLER_CLI)
	    printf("\n%% Attempt to change ELAN name");
	return FALSE;
    } else if (lane_info->type_from_les &&
	       lane_info->type != lane_type) {
	if (lane_caller == LANE_CALLER_CLI)
	    printf("%% Attempt to change ELAN type");
	return FALSE;
    }

    /* Take name if given.  If not, use name from LES/BUS command.
     * Use 0 if no name given anywhere.
     */
    if (name_size) {
	lane_info->name_from_lec = TRUE;
	memset(lane_info->name, 0, MAX_ELAN_NAME_LENGTH);
	bcopy(elan_name, lane_info->name, name_size);
	lane_info->name_size = name_size;
    } else if (lane_info->name_from_les) {
	lane_info->name_from_lec = FALSE;
    } else {
	memset(lane_info->name, 0, MAX_ELAN_NAME_LENGTH);
	lane_info->name_size = 0;
    }
    lec_info->actual_name_size = lane_info->name_size;
    bcopy(lane_info->name, lec_info->actual_elan_name,
	  MAX_ELAN_NAME_LENGTH);

    /*
     * Set type and frame size
     */
    lane_info->type = lane_type;
    if (!if_lane_exist(swidb)) {
	/* 
	 * if mtu is configured on this sub-interface, 
	 * check whether it is a valid value;
	 * otherwise use the default values.
	 */
	if (swidb->user_configured_mtu) {
	    if (is_valid_lane_mtu(swidb, swidb->sub_mtu)) {
		lane_info->max_data_frame = lane_mtu_to_frame(swidb);
	    } else {
		if (lane_caller == LANE_CALLER_CLI)
		    printf("%% Invalid LANE MTU %d.", swidb->sub_mtu);
		    printf("\n  Valid sizes are %d and %d for Ethernet and "
			   "%d and %d for TR.",
			   LANE_MTU_ETHER_1516, LANE_MTU_ETHER_4544,
			   LANE_MTU_TR_4544, LANE_MTU_TR_9234);	
		return(FALSE);
	    }
	} else {
	    lane_info->max_data_frame = lane_default_frame_size(swidb);
	    sub_mtu = lane_default_mtu(swidb);

	    if (!lane_set_maxmtu(swidb, sub_mtu))
		return(FALSE);
	}
    }

    /*
     * Set data connection sdu size
     */
    lec_info->mcast_send.sdu_size =
    lec_info->mcast_forward.sdu_size =
	lane_frame_to_sdu_size(lane_info->max_data_frame);
   
    if (lane_info->type == LANE_TYPE_802_5) {
        srb_vector_table_t  *srbV_p;
	reg_invoke_tr_lane_start();

	/* find out whether AIP is already configured for LANE SRB */
        srbV_p = reg_invoke_srb_get_vector_table();
        if (srbV_p) {
            ushort srb_thisring = srbV_p->srb_sb_get_thisring(swidb);
            ushort srb_targetring = srbV_p->srb_sb_get_targetring(swidb);
	    if ((srb_thisring == SRB_INVALID_RING_NUM) && 
	        (srb_targetring == SRB_INVALID_RING_NUM)) {
	        /* it's new! - add the virtual ring */
                srbV_p->srb_sb_set_bridge_num(swidb, lec_info->srb_bn);
	        srbV_p->srb_sb_set_targetring(swidb, lec_info->vr_rn);
	        hwptr->tr_transit_oui = ETHEROUI;
	    }
	}
    }

    /*
     * setup the interface to be a broadcast for OSPF's use.
     */
    reg_invoke_ospf_configure_for_broadcast(swidb, TRUE);

    /*
     * Change in encapsulation due to Client being configured on the
     * interface.
     */
    reg_invoke_set_protocol_encaps(LINK_VINES, swidb, ET_ARPA);
    lane_info->type_from_lec = TRUE;
    lec_info->actual_lan_type = lane_info->type;
    lec_info->actual_max_frame_size = lane_info->max_data_frame;

    if (! lane_info->lec_enabled) lec_count++;

    lec_inputQ_enabled = TRUE;
    lane_info->lec_enabled = TRUE;
    /* 
     * Make sure IPX is configured for LANE on this idb.
     */
    reg_invoke_ipx_configure_for_lane(lec_info->swidb);
    lec_start_process();

    if (interface_admin_down(swidb)) {
	lec_info->admin_status = LANE_INACTIVE;
    } else {
	lec_info->admin_status = LANE_ACTIVE;
    }

    /* register the lec_info struct with SNMP for
     * LEC-MIB and IF-MIB support
     * If the entry was created by SNMP, then this call will overwrite
     * any values already written to the lec_info struct, depending
     * on the values set in the SNMP PDU (i.e. lecConfigMode)
     */
    lec_info->lec_snmp = NULL;
    reg_invoke_lecmib_register_lec_info(lec_info);

    lec_fsm(lec_info, LEC_MAJOR_LOCAL, LEC_LOCAL_ACTIVATE);
 
    return TRUE;
}


/* 
 * lec_delete_client
 *
 * Given a sw idb, delete the LEC on that sub-interface.
 */
boolean lec_delete_client (idbtype 	*swidb, 
			   lane_caller_t lane_caller)
{
    lane_info_t		*lane_info;
    lec_info_t		*lec_info;

    lane_info = swidb->lane_info;
    /*
     * setup the interface to be a non-broadcast for OSPF's use.
     */
    reg_invoke_ospf_configure_for_broadcast(swidb, FALSE);

    if (lane_info) {
	/* 
	 * if none running anyway, get out
	 */
	if (! lane_info->lec_enabled) {
	    if (lane_caller == LANE_CALLER_CLI)
		printf("\n%% there is no LEC running here");
	    return TRUE;
	}

	lec_info = lane_info->lec_info;
	lane_info->type_from_lec = FALSE;
	lane_info->name_from_lec = FALSE;

	if (lec_info) {

	    /* clear out hsrp addresses */
	    bzero(lec_info->lecHsrpMacs, LEC_HSRP_MACS);

	    if (lec_info->admin_status == LANE_ACTIVE) {
		/*
		 * The interface was up previously. But, LANE may not
		 * be operational. Restore the subif state so that 
		 * "no lane" would leave subif in the "correct" state.
		 */
		lec_line_statechange(lec_info->swidb,
				     SUBIF_STATE_ADMINDOWN);
		lec_info->swidb->subif_line_state = SUBIF_STATE_UP;
		subif_line_statechange(lec_info->swidb, SUBIF_STATE_UP, FALSE);
	    }
	    lec_info->admin_status = LANE_INACTIVE;
	    lec_fsm(lec_info, LEC_MAJOR_LOCAL, LEC_LOCAL_DEACTIVATE);
	    lec_info->vlanid = 0;
	    lec_info->actual_name_size    = 0;
	    lec_info->actual_elan_name[0] = '\0';

	    /*
	     * If no other lec is enabled, shut down the
	     * lec_inputQ by setting lec_inputQ_enabled to FALSE.
	     */
	    if (lane_info->lec_enabled) lec_count--;

	    if (lec_count <= 0) {
		lec_count = 0;
		lec_inputQ_enabled = FALSE;
	    }

	    lec_info->join_attempt_cnt = 0;
	    reg_invoke_lecmib_deregister_lec_info(lec_info);

	}
	lane_info->lec_enabled = FALSE;
    }
    /*
     * If Vines is configured, then set the encapsulation to be SNAP
     */
    reg_invoke_set_protocol_encaps(LINK_VINES, swidb, ET_SNAP);

    reg_invoke_set_protocol_encaps(LINK_NOVELL, swidb, ET_NULL);

    return TRUE;
}


/*
 * lec_cfg_lec
 *
 * This function handles the config command:
 *
 * [no] lane client {ethernet | token-ring} [segment SEGMENT] [VLAN-ID] [ELAN-NAME]
 */
void lec_cfg_lec (parseinfo *csb)
{
    idbtype	*swidb;
    uchar	*elan_name;
    int		lane_type;
    int		srb_bn, vr_rn, vlan_id;
    boolean	result;

    swidb = csb->interface;
    if (!swidb) {
	return;
    }

    if (csb->sense) {
	lane_type = GETOBJ(int, 1);
	vr_rn 	= GETOBJ(int, 2);
	srb_bn 	= GETOBJ(int, 3);
	vlan_id = GETOBJ(int, 4);
	elan_name = (uchar *)GETOBJ(string, 1);

	result = lec_create_client(swidb, LANE_CALLER_CLI,
				   lane_type, elan_name, vr_rn, 
				   srb_bn, vlan_id);
    } else {
	result = lec_delete_client(swidb, LANE_CALLER_CLI);
    }

    return;
}


/*
 * lec_cfg_lec_nsap
 *
 * This function handles the config command:
 *
 * [no] lane client-atm-address NSAP-TEMPLATE
 */
void lec_cfg_lec_nsap (parseinfo *csb)
{
    lec_info_t	*lec_info;
    idbtype	*swidb;

    swidb = csb->interface;
    if (!swidb) {
	return;
    }

    lec_info = lec_info_from_swidb(swidb);
    if (!lec_info) {
	    lec_info = lec_init_idb(swidb);
    }

    /*
     * Either create or delete the NSAP-TEMPLATE.
     */
    if (csb->sense) {
	bcopy(GETOBJ(maskedAddr,1), 
	      &lec_info->masked_addr, 
	      sizeof(MaskedAddress));
	if (lec_info->masked_addr.length == 0) {
	    memset(&lec_info->masked_addr, 0, sizeof(MaskedAddress));
	    printf("\n%%ATM nsap template not configured, length was zero.");
	}
    } else {
	memset(&lec_info->masked_addr, 0, sizeof(MaskedAddress));
    }

    /*
     * Kick the state machine with a config change if we're already up.
     */
    lec_fsm(lec_info, LEC_MAJOR_LOCAL, LEC_LOCAL_CONFIG_CHANGE);
}


/*
 * lec_cfg_le_arp
 *
 * This function handles the config command: 
 *
 *	[no] lane le-arp MAC-ADDRESS ATM-ADDRESS
 *
 * Stuffs/clears a *permanent* entry in the LE ARP table.  If the
 * entry already exists, delete and recreate the information.  Note that this
 * does not result in an entry in the nsap table and subsequent call
 * being set up.  This will occur when the packet flows through lec_find_vc.
 */
void lec_cfg_le_arp (parseinfo *csb)
{
    lec_info_t *lec_info;
    lec_le_arp_t *le_arp;
    idbtype	*swidb;
    uchar       *src_mac;
    hwaddrtype  *dst;

    swidb = csb->interface;
    if (!swidb) {
	return;
    }

    lec_info = lec_info_from_swidb(swidb);
    if (!lec_info) {
	lec_info = lec_init_idb(swidb);
    }
    
    /*
     * If sense == TRUE, create an entry.
     */
    dst = GETOBJ(hwaddr, 1);
    if (csb->sense) { 

	le_arp = lec_lookup_le_arp(lec_info, dst->addr); 
	if (le_arp) {
	    lec_delete_le_arp_entry(lec_info, dst->addr);
	}
	src_mac = lec_info->normalMac;
	le_arp = lec_create_le_arp_entry(lec_info, 
					 dst->addr,
					 src_mac,
					 LEC_MAC_RESOLVED,
					 LEC_LE_ARP_PERMANENT);
	bcopy(GETOBJ(hwaddr, 2), &le_arp->nsap, sizeof(hwaddrtype));
        if (lec_enabled(swidb))
	    lec_lookup_connection(le_arp);
    } else {
	le_arp = lec_lookup_le_arp(lec_info, dst->addr);
	if (le_arp && (le_arp->entry_type == LEC_LE_ARP_PERMANENT)) {
	    lec_delete_le_arp_entry(lec_info, dst->addr);
	}
    }
}


/*
 * lec_exec_debug_all
 *
 * This function handles the exec command:
 *
 *	[no] debug lane lec all
 *
 */
void lec_exec_debug_all (parseinfo *csb)
{
    lec_debug_all(csb->sense);
}


/*
 * lec_gen_lec
 *
 * This function generates the output for nvgen:
 *
 *    "lane client {ethernet | tokenring SEGMENT bridge BRIDGE} VLANID [ELAN-NAME] "
 */
void lec_gen_lec (parseinfo *csb)
{
    lane_info_t	*lane_info;
    lec_info_t 	*lec_info;

    lec_info  = lec_info_from_csb(csb);
    if (!lec_info) {
	return;
    }

    lane_info = lec_info->lane_info;
    if (lec_info && lane_info->type_from_lec) {
	nv_write(TRUE, "%s", csb->nv_command);

	switch(lane_info->type) {
	case LANE_TYPE_ETHERNET:
	    nv_add(TRUE, " %s", "ethernet");
	    break;
	case LANE_TYPE_TOKEN_RING:
	    nv_add(TRUE, " %s %s %d %s %d", "tokenring",
		   "segment", lec_info->vr_rn, "bridge", lec_info->srb_bn);
	    break;
	}
	if (cpu_type == CPU_SYNALC && lec_info->vlanid)
	    nv_add(lec_info->vlanid, " %d", lec_info->vlanid);
	nv_add(lane_info->name_from_lec, " %s", lane_info->name);
    }
}


/*
 * lec_gen_le_arp
 *
 * This function generates the output for nvgen:
 *
 *	lane le-arp MAC-ADDRESS ATM-ADDRESS
 *
 */
void lec_gen_le_arp (parseinfo *csb)
{
    int			index;
    lec_info_t		*lec_info;
    lec_le_arp_t	*le_arp, *temp;
    uchar		nsapstr[ATM_MAX_NSAP_STRING];

    lec_info  = lec_info_from_csb(csb);
    if (!lec_info) {
	return;
    }

    mem_lock(lec_info);
    for (index = 0; index < LANE_NETHASHLEN; index++) {
	if (lec_info->le_arp_table[index]) {
	    le_arp = lec_info->le_arp_table[index];
	    while (le_arp) {
		mem_lock(le_arp);
		if (le_arp->entry_type == LEC_LE_ARP_PERMANENT) {
		    atm_printnsap(le_arp->nsap.addr, nsapstr);
		    nv_write(TRUE, "%s", csb->nv_command);
		    nv_add(TRUE, " %e", le_arp->mac);
		    nv_add(TRUE, " %s", nsapstr);
		}
		temp = le_arp->next;
		free(le_arp);
		le_arp = temp;
	    }
	}
    }
    free(lec_info);
}


/*
 * lec_gen_lec_nsap
 *
 * This function generates the output for nvgen:
 *
 * 	"lane client-atm-address NSAP-TEMPLATE"
 */
void lec_gen_lec_nsap (parseinfo *csb)
{
    lec_info_t *lec_info;

    lec_info = lec_info_from_csb(csb);
    if (lec_info) {
	nv_write((lec_info->masked_addr.length != 0), 
		 "%s %Cw", csb->nv_command, &lec_info->masked_addr);
    }
}


/*
 * lec_exec_clear_le_arp
 *
 * This function handles the exec command:
 *
 *	clear lane le-arp [interface SUBINTERFACE [MAC] | name ELAN-NAME [MAC]]
 *
 * OBJ(int, 1) tells us whether common_str_interface or "name" was specified.
 *
 * OBJ(int, 2) tells us if MAC was specified.
 *
 */
void lec_exec_clear_le_arp (parseinfo *csb)
{
    lec_info_t		*lec_info;
    hwaddrtype  	*dst;

    switch (GETOBJ(int, 1)) {

    case CLEAR_LE_ARP_BY_INT:
        FOR_ALL_LEC(lec_info) {
            if ((lec_info->swidb == GETOBJ(idb, 1)) &&
                 (lec_info->admin_status != LANE_INACTIVE))
                goto got_lec;
        }
        printf("\n%%no LAN Emulation client on interface %s",
               GETOBJ(idb, 1)->namestring);
        return;

    case CLEAR_LE_ARP_BY_NAME:
        FOR_ALL_LEC(lec_info) {
            if ((lec_info->actual_name_size == strlen(GETOBJ(string, 1))) &&
                !memcmp(lec_info->actual_elan_name, GETOBJ(string, 1),
			lec_info->actual_name_size) &&
                (lec_info->admin_status != LANE_INACTIVE))
                goto got_lec;
        }
        printf("\n%%no LAN Emulation client with name %s", GETOBJ(string, 1));
        return;

    default:
    case CLEAR_LE_ARP_ALL:
        FOR_ALL_LEC(lec_info) {
            lec_flush_le_arp_table(lec_info);
        }
        return;

    }

   got_lec:
    if (GETOBJ(int, 2) == CLEAR_LE_ARP_MAC) {
    	dst = GETOBJ(hwaddr, 1);
        if (!lec_delete_le_arp_entry(lec_info, dst->addr))
            printf("\n%%MAC address %e not found", dst->addr);
    } else {
        lec_flush_le_arp_table(lec_info);
    }

    return;        
                
}


/*
 * lec_show_one_le_arp
 *
 * A utility function used by lec_show_one_le_arp to display a single
 * LEC's le-arp table.  It prints out the MAC, ATM address, vcd and
 * interface for each row.
 */
static void lec_show_one_le_arp (lec_info_t *lec_info)
{
    int			index;
    lec_le_arp_t	*le_arp, *temp;
    lec_nsap_info_t	*lec_nsap_info;
    lane_info_t		*lane_info;
    uchar		nsapstr[ATM_MAX_NSAP_STRING];
    vcd_t		vcd;

    lane_info = lec_info->lane_info;
    mem_lock(lane_info);
    mem_lock(lec_info);

    for (index = 0; index < LANE_NETHASHLEN; index++) {
	if (lec_info->le_arp_table[index]) {
	    le_arp = lec_info->le_arp_table[index];
	    while (le_arp) {
		/*
		 * Need the nsap_info entry to print out the vcd.
		 */
		lec_nsap_info = lec_lookup_nsap(lec_info,
						&le_arp->nsap);
		if (lec_nsap_info) {
		    vcd = lec_nsap_info->vc.vcd;
		} else {
		    vcd = lec_info->mcast_send.vcd;
		}
		mem_lock(le_arp);
		atm_printnsap(le_arp->nsap.addr, nsapstr);
		printf("\n%e  %s %-3u%s %s",
		       le_arp->mac,
		       nsapstr,
		       vcd,
		       lec_nsap_info ? " " : "*",
		       lec_info->swidb->namestring);
		temp = le_arp->next;
		free(le_arp);
		le_arp = temp;
	    }
	}
    }
    free(lec_info);
    free(lane_info);
}


/*
 * lane_show_one_lec
 *
 */
void lane_show_one_lec (lane_info_t *lane_info, boolean brief)
{
    int			index;
    lec_nsap_info_t	*nsap_info, *temp;
    lec_info_t		*lec_info;
    uchar		elan_name[MAX_ELAN_NAME_LENGTH + 1];
    uchar		nsapstr[ATM_MAX_NSAP_STRING];
    unsigned		in_pkts, out_pkts;

    if (!lane_info) {
	return;
    }

    lec_info = lane_info->lec_info;
    if (!lec_info ||
	!lec_info->swidb ||
	!lec_info->swidb->hwptr ||
	!lec_enabled(lec_info->swidb)) {
	return;
    }

    mem_lock(lane_info);
    mem_lock(lec_info);

    bcopy(lec_info->actual_elan_name, 
	  elan_name,
	  lec_info->actual_name_size);
    elan_name[lec_info->actual_name_size] = 0;

    printf("\nLE Client %s  ELAN name: %s  Admin: %s  State: %s",
	   lec_info->swidb->namestring,
	   elan_name,
	   lec_info->admin_status == LANE_ACTIVE ? "up" : "down",
	   lec_pr_atmf_state(lec_info->state));

    if (lec_info->state == LEC_ST_IDLE) {
	printf("\nClient ID: unassigned        ");
	if (lec_info->admin_status == LANE_ACTIVE) {
	    printf("Next join attempt in %u seconds",
	       mgd_timer_left_sleeping(&lec_info->lec_timer) / ONESEC);
	}

    } else if (lec_info->state == LEC_ST_ACTIVE) {

	sys_timestamp now;
	typedef struct lec_time_t {
	    ulong sec;
	    ulong min;
	    ulong hour;
	    ulong day;
	    ulonglong msec;
	} lec_time;
	lec_time time;
	
	GET_TIMESTAMP(now);
	time.msec = TS_VALUE(now) - TS_VALUE(lec_info->time_when_active);
	time.sec = (time.msec)/1000; 	time.msec = (time.msec)%1000;
	time.min = (time.sec)/60;	time.sec = (time.sec)%60;
	time.hour = (time.min)/60;	time.min = (time.min)%60;
	time.day = (time.hour)/24;	time.hour = (time.hour)%24;
	
    	/* sprint_dhms(buff, lec_info->time_when_active); */
	printf("\nClient ID: %18u", lec_info->lecid);

	printf("LEC up for ");
	if (time.day)
	    printf("%u day%s ", time.day, (time.day > 1) ? "s" : "");
	if (time.hour)
	    printf("%u hour%s ", time.hour, (time.hour > 1) ? "s" : "");
	if (time.min)
	    printf("%u minute%s ", time.min, (time.min > 1) ? "s" : "");
	printf("%u second%s ", time.sec, (time.sec > 1) ? "s" :	"");	

    } else if (lec_info->state >= LEC_ST_REGISTRATION) {
	printf("\nClient ID: %18u", lec_info->lecid);

    } else {
	printf("\nClient ID: unassigned        ");
    }

    printf("\nJoin Attempt: %15u", lec_info->join_attempt_cnt);

    if (lec_info->config_idle_time) {
	printf("Configured Idle Time: %d seconds", 
	       (lec_info->config_idle_time) / ONESEC);
    }

    if ((lec_info->admin_status == LANE_ACTIVE) && 
	(lec_info->state != LEC_ST_ACTIVE) &&
	(lec_info->last_fail_reason != LEC_FAIL_INIT))
	printf("\nLast Fail Reason: %s",
	       lec_pr_fail_reason(lec_info->last_fail_reason));

    printf("\nHW Address: %e   Type: %s\t\tMax Frame Size: %16s",
	   lec_info->normalMac,
	   lane_pr_lan_type(lec_info->actual_lan_type),
	   lane_pr_max_frame_size(lec_info->actual_max_frame_size));

    /*
     * This variable is only set in the Synergy platform.  Yeah, it
     * should be a registry, but for some reason, the printfs come out
     * async. 
     */
    if (lec_info->vlanid != 0) {
	printf("VLANID: %21u\t", lec_info->vlanid);
    }

    if (lec_info->actual_lan_type == LANE_TYPE_802_5) {
	 printf("\nRoute Descriptor: %d.%d", lec_info->vr_rn, 
                  lec_info->srb_bn);
    	 printf("       ELAN Segment ID: ");
	 if (lec_info->elan_rn == 0)
	    printf("unassigned");
	 else
	    printf("%d", lec_info->elan_rn);
    }

    atm_printnsap(lec_info->nsap.addr, nsapstr);
    printf("\nATM Address: %s\n", nsapstr);

    if (!brief) {

        automore_header(" VCD  rxFrames  txFrames  Type       ATM Address");

        /* Print the CONFIGURE nsap information */
        atm_printnsap(lec_info->configure.nsap.addr, nsapstr);
        atm_getvc_counts(lec_info->swidb->hwptr,
                         lec_info->configure.vcd,
                         &in_pkts, &out_pkts);
        printf("\n%-4u%-10u%-10u  configure  %s",
               lec_info->configure.vcd, in_pkts, out_pkts, nsapstr);

        /* Print the CONTROL DIRECT nsap information */
        atm_printnsap(lec_info->ctl_direct.nsap.addr, nsapstr);
        atm_getvc_counts(lec_info->swidb->hwptr,
                         lec_info->ctl_direct.vcd,
                         &in_pkts, &out_pkts);
        printf("\n%-4u%-10u%-10u  direct     %s",
               lec_info->ctl_direct.vcd, in_pkts, out_pkts, nsapstr);
	   
        /* Print the CONTROL DISTRIBUTE nsap information */
        atm_printnsap(lec_info->ctl_distribute.nsap.addr, nsapstr);
        atm_getvc_counts(lec_info->swidb->hwptr,
                         lec_info->ctl_distribute.vcd,
                         &in_pkts, &out_pkts);
        printf("\n%-4u%-10u%-10u  distribute %s",
               lec_info->ctl_distribute.vcd, in_pkts, out_pkts, nsapstr);
   
        /* Print the MCAST SEND nsap information */
        atm_printnsap(lec_info->mcast_send.nsap.addr, nsapstr);
        atm_getvc_counts(lec_info->swidb->hwptr,
                         lec_info->mcast_send.vcd,
                         &in_pkts, &out_pkts);
        printf("\n%-4u%-10u%-10u  send       %s",
               lec_info->mcast_send.vcd, in_pkts, out_pkts, nsapstr);
	   
        /* Print the MCAST FORWARD nsap information */
        atm_printnsap(lec_info->mcast_forward.nsap.addr, nsapstr);
        atm_getvc_counts(lec_info->swidb->hwptr,
                         lec_info->mcast_forward.vcd,
                         &in_pkts, &out_pkts);
        printf("\n%-4u%-10u%-10u  forward    %s",
               lec_info->mcast_forward.vcd, in_pkts, out_pkts, nsapstr);

        /* Print all of the data direct VCs for this ELAN */
        for (index = 0; index < LANE_NETHASHLEN; index++ ) {
            nsap_info = lec_info->nsap_table[index];
            while (nsap_info) {
                mem_lock(nsap_info);
                atm_printnsap(nsap_info->vc.nsap.addr, nsapstr);
                atm_getvc_counts(lec_info->swidb->hwptr,
                                 nsap_info->vc.vcd,
                                 &in_pkts, &out_pkts);
                printf("\n%-4u%-10u%-10u  data       %s",
                       nsap_info->vc.vcd, in_pkts, out_pkts, nsapstr);
		temp = nsap_info->next;
		free(nsap_info);
		nsap_info = temp;
            }
        }

    } /* end if (!brief) */

    printf("\n");

    free(lec_info);
    free(lane_info);

}


/*
 * lec_show_commands
 *
 * This function handles the exec command:
 *
 *	show lane le-arp [interface SUBINTERFACE | name ELAN-NAME]
 *	show lane lec [interface SUBINTERFACE | name ELAN-NAME]
 *
 * show client information attached to specified lane_info structure.
 */
static const char* const lec_le_arp_banner =
"Hardware Addr   ATM Address                                 VCD  Interface";

void lec_show_commands (parseinfo *csb)
{
    lec_info_t *lec_info;
    boolean brief;

    switch (csb->which) {
    case SHOW_LANE_LE_ARP:
	automore_enable(lec_le_arp_banner);
        if (GETOBJ(int, 2) & SHOW_LANE_NAME) {
            FOR_ALL_LEC(lec_info) {
                if (!memcmp(lec_info->actual_elan_name,
                            GETOBJ(string, 1),
                            lec_info->actual_name_size)) {
                    lec_show_one_le_arp(lec_info);
                }
            }
        } else if (GETOBJ(int, 2) & SHOW_LANE_INT) {
            FOR_ALL_LEC(lec_info) {
                if ((csb->subif_specified &&
                     (lec_info->swidb == GETOBJ(idb, 1))) ||
                    (!csb->subif_specified &&
                     (lec_info->swidb->hwptr == GETOBJ(idb, 1)->hwptr)))
                {
                    lec_show_one_le_arp(lec_info);
                }
            }
        } else {
            FOR_ALL_LEC(lec_info) {
                lec_show_one_le_arp(lec_info);
            }
        }
	automore_disable();
	break;

    case SHOW_LANE_LEC:

	automore_enable(NULL);
        brief = GETOBJ(int, 2) & SHOW_LANE_BRIEF;

        if (GETOBJ(int, 2) & SHOW_LANE_INT) {
            FOR_ALL_LEC(lec_info) {
                if ((csb->subif_specified &&
                     (lec_info->swidb == GETOBJ(idb, 1))) ||
                    (!csb->subif_specified &&
                     (lec_info->swidb->hwptr == GETOBJ(idb, 1)->hwptr)))
                {
                    lane_show_one_lec(lec_info->lane_info, brief);
                }
            }
        } else if (GETOBJ(int, 2) & SHOW_LANE_NAME) {
            FOR_ALL_LEC(lec_info) {
                if ((strlen(GETOBJ(string,1)) == lec_info->actual_name_size)
                    && !memcmp(lec_info->actual_elan_name,
                               GETOBJ(string, 1),
                               lec_info->actual_name_size)) {
                    lane_show_one_lec(lec_info->lane_info, brief);
                }
	    }
        } else {
            FOR_ALL_LEC(lec_info) {
                lane_show_one_lec(lec_info->lane_info, brief);
            }
        }

	automore_disable();
	break;

    default:
	bad_parser_subcommand(csb, csb->which);
	break;
    }
}


/*
 * lec_cfg_idle_time
 *
 * This function handles the config command:
 *
 * [no] lane lec-idle-time IDLE-TIME
 */
void lec_cfg_idle_time (parseinfo *csb)
{
    idbtype             *swidb;
    lane_info_t         *lane_info;
    lec_info_t          *lec_info;

    swidb = csb->interface;
    if (!swidb) {
	return;
    }

    lane_info = swidb->lane_info;

    if (csb->sense) { 
	/*
	 * Set the idle timer.  If there is no lane_info or lec_info
	 * struct, the call to lec_init_idb will create them.
	 */
	if (!lane_info || (lane_info && !lane_info->lec_info)){
	    lec_info = lec_init_idb(swidb);
	    
	    if (!lec_info)
		return;

	    lane_info = lec_info->lane_info;

	} else {
	    lec_info = lane_info->lec_info;
	}	
	lec_info->config_idle_time = GETOBJ(int, 1) * ONESEC;
	lec_info->idle_time = lec_info->config_idle_time;

    } else {
        if (lane_info) {
	    if (lane_info->lec_info) {
		lec_info = lane_info->lec_info;
	        lec_info->config_idle_time = 0;
		lec_info->idle_time = LEC_IDLE_TIMEOUT;
            }
        }
    }
}


/*
 * lec_exec_clear_join_cnt
 *
 * This exec level function deals with the
 *
 * 	"clear lane join-cnt"
 *
 * command.  It's used to clear the LEC join counter which counts the
 * number of times that the client has successfully joined to the ELAN.
 */
void lec_exec_clear_join_cnt (parseinfo *csb)
{
    lec_info_t		*lec_info = NULL;
    
    switch (csb->which) {

    case CLEAR_LANE_LEC_JOIN_CNT_INT:
	FOR_ALL_LEC(lec_info) {
	    if (lec_info->swidb == GETOBJ(idb, 1)) {
		lec_info->join_attempt_cnt = 0;
		return;
	    }
	}
	printf("\n%%no LAN Emulation client on interface %s",
	       GETOBJ(idb, 1)->namestring);
	break;

    case CLEAR_LANE_LEC_JOIN_CNT_ALL:
	FOR_ALL_LEC(lec_info) {
	    lec_info->join_attempt_cnt = 0;
	}
	break;

    default:
	bad_parser_subcommand(csb, csb->which);
	break;

    }
}

/* 
 * lec_gen_idle_time
 *
 * This function generates the output for nvgen:
 * 
 *	"lane client idle-time IDLE-TIME"
 */
void lec_gen_idle_time(parseinfo *csb)
{
    lane_info_t	*lane_info;
    lec_info_t 	*lec_info;

    lec_info  = lec_info_from_csb(csb);
    if (!lec_info) {
	return;
    }

    if (lec_info->config_idle_time) {
	lane_info = lec_info->lane_info;
	nv_write(TRUE, "%s", csb->nv_command);
	nv_add(TRUE, " %d", (lec_info->config_idle_time / ONESEC));
    }
}


