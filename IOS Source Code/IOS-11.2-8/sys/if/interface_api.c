/* $Id: interface_api.c,v 3.7.20.8 1996/08/15 02:32:32 hampton Exp $
 * $Source: /release/112/cvs/Xsys/if/interface_api.c,v $
 *------------------------------------------------------------------
 * Binary API to the idb structure
 *
 * April 1995, Steve Berl
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: interface_api.c,v $
 * Revision 3.7.20.8  1996/08/15  02:32:32  hampton
 * Change some function arguments to be 'const'.  [CSCdi66102]
 * Branch: California_branch
 *
 * Revision 3.7.20.7  1996/07/26  00:07:36  hampton
 * Add some accessor routines for various IDB values, and make some
 * existing routines use const values.  [CSCdi64087]
 * Branch: California_branch
 *
 * Revision 3.7.20.6  1996/06/01  01:31:44  hampton
 * Remove unnecessary cross module references.  [CSCdi59221]
 * Branch: California_branch
 *
 * Revision 3.7.20.5  1996/05/17  11:22:46  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.7.6.2  1996/04/03  14:37:57  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.7.6.1  1996/03/21  06:10:29  ppearce
 * IBU modularity: Channel subblock
 * Branch: IbuMod_Calif_branch
 *   Move Channel fields from hwidbtype/idbtype structs to Channel subblock
 *
 * Revision 3.7.20.4  1996/05/09  14:30:03  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.7.20.3.14.1  1996/04/27  07:03:52  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.7.20.3.4.1  1996/04/08  01:54:42  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.7.20.3  1996/03/26  19:16:26  jjohnson
 * CSCdi52557:  idb_get_hwaddr() can vector to address 0
 * Make sure the extract_hwaddr vector is valid before vectoring thru it
 * Branch: California_branch
 *
 * Revision 3.7.20.2  1996/03/23  01:32:33  slin
 * Branch: California_branch
 *
 * AppleTalk makes its exodus from the idb.
 *
 * Revision 3.7.20.1  1996/03/18  20:14:02  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.7.24.2  1996/03/28  17:25:30  rbadri
 * Branch: LE_Syn_Cal_V111_1_3_branch
 * MTU support for sub-interfaces (used by ATM interfaces)
 *
 * Revision 3.7.24.1  1996/03/22  09:37:18  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.7.10.1  1996/03/05  06:13:53  rlowe
 * Apply LANE content to merge branch.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 3.7.4.1  1996/02/27  21:08:22  cakyol
 * non sync sync of LE_Cal_V111_0_16_branch to V111_1_0_3 yielding
 * LE_Cal_V111_1_0_3_branch
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 3.7  1996/01/25  23:11:22  gstovall
 * CSCdi47390:  down-when-looped command not working (regression)
 * Remember to reset the itemlist before tossing it up to the next level.
 *
 * Revision 3.6  1996/01/25  20:46:14  ccpoe
 * CSCdi47341:  remove subif_tmp_state from idb. It's no longer used.
 *
 * Revision 3.5  1996/01/22  03:55:13  dmccowan
 * CSCdi45007:  LLC2 LCP negotiated over PPP when not wanted
 *
 * Revision 3.4  1996/01/17  11:50:18  dcalia
 * Update port config to disallow illegal configuration (CSCdi46447)
 *
 * Revision 3.3.14.1  1996/01/24  01:34:16  cyoung
 * VTP MIB support and dependant auto-config base. Part I
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 3.3  1995/11/17  09:29:55  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:38:12  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:50:38  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.5  1995/11/08  21:03:05  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.4  1995/10/18  06:58:18  pmorton
 * CSCdi40639:  CMNS uses wrong MAC address to open LLC2 session
 * Fix stupid comment.  Function wasn't inline at all.
 *
 * Revision 2.3  1995/10/18  05:32:13  pmorton
 * CSCdi40639:  CMNS uses wrong MAC address to open LLC2 session
 * Add idb_is_tokenring_like, and llc_clear_rii.
 *
 * Revision 2.2  1995/06/30  06:41:56  dstine
 * CSCdi36619:  IDB nits
 *         - tidy up some nits
 *         - fix error in dynamic hwsb usage check
 *
 * Revision 2.1  1995/06/07  20:53:18  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#include "master.h"
#include <string.h>
#include "interface_private.h"
#include "../ibm/sdlc.h"
#include "packet.h"
#include "ieee.h"
#include "../util/itemlist.h"
#include "../atalk/atalk_private.h"
#include "../hes/channel_sb.h"
#include "../decnet/decnet.h"
#include "../xns/xns.h"
#include "../xns/novell.h"
#include "../lane/lane.h"


const char* idb_get_namestring (const idbtype* idb)
{
    if (idb != NULL)
	return (idb->namestring);
    else
	return (NULL);
}

void idb_get_hwaddr (const idbtype* idb, hwaddrtype* addrtype)
{
    hwidbtype* hwidb;

    if ((idb != NULL) && (addrtype != NULL)) {
	hwidb = idb->hwptr;
	if (hwidb != NULL && hwidb->extract_hwaddr != NULL) {
	    (*hwidb->extract_hwaddr)(hwidb, addrtype);
	}
    }
}

const boolean idb_is_virtual (const idbtype* idb)
{
    if ((idb != NULL) && (idb->hwptr != NULL)) {
	return (idb->hwptr->status & IDB_VIRTUAL);
    } else {
	return (FALSE);
    }
}

const boolean idb_is_ieee (const idbtype* idb)
{
    if (idb == NULL)
	return (FALSE);
    return (is_ieee802(idb->hwptr));
}

const boolean idb_is_ieeelike (const idbtype* idb)
{
    if (idb == NULL)
	return (FALSE);
    return (is_ieee802(idb->hwptr) || is_atm_lane(idb));
}

const boolean idb_is_tokenring (const idbtype* idb)
{
    if (idb != NULL) {
	return (is_tokenring(idb->hwptr));
    } else {
	return (FALSE);
    }
}

const boolean idb_is_fddi (const idbtype* idb)
{
    if (idb != NULL) {
	return (is_fddi(idb->hwptr));
    } else {
	return (FALSE);
    }
}

const boolean idb_is_ethernet (const idbtype* idb)
{
    if (idb != NULL) {
	return (is_ethernet(idb->hwptr));
    } else {
	return (FALSE);
    }
}

/*
 * Predicate for "etherlike" interfaces.  This consists of Ethernet, LEX,
 * and ATM LANE.  Note that it does *not* include BVI, since BVI is not
 * really transparent to the protocols.  The old idb_is_etherlike()
 * routine included BVI.  The name has been changed since the semantics
 * have changed as well.
 */
const boolean idb_is_ether_like (const idbtype* idb)
{
    if (idb == NULL)
	return (FALSE);
    return((idb->hwptr->status & (IDB_ETHER|IDB_LEX)) || is_atm_lane(idb));
}

const boolean idb_is_fastethernet (const idbtype* idb)
{
    if ((idb != NULL) && (idb->hwptr) && (idb->hwptr->type ==
					  IDBTYPE_FEIP)) 
	    return TRUE;
    return FALSE;
}

const boolean idb_is_serial (const idbtype* idb)
{
    if (idb == NULL)
	return (FALSE);
    return (is_serial(idb->hwptr));
}

const boolean idb_is_hdlc (const idbtype* idb)
{
    if (idb == NULL)
	return (FALSE);
    return (is_hdlc(idb->hwptr));
}

const boolean idb_is_framerelay (const idbtype* idb)
{
    if (idb != NULL) {
	return (is_frame_relay(idb->hwptr));
    } else {
	return (FALSE);
    }
}

const boolean idb_is_sdlc (const idbtype* idb)
{
    if (idb != NULL) {
	return (is_sdlc(idb->hwptr));
    } else {
	return (FALSE);
    }
}

const boolean idb_is_x25 (const idbtype* idb)
{
    if (idb != NULL) {
	return (is_x25(idb->hwptr));
    } else {
	return (FALSE);
    }
}   

const boolean idb_is_smds (const idbtype* idb)
{
    if (idb != NULL) {
	return (is_smds(idb->hwptr));
    } else {
	return (FALSE);
    }
}   

/*
 * is_p2p
 *
 * Returns TRUE if the idb is a point-to-point interface.
 */
const boolean is_p2p (const idbtype *idb)
{
    return (is_p2p_inline(idb));
}

/*
 * is_p2p_hw
 *
 * Returns TRUE if the hwidb is a point-to-point interface.
 */
const boolean is_p2p_hw (const hwidbtype *hwidb)
{
    return (is_p2p_hw_inline(hwidb));
}

/*
 * is_ppp
 *
 * Returns TRUE if the idb is using the PPP encapsulation.
 */
const boolean idb_is_ppp (const idbtype* idb)
{
    if (idb != NULL) {
	return (is_ppp(idb->hwptr));
    } else {
	return (FALSE);
    }
}   

const boolean idb_is_tunnel (const idbtype* idb)
{
    if (idb == NULL)
	return (FALSE);
    return (is_tunnel(idb->hwptr));
}

const boolean idb_is_atmhw (const idbtype* idb)
{
    if (idb == NULL)
	return (FALSE);
    return (is_atmhw(idb->hwptr));
}

const boolean idb_is_atm (const idbtype* idb)
{
    if (idb != NULL) {
	return (is_atm(idb->hwptr));
    } else {
	return (FALSE);
    }
}   

const boolean idb_is_atm_lane (const idbtype* idb)
{
    return (is_atm_lane(idb));
}   

const boolean idb_is_bvi (const idbtype* idb)
{
    if (idb == NULL)
	return (FALSE);
    return (is_bvi(idb->hwptr));
}

const boolean idb_is_lan (const idbtype* swidb)
{
    hwidbtype *hwidb;

    if (swidb == NULL)
	return(FALSE);
    hwidb = swidb->hwptr;
    if (is_ieee802(hwidb))
	return(TRUE);
    if (is_bvi(hwidb))
	return(TRUE);
    return(is_atm_lane(swidb));
}

const int idb_get_unit (const idbtype* idb)
{
    if ((idb != NULL) && (idb->hwptr != NULL)) {
	return (idb->hwptr->unit);
    } else {
	return (0);
    }
}

const ipaddrtype idb_sw_get_ipaddress (const idbtype *idb)
{
    return (idb ? idb->ip_address : 0);
}

const boolean idb_validate_sdlc_address(idbtype* idb,int sec_addr)
{
    hwidbtype *hwidb;

    hwidb = hwidb_or_null(idb);
    if ((hwidb) && (hwidb->sdlc_data[sec_addr] != NULL))
       return (TRUE);
    return(FALSE);
}

const ushort idb_sw_get_atalknet (const idbtype *idb)
{
    atalkidbtype *atidb;

    if (idb == NULL)
	return 0;
    
    atidb = idb_get_swsb(idb, SWIDB_SB_APPLE);
    return (atidb ? atidb->atalknet : 0);
}

const boolean
idb_sw_get_atalkclientmode (const idbtype *idb)
{
    atalkidbtype *atidb;

    if (idb == NULL)
	return FALSE;

    atidb = idb_get_swsb(idb, SWIDB_SB_APPLE);
    return (atidb ? FALSE : atidb->atalk_allow_clients);
}

const uchar *idb_sw_get_clnsnet (const idbtype *swidb)
{
    return(swidb->clns_net);
}

const ulong idb_sw_get_dnnet (const idbtype *swidb)
{
    return(swidb ? swidb->dn_network : 0);
}

const ulong idb_sw_get_ipxnet (const idbtype *swidb)
{
    return(swidb->primary_nidb ? swidb->primary_nidb->novellnet : 0);
}

const uchar *idb_sw_get_ipxnode (const idbtype *swidb)
{
    return(swidb->primary_nidb ? swidb->novell_node : zeromac);
}

const ulong idb_sw_get_spanindex (const idbtype *swidb)
{
    return(swidb->span_index);
}

const ulong idb_sw_get_xnsnet (const idbtype *swidb)
{
    xnssb *idb;

    idb = idb_get_swsb(swidb, SWIDB_SB_XNS);
    return(idb ? idb->xnsnet : 0);
}

const uchar *idb_sw_get_xnsnode (const idbtype *swidb)
{
    xnssb *idb;

    idb = idb_get_swsb(swidb, SWIDB_SB_XNS);
    return(idb ? idb->xns_node : NULL);
}

/*
 * idb_hw_set_generic_config
 *
 * IDB Configuration setting function for generic software IDB fields.
 *
 */
static boolean
idb_hw_set_generic_config (hwidbtype *hwidb, itemlist *ilist)
{
    itemdesc	*item;
    boolean	any_unhandled = FALSE;	/* assume we've handled all items */
    tinybool	this_item_unhandled;

    while ((item = itemlist_getnext(ilist))) {
	if (item->handled)
	    continue;
	this_item_unhandled = FALSE;

	switch (item->item) {
	case ITM_HW_NAME:
	    hwidb->name = (uchar *)item->u.buffer;
	    break;

	case ITM_HW_TYPESTRING:
	    hwidb->typestring = (uchar *)item->u.buffer;
	    break;

	case ITM_HW_HW_NAMESTRING:
	    hwidb->hw_namestring = (uchar *)item->u.buffer;
	    break;

	case ITM_HW_HW_SHORT_NAMESTRING:
	    hwidb->hw_short_namestring = (uchar *)item->u.buffer;
	    break;

	case ITM_HW_BIA:
	    ieee_copy(item->u.buffer, hwidb->bia);
	    break;

	case ITM_HW_MAC_ADDRESS:
	    ieee_copy(item->u.buffer, hwidb->hardware);
	    break;

	case ITM_HW_FORCED_MAC:
	    ieee_copy(item->u.buffer, hwidb->forced_mac);
	    break;

	case ITM_HW_MTU_MAX:
	    hwidb->maxmtu = item->u.s_value[0];
	    break;

	case ITM_HW_MTU_DEF:
	    hwidb->maxmtu_def = item->u.s_value[0];
	    hwidb->firstsw->sub_mtu = hwidb->maxmtu_def;
	    break;

	case ITM_HW_BUFFERSIZE:
	    hwidb->max_buffers = item->u.s_value[0];
	    break;

	case ITM_HW_CARRIER_DELAY:
	    hwidb->carrierdelay = item->u.l_value;
	    break;

	case ITM_HW_ENCTYPE:
	    hwidb->enctype = item->u.l_value;
	    break;

	case ITM_HW_ENCTYPE_DEF:
	    hwidb->enctype_def = item->u.l_value;
	    break;

	case ITM_HW_ENCTYPE_CFG:
	    hwidb->enctype_cfg = item->u.l_value;
	    break;

	case ITM_HW_ENCSIZE:
	    hwidb->encsize = item->u.l_value;
	    break;

	case ITM_HW_START_OFFSET:
	    hwidb->start_offset = item->u.l_value;
	    break;

	case ITM_HW_NOKEEPALIVE:
	    hwidb->nokeepalive = item->u.l_value;
	    break;

	case ITM_HW_EXT_LOOP_TYPE:
	    hwidb->ext_loop_type = item->u.s_value[0];
	    break;

	case ITM_HW_KEEP_PERIOD:
	    hwidb->keep_period = item->u.s_value[0];
	    break;

	case ITM_HW_KEEP_COUNT:
	    hwidb->keep_count = item->u.s_value[0];
	    break;

	case ITM_HW_QUEUE_ALGORITHM:
	    hwidb->queue_algorithm = item->u.s_value[0];
	    break;

	default:
	    break;
	}
	if (!this_item_unhandled)
	    item->handled = TRUE;
    }
    return (!any_unhandled);
}

#ifdef NOBODY_CALLS_ME

/*
 * idb_sw_set_generic_config
 *
 * IDB Configuration setting function for generic software IDB fields.
 *
 */
static boolean
idb_sw_set_generic_config (idbtype *idb, itemlist *ilist)
{
    itemdesc	*item;
    boolean	any_unhandled = FALSE;	/* assume we've handled all items */
    tinybool	this_item_unhandled;

    while ((item = itemlist_getnext(ilist))) {
	if (item->handled)
	    continue;
	this_item_unhandled = FALSE;
	switch (item->item) {
	case ITM_SW_IP_MTU:
	    idb->ip_mtu = item->u.l_value;
	    break;

	case ITM_SW_CLNS_MTU:
	    idb->clns_mtu = item->u.l_value;
	    break;

	case ITM_SW_TBRIDGE_ON_SWIDB:
	    idb->tbridge_on_swidb = item->u.l_value;
	    break;

	case ITM_SW_SUBIF_LINK_TYPE:
	    break;

	case ITM_SW_NAMESTRING:
	    idb->namestring = item->u.buffer;
	    break;

	case ITM_SW_SHORT_NAMESTRING:
	    idb->short_namestring = item->u.buffer;
	    break;

	case ITM_SW_DESCRIPTION:
	    idb->description = item->u.buffer;
	    break;

	case ITM_SW_LAN_NAME:
	    strncpy(idb->lanName, item->u.buffer, sizeof(idb->lanName));
	    break;

	case ITM_SW_STATIC_ROUTING:
	    idb->static_routing = item->u.l_value;
	    break;

	case ITM_SW_RESOLVEMETHOD:
	    idb->resolvemethod = item->u.l_value;
	    break;

	case ITM_SW_DELAY:
	    
	    break;

	case ITM_SW_DELAY_DEF:
	    break;

	case ITM_SW_VISIBLE_BANDWIDTH_DEF:
	    break;

	case ITM_SW_VISIBLE_BANDWIDTH:
	    break;

	case ITM_SW_MANUAL_DELAY:
	    break;

	case ITM_SW_MANUAL_BANDWIDTH:
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
#endif /* NOBODY_CALLS_ME */


/*
 * idb_hw_read_generic_config
 *
 * IDB Configuration setting function for generic software IDB fields.
 *
 */
static boolean
idb_hw_read_generic_config (hwidbtype *hwidb, itemlist *ilist)
{
    itemdesc	*item;
    boolean	any_unhandled = FALSE;	/* assume we've handled all items */
    tinybool	this_item_unhandled;

    while ((item = itemlist_getnext(ilist))) {
	if (item->handled)
	    continue;
	this_item_unhandled = FALSE;
	switch (item->item) {
	case ITM_HW_NAME:
	    item->u.buffer = hwidb->name;
	    item->length = sizeof(char *);
	    break;

	case ITM_HW_TYPESTRING:
	    item->u.buffer = hwidb->typestring;
	    item->length = sizeof(char *);
	    break;

	case ITM_HW_HW_NAMESTRING:
	    item->u.buffer = hwidb->hw_namestring;
	    item->length = sizeof(char *);
	    break;

	case ITM_HW_HW_SHORT_NAMESTRING:
	    item->u.buffer = hwidb->hw_short_namestring;
	    item->length = sizeof(char *);
	    break;

	case ITM_HW_BIA:
	    ieee_copy(hwidb->bia, item->u.buffer);
	    item->length = IEEEBYTES;
	    break;

	case ITM_HW_MAC_ADDRESS:
	    ieee_copy(hwidb->hardware, item->u.buffer);
	    item->length = IEEEBYTES;
	    break;

	case ITM_HW_FORCED_MAC:
	    ieee_copy(hwidb->forced_mac, item->u.buffer);
	    item->length = IEEEBYTES;
	    break;

	case ITM_HW_MTU_MAX:
	    item->u.s_value[0] = hwidb->maxmtu;
	    item->length = sizeof(ushort);
	    break;

	case ITM_HW_MTU_DEF:
	    item->u.s_value[0] = hwidb->maxmtu_def;
	    item->length = sizeof(ushort);
	    break;

	case ITM_HW_BUFFERSIZE:
	    item->u.s_value[0] = hwidb->maxmtu;
	    item->length = sizeof(ulong);
	    break;

	case ITM_HW_CARRIER_DELAY:
	    item->u.l_value = hwidb->carrierdelay;
	    item->length = sizeof(ulong);
	    break;

	case ITM_HW_ENCTYPE:
	    item->u.l_value = hwidb->enctype;
	    item->length = sizeof(ulong);
	    break;

	case ITM_HW_ENCTYPE_DEF:
	    item->u.l_value = hwidb->enctype_def;
	    item->length = sizeof(ulong);
	    break;

	case ITM_HW_ENCTYPE_CFG:
	    item->u.l_value = hwidb->enctype_cfg;
	    item->length = sizeof(ulong);
	    break;

	case ITM_HW_ENCSIZE:
	    item->u.l_value = hwidb->encsize;
	    item->length = sizeof(int);
	    break;

	case ITM_HW_START_OFFSET:
	    item->u.l_value = hwidb->start_offset;
	    item->length = sizeof(int);
	    break;

	case ITM_HW_NOKEEPALIVE:
	    item->u.l_value = hwidb->nokeepalive;
	    item->length = sizeof(boolean);
	    break;

	case ITM_HW_EXT_LOOP_TYPE:
	    item->u.s_value[0] = hwidb->ext_loop_type;
	    item->length = sizeof(ushort);
	    break;

	case ITM_HW_KEEP_PERIOD:
	    item->u.s_value[0] = hwidb->keep_period;
	    item->length = sizeof(short);
	    break;

	case ITM_HW_KEEP_COUNT:
	    item->u.s_value[0] = hwidb->keep_count;
	    item->length = sizeof(short);
	    break;

	case ITM_HW_QUEUE_ALGORITHM:
	    item->u.s_value[0] = hwidb->queue_algorithm;
	    item->length = sizeof(ushort);
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

#ifdef NOBODY_CALLS_ME

/*
 * idb_sw_read_generic_config
 *
 * IDB Configuration setting function for generic software IDB fields.
 *
 */
static boolean
idb_sw_read_generic_config (idbtype *idb, itemlist *ilist)
{
    itemdesc	*item;
    boolean	any_unhandled = FALSE;	/* assume we've handled all items */
    tinybool	this_item_unhandled;

    while ((item = itemlist_getnext(ilist))) {
	if (item->handled)
	    continue;
	this_item_unhandled = FALSE;
	switch (item->item) {
	case ITM_SW_IP_MTU:
	    break;

	case ITM_SW_CLNS_MTU:
	    break;

	case ITM_SW_TBRIDGE_ON_SWIDB:
	    break;

	case ITM_SW_SUBIF_LINK_TYPE:
	    break;

	case ITM_SW_NAMESTRING:
	    break;

	case ITM_SW_SHORT_NAMESTRING:
	    break;

	case ITM_SW_DESCRIPTION:
	    break;

	case ITM_SW_LAN_NAME:
	    break;

	case ITM_SW_STATIC_ROUTING:
	    break;

	case ITM_SW_RESOLVEMETHOD:
	    break;

	case ITM_SW_DELAY:
	    break;

	case ITM_SW_DELAY_DEF:
	    break;

	case ITM_SW_VISIBLE_BANDWIDTH_DEF:
	    break;

	case ITM_SW_VISIBLE_BANDWIDTH:
	    break;

	case ITM_SW_MANUAL_DELAY:
	    break;

	case ITM_SW_MANUAL_BANDWIDTH:
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
#endif /* NOBODY_CALLS_ME */

/*
 * idb_hw_set_generic_state
 *
 * IDB State setting function for generic hardware IDB fields.
 *
 */
static boolean
idb_hw_set_generic_state (hwidbtype *hwidb, itemlist *ilist)
{
    itemdesc	*item;
    boolean	any_unhandled = FALSE;	/* assume we've handled all items */
    tinybool	this_item_unhandled;

    while ((item = itemlist_getnext(ilist))) {
	if (item->handled)
	    continue;
	this_item_unhandled = FALSE;
	switch (item->item) {
	case ITM_HW_HW_ENABLED:
	    hwidb->hw_enabled = item->u.b_value[0];
	    break;

	case ITM_HW_STATUS:
	    hwidb->status = item->u.l_value;
	    break;

	case ITM_HW_STATE:
	    hwidb->state = item->u.l_value;
	    break;

	case ITM_HW_STATE_TIME:
	    hwidb->state_time = item_get_timestamp(ilist, item);
	    break;

	case ITM_HW_OLDSTATE:
	    hwidb->oldstate = item->u.l_value;
	    break;

	case ITM_HW_NEWPROTO:
	    hwidb->newproto = item->u.l_value;
	    break;

	case ITM_HW_NEWFDDISTAT:
	    hwidb->newfddistat = item->u.l_value;
	    break;

	case ITM_HW_CARRIERTIMER:
	    break;

	case ITM_HW_LASTINPUT:
	    break;

	case ITM_HW_LASTOUTPUT:
	    break;

	case ITM_HW_LASTRESET:
	    break;

	case ITM_HW_REASON:
	    hwidb->reason = item->u.buffer;
	    break;

	case ITM_HW_LOOPBACK:
	    hwidb->loopback = item->u.l_value;
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

#ifdef NOBODY_CALLS_ME
/*
 * idb_sw_set_generic_state
 *
 * IDB State setting function for generic software IDB fields.
 *
 */
static boolean
idb_sw_set_generic_state (idbtype *idb, itemlist *ilist)
{
    itemdesc	*item;
    boolean	any_unhandled = FALSE;	/* assume we've handled all items */
    tinybool	this_item_unhandled;

    while ((item = itemlist_getnext(ilist))) {
	if (item->handled)
	    continue;
	this_item_unhandled = FALSE;
	switch (item->item) {
	case ITM_SW_SUBIF_STATE:
	    idb->subif_state = item->u.l_value;
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
#endif /* NOBODY_CALLS_ME */

/*
 * idb_hw_read_generic_state
 *
 * IDB State reading function for generic hardware IDB fields.
 *
 */
static boolean
idb_hw_read_generic_state (hwidbtype *hwidb, itemlist *ilist)
{
    itemdesc	*item;
    boolean	any_unhandled = FALSE;	/* assume we've handled all items */
    tinybool	this_item_unhandled;

    while ((item = itemlist_getnext(ilist))) {
	if (item->handled)
	    continue;
	this_item_unhandled = FALSE;
	switch (item->item) {
	case ITM_HW_HW_ENABLED:
	    item->u.b_value[0] = hwidb->hw_enabled;
	    item->length = sizeof(tinybool);
	    break;

	case ITM_HW_STATUS:
	    item->u.l_value = hwidb->status;
	    item->length = sizeof(ulong);
	    break;

	case ITM_HW_STATE:
	    item->u.l_value = hwidb->state;
	    item->length = sizeof(ulong);
	    break;

	case ITM_HW_STATE_TIME:
	    item_put_timestamp(ilist, item, hwidb->state_time);
	    item->length = sizeof(sys_timestamp);
	    break;

	case ITM_HW_OLDSTATE:
	    item->u.l_value = hwidb->oldstate;
	    item->length = sizeof(ulong);
	    break;

	case ITM_HW_NEWPROTO:
	    item->u.l_value = hwidb->newproto;
	    item->length = sizeof(boolean);
	    break;

	case ITM_HW_NEWFDDISTAT:
	    item->u.l_value = hwidb->newfddistat;
	    item->length = sizeof(int);
	    break;

	case ITM_HW_LASTINPUT:
	    item_put_timestamp(ilist, item, hwidb->lastinput);
	    item->length = sizeof(sys_timestamp);
	    break;

	case ITM_HW_LASTOUTPUT:
	    item_put_timestamp(ilist, item, hwidb->lastoutput);
	    item->length = sizeof(sys_timestamp);
	    break;

	case ITM_HW_LASTRESET:
	    item_put_timestamp(ilist, item, hwidb->lastreset);
	    item->length = sizeof(sys_timestamp);
	    break;

	case ITM_HW_REASON:
	    item->u.buffer = (void *)hwidb->reason;
	    item->length = strlen(hwidb->reason);
	    break;

	case ITM_HW_IN_LOOPBACK:
	    item->u.l_value = hwidb->inloopback;
	    item->length = sizeof(boolean);
	    break;

	case ITM_HW_LOOPBACK:
	    item->u.l_value = hwidb->loopback;
	    item->length = sizeof(boolean);
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

#ifdef NOBODY_CALLS_ME
/*
 * idb_sw_read_generic_state
 *
 * IDB State reading function for generic software IDB fields.
 *
 */
static boolean
idb_sw_read_generic_state (idbtype *idb, itemlist *ilist)
{
    itemdesc	*item;
    boolean	any_unhandled = FALSE;	/* assume we've handled all items */
    tinybool	this_item_unhandled;

    while ((item = itemlist_getnext(ilist))) {
	if (item->handled)
	    continue;
	this_item_unhandled = FALSE;
	switch (item->item) {
	case ITM_SW_SUBIF_STATE:
	    item->u.l_value = idb->subif_state;
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
#endif /* NOBODY_CALLS_ME */


boolean
hwidb_generic_state_config (hwidbtype *hwidb, ulong opcode, itemlist *ilist)
{
    boolean	ret;

    if (hwidb && ilist)
	return (FALSE);

    if (ilist->item_count == -1)
	ilist->item_count = itemlist_count(ilist);

    switch (opcode) {
    case IDB_CONTROL_SET_CONFIG:
	ret = idb_hw_set_generic_config(hwidb, ilist);
	break;
	
    case IDB_CONTROL_READ_CONFIG:
	ret = idb_hw_read_generic_config(hwidb, ilist);
	break;

    case IDB_CONTROL_SET_STATE:
	ret = idb_hw_set_generic_state(hwidb, ilist);
	break;

    case IDB_CONTROL_READ_STATE:
	ret = idb_hw_read_generic_state(hwidb, ilist);
	break;

    default:
	return (FALSE);
    }
    return (ret);
}

#ifdef NOBODY_CALLS_ME
boolean
swidb_generic_state_config (idbtype *idb, ulong opcode, itemlist *ilist)
{
    boolean	ret;

    if (idb && ilist)
	return (FALSE);

    if (ilist->item_count == -1)
	ilist->item_count = itemlist_count(ilist);

    switch (opcode) {
    case IDB_CONTROL_SET_CONFIG:
	ret = idb_sw_set_generic_config(idb, ilist);
	break;
	
    case IDB_CONTROL_READ_CONFIG:
	ret = idb_sw_read_generic_config(idb, ilist);
	break;

    case IDB_CONTROL_SET_STATE:
	ret = idb_sw_set_generic_state(idb, ilist);
	break;

    case IDB_CONTROL_READ_STATE:
	ret= idb_sw_read_generic_state(idb, ilist);
	break;

    default:
	return (FALSE);
    }
    /*
     * We had unhandled items in our item list. Toss them at the
     * hardware IDB processing chain for one last change to be
     * handled. If the itemlist gets all the way down to the
     * bottom of the hardware chain, then there is no hope to handle
     * the items which weren't handled here.
     */
    if (!ret) {
	itemlist_reset(ilist);
	ret = (*idb->hwptr->state_config)(idb->hwptr, opcode, ilist);
    }
    return (ret);
}
#endif /* NOBODY_CALLS_ME */


/*
 * Interface encapsulation and IO functions.
 */
boolean
idb_pak_vencap (paktype *pak, long address)
{
    if (!(pak && pak->if_output && pak->if_output->hwptr))
	return (FALSE);

    return ((*pak->if_output->hwptr->vencap)(pak, address));
}

uchar *
idb_bridge_vencap (paktype *pak, long address)
{
    if (!(pak && pak->if_output && pak->if_output->hwptr))
	return (FALSE);

    return ((*pak->if_output->hwptr->bridge_vencap)(pak, address));
}
    
boolean
idb_board_encap (paktype *pak, hwidbtype *hwidb)
{
    if (!(pak && pak->if_output && pak->if_output->hwptr))
	return (FALSE);

    return ((*pak->if_output->hwptr->board_encap)(pak, hwidb));
}

void
idb_deencap (hwidbtype *hwidb)
{
    if (hwidb == NULL)
	return;

    (*hwidb->deencap)(hwidb);
}

/*
 * IDB IO Functions.
 */
void
idb_start_output (hwidbtype *hwidb)
{
    if (hwidb == NULL)
	return;

    (*hwidb->soutput)(hwidb);
}

void
idb_start_input (hwidbtype *hwidb)
{
    if (hwidb == NULL)
	return;

    (*hwidb->sinput)(hwidb);
}

void
idb_queue_for_output (hwidbtype *hwidb, paktype *pak, enum HEADTAIL which)
{
    if (!(hwidb && pak))
	return;

    (*hwidb->oqueue)(hwidb, pak, which);
}

void
idb_rx_char_int (hwidbtype *hwidb, uchar input, boolean overrun)
{
    if (hwidb == NULL)
	return;

    (*hwidb->rx_char_int)(hwidb, input, overrun);
}

/*
 * Determine if an interface's address is Token Ring format or not
 */

const boolean idb_is_tokenring_like (hwidbtype *idb)
{
    if (idb == NULL)
	return(FALSE);

    if ( is_cip_lan(idb) ) {
	switch(channel_sb_get_emulated_media(idb)) {

        default:
            return(FALSE);

        case CIP_VLAN_TOKEN:
        case CIP_VLAN_FDDI:
            return(TRUE);

	} /* End switch() */
    }  /* End if() */

    if (is_fddi(idb) && (idb->type != IDBTYPE_FDDIT) )
	return(TRUE);

    if (is_tokenring(idb))
	return(TRUE);

    return(FALSE);
}
