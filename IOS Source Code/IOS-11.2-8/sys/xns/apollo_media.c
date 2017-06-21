/* $Id: apollo_media.c,v 3.4.6.3 1996/06/28 23:33:14 hampton Exp $
 * $Source: /release/112/cvs/Xsys/xns/apollo_media.c,v $
 *------------------------------------------------------------------
 * apollo_media.c -- media dependent Apollo routines.
 *
 * 13-April-1993, Kirk Lougheed
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: apollo_media.c,v $
 * Revision 3.4.6.3  1996/06/28  23:33:14  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.4.6.2  1996/06/18  01:50:23  hampton
 * Split the monolithic traffic array into smaller per-protocol traffic
 * arrays.  [CSCdi59224]
 * Branch: California_branch
 *
 * Revision 3.4.6.1  1996/03/18  22:52:26  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.26.3  1996/03/16  07:58:53  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.3.26.2  1996/03/07  11:21:24  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.3.26.1  1996/02/20  21:58:22  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.4  1996/03/02  23:33:08  rchandra
 * CSCdi50381:  IP: show ip arp command is not flexible
 * - make 'show ip arp [<address>] [<hostname>] [<mac>] [<interface>]' work
 *
 * Revision 3.3  1995/11/17  19:20:41  gstovall
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:53:07  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:57:04  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/09/11  23:58:23  hampton
 * Convert XNS and Apollo to fully use the passive timers macros instead
 * of referencing the system clock directly.  [CSCdi39966]
 *
 * Revision 2.1  1995/06/07 23:26:51  hampton
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
#include "../ui/debug.h"
#include "../if/network.h"
#include "../if/static_map.h"
#include "../if/ether.h"
#include "../if/fddi.h"
#include "../if/rif.h"
#include "../if/arp.h"
#include "../if/arp_debug.h"
#include "xns.h"
#include "apollo.h"
#include "../wan/smds.h"
#include "../x25/x25.h"
#include "../smf/smf_public.h"
#include "../wan/dialer.h"

/*
 * External Declarations
 */
uchar apollo_multicast[IEEEBYTES] = {0x09, 0x00, 0x1E, 0x00, 0x00, 0x00};
uchar apollo_arpmulti[IEEEBYTES]  = {0x09, 0x00, 0x1E, 0x00, 0x00, 0x01};
uchar apollo_bdcast[IEEEBYTES]    = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};


/*
 * apollo_arp_input
 *
 * We've received an ARP packet for the Apollo protocol
 */
static void apollo_arp_input (paktype *pak)
{
    rfc_arp *arp;
    boolean reply_flag;
    boolean merge_flag;
    int bucket;
    arptype *ptr;
    ulong temp;
    
    arp = (rfc_arp *)rfc_arp_headstart(pak);

    /*
     * If we're not running Apollo on the input interface,
     * then pass packet to bridging code.
     */
    if (!RUNNING_IF(LINK_APOLLO, pak->if_input)) {
	net_bridge_enqueue(pak);
	return;
    }
    arp = (rfc_arp *)pak->network_start;
    if (arp_debug)
	    buginf("\nAPOLLO ARP: rcvd %s src %x %e, dst %x",
		   arp->opcode == OP_REQ ? "req" : "rep",
		   GETLONG(&arp->ipspro), arp->ipshdw, arp->iptpro);
    /*
     * Minor sanity checking.  Apollo hosts are well behaved.
     * Check hardware type and length as well as protocol length.
     */
    if (((arp->arhrd != HW_ETH) && (arp->arhrd != HW_802))
	|| (arp->arhln != HL_ETH) || (arp->arpln != PL_APOLLO)) {
	retbuffer(pak);
	return;
    }

    /*
     * Check and remember if we need to generate a reply.
     */
    if ((arp->opcode == OP_REQ) && (arp->iptpro  == apollo_address))  {
	arp_traffic.reqrcv++;
	reply_flag = TRUE;		/* reply to requests */
    } else {
	arp_traffic.reprcv++;
	reply_flag = FALSE;
    }

    /*
     * See if we've heard of this guy before.  If we have, update her
     * hardware address and the reference time and set the merge_flag.
     */
    merge_flag = FALSE;		   /* we've not yet seen this guy */
    bucket = nethash(GETLONG(&arp->ipspro)); /* determine hash bucket */
    ptr = arptable[bucket];	   /* set pointer to hash chain */
    while (ptr) {
	if ((ptr->type == LINK_APOLLO) && (ptr->address == GETLONG(&arp->ipspro)) &&
	    (ptr->idb == pak->if_input)) {
	    ieee_copy(arp->ipshdw, ptr->hardware);
	    GET_TIMESTAMP(ptr->time);	/* update last reference */
	    ptr->idb = pak->if_input;   /* update interface pointer */
	    merge_flag = TRUE;    	/* we've updated a record */
	    break;		        /* quit scanning hash chain */
	}
	ptr = ptr->next;
    }

    /*
     * Create an entry if we've never seen this host before.
     */
    if (merge_flag == FALSE) {
	ptr = malloc(sizeof(arptype));
	if (!ptr)
	    return;
	if (arp_debug) {
	    buginf("\nAPOLLO ARP: Creating entry for %x, hw: %e",
		   GETLONG(&arp->ipspro), arp->ipshdw);
	}
	ieee_copy(arp->ipshdw, ptr->hardware); /* set hardware addr */
	ptr->type = LINK_APOLLO; /* set protocol type */
	ptr->idb = pak->if_input; /* remember interface */
	ptr->address = GETLONG(&arp->ipspro); /* set protocol address */
	ptr->enctype = ET_ARPA;  /* set encapsulation method */
	ptr->mode = ARP_DYNAMIC; /* entry can be timed out */
	ptr->flag = 0;		/* used for flags  */
	GET_TIMESTAMP(ptr->time);	/* set reference time */
	ptr->next = arptable[bucket]; /* install new entry... */
	arptable[bucket] = ptr;	/* ...at head of chain */
    }

    /*
     * If no reply desired, discard datagram now and return
     */
    if (!reply_flag) {
	retbuffer(pak);
	return;
    }

    /*
     * We're going to send a reply.
     * Set target hardware address to that of originator
     * Set source hardware address to our own.
     */
    arp_traffic.repsent++;
    arp->opcode = OP_REP;
    ieee_copy(arp->ipshdw, arp->ipthdw);
    ieee_copy(pak->if_input->hwptr->hardware, arp->ipshdw);
    temp = GETLONG(&arp->ipspro);
    PUTLONG(&arp->ipspro, arp->iptpro);
    arp->iptpro = temp;
    arp_send(pak, APOLLO_RBYTES, arp->ipthdw, LINK_ARP,
	     pak->if_input, ET_ARPA, LINK_APOLLO);
}

/*
 * apollo_arp_lookup
 *
 * Do Apollo address resolution
 */
static arptype *apollo_arp_lookup (ulong destination, idbtype *interface)
{
    arptype *ptr;
    rfc_arp *arp;
    paktype *pak;

    /*
     * First we look in the AR cache.  If it isn't there,
     * generate the appropriate resolution request.
     */	 
    ptr = (arptype *)arp_table_lookup(LINK_APOLLO, destination, interface);
    if (ptr)
	return(ptr);
    pak = getbuffer(RBYTES);
    if (!pak)
	return(NULL);
    arp = (rfc_arp *)rfc_arp_headstart(pak);

    arp_traffic.reqsent++;
    arp->arhln = HL_ETH;
    arp->arhrd = HW_ETH;
    arp->opcode = OP_REQ;
    arp->arpro = PR_APOLLO;
    arp->arpln = PL_APOLLO;
    ieee_copy(interface->hwptr->hardware, arp->ipshdw);
    ieee_zero(arp->ipthdw);
    PUTLONG(&arp->ipspro, apollo_address);
    arp->iptpro = destination;
    if (arp_debug)
	buginf("\nAPOLLO ARP: sent %s src %x %e, dst %x %e",
	       arp->opcode == OP_REQ ? "req" : "rep",
	       GETLONG(&arp->ipspro), arp->ipshdw, arp->iptpro, arp->ipthdw);
    arp_send(pak, APOLLO_RBYTES, apollo_arpmulti, LINK_ARP, interface,
	     ET_ARPA, LINK_APOLLO);
    return(NULL);
}

/*
 * apollo_arp_callback
 *
 * Protocol specific string and address to be printed in "show apollo arp".
 */
static int apollo_arp_string (arptype *ptr)
{			  
    apollosb	*asb;
    ulong	net = 0;

    asb = idb_get_swsb_inline(ptr->idb, SWIDB_SB_APOLLO);
    if (asb)
	net = asb->apollonet;
    return (printf("\nApollo    %x.%x", net, ptr->address));
}

/*
 * apollo_arp_display
 *
 * Show the Apollo portion of the ARP table
 */
void apollo_arp_display (void)
{
    arp_display(LINK_APOLLO, apollo_arp_string, NULL, 0, NULL);
}

/*
 * apollo_arp_reload
 * Insert our addresses into the ARP table
 */

static void apollo_arp_reload (idbtype *idb)
{
    hwaddrtype	hwaddr;
    apollosb	*asb;

    hwaddr.type = STATION_IEEE48;
    hwaddr.length = STATIONLEN_IEEE48;
    bcopy(idb->hwptr->hardware, &hwaddr.addr, hwaddr.length);
    asb = idb_get_swsb_inline(idb, SWIDB_SB_APOLLO);
    if (   (asb != NULL) && (asb->apollonet != 0)
	&& (idb->hwptr->status & IDB_IEEEMASK))
	arp_table_add(idb, apollo_address, LINK_APOLLO, &hwaddr,
		      ET_ARPA, ARP_INTERFACE);
}

/*
 * apollo_build_address
 *
 * Build an addrtype data structure containing the APOLLO address
 * currently in use on this interface.
 */
static void apollo_build_address (
    idbtype *swidb,
    paktype *pak,
    addrtype *addr)
{
    apollosb	*asb;
    ulong	net = 0;

    asb = idb_get_swsb_inline(swidb, SWIDB_SB_APOLLO);
    if (asb)
	net = asb->apollonet;
    memset(addr, 0, sizeof(addrtype));
    addr->type = ADDR_APOLLO;
    addr->length = ADDRLEN_APOLLO;
    addr->apollo_addr.net = net;
    bcopy(&apollo_address, addr->apollo_addr.host + 2, 4);
    addr->apollo_addr.host[0] = 0;
    addr->apollo_addr.host[1] = 0;
}

/*
 * apollo_sprintf_address
 *
 * Take an addrtype data structure and print it as an APOLLO address.
 */
static void apollo_sprintf_address (
    char *string,
    addrtype *addr)
{
    ulong host;
    bcopy(&(addr->apollo_addr.host[2]), &host, sizeof(host));
    sprintf(string, "%x.%x", addr->apollo_addr.net, host);
}

/*
 * apollo_sprintf_src_dst
 *
 * Given a packet, extract and print an APOLLO address.
 */
static void apollo_sprintf_src_dst (
    char *string,
    paktype *pak,
    enum SRCDST which)
{
    apollohdrtype *apollo;
    ulong host;

    apollo = (apollohdrtype *)apolloheadstart(pak);

    if (which == SOURCE) {
	bcopy(&(apollo->shost[2]), &host, sizeof(host));
	sprintf(string, "%x.%x", GETLONG(&apollo->snet), host);
    } else {
	bcopy(&(apollo->dhost[2]), &host, sizeof(host));
        sprintf(string, "%x.%x", GETLONG(&apollo->dnet), host);
    }
}

/*
 * apollo_macaddrs_update
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
static void apollo_macaddrs_update (idbtype *idb)
{
    if (apollo_on_idb(idb)) {
	reg_invoke_tbridge_smf_update(apollo_multicast,
				      idb,
				      SMF_MACTYP_APOLLO_MCAST |
				      SMF_ACTION_RECEIVE);
	reg_invoke_tbridge_smf_update(apollo_arpmulti,
				      idb,
				      SMF_MACTYP_APOLLO_MCAST |
				      SMF_ACTION_RECEIVE);
	if_multi_add(idb->hwptr, idb, apollo_multicast);
	if_multi_add(idb->hwptr, idb, apollo_arpmulti);
    } else {
	reg_invoke_tbridge_smf_delete(apollo_multicast,
				      idb,
				      SMF_MACTYP_APOLLO_MCAST |
				      SMF_ACTION_RECEIVE);
	reg_invoke_tbridge_smf_delete(apollo_arpmulti,
				      idb,
				      SMF_MACTYP_APOLLO_MCAST |
				      SMF_ACTION_RECEIVE);
    }
}

/*
 * ether_et_apollo
 *
 * Encapsulation for Apollo Domain on Ethernets
 * Called by MEDIA_ETHER_MACWRITE
 */
static boolean ether_et_apollo (paktype *pak, hwidbtype *hwidb, int linktype,
				char *daddr, char const *other_info)
{
    ether_apollo *apollo;

    pak->datagramstart -= sizeof(ether_apollo);
    pak->datagramsize += sizeof(ether_apollo);
    apollo = (ether_apollo *)pak->datagramstart;
    ieee_copy(daddr, apollo->daddr);
    ieee_copy(hwidb->hardware, apollo->saddr);
    apollo->type = (ushort)link2ether(linktype);
    return(TRUE);
}

/*
 * apollo_ether_macaddr
 *
 * Turn an Apollo address into an encapsulation type and a MAC address
 * Called by MEDIA_ETHER_MACADDR
 */

static boolean apollo_ether_macaddr (paktype *pak, long address, int *enctype,
				     uchar **daddr, uchar const **other_info)
{
    arptype *ptr;
    ulong dest;

    *enctype = ET_APOLLO;
    if ((uchar *)address != apollo_bdcast) {
	bcopy((uchar *)address+2, (uchar *)&dest, sizeof(dest));
	ptr = apollo_arp_lookup(dest, pak->if_output);
	if (!ptr)
	    return(FALSE);
	*daddr = (uchar *)ptr->hardware;
    } else
	*daddr = apollo_multicast;
    return(TRUE);
}

/*
 * fddi_et_apollo
 *
 * Fixed offset encapsulation for Apollo Domain on FDDI
 * Called by MEDIA_FDDI_MACWRITE
 */
static boolean fddi_et_apollo (paktype *pak, hwidbtype *hwidb, int linktype,
			       char *daddr, char *saddr,
			       char const *other_info, riftype *rifptr)
{
    fddi_apollo *apollo;

    pak->datagramstart -= FDDI_APOLLO_ENCAPBYTES;
    pak->datagramsize += FDDI_APOLLO_ENCAPBYTES;
    apollo = (fddi_apollo *)pak->datagramstart;
    ieee_copy(daddr, apollo->daddr);
    ieee_copy(hwidb->hardware, apollo->saddr);
    apollo->dsap    = SAP_SNAP;
    apollo->ssap    = SAP_SNAP;
    apollo->control = LLC1_UI;
    apollo->oui[0]  = other_info[0];
    apollo->oui[1]  = other_info[1];
    apollo->oui[2]  = other_info[2];
    PUTSHORT(apollo->type, link2ether(linktype));
    return(TRUE);
}

/*
 * apollo_fddi_macaddr
 *
 * Turn an Apollo address into an encapsulation type and a MAC address
 * Called by MEDIA_FDDI_MACADDR
 */
static boolean apollo_fddi_macaddr (paktype *pak, long address, int *enctype,
				    uchar **daddr, uchar const **other_info,
				    ulong *rif_flags)
{
    arptype *ptr;
    ulong dest;

    *enctype = ET_APOLLO;
    if ((uchar *)address != apollo_bdcast) {
	bcopy((uchar *)address+2, (uchar *)&dest, sizeof(dest));
	ptr = apollo_arp_lookup(dest, pak->if_output);

	if (!ptr)			/* didn't find ARP entry */
	    return(FALSE);
	*daddr = (uchar *)ptr->hardware;
    } else
	*daddr = apollo_multicast;
    *rif_flags = RIF_NOT_PRESENT;
    return(TRUE);
}

/*
 * apollo_smds_macaddr
 *
 * Turn an APOLLO address into an encapsulation type and a MAC address
 * Called by MEDIA_SMDS_MACADDR, indexed by LINK_APOLLO.
 */
static boolean apollo_smds_macaddr (paktype *pak, long address, int *enctype,
				    uchar const **daddr,
				    uchar const **other_info,
				    addrtype *proto_addr)
{
    apollosb	*asb;
    ulong	net = 0;

    if ((uchar *)address != apollo_bdcast) {
	proto_addr->type = ADDR_APOLLO;
	proto_addr->length = ADDRLEN_APOLLO;
	asb = idb_get_swsb_inline(pak->if_output, SWIDB_SB_APOLLO);
	net = (asb != NULL) ? asb->apollonet : 0;
	proto_addr->apollo_addr.net = net;
	bcopy((uchar *)address, &proto_addr->apollo_addr.host,
	      ADDRLEN_APOLLO);
	*daddr = NULL;
    } else {
	*daddr = baddr;
    }

    *enctype = ET_SNAP;
    *other_info = zerosnap;
    return (TRUE);
}

/*
 * apollo_x25_macaddr
 *
 * Turn an APOLLO address into a broadcast flag, or a filled in addrtype
 * structure that will be used to look up a static mapping.
 * Called by MEDIA_X25_MACADDR, indexed by LINK_APOLLO.
 */
static boolean apollo_x25_macaddr (paktype *pak, long address,
				   addrtype *proto_addr, lcitype **lci,
				   boolean *broadcast)
{
    idbtype	*idb;
    apollosb	*asb;
    ulong	net = 0;

    idb = get_pak_netidb(pak);
    if (!idb) {
	return(FALSE);
    }
    if (!ieee_equal((uchar *)address, apollo_bdcast)) {
	proto_addr->type = ADDR_APOLLO;
	proto_addr->length = ADDRLEN_APOLLO;
	asb = idb_get_swsb_inline(idb, SWIDB_SB_APOLLO);
	if (asb)
	    net = asb->apollonet;
	proto_addr->apollo_addr.net = net;
	bcopy((uchar *) address, &proto_addr->apollo_addr.host,
	      ADDRLEN_APOLLO);
    } else {
	*broadcast = TRUE;
    }
    return(TRUE);
}

/*
 * apollo_frame_macaddr
 *
 * Turn an APOLLO address into a broadcast flag, or a filled in addrtype
 * structure that will be used to look up a static mapping.
 * Called by MEDIA_FRAME_MACADDR, indexed by LINK_APOLLO.
 */
static boolean apollo_frame_macaddr (paktype *pak, long address,
				     addrtype *proto_addr, boolean *broadcast)
{
    idbtype	*idb;
    apollosb	*asb;
    ulong	net = 0;

    idb = pak->if_output;
    if (!ieee_equal((uchar *)address, apollo_bdcast)) {
	proto_addr->type = ADDR_APOLLO;
	proto_addr->length = ADDRLEN_APOLLO;
	asb = idb_get_swsb_inline(idb, SWIDB_SB_APOLLO);
	if (asb)
	    net = asb->apollonet;
	proto_addr->apollo_addr.net = net;
	bcopy((uchar *) address, &proto_addr->apollo_addr.host,
	      ADDRLEN_APOLLO);
    } else {
	*broadcast = TRUE;
    }
    return(TRUE);
}

/*
 * apollo_atm_macaddr
 *
 * Turn an APOLLO address into a broadcast flag, or a filled in addrtype
 * structure that will be used to look up a static mapping.
 * Called by MEDIA_ATM_MACADDR, indexed by LINK_APOLLO.
 */
static boolean apollo_atm_macaddr (paktype *pak, long address, 
				    hwaddrtype *daddr, uchar const **snap_info,
				    staticmap_type **map, boolean *broadcast)
{
    idbtype	*idb;
    arptype	*ptr;
    addrtype	proto_addr;
    ulong	dest;
    apollosb	*asb;
    ulong	net = 0;

    *snap_info = zerosnap;
    idb = pak->if_output;
    memset(daddr, 0, sizeof(hwaddrtype));
    *broadcast = FALSE;
    if (!ieee_equal((uchar *)address, apollo_bdcast)) {
	memset(&proto_addr, 0, sizeof(addrtype));
	proto_addr.type = ADDR_APOLLO;
	proto_addr.length = ADDRLEN_APOLLO;
	asb = idb_get_swsb_inline(idb, SWIDB_SB_APOLLO);
	if (asb)
	    net = asb->apollonet;
	proto_addr.apollo_addr.net = net;
	bcopy((uchar *)address+2, &proto_addr.apollo_addr.host, APOLLOADDRBYTES);
	bcopy((uchar *)address+2, (uchar *)&dest, sizeof(dest));
	ptr = apollo_arp_lookup(dest, pak->if_output);
	if (ptr) {
	    daddr->length = ptr->hwlen;
	    bcopy(ptr->hardware, daddr->addr, ptr->hwlen);
	    return(TRUE);
	}
    } else {
	*broadcast = TRUE;
    }
    if ((*map = staticmap_lookup(pak->if_output, &proto_addr, LINK_APOLLO,
                                                         ET_ATM, *broadcast)))
        return(TRUE);
    else
        return(FALSE);
}

/*
 * apollo_media_registry
 * Register media specific functions
 */

void apollo_media_registry (void)
{
    reg_add_arp_enq(PR_APOLLO, apollo_arp_input, "apollo_arp_input");
    reg_add_arp_reload(apollo_arp_reload, "apollo_arp_reload");
    reg_add_proto_address(LINK_APOLLO,
		      apollo_build_address, "apollo_build_address");
    reg_add_sprintf_address_link(LINK_APOLLO,
		      apollo_sprintf_address, "apollo_sprintf_address");
    reg_add_sprintf_address_addr(ADDR_APOLLO,
		      apollo_sprintf_address, "apollo_sprintf_address");
    reg_add_sprintf_src_dst(LINK_APOLLO,
		      apollo_sprintf_src_dst, "apollo_sprintf_src_dst");

    reg_add_media_interesting_addresses(apollo_macaddrs_update,
					"apollo_macaddrs_update");

    reg_add_media_ether_macaddr(LINK_APOLLO, apollo_ether_macaddr,
				"apollo_ether_macaddr");
    reg_add_media_ether_macwrite(ET_APOLLO, ether_et_apollo,
				 "ether_et_apollo");
    reg_add_media_fddi_macaddr(LINK_APOLLO, apollo_fddi_macaddr,
			       "apollo_fddi_macaddr");
    reg_add_media_fddi_macwrite(ET_APOLLO, fddi_et_apollo, "fddi_et_apollo");
    reg_add_media_smds_macaddr(LINK_APOLLO, apollo_smds_macaddr,
			       "apollo_smds_macaddr");
    reg_add_media_x25_macaddr(LINK_APOLLO, apollo_x25_macaddr,
			      "apollo_x25_macaddr");
    reg_add_media_frame_macaddr(LINK_APOLLO, apollo_frame_macaddr,
				"apollo_frame_macaddr");

    reg_add_media_atm_macaddr(LINK_APOLLO, apollo_atm_macaddr,
			      "apollo_atm_macaddr");
}
