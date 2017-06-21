/* $Id: cdp_media.c,v 3.3.58.3 1996/05/04 00:34:47 wilber Exp $:
 * $Source: /release/112/cvs/Xsys/cdp/cdp_media.c,v $
 *------------------------------------------------------------------
 * Media registry initialization for CDP.
 *
 * October 1994, Arun Sastry
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cdp_media.c,v $
 * Revision 3.3.58.3  1996/05/04  00:34:47  wilber
 * Branch: California_branch
 *
 * Integrated Routing and Bridging
 * - Bridge-group Virtual Interface smf support.
 *
 * Revision 3.3.58.2  1996/04/15  21:13:09  bcole
 * CSCdi54649:  IP Host Modularity, IP Automatic Stub Routing feature
 * commit
 * Branch: California_branch
 *
 * Revision 3.3.58.1  1996/03/18  19:01:53  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.24.3  1996/03/13  01:12:28  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.3.24.2  1996/03/07  08:39:27  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.3.24.1  1996/02/20  13:39:27  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.3  1995/11/17  08:49:50  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:11:47  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:05:16  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:15:07  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <string.h>
#include <ciscolib.h>
#include "sys_registry.h"
#include "media_registry.h"
#include "interface_private.h"
#include "packet.h"
#include "address.h"
#include "ieee.h"
#include "../if/network.h"
#include "../if/rif.h"
#include "../if/static_map.h"
#include "../srt/srt_registry.h"
#include "../wan/serial.h"
#include "cdp_externs.h"

#include "../smf/smf_public.h"


/*
 * cdp_ether_macaddr
 * 
 * Turn a CDP address into an encapsulation type and a MAC address.
 * Called by MEDIA_ETHER_MACADDR, indexed by LINK_CDP.
 */
static boolean cdp_ether_macaddr (paktype *pak, long address,
				  int *enctype, uchar **daddr,
				  uchar const **other_info)
{
    *daddr = cdp_multicast;
    *enctype = ET_SNAP;
    *other_info = cisco_snap;
    return(TRUE);
}

/*
 * cdp_fddi_macaddr
 * 
 * Turn a CDP address into an encapsulation type and a MAC address
 * Called by MEDIA_FDDI_MACADDR, indexed by LINK_CDP.
 */
static boolean cdp_fddi_macaddr (paktype *pak, long address,
				 int *enctype, uchar **daddr,
				 uchar const **other_info, ulong *rif_flags)
{
    *daddr = cdp_multicast;
    *enctype = ET_SNAP;
    *other_info = cisco_snap;
    *rif_flags = RIF_NOT_PRESENT;
    return(TRUE);
}

/*
 * cdp_token_macaddr
 * 
 * Turn a CDP address into an encapsulation type and a MAC address
 * Called by MEDIA_TOKEN_MACADDR, indexed by LINK_CDP.
 */
static boolean cdp_token_macaddr (paktype *pak, long address,
				  int *enctype, uchar **daddr,
				  uchar const **other_info,
				  ulong *rif_flags)
{
    *daddr = cdp_tr_func_addr;
    *enctype = ET_SNAP;
    *other_info = cisco_snap;
    *rif_flags = RIF_NOT_PRESENT;
    return(TRUE);
}

/*
 * cdp_hdlc_macaddr
 * 
 * Turn a CDP address into an encapsulation type and a MAC address
 * Called by MEDIA_HDLC_MACADDR, indexed by LINK_CDP.
 */
static boolean cdp_hdlc_macaddr (paktype *pak, long address,
				 int *enctype, ushort *flags)	 
{
    *flags |= HDLC_BCAST;
    *enctype = ET_HDLC;
    return (TRUE);
}

/*
 * cdp_smds_macaddr
 * 
 * Turn a CDP address into an encapsulation type and a MAC address
 * Called by MEDIA_SMDS_MACADDR, indexed by LINK_CDP.
 */
static boolean cdp_smds_macaddr (paktype *pak, long address, int *enctype,
				 uchar const **daddr,
				 uchar const **other_info,
				 addrtype *proto_addr)
{
    *daddr = baddr;
    *enctype = ET_SNAP;
    *other_info = zerosnap;
    return (TRUE);
}

/*
 * cdp_x25_macaddr
 * 
 * Turn a CDP address into an encapsulation type and a MAC address
 * Called by MEDIA_X25_MACADDR, indexed by LINK_CDP.
 */
static boolean cdp_x25_macaddr (paktype *pak, long address,
				addrtype *proto_addr, lcitype **lci,
				boolean *broadcast)
{
    *broadcast = TRUE;
    return(TRUE);
}

/*
 * cdp_frame_macaddr
 *
 * Set broadcast flag, so that CDP packets can go through.
 * Called by MEDIA_FRAME_MACADDR, indexed by LINK_CDP.
 */
static boolean cdp_frame_macaddr (paktype *pak, long address,
			          addrtype *proto_addr, boolean *broadcast)
{
    *broadcast = TRUE;
    return(TRUE);
}

/*
 * cdp_atm_macaddr
 *
 * Turn a CDP address into a broadcast flag.
 *
 * Called by MEDIA_ATM_MACADDR, indexed by LINK_CDP.
 */
static boolean cdp_atm_macaddr (paktype *pak, long address, hwaddrtype *daddr, 
				uchar const **snap_info, staticmap_type **map, 
				boolean *broadcast)
{
    *broadcast = TRUE;
    memset(daddr, 0, sizeof(hwaddrtype));
    *snap_info = cisco_snap;

    if ((*map = staticmap_lookup(pak->if_output, NULL, LINK_CDP,
                                 ET_ATM, *broadcast)))
        return(TRUE);
    else
        return(FALSE);
}

/*
 * cdp_macaddrs_update
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
void cdp_macaddrs_update (idbtype *idb)
{
    uchar *cdp_mcast;
    uchar cdp_multicast_swap[IEEEBYTES];

    if (is_bvi(idb->hwptr)) {
	ieee_swap(cdp_tr_func_addr, cdp_multicast_swap);
	if (RUNNING_IF(LINK_CDP, idb)) {
	    /* token only */
	    reg_invoke_bvi_smf_update(cdp_multicast_swap,
				      idb,
				      SMF_MACTYP_CDP_MCAST |
				      SMF_ACTION_RECEIVE,
				      IDB_TR);

	    /* All interfaces but token */
	    reg_invoke_bvi_smf_update(cdp_multicast,
				      idb,
				      SMF_MACTYP_CDP_MCAST |
				      SMF_ACTION_RECEIVE,
				      (IDB_LEX|IDB_ETHER|IDB_SERIAL|
				       IDB_FDDI|IDB_ISDN|IDB_ATM));
	} else {
	    /* token only */
	    reg_invoke_bvi_smf_delete(cdp_multicast_swap,
				      idb,
				      SMF_MACTYP_CDP_MCAST |
				      SMF_ACTION_RECEIVE,
				      IDB_TR);
	    /* All interfaces but token */
	    reg_invoke_bvi_smf_delete(cdp_multicast,
				      idb,
				      SMF_MACTYP_CDP_MCAST |
				      SMF_ACTION_RECEIVE,
				      (IDB_LEX|IDB_ETHER|IDB_SERIAL|
				       IDB_FDDI|IDB_ISDN|IDB_ATM));
	}
	return;
    }

    if (is_tokenring(idb->hwptr)) {
	ieee_swap(cdp_tr_func_addr, cdp_multicast_swap);
	cdp_mcast = cdp_multicast_swap;
    } else {
	cdp_mcast = cdp_multicast;
    }

    if (RUNNING_IF(LINK_CDP, idb)) {
	reg_invoke_tbridge_smf_update(cdp_mcast,
				      idb,
				      SMF_MACTYP_CDP_MCAST |
				      SMF_ACTION_RECEIVE);
	if (idb->hwptr && idb->hwptr->listen)
	    if_multi_add(idb->hwptr, idb, cdp_mcast);
    } else {
	reg_invoke_tbridge_smf_delete(cdp_mcast,
				      idb,
				      SMF_MACTYP_CDP_MCAST |
				      SMF_ACTION_RECEIVE);
    }
}

/*
 * cdp_ddr_macaddr
 * Turn an CDP address into a broadcast flag, or a filled in addrtype
 * structure that will be used to look up a static mapping.
 * Called by MEDIA_DDR_MACADDR, indexed by LINK_CDP.
 */
static boolean cdp_ddr_macaddr (paktype *pak, long address,
				addrtype *proto_addr, boolean *broadcast)
{
    /*
     * Follow whatever dialer map is already set up for IP
     */
    pak->dialer_save_type = LINK_IP;
    *broadcast = TRUE;
    return(TRUE);
}

/*
 * cdp_media_registry
 * Register media dependent code for CDP
 */

void cdp_media_registry (void)
{
    reg_add_media_ether_macaddr(LINK_CDP, cdp_ether_macaddr,
				"cdp_ether_macaddr");
    reg_add_media_fddi_macaddr(LINK_CDP, cdp_fddi_macaddr,
			       "cdp_fddi_macaddr");
    reg_add_media_token_macaddr(LINK_CDP, cdp_token_macaddr,
			        "cdp_token_macaddr");
    reg_add_media_x25_macaddr(LINK_CDP, cdp_x25_macaddr,
			        "cdp_x25_macaddr");
    reg_add_media_hdlc_macaddr(LINK_CDP, cdp_hdlc_macaddr,
			        "cdp_hdlc_macaddr");
    reg_add_media_smds_macaddr(LINK_CDP, cdp_smds_macaddr,
			        "cdp_smds_macaddr");
    reg_add_media_frame_macaddr(LINK_CDP, cdp_frame_macaddr,
				"cdp_frame_macaddr");
    reg_add_media_atm_macaddr(LINK_CDP, cdp_atm_macaddr,
				"cdp_atm_macaddr");
    reg_add_media_ddr_macaddr(LINK_CDP, cdp_ddr_macaddr, 
			      "cdp_ddr_macaddr");
    reg_add_media_interesting_addresses(cdp_macaddrs_update,
					"cdp_macaddrs_update");
}

