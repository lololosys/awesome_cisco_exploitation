/* $Id: clns_media.c,v 3.3.60.12 1996/09/09 22:43:30 wilber Exp $
 * $Source: /release/112/cvs/Xsys/clns/clns_media.c,v $
 *------------------------------------------------------------------
 * clns_media.c -- media dependent CLNS routines.
 *
 * 13-April-1993, Kirk Lougheed
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: clns_media.c,v $
 * Revision 3.3.60.12  1996/09/09  22:43:30  wilber
 * CSCdi67888: CLNS routes failed to come up over token when bridging is
 * configured
 * Add CLNS multicast addresses into the SMF of bridged token interface
 * Branch: California_branch
 *
 * Revision 3.3.60.11  1996/08/08  14:50:22  dkatz
 * CSCdi65076:  ISIS/NLSP uses old scheduler primitives
 * Branch: California_branch
 * Use new scheduler primitives.  Get rid of gratuitous type coercion.
 *
 * Revision 3.3.60.10  1996/08/07  08:57:03  snyder
 * CSCdi65343:  more things that should be declared const
 *              char * [] -> char *const []
 *              char *    -> const char []
 *              savings 17,496 data space, 1,852 image space
 * Branch: California_branch
 *
 * Revision 3.3.60.9  1996/07/30  14:45:48  michellw
 * CSCdi64226:  Support larger max_frame_size (>1516) for LANE
 * Branch: California_branch
 * Add one parameter (swidb) to reg_invoke_media_change_mtu registry
 * function.
 *
 * Revision 3.3.60.8  1996/07/26  18:13:59  tylin
 * CSCdi59808:  clns ping fails between 7000 ethernet interfaces
 *    Fix dialer i/f checking by calling is_ddr().
 *    Update driver's filter table for all ES/IS multicast addresses
 *    whenever enable/disable CLNS routing.
 * Branch: California_branch
 *
 * Revision 3.3.60.7  1996/07/23  18:43:27  ppearce
 * CSCdi63747:  Move srb_multiring out of SRB subblock
 * Branch: California_branch
 *   (1) srb_multiring doesn't really belong exclusively to SRB
 *         move srb_multiring out of SRB subblock and into swidb
 *         provide srb_multiring accessor routines
 *   (2) determine_rif() should take swidb as argument rather than hwidb
 *
 * Revision 3.3.60.6  1996/07/18  18:14:58  tylin
 * CSCdi62693:  Command cdp neighbors detail generates alignment
 * corrections (IPX)
 *    Fix clns_sprintf_address().
 * Branch: California_branch
 *
 * Revision 3.3.60.5  1996/07/11  18:29:19  asastry
 * CSCdi60438:  CLNS fails to work over DDR unless dialer strings are
 * used. Use next hop NSAP to do the dialer look-up.
 *
 * Branch: California_branch
 *
 * Revision 3.3.60.4  1996/05/09  14:12:07  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.3.60.3  1996/05/04  00:35:34  wilber
 * Branch: California_branch
 *
 * Integrated Routing and Bridging
 * - Bridge-group Virtual Interface smf support.
 * - Set up bridge/route indicators based on CRB/IRB is enabled or not.
 *
 * Revision 3.3.60.2.2.1  1996/04/30  20:49:17  rbadri
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 * IS-IS support for LANE.
 *
 * Revision 3.3.60.2  1996/04/25  23:03:20  ronnie
 * CSCdi55766:  Hey!  This ATM and Frame Relay is no different than in 11.1
 * Feature commit for ATM and Frame Relay into 11.2
 * Branch: California_branch
 *
 * Revision 3.3.60.1.2.1  1996/03/24  03:46:27  rzagst
 * ATM modularity: Remove AIP from all files that are not referring to the
 * AIP i/f
 * Branch: ATM_Cal_branch
 *
 * Revision 3.3.60.1  1996/03/18  19:02:46  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.26.3  1996/03/13  01:12:58  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.3.26.2  1996/03/07  08:40:04  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.3.26.1  1996/02/20  13:40:10  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.3  1995/11/17  08:52:18  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:12:17  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:07:47  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/08/07  05:29:22  richl
 * CSCdi38315:  makefile rework
 *              add rif_util.c to libutil.a
 *
 * Revision 2.1  1995/06/07  20:17:23  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <string.h>
#include "sys_registry.h"
#include "media_registry.h"
#include "interface_private.h"
#include "packet.h"
#include "clns_externs.h"
#include "../if/static_map.h"
#include "../if/network.h"
#include "../if/rif_inline.h"
#include "../wan/serial.h"
#include "../x25/x25.h"
#include "../wan/frame_relay.h"
#include "../smf/smf_public.h"
#include "../srt/srt_registry.h"


/*
 *
 */
static const uchar clns_llc[SAP_HDRBYTES] = {SAP_CLNS, SAP_CLNS, LLC1_UI};


/*
 * clns_build_address
 *
 * Get a CLNS address if we have one.
 * Look for a global NET first; if we don't find one, look for a PDB
 * NET. If that also fails, we'll settle for interface NET. This is primarily
 * used by CDP.
 */
static void clns_build_address (
    idbtype *swidb,
    paktype *pak,
    addrtype *addr)
{
    clns_local_nettype *local_net, *global_net, *pdb_net, *idb_net;
    clns_ddbtype *ddb;

    memset(addr, 0, sizeof(addrtype));
    addr->type = ADDR_CLNS;

    global_net = pdb_net = idb_net = NULL;
    for (local_net = clns_local_nets.qhead;
	 local_net; local_net = local_net->next) {
	    
	/*
	 * We prefer to send a global NET, if one exists. If we don't
	 * get one, we will settle for a PDB NET, or else an
	 * interface NET.
	 */
	ddb = nsap_lookup(local_net->net, MATCH_EXACT);
	if (ddb) {
	    if (ddb->entry_type == CLNS_OURNSAP_ENTRY) {
		global_net = local_net;
		break;
	    }
	    else if (ddb->entry_type == CLNS_OURNET_ENTRY) {
		idb_net = local_net;
		continue;
	    }
	}
	else if (net_equ(local_net->net, swidb->clns_net)) {
	    idb_net = local_net;
	    continue;
	}
	else {
	    pdb_net = local_net;
	    continue;
	}
    }

    /* If no NET was found, return FALSE ... */
    if (!global_net && !pdb_net && !idb_net) 
        return;

    if (global_net)
        nsap_copy(global_net->net, addr->clns_addr);
    else if (pdb_net)
	nsap_copy(pdb_net->net, addr->clns_addr);
    else if (idb_net)
	nsap_copy(idb_net->net, addr->clns_addr);

    /* Include length of NSAP length byte */
    addr->length = addr->clns_addr[0] + 1;
    return;
}

/*
 * clns_sprintf_address
 *
 * Take an addrtype data structure and print it an an CLNS address.
 */
static void clns_sprintf_address (
    char *string,
    addrtype *addr)
{
    sprintf(string, "%s", nsap_string(addr->clns_addr));
}

/*
 * clns_sprintf_src_dst
 * Given a packet, extract and print a CLNS address.
 */
static void clns_sprintf_src_dst (
    char *string, 
    paktype *pak,
    enum SRCDST which)
{
    if (which == SOURCE)
	sprintf(string, "%n", (nsap_addr *) pak->clns_srcaddr);
    else
	sprintf(string, "%n", (nsap_addr *) PAK2ADDR(pak));
}

/*
 * clns_macaddrs_update
 *
 * Called (via reg_invoke_media_interesting_addresses) when a
 * change in the listening state of an interface has taken place.
 * The interface is listening either selectively or promiscuously.
 *
 * If this protocol is configured on the interface, update the SMF
 * so that the protocol-specific MAC addresses are either inserted
 * (the interface is listening promiscuously) or deleted (the
 * interface is listening selectively).  In addition, populate
 * any hardware address filters with the protocol-specific MAC
 * addresses.
 *
 * If this protocol is NOT configured on the interface, simply
 * delete the protocol-specific MAC addresses from the SMF if
 * they are present.
 */
void clns_macaddrs_update (idbtype *idb)
{
    hwidbtype *hwidb;
    uchar clns_multi_swap[IEEEBYTES];
    hwidb = idb->hwptr;

    if (clns_on_idb(idb)) {
	if (is_bvi(hwidb)) {
	    reg_invoke_bvi_smf_update(esis_8023_all_es_multi,
				      idb,
				      SMF_MACTYP_CLNS_MCAST |
				      SMF_ACTION_RECEIVE,
				      IDB_BRIDGE);
	    reg_invoke_bvi_smf_update(esis_8023_all_is_multi,
				      idb,
				      SMF_MACTYP_CLNS_MCAST |
				      SMF_ACTION_RECEIVE,
				      IDB_BRIDGE);
	    reg_invoke_bvi_smf_update(isis_all_l1_is_multi,
				      idb,
				      SMF_MACTYP_CLNS_MCAST |
				      SMF_ACTION_RECEIVE,
				      IDB_BRIDGE);
	    reg_invoke_bvi_smf_update(isis_all_l2_is_multi,
				      idb,
				      SMF_MACTYP_CLNS_MCAST |
				      SMF_ACTION_RECEIVE,
				      IDB_BRIDGE);

	    ieee_swap(esis_8025_all_es_multi, clns_multi_swap);
	    reg_invoke_bvi_smf_update(clns_multi_swap,
				      idb,
				      SMF_MACTYP_CLNS_MCAST |
				      SMF_ACTION_RECEIVE,
				      IDB_TR);
	    ieee_swap(esis_8025_all_is_multi, clns_multi_swap);
	    reg_invoke_bvi_smf_update(clns_multi_swap,
				      idb,
				      SMF_MACTYP_CLNS_MCAST |
				      SMF_ACTION_RECEIVE,
				      IDB_TR);
	    ieee_swap(isis_8025_all_is_multi, clns_multi_swap);
	    reg_invoke_bvi_smf_update(clns_multi_swap,
				      idb,
				      SMF_MACTYP_CLNS_MCAST |
				      SMF_ACTION_RECEIVE,
				      IDB_TR);
	    return;
	}

	if (idb_is_tokenring(idb)) {
	    ieee_swap(esis_8025_all_es_multi, clns_multi_swap);
	    reg_invoke_tbridge_smf_update(clns_multi_swap,
					  idb,
					  SMF_MACTYP_CLNS_MCAST |
					  SMF_ACTION_RECEIVE);
	    ieee_swap(esis_8025_all_is_multi, clns_multi_swap);
	    reg_invoke_tbridge_smf_update(clns_multi_swap,
					  idb,
					  SMF_MACTYP_CLNS_MCAST |
					  SMF_ACTION_RECEIVE);
	    ieee_swap(isis_8025_all_is_multi, clns_multi_swap);
	    reg_invoke_tbridge_smf_update(clns_multi_swap,
					  idb,
					  SMF_MACTYP_CLNS_MCAST |
					  SMF_ACTION_RECEIVE);
	} else {
	    reg_invoke_tbridge_smf_update(esis_8023_all_es_multi,
					  idb,
					  SMF_MACTYP_CLNS_MCAST |
					  SMF_ACTION_RECEIVE);
	    reg_invoke_tbridge_smf_update(esis_8023_all_is_multi,
					  idb,
					  SMF_MACTYP_CLNS_MCAST |
					  SMF_ACTION_RECEIVE);
	    reg_invoke_tbridge_smf_update(isis_all_l1_is_multi,
					  idb,
					  SMF_MACTYP_CLNS_MCAST |
					  SMF_ACTION_RECEIVE);
	    reg_invoke_tbridge_smf_update(isis_all_l2_is_multi,
					  idb,
					  SMF_MACTYP_CLNS_MCAST |
					  SMF_ACTION_RECEIVE);
	}
	if_multi_add(hwidb, idb, esis_8023_all_es_multi);
	if_multi_add(hwidb, idb, esis_8023_all_is_multi);
	if_multi_add(hwidb, idb, isis_all_l1_is_multi);
	if_multi_add(hwidb, idb, isis_all_l2_is_multi);
    } else {
	if (is_bvi(hwidb)) {
	    reg_invoke_bvi_smf_delete(esis_8023_all_es_multi,
				      idb,
				      SMF_MACTYP_CLNS_MCAST |
				      SMF_ACTION_RECEIVE,
				      IDB_BRIDGE);
	    reg_invoke_bvi_smf_delete(esis_8023_all_is_multi,
				      idb,
				      SMF_MACTYP_CLNS_MCAST |
				      SMF_ACTION_RECEIVE,
				      IDB_BRIDGE);
	    reg_invoke_bvi_smf_delete(isis_all_l1_is_multi,
				      idb,
				      SMF_MACTYP_CLNS_MCAST |
				      SMF_ACTION_RECEIVE,
				      IDB_BRIDGE);
	    reg_invoke_bvi_smf_delete(isis_all_l2_is_multi,
				      idb,
				      SMF_MACTYP_CLNS_MCAST |
				      SMF_ACTION_RECEIVE,
				      IDB_BRIDGE);
	    ieee_swap(esis_8025_all_es_multi, clns_multi_swap);
	    reg_invoke_bvi_smf_delete(clns_multi_swap,
				      idb,
				      SMF_MACTYP_CLNS_MCAST |
				      SMF_ACTION_RECEIVE,
				      IDB_TR);
	    ieee_swap(esis_8025_all_is_multi, clns_multi_swap);
	    reg_invoke_bvi_smf_delete(clns_multi_swap,
				      idb,
				      SMF_MACTYP_CLNS_MCAST |
				      SMF_ACTION_RECEIVE,
				      IDB_TR);
	    ieee_swap(isis_8025_all_is_multi, clns_multi_swap);
	    reg_invoke_bvi_smf_delete(clns_multi_swap,
				      idb,
				      SMF_MACTYP_CLNS_MCAST |
				      SMF_ACTION_RECEIVE,
				      IDB_TR);
	    return;
	}

	if (idb_is_tokenring(idb)) {
	    ieee_swap(esis_8025_all_es_multi, clns_multi_swap);
	    reg_invoke_tbridge_smf_delete(clns_multi_swap,
					  idb,
					  SMF_MACTYP_CLNS_MCAST |
					  SMF_ACTION_RECEIVE);
	    ieee_swap(esis_8025_all_is_multi, clns_multi_swap);
	    reg_invoke_tbridge_smf_delete(clns_multi_swap,
					  idb,
					  SMF_MACTYP_CLNS_MCAST |
					  SMF_ACTION_RECEIVE);
	    ieee_swap(isis_8025_all_is_multi, clns_multi_swap);
	    reg_invoke_tbridge_smf_delete(clns_multi_swap,
					  idb,
					  SMF_MACTYP_CLNS_MCAST |
					  SMF_ACTION_RECEIVE);
	} else {
	    reg_invoke_tbridge_smf_delete(esis_8023_all_es_multi,
					  idb,
					  SMF_MACTYP_CLNS_MCAST |
					  SMF_ACTION_RECEIVE);
	    reg_invoke_tbridge_smf_delete(esis_8023_all_is_multi,
					  idb,
					  SMF_MACTYP_CLNS_MCAST |
					  SMF_ACTION_RECEIVE);
	    reg_invoke_tbridge_smf_delete(isis_all_l1_is_multi,
					  idb,
					  SMF_MACTYP_CLNS_MCAST |
					  SMF_ACTION_RECEIVE);
	    reg_invoke_tbridge_smf_delete(isis_all_l2_is_multi,
					  idb,
					  SMF_MACTYP_CLNS_MCAST |
					  SMF_ACTION_RECEIVE);
	}
    }
}

/*
 * clns_ether_macaddr
 *
 * Turn a CLNS address into an encapsulation type and a MAC address
 * Called by MEDIA_ETHER_MACADDR, index is LINK_CLNS
 */
static boolean clns_ether_macaddr (paktype *pak, long address, int *enctype,
				   uchar **daddr, uchar const **other_info)
{
    *daddr = (uchar *)address;
    *enctype = ET_SAP;
    *other_info = clns_llc;
    return(TRUE);
}

/*
 * clns_all_es_ether_macaddr
 *
 * Turn a CLNS address into an encapsulation type and a MAC address
 * Called by MEDIA_ETHER_MACADDR, index is LINK_CLNS_ALL_ES
 */
static boolean clns_all_es_ether_macaddr (paktype *pak, long address,
					  int *enctype, uchar **daddr,
					  uchar const **other_info)
{
    *daddr = esis_8023_all_es_multi;
    *enctype = ET_SAP;
    *other_info = clns_llc;
    return(TRUE);
}

/*
 * clns_all_is_ether_macaddr
 *
 * Turn a CLNS address into an encapsulation type and a MAC address
 * Called by MEDIA_ETHER_MACADDR, index is LINK_CLNS_ALL_IS
 */
static boolean clns_all_is_ether_macaddr (paktype *pak, long address,
					  int *enctype, uchar **daddr,
					  uchar const **other_info)
{
    *daddr = esis_8023_all_is_multi;
    *enctype = ET_SAP;
    *other_info = clns_llc;
    return(TRUE);
}

/*
 * clns_bcast_ether_macaddr
 *
 * Turn a CLNS address into an encapsulation type and a MAC address
 * Called by MEDIA_ETHER_MACADDR, index is LINK_CLNS_BCAST
 */
static boolean clns_bcast_ether_macaddr (paktype *pak, long address,
					 int *enctype, uchar **daddr,
					 uchar const **other_info)
{
    *daddr = (uchar *)baddr;
    *enctype = ET_SAP;
    *other_info = clns_llc;
    return(TRUE);
}

/*
 * clns_isis_all_l1_ether_macaddr
 *
 * Turn a CLNS address into an encapsulation type and a MAC address
 * Called by MEDIA_ETHER_MACADDR, index is LINK_ISIS_ALL_L1_IS
 */
static boolean clns_isis_all_l1_ether_macaddr (paktype *pak, long address,
					       int *enctype, uchar **daddr,
					       uchar const **other_info)
{
    *daddr = isis_all_l1_is_multi;
    *enctype = ET_SAP;
    *other_info = clns_llc;
    return(TRUE);
}

/*
 * clns_isis_all_l2_ether_macaddr
 *
 * Turn a CLNS address into an encapsulation type and a MAC address
 * Called by MEDIA_ETHER_MACADDR, index is LINK_ISIS_ALL_L2_IS
 */
static boolean clns_isis_all_l2_ether_macaddr (paktype *pak, long address,
					       int *enctype, uchar **daddr,
					       uchar const **other_info)
{
    *daddr = isis_all_l2_is_multi;
    *enctype = ET_SAP;
    *other_info = clns_llc;
    return(TRUE);
}

/*
 * clns_token_macaddr
 *
 * Turn a CLNS address into an encapsulation type and a MAC address
 * Called by MEDIA_TOKEN_MACADDR, index is LINK_CLNS
 */
static boolean clns_token_macaddr (paktype *pak, long address, int *enctype,
				   uchar **daddr, uchar const **other_info,
				   ulong *rif_flags)
{
    idbtype *interface;

    interface = pak->if_output;
    *daddr = (uchar *)address;
    *enctype = ET_SAP;
    *other_info = clns_llc;
    *rif_flags = determine_rif(interface, MULTIRING_BIT_CLNS);
    return(TRUE);
}

/*
 * clns_all_es_token_macaddr
 *
 * Turn a CLNS address into an encapsulation type and a MAC address
 * Called by MEDIA_TOKEN_MACADDR, index is LINK_CLNS_ALL_ES
 */
static boolean clns_all_es_token_macaddr (paktype *pak, long address,
					  int *enctype, uchar **daddr,
					  uchar const **other_info,
					  ulong *rif_flags)
{
    idbtype *interface;

    interface = pak->if_output;
    *daddr = esis_8025_all_es_multi;
    *enctype = ET_SAP;
    *other_info = clns_llc;
    *rif_flags = determine_rif(interface, MULTIRING_BIT_CLNS);
    return(TRUE);
}

/*
 * clns_all_is_token_macaddr
 *
 * Turn a CLNS address into an encapsulation type and a MAC address
 * Called by MEDIA_TOKEN_MACADDR, index is LINK_CLNS_ALL_IS
 */
static boolean clns_all_is_token_macaddr (paktype *pak, long address,
					  int *enctype, uchar **daddr,
					  uchar const **other_info,
					  ulong *rif_flags)
{
    idbtype *interface;

    interface = pak->if_output;
    *daddr = esis_8025_all_is_multi;
    *enctype = ET_SAP;
    *other_info = clns_llc;
    *rif_flags = determine_rif(interface, MULTIRING_BIT_CLNS);
    return(TRUE);
}

/*
 * clns_bcast_token_macaddr
 *
 * Turn a CLNS address into an encapsulation type and a MAC address
 * Called by MEDIA_TOKEN_MACADDR, index is LINK_CLNS_BCAST
 */
static boolean clns_bcast_token_macaddr (paktype *pak, long address,
					 int *enctype, uchar **daddr,
					 uchar const **other_info,
					 ulong *rif_flags)
{
    idbtype *interface;

    interface = pak->if_output;
    *daddr = (uchar *)trbaddr1;
    *enctype = ET_SAP;
    *other_info = clns_llc;
    *rif_flags = determine_rif(interface, MULTIRING_BIT_CLNS);
    return(TRUE);
}

/*
 * clns_isis_all_l1_token_macaddr
 *
 * Turn a CLNS address into an encapsulation type and a MAC address
 * Called by MEDIA_TOKEN_MACADDR, index is LINK_CLNS
 */
static boolean clns_isis_all_l1_l2_token_macaddr (paktype *pak, long address,
						  int *enctype, uchar **daddr,
						  uchar const **other_info,
						  ulong *rif_flags)
{
    idbtype *interface;

    interface = pak->if_output;
    *daddr = isis_8025_all_is_multi;
    *enctype = ET_SAP;
    *other_info = clns_llc;
    *rif_flags = determine_rif(interface, MULTIRING_BIT_CLNS);
    return(TRUE);
}

/*
 * clns_fddi_macaddr
 *
 * Turn a CLNS address into an encapsulation type and a MAC address
 * Called by MEDIA_FDDI_MACADDR, index is LINK_CLNS
 */
static boolean clns_fddi_macaddr (paktype *pak, long address, int *enctype,
				  uchar **daddr, uchar const **other_info,
				  ulong *rif_flags)
{
    idbtype *interface;

    interface = pak->if_output;

    *daddr = (uchar *)address;
    *enctype = ET_SAP;
    *other_info = clns_llc;
    *rif_flags = determine_rif(interface, MULTIRING_BIT_CLNS);
    return(TRUE);
}

/*
 * clns_all_es_fddi_macaddr
 *
 * Turn a CLNS address into an encapsulation type and a MAC address
 * Called by MEDIA_FDDI_MACADDR, index is LINK_CLNS_ALL_ES
 */
static boolean clns_all_es_fddi_macaddr (paktype *pak, long address,
					  int *enctype, uchar **daddr,
					 uchar const **other_info,
					 ulong *rif_flags)
{
    idbtype *interface;

    interface = pak->if_output;

    *daddr = esis_8023_all_es_multi;
    *enctype = ET_SAP;
    *other_info = clns_llc;
    *rif_flags = determine_rif(interface, MULTIRING_BIT_CLNS);
    return(TRUE);
}

/*
 * clns_all_is_fddi_macaddr
 *
 * Turn a CLNS address into an encapsulation type and a MAC address
 * Called by MEDIA_FDDI_MACADDR, index is LINK_CLNS_ALL_IS
 */
static boolean clns_all_is_fddi_macaddr (paktype *pak, long address,
					  int *enctype, uchar **daddr,
					  uchar const **other_info,
					  ulong *rif_flags)
{
    idbtype *interface;

    interface = pak->if_output;

    *daddr = esis_8023_all_is_multi;
    *enctype = ET_SAP;
    *other_info = clns_llc;
    *rif_flags = determine_rif(interface, MULTIRING_BIT_CLNS);
    return(TRUE);
}

/*
 * clns_bcast_fddi_macaddr
 *
 * Turn a CLNS address into an encapsulation type and a MAC address
 * Called by MEDIA_FDDI_MACADDR, index is LINK_CLNS_BCAST
 */
static boolean clns_bcast_fddi_macaddr (paktype *pak, long address,
					 int *enctype, uchar **daddr,
					 uchar const **other_info,
					 ulong *rif_flags)
{
    idbtype *interface;

    interface = pak->if_output;

    *daddr = (uchar *)baddr;
    *enctype = ET_SAP;
    *other_info = clns_llc;
    *rif_flags = determine_rif(interface, MULTIRING_BIT_CLNS);
    return(TRUE);
}

/*
 * clns_isis_all_l1_fddi_macaddr
 *
 * Turn a CLNS address into an encapsulation type and a MAC address
 * Called by MEDIA_FDDI_MACADDR, index is LINK_ISIS_ALL_L1_IS
 */
static boolean clns_isis_all_l1_fddi_macaddr (paktype *pak, long address,
					       int *enctype, uchar **daddr,
					       uchar const **other_info,
					       ulong *rif_flags)
{
    idbtype *interface;

    interface = pak->if_output;

    *daddr = isis_all_l1_is_multi;
    *enctype = ET_SAP;
    *other_info = clns_llc;
    *rif_flags = determine_rif(interface, MULTIRING_BIT_CLNS);
    return(TRUE);
}

/*
 * clns_isis_all_l2_fddi_macaddr
 *
 * Turn a CLNS address into an encapsulation type and a MAC address
 * Called by MEDIA_FDDI_MACADDR, index is LINK_ISIS_ALL_L2_IS
 */
static boolean clns_isis_all_l2_fddi_macaddr (paktype *pak, long address,
					      int *enctype, uchar **daddr,
					      uchar const **other_info,
					      ulong *rif_flags)
{
    idbtype *interface;

    interface = pak->if_output;

    *daddr = isis_all_l2_is_multi;
    *enctype = ET_SAP;
    *other_info = clns_llc;
    *rif_flags = determine_rif(interface, MULTIRING_BIT_CLNS);
    return(TRUE);
}

/*
 * clns_hdlc_macaddr
 *
 * Turn an IP address into an encapsuclnsion type and a MAC address
 * Called by MEDIA_HDLC_MACADDR, indexed by LINK_CLNS
 */
static boolean clns_hdlc_macaddr (paktype *pak, long address,
				  int *enctype, ushort *flags)
{
    if (pak->linktype != LINK_CLNS)
	*flags |= HDLC_BCAST;
    *enctype = ET_HDLC;

    /*
     * WARNING!
     *
     * The following code inserts a padding byte between the end of the 4
     * bytes of HDLC encapsulation and the start of the CLNS header (the PID
     * byte). This is for backwards compatibility with older versions of
     * cisco code. If none of this makes sense, ask a grown-up.
     */
    pak->datagramstart--;
    pak->datagramsize++;
    pak->encsize++;

    return(TRUE);
}

/*
 * clns_smds_macaddr
 *
 * Turn an CLNS address into an encapsulation type and a MAC address
 * Called by MEDIA_SMDS_MACADDR, indexed by LINK_CLNS.
 */
static boolean clns_smds_macaddr (paktype *pak, long address, int *enctype,
				  uchar const **daddr,
				  uchar const **other_info,
				  addrtype *proto_addr)
{
    switch (pak->linktype) {
      case LINK_CLNS:
	*daddr = (uchar *)address;
	break;

      case LINK_CLNS_BCAST:
	*daddr = baddr;
	break;

      case LINK_CLNS_ALL_ES:
      case LINK_CLNS_ALL_IS:
	pak->linktype = LINK_CLNS_ALL_ES;	/* change for map lookups */
	*daddr = baddr;
	break;

      case LINK_ISIS_ALL_L1_IS:
      case LINK_ISIS_ALL_L2_IS:
	pak->linktype = LINK_CLNS_ALL_IS;	/* change for map lookups */
	*daddr = baddr;
	break;

      default:
	return(FALSE);
    }
    *enctype = ET_SAP;
    *other_info = clns_llc;
    return(TRUE);
}

/*
 * clns_x25_macaddr
 *
 * Turn an CLNS address into a broadcast flag, or a filled in addrtype
 * structure that will be used to look up a static mapping.
 * Called by MEDIA_X25_MACADDR, indexed by LINK_CLNS.
 */
static boolean clns_x25_macaddr (paktype *pak, long address,
				 addrtype *proto_addr, lcitype **lci,
				 boolean *broadcast)
{
    if (pak->linktype == LINK_CLNS) {
	bcopy((char *)address, proto_addr->x121_addr, ADDRLEN_X121);
	proto_addr->type = ADDR_X121;
	proto_addr->length = strlen(proto_addr->x121_addr);;
	*broadcast = FALSE;
    } else {
	*broadcast = TRUE;
    }
    return(TRUE);
}

/*
 * clns_frame_macaddr
 *
 * Turn an CLNS address into a broadcast flag, or a filled in addrtype
 * structure that will be used to look up a static mapping.
 * Called by MEDIA_FRAME_MACADDR, indexed by LINK_CLNS.
 */
static boolean clns_frame_macaddr (paktype *pak, long address,
				   addrtype *proto_addr, boolean *broadcast)
{
    if (pak->linktype == LINK_CLNS) {
	proto_addr->type = 0;
	proto_addr->length = 0;
	bcopy((char *)address, &proto_addr->addr.fr_address,
	      sizeof(proto_addr->addr.fr_address));
	*broadcast = FALSE;
    } else {
	pak->linktype = LINK_CLNS;
	*broadcast = TRUE;
    }

    return(TRUE);
}

/*
 * clns_ddr_macaddr
 *
 * Turn an CLNS address into a broadcast flag, or a filled in addrtype
 * structure that will be used to look up a static mapping.
 * Called by MEDIA_DDR_MACADDR, indexed by LINK_CLNS.
 */
static boolean clns_ddr_macaddr (paktype *pak, long address,
				   addrtype *proto_addr, boolean *broadcast)
{
    uchar *nsap_ptr;

    if (pak->linktype == LINK_CLNS) {
	if (!address)
	    return(FALSE);

	nsap_ptr = (uchar *)address;
	proto_addr->type = ADDR_CLNS;
	proto_addr->length = nsap_ptr[0];
	nsap_copy(nsap_ptr, proto_addr->clns_addr);
	*broadcast = FALSE;
    } else {
	*broadcast = TRUE;
    }
    pak->dialer_save_type = LINK_CLNS;
    return(TRUE);
}

/*
 * clns_atm_macaddr
 *
 * Turn an CLNS address into a broadcast flag, or a filled in addrtype
 * structure that will be used to look up a static mapping.
 * Called by MEDIA_ATM_MACADDR, indexed by LINK_CLNS.
 */
static boolean clns_atm_macaddr (paktype *pak, long address, 
				   hwaddrtype *daddr, uchar const **snap_info,
				   staticmap_type **map, boolean *broadcast)
{
    addrtype proto_addr;

    memset(daddr, 0, sizeof(hwaddrtype));
    *broadcast = FALSE;
    *snap_info = zerosnap;
    memset(&proto_addr, 0, sizeof(addrtype));
    if (pak->linktype == LINK_CLNS) {
	proto_addr.type = ADDR_CLNS;
	if (pak->clns_dstaddr) {
	  proto_addr.length = pak->clns_dstaddr[0];
	  bcopy((char *)pak->clns_dstaddr, proto_addr.clns_addr, 
		STATIONLEN_MAX);
	}
    } else {
        pak->linktype = LINK_CLNS;
        *broadcast = TRUE;
    }
    *map = staticmap_lookup(pak->if_output, &proto_addr, LINK_CLNS, ET_ATM, 
								*broadcast);
    if((*map) == NULL && *broadcast == FALSE) {
	daddr->type = STATION_ATMVC;
	daddr->length = STATIONLEN_SMDS64;
	bcopy((uchar *)address, daddr->addr, daddr->length);
    }
    return(TRUE);
}

/*
 * clns_compute_mtu
 *
 * Always ISO1 encapsulation.
 */
static int clns_compute_mtu (idbtype *swidb, boolean get_default)
{
    return(swidb->sub_mtu - ET_SAP_LEN);
}

/*
 * clns_compute_bias_mtu
 *
 * Always ISO1 encapsulation.  Correct for the fact that the
 * interface MTU is biased for ISO2 encapsulation.
 */
static int clns_compute_biased_mtu (idbtype *swidb, boolean get_default)
{
    return(swidb->hwptr->maxmtu + ET_SNAP_LEN - ET_SAP_LEN);
}

/*
 * clns_change_mtu
 *
 * A user has configured a different physical MTU size for this
 * interface.  Adjust the CLNS MTU size to make sure that it is still
 * valid.
 */
static void clns_change_mtu (hwidbtype *hwidb, idbtype *swidb, int delta)
{
    idbtype *idb;

    /*
     * If the mtu is being adjusted upward and the CLNS mtu was
     * previously equal to the maxmtu then adjust it upward as well.
     * This handles the case where the nvram has only an mtu command that
     * adjusts the mtu skyward.  If the CLNS mtu is the same as the max
     * then it wont show up in the nvram at all since max is the
     * defaults.
     */
    if (delta > 0) {
	FOR_ALL_SWIDBS_ON_HW(hwidb, idb) {
	    if (def_proto_mtu(idb, LINK_CLNS) == (idb->clns_mtu + delta))
		idb->clns_mtu += delta;
	}
    }

    /*
     * Don't let the max go lower than the CLNS mtu.
     */
    FOR_ALL_SWIDBS_ON_HW(hwidb, idb) {
	if (max_proto_mtu(idb, LINK_CLNS) < idb->clns_mtu)
	    idb->clns_mtu = max_proto_mtu(idb, LINK_CLNS);
    }
}

/*
 * clns_set_default_mtu
 *
 * Set the default MTU value for an interface.  This routine is called
 * when an interface changes its physical MTU size.  For example, when a
 * token ring interface changes from 16M to 4M.
 */
static void clns_set_default_mtu (hwidbtype *hwidb)
{
    idbtype *idb;

    FOR_ALL_SWIDBS_ON_HW(hwidb, idb) {
	idb->clns_mtu = def_proto_mtu(idb, LINK_CLNS);
    }
}

/*
 * clns_media_registry
 *
 * Register media dependent code for CLNS
 */
void clns_media_registry (void)
{
    reg_add_proto_address(LINK_CLNS, clns_build_address, "clns_build_address");
    reg_add_proto_address(LINK_CLNS_BCAST,
		      clns_build_address, "clns_build_address");
    reg_add_proto_address(LINK_CLNS_ALL_ES,
		      clns_build_address, "clns_build_address");
    reg_add_proto_address(LINK_CLNS_ALL_IS,
		      clns_build_address, "clns_build_address");
    reg_add_sprintf_address_link(LINK_CLNS,
		      clns_sprintf_address, "clns_sprintf_address");
    reg_add_sprintf_address_addr(ADDR_CLNS,
		      clns_sprintf_address, "clns_sprintf_address");

    reg_add_sprintf_address_link(LINK_CMNS,
		      clns_sprintf_address, "clns_sprintf_address");
    reg_add_sprintf_address_addr(ADDR_CMNS,
		      clns_sprintf_address, "clns_sprintf_address");

    reg_add_sprintf_src_dst(LINK_CLNS, clns_sprintf_src_dst, 
			    "clns_sprintf_src_dst");
    reg_add_access_check(LINK_CLNS, clns_accesscheck, "clns_accesscheck");
    reg_add_media_interesting_addresses(clns_macaddrs_update,
					"clns_macaddrs_update");

    reg_add_media_ether_macaddr(LINK_CLNS, clns_ether_macaddr,
				"clns_ether_macaddr");
    reg_add_media_ether_macaddr(LINK_CLNS_ALL_ES, clns_all_es_ether_macaddr,
				"clns_all_es_ether_macaddr");
    reg_add_media_ether_macaddr(LINK_CLNS_ALL_IS, clns_all_is_ether_macaddr,
				"clns_all_is_ether_macaddr");
    reg_add_media_ether_macaddr(LINK_CLNS_BCAST, clns_bcast_ether_macaddr,
				"clns_bcast_ether_macaddr");
    reg_add_media_ether_macaddr(LINK_ISIS_ALL_L1_IS,
				clns_isis_all_l1_ether_macaddr,
				"clns_isis_all_l1_ether_macaddr");
    reg_add_media_ether_macaddr(LINK_ISIS_ALL_L2_IS,
				clns_isis_all_l2_ether_macaddr,
				"clns_isis_all_l2_ether_macaddr");

    reg_add_media_token_macaddr(LINK_CLNS, clns_token_macaddr,
				"clns_token_macaddr");
    reg_add_media_token_macaddr(LINK_CLNS_ALL_ES, clns_all_es_token_macaddr,
				"clns_all_es_token_macaddr");
    reg_add_media_token_macaddr(LINK_CLNS_ALL_IS, clns_all_is_token_macaddr,
				"clns_all_is_token_macaddr");
    reg_add_media_token_macaddr(LINK_CLNS_BCAST, clns_bcast_token_macaddr,
				"clns_bcast_token_macaddr");
    reg_add_media_token_macaddr(LINK_ISIS_ALL_L1_IS,
				clns_isis_all_l1_l2_token_macaddr,
				"clns_isis_all_l1_l2_token_macaddr");
    reg_add_media_token_macaddr(LINK_ISIS_ALL_L2_IS,
				clns_isis_all_l1_l2_token_macaddr,
				"clns_isis_all_l1_l2_token_macaddr");

    reg_add_media_fddi_macaddr(LINK_CLNS, clns_fddi_macaddr,
			       "clns_fddi_macaddr");
    reg_add_media_fddi_macaddr(LINK_CLNS_ALL_ES, clns_all_es_fddi_macaddr,
			       "clns_all_es_fddi_macaddr");
    reg_add_media_fddi_macaddr(LINK_CLNS_ALL_IS, clns_all_is_fddi_macaddr,
			       "clns_all_is_fddi_macaddr");
    reg_add_media_fddi_macaddr(LINK_CLNS_BCAST, clns_bcast_fddi_macaddr,
			       "clns_bcast_fddi_macaddr");
    reg_add_media_fddi_macaddr(LINK_ISIS_ALL_L1_IS,
			       clns_isis_all_l1_fddi_macaddr,
			       "clns_isis_all_l1_fddi_macaddr");
    reg_add_media_fddi_macaddr(LINK_ISIS_ALL_L2_IS,
			       clns_isis_all_l2_fddi_macaddr,
			       "clns_isis_all_l2_fddi_macaddr");

    reg_add_media_hdlc_macaddr(LINK_CLNS, clns_hdlc_macaddr,
			       "clns_hdlc_macaddr");
    reg_add_media_hdlc_macaddr(LINK_CLNS_ALL_ES, clns_hdlc_macaddr,
			       "clns_hdlc_macaddr");
    reg_add_media_hdlc_macaddr(LINK_CLNS_ALL_IS, clns_hdlc_macaddr,
			       "clns_hdlc_macaddr");
    reg_add_media_hdlc_macaddr(LINK_CLNS_BCAST, clns_hdlc_macaddr,
			       "clns_hdlc_macaddr");
    reg_add_media_hdlc_macaddr(LINK_ISIS_ALL_L1_IS, clns_hdlc_macaddr,
			       "clns_hdlc_macaddr");
    reg_add_media_hdlc_macaddr(LINK_ISIS_ALL_L2_IS, clns_hdlc_macaddr,
			       "clns_hdlc_macaddr");

    reg_add_media_smds_macaddr(LINK_CLNS, clns_smds_macaddr,
			       "clns_smds_macaddr");
    reg_add_media_smds_macaddr(LINK_CLNS_ALL_ES, clns_smds_macaddr,
			       "clns_smds_macaddr");
    reg_add_media_smds_macaddr(LINK_CLNS_ALL_IS, clns_smds_macaddr,
			       "clns_smds_macaddr");
    reg_add_media_smds_macaddr(LINK_CLNS_BCAST, clns_smds_macaddr,
			       "clns_smds_macaddr");
    reg_add_media_smds_macaddr(LINK_ISIS_ALL_L1_IS, clns_smds_macaddr,
			       "clns_smds_macaddr");
    reg_add_media_smds_macaddr(LINK_ISIS_ALL_L2_IS, clns_smds_macaddr,
			       "clns_smds_macaddr");

    reg_add_media_x25_macaddr(LINK_CLNS, clns_x25_macaddr, "clns_x25_macaddr");
    reg_add_media_x25_macaddr(LINK_CLNS_ALL_ES, clns_x25_macaddr,
			      "clns_x25_macaddr");
    reg_add_media_x25_macaddr(LINK_CLNS_ALL_IS, clns_x25_macaddr,
			      "clns_x25_macaddr");
    reg_add_media_x25_macaddr(LINK_CLNS_BCAST, clns_x25_macaddr,
			      "clns_x25_macaddr");
    reg_add_media_x25_macaddr(LINK_ISIS_ALL_L1_IS, clns_x25_macaddr,
			      "clns_x25_macaddr");
    reg_add_media_x25_macaddr(LINK_ISIS_ALL_L2_IS, clns_x25_macaddr,
			      "clns_x25_macaddr");

    reg_add_media_x25_nsap_compare(nsap_match, "nsap_match");

    reg_add_media_frame_macaddr(LINK_CLNS, clns_frame_macaddr,
				"clns_frame_macaddr");
    reg_add_media_frame_macaddr(LINK_CLNS_ALL_ES, clns_frame_macaddr,
				"clns_frame_macaddr");
    reg_add_media_frame_macaddr(LINK_CLNS_ALL_IS, clns_frame_macaddr,
				"clns_frame_macaddr");
    reg_add_media_frame_macaddr(LINK_CLNS_BCAST, clns_frame_macaddr,
				"clns_frame_macaddr");
    reg_add_media_frame_macaddr(LINK_ISIS_ALL_L1_IS, clns_frame_macaddr,
				"clns_frame_macaddr");
    reg_add_media_frame_macaddr(LINK_ISIS_ALL_L2_IS, clns_frame_macaddr,
				"clns_frame_macaddr");

    reg_add_media_ddr_macaddr(LINK_CLNS, clns_ddr_macaddr,
				"clns_ddr_macaddr");
    reg_add_media_ddr_macaddr(LINK_CLNS_ALL_ES, clns_ddr_macaddr,
				"clns_ddr_macaddr");
    reg_add_media_ddr_macaddr(LINK_CLNS_ALL_IS, clns_ddr_macaddr,
				"clns_ddr_macaddr");
    reg_add_media_ddr_macaddr(LINK_CLNS_BCAST, clns_ddr_macaddr,
				"clns_ddr_macaddr");
    reg_add_media_ddr_macaddr(LINK_ISIS_ALL_L1_IS, clns_ddr_macaddr,
				"clns_ddr_macaddr");
    reg_add_media_ddr_macaddr(LINK_ISIS_ALL_L2_IS, clns_ddr_macaddr,
				"clns_ddr_macaddr");

    reg_add_media_atm_macaddr(LINK_CLNS, clns_atm_macaddr, "clns_atm_macaddr");
    reg_add_media_atm_macaddr(LINK_CLNS_ALL_ES, clns_atm_macaddr,
			      "clns_atm_macaddr");
    reg_add_media_atm_macaddr(LINK_CLNS_ALL_IS, clns_atm_macaddr,
			      "clns_atm_macaddr");
    reg_add_media_atm_macaddr(LINK_CLNS_BCAST, clns_atm_macaddr,
			      "clns_atm_macaddr");
    reg_add_media_atm_macaddr(LINK_ISIS_ALL_L1_IS, clns_atm_macaddr,
			      "clns_atm_macaddr");
    reg_add_media_atm_macaddr(LINK_ISIS_ALL_L2_IS, clns_atm_macaddr,
			      "clns_atm_macaddr");

    reg_add_media_ether_mtu(LINK_CLNS, clns_compute_mtu, "clns_compute_mtu");
    reg_add_media_atm_mtu(LINK_CLNS, clns_compute_mtu, "clns_compute_mtu");
    reg_add_media_tring_mtu(LINK_CLNS, clns_compute_biased_mtu,
			    "clns_compute_biased_mtu");
    reg_add_media_fddi_mtu(LINK_CLNS, clns_compute_biased_mtu,
			   "clns_compute_biased_mtu");
    reg_add_media_change_mtu(clns_change_mtu, "clns_change_mtu");
    reg_add_media_default_mtu(clns_set_default_mtu, "clns_set_default_mtu");
}
