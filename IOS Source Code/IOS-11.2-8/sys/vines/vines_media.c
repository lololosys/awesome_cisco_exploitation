/* $Id: vines_media.c,v 3.6.28.4 1996/07/23 18:49:43 ppearce Exp $
 * $Source: /release/112/cvs/Xsys/vines/vines_media.c,v $
 *------------------------------------------------------------------
 * vines_media.c - vines support routines for media layers
 *
 * August 1992, David R. Hampton
 *
 * Copyright (c) 1992-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: vines_media.c,v $
 * Revision 3.6.28.4  1996/07/23  18:49:43  ppearce
 * CSCdi63747:  Move srb_multiring out of SRB subblock
 * Branch: California_branch
 *   (1) srb_multiring doesn't really belong exclusively to SRB
 *         move srb_multiring out of SRB subblock and into swidb
 *         provide srb_multiring accessor routines
 *   (2) determine_rif() should take swidb as argument rather than hwidb
 *
 * Revision 3.6.28.3  1996/04/25  23:22:52  ronnie
 * CSCdi55766:  Hey!  This ATM and Frame Relay is no different than in 11.1
 * Feature commit for ATM and Frame Relay into 11.2
 * Branch: California_branch
 *
 * Revision 3.6.28.1.2.2  1996/04/09  21:07:47  rzagst
 * Syncing ATM_Cal_branch to the California mainline.
 * Branch: ATM_Cal_branch
 *
 * Revision 3.6.28.1.2.1  1996/03/24  04:05:25  rzagst
 * ATM modularity: Remove AIP from all files that are not referring to the
 * AIP i/f
 * Branch: ATM_Cal_branch
 *
 * Revision 3.6.28.2  1996/03/28  22:42:27  ehk
 * CSCdi51823:  Vines recompute does not recompute.
 * Branch: California_branch
 * take all interfaces into account when computing vines network
 * number (CSCdi51823). if no interface MAC address is available,
 * use the system default MAC addres (CSCdi52195). to be consistent
 * with other protocols, disallow vines UI configuration unless
 * vines routing is enabled (CSCdi52009).
 *
 * Revision 3.6.28.1  1996/03/18  22:31:55  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.6.2.2  1996/03/13  02:04:38  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.6.2.1  1996/03/07  11:04:37  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.6  1996/01/22  07:44:39  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.5  1995/12/01  21:39:03  slin
 * CSCdi44836:  Vines doesnt use idb subblocks
 *
 * Revision 3.4  1995/11/22  18:55:03  slin
 * CSCdi38892:  vines over X.25 requesting a full routing update on all
 *              PVCs
 * - Send only to newly established PVC
 *
 * Revision 3.3  1995/11/17  17:58:00  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:47:47  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:44:48  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/08/07  05:44:43  richl
 * CSCdi38315:  makefile rework
 *              header changes for rif_util.c and tring_inline.h
 *
 * Revision 2.2  1995/06/13  18:31:39  slin
 * CSCdi35622:  Vines not correctly notified of FR PVC state changes
 *
 * Revision 2.1  1995/06/07  23:14:33  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <string.h>
#include <ciscolib.h>
#include "media_registry.h"
#include "interface_private.h"
#include "packet.h"
#include "address.h"
#include "../if/static_map.h"
#include "../if/tring.h"
#include "../if/rif.h"
#include "../if/rif_inline.h"
#include "../if/network.h"
#include "../wan/serial.h"
#include "vines_private.h"

static const uchar vines_llc[SAP_HDRBYTES] = {SAP_VINES, SAP_VINES, LLC1_UI};


/**********************************************************************
 *
 *		    VINES MAC LEVEL ECHO FUNCTIONS
 *
 **********************************************************************/

/*
 * vines_mac_echo
 *
 * Turn a vines echo packet around and send it back to its source.  This
 * is a MAC level echo function.  There are no network level addresses in
 * the packet.
 */
void vines_mac_echo (paktype *pak)
{
    int length;
    idbtype *swidb;
    vinesidbtype *idb;
    vinesneighbortype gw;
    vinespathtype path;

    swidb = pak->if_input;
    idb = vines_getidb(swidb);
    idb->rx_mac_echo++;
    if (!(idb_is_ieeelike(swidb))) {
	if (vinesecho_debug)
	    buginf("\nVINESECHO: packet from non IEEE media");
	datagram_done(pak);
	return;
    }

    /*
     * Extract necessary info into a neighbor structure.
     */
    memset(&gw, 0, sizeof(vinesneighbortype));
    memset(&path, 0, sizeof(vinespathtype));
    path.enctype = pak->enctype;
    (*swidb->hwptr->extract_snpa)(pak, &path.dlc_addr, SOURCE);
    gw.available_paths = gw.best_path = &path;
    if (!is_ieee_bcast(path.dlc_addr.addr)) {
	if (vinesecho_debug)
	    buginf("\nVINESECHO: %d byte echo reply from %e", path.dlc_addr.addr);
	datagram_done(pak);
	return;
    }

    /*
     * Ugly gross kludge to determine the packet length.
     */
    length = (pak->datagramsize - (pak->network_start - pak->datagramstart));
    if (vinesecho_debug)
	buginf("\nVINESECHO: %d byte echo request from %e", length, path.dlc_addr.addr);
    pak->if_output = swidb;
    pak->acct_proto = ACCT_PROTO_VINES;
    clear_if_input(pak);
    if (pak_encap(swidb->hwptr, pak, length, (long)&gw)) {
	idb->tx_mac_echo++;
	if (vinesecho_debug)
	    buginf("\nVINESECHO: returned %d byte echo reply to %e", length, path.dlc_addr.addr);
    	datagram_out(pak);
	return;
    }

    /*
     * Encapsulation failed for some reason.
     */
    if (vinesecho_debug)
	buginf("\nVINESECHO: encapsulation failed on reply to %e", path.dlc_addr.addr);
	idb->tx_failed_encaps++;
    datagram_done(pak);
    return;
}

/**********************************************************************
 *
 *		 VINES ADDRESS MANIPULATION FUNCTIONS
 *
 **********************************************************************/

/*
 * vines_pick_default_net
 *
 * Choose the network number from one of our hardware address when the
 * user defaults it.  Use the first address found that is from an IEEE
 * that is non-zero.  This later restriction exists because the 4mb token
 * ring takes a while coming up for the first time.  Once it has an
 * address, it is OK to use.  We return the lower four bytes of the
 * address as an integer, since these are the most likely bytes to change
 * between systems.
 */
ulong vines_pick_default_net (void)
{
    hwidbtype *idb;
    uint net;
    vinesnettype *server;

    FOR_ALL_HWIDBS(idb) {
	if (!(idb->status & IDB_IEEEMASK))
            continue;
	if ((GETSHORT(&idb->bia[4]) != 0) ||
	    (GETSHORT(&idb->bia[2]) != 0) ||
	    (GETSHORT(&idb->bia[0]) != 0)) {
	    net = (GETSHORT(&idb->bia[2]) << 16) |
		GETSHORT(&idb->bia[4]);
	    net &= VINES_NET_SEQ;
	    net |= VINES_NET_CISCO;

	    /*
	     * if this network is being used by someone else or
	     * vines address recomputation is requested, move on
	     * to next interface mac address.
	     */
	    if (vines_network && 
		(((server = vroute_FindEntry(net)) != NULL) ||
		 (vines_network == net)))
		continue;
	    return(net);
	}
    }

    /*
     * default mac address is derived from the current clock
     * when no default IEEE address is found in the system.
     * for details, see ../if/network.c 
     */
    net = (GETSHORT(&default_mac_addr[2]) << 16) |
	   GETSHORT(&default_mac_addr[4]);
    net &= VINES_NET_SEQ;
    net |= VINES_NET_CISCO;
    return(net);
}

/*
 * vines_validate_net
 *
 * Check the network number to see if it came from one of our hardware
 * address.  This is used to see if a user entered network number is a
 * number that could have been automatically generated by this router.
 * This is mainly used to insure that configuration files aren't copied
 * around, causing duplicate router addreses.
 */
boolean vines_validate_net (ulong net)
{
    hwidbtype *idb;
    boolean unknown_addrs;
    ulong temp;

    unknown_addrs = FALSE;
    FOR_ALL_HWIDBS(idb) {
	 if (!(idb->status & IDB_IEEEMASK))
	     continue;
	if ((GETSHORT(&idb->bia[4]) != 0) ||
	    (GETSHORT(&idb->bia[2]) != 0) ||
	    GETSHORT(&idb->bia[0]) != 0) {
	    temp = (GETSHORT(&idb->bia[2]) << 16) |
		GETSHORT(&idb->bia[4]);
	    temp &= VINES_NET_SEQ;
	    temp |= VINES_NET_CISCO;
	    if (temp == net) {
		return(TRUE);
	    }
	} else {
	    unknown_addrs = TRUE;
	}    
    }
    return(unknown_addrs);
}


/**********************************************************************
 *
 *		 VINES METRIC MANIPULATION FUNCTIONS
 *
 **********************************************************************/

/*
 * vines_compute_default_metric
 *
 * What is a reasonable metric for an interface?  This routine has fixed
 * values for LAN interfaces.  For WAN interfaces, the bandwidth value is
 * used to calculate a metric.  It also tacks on the appropriate addidion
 * if the interface is X.25.  This routine will also insure that a zero
 * value is never produced so VINES isn't turned off by accident.
 */
int vines_compute_default_metric (idbtype *idb)
{
    ulong status, bandwidth;
    int metric;

    status = idb->hwptr->status & IDB_IEEEMASK;
    if (status) {
	switch (status) {
	  case IDB_ETHER:
	    return(V_METRIC_ETHER);
	  case IDB_FDDI:
	    return(V_METRIC_FDDI);
	  case IDB_TR: 
	    if (idb->hwptr->tr_ring_speed == 16)
		return(V_METRIC_16MBTR);
	    return(V_METRIC_4MBTR);
	}
	
	/*
	 * If this is an unknown IEEE interface, use the value for ethernet.
	 */
	return(V_METRIC_ETHER);
    }
    
    /*
     * Map the interface bandwidth to a Vines metric.
     */
    bandwidth = idb->visible_bandwidth;
    if (bandwidth >= 1540)
	metric = V_METRIC_T1;
    else if (bandwidth >= 56)
	metric = V_METRIC_56K;
    else if (bandwidth >= 9)
	metric = V_METRIC_9600;
    else if (bandwidth >= 4)
	metric = V_METRIC_4800;
    else if (bandwidth >= 2)
	metric = V_METRIC_2400;
    else
	metric = V_METRIC_1200;
    
    /*
     * Tack on two seconds for X.25 processing.
     */
    if (is_x25(idb->hwptr))
	metric += V_METRIC_X25;

    return(metric);
}


/**********************************************************************
 *
 *	       VINES MAC LEVEL MISCELLANEOUS FUNCTIONS
 *
 **********************************************************************/

/*
 * vines_getlink
 *
 * Convert the vines multiplexing byte into a link type.
 */
static ulong vines_getlink (
    idbtype *idb,
    paktype *pak)
{
    sap_hdr *sap;

    sap = (sap_hdr *)pak->info_start;
    switch (sap->data[0]) {
      case VINES_DEMUX_IP:	return(LINK_VINES);
      case VINES_DEMUX_ECHO:	return(LINK_VINES_ECHO);
      default:			return(LINK_ILLEGAL);
    }
}

/*
 * vines_demuxbyte
 *
 * Convert a link type into the vines multiplexing byte.
 */
uchar vines_demuxbyte (long linktype)
{
    switch (linktype) {
      case LINK_VINES:
	return(VINES_DEMUX_IP);

      case LINK_VINES_ECHO:
	return(VINES_DEMUX_ECHO);

      default:
	return(-1);
    }
}

/*
 * vines_compute_metric
 *
 * Given a received packet, determine the appropriate metric to reach the
 * sender.  This is used to determine if there any source route bridges
 * in the path between the sender and this router.
 */
int vines_compute_metric (paktype *pak)
{
    int metric;
    idbtype *swidb;
    hwaddrtype dlc_addr;

    swidb = pak->if_input;
    metric = vines_getidb(swidb)->vines_metric;
    if (swidb->hwptr->status & (IDB_TR | IDB_FDDI)) {
	(*swidb->hwptr->extract_snpa)(pak, &dlc_addr, SOURCE);
        metric += (V_METRIC_BRIDGE * rif_count_bridges(dlc_addr.addr, swidb->hwptr));
    }
    return(metric);
}

/*
 * vines_dlc
 *
 * Pretty-print a hardware address.  This routine should be in the 'os'
 * directory somewhere.
 */
char *vines_dlc (char *buffer, hwaddrtype *addr)
{
    switch (addr->type) {
      case STATION_IEEE48:
	sprintf(buffer, "%e", addr->addr);
	break;
      case STATION_X121:
	sprintf(buffer, "%s", addr->addr);
	break;
      case STATION_CISCO_HDLC:
	sprintf(buffer, "HDLC");
	break;
      case STATION_CISCO_MLAPB:
      case STATION_LAPB:
	sprintf(buffer, "LAPB");
	break;
      case STATION_SMDS48:
	sprintf(buffer, "%e", addr->addr);
	break;
      case STATION_SMDS64:
	sprintf(buffer, "%E", addr->addr);
	break;
      case STATION_CISCO_MSDLC:
      case STATION_FR10:
	sprintf(buffer, "%4d", *(ushort *)addr->addr);
	break;
      case STATION_CISCO_TUNNEL:
	sprintf(buffer, "Tunnel");
	break;
      case STATION_PPP:
	sprintf(buffer, "PPP");
	break;
      case STATION_ATMVC:
	sprintf(buffer, "%d", *(ushort *)addr->addr);
	break;
      case STATION_ATMNSAP:
	sprintf(buffer, "ATM_NSAP");
      default:
	sprintf(buffer, "unknown");
	break;
    }
    return(buffer);
}

/*
 * vines_on_idb
 *
 * Return TRUE if Vines is enabled on the given interface
 */
boolean vines_on_idb (idbtype *idb)
{
    vinesidbtype *vidb = vines_getidb(idb);
    
    if (vines_running &&
	idb &&
	vidb &&
	vidb->vines_metric > 0)
	return(TRUE);
    return(FALSE);
}

/*
 * vines_interface_up
 *
 * Determine if the vines protocol is up or down on a given interface.
 */
boolean vines_interface_up (vinesidbtype *idb)
{
    if (!idb || !idb->vines_metric)
	return(FALSE);
    return(interface_up(idb->phys_idb));
}

/*
 * vines_get_interface_metric
 *
 * Return the vines metric for a given interface.
 */
uint vines_get_interface_metric (idbtype *swidb)
{
    vinesidbtype *idb;

    idb = vines_getidb(swidb);
    if (idb == NULL)
	return(0);
    return(idb->vines_metric);
}

/*
 * vines_get_default_enctype
 *
 * Return the vines encapsulation type for a given interface.
 */
int vines_get_default_enctype (idbtype *swidb)
{
    vinesidbtype *idb;

    idb = vines_getidb(swidb);
    if (idb == NULL)
	return(ET_NULL);
    return(idb->vines_enctype);
}

/*
 * vines_valid_access_list
 */
static boolean vines_valid_access_list (int number)
{
    return((number >= VINESMINACC) && (number <= VINESMAXACC));
}

/*
 * vines_is_my_saddr
 *
 * Attempt to see if the source hardware address in the packet belongs to
 * one of our interfaces.  This is used to prevent erroneous error
 * message when two interfaces are inserted onto the same network.
 */
boolean vines_is_my_saddr (paktype *pak)
{
    hwaddrtype saddr, if_addr;
    hwidbtype *hwidb;
    vinesidbtype *idb;

    hwidb = pak->if_input->hwptr;
    memset(&saddr, 0, sizeof(hwaddrtype));
    if (!(hwidb->extract_snpa)(pak, &saddr, SOURCE)) {
	/*
	 * Can't get packet's source address.  Give up and return TRUE.
	 */
	return(TRUE);
    }

    FOR_ALL_VIDBS(idb) {
	hwidb = idb->phys_idb->hwptr;
	memset(&if_addr, 0, sizeof(hwaddrtype));
	if (hwidb->extract_hwaddr == NULL)
	    continue;
	if ((hwidb->extract_hwaddr)(hwidb, &if_addr) == FALSE)
	    continue;
	if (bcmp(&saddr, &if_addr, sizeof(hwaddrtype)) == 0)
	    return(TRUE);
    }
    return(FALSE);
}

/**********************************************************************
 *
 *		 VINES ADDRESS MANIPULATION FUNCTIONS
 *
 **********************************************************************/

/*
 * vines_build_address
 *
 * Build an addrtype data structure containing the VINES address
 * currently in use on this interface.
 */
static void vines_build_address (
    idbtype *swidb,
    paktype *pak,
    addrtype *addr)
{
    memset(addr, 0, sizeof(addrtype));
    addr->type = ADDR_VINES;
    addr->length = ADDRLEN_VINES;
    addr->vines_addr.net = vines_network;
    addr->vines_addr.host = vines_host;
}

/*
 * vines_sprintf_address
 *
 * Take an addrtype data structure and print it as an VINES address.
 */
static void vines_sprintf_address (
    char *string,
    addrtype *addr)
{
    sprintf(string, "%z", addr->vines_addr.net, addr->vines_addr.host);
}

/*
 * vines_build_packet_address
 *
 * Given a packet, build a addrtype data structure of the source/dest address.
 * The memset zero is important!
 */
static void vines_build_packet_address (
    paktype *pak,
    addrtype *addr,
    enum SRCDST which)
{
    vinesiptype *vip = (vinesiptype *) pak->network_start;
    
    memset(addr, 0, sizeof(addrtype));
    addr->type = ADDR_VINES;
    addr->length = ADDRLEN_VINES;
    if (which == SOURCE) {
	addr->vines_addr.net = GET2WD(vip->ssrcnet);
	addr->vines_addr.host = vip->srchost;
    } else {
	addr->vines_addr.net = GET2WD(vip->ddstnet);
	addr->vines_addr.host = vip->dsthost;
    }
}

/*
 * vines_sprintf_src_dst
 *
 * Given a packet, extract and print a VINES address.
 */
static void vines_sprintf_src_dst (
    char *string,
    paktype *pak,
    enum SRCDST which)
{
    vinesiptype *vip;

    vip = (vinesiptype *)pak->network_start;
    if (which == SOURCE)
	sprintf(string, "%z", GET2WD(vip->ssrcnet), vip->srchost);
    else
        sprintf(string, "%z", GET2WD(vip->ddstnet), vip->dsthost);
}


/**********************************************************************
 *
 *		 VINES ENCAPSULATION HELPER FUNCTIONS
 *
 **********************************************************************/

static inline int vines_get_enctype (long ptr)
{
    vinespathtype *path = (vinespathtype *)ptr;

    return(path->enctype);
}

static inline uchar *vines_get_mac_address (long ptr)
{
    vinespathtype *path = (vinespathtype *)ptr;

    return(path->dlc_addr.addr);
}

static inline ulong vines_get_net (long ptr)
{
    vinespathtype *path = (vinespathtype *)ptr;

    return(path->neighbor->net);
}

static inline ushort vines_get_host (long ptr)
{
    vinespathtype *path = (vinespathtype *)ptr;

    return(path->neighbor->host);
}

/*
 * vines_ether_macaddr
 *
 * Turn a Vines address into an encapsulation type and a MAC address
 * Called by MEDIA_ETHER_MACADDR
 */
static boolean vines_ether_macaddr (paktype *pak, long address, int *enctype,
				    uchar **daddr, uchar const **other_info)
{
    if (address == 0) {
	*daddr = (uchar *)baddr;
	*enctype = vines_get_default_enctype(pak->if_output);
    } else {
	*daddr = vines_get_mac_address(address);
	*enctype = vines_get_enctype(address);
    }
    return(TRUE);
}

/*
 * vines_token_macaddr
 *
 * Turn a Vines address into an encapsulation type and a MAC address
 * Called by MEDIA_TOKEN_MACADDR
 */
static boolean vines_token_macaddr (paktype *pak, long address, int *enctype,
				    uchar **daddr, uchar const **other_info,
				    ulong *rif_flags)
{
    idbtype *interface;

    interface = pak->if_output;
    if (address == 0) {
	*daddr = (uchar *)trbaddr1;
	*enctype = vines_get_default_enctype(pak->if_output);
    } else {
	*daddr = vines_get_mac_address(address);
	*enctype = vines_get_enctype(address);
    }
    *other_info = vines_llc;
    *rif_flags = determine_rif(interface, MULTIRING_BIT_VINES);
    return(TRUE);
}

/*
 * vines_et_vines_tr
 *
 * Do the VINES layer encapsulation for a Token Ring.
 * Called by MEDIA_TOKEN_MACWRITE, indexed by ET_VINES_TR.
 */
static boolean token_et_vines_tr (paktype *pak, int linktype, char *daddr,
				  char *saddr, char const *other_info,
				  riftype *rifptr)
{
    sap_hdr *llc1;
    tring_hdr *trh;
    int riflen;

    riflen = rifptr ? rifptr->length : 0;
    pak->datagramstart -= (TRING_ENCAPBYTES + ET_VINES_LEN + riflen);
    pak->datagramsize += (TRING_ENCAPBYTES + ET_VINES_LEN + riflen);

    trh = (tring_hdr *)pak->datagramstart;
    trh->ac = AC_PRI0;		/* could set priority here */
    trh->fc = FC_TYPE_LLC;
    ieee_copy(daddr, trh->daddr);
    ieee_copy(saddr, trh->saddr);
    if (riflen) {
	trh->saddr[0] |= TR_RII;	/* indicate we are here */
	bcopy(rifptr->rif, trh->data, riflen);
    }
    llc1 = (sap_hdr *)(&trh->data[riflen]);
    llc1->dsap    = other_info[0];
    llc1->ssap    = other_info[1];
    llc1->control = other_info[2];
    llc1->data[0]  = vines_demuxbyte(linktype);

    pak->mac_start  = (uchar *)trh;
    pak->addr_start = trh->daddr;
    pak->rif_start  = (riflen ? trh->data : NULL);
    pak->info_start = &trh->data[riflen];
    pak->encsize    = TRING_ENCAPBYTES + ET_VINES_LEN + riflen;
    return(TRUE);
}

/*
 * vines_fddi_macaddr
 *
 * Turn a Vines address into an encapsulation type and a MAC address
 * Called by MEDIA_FDDI_MACADDR
 */
static boolean vines_fddi_macaddr (paktype *pak, long address, int *enctype,
				   uchar **daddr, uchar const **other_info,
				   ulong *rif_flags)
{
    idbtype *interface;

    interface = pak->if_output;
    if (address == 0) {
	*daddr = (uchar *)baddr;
	*enctype = vines_get_default_enctype(pak->if_output);
    } else {
	*daddr = vines_get_mac_address(address);
	*enctype = vines_get_enctype(address);
    }
    *rif_flags = determine_rif(interface, MULTIRING_BIT_VINES);
    return(TRUE);
}

/*
 * vines_hdlc_macaddr
 *
 * Turn an Vines address into an encapsulation type and a MAC address
 * Called by MEDIA_HDLC_MACADDR, indexed by LINK_VINES
 */
static boolean vines_hdlc_macaddr (paktype *pak, long address,
				  int *enctype, ushort *flags)
{
    if (address == 0)
	*flags |= HDLC_BCAST;
    *enctype = ET_HDLC;
    return(TRUE);
}

/*
 * vines_smds_macaddr
 *
 * Turn an VINES address into an encapsulation type and a MAC address
 * Called by MEDIA_SMDS_MACADDR, indexed by LINK_VINES.
 */
static boolean vines_smds_macaddr (paktype *pak, long address, int *enctype,
				   uchar const **daddr,
				   uchar const **other_info,
				   addrtype *proto_addr)
{
    if (address) {
	*daddr = vines_get_mac_address(address);
    } else {
	*daddr = baddr;
    }

    *enctype = ET_SNAP;
    *other_info = zerosnap;
    return(TRUE);
}

/*
 * vines_x25_macaddr
 *
 * Turn an VINES address into a broadcast flag, or a filled in addrtype
 * structure that will be used to look up a static mapping.
 * Called by MEDIA_X25_MACADDR, indexed by LINK_VINES.
 */
static boolean vines_x25_macaddr (paktype *pak, long address,
				  addrtype *proto_addr, lcitype **lci,
				  boolean *broadcast)
{
    vinespathtype *path;

    if (address) {
	path = (vinespathtype *)address;
	proto_addr->type = ADDR_VINES;
	proto_addr->length = ADDRLEN_VINES;
	proto_addr->vines_addr.net = path->neighbor->net;
	proto_addr->vines_addr.host = path->neighbor->host;
	*broadcast = FALSE;
    } else {
	*broadcast = TRUE;
    }
    return(TRUE);
}

/*
 * vines_frame_macaddr
 *
 * Turn an VINES address into a broadcast flag, or a filled in addrtype
 * structure that will be used to look up a static mapping.
 * Called by MEDIA_FRAME_MACADDR, indexed by LINK_VINES.
 */
static boolean vines_frame_macaddr (paktype *pak, long address,
				    addrtype *proto_addr, boolean *broadcast)
{
    vinespathtype *path;

    if (address) {
	path = (vinespathtype *)address;
	proto_addr->type = ADDR_VINES;
	proto_addr->length = ADDRLEN_VINES;
	proto_addr->vines_addr.net = path->neighbor->net;
	proto_addr->vines_addr.host = path->neighbor->host;
	*broadcast = FALSE;
    } else {
	*broadcast = TRUE;
    }
    return(TRUE);
}

/*
 * vines_ddr_macaddr
 *
 * Turn an VINES address into a broadcast flag, or a filled in addrtype
 * structure that will be used to look up a static mapping.
 * Called by MEDIA_DDR_MACADDR, indexed by LINK_VINES.
 */
static boolean vines_ddr_macaddr (paktype *pak, long address,
				    addrtype *proto_addr, boolean *broadcast)
{
    vinespathtype *path;

    if (address) {
	path = (vinespathtype *)address;
	proto_addr->type = ADDR_VINES;
	proto_addr->length = ADDRLEN_VINES;
	proto_addr->vines_addr.net = path->neighbor->net;
	proto_addr->vines_addr.host = path->neighbor->host;
	*broadcast = FALSE;
    } else {
	*broadcast = TRUE;
    }
    pak->dialer_save_type = LINK_VINES;
    return(TRUE);
}

/*
 * vines_atm_macaddr
 *
 * Turn an VINES address into a broadcast flag, or a filled in addrtype
 * structure that will be used to look up a static mapping.
 * Called by MEDIA_FRAME_MACADDR, indexed by LINK_VINES.
 */
static boolean
vines_atm_macaddr (paktype *pak, long address, hwaddrtype *daddr, 
		   uchar const **snap_info, staticmap_type **map, 
		   boolean *broadcast)
{
    vinespathtype *path;
    addrtype proto_addr;

    *snap_info = zerosnap;
    *broadcast = FALSE;
    memset(daddr, 0, sizeof(hwaddrtype));
    if (address) {
	memset(&proto_addr, 0, sizeof(addrtype));
	path = (vinespathtype *)address;
	proto_addr.type = ADDR_VINES;
	proto_addr.length = ADDRLEN_VINES;
	proto_addr.vines_addr.net = path->neighbor->net;
	proto_addr.vines_addr.host = path->neighbor->host;
    } else {
	*broadcast = TRUE;
    }
    *map = staticmap_lookup(pak->if_output, &proto_addr, LINK_VINES,
                                                         ET_ATM, *broadcast);
    return(TRUE);
}

/**********************************************************************
 *
 *		 VINES PVC/SVC STATE CHANGE FUNCTIONS
 *
 **********************************************************************/

/*
 * vines_x25_vc_added
 *
 * This routine is called when an X.25 PVC is created or an X.25 SVC is
 * connected.  It handles anything that is X.25 specific, and then calls
 * vroute_vc_added to perform general changes.
 */
static void vines_x25_vc_added (idbtype *swidb, addrtype *proto_addr)
{
    vinesidbtype *idb;

    if (!vines_on_idb(swidb))
	return;
    
    idb = vines_getidb(swidb);
    if (vinesrtp_debug || vinestable_debug)
	buginf("\nVINESRTP: X.25 VC to %#z connected/added on interface %s.",
	       proto_addr->vines_addr.net, proto_addr->vines_addr.host,
	       swidb->namestring);
    vroute_vc_added(idb, proto_addr);
}

/*
 * vines_x25_vc_removed
 *
 * This routine is called when an X.25 PVC is deleted or an X.25 SVC is
 * cleared.  It handles anything that is X.25 specific, and then calls
 * vroute_vc_removed to perform general changes.
 */
static void vines_x25_vc_removed (idbtype *swidb, lcitype *lci)
{
    vinesidbtype *idb;

    if (!vines_on_idb(swidb))
	return;
    
    idb = vines_getidb(swidb);
    if (vinesrtp_debug || vinestable_debug)
	buginf("\nVINESRTP: X.25 VC cleared/deleted on interface %s.",
	       swidb->namestring);
    vroute_vc_removed(idb);
}


/**********************************************************************
 *
 *		       VINES MEDIA REGISTRATION
 *
 **********************************************************************/

/*
 * vines_media_registry
 *
 * Register media dependent functions for Vines
 */
void vines_media_registry (void)
{
    reg_add_proto_address(LINK_VINES,
		      vines_build_address, "vines_build_address");
    reg_add_sprintf_address_link(LINK_VINES,
		      vines_sprintf_address, "vines_sprintf_address");
    reg_add_sprintf_address_addr(ADDR_VINES,
		      vines_sprintf_address, "vines_sprintf_address");
    reg_add_build_packet_address(LINK_VINES, vines_build_packet_address,
				 "vines_build_packet_address");
    reg_add_sprintf_src_dst(LINK_VINES,
		      vines_sprintf_src_dst, "vines_sprintf_src_dst");
    reg_add_access_check(LINK_VINES, vines_accesscheck, "vines_accesscheck");
    reg_add_valid_access_list(LINK_VINES,
		      vines_valid_access_list, "vines_valid_access_list");

    reg_add_media_getlink(ET_VINES_TR, vines_getlink, "vines_getlink");

    reg_add_media_ether_macaddr(LINK_VINES, vines_ether_macaddr,
				"vines_ether_macaddr");
    reg_add_media_ether_macaddr(LINK_VINES_ECHO, vines_ether_macaddr,
				"vines_ether_macaddr");
    reg_add_media_ether_macaddr(LINK_VINES_LOOP, vines_ether_macaddr,
				"vines_ether_macaddr");

    reg_add_media_token_macwrite(ET_VINES_TR, token_et_vines_tr,
				 "token_et_vines_tr");
    reg_add_media_token_macaddr(LINK_VINES, vines_token_macaddr,
				"vines_token_macaddr");
    reg_add_media_token_macaddr(LINK_VINES_ECHO, vines_token_macaddr,
				"vines_token_macaddr");
    reg_add_media_token_macaddr(LINK_VINES_LOOP, vines_token_macaddr,
				"vines_token_macaddr");

    reg_add_media_fddi_macaddr(LINK_VINES, vines_fddi_macaddr,
			       "vines_fddi_macaddr");
    reg_add_media_fddi_macaddr(LINK_VINES_ECHO, vines_fddi_macaddr,
			       "vines_fddi_macaddr");
    reg_add_media_fddi_macaddr(LINK_VINES_LOOP, vines_fddi_macaddr,
			       "vines_fddi_macaddr");

    reg_add_media_hdlc_macaddr(LINK_VINES, vines_hdlc_macaddr,
			       "vines_hdlc_macaddr");
    reg_add_media_hdlc_macaddr(LINK_VINES_ECHO, vines_hdlc_macaddr,
			       "vines_hdlc_macaddr");
    reg_add_media_hdlc_macaddr(LINK_VINES_LOOP, vines_hdlc_macaddr,
			       "vines_hdlc_macaddr");

    reg_add_media_smds_macaddr(LINK_VINES, vines_smds_macaddr,
			       "vines_smds_macaddr");
    reg_add_media_smds_macaddr(LINK_VINES_ECHO, vines_smds_macaddr,
			       "vines_smds_macaddr");
    reg_add_media_smds_macaddr(LINK_VINES_LOOP, vines_smds_macaddr,
			       "vines_smds_macaddr");

    reg_add_media_x25_macaddr(LINK_VINES, vines_x25_macaddr,
			      "vines_x25_macaddr");
    reg_add_media_x25_pvc_create(LINK_VINES, vines_x25_vc_added,
				 "vines_x25_vc_added");
    reg_add_media_x25_pvc_delete(LINK_VINES, vines_x25_vc_removed,
				 "vines_x25_vc_removed");
    reg_add_media_x25_svc_connect(LINK_VINES, vines_x25_vc_added,
				  "vines_x25_vc_added");
    reg_add_media_x25_svc_clear(LINK_VINES, vines_x25_vc_removed,
				"vines_x25_vc_removed");

    reg_add_media_frame_macaddr(LINK_VINES, vines_frame_macaddr,
				"vines_frame_macaddr");

    reg_add_media_ddr_macaddr(LINK_VINES, vines_ddr_macaddr,
				"vines_ddr_macaddr");

    reg_add_media_atm_macaddr(LINK_VINES, vines_atm_macaddr,
			      "vines_atm_macaddr");
}
