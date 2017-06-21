/* $Id: probe.c,v 3.4.26.7 1996/08/28 12:50:18 thille Exp $
 * $Source: /release/112/cvs/Xsys/ip/probe.c,v $
 *------------------------------------------------------------------
 * probe.c -- Hewlett Packard 802.2 Probe Protocol
 *
 * 11-October-1986, Kirk Lougheed
 *
 * Copyright (c) 1986-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: probe.c,v $
 * Revision 3.4.26.7  1996/08/28  12:50:18  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.4.26.6  1996/07/06  05:53:56  thille
 * CSCdi62136:  seq: iphost and req: iphost duplicated and waste memory
 * Branch: California_branch
 * Put copies of the strings in common_strings.c/h and reference them
 * instead of having multiple copies of the literals.  Save 660 bytes.
 *
 * Revision 3.4.26.5  1996/07/02  23:11:17  wmay
 * CSCdi34302:  Protocol accounting not done correctly for dialer ints
 * do receive accounting for dialer ints on the actual input interface
 * Branch: California_branch
 *
 * Revision 3.4.26.4  1996/06/18  01:45:59  hampton
 * Split the monolithic traffic array into smaller per-protocol traffic
 * arrays.  [CSCdi59224]
 * Branch: California_branch
 *
 * Revision 3.4.26.3  1996/05/04  01:04:57  wilber
 * Branch: California_branch
 *
 * Integrated Routing and Bridging
 * - ARP, IP protocol, fast-switching and smf support for the Bridge-group
 *   Virtual Interface.
 * - Set up bridge/route indicators based on CRB/IRB is enabled or not.
 *
 * Constrained Multicast Flooding
 * - Convert tbridge flood array to a linked list of tbifd's.
 *
 * Name Access List
 *
 * Revision 3.4.26.2  1996/04/15  21:16:45  bcole
 * CSCdi54649:  IP Host Modularity, IP Automatic Stub Routing feature
 * commit
 * Branch: California_branch
 *
 * Revision 3.4.26.1  1996/03/18  20:17:42  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.4.2.2  1996/03/13  01:38:15  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.4.2.1  1996/03/07  09:47:03  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.4  1996/01/19  00:57:44  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.3  1995/11/17  09:36:11  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:41:01  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:58:02  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/10/25  17:18:34  albright
 * CSCdi41952:  HP probe no longer working
 *
 * Revision 2.3  1995/09/13  07:01:36  hampton
 * All code should use the routine GETSHORT()/PUTSHORT() instead of using
 * the routines getshort()/putshort().  [CSCdi40249]
 *
 * Revision 2.2  1995/09/11 23:49:32  hampton
 * Convert IP host and IP routing to fully use the passive timers macros
 * instead of referencing the system clock directly.  [CSCdi40085]
 *
 * Revision 2.1  1995/06/07 21:00:31  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include <ciscolib.h>
#include "interface_private.h"
#include "packet.h"
#include "config.h"
#include "sys_registry.h"
#include "../ip/ip_registry.h"
#include "../ui/debug.h"
#include "ieee.h"
#include "ip_media.h"
#include "../if/network.h"
#include "ip.h"
#include "probe.h"
#include "address.h"
#include "name.h"
#include "../if/arp_debug.h"
#include "logger.h"
#include "../os/msg_sched.c"		/* Not a typo, see logger.h */
#include "../smf/smf_public.h"
#include "media_registry.h"
#include "subsys.h"
#include "../srt/srt_registry.h"

/*
 * hpcast1 is the primary HP multicast address.  hpcast2 is the seconday HP
 * multicast address. 
 * hpcode is an extended SAP.  See the explanation in probe.h
 */

static const uchar hpcode[HPCODELEN] = {0,0,0,5,3,5,3};
static uchar hpcast1[IEEEBYTES] = {9,0,9,0,0,1};
static uchar hpcast2[IEEEBYTES] = {9,0,9,0,0,2};
static watched_queue *probeQ;
static ushort probe_sequence;
static probesenttype requests[MAXPROBES];
static pid_t probe_pid;
static probe_traffic_t probe_traffic;


/*
 * probe_macaddrs_update
 * See comment for ip_macaddrs_update
 */

static void probe_macaddrs_update (idbtype *swidb)
{
    hwidbtype *hwidb;

    hwidb = swidb->hwptr;

    if (is_bvi(hwidb)) {
	if (swidb->arp_probe) {
	    reg_invoke_bvi_smf_update(hpcast1, swidb,
				      SMF_MACTYP_HP_MCAST|SMF_ACTION_RECEIVE,
				      IDB_BRIDGE);
	} else {
	    reg_invoke_bvi_smf_delete(hpcast1, swidb,
				      SMF_MACTYP_HP_MCAST|SMF_ACTION_RECEIVE,
				      IDB_BRIDGE);
	}
	if (swidb->ip_probe_name) {
	    reg_invoke_bvi_smf_update(hpcast2, swidb,
				      SMF_MACTYP_HP_MCAST|SMF_ACTION_RECEIVE,
				      IDB_BRIDGE);
	} else {
	    reg_invoke_bvi_smf_delete(hpcast2, swidb,
				      SMF_MACTYP_HP_MCAST|SMF_ACTION_RECEIVE,
				      IDB_BRIDGE);
	}
	return;
    }

    if (swidb->arp_probe) {
	reg_invoke_tbridge_smf_update(hpcast1, swidb,
				      SMF_MACTYP_HP_MCAST|SMF_ACTION_RECEIVE);
	if_multi_add(hwidb, swidb, hpcast1);
    } else {
	reg_invoke_tbridge_smf_delete(hpcast1, swidb,
				      SMF_MACTYP_HP_MCAST|SMF_ACTION_RECEIVE);
    }
    if (swidb->ip_probe_name) {
	reg_invoke_tbridge_smf_update(hpcast2, swidb,
				      SMF_MACTYP_HP_MCAST|SMF_ACTION_RECEIVE);
	if_multi_add(hwidb, swidb, hpcast2);
    } else {
	reg_invoke_tbridge_smf_delete(hpcast2, swidb,
				      SMF_MACTYP_HP_MCAST|SMF_ACTION_RECEIVE);
    }
}

/*
 * probe_init
 * Here to initialize HP probe subsystem
 */

static void probe_init (subsystype *dummy)
{
    ushort i;

    /*
     * Register Probe parse chains
     */
    probe_parser_init();
    reg_add_ip_show_traffic(probe_show_traffic, "probe_show_traffic");
    reg_add_media_interesting_addresses(probe_macaddrs_update,
					"probe_macaddrs_update");
    reg_add_arp_sendreq(ET_SAP, probe_sendreq, "PROBE Send Request");

    probe_pid = 0;
    probe_sequence = 0;
    probeQ = NULL;
    memset(&probe_traffic, 0, sizeof(probe_traffic));
    for (i = 0; i < MAXPROBES; i++)
	requests[i].address = 0;
    if (nethernets) {
	probe_pid = process_create(probe_input, "Probe Input", NORMAL_STACK,
				   PRIO_LOW);
	if (probe_pid == NO_PROCESS)
	    probe_pid = 0;
    }
}

/*
 * probe_enqueue
 * Receive a Probe datagram at interrupt level.
 */

void probe_enqueue (paktype *pak)
{
    if (BRIDGING_IF(pak->if_input, LINK_IP)) {
	net_bridge_enqueue_broadcast(pak);
    } else if (probe_pid) {
	process_enqueue_pak(probeQ, pak);
    } else {
	protocol_discard(pak, TRUE);
    }
}

/*
 * probe_process_input
 *
 * Process any packets on the Probe input queue/
 */
static void probe_process_input (void)
{
    paktype *pak;
    probetype *probe;
    probenametype *probename;
    idbtype *idb;
    ipaddrtype address;
    uchar hardware[IEEEBYTES], dest[IEEEBYTES];
    int count;
    probepath *path;
    hwaddrtype hwaddr;

    count = PAK_SWITCHCOUNT;
    while (TRUE) {
	if (--count == 0)
	    break;
	pak = process_dequeue(probeQ);
	if (!pak)
	    break;
	idb = pak->if_input;

	(*idb->hwptr->extract_snpa)(pak, &hwaddr, SOURCE);
	ieee_copy(hwaddr.addr, hardware);
	(*idb->hwptr->extract_snpa)(pak, &hwaddr, DESTINATION);
	ieee_copy(hwaddr.addr, dest);

	if (pak->enctype == ET_ARPA) {
	    probe = (probetype *)(pak->network_start - HPEXTEND_PAD);
	} else {
	    probe = (probetype *)pak->network_start;
	}

	/*
	 * HP has these wonderful terminal servers called DTC's.  They
	 * speak a LAT-like protocol called AFCP.  But they _also_
	 * generate probe packets.  The only way to differentiate DTC
	 * probe from a real probe packet is via the extended SAP.
	 * DTC probe should be bridged, if bridging is configured.
	 */

	bcopy(&probe->hpcode[HPEXTEND_PAD], (uchar *) &address,
	      HPCODELEN - HPEXTEND_PAD);

	/*
	 * If it's DTC probe, offer it up for bridging.
	 */
	if (address != HPEXTENDEDSAP) {
	    net_bridge_enqueue_broadcast(pak);
	    continue;
	}

	/*
	 * CSCdi07039
	 * Discard probe arp packets when we are not
	 * explicitly configured to handle them.
	 */
	if (!idb->arp_probe &&
	    ((1 << probe->type) & PROBE_ARP_TYPES)) { 
	    retbuffer(pak);
	    continue;
	}

	idb_increment_rx_proto_counts(pak, ACCT_PROTO_PROBE);

	switch (probe->type) {
	  case ADDR_REPLY:
	    probe_traffic.replyrcv++;
	    address = lookup_sequence(GETSHORT(probe->sequence));
	    if (probe_debug) {
		if (address) 
		    buginf("\nPROBE: recv VNA reply %i from %e",
			   address, hardware);
		else 
		    buginf("\nPROBE: recv duplicate VNA reply %i"
			   " from %e", address, hardware);
	    }
	    probe_arp_update(idb, address, LINK_IP,
			     hardware, pak->enctype, ARP_DYNAMIC);
	    break;
	  case ADDR_REQUEST:
	    probe_traffic.reqrcv++;
	    bcopy(probe->address, (uchar *)&address, 4);
	    if (probe_debug)
		buginf("\nPROBE: recv VNA request %i from %e",
		       address, hardware);
	    if (!ip_ifaddress(idb, address)) {
		/*
		 * Crock: telling ip_arp_assist that the source
		 * address is 0, but HP didn't include a source
		 * address in the packet.
		 */
		if (!ip_arp_assist(0, address, idb, LINK_IP,
				   pak->enctype, NULL))
		    break;
		probe_traffic.replyproxy++;
	    }
	    probe_traffic.replysent++;
	    probe->type = ADDR_REPLY;
	    PUTSHORT(probe->messlen, ADDR_REPLEN);
	    if (probe_debug)
		buginf("\nPROBE: send VNA reply");
	    pak->if_output = idb;
	    pak->linktype = LINK_PROBE;
	    pak->acct_proto = ACCT_PROTO_PROBE;
	    if (pak_encap(idb->hwptr, pak, PROBETYPELENGTH, (long) hardware)) {
		datagram_out(pak);
		pak = NULL;
	    }
	    break;
	  case PROXY_REQUEST:
	    pak = hp_proxy_request(pak, idb, probe, hardware);
	    break;
	  case PROXY_REPLY:
	    if (probe_debug)
		buginf("\nPROBE: recv proxy reply from %e", hardware);
	    if (!idb->ip_probe_name) {
		net_bridge_enqueue_broadcast(pak);
		pak = NULL;
	    }
	    probe_traffic.otherrcv++;
	    break;
	  case NAME_REPLY:
	  case NAME_REQUEST:
	    if (probe_debug) 
		buginf("\nPROBE: recv name re%s from %e (ignored)",
		       probe->type == NAME_REPLY ? "ply" : "quest", 
		       hardware);
	    probe_traffic.otherrcv++;
	    break;
	  case UNSOL_REPLY:
	    probe_traffic.otherrcv++;
	    probename = (probenametype *) probe;
	    count = probename->namelen;
	    if (count & 1)
		count++;
	    path = (probepath *) ((char *)&probename->name[0] + count);
	    bcopy((uchar *) path->address, (uchar *) &address,
		  sizeof(ipaddrtype)); 
	    if (probe_debug)
		buginf("\nPROBE: recv unsolicited reply %i "
		       "from %e", address, hardware);
	    probe_arp_update(idb, address, LINK_IP,
			     hardware, pak->enctype, ARP_DYNAMIC);
	    break;
	  case WHERE_REQUEST:
	    pak = hp_whereis_request(pak, idb, (probewhereis *)probe,
				     LINK_IP, hardware);
	    break;
	  default:
	    if (probe_debug)
		buginf("\nPROBE: recv probe packet type 0x%x from %e", 
		       probe->type, hardware);
	    /*
	     * We are routing IP so we must absorb VNA request
	     * and reply packets.  Any other sort of Probe
	     * we will offer up for bridging.
	     */
	    probe_traffic.otherrcv++;
	    net_bridge_enqueue_broadcast(pak);
	    pak = NULL;    
	    break;
	    }
	retbuffer(pak);
    }
}

/*
 * probe_input
 * Handle Probe input packets
 */

process probe_input (void)
{
    ulong major, minor;

    probeQ = create_watched_queue("HP Probe Packets", 0, 0);
    process_watch_queue(probeQ, ENABLE, RECURRING);
    reg_add_raw_enqueue(LINK_PROBE, probe_enqueue, "probe_enqueue");
    while (TRUE) {
	process_wait_for_event();
	while (process_get_wakeup(&major, &minor)) {
	    switch (major) {
	      case QUEUE_EVENT:
		probe_process_input();
		break;

	      default:
		errmsg(&msgsym(UNEXPECTEDEVENT,SCHED), major, minor);
		break;
	    }
	}
    }
}

/*
 * hp_whereis_request
 * Respond to a probe where-is-gateway request.
 */

paktype *hp_whereis_request (
    paktype *pak,
    idbtype *idb,
    probewhereis *request,
    uint linktype,
    uchar *hardware)
{
    probewhereisreply *reply;
    ipaddrtype nexthop, source;
    arptype *ptr;
    idbtype *nextidb;
    int siz;

    probe_traffic.wherercv++;
    bcopy(&request->destination, &nexthop, sizeof(ipaddrtype));
    bcopy(&request->source, &source, sizeof(ipaddrtype));
    if (probe_debug)
	buginf("\nPROBE: recv where-is-gateway request from %e for %i",
	       hardware, nexthop);
    nextidb = reg_invoke_ip_first_hop(&nexthop, nexthop, IP_PATH_DEFFLAGS);
    if (!nextidb) {
	if (probe_debug)
	    buginf("\nPROBE: no route to %i", nexthop);
	return(pak);
    }

    /*
     * Make sure that there's enough room in the buffer for the reply.
     */
    siz = (int) ((uchar *) request - pak->network_start);
    if (siz + sizeof(probewhereisreply) > SMALLDATA) {
	if (probe_debug)
	    buginf("\nPROBE: buffer too small, dropped - 0x%x",
		   pak);
	return(pak);
    }

    reply = (probewhereisreply *) request;
    reply->type = WHERE_REPLY;
    PUTSHORT(reply->messlen, 30);
    PUTSHORT(reply->replen, 20);
    PUTSHORT(reply->domreplen, 18);
    reply->vers = PROBE_VERSION;
    reply->domain = HPDSN;
    PUTSHORT(reply->pathlen, 10);
    reply->pid = PID_IEEE;
    reply->elemlen = 8;
    PUTSHORT(reply->sap, 0);
    if ((nextidb == idb) && (ptr = arp_table_lookup(linktype, nexthop, idb))) {
	ieee_copy(ptr->hardware, reply->ieeeaddr);
	bcopy(&nexthop, &reply->gateway, sizeof(ipaddrtype));
    } else {
	ieee_copy(idb->hwptr->hardware, reply->ieeeaddr);
	bcopy(&idb->ip_address, &reply->gateway, sizeof(ipaddrtype));
	nexthop = idb->ip_address;
    }
    PUTSHORT(reply->hopcount, 1);
    if (probe_debug) {
	buginf("\nPROBE: send where-is-gateway reply %i (%e) "
	       "\n       to %i (%e)",
	       nexthop, reply->ieeeaddr, source,
	       hardware);
    }
    pak->if_output = idb;
    pak->linktype = LINK_PROBE;
    if (pak_encap(idb->hwptr, pak, PROBEWIRLENGTH, (long) hardware)) {
	datagram_out(pak);
	pak = NULL;
	probe_traffic.wheresent++;
    } else {
        if (probe_debug)
	    buginf("\nPROBE: encapsulation failed");
    }
    return(pak);
}

/*
 * hp_proxy_request
 * Deal with a probe proxy request.
 */

paktype *hp_proxy_request (
    paktype *pak,
    idbtype *idb,
    probetype *probe,
    uchar *hardware)
{
    probenametype *probename;
    int siz;
    nametype *ptr;
    probenamereptype *namerep;

    probename = (probenametype *) probe;
    if (probename->namelen > 50) {
	if (probe_debug)
	    buginf("\nPROBE: bogus name length: %d",
		   probename->namelen);
	return(pak);
    }
    
    /*
     * Make sure that there's enough room in the buffer for
     * us to null terminate the name and for the reply.
     */
    siz = (int) ((probename->name + probename->namelen) -
		 pak->network_start);
    if (siz + 1 > SMALLDATA ||
	SMALLDATA < PROBENAMEREPTYPELENGTH) {
	if (probe_debug)
	    buginf("\nPROBE: buffer too small, dropped - 0x%x",
		   pak);
	return(pak);
    }
    *(probename->name + probename->namelen) = '\0';
    if (probe_debug)
	buginf("\nPROBE: recv proxy request from %e: %*s", 
	       hardware, probename->namelen,
	       probename->name);
    if (!idb->ip_probe_name) {
	net_bridge_enqueue_broadcast(pak);
	return(NULL);
    }
    probe_traffic.namercv++;
    ptr = (nametype *)name_cache_lookup((char *) probename->name,NULL,NAM_HP);
    if (!ptr)
	return(pak);
    
    /*
     * Found an address, now format and send a reply.
     */
    probe_traffic.namesent++;
    pak->if_output = idb;
    pak->linktype = LINK_PROBE;
    
    /*
     * Using all of these constants here is really tacky,
     * but I have no idea of what their semantics is.
     */
    namerep = (probenamereptype *) probe;
    namerep->type = PROXY_REPLY;
    PUTSHORT(namerep->messlen, PROBENAMEREPTYPELENGTH -
	     HPCODELEN);
    PUTSHORT(namerep->reportlen, 32);
    PUTSHORT(namerep->domreportlen, 30);
    namerep->vnaversion = PROBE_VERSION;
    namerep->domain = HPDSN;
    bcopy((uchar *) &ptr->v.ip.adr[0].addr.ip_address,
	  (uchar *) namerep->address,
	  sizeof(ipaddrtype));
    PUTSHORT(namerep->pathlen, 22);
    namerep->pid1 = PID_ADVANCENET;
    namerep->pid1len = 2;
    PUTSHORT(namerep->pid1bits, CAPA_ALL);
    namerep->pid2 = PID_TRANSPORT;
    namerep->pid2len = 2;
    PUTSHORT(namerep->pid2bits, CAPA_ALL);
    namerep->pid3 = PID_IP;
    namerep->pid3len = 2;
    PUTSHORT(namerep->ipsapnum, 0);
    namerep->pid4 = PID_IEEE;
    namerep->pid4len = 8;
    PUTSHORT(namerep->ieeesap, 6);
    ieee_copy(&idb->hwptr->hardware[0], namerep->ieeeaddr);
    pak->acct_proto = ACCT_PROTO_PROBE;
    if (pak_encap(idb->hwptr, pak, PROBENAMEREPTYPELENGTH, (long) hardware)) {
	datagram_out(pak);
	pak = NULL;
    }
    if (probe_debug)
	buginf("\nPROBE: send proxy reply %i to %e",
	       ptr->v.ip.adr[0].addr.ip_address, hardware);
    return(pak);
}

/*
 * hp_probe 
 * Send an HP Probe request for the given address
 */

void probe_sendreq (paktype *dummy1, ipaddrtype address, idbtype *interface,
		    hwaddrtype *dummy2)
{
    paktype *pak;
    register probetype *probe;
    int sequence;

    sequence = lookup_address(address);
    if (!sequence)
	return;
    pak = getbuffer(SMALLDATA);
    if (!pak)
	return;
    if (probe_debug)
	buginf("\nPROBE: send VNA request %i", address);

    /*
     * First, send it out with ISO1 encapsulation.
     */
    probe = (probetype *)pak->network_start;
    bcopy(hpcode, probe->hpcode, HPCODELEN);
    probe->version = PROBE_VERSION;
    probe->type = ADDR_REQUEST;
    PUTSHORT(probe->messlen, ADDR_REQLEN);
    PUTSHORT(probe->sequence,sequence);
    PUTSHORT(probe->reportlen,REPORT_LENGTH);
    PUTSHORT(probe->domlen,DOMAIN_LENGTH);
    probe->reqversion = REQUEST_VERSION;
    probe->domain = DOMAIN_NUMBER;
    bcopy((uchar *)&address,probe->address,4);
    pak->linktype = LINK_PROBE;
    pak->if_output = interface;
    pak->enctype = ET_SAP;
    pak->acct_proto = ACCT_PROTO_PROBE;
    if (pak_encap(interface->hwptr, pak, PROBETYPELENGTH, (long) hpcast1)) {
  	datagram_out(pak);
	probe_traffic.reqsent++;
    } else
 	datagram_done(pak);

    /*
     * Have to get a new buffer to avoid a race with the driver.
     */
    pak = getbuffer(SMALLDATA);
    if (!pak)
 	return;
    if (probe_debug)
 	buginf("\nPROBE: send VNA request %i", address);

    /*
     * Send it again via Ethernet encapsulation.
     */
    probe = (probetype *)(pak->network_start - HPEXTEND_PAD);
    bcopy(&hpcode[HPEXTEND_PAD], &probe->hpcode[HPEXTEND_PAD], HPCODELEN_ARPA);
    probe->version = PROBE_VERSION;
    probe->type = ADDR_REQUEST;
    PUTSHORT(probe->messlen, ADDR_REQLEN);
    PUTSHORT(probe->sequence,sequence);
    PUTSHORT(probe->reportlen,REPORT_LENGTH);
    PUTSHORT(probe->domlen,DOMAIN_LENGTH);
    probe->reqversion = REQUEST_VERSION;
    probe->domain = DOMAIN_NUMBER;
    bcopy((uchar *)&address,probe->address,4);
    pak->linktype = LINK_PROBE;
    pak->if_output = interface;
    pak->enctype = ET_ARPA;
    pak->acct_proto = ACCT_PROTO_PROBE;
    if (pak_encap(interface->hwptr, pak, PROBETYPELENGTH, (long) hpcast1)) {
 	datagram_out(pak);
 	probe_traffic.reqsent++;
    } else
 	datagram_done(pak);
}

/*
 * lookup_sequence
 * Given a sequence number, return address if match, zero otherwise.
 */

int lookup_sequence (uint sequence)
{
    register int i, j;
   
    j = 0;
    for (i = 0; i < MAXPROBES; i++) {
	if (!requests[i].address)
	    continue;
	if (requests[i].sequence == sequence)
	    j = requests[i].address;
	if (--requests[i].decay <= 0)
	    requests[i].address = 0;
	if (j)
	    return(j);
    }
    return(0);
}

/*
 * lookup_address
 * Given an address, return a sequence number.
 * Sequence number zero indicates some failure.
 */

int lookup_address (uint address)
{
    register int i, j;

    for (i = 0, j = -1; i < MAXPROBES; i++) {
	if (!requests[i].address) {
	    if (j == -1)
		j = i;
	    continue;
	}
	if (requests[i].address == address)
	    return((int)requests[i].sequence);
	else {
	    if (--requests[i].decay <= 0) {
		requests[i].address = 0;
		if (j == -1)
		    j = i;
	    }
	}
    }
    if (j != -1) {
	if (!(++probe_sequence))
	   probe_sequence++;
	requests[j].address = address;
	requests[j].sequence = probe_sequence;
	requests[j].decay = DECAYCOUNT;
	return((int) probe_sequence);
    }
    return(0);
}

/*
 * probe_show_traffic
 * Display Probe traffic statistics
 */

void probe_show_traffic (void)
{
    printf("\n\nProbe statistics:");
    printf("\n  Rcvd: %lu address requests, %lu address replies",
	   probe_traffic.reqrcv,probe_traffic.replyrcv);
    printf("\n        %lu proxy name requests, %lu where-is requests, %lu other",
	   probe_traffic.namercv, probe_traffic.wherercv,
	   probe_traffic.otherrcv);
    printf("\n  Sent: %lu address requests, %lu address replies (%lu proxy)",
	   probe_traffic.reqsent, probe_traffic.replysent,
	   probe_traffic.replyproxy);
    printf("\n        %lu proxy name replies, %lu where-is replies",
	   probe_traffic.namesent, probe_traffic.wheresent); 
}

/*
 * probe_arp_update
 * Update the arp cache based on VNA information.  This differs somewhat
 * from the standard updating, since we do a little song and dance about
 * encapsuation.
 */

void probe_arp_update (
    idbtype *idb,
    ipaddrtype address,
    long linktype,
    uchar *hardware,
    int method,
    int flag)
{
    arptype *ptr;
    hwaddrtype hwaddr;

    /*
     * Look to see what's going on.
     */
    ptr = arp_table_lookup(linktype, address, idb);

    if (ptr) {

	/*
	 * If there is something there, and it's not dynamic or
	 * incomplete, then don't change it.  If it is very recent and
	 * not ARPA encapsulation, ignore it. Otherwise, flush and
	 * reinstall.  The flush is necessary since arp_table_add()
	 * doesn't update encapsulation types.
	 */
	if (ptr->mode == ARP_INCOMPLETE) {
	    /*
	     * It seems silly that we are doing this when we are only
	     * going to be deleting it just now.  Actually, it checks
	     * to see if mode is dynamic before allowing an
	     * invalidation.
	     */
	    ptr->mode = ARP_DYNAMIC;
	} else {
	    if (ptr->mode != ARP_DYNAMIC)
		return;
	    if ((ELAPSED_TIME(ptr->time) < ONEMIN) &&
		(ptr->enctype == ET_ARPA))
		return;
	}
    }

    /*
     * If nothing there or we just removed it, just add it.
     */
    if(method == ET_SMDS) {
	hwaddr.type = STATION_SMDS64;
	hwaddr.length = STATIONLEN_SMDS64;
    } else {
    	hwaddr.type = STATION_IEEE48;
    	hwaddr.length = STATIONLEN_IEEE48;
    }
    bcopy(hardware, &hwaddr.addr, hwaddr.length);
    arp_table_add(idb, address, linktype, &hwaddr, method, flag);
    return;
}

/*
 * PROBE subsystem header
 */

#define PROBE_MAJVERSION 1
#define PROBE_MINVERSION 0
#define PROBE_EDITVERSION  1

SUBSYS_HEADER(probe, PROBE_MAJVERSION, PROBE_MINVERSION, PROBE_EDITVERSION,
	      probe_init, SUBSYS_CLASS_PROTOCOL,
	      subsys_seq_iphost,
	      subsys_req_iphost);
