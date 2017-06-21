/* $Id: ether.c,v 3.12.10.10 1996/08/28 12:47:56 thille Exp $
 * $Source: /release/112/cvs/Xsys/if/ether.c,v $
 *------------------------------------------------------------------
 * ether.c -- 10MB Ethernet support routines
 *
 * 16-July-1986, Kirk Lougheed
 *
 * Copyright (c) 1986-1996, 1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ether.c,v $
 * Revision 3.12.10.10  1996/08/28  12:47:56  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.12.10.9  1996/08/27  23:31:42  hou
 * CSCdi66323:  Routers ISL encapsultaion on CGMP messages is incorrect
 * Branch: California_branch
 * - Clean up the ISL vLAN code so that we can do ISL encapsulation on
 *   different type packet not only ARPA type.
 *
 * Revision 3.12.10.8  1996/08/13  02:18:37  gstovall
 * CSCdi39484:  Giant packets output on ethernet interface
 * Branch: California_branch
 * Make sure we drop giants, even if they are just a wee bit too big.
 *
 * Revision 3.12.10.7  1996/06/27  09:33:29  fox
 * CSCdi61238:  linktype is an overloaded symbol
 * Branch: California_branch
 * Change idb->linktype to idb->getlink.
 *
 * Revision 3.12.10.6  1996/05/10  01:38:18  wmay
 * CPP and ppp half bridge commit
 * Branch: California_branch
 *
 * Revision 3.12.10.5  1996/05/09  14:29:47  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.12.10.4.4.1  1996/04/27  07:03:37  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.12.10.4  1996/04/22  18:12:30  gstovall
 * CSCdi53071:  media idb init routines will fail on second pass
 * Branch: California_branch
 * Allow media idb init routines to be called again.  Also clean up the
 * handling of the media idb queues.
 *
 * Revision 3.12.10.3  1996/04/03  23:19:20  dino
 * CSCdi51598:  Problem of interoperability with switches
 * Branch: California_branch
 *
 * Revision 3.12.10.2.6.1  1996/04/08  01:54:27  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.12.10.2  1996/03/21  22:48:39  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.12.10.1  1996/03/18  20:13:27  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.12.14.2  1996/03/28  17:25:17  rbadri
 * Branch: LE_Syn_Cal_V111_1_3_branch
 * MTU support for sub-interfaces (used by ATM interfaces)
 *
 * Revision 3.12.14.1  1996/03/22  09:37:09  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.12  1996/02/25  03:51:52  mmcneali
 * CSCdi49686:  IPX non-functional in gs7 images on ISL encapsulating
 *              subinterfaces.
 *
 * Revision 3.11  1996/02/13  08:12:49  dstine
 * CSCdi48797:  Extraneous definitions in interface_private.h
 *
 * Revision 3.10.8.1  1996/03/05  06:13:43  rlowe
 * Apply LANE content to merge branch.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 3.10.4.1  1996/02/27  21:08:13  cakyol
 * non sync sync of LE_Cal_V111_0_16_branch to V111_1_0_3 yielding
 * LE_Cal_V111_1_0_3_branch
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 3.10  1996/01/30  17:29:45  bcole
 * CSCdi47620:  IP ARP does not support full SMDS addresses
 *
 * Fix ARP code to handle 8 byte SMDS addresses in SOME cases.
 * (Code still assumes 6 byte layer 2 addresses in other less critical
 * cases.)
 *
 * Revision 3.9  1996/01/25  23:11:15  gstovall
 * CSCdi47390:  down-when-looped command not working (regression)
 * Remember to reset the itemlist before tossing it up to the next level.
 *
 * Revision 3.8  1996/01/23  08:48:50  mmcneali
 * CSCdi47323:  vLAN encapsulated IP SSE switched.
 *
 * Faster Baby, Faster!
 *
 * Revision 3.7.12.1  1996/01/24  22:18:01  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.7.2.2  1996/02/13  00:18:31  rbadri
 * Branch: LE_Cal_V111_0_16_branch
 * DECnet routing for LANE
 *
 * Revision 3.7.2.1  1996/01/11  01:45:21  cakyol
 * non sync sync of LE_Calif_branch to V111_0_16 yielding
 *     LE_Cal_V111_0_16_branch
 *
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 3.7  1995/12/19  23:43:53  gstovall
 * CSCdi45884:  Output collisions miscounted
 * Make sure to use the right fields.
 *
 * Revision 3.6  1995/12/19  00:40:32  hampton
 * Obsolete the GET_NONZERO_TIMESTAMP routine.  It is no longer necessary
 * with 64bit timestamps.  [CSCdi45208]
 *
 * Revision 3.5  1995/12/17  06:03:32  mmcneali
 * CSCdi43825:  No fastswitching for 802.10 vLAN packets.
 *
 * Revision 3.4  1995/12/06  18:29:40  gstovall
 * CSCdi44769:  Interface idb queues are not getting populated correctly
 * Make sure the hwidbs are in the same order on the media hwidbQs as on
 * the hwidbQ.
 *
 * Revision 3.3.10.1  1995/12/23  03:59:19  dcheng
 * Branch: LE_Calif_branch
 * Added support for VTP.
 *
 * Revision 3.3  1995/11/17  09:26:49  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:37:30  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:48:47  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/11/08  21:02:47  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.3  1995/09/09  00:52:19  ronnie
 * CSCdi35837:  ping works, but traceroute fails to go across ATM interface
 * Introduce a new variable type, fuzzy, for hwidb->broadcast to return
 * instead of boolean.  This allows the return of NOT_SURE.  Adjust all
 * hwidb->broadcast routines to return fuzzy instead of boolean.
 *
 * Revision 2.2  1995/08/25  03:30:37  speakman
 * CSCdi38884:  IP ARP over token/serial broken
 * SNAP over HDLC, who woulda thunk it.  Do the GETSHORT thing.
 *
 * Revision 2.1  1995/06/07  20:51:31  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <string.h>
#include <ctype.h>
#undef   toupper			/* use library function call */
#include <ciscolib.h>
#include "../../boot/cpu.h"
#include "sys_registry.h"
#include "media_registry.h"
#include "fastswitch_registry.h"
#include "subsys.h"
#include "interface_private.h"
#include "packet.h"
#include "logger.h"
#include "address.h"
#include "../ui/debug.h"
#include "ether.h"
#include "ether_private.h"
#include "ether_debug.h"
#include "network.h"
#include "arp.h"
#include "../hes/if_mci.h"
#include "../tbridge/tbridge_sde.h"
#include "../if/atm.h"
#include "../lane/lane.h"
#include "../if/msg_datalink.c"		/* Not a typo, see logger.h */
#include "../util/itemlist.h"
#include "../vlan/vlan.h"
#include "../parser/parser_defs_parser.h"

static char *hextet(char *, ushort *, char); /* forward declarations */
char *get_hex_num(char *, ushort *);

static boolean ether_hw_set_config (hwidbtype *hwidb, itemlist *ilist,
				    ethersb *esb)
{
    itemdesc	*item;
    boolean	any_unhandled = FALSE;	/* assume we've handled all items */
    tinybool	this_item_unhandled;

    while ((item = itemlist_getnext(ilist))) {
	if (item->handled)
	    continue;
	this_item_unhandled = FALSE;
	switch (item->item) {
	case ETHER_HW_MEDIA_TYPE:
	    esb->media_type = item->u.l_value;
	    break;
	case ETHER_HW_FEIP_FULLDUPLEX:
	    esb->feip_fullduplex = item->u.l_value;
	    break;
	default:
	    this_item_unhandled = TRUE;
	    any_unhandled = TRUE;
	    break;
	}
	if (!this_item_unhandled)
	    item->handled = TRUE;
    }
    return (!any_unhandled);
}

static boolean ether_hw_read_config (hwidbtype *hwidb, itemlist *ilist,
				     ethersb *esb)
{
    itemdesc	*item;
    boolean	any_unhandled = FALSE;	/* assume we've handled all items */
    tinybool	this_item_unhandled;

    while ((item = itemlist_getnext(ilist))) {
	if (item->handled)
	    continue;
	this_item_unhandled = FALSE;
	switch (item->item) {
	case ETHER_HW_MEDIA_TYPE:
	    item->u.l_value = esb->media_type;
	    item->length = sizeof(ulong);
	    break;
	case ETHER_HW_FEIP_FULLDUPLEX:
	    item->u.l_value = esb->feip_fullduplex;
	    item->length = sizeof(ulong);
	    break;
	default:
	    this_item_unhandled = TRUE;
	    any_unhandled = TRUE;
	    break;
	}
	if (!this_item_unhandled)
	    item->handled = TRUE;
    }
    return (!any_unhandled);
}

static boolean ether_hw_set_state (hwidbtype *hwidb, itemlist *ilist,
				   ethersb *esb)
{
    itemdesc	*item;
    boolean	any_unhandled = FALSE;	/* assume we've handled all items */
    tinybool	this_item_unhandled;

    while ((item = itemlist_getnext(ilist))) {
	if (item->handled)
	    continue;
	this_item_unhandled = FALSE;
	switch (item->item) {
	case ETHER_HW_OUTPUT_ONE_COLLISION:
	    esb->output_one_collision = item->u.l_value;
	    break;

	case ETHER_HW_OUTPUT_MORE_COLLISION:
	    esb->output_more_collision = item->u.l_value;
	    break;

	case ETHER_HW_OUTPUT_LATE_COLLISION:
	    esb->output_late_collision = item->u.l_value;
	    break;

	case ETHER_HW_OUTPUT_EXCESSIVE_COLLISION:
	    esb->output_excessive_collision = item->u.l_value;
	    break;

	case ETHER_HW_OUTPUT_DEFERRED:
	    esb->output_deferred = item->u.l_value;
	    break;

	case ETHER_HW_OUTPUT_LOST_CARRIER:
	    esb->output_lost_carrier = item->u.l_value;
	    break;

	case ETHER_HW_OUTPUT_BABBLES:
	    esb->output_babbles = item->u.l_value;
	    break;

	case ETHER_HW_SI_OUTPUT_ONE_COLLISION:
	    esb->si_output_one_collision = item->u.l_value;
	    break;

	case ETHER_HW_SI_OUTPUT_MORE_COLLISION:
	    esb->si_output_more_collision = item->u.l_value;
	    break;

	case ETHER_HW_SI_OUTPUT_LATE_COLLISION:
	    esb->si_output_late_collision = item->u.l_value;
	    break;

	case ETHER_HW_SI_OUTPUT_EXCESSIVE_COLLISION:
	    esb->si_output_excessive_collision = item->u.l_value;
	    break;

	case ETHER_HW_SI_OUTPUT_DEFERRED:
	    esb->si_output_deferred = item->u.l_value;
	    break;

	case ETHER_HW_SI_OUTPUT_LOST_CARRIER:
	    esb->si_output_lost_carrier = item->u.l_value;
	    break;

	case ETHER_HW_SI_OUTPUT_BABBLES:
	    esb->si_output_babbles = item->u.l_value;
	    break;

	case ETHER_HW_FEIP_NO_CARRIER:
	    esb->feip_no_carrier = item->u.l_value;
	    break;

	case ETHER_HW_FEIP_MULTICAST:
	    esb->feip_multicast = item->u.l_value;
	    break;

	case ETHER_HW_FEIP_WATCHDOG:
	    esb->feip_watchdog = item->u.l_value;
	    break;

	case ETHER_HW_CLEAR_STATS:
	    esb->si_output_one_collision = esb->output_one_collision;
	    esb->si_output_more_collision = esb->output_more_collision;
	    esb->si_output_late_collision = esb->output_late_collision;
	    esb->si_output_excessive_collision =
		esb->output_excessive_collision;
	    esb->si_output_deferred = esb->output_deferred;
	    esb->si_output_lost_carrier = esb->output_lost_carrier;
	    esb->si_output_babbles = esb->output_babbles;
	    
	    /*
	     * Fast Ethernet fields
	     */
	    esb->si_feip_no_carrier = esb->feip_no_carrier;
	    esb->si_feip_multicast = esb->feip_multicast;
	    esb->si_feip_watchdog = esb->feip_watchdog;
	    
	    break;

	default:
	    this_item_unhandled = TRUE;
	    any_unhandled = TRUE;
	    break;
	}
	if (!this_item_unhandled)
	    item->handled = TRUE;
    }
    return (!any_unhandled);
}

static boolean ether_hw_read_state (hwidbtype *hwidb, itemlist *ilist,
				    ethersb *esb)
{
    itemdesc	*item;
    boolean	any_unhandled = FALSE;	/* assume we've handled all items */
    tinybool	this_item_unhandled = FALSE;

    while ((item = itemlist_getnext(ilist))) {
	if (item->handled)
	    continue;
	switch (item->item) {
	case ETHER_HW_OUTPUT_ONE_COLLISION:
	    item->u.l_value = esb->output_one_collision;
	    item->length = sizeof(ulong);
	    break;

	case ETHER_HW_OUTPUT_MORE_COLLISION:
	    item->u.l_value = esb->output_more_collision;
	    item->length = sizeof(ulong);
	    break;

	case ETHER_HW_OUTPUT_LATE_COLLISION:
	    item->u.l_value = esb->output_late_collision;
	    item->length = sizeof(ulong);
	    break;

	case ETHER_HW_OUTPUT_EXCESSIVE_COLLISION:
	    item->u.l_value = esb->output_excessive_collision;
	    item->length = sizeof(ulong);
	    break;

	case ETHER_HW_OUTPUT_DEFERRED:
	    item->u.l_value = esb->output_deferred;
	    item->length = sizeof(ulong);
	    break;

	case ETHER_HW_OUTPUT_LOST_CARRIER:
	    item->u.l_value = esb->output_lost_carrier;
	    item->length = sizeof(ulong);
	    break;

	case ETHER_HW_OUTPUT_BABBLES:
	    item->u.l_value = esb->output_babbles;
	    item->length = sizeof(ulong);
	    break;

	case ETHER_HW_SI_OUTPUT_ONE_COLLISION:
	    item->u.l_value = esb->si_output_one_collision;
	    item->length = sizeof(ulong);
	    break;

	case ETHER_HW_SI_OUTPUT_MORE_COLLISION:
	    item->u.l_value = esb->si_output_more_collision;
	    item->length = sizeof(ulong);
	    break;

	case ETHER_HW_SI_OUTPUT_LATE_COLLISION:
	    item->u.l_value = esb->si_output_late_collision;
	    item->length = sizeof(ulong);
	    break;

	case ETHER_HW_SI_OUTPUT_EXCESSIVE_COLLISION:
	    item->u.l_value = esb->si_output_excessive_collision;
	    item->length = sizeof(ulong);
	    break;

	case ETHER_HW_SI_OUTPUT_DEFERRED:
	    item->u.l_value = esb->si_output_deferred;
	    item->length = sizeof(ulong);
	    break;

	case ETHER_HW_SI_OUTPUT_LOST_CARRIER:
	    item->u.l_value = esb->si_output_lost_carrier;
	    item->length = sizeof(ulong);
	    break;

	case ETHER_HW_SI_OUTPUT_BABBLES:
	    item->u.l_value = esb->si_output_babbles;
	    item->length = sizeof(ulong);
	    break;

	case ETHER_HW_FEIP_NO_CARRIER:
	    item->u.l_value = esb->feip_no_carrier;
	    item->length = sizeof(ulong);
	    break;

	case ETHER_HW_FEIP_MULTICAST:
	    item->u.l_value = esb->feip_multicast;
	    item->length = sizeof(ulong);
	    break;

	case ETHER_HW_FEIP_WATCHDOG:
	    item->u.l_value = esb->feip_watchdog;
	    item->length = sizeof(ulong);
	    break;

	case ETHER_HW_OUTPUT_ONE_COLLISION_STAT:
	    item->u.l_value = esb->output_one_collision -
		esb->si_output_one_collision;
	    item->length = sizeof(ulong);
	    break;

	case ETHER_HW_OUTPUT_MORE_COLLISION_STAT:
	    item->u.l_value = esb->output_more_collision -
		esb->si_output_more_collision;
	    item->length = sizeof(ulong);
	    break;

	case ETHER_HW_OUTPUT_LATE_COLLISION_STAT:
	    item->u.l_value = esb->output_late_collision -
		esb->si_output_late_collision;
	    item->length = sizeof(ulong);
	    break;

	case ETHER_HW_OUTPUT_EXCESSIVE_COLLISION_STAT:
	    item->u.l_value = esb->output_excessive_collision -
		esb->si_output_excessive_collision;
	    item->length = sizeof(ulong);
	    break;

	case ETHER_HW_OUTPUT_DEFERRED_STAT:
	    item->u.l_value = esb->output_deferred - esb->si_output_deferred;
	    item->length = sizeof(ulong);
	    break;

	case ETHER_HW_OUTPUT_LOST_CARRIER_STAT:
	    item->u.l_value = esb->output_lost_carrier -
		esb->si_output_lost_carrier;
	    item->length = sizeof(ulong);
	    break;

	case ETHER_HW_OUTPUT_BABBLES_STAT:
	    item->u.l_value = esb->output_babbles - esb->si_output_babbles;
	    item->length = sizeof(ulong);
	    break;

	case ETHER_HW_FEIP_NO_CARRIER_STAT:
	    item->u.l_value = esb->feip_no_carrier - esb->si_feip_no_carrier;
	    item->length = sizeof(ulong);
	    break;

        case ETHER_HW_FEIP_MULTICAST_STAT:
	    item->u.l_value = esb->feip_multicast - esb->si_feip_multicast;
	    item->length = sizeof(ulong);
	    break;

	case ETHER_HW_FEIP_WATCHDOG_STAT:
	    item->u.l_value = esb->feip_watchdog - esb->si_feip_watchdog;
	    item->length = sizeof(ulong);
	    break;

	default:
	    this_item_unhandled = TRUE;
	    any_unhandled = TRUE;
	    break;
	}
	if (!this_item_unhandled)
	    item->handled = TRUE;
    }
    return (!any_unhandled);
}

static boolean ether_hwidb_state_config (hwidbtype *hwidb, ulong opcode,
					 itemlist *ilist)
{
    boolean	ret;
    ethersb	*esb;

    if (!(hwidb && ilist))
	return (FALSE);

    if (ilist->item_count == -1)
	ilist->item_count = itemlist_count(ilist);

    esb = idb_use_hwsb_inline(hwidb, HWIDB_SB_ETHER);
    if (esb == NULL)
	return (FALSE);

    switch (opcode) {
    case IDB_CONTROL_SET_CONFIG:
	ret = ether_hw_set_config(hwidb, ilist, esb);
	break;

    case IDB_CONTROL_READ_CONFIG:
	ret = ether_hw_read_config(hwidb, ilist, esb);
	break;

    case IDB_CONTROL_SET_STATE:
	ret = ether_hw_set_state(hwidb, ilist, esb);
	break;

    case IDB_CONTROL_READ_STATE:
	ret = ether_hw_read_state(hwidb, ilist, esb);
	break;

    default:
	ret = FALSE;
	break;
    }
    idb_release_hwsb_inline(hwidb, HWIDB_SB_ETHER);

    if (!ret) {
	itemlist_reset(ilist);
	ret = (*esb->super_state_config)(hwidb, opcode, ilist);
    }
    return (ret);
}


/*
 * ether_extract_addr
 *
 * Given an Ethernet packet, extract the requested MAC level address into
 * a hwaddrtype structure.  Assumes pak->addr_start is set.
 */
boolean ether_extract_addr (
    paktype *pak,
    hwaddrtype *address,
    enum SRCDST which)
{
    ether_hdr *ether;

    ether = (ether_hdr *)pak->addr_start;
    address->type = STATION_IEEE48;
    address->length = STATIONLEN_IEEE48;
    address->target_type = TARGET_UNICAST;
    switch (which) {
      case SOURCE:
	ieee_copy(ether->saddr, address->addr);
	break;
      case DESTINATION:
	ieee_copy(ether->daddr, address->addr);
	break;
    }

    if (is_ieee_bcast(address->addr))
	address->target_type = TARGET_BROADCAST;
    else if (ieee_equal(esis_8023_all_is_multi, address->addr))
	address->target_type = TARGET_OSI_ALL_IS;
    else if (ieee_equal(esis_8023_all_es_multi, address->addr))
	address->target_type = TARGET_OSI_ALL_ES;
    
    return(TRUE);
}


/*
 * ether_max_proto_mtu
 *
 * Get the max MTU for a particular protocol.
 * The default is the max MTU of the interface.
 */
static int ether_max_proto_mtu (
    idbtype *swidb,
    enum LINK linktype,
    boolean get_default)
{
    int value;

    value = reg_invoke_media_ether_mtu(linktype, swidb, get_default);
    return(value ? value : swidb->hwptr->maxmtu);
}


/*
 * bcast10mb
 * Returns TRUE if datagram is a physical broadcast
 */
static fuzzy bcast10mb (hwidbtype *hwidb, paktype *pak)
{
    return(is_ieee_bcast(pak->datagramstart) ? FUZ_TRUE : FUZ_FALSE);
}


/*
 * ether_parse_packet
 *
 * Set up a packet buffer so that the network level protocol fields
 * are correctly initialized
 *
 * If data is NULL, pak->datagramstart is assumed to point to the
 * start of the frame to parse. No attempt will be made to align
 * the frame to The Line.
 *
 * WARNING: It it up to the calling routine to insure that the
 * (potential) snap header is in a location that it *byte* accessible.
 * For MCI/Cbus devices, this means reading this information off of the
 * controller and into local memory.  The same rules apply for all other
 * drivers.
 */
iqueue_t ether_parse_packet (
    hwidbtype *idb,
    paktype *pak,
    uchar *data)
{
    ushort type_or_len;
    snap_hdr *snap;
    int info_size;

    /*
     * Check to see if building a frame in a virgin packet, or whether
     * we're just setting up media pointers into the frame
     */
    if (data) {
	/*
	 * We've got a virgin packet. 
	 */
	type_or_len = GETSHORT(data);
	snap = (snap_hdr *)(data + 2);
    } else {
	/*
	 * Soiled packet. Start from datagramstart
	 */
	ether_hdr *ether = (ether_hdr *)pak->datagramstart;
	
	type_or_len = GETSHORT(&ether->type_or_len);
	snap        = (snap_hdr *)ether->data;
    }

    /*
     * If the length field is within the Ethernet MTU of 1500 bytes,
     * then we have an IEEE 802.2 style datagram.  The old Ethernet
     * protocols have type fields greater than 1500 -- except for Xerox PUP.
     */
    if (type_or_len <= MAXETHERSIZE) {
  	switch (snap->dsap) {

	case SAP_SNAP:
	    pak->enctype = ET_SNAP;
	    info_size = SNAP_HDRBYTES;
	    break;
	    
	case SAP_CLNS:
	    pak->enctype = ET_SAP;
	    info_size = SAP_HDRBYTES;
	    break;
	    
	case SAP_NOVELL_ETHER:
	    pak->enctype = ET_NOVELL_ETHER;
	    info_size = 0;
	    break;
	    
        case SAP_SDE:
            pak->enctype = ET_SDE;
            info_size = SDE_ENCAPBYTES;
            break;
	    
	case SAP_IBMNM:
	case SAP_CONS:
	case SAP_NETBIOS:
	default:
	    pak->llc2_sapoffset = ETHER_8023_ENCAPBYTES;
	    
 	    if (snap->control == LLC1_UI) {
		pak->enctype = ET_SAP;
		info_size = SAP_HDRBYTES;

		/*
		 * Check that dsap, ssap and control are zero.
		 * The compiler will optimize two byte reads for dsap
		 * and ssap into a ushort read which can cause alignment
		 * errors, so explicitly use a GETSHORT to get both.
		 */
	    } else if ((GETSHORT(&snap->dsap) == SAPSAP_NULL) &&
		       (snap->control == 0)) { /* Some terminal servers use */
					       /* this - CSCdi20448 */
		pak->enctype = ET_SAP;
		info_size = SAP_HDRBYTES;
	    } else {
		pak->enctype = ET_LLC2;
		/*
		 * Note: This logic will cause pak->network start to
		 * point to the start of the I-field of any LLC frame
		 * whether it actually has an I-field or not. The code
		 * over in the llc2 stack knows whether to look for an
		 * I-field or not depending on the control field value,
		 * and datagramsize.
		 */
		if ((snap->control & LLC1_CLASS_U) == LLC1_CLASS_U) {
		    info_size = SAP_HDRBYTES; /* U-format 3 byte LLC header */
		} else {
		    info_size = SAP_LONGHDRBYTES; /* I or S format 4 byte
						     LLC header */
		}
	    }
	    break;
	}

	/*
	 * If we're building a virgin frame, we have to set datagramstart
	 */
	if (data) {
	    pak->datagramstart = pak_center(pak) -
		ETHER_8023_ENCAPBYTES - info_size;
	}
	pak->mac_start     = pak->datagramstart;
	pak->addr_start    = pak->mac_start;
	pak->rif_start     = NULL;
	pak->riflen 	   = 0;	

	if (info_size) {
	    pak->info_start    = pak->addr_start + ETHER_8023_ENCAPBYTES;
	    pak->network_start = pak->info_start + info_size;
	} else {
	    pak->info_start    = NULL;
	    pak->network_start = pak->addr_start + ETHER_8023_ENCAPBYTES;
	}

	/*
	 * For frames with an IEEE length field, use the smallest size between
	 * what we received (datagramsize) and what the frame is telling us.
	 *
	 * This effectively clips any padding from the frame.
	 *
	 * [ Note : datagramsize *must* be set up by the driver before calling
	 *          this routine. Failure to do this breaks everything ]
	 */
	pak->datagramsize = min(pak->datagramsize,
				(type_or_len + ETHER_8023_ENCAPBYTES));
    } else {
	ushort real_length = 0;

	switch (type_or_len) {
	  case TYPE_APOLLO:
	    pak->enctype = ET_APOLLO;
	    info_size = APOLLO_EXTRABYTES;
	    break;
	    
	  case TYPE_ETHERTALK:
	    pak->enctype = ET_ETHERTALK;
	    info_size = ATALK_LAP_EXTRABYTES;
	    break;
	    
	  case ETHER_80D5_TYPE:
	    pak->enctype = ET_LLC2;
	    info_size = ETHER_80D5_EXTRA_BYTES;
	    {
		sap_hdr * sap_hdr_p;
		uchar   * length_bytes_p;

		if (data) {
		    /* data points at the type_or_length field in the ethernet packet.
		     * Thus, for our 80d5 V2 header, we have the next short as the
		     * length field, then a pad byte, then the actual saps and llc2 data.
		     * Look at the ether_mac80d5_hdr structure, and notice that the 'data'
		     * variable starts at the Type field.
		     */
		    length_bytes_p = data + 2;
		    sap_hdr_p      = (sap_hdr *) (data + 5);
		} else {
		    length_bytes_p = pak->datagramstart + ETHER_ARPA_ENCAPBYTES;
		    sap_hdr_p      = (sap_hdr *) (pak->datagramstart + ETHER_80D5_LENGTH);
		}

		real_length = length_bytes_p[0];
		real_length <<= 8;
		real_length |= length_bytes_p[1];
		real_length += ETHER_80D5_LENGTH;

		if ((sap_hdr_p->control & LLC1_CLASS_U) == LLC1_CLASS_U) {
		    /* Two sap bytes, and a control byte */
		    info_size += 3;
		} else {
		    /* Two sap bytes, and two control bytes */
		    info_size += 4;
		}
	    }
	    break;

	  default:
	    pak->enctype = ET_ARPA;
	    info_size = 0;
	    break;
	}

	/*
	 * If we're building a virgin frame, we have to set datagramstart so
	 * the caller knows where to start copying the packet data.
	 */
	if (data) {
	    pak->datagramstart = pak_center(pak) -
		ETHER_ARPA_ENCAPBYTES - info_size;
	}

	pak->mac_start     = pak->datagramstart;
	pak->addr_start    = pak->mac_start;
	pak->rif_start     = NULL;
	pak->riflen 	   = 0;

	if (pak->enctype == ET_LLC2) {

	    pak->info_start = pak->mac_start + ETHER_80D5_LENGTH;
	    pak->network_start = pak->info_start + info_size;

	    /* This will cut down the datagramsize based on what
	     * is encoded in the packet. If that value is preposterous, 
	     * then don't set it. We need to change the length, because
	     * there will be padding bytes at the end of the datagram
	     * that we want to dispose of. We could also notice if
	     * the datagramsize is < real_length, in which case the
	     * frame is busted. However, there doesn't seem to be
	     * a way to communicate this fact to the caller. Odd.
	     */

	    if (real_length < pak->datagramsize) {
		pak->datagramsize = real_length;
	    }

	} else {
	    pak->info_start    = NULL;
	    pak->network_start = 
		pak->mac_start + ETHER_ARPA_ENCAPBYTES + info_size;
	}
    }

    /*
     * Record just how big this encapsulation was rather than every protocol
     * have it's own fun way to work it out.
     */
    pak->encsize = pak->network_start - pak->datagramstart;
    return(raw_enqueue);
}

/*
 * ether_idb_enqueue()
 * Enqueue an idb onto the ethernet idb list
 */
void ether_idb_enqueue (hwidbtype *hwidb)
{
    ulong idblist_type;

    if (!(hwidb->status & IDB_ETHER)) return;

    /*
     * Check for ethernet hwidb list if not there, create it.
     */
    if (idb_get_listhead(HWIDBLIST_ETHER) == NULL) {
	if (!idb_create_list(HWIDBLIST_ETHER, &idblist_type) ||
	    idblist_type != HWIDBLIST_ETHER) {
	    if (ether_debug) {
		idb_report_create_error("ether");
	    }
	    return;
	}
    }

    /*
     * Add the idb to the ether idb list
     */
    if (!idb_add_hwidb_to_list(hwidb, HWIDBLIST_ETHER)) {
	if (ether_debug) {
	    idb_report_add_error("ether");
	}
    }
}


/*
 * ether_idb_dequeue()
 * Dequeue an idb from the ether idb list
 */
void ether_idb_dequeue (hwidbtype *hwidb)
{
    if (!idb_remove_from_list(hwidb, HWIDBLIST_ETHER)) {
	if (ether_debug) {
	    idb_report_removal_error("ether");
	}
    }
}


/*
 * ether_idb_init
 *
 * Do Ethernet specific interface initializations.
 * Protocol specific routines (such as adding ARP entries) are done elsewhere.
 *
 * This routine sets up some fields in the first (master) soft IDB;  later
 * subinterfaces will clone this information.
 */
void ether_idb_init (hwidbtype *hwidb)
{
    ethersb *esb;
    hwidb_sb_t type = HWIDB_SB_ETHER;

    idbtype *idb = hwidb->firstsw;

    /*
     * Create the subblock, if needed
     */
    esb = idb_get_hwsb_inline(hwidb, HWIDB_SB_ETHER);
    if (esb == NULL) {
	esb = malloc(sizeof(ethersb));
	if (esb == NULL) {
	    /*
	     * This is way bad, we have no memory, crash and burn
	     */
	    crashdump(0);
	}
	
	if (!idb_add_hwsb(hwidb, &type, esb))
	    return;
    }

    /*
     * Stack the state/config vectors, if not already done
     */
    if (hwidb->state_config != ether_hwidb_state_config) {
	esb->super_state_config = hwidb->state_config;
	hwidb->state_config = ether_hwidb_state_config;
    }

    /*
     * Set the fastswitch flavor
     */
    hwidb->fast_switch_type = FS_ETHER;

    GET_TIMESTAMP(hwidb->state_time);
    hwidb->vencap = ether_vencap;
    hwidb->deencap = NULL;
    hwidb->bridge_vencap = (bridge_vencap_t)return_null;
    hwidb->getlink = ether_getlink;
    set_default_queueing(hwidb);
    hwidb->iqueue = raw_enqueue;
    hwidb->parse_packet = ether_parse_packet;
    hwidb->extract_hwaddr = ieee_extract_hwaddr;
    hwidb->extract_snpa = ether_extract_addr;
    hwidb->broadcast = bcast10mb;
    holdq_init(hwidb, ETHER_HOLDQ);
    hwidb->maxmtu = hwidb->maxmtu_def = MAXETHERSIZE; /* set the maximum MTU */
    hwidb->firstsw->sub_mtu = hwidb->maxmtu_def;
    hwidb->max_pak_size_def = ETHER_MAXPAK;
    if_maxdgram_adjust(hwidb, ETHER_MAXDGRAM_DEF);
    hwidb->buffersize = ETHER_BUFFERSIZE_BYTES;
    hwidb->enctype = hwidb->enctype_def = hwidb->enctype_cfg = ET_ARPA;
					/* default encapsulation */
    if (hwidb->type == IDBTYPE_FEIP) {
	hwidb->name = IFNAME_FASTETHER;		/* set preferred device name */
	set_default_interface_bandwidth(idb, FETHER_VISIBLE_BANDWIDTH);
	set_default_interface_delay(idb, FETHER_DELAY);
    } else {
	hwidb->name = IFNAME_ETHER;		/* set preferred device name */
	set_default_interface_bandwidth(idb, VISIBLE_ETHER_BANDWIDTH);
	set_default_interface_delay(idb, ETHER_DELAY);
    }
    hwidb->reliability = 255;
    hwidb->load = 1;
    reg_invoke_media_enctype_setup(idb, ET_ARPA);
    idb->ip_enctype = ET_ARPA;
    hwidb->max_proto_mtu = ether_max_proto_mtu;

    /*
     * Set up how ARP is done.
     */
    idb->arp_arpa = idb->arp_arpa_def = TRUE;
    idb->arp_probe = idb->arp_probe_def = FALSE;
    idb->arp_snap = idb->arp_snap_def = FALSE;
    idb->arp_timeout = ARP_MAXAGE;
}


/*
 * ether_vencap
 *
 * Variable offset encapsulation vector for ethernet.
 *
 * WARNING!!!  This routine assumes that pak->datagramstart has already
 * been set up, and places the header relative to that pointer.
 */
boolean ether_vencap (paktype *pak, long int address)
{
    uchar *daddr = NULL, *ptr;
    uchar const *other_info = zerosnap;
    int enctype = ET_ARPA, length;
    idbtype *interface;
    hwidbtype *hwidb;
    ether_hdr *ether;
    sap_hdr *sap;
    snap_hdr *snap;
    uchar *shdw, srcmac[IEEEBYTES];

    /*
     * If datagramstart not setup, we have a bad caller
     */
    if (pak->datagramstart == NULL)
 	return(FALSE);
    
    interface = pak->if_output;
    hwidb = hwidb_or_null(interface);

    if (!hwidb) {
	return(FALSE);
    }

    /*
     * Do address resolution. We want an encapsulation method, a destination
     * MAC address, and an optional pointer to any other addressing
     * information (presently any SNAP information). We are careful to
     * reset pak->enctype with any new information.
     */
    if (!reg_invoke_media_ether_macaddr(pak->linktype, pak, address, &enctype,
				    &daddr, &other_info)) {
	if (!reg_used_media_ether_macaddr(pak->linktype))
	    errmsg(&msgsym(BADMACREG, LINK), interface->namestring,
		   pak->linktype);
	return(FALSE);
    }

    /*
     * If the egressing interface is vLAN encapsulating,
     * then we need to re-set the encapsulation here.
     */
    pak->enctype = enctype;

    /*
     * We know the destination Ethernet address, now encapsulate the
     * datagram according to method. Quit if no method has emerged by
     * now.
     */
    if (!enctype || !daddr)
	return(FALSE);
    if (pak->enc_flags & ENC_SRCMAC_OVERRIDE) {
	/*
	 * Extract source MAC address from front of packet's data
	 * area, before we scribble over it with the encapsulation
	 * information.
	 */
	ieee_copy(pak->data_area, srcmac);
	shdw = srcmac;
    } else {
	shdw = hwidb->hardware;
    }

    switch (enctype) {

    case ET_PPP:
	if (hwidb->ppp_bridge_ip)
	    goto ether_vencap_arpa;
	/* else fall through */
    case ET_HDLC:
    case ET_FRAME_RELAY:
    case ET_SMDS:
    case ET_ATM:
    case ET_BRIDGE:
	if ((pak->linktype == LINK_ARP || pak->linktype == LINK_IP) &&
	    ROUTING_IF(interface, LINK_IP))
	    return(FALSE);
	/* fall through to ET_ARPA handling */

    case ET_SDE:

	/*
	 * For traffic we generate (e.g. Routing Protocol updates) through
         * Dot10 vLAN configured Ethernet interfaces, we assume them to start
         * out as Ethernet-like encapsulated and will then rewrite the header
         * as appropriate below ...
	 */

    case ET_ARPA:
    case ET_CPP:
    ether_vencap_arpa:
	pak->datagramstart -= ETHER_ARPA_ENCAPBYTES;
	pak->datagramsize += ETHER_ARPA_ENCAPBYTES;
	ether = (ether_hdr *)pak->datagramstart;
	ieee_copy(daddr, ether->daddr);
	if (is_smds(hwidb) ||
	    is_cpp(hwidb) ||
	    (is_ppp(hwidb) && hwidb->ppp_bridge_ip)) {
	    ieee_copy(default_mac_addr, ether->saddr);
	}
	else if (!(hwidb->status & IDB_IEEEMASK) &&
		  ((pak->linktype == LINK_MOP_CONSOLE) ||
		  (pak->linktype == LINK_MOP_BOOT))) {
	    ptr = reg_invoke_media_get_first_ieee_hardware_address();
	    if (ptr)
		ieee_copy(ptr, ether->saddr);
	    else
		ieee_copy(hwidb->hardware, ether->saddr);
	} else {
	    ieee_copy(shdw, ether->saddr);
	}
	PUTSHORT(&ether->type_or_len,(ushort)link2ether(pak->linktype));

	pak->mac_start = (uchar *)ether;
	pak->addr_start = ether->daddr;
	pak->rif_start = NULL;
	pak->riflen = 0;
	pak->info_start = NULL;
	pak->encsize = ETHER_ARPA_ENCAPBYTES;
	break;

      case ET_SAP:
	length = pak->datagramsize + SAP_HDRBYTES;
	pak->datagramstart -= ETHER_SAP_ENCAPBYTES;
	pak->datagramsize += ETHER_SAP_ENCAPBYTES;
	ether = (ether_hdr *)pak->datagramstart;
	ieee_copy(daddr, ether->daddr);
	ieee_copy(shdw, ether->saddr);
	PUTSHORT(&ether->type_or_len, length);
	sap = (sap_hdr *)ether->data;
	sap->dsap    = other_info[0];
	sap->ssap    = other_info[1];
	sap->control = other_info[2];

	pak->mac_start = (uchar *)ether;
	pak->addr_start = ether->daddr;
	pak->rif_start = NULL;
	pak->riflen = 0;
	pak->info_start = ether->data;
	pak->encsize = ETHER_SAP_ENCAPBYTES;
	break;

      case ET_SNAP:
	length = pak->datagramsize + SNAP_HDRBYTES;
	pak->datagramstart -= ETHER_SNAP_ENCAPBYTES;
	pak->datagramsize += ETHER_SNAP_ENCAPBYTES;
	ether = (ether_hdr *)pak->datagramstart;
	ieee_copy(daddr, ether->daddr);
	ieee_copy(shdw, ether->saddr);
	PUTSHORT(&ether->type_or_len, length);
	snap = (snap_hdr *)ether->data;
	snap->dsap    = SAP_SNAP;
	snap->ssap    = SAP_SNAP;
	snap->control = LLC1_UI;
	snap->oui[0]  = other_info[0];
	snap->oui[1]  = other_info[1];
	snap->oui[2]  = other_info[2];
	PUTSHORT(&snap->type, (ushort)link2snaptype(pak->linktype));

	pak->mac_start = (uchar *)ether;
	pak->addr_start = ether->daddr;
	pak->rif_start = NULL;
	pak->riflen = 0;
	pak->info_start = ether->data;
	pak->encsize = ETHER_SNAP_ENCAPBYTES;
	break;
	
      default:
	if (!reg_invoke_media_ether_macwrite(enctype, pak, hwidb,
					     pak->linktype, daddr,
					     other_info)) {

	    errmsg(&msgsym(BADENCAP, LINK), interface->namestring, enctype);
	    return(FALSE);
	}
	break;
    }
    
    if (IS_ISL_VLAN_SUBINTERFACE(interface)) {
	pak->enctype = enctype = ET_ISL;
	if (!reg_invoke_media_ether_macwrite(enctype, pak, hwidb,
					     pak->linktype, daddr,
					     other_info)) {
	    errmsg(&msgsym(BADENCAP, LINK), interface->namestring, enctype);
	    return(FALSE);
	}
    }

    /*
     * Now check to see if the outgoing interface is actually a Dot10 vLAN
     * interface. Doing this here allows for any interior encapsulation to
     * have been prepended above.
     *
     * Increase the encapsulation size as used by the cache update routines
     * to figure out how much MAC to prepend.
     */

    if (IS_DOT10_VLAN_SUBINTERFACE(interface)) {

        if (!reg_invoke_write_ether_dot10_encapsulation(pak, interface))
            return(FALSE);

	reg_invoke_vlan_packet_counters(pak, ET_SDE);
        pak->enctype = ET_SDE;
        pak->encsize += SDE_ENCAPBYTES;
    }

    /*
     * Make sure the datagram size is large enough.
     * When acting as a transparent bridge, we will generate Ethernet
     * framed packets on Serial lines.  Don't pad in that case.
     */
    if ((pak->datagramsize < MINETHERSIZE) && !(hwidb->status & IDB_SERIAL)) {
	memset(pak->datagramstart+pak->datagramsize, 0,
	       MINETHERSIZE - pak->datagramsize);
	pak->datagramsize = MINETHERSIZE;
    }

    /*
     * Make sure routing cache is populated by transit traffic.  Don't
     * do it for LAN emulation.  It will get populated out of atm_vencap. 
     */

    if (!is_atm_lane(interface) && (!is_ppp(hwidb))) {
	reg_invoke_fs_cache_update(pak->linktype, pak, 0L);
    }

    return(TRUE);
}


/*
 * ether_encap
 *
 * Called by the idb dispatch routines
 * Perform various Ethernet/802.3 encapsulation dances
 */
static boolean ether_encap (
    paktype *pak,
    int length,
    long address)
{
    pak->datagramstart = pak->network_start;
    pak->datagramsize = length;
    return(ether_vencap(pak, address));
}


/*
 * ether_getlink
 * Return generic link code for a 10MB datagram
 */ 
long ether_getlink (paktype *pak)
{
    ether_hdr *ether;
    sap_hdr *sap;
    snap_hdr *snap;

    /*
     * First try differentiating based solely upon the encapsulation type.
     */
    ether = (ether_hdr *)pak->datagramstart;
    switch (pak->enctype) {
      case ET_BRIDGE_ENCAPS:
      case ET_HDLC:
	/*
	 * BOGON!
	 */
	return(type2link(GETSHORT(pak->network_start - sizeof(ushort))));

      case ET_TUNNEL:
	return(type2link(pak->tunnelled_type));

      case ET_ARPA:
      case ET_CPP:
	return(type2link(GETSHORT(&ether->type_or_len)));

      case ET_SNAP:
	snap = (snap_hdr *)ether->data;
	return(type2link(GETSHORT(&snap->type)));

      case ET_SAP:
	sap = (sap_hdr *)ether->data;
	pak->llc2_cb = NULL;	/* indicate UI for LLC2 */
	return(sap2link(sap->dsap));
	
      case ET_ETHERTALK:
	switch (pak->datagramstart[ETALKLAPTYPE]) {
	  case ATALK_LAP_SDDP: return LINK_ATALK_SHORT;
	  case ATALK_LAP_XDDP: return LINK_APPLETALK;
	  default:             return LINK_ILLEGAL;
	}
	break;

      case ET_LLC2:
	return(sap2link(pak->llc2_sap));

      case ET_NOVELL_ETHER: return(LINK_NOVELL);
      case ET_APOLLO:	    return(LINK_APOLLO);

      case ET_SDE:
        sap = (sap_hdr *)ether->data;
        return(sap2link(sap->dsap));

      case ET_ISL: return(LINK_ISL);
	
      default:
	return(reg_invoke_media_getlink( pak->enctype, pak->if_input, pak));
    }
    return(LINK_ILLEGAL);
}


/*
 * link2ether
 * Given a link type, return the 10MB Ethernet type code
 */

long link2ether (long link)
{
    ulong value;

    switch (link) {
	case LINK_IP:            return(TYPE_IP10MB);
	case LINK_ARP:		 return(TYPE_RFC826_ARP);
	case LINK_XARP:		 return(TYPE_XEROX_ARP); 
	case LINK_RARP:		 return(TYPE_REVERSE_ARP);
 	case LINK_DECNET:	 return(TYPE_DECNET);
 	case LINK_DECNET_ROUTER_L1: return(TYPE_DECNET);
 	case LINK_DECNET_ROUTER_L2: return(TYPE_DECNET);
 	case LINK_DECNET_PRIME_ROUTER: return(TYPE_DECNET);
 	case LINK_DECNET_NODE:	 return(TYPE_DECNET);
	case LINK_LAT:		 return(TYPE_LAT);
	case LINK_LOOP:		 return(TYPE_LOOP);
	case LINK_MOP_BOOT:	 return(TYPE_MOP_BOOT);
	case LINK_MOP_CONSOLE:	 return(TYPE_MOP_CONSOLE);
	case LINK_XNS:		 return(TYPE_XNS);
	case LINK_NOVELL:	 return(TYPE_NOVELL1);
	case LINK_DEC_SPANNING:	 return(TYPE_DEC_SPANNING);
	case LINK_AARP:		 return(TYPE_AARP);
	case LINK_APPLETALK:	 return(TYPE_ETHERTALK);
	case LINK_APOLLO:	 return(TYPE_APOLLO);
	case LINK_VINES:	 return(TYPE_VINES);
	case LINK_VINES_ECHO:	 return(TYPE_VINES_ECHO);
	case LINK_VINES_LOOP:	 return(TYPE_VINES_LOOP);
	case LINK_PROBE:	 return(TYPE_HP_PROBE);
	case LINK_RSRB:		 return(TYPE_RSRB);
	case LINK_DLSW:		 return(TYPE_DLSW);
	case LINK_CDP:		 return(TYPE_CDP);
	case LINK_CGMP:		 return(TYPE_CGMP);
	case LINK_VTP:		 return(TYPE_VTP);
	default:
	  value = reg_invoke_media_link_to_type(link);
	  return(value ? value : 0L);
    }
}


/*
 * get_hex_num - parse a hex number.
 * Returns a pointer to where the terminator lives.  A terminator
 * is any character that is not a valid hex digit.
 */
char *get_hex_num (
    register char *ptr,			/* the string pointer */
    ushort *address)			/* where to stash */
{
    register int i = 0;			/* accumulate number here */
    register char c = toupper(*ptr++);	/* get first character */

    while (((c >= '0') && (c <= '9')) || ((c >= 'A') && (c <= 'F'))) {
	i = (i << 4) + ((int) c - (((c >= '0') && (c <= '9')) ? '0' : '7'));
	c = toupper(*ptr++);
    }
    *address = (i <= 0xFFFF) ? (ushort) i : 0;
    return(--ptr);			/* give a good return */
}


/*
 * hextet - parse one hextet of an Ethernet address.
 * Returns valid char pointer on success, nil pointer otherwise
 */
static char *
hextet (char *ptr, ushort *address, char brkchar)
{
    ptr = get_hex_num(ptr,address);
    return((*ptr == brkchar) ? ++ptr : NULL);
}


/*
 * parse_ether_address
 * Convert a string of the form A.B.C to a 10MB Ethernet address.
 * Return FALSE if parse fails, TRUE otherwise with "result" updated.
 */
boolean parse_ether_address (register char *ptr, uchar *result)
{
    ptr = (char *) deblank (ptr);	/* flush leading blanks */
    if ((ptr = hextet(ptr,(ushort *)&result[0],'.')) &&
	(ptr = hextet(ptr,(ushort *)&result[2],'.')) &&
	(ptr = hextet(ptr,(ushort *)&result[4],'\0')))
	return(TRUE);		/* a good Ethernet address */
    else
	return(FALSE);		/* an illegal address */
}


/*
 * loop_macaddr_ether
 *
 * Turn an Ethernet loopback address into an encapsulation type and
 * a MAC address. Called by SERVICE_MACADDR_ETHER, indexed by LINK_LOOP.
 */
static boolean loop_macaddr_ether (paktype *pak, long address, int *enctype,
				   uchar **daddr, uchar const **snapaddr)
{
    *enctype = ET_ARPA;
    *daddr = (uchar *)address;
    *snapaddr = NULL;
    return(TRUE);
}


/*
 * ether_loop
 * Handle the Ethernet loopback packet.
 *
 * We can receive a loopback packet for several reasons. If this is
 * a forward request, we will update the skip count, and forward it
 * to the requested destination.
 *
 * If this is a reply packet, then we check to see if the packet was
 * received on a device that can hear its own transmits. If so, and
 * this packet has our source and destination address, then we consider
 * this a valid keepalive packet and update our keepalive timer.
 * If the packet is received on a device that can't hear itself, we
 * update the timer regardless of the source and destination address
 * (presumably, we got the packet because it was sent to the broadcast
 * address).
 */

static void ether_loop (paktype *pak)
{
    idbtype *idb;
    hwidbtype *hwidb;
    ushort *ptr;
    uchar *dataptr = pak->network_start;
    int skip;

    idb = pak->if_input;     
    hwidb = hwidb_or_null(idb);
    skip = lit2h(*(ushort *)dataptr);	    /* Pick up the skip count */

    /* check that skip is even and less than length */

    if (((skip & 1) == 0) &&
	((skip + 2) <= (pak->datagramsize - ETHER_ARPA_ENCAPBYTES))) {
	ptr = (ushort *)(dataptr + skip + 2);
	switch (lit2h(*ptr++)) {
	  case LOOP_FORWARD:
	    if (((*(uchar *)ptr & 1) == 0) &&
		((skip + 10) < (pak->datagramsize - ETHER_ARPA_ENCAPBYTES))) {
		*(ushort *)dataptr = h2lit(skip + IEEEBYTES + 2);
		pak->if_output = idb;
		pak->enctype = ET_ARPA;
		if (ether_encap(pak, pak->datagramsize - ETHER_ARPA_ENCAPBYTES,
			       (long)ptr)) {
		    datagram_out(pak);
		    return;
		}
	    }
	    break;
	  case LOOP_REPLY:
	    if (hwidb != hwidb->output_if_hw) {
		GET_TIMESTAMP(hwidb->mci_lineuptimer);
		hwidb->output_if_hw->mci_lineuptimer = hwidb->mci_lineuptimer;
	    } else if (ieee_equal(pak->datagramstart, hwidb->hardware) &&
		       ieee_equal(pak->datagramstart + IEEEBYTES,
				  hwidb->hardware))
		GET_TIMESTAMP(hwidb->mci_lineuptimer);
	    break;
	}
    }
    retbuffer(pak);
}


/*
 * send_loop
 * Send an ethernet loopback packet to ourself.
 * Currently only MCI interfaces can actually hear themselves.
 * Send to broadcast address if interface is simplex (idb != idb->output_if)
 * Returns TRUE if packet actually sent, FALSE otherwise.
 */
boolean send_loop (hwidbtype *idb, uchar *dest)
{
    ushort *ptr;
    paktype *pak;

    pak = getbuffer(SMALLDATA);
    if (!pak)
	return(FALSE);
    ptr = (ushort *)pak->network_start;
    *ptr++ = 0;
    *ptr++ = h2lit(LOOP_REPLY);
    pak->if_output = idb->output_if_hw->firstsw;
    pak->flags |= (PAK_HEARSELF | PAK_PRIORITY);
    pak->linktype = LINK_LOOP;
    pak->enctype = ET_ARPA;
    if (idb != idb->output_if_hw)
	dest = (uchar *) baddr;
    if (ether_encap(pak, (uchar*) ptr - pak->network_start, (long) dest)) {
	datagram_out(pak);
	return(TRUE);
    } else {
	retbuffer(pak);
	return(FALSE);
    }
}

#if 0
/*
 * print_encaps
 * Print the encapsulation of a Ethernet datagram
 *
 * WARNING:  This doesn't work for TR because of RIFs
 */

void print_encaps (pak,str)
    paktype *pak;
    char *str;
{
    ether_hdr *ether;
    sap_hdr *sap;
    snaphdr *snap;

    ether = (ether_hdr *)pak->datagramstart;
    switch (pak->enctype) {
      case ET_ARPA:
	buginf("\n%s: ARPA dst=%e, src=%e, type=%#x",  str,
	       ether->daddr, ether->saddr, GETSHORT(&ether->type_or_len));
	break;
      case ET_SAP:
	sap = (sap_hdr *)ether->data;
	buginf("\n%s: ISO1 dst=%e, src=%e, len=%d, 802.2=(%-02x, %-02x, %-02x)",
	       str, ether->daddr, ether->saddr, GETSHORT(&ether->type_or_len),
	       sap->dsap, sap->ssap, sap->control);
	break;
      case ET_SNAP:
	snap = (snap_hdr *)ether->data;
	buginf("\n%s: SNAP dst=%e, src=%e, len=%d, 802.2=(%-02x, %-02x, %-02x), type=%x",
	       str, ether->daddr, ether->saddr, GETSHORT(&ether->type_or_len),
	       snap->dsap, snap->ssap, snap->control, GETSHORT(&snap->type));
  	break;
      case ET_ETHERTALK:
	buginf("\n%s: EtherTalk dst=%e, src=%e, type=%#x",  str,
	       &pak->etalketherdst, &pak->etalkethersrc, 
	       pak->etalkethertype);
	break;
      case ET_APOLLO:
	buginf("\n%s: Apollo dst=%e, src=%e, type=%#x",  str,
	       &pak->apolloetherdst, &pak->apolloethersrc,
	       pak->apolloethertype);
	break;
      default:
	buginf("\n%s: unk ethernet type - %x", str,pak->enctype);
    }
}
#endif

/*
 * ether_odd_byte_start_bug
 */
boolean ether_odd_byte_start_bug (hwidbtype *idb)
{
    mcitype* mci;
    dcitype* dci;

    /*
     * This is only valid for the AGS
     */
    if (IS_FAMILY(FAMILY_CSC2)) {
	if (idb->status & IDB_MCI) {
	    mci = idb->devctl;
	    if (mci->sw_version < 0x0109)
		return(TRUE);
	} else if (idb->status & IDB_CBUS) {
	    dci = ((mcitype *)idb->devctl)->dci[idb->cbus_ctrlr_index];
	    if (dci->hw_version == 0x0500) {
		if (dci->sw_version < 0x0107)
		    return(TRUE);
	    } else if (dci->hw_version == 0x0501) {
		if (dci->sw_version < 0x0202)
		return(TRUE);
	    }
	}
    }
    return(FALSE);
}


/*
 * ether_init
 *
 * Initialize global Ethernet related information.
 */
static void ether_init (subsystype* subsys)
{
    /*
     * Set up the debugging flags
     */
    ether_debug_init();

    /*
     * Add parser support
     */
    ether_parser_init();

    /*
     * Register some functions
     */
    reg_add_loop_enqueue(IDB_ETHER, ether_loop,"ether_loop");
    reg_add_media_ether_macaddr(LINK_LOOP,
		      loop_macaddr_ether, "loop_macaddr_ether");
    reg_add_media_hwidb_enqueue_private_list(ether_idb_enqueue,
					     "ether_idb_enqueue");
    reg_add_default_media_ether_macaddr((service_media_ether_macaddr_type)
					return_false, "return_false");
    reg_add_default_media_ether_macwrite((service_media_ether_macwrite_type)
					 return_false, "return_false");
}

/*
 * Ethernet subsystem header
 */

#define ETHER_MAJVERSION 1
#define ETHER_MINVERSION 0
#define ETHER_EDITVERSION 1

SUBSYS_HEADER(ether, ETHER_MAJVERSION, ETHER_MINVERSION, ETHER_EDITVERSION,
	      ether_init, SUBSYS_CLASS_KERNEL,
	      NULL,
	      NULL);
