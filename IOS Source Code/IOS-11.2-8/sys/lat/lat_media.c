/* $Id: lat_media.c,v 3.4.32.3 1996/07/23 18:47:10 ppearce Exp $
 * $Source: /release/112/cvs/Xsys/lat/lat_media.c,v $
 *------------------------------------------------------------------
 * lat_media.c -- media dependent LAT routines.
 *
 * 13-April-1993, Kirk Lougheed
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: lat_media.c,v $
 * Revision 3.4.32.3  1996/07/23  18:47:10  ppearce
 * CSCdi63747:  Move srb_multiring out of SRB subblock
 * Branch: California_branch
 *   (1) srb_multiring doesn't really belong exclusively to SRB
 *         move srb_multiring out of SRB subblock and into swidb
 *         provide srb_multiring accessor routines
 *   (2) determine_rif() should take swidb as argument rather than hwidb
 *
 * Revision 3.4.32.2  1996/05/04  01:23:50  wilber
 * Branch: California_branch
 *
 * Integrated Routing and Bridging
 * - Bridge-group Virtual Interface smf support.
 *
 * Revision 3.4.32.1  1996/03/18  20:39:20  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.4.6.1  1996/03/13  02:35:56  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.4  1996/01/10  06:49:32  syiu
 * CSCdi41302:  Concurrent LAT & bridging for HDLC and PPP
 *
 * Revision 3.3  1995/11/17  17:33:35  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:56:10  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:26:32  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/08/07  05:38:22  richl
 * CSCdi38315:  makefile rework
 *              move inlines to tring_inline and utilities to rif_util.c
 *
 * Revision 2.1  1995/06/07  21:25:00  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "sys_registry.h"
#include "media_registry.h"
#include "interface_private.h"
#include "packet.h"
#include "../if/network.h"
#include "../if/ether.h"
#include "../if/rif_inline.h"
#include "../if/fddi.h"
#include "../srt/span.h"
#include "../srt/srt_registry.h"
#include "../wan/smds.h"
#include "../wan/serial.h"
#include "../smf/smf_public.h"

/*
 * External references
 */
extern int lat_local_services_offered(void);
extern boolean lat_on_idb(idbtype *);


/*
 * These constants are used to build node specific LAT addresses.
 */
static uchar tr_lat_multi[IEEEBYTES]  = {0xC0, 0x00, 0x10, 0x00, 0x00, 0x00};
static uchar lat_multicast[IEEEBYTES] = { 0x09, 0x00, 0x2B, 0x00, 0x00, 0x0F};
					/* All LAT master multicast */
static uchar lat_multicast_slave[IEEEBYTES] = { 0x09, 0x00, 0x2b, 0x02, 0x01, 0x04};
					/* All LAT slave multicast */

/*
 * lat_mass_macaddrs_update
 * 
 * This goes thru all hwidbs and updates/deletes the smf tables on the 
 * valid interfaces. This is called when a local LAT service is called and
 * and all interfaces that are bridge enabled and lat enabled should register
 * their respective interface hardware address to smf
 */
void lat_mass_macaddrs_update (boolean add)
{
    hwidbtype *hwidb;

    /*
     * Loop thru all interfaces looking only for serial interfaces that 
     * have lat enabled.
     * If add is requested, add the interface hardware MAC address to smf.
     * Otherwise check to see if there are any more local services offered. 
     * If none, delete the local MAC address of the interface in smf
     */ 
    FOR_ALL_HWIDBS(hwidb) {

       if (!((hwidb->status & IDB_SERIAL) && lat_on_idb(hwidb->firstsw)))
          continue;

       if (add)
	    reg_invoke_tbridge_smf_update(hwidb->hardware,
				      hwidb->firstsw,
				      SMF_MACTYP_LAT_UNICAST |
				      SMF_ACTION_RECEIVE);
       else if (!add && (lat_local_services_offered() == 0))
	    reg_invoke_tbridge_smf_delete(hwidb->hardware,
				      hwidb->firstsw,
				      SMF_MACTYP_LAT_UNICAST |
				      SMF_ACTION_RECEIVE);
     
    }
}


/*
 * lat_macaddrs_update
 *
 * Record any LAT specific addresses that the spanning tree code or
 * the interface needs to explicitly know about.
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
static void lat_macaddrs_update (idbtype *idb)
{
    hwidbtype *hwidb;
    
    hwidb = idb->hwptr;

    if (is_bvi(hwidb)) {

	idbtype *anyswidb;
	boolean found;

	if (lat_on_idb(idb)) {
	    reg_invoke_bvi_smf_update(lat_multicast,
				      idb,
				      SMF_MACTYP_LAT_MCAST |
				      SMF_ACTION_RECEIVE,
				      IDB_BRIDGE);
	    reg_invoke_bvi_smf_update(lat_multicast_slave,
				      idb,
				      SMF_MACTYP_LAT_MCAST |
				      SMF_ACTION_RECEIVE,
				      IDB_BRIDGE);
	} else {
	    reg_invoke_bvi_smf_delete(lat_multicast,
				      idb,
				      SMF_MACTYP_LAT_MCAST |
				      SMF_ACTION_RECEIVE,
				      IDB_BRIDGE);
	    reg_invoke_bvi_smf_delete(lat_multicast_slave,
				      idb,
				      SMF_MACTYP_LAT_MCAST |
				      SMF_ACTION_RECEIVE,
				      IDB_BRIDGE);
	}

	/* 
	 * This ugly search comes from the fast we don't have
	 * a registry mechanism to find the corresponding bridge 
	 * group from the BVI though we know the bridge-group number
	 * (hwidb->unit). Search the idb chain to find the first
	 * bridged interface in this bridge group.
	 */
	found = FALSE;
	FOR_ALL_SWIDBS(anyswidb) {
	    if (anyswidb->span_ptr && 
		(anyswidb->span_index == hwidb->unit)) {
		found = TRUE;
		break;
	    }
	}

	if (found && (anyswidb->span_ptr->lat_svc_filter)) {
	    reg_invoke_bvi_smf_update(lat_multicast,
				      idb,
				      SMF_MACTYP_LAT_MCAST |
				      SMF_ACTION_RECEIVE,
				      IDB_ETHER);
	} else {
	    reg_invoke_bvi_smf_delete(lat_multicast,
				      idb,
				      SMF_MACTYP_LAT_MCAST |
				      SMF_ACTION_RECEIVE,
				      IDB_ETHER);
	}
	return;
    }

    if (lat_on_idb(idb)) {
	reg_invoke_tbridge_smf_update(lat_multicast,
				      idb,
				      SMF_MACTYP_LAT_MCAST |
				      SMF_ACTION_RECEIVE);
	if_multi_add(hwidb, idb, lat_multicast);
	reg_invoke_tbridge_smf_update(lat_multicast_slave,
				      idb,
				      SMF_MACTYP_LAT_MCAST |
				      SMF_ACTION_RECEIVE);
	if_multi_add(hwidb, idb, lat_multicast_slave);

        /*
         * If this is a serial interface and there is at least one local 
         * LAT service offered on this system, register the serial interface's
         * hardware address (it really is the LAN's MAC address). This allows
         * unicast LAT directed to this box to be received by this LAT engine. 
         */
        if ((idb->hwptr->status & IDB_SERIAL) &&
            (lat_local_services_offered() > 0))
	    reg_invoke_tbridge_smf_update(hwidb->hardware,
				      idb,
				      SMF_MACTYP_LAT_UNICAST |
				      SMF_ACTION_RECEIVE);

	return;

    } else {
	reg_invoke_tbridge_smf_delete(lat_multicast,
				      idb,
				      SMF_MACTYP_LAT_MCAST |
				      SMF_ACTION_RECEIVE);
	reg_invoke_tbridge_smf_delete(lat_multicast_slave,
				      idb,
				      SMF_MACTYP_LAT_MCAST |
				      SMF_ACTION_RECEIVE);

        if (idb->hwptr->status & IDB_SERIAL)
	    reg_invoke_tbridge_smf_delete(hwidb->hardware,
				      idb,
				      SMF_MACTYP_LAT_UNICAST |
				      SMF_ACTION_RECEIVE);
    }

    if (idb->hwptr->status & IDB_ETHER) {
	if ((idb->span_ptr) && (idb->span_ptr->lat_svc_filter)) {
	    reg_invoke_tbridge_smf_update(lat_multicast,
					  idb,
					  SMF_MACTYP_LAT_MCAST |
					  SMF_ACTION_RECEIVE);
	    if_multi_add(hwidb, idb, lat_multicast);
	} else {
	    reg_invoke_tbridge_smf_delete(lat_multicast,
					  idb,
					  SMF_MACTYP_LAT_MCAST |
					  SMF_ACTION_RECEIVE);
	}
    }
}

/*
 * lat_ether_macaddr
 *
 * Turn a LAT address into an encapsulation type and a MAC address
 * Called by MEDIA_ETHER_MACADDR, indexed by LINK_LAT
 */
static boolean lat_ether_macaddr (paktype *pak, long address, int *enctype,
				  uchar **daddr, uchar const **other_info)
{
    *enctype = ET_ARPA;
    *daddr = (uchar *)address;
    return(TRUE);
}

/*
 * lat_token_macaddr
 *
 * Turn a LAT address into an encapsulation type and a MAC address
 * Called by MEDIA_TOKEN_MACADDR, indexed by LINK_LAT
 */
static boolean lat_token_macaddr (paktype *pak, long address, int *enctype,
				  uchar **daddr, uchar const **other_info,
				  ulong *rif_flags)
{
    static uchar bsaddr[IEEEBYTES];
    idbtype *interface;

    interface = pak->if_output;

    /*
     * LAT passes us the canonical address. In addition, it
     * uses a multicast address for All LAT servers. Since it
     * only uses that one multicast address, and never the broadcast
     * address, we can simply look for the ethernet multicast bit
     * to determine what to do.
     */
    if (((uchar *)address)[0] & 0x01) {
	*daddr = tr_lat_multi;
    } else {
	bsaddr[0] = BITSWAP(((uchar *)address)[0]);
	bsaddr[1] = BITSWAP(((uchar *)address)[1]);
	bsaddr[2] = BITSWAP(((uchar *)address)[2]);
	bsaddr[3] = BITSWAP(((uchar *)address)[3]);
	bsaddr[4] = BITSWAP(((uchar *)address)[4]);
	bsaddr[5] = BITSWAP(((uchar *)address)[5]);
	*daddr = bsaddr;
    }
    *enctype = ET_SNAP;
    *other_info = zerosnap;
    *rif_flags = determine_rif(interface, MULTIRING_BIT_OTHER);
    return(TRUE);
}

/*
 * lat_fddi_macaddr
 *
 * Turn a LAT address into an encapsulation type and a MAC address
 * Called by MEDIA_FDDI_MACADDR, indexed by LINK_LAT
 */
static boolean lat_fddi_macaddr (paktype *pak, long address, int *enctype,
				 uchar **daddr, uchar const **other_info,
				 ulong *rif_flags)
{
    idbtype *interface;
    hwidbtype *hwidb;

    interface = pak->if_output;
    hwidb = hwidb_or_null(interface);

    /*
     * For LAT, if the specified encapsulation is ET_BRIDGE, we will
     * look up the address in the bridging database. If we find it,
     * we will return the packet as a bridged datagram. If we
     * are not running bridging, or the address is not found, we
     * do not encapsulate the packet.
     *
     * If the encapsulation specified is anything else, we will
     * use the common code with LOOP and XARP, using ISO2 encapsulation.
     */
    if ((pak->enctype == ET_BRIDGE) &&
	(hwidb->fddi_options & FDDI_ENCAPSULATE)) {
	if (!RUNNING_IF(LINK_BRIDGE,interface))
	    return(FALSE);
	*daddr = (*pak->if_output->hwptr->bridge_vencap)(pak, address);
	if (*daddr) {
	    pak->linktype = LINK_BRIDGE;
	    *enctype = ET_SNAP;
	    *other_info = cisco_snap;
	    return(TRUE);
	}
	return(FALSE);
    }
    *enctype = ET_SNAP;
    *daddr = (uchar *)address;
    *other_info = zerosnap;
    *rif_flags = determine_rif(interface, MULTIRING_BIT_OTHER);
    return(TRUE);
}

/*
 * lat_hdlc_macaddr
 *
 * Turn an LAT address into an encapsulation type and a MAC address
 * Called by MEDIA_HDLC_MACADDR, indexed by LINK_LAT
 */
static boolean lat_hdlc_macaddr (paktype *pak, long address,
				 int *enctype, ushort *flags)
{
    if (ether_vencap(pak, address)) {
	pak->datagramsize += HDLC_ENCAPBYTES;
	pak->datagramstart -= HDLC_ENCAPBYTES;
	*((long *)pak->datagramstart) = HDLC_BRIDGECODE;
	pak->enctype = ET_BRIDGE;	/* Set bridge encapsulation */
	*enctype = ET_NULL;
	return(TRUE);
    } else {
	return(FALSE);
    }
    if ((address == 0) || (address == -1))
	*flags |= HDLC_BCAST;
    return(TRUE);
}

/*
 * lat_smds_macaddr
 *
 * Turn a LAT address into an encapsulation type and a MAC address
 * Called by MEDIA_SMDS_MACADDR, indexed by LINK_LAT
 */
static boolean lat_smds_macaddr (paktype *pak, long address, int *enctype,
				 uchar const **daddr, uchar const **other_info,
				 addrtype *proto_addr)
{
    idbtype *idb;
    hwidbtype *hwidb;

    idb = pak->if_output;
    hwidb = hwidb_or_null(idb);

    /*
     * For LAT, if the specified encapsulation is ET_BRIDGE, we will
     * look up the address in the bridging database. If we find it,
     * we will return the packet as a bridged datagram. If we
     * are not running bridging, or the address is not found, we
     * do not encapsulate the packet.
     */
    if (!RUNNING_IF(LINK_BRIDGE,idb))
	return(FALSE);
    *daddr = (*pak->if_output->hwptr->bridge_vencap)(pak, address);
    if (*(char *)address & 0x1) {		/* Multicast? */
	*daddr = baddr;				/* Force a broadcast */
    }
    pak->linktype = LINK_BRIDGE;
    *enctype = ET_SNAP;
    *other_info = zerosnap;
    return(TRUE);
}


/*
 * lat_media_registry
 *
 * Register media dependent functions for LAT
 */
void lat_media_registry (void)
{
    reg_add_media_interesting_addresses(lat_macaddrs_update,
					"lat_macaddrs_update");
    reg_add_media_ether_macaddr(LINK_LAT, lat_ether_macaddr,
				"lat_ether_macaddr");
    reg_add_media_token_macaddr(LINK_LAT, lat_token_macaddr,
				"lat_token_macaddr");
    reg_add_media_fddi_macaddr(LINK_LAT, lat_fddi_macaddr, "lat_fddi_macaddr");
    reg_add_media_hdlc_macaddr(LINK_LAT, lat_hdlc_macaddr, "lat_hdlc_macaddr");
    reg_add_media_smds_macaddr(LINK_LAT, lat_smds_macaddr, "lat_smds_macaddr");
}
