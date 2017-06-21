/* $Id: netbios_as_media.c,v 3.3.52.2 1996/07/23 18:45:30 ppearce Exp $
 * $Source: /release/112/cvs/Xsys/ibm/netbios_as_media.c,v $
 *------------------------------------------------------------------
 * netbios_as_media.c -- media dependent NBF routines
 *
 * September 1995, Shoou Yiu
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: netbios_as_media.c,v $
 * Revision 3.3.52.2  1996/07/23  18:45:30  ppearce
 * CSCdi63747:  Move srb_multiring out of SRB subblock
 * Branch: California_branch
 *   (1) srb_multiring doesn't really belong exclusively to SRB
 *         move srb_multiring out of SRB subblock and into swidb
 *         provide srb_multiring accessor routines
 *   (2) determine_rif() should take swidb as argument rather than hwidb
 *
 * Revision 3.3.52.1  1996/03/18  20:11:47  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.20.3  1996/03/16  06:53:17  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.3.20.2  1996/03/13  01:23:30  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.3.20.1  1996/02/20  14:21:26  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.3  1995/11/30  02:59:50  syiu
 * CSCdi44150:  Remote Node NetBEUI on Token Ring platform not working
 *
 * Revision 3.2  1995/11/17  09:23:23  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:45:40  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:02:10  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/09/21  00:31:55  syiu
 * Initial placeholder for NBF.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
#include "master.h"
#include "sys_registry.h"
#include "media_registry.h"
#include "interface_private.h"
#include "packet.h"
#include "ieee.h"
#include "../if/ether.h"
#include "../if/tring.h"
#include "../if/rif.h"
#include "../if/rif_inline.h"
#include "../if/network.h"
  
static uchar netbios_tr_broadcast[IEEEBYTES] = {0xC0, 0x00, 0x00, 0x00, 0x00, 0x80};
static const uchar netbios_llc[SAP_HDRBYTES] = {SAP_NETBIOS, SAP_NETBIOS, LLC1_UI};

uchar netbios_multicast[IEEEBYTES] = {0x03, 0x00, 0x00, 0x00, 0x00, 0x01};


/*
 * nbf_macaddrs_update
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
static void nbf_macaddrs_update (idbtype *idb)
{
    hwidbtype *hwidb;

    hwidb = idb->hwptr;

    if (idb->hwptr->status & IDB_ETHER) {
      if_multi_add(hwidb, idb, netbios_multicast);
    }
}

/*
 * nbf_ether_macaddr
 *
 * Turn a NBF address into an encapsulation type and a MAC address
 * Called by MEDIA_ETHER_MACADDR, indexed by LINK_NBF
 */
static boolean nbf_ether_macaddr (paktype *pak, long address, int *enctype,
				  uchar **daddr, uchar const **other_info)
{
    /*
     * Prepare other_info to point at beggining of SAP. 
     * This depends on whether it is LLC1 or LLC2 (Supervisory and 
     * Information formats have control fields as 2 bytes)
     */
    if (pak->llc2_enctype == ET_SAP) {
      *other_info = (pak->network_start - SAP_HDRBYTES);
      *enctype = ET_SAP;
    } else if (pak->llc2_enctype == ET_LLC2)
      *enctype = ET_LLC2;

    *daddr = (uchar *)address;
    return(TRUE);
}

/*
 * nbf_token_macaddr
 *
 * Turn a LAT address into an encapsulation type and a MAC address
 * Called by MEDIA_TOKEN_MACADDR, indexed by LINK_NBF
 */
static boolean nbf_token_macaddr (paktype *pak, long address, int *enctype,
				  uchar **daddr, uchar const **other_info,
				  ulong *rif_flags)
{
    idbtype *interface;
 
    interface = pak->if_output;
 
    /*
     * Prepare other_info to point at beggining of SAP. 
     * This depends on whether it is LLC1 or LLC2 (Supervisory and 
     * Information formats have control fields as 2 bytes)
     */
    if (pak->llc2_enctype == ET_SAP) {
       *enctype = ET_SAP;
       *daddr = netbios_tr_broadcast;
    } else if (pak->llc2_enctype == ET_LLC2) {
       *enctype = ET_LLC2; 
       *daddr = (uchar *)address;
    }
    *other_info = netbios_llc;
    *rif_flags = determine_rif(interface, MULTIRING_BIT_OTHER);
    return(TRUE);
}

/*
 * nbf_media_registry
 *
 * Register media dependent functions for NBF
 */
void nbf_media_registry (void)
{
    reg_add_media_interesting_addresses(nbf_macaddrs_update,
					"nbf_macaddrs_update");
    reg_add_media_ether_macaddr(LINK_NBF, nbf_ether_macaddr,
				"nbf_ether_macaddr");
    reg_add_media_token_macaddr(LINK_NBF, nbf_token_macaddr,
				"nbf_token_macaddr");
}

