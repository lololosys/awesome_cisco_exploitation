/* $Id: lane.c,v 3.3.42.9 1996/08/28 12:54:33 thille Exp $
 * $Source: /release/112/cvs/Xsys/lane/lane.c,v $
 *------------------------------------------------------------------
 * lane.c -- LAN Emulation registration, inits, subsys declaration
 *
 * October, 1994  Brad Benson
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: lane.c,v $
 * Revision 3.3.42.9  1996/08/28  12:54:33  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.3.42.8  1996/07/30  20:24:02  bbenson
 * CSCdi64226:  Support larger max_frame_size (>1516) for LANE
 * Branch: California_branch
 * Additional reg_add calls to remove direct calls out of network.c
 *
 * Revision 3.3.42.7  1996/06/25  18:42:01  rbadri
 * CSCdi61089:  c4500 crash upon shutdown of atm0 in LANE BATs
 * Branch: California_branch
 * When a "no int a0.3" is done, make sure to disable the client and
 * release all lsv/BUS info.
 *
 * Revision 3.3.42.6  1996/06/04  21:48:23  ddecapit
 * aCVS:
 * ----------------------------------------------------------------------
 *
 * CSCdi59496:  Code Review Comments for Token Ring LANE
 * Branch: California_branch
 *
 * Revision 3.3.42.5  1996/05/30  21:32:24  rbadri
 * CSCdi59101:  LE-ARP entry should use 6 byte MAC address instead of
 * hwaddrtype
 * Branch: California_branch
 * Changed the le_arp structure and all the associated functions to use
 * 6 byte MAC address instead of using 24 byte hwaddrtype to store 6
 * bytes.
 *
 * Revision 3.3.42.4  1996/05/25  22:23:42  cakyol
 * CSCdi58700:  start the lecs finder ONLY when a relevant request comes
 * in.
 * Branch: California_branch
 *
 * Revision 3.3.42.3  1996/05/17  23:26:33  rbadri
 * CSCdi56574:  LANE clients go down/up during 500 station multiping
 * Branch: California_branch
 * Flush requests are to be sent in the context of the client process
 * rather than signalling process. Throttle the number of requests to be
 * sent and be generous about giving up the CPU.
 *
 * Revision 3.3.42.2  1996/05/09  14:32:10  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.3.42.1.18.1  1996/04/27  07:06:05  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.3.42.1.8.11  1996/04/25  22:12:31  cakyol
 * - rename lecsFinderModuleInit to lecs_finder_init, similar
 *   to all the other LANE module init routines.
 * - call it from the appropriate place (lane.c: lane_init).
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 3.3.42.1.8.10  1996/04/17  23:14:30  rbadri
 * Branch: LE_Cal_V112_0_2_branch
 * Convert TokenRing LANE related functions to registries
 * Move contents of lec_srb* srt/srb_lane.[ch], to facilitate
 * creation of new sub-system.
 *
 * Revision 3.3.42.1.8.9  1996/04/16  02:44:04  cakyol
 * goodbye to PVC support for LANE.
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 3.3.42.1.8.8  1996/04/15  19:03:21  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Try doing something with TRLANE after bcopying the mac addresses before
 * doing the lec_find_vc call.
 *
 * Revision 3.3.42.1.8.7  1996/04/15  07:26:52  shj
 * CSCdi52538:  IP multicast packets do not fastswitch to LANE
 * Branch: LE_Cal_V112_0_2_branch
 * Allow BUS path to be added to fast cache if destination is multicast.
 * Suppress fast cache only if BUS path will be replaced later with direct
 * path.
 *
 * Revision 3.3.42.1.8.6  1996/04/13  23:13:00  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * - Don't call tr_lane_start, llc2_start or start_srb_background unless
 *   tr-lane is configured.
 * - First shot at calling lec_set_maxmtu to set the tr-lane MTU.
 * - Remove lane_type from lec_muex_config.  It's not needed.
 * - Only do the tr-lane config of srb when tr-lane is configured.
 *
 * Revision 3.3.42.1.8.5  1996/04/09  07:18:20  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Incorporate code review comments regarding if/else reign of terror.
 *
 * Revision 3.3.42.1.8.4  1996/04/09  02:15:52  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Easter bunny had a field day last weekend through the code.  Non-cisco
 * coding styles squashed.  Dangling else's now attached to }'s, else's
 * now attached to if's, 10 character tabs removed...
 *
 * Revision 3.3.42.1.8.3  1996/04/08  20:46:57  cakyol
 * sync corrections:
 * - value "LANE_LECS_BIND_NSAP" should not be there.
 * - parser user errors should emit a "%" as the first character.
 * - white space cleanup.
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 3.3.42.1.8.2  1996/04/08  14:51:28  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 3.3.42.1.8.1  1996/04/08  01:56:31  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.3.42.1  1996/03/18  20:32:14  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.46.6  1996/04/06  02:52:14  rbadri
 * Branch: LE_Syn_Cal_V111_1_3_branch
 * In LE-ARP requests, fill in the source MAC of the originator and not
 * the MAC of client.
 *
 * Revision 3.3.46.5  1996/04/05  21:28:52  cakyol
 * Combine all the special atm printing routines into a
 * single one (atm_related_printf), saving code space.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.3.46.4  1996/04/05  03:20:13  cakyol
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
 * Revision 3.3.46.3  1996/04/02  10:09:39  cakyol
 * - perform re-verifications ONLY on completely operational clients.
 * - tidy up initializations, they were scattered all over the place.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.3.46.2  1996/03/27  00:36:51  cakyol
 * Use only %h and %j for new printf format characters.
 * I was also using the uppercase versions as well.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.3.46.1  1996/03/22  22:53:48  rlowe
 * Non-sync of Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.3.32.1  1996/03/05  06:42:33  rlowe
 * Apply LANE content to merge branch.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 3.3.22.1  1996/02/27  21:09:54  cakyol
 * non sync sync of LE_Cal_V111_0_16_branch to V111_1_0_3 yielding
 * LE_Cal_V111_1_0_3_branch
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 3.3.2.5  1996/02/22  09:05:25  cakyol
 * the new LECS config command syntax enabled provisionally
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 3.3.2.4  1996/02/13  00:17:49  rbadri
 * Branch: LE_Cal_V111_0_16_branch
 * DECnet routing for LANE
 *
 * Revision 3.3.2.3  1996/02/06  22:19:10  cakyol
 * cleanup for the last code review:
 * - get rid of "#ifdef PARANOID...#endif".  Leave some in
 *   permanently, take some out completely.
 * - get rid of all "if 0....endif" type constructs.
 * - debug messages speeded up by using variable argument
 *   macro expansion.
 * - most fatal type errors are converted to crashdump with
 *   __FILE__ and __LINE__ printing.
 * - in the LECS, do *NOT* update the table of global LECS
 *   addresses, if any outstanding signalling requests remain.
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 3.3.2.2  1996/01/26  02:02:25  cakyol
 * more code review mods (still more to come):
 * Revision 3.3.2.1.2.3  1996/03/21  03:25:56  ddecapit
 * Branch: TRlane_branch
 * RSP Support
 * Code Review Comments
 * Revision 3.3.2.1.2.2  1996/03/06  18:52:44  ddecapit
 * Branch: TRlane_branch
 *
 * TR LANE debug
 * Remove tr_lane_explorer
 *
 * Revision 3.3.2.1.2.1  1996/02/26  23:05:41  ddecapit
 * Branch: TRlane_branch
 *
 * Revision 3.3.2.1  1996/01/11  01:46:01  cakyol
 * non sync sync of LE_Calif_branch to V111_0_16 yielding
 *     LE_Cal_V111_0_16_branch
 *
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 3.3  1996/01/05  04:56:04  hampton
 * Remove extraneous includes of h/timer.h.  [CSCdi46482]
 * this is only a test
 *
 * Revision 3.2.10.1  1995/12/29  01:06:25  cakyol
 * Initial commits for:
 *
 * LANE Fault Tolerant LECS Servers
 * LANE Fault Tolerant LES Servers
 * HSRP over LANE Client
 *
 * Branch: LE_Calif_branch
 *
 * Revision 3.2  1995/11/17  17:50:02  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:22:38  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.7  1995/11/08  21:12:16  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.6  1995/08/06  21:59:42  bbenson
 * CSCdi36725:  LEC needs to obtain LECS address from the switch.
 * - Locally implement LECS address determination (not using cfgacc).
 * - Use interface_admin_down instead of interface_up_simple.
 *
 * Revision 2.5  1995/06/30  18:34:45  bbenson
 * CSCdi36642:  AppleTalk fastcache getting populated with phase1 LANE
 * - Set PAK_ENCAPS_ONLY for packets that shouldn't get cached (those
 *   unicast that are being sent over the mcast_send).
 * Revision 2.4  1995/06/28  09:28:27  smackie
 * Repair widespread subsystem header braindamage. (CSCdi23568)
 *
 *   o Fix subsystem entry points to be foo(subsystype *) not foo(void)
 *   o Repair nonsensical and redundant subsystem sequences
 *   o Use NULL where no property is required
 *
 * Revision 2.3  1995/06/23  18:48:03  ahh
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase III -- remove unneeded NOMEMORY message externs.
 *
 * Revision 2.2  1995/06/19  06:45:47  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.1  1995/06/07  21:18:47  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "mgd_timers.h"
#include "interface_private.h"
#include "packet.h"
#include "address.h"
#include "name.h"
#include "config.h"
#include "ttysrv.h"
#include "logger.h"
#include "../lane/msg_lane.c"		/* Not a typo, see logger.h */
#include "../ui/debug.h"
#include "sys_registry.h"
#include "fastswitch_registry.h"
#include "subsys.h"
#include "../if/ether.h"
#include "../if/tring.h"
#include "../if/atm.h"
#include "../atm/ilmi_api.h"
#include "../atm/atmsig_api.h"
#include "parser_defs_lane.h"

#include "../srt/rsrb.h"        /* for Token Ring LAN Emulation */
#include "../srt/srb.h"
#include "../llc2/llc2.h"

#include "lane.h"
#include "lane_private.h"
#include "lane_signal.h"
#include "lec_private.h"
#include "lane_debug.h"
#include "lane_registry.h"
#include "lane_ilmi.h"
#include "../decnet/decnet_externs.h"

/* all digits of an address is significant mask */
uchar ALL_ONES_MASK [] =
{
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
};

queuetype lane_infoQ;		/* queue of lane_info's */


/*
 * lane_control_encap
 *
 * Exported to LANE components (LEC, LES/BUS, and LECS) to encapsulate 
 * lane packets with vcd/enctype.  Note that the internal function 
 * used by atm_cpcs_encap is atm_cpcs_lane.
 */
boolean lane_control_encap(paktype *pak, vcd_t vcd)
{
    idbtype 	*idb;

    idb = pak->if_output;
    if (idb && atm_interface_up(idb)) {
	hwidbtype	*hwout;
	vc_info_t	*vc_info;
	atm_db_t	*atm;

	hwout = idb->hwptr;
	atm = hwout->atm_db;
	vc_info = atm_getvc(atm, vcd);
	if (!vc_info) {
	    return(FALSE);
	}

	if ((vc_info->flags & VC_FLAG_AAL_MASK) != ATM_VC_AAL5LANE) {
	    return(FALSE);
	}

	(*atm->funcs.encapsulate)(atm, pak, vc_info, 0);

	atm_show_packet(hwout, pak, TRUE);
	
	atm_out_inc(atm, vc_info);

	return(TRUE);
    }

    return(FALSE);

}

/*
 * lane_data_vencap
 * 
 * This is called through a registry from within atm_vencap when it
 * determines that the outgoing packet has to have LANE encaps put down.
 */
static boolean lane_data_vencap (paktype *pak, long dhost) 
{
    idbtype	*swidb;
    hwidbtype	*hwout;
    atm_db_t	*atm_db;
    vcd_t	vcd;
    vc_info_t	*vc_info;
    uchar const *snap_info = zerosnap;
    hwaddrtype 	*hwaddr = NULL; 
    uchar       lane_mac[STATIONLEN_IEEE48], src_mac[STATIONLEN_IEEE48];
    boolean     cache_ok;

    lec_info_t 	*lec_info;      /* for Token Ring LAN Emulation */ 
    trrif_t	*trh;		/* pointer to TR MAC header */
    int   	riflen = 0;	/* length of RIF in TR header */
    sap_hdr	*llc1;		/* pointer to llc-1 header */
    uchar       *frameptr;
    boolean     is_response;
    uchar       conch;

    swidb = pak->if_output;
    hwout = swidb->hwptr;
    atm_db = hwout->atm_db;

    lec_info = lec_info_from_swidb(swidb);    

    switch (lec_info->actual_lan_type) {
    case LANE_TYPE_802_3:
	if ((pak->linktype != LINK_BRIDGE) && (!ether_vencap(pak, dhost))) {
	    return(FALSE);
	}
	bcopy(pak->datagramstart, lane_mac, STATIONLEN_IEEE48);
	bcopy(pak->datagramstart+IEEEBYTES, src_mac, STATIONLEN_IEEE48);
	break;
    case LANE_TYPE_802_5:
	trh = (trrif_t *) pak->datagramstart;
	riflen = (trh->saddr[0] & TR_RII) ? trh->rc_blth & RC_LTHMASK : 0;
	pak->llc2_sapoffset = TRING_ENCAPBYTES + riflen;
	frameptr    = pak->datagramstart + pak->llc2_sapoffset;
	llc1 = (sap_hdr *)(pak->datagramstart + TRING_ENCAPBYTES + riflen);
	is_response = (llc1->ssap & SAP_RESPONSE);
	conch = llc1->control & ~P_BIT;     /* snuff out p/f bit */
	if (conch != LLC_TEST) {
	    if ((conch == XID) || 
		(pak->enctype == ET_SAP) || 
		(pak->enctype == ET_LLC2) || 
		(pak->enctype == ET_UNSUPPORTED)) {
		;
	    } else if (!reg_invoke_lane_tr_vencap(pak, dhost)){
		return(FALSE);
	    }
	}
	bcopy((pak->datagramstart + TR_DA_OFFSET), lane_mac, STATIONLEN_IEEE48);
	bcopy(pak->datagramstart + IEEEBYTES + TR_DA_OFFSET, src_mac, STATIONLEN_IEEE48);
	break;
    default:
	return(FALSE);
    }

    /*
     * Use the resulting dest pak->MAC address to find the VCD.  The
     * src MAC is loaded into any LE_ARPs that get generated.
     */
    if (!lec_find_vc(swidb->lane_info, lane_mac, src_mac, &vcd, &cache_ok)) {
        return(FALSE);
    }

    vc_info = atm_getvc(atm_db, vcd);
    if (!vc_info) {
        return(FALSE);
    }

    atm_cpcs_encap(pak, atm_db, vc_info, snap_info, hwaddr, 0);

    atm_show_packet(hwout, pak, TRUE);


    /*
     * Don't populate any of the fs caches with flooded encaps.
     * Indicate to those protocols that don't use the registry that this
     * is a packet that shouldn't be cached by setting the
     * PAK_ENCAPS_ONLY flag.
     */

    if (cache_ok && (!(pak->flags & PAK_ENCAPS_ONLY)) &&
        (lec_info->actual_lan_type != LANE_TYPE_802_5)) {
	reg_invoke_fs_cache_update(pak->linktype, pak, 0L);
    } else {
	pak->flags |= PAK_ENCAPS_ONLY;
    }

    atm_counters[ATM_OUTPUT]++;
    vc_info->outpkts++;
    atm_db->outpkts++;

    return(TRUE);
}

/*
** [sub] interface up/down registry handler
*/
static void lane_interface_updown (idbtype *swidb, hwidbtype *hwidb)
{
    lecs_interface_updown(swidb, hwidb);
    lesbus_interface_updown(swidb, hwidb);
    lec_interface_updown(swidb, hwidb);
}

/*
** interface mac address change registry handler
*/
static void lane_mac_addr_change (hwidbtype *hwidb)
{
    if (lane_debug_any()) {
	buginf("\nLANE %s: possible mac address change", 
	    hwidb->hw_namestring);
    }
    lecs_addr_recycle(hwidb, MAC_ADDRESS_CHANGE_EVENT);
    lesbus_addr_recycle(hwidb, MAC_ADDRESS_CHANGE_EVENT);
    lec_addr_recycle(hwidb, MAC_ADDRESS_CHANGE_EVENT);
}


/*
** interface ILMI prefix event registry handler
*/
static void lane_prefix_change (void *arg)
{
    ilmiClientReq *ilmiPrefixEvent = (ilmiClientReq*) arg;
    uchar prefix [PREFIX_SIZE];
    int i;
    hwidbtype *hwidb;
    lane_addr_recycle_reason reason;

    hwidb = hwidb_or_null_from_index(ilmiPrefixEvent->intf_id);
    if (!hwidb) return;
    if (!is_atm(hwidb)) return;
    reason = (ilmiPrefixEvent->notify_type == ilmiAddressAdd) ?
		PREFIX_ADD_EVENT : PREFIX_DELETE_EVENT;

    if (lane_debug_any()) {
	buginf("\nLANE %s: %s event for ", 
	    hwidb->hw_namestring,
	    lane_pr_addr_recycle_reason(reason));

	for (i=0; i<ilmiPrefixEvent->address->length; i++) 
	    buginf("%02x", ilmiPrefixEvent->address->octet_ptr[i]);

	buginf(" ptr=0x%x len=%d", 
	    ilmiPrefixEvent->address->octet_ptr,
	    ilmiPrefixEvent->address->length);

	buginf("\n    the current first prefix is now: ");
	if (getIlmiPrefix(hwidb, prefix)) {
	    for (i=0; i<PREFIX_SIZE; i++) 
		buginf("%02x", prefix [i]);
	} else {
	    buginf("NONE");
	}
    }

    /* notify all lane components */
    lecs_addr_recycle(hwidb, reason);
    lesbus_addr_recycle(hwidb, reason);
    lec_addr_recycle(hwidb, reason);
}

/*
 * lane_register
 */
static void lane_register (void)
{
    int ilmiClientId;
    ilmiClientReq req;

    /* some useful printf's */
    reg_add_printf('j', atm_related_printf, "atm_raw_bytes");
    reg_add_printf('h', atm_related_printf, "atm_hwaddrtype");
    reg_add_printf('w', atm_related_printf, "atm_maskedaddr");

    /* correct data encapsulation */
    reg_add_lane_data_vencap(lane_data_vencap, "lane_data_vencap");

    /* subinterface deletion */
    reg_add_swif_erase(lane_swif_erase, "lane_swif_erase");

    /* MTU handling stuff */
    reg_add_is_valid_lane_mtu(is_valid_lane_mtu, "is_valid_lane_mtu");
    reg_add_lane_default_mtu(lane_default_mtu, "lane_default_mtu");

    /* register with interface up/down events */
    reg_add_if_statechange_complete(lane_interface_updown, 
	"lane_interface_updown");

    /* mac address changes */
    reg_add_mac_address_change(lane_mac_addr_change, 
	"lane_mac_addr_change");

    /* ILMI prefix notifications */
    ilmiClientId = ilmi_register_client("LANE PREFIX EVENTS");
    if (ilmiClientId != ILMI_ERROR) {
	req.client_id = ilmiClientId;
	req.notifyCB = lane_prefix_change;
	req.notify_type = ilmiAddressAdd;
	ilmi_register_service(&req);
	req.notify_type = ilmiAddressDel;
	ilmi_register_service(&req);
    }
}


void set_default_mac_plan (lane_info_t *lane_info)
{
    idbtype *swidb;

    if (lane_info == NULL)
	return;

    swidb = lane_info->swidb;
    lane_info->mac_plan_type = LANE_MACPLAN_SAME;
    ieee_copy(swidb->hwptr->hardware, lane_info->mac_plan_base);
}


/*
 * lane_init_idb
 *
 * Initializes an interface for LAN emulation.
 */
lane_info_t *lane_init_idb (idbtype *swidb)
{
    lane_info_t	*lane_info;
    idbtype *firstsw = NULL;

    if (! swidb) return NULL;

    lane_info = swidb->lane_info;
    if (lane_info) {
	/* can't create a new one 'til old is gone */
	if (lane_info->dead) {
	    errmsg(&msgsym(NOINT, LANE), swidb->namestring);
	    return NULL;
	}
	return lane_info;
    }

    lane_info = malloc_named(sizeof(lane_info_t), "lane_info_t");
    if (! lane_info) return NULL;

    /* enter lane_info into global list */
    bzero(lane_info, sizeof(lane_info_t));
    enqueue(&lane_infoQ, lane_info);

    /* set the cross pointers */
    swidb->lane_info = lane_info;
    lane_info->swidb = swidb;

    /* initialize other fields */
    lane_info->lecs_nsap_type = LANE_LECS_BIND_ADDR;

    lane_info->mac_plan_type = LANE_MACPLAN_SAME;
    lane_info->type = LANE_TYPE_UNSPECIFIED;
    lane_info->max_data_frame = LANE_MAX_FRAME_1516;

    /* set the default mac plan */
    set_default_mac_plan(lane_info);

    /* there is NO lecs address as of yet so set it to fixed */
    setToFixedLecsAddress(&lane_info->masterLecsAddress);

    /*
    ** Do this LAST, after everything is initialized.
    **
    ** if LANE gets configured on ANY subif, we MUST
    ** initialize everything about the LECS Finder.
    */
    firstsw = firstsw_or_null(swidb->hwptr);
    if (firstsw == swidb) {
	/* we are on the main interface, init data structures */
	if (! prepareLecsFinderModule(lane_info)) {
	    free(lane_info);
	    return NULL;
	}
    } else {
	/* create the main subif lane info by force */
	lane_init_idb(firstsw);
    }

    return lane_info;
}


/*
 * lane_init()
 *
 */
void lane_init (subsystype *subsys)
{
   queue_init(&lane_infoQ, 0);
   lane_register();
   lane_parser_init();
   lane_debug_init();

   /* maintain the correct ordering below */
   lecs_finder_init();
   lecs_init();
   lsv_init();
   lec_init();
}


/*
 * lane_info_destroy - Destroy lane_info_t if and only if it is no
 *			longer in use.  See also comments in lane_swif_erase.
 */
void
lane_info_destroy (lane_info_t	*lane_info)
{
    if (lane_info->dead			 &&
	lane_info->lec_info     == NULL  &&
	lane_info->lsv_info     == NULL  &&
	lane_info->lecs_info    == NULL  &&
	lane_info->signal_chain == NULL  &&
	lane_info->lecsFinderContext == NULL) {
	/* Now delete the LANE structure, itself
	 */
	if (lane_debug_any())
	    buginf("\nLANE %s deleting all LANE structures",
		   lane_info->swidb->namestring);
	p_unqueue(&lane_infoQ, lane_info);
	lane_info->swidb->lane_info = NULL;
	free(lane_info);
    }
}


/*
 * lane_swif_erase
 *
 * Sub-interface has been killed.  Tell everybody to start killing
 *		themselves.  They'll call lane_info_destroy when done.
 *
 *
 * lane_swif_erase first calls all the LANE swif_erase routines.
 * These routines are the "start doing whatever you need to do when
 * you find out that the swidb is going away" routines.
 *
 * The last thing that lane_swif_erase does is to set the flag:
 * lane_info->dead = TRUE and call lane_info_destroy(lane_info).
 * lane_info_destroy simple checks that 1) the dead flag is set,
 * and 2) that all the above-named pointers are NULL.  If either
 * condition is not true, then it does nothing.  If all conditions
 * are true, it p_unqueues the lane_info_t and free()s it.
 *
 * If your xxx_swif_erase routine can do all its stuff and NULL
 * its corresponding pionter in lane_info without pending, then
 * just do it, and you're done.  For example, if your pointer(s)
 * in lane_info are already NULL, you're done.  Just return.
 *
 * If your xxx_swif_erase routine returns before it has finished
 * and NULLed its pointer(s), that's OK.  It just means that, in
 * whatever process finally finishes the clean-up, you NULL your
 * pointer(s) in lane_info and call lane_info_destroy.
 *
 * It's OK if you call lane_info_destroy before you're all done
 * and the pointer(s) are NULLed.  That's what the lane_info->dead
 * flag is for.  Just be sure you call lane_info_destroy again, when
 * you're all done.
 *
 * Note that lane_init_idb checks the lane_info->dead flag, and
 * returns NULL if it's set, just like when memory is exhausted.
 * That prevents re-use of a lane_info before it's been deleted.
 */
void
lane_swif_erase (idbtype *swidb)
{
    lane_info_t	*lane_info;

    /* LANE in use?
     */
    lane_info = swidb->lane_info;
    if (lane_info == NULL)
	return;
    /*
     * ignore erasure if structure is already being deleted.
     */
    if (lane_info->dead)
	return;
    /*
     * Initialize the swidb back to TBR_UNKNOWN,
     * so that bridging code dosen't get confused 
     * (look in srt/span1.c)
     */
    swidb->tbridge_media = TBR_UNKNOWN;
    /* notify all LANE components
     */
    lec_swif_erase(lane_info);
    lsv_swif_erase(lane_info);
    lecs_swif_erase(lane_info);
    /*
     * Make first attempt to delete the LANE structure, itself
     */
    lane_info->dead = TRUE;
    lane_info_destroy(lane_info);

}
/*
 * LANE subsystem header
 */

#define LANE_MAJVERSION      1
#define LANE_MINVERSION      0
#define LANE_EDITVERSION     0

/*
 * Sequenced and required initialization.
 */
SUBSYS_HEADER(lane, LANE_MAJVERSION, LANE_MINVERSION, LANE_EDITVERSION,
              lane_init, SUBSYS_CLASS_PROTOCOL,
              "seq: atmsig, sscop", 
	      "req: atm, atmsig, sscop, atmilmi");

