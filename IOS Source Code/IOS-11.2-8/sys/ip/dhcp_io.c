/* $Id: dhcp_io.c,v 3.3.20.3 1996/07/19 22:55:13 irfan Exp $
 * $Source: /release/112/cvs/Xsys/ip/dhcp_io.c,v $
 *------------------------------------------------------------------
 * dhcp_io.c - DHCP proxy client data movement
 *
 * Mar 10, 1994, Syed Irfan Ashraf
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 * This module handles the packet and other I/O stuff for DHCP
 * (mainly client mode) protocol.
 *------------------------------------------------------------------
 * $Log: dhcp_io.c,v $
 * Revision 3.3.20.3  1996/07/19  22:55:13  irfan
 * CSCdi63267:  dhcp hickups
 * Branch: California_branch
 * misc. dhcp proxy client improvements:
 *  - appear to the server as a relay agent to get unicasts back correctly.
 *  - tap packets from bootp server to see if they are responses directed
 *    to the dhcp proxy client (relay-agent).
 *  - make it work for lines that don't have a logged-in user.
 *  - reduce the sleep interval in allocation attempts.
 *  - use client ID strings beginning with a byte of 0.
 *  - rename dhcp_sock.
 *  - make allocation work if prev. request is in selecting/requesting
 *    states.
 *  - make allocation work if prev. lease entry is being purged.
 *  - should use new process primitives. also block on crashes.
 *  - add more debug information.
 *
 * Revision 3.3.20.2  1996/06/21  23:48:43  irfan
 * CSCdi60979:  dhcp proxy-client should release leases before reload
 *              release all assigned leases when gracefully reloading.
 * CSCdi60953:  dhcp should be more forgiving when validating packets.
 * CSCdi52819:  dhcp proxy client should support unicast messages.
 *
 * Branch: California_branch
 *
 * Revision 3.3.20.1  1996/03/18  20:15:48  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.2  1996/03/07  09:45:24  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.2.26.1  1996/02/20  00:58:28  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.3  1996/01/29  07:28:28  hampton
 * Remove unnecessary includes of parser definition files.  [CSCdi47717]
 *
 * Revision 3.2  1995/11/17  09:32:33  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:54:26  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/10/25  16:25:25  vandys
 * CSCdi42511:  DHCP fumbles if IPCP does multiple rounds of CONFREQ
 * Debounce free/alloc to avoid spinning the DHCP
 * state machine on quick-turnaround transactions.
 *
 * Revision 2.2  1995/09/18  19:35:28  fox
 * CSCdi40523:  Add IP Address Pooling support for ISDN and other sync
 * interfaces
 * Modify IP address pooling to support all Point-to-Point interfaces
 * Remove Odd/Even Address feature
 * Add 'ppp negotiate' command
 *
 * Revision 2.1  1995/06/07  20:56:20  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define DHCP_LENGTH_FILL            1

#include "master.h"
#include <string.h>
#include <stdarg.h>
#include <ciscolib.h>
#include "subsys.h"
#include "sys_registry.h"
#include "../h/cs_registry.regh"
#include "interface_private.h"
#include "address.h"
#include "config.h"
#include "parser.h"
#include "ttysrv.h"
#include "packet.h"
#include "ieee.h"
#include "logger.h"
#include "../if/network.h"
#include "../ip/ip.h"
#include "../ip/ipaddress.h"
#include "../ip/dhcp.h"
#include "../ip/ip_registry.h"
#include "../parser/parser_defs_tcpip.h"
#include "../ui/debug.h"
#include "../ip/dhcp_debug.h"



/*
 * forward declarations
 */
static paktype *dhcp_setup_xmit_pkt(udptype **, dhcptype **);
uint dhcp_send_discover(iplease_type *);
uint dhcp_send_request(iplease_type *);
uint dhcp_send_decline(dhcp_values *, iplease_type *);
uint dhcp_send_release(iplease_type *);
static int dhcp_send_pack (paktype *, dhcptype *, int,
			   dhcp_server_type *, iplease_type *, uint);


/*
 * dhcp_setup_xmit_pkt
 *
 * some common code for sending a DHCP client packet
 */
static paktype *dhcp_setup_xmit_pkt (udptype **pudp, dhcptype **pdhcp)
{

    uint offset;
    dhcptype *dhcp;
    udptype *udp;
    paktype *pak;

    pak = setup_ipsocket(dhcp_client_soc, sizeof(dhcptype) +
			 DHCP_MAX_MSG_SIZE_OPTION, &offset);
    if (NULL == pak) {
	if (dhcp_debug) {
	    buginf("\nDHCP: Unable to setup output packet");
	}
	return (NULL);
    }

    dhcp = *pdhcp = (dhcptype *)(pak->data_area + offset);
    udp = *pudp = (udptype *) ((char *) dhcp - UDPHEADERBYTES);
 
    /*
     * Set up some of the DHCP fixed fields
     */
    memset(dhcp, 0, sizeof(dhcptype) + DHCP_MIN_OPTION_FIELD);
    dhcp->dhc_op = DHCP_BOOTREQUEST;	/* all client pkts are D_BREQ's */

    /*
     * we don't touch hops, secs, sname, file fields etc.
     * we do use hardware type, mac-addr-len and mac-addr
     *    but these are used as a key by sucking implementations
     *    All this def'ed code is the result of futile attempts
     *    to get to work with those creatures.
     */

    dhcp->dhc_htype = 1;		/* 10MB Ethernet */
    dhcp->dhc_hlen = IEEEBYTES;  	/* 6 bytes in hardware address */
    ieee_copy(default_mac_addr, dhcp->dhc_chaddr);
    PUTLONG(&dhcp->dhc_vend_cookie, h2nl(DHCP_MAGIC_COOKIE));
    return (pak);
}

/*
 * dhcp_send_discover
 *
 * send a DHCP discover out on all interfaces
 */
uint dhcp_send_discover (iplease_type *ipl)
{
    paktype *pak;
    dhcptype *dhcp;
    udptype *udp;
    uchar *cp;
    uint dhcp_len;
    dhcp_server_type *dserv;
    uint count;
    boolean have_unicast_servers = FALSE;

    SAVE_CALLER();

    if (!ipl) {
	if (dhcp_debug) {
	    buginf("\nDHCP: SDiscover for NULL ipl ? from %X", caller());
	}
	return (DHCP_RET_FAIL);
    }
    ipl->retry_count++;

    if (dhcp_debug) {
	buginf("\nDHCP: SDiscover attempt # %d for entry:",
	       ipl->retry_count);
	print_lease_entry(TRUE, ipl);
    }

    pak = dhcp_setup_xmit_pkt(&udp, &dhcp);
    if (pak == NULL) {
	return (DHCP_RET_FAIL);
    }

    /*
     * transaction id
     */
    PUTLONG(&dhcp->dhc_xid, h2nl(ipl->my_xid));

    /*
     * we don't use/implement secs (since booting) field.
     */

    /*
     * we leave the broadcast bit off
     */

    /*
     * zero out ciaddr, yiaddr, siaddr, giaddr. For
     * clarity only, else we don't need to rewrite bzero'd
     * fields
     */
    PUTLONG(&dhcp->dhc_ciaddr, 0);
    PUTLONG(&dhcp->dhc_yiaddr, 0) ;
    PUTLONG(&dhcp->dhc_siaddr, 0);
    PUTLONG(&dhcp->dhc_giaddr, 0);

    /*
     * Now start placing options
     */
    cp = &dhcp->dhc_options[0];

    /* DHCP message type option... A MUST */
    *cp++ = DHCO_MSG_TYPE;
    *cp++ = DHCO_LEN_OCTET;
    *cp++ = DHCPM_DISCOVER;

    /* DHCP max message size option */
    *cp++ = DHCO_MAX_MSG;
    *cp++ = DHCO_LEN_SHORT;
    DHCP_PUTSHORT(h2n(DHCP_MAX_MSG_SIZE_OPTION), cp);

    /*
     * DHCP client identifier....A must for us
     *   this is what allows us to ask for more than one
     *   address per mac-address.
     */
    *cp++ = DHCO_CLIENT_ID;
    dhcp_len = strlen(ipl->client_id) + 1;
    if ((!dhcp_len) || (dhcp_len > DHCP_MAX_STR)) {
	if (dhcp_debug) {
	    buginf("\nDHCP: SDiscover: Bad len %u for client id string: %s",
		   ipl->client_id, dhcp_len);
	}
	/* this is probably fatal for us. Should we bail out ? */
	retbuffer (pak);
	return (DHCP_RET_FAIL);
    }
    sstrncpy(cp+2, ipl->client_id, dhcp_len);
    *cp++ = (uchar) dhcp_len;       /* option length */
    *cp = DHCO_CLIENT_ID_TYPE;      /* We use Cl-ID type 1 */  
    cp += dhcp_len;                 /* skip to next field */

    /* DHCP requested IP address & lease */
    if (ipl->request_addr) {
	*cp++ = DHCO_IPADDR;        /* Requested IP address option */
	*cp++ = DHCO_LEN_LONG;      /* Option length */
	DHCP_PUTLONG(h2nl(ipl->request_addr), cp);  /* place the address */
	if (dhcp_debug) {
	    buginf("\nDHCP: SDiscover placed IP addr option: %i",
		   ipl->request_addr);
	}
	if (ipl->lease_secs) {
	    *cp++ = DHCO_LEASE;           /* Requested Lease len option */
	    *cp++ = DHCO_LEN_LONG;        /* Option length */
	    DHCP_PUTLONG(h2nl(ipl->lease_secs), cp); /* lease request secs */
	    if (dhcp_debug) {
		buginf("\nDHCP: SDiscover placed lease len option: %u",
		       ipl->lease_secs);
	    }
	}
    }

    *cp++ = DHCO_END;

    dhcp_len = ((uint) (cp - &dhcp->dhc_op));

    /*
     * see if we can send unicasts to the server ?
     */
    dserv = &dhcps_table[0];           /* Point at the first server */
    for (count = 0; count < MAX_DHCP_SERVERS; dserv++, count++) {
	if ((dserv->flags & DHCPS_VALID) &&    /* unicast server address ? */
	    (!(dserv->flags & DHCPS_DEFAULT))) {
	    have_unicast_servers = TRUE;
	    break;
	}
    }

    if (dhcp_debug) {
	buginf("\nDHCP: SDiscover: sending %u byte length DHCP packet", 
	       dhcp_len);
    }

    /*
     * If we have unicast servers send them the discover
     * else broadcast out.
     */
    if (have_unicast_servers) {

	dserv = &dhcps_table[0];           /* Point at the first server */
	for (count = 0; count < MAX_DHCP_SERVERS; dserv++, count++) {
	    if ((dserv->flags & DHCPS_VALID) &&    /* unicast srvr addr ? */
		(!(dserv->flags & DHCPS_DEFAULT))) {

		if (dhcp_debug) {
		    buginf("\nDHCP: SDiscover %u bytes ", dhcp_len);
		}
		dhcp_send_pack(pak, dhcp, DHCP_SEND_UNICAST,
			       dserv, ipl, dhcp_len);
	    }                      /* if server entry valid */
	}                          /* for server table */

    } else {
	/*
	 * should be broadcasted
	 */
	if (dhcp_debug) {
	    buginf("\nDHCP: SDiscover %u bytes ", dhcp_len);
	}
	dhcp_send_pack(pak, dhcp, DHCP_SEND_BROADCAST,
		       NULL, NULL, dhcp_len);
    }
    retbuffer(pak);
    return (DHCP_RET_OK);
}

/*
 * dhcp_send_request
 *
 * send a dhcp request packet out
 */
uint dhcp_send_request (iplease_type *ipl)
{
    paktype *pak;
    dhcptype *dhcp;
    udptype *udp;
    uchar *cp;
    uint dhcp_len;
    dhcp_server_type *dserv;
    uint count;
    boolean send_broadcast_request = TRUE;

    SAVE_CALLER();

    if (!ipl) {
	if (dhcp_debug) {
	    buginf("\nDHCP: SRequest for NULL ipl ? from %X", caller());
	}
	return (DHCP_RET_FAIL);
    }
    ipl->retry_count++;

    if (dhcp_debug) {
	buginf("\nDHCP: SRequest attempt # %u for entry:",
	       ipl->retry_count);
	print_lease_entry(TRUE, ipl);
    }

    pak = dhcp_setup_xmit_pkt(&udp, &dhcp);
    if (pak == NULL) {
	return (DHCP_RET_FAIL);
    }

    /*
     * transaction id
     */
    PUTLONG(&dhcp->dhc_xid, h2nl(ipl->my_xid));

    /*
     * we don't use/implement secs (since booting) field.
     */

    /*
     * we leave the broadcast bit off
     */

    /*
     * zero out ciaddr, yiaddr, siaddr, giaddr. For
     * clarity only, else we don't need to rewrite bzero'd
     * fields
     */
    PUTLONG(&dhcp->dhc_ciaddr, 0);
    PUTLONG(&dhcp->dhc_yiaddr, 0) ;
    PUTLONG(&dhcp->dhc_siaddr, 0);
    PUTLONG(&dhcp->dhc_giaddr, 0);

    /*
     * Now start placing options
     */
    cp = &dhcp->dhc_options[0];

    /* DHCP message type option... A MUST */
    *cp++ = DHCO_MSG_TYPE;
    *cp++ = DHCO_LEN_OCTET;
    *cp++ = DHCPM_REQUEST;           /* it's a DHCP request message */

    /* DHCP max message size option */
    *cp++ = DHCO_MAX_MSG;
    *cp++ = DHCO_LEN_SHORT;
    DHCP_PUTSHORT(h2n(DHCP_MAX_MSG_SIZE_OPTION), cp);

    /* DHCP client identifier....A must for us */
    *cp++ = DHCO_CLIENT_ID;
    dhcp_len = strlen(ipl->client_id) + 1;  /* strlen doesn't count 0 */
    if ((!dhcp_len) || (dhcp_len > DHCP_MAX_STR)) {
	if (dhcp_debug) {
	    buginf("\nDHCP: SRequest: Bad len %u for client id string: %s",
		   ipl->client_id, dhcp_len);
	}
	/* this is probably fatal for us. Should we bail out ? */
	retbuffer (pak);
	return (DHCP_RET_FAIL);
    }
    sstrncpy(cp+2, ipl->client_id, dhcp_len);
    *cp++ = (uchar) dhcp_len;        /* option length */
    *cp = DHCO_CLIENT_ID_TYPE;       /* We use CL-ID type 1 */
    cp += dhcp_len;                  /* skip to next field */

    /* DHCP selected server identifier */
    *cp++ = DHCO_SRVR_ID;            /* Server ID option */
    *cp++ = DHCO_LEN_LONG;           /* Option length */
    DHCP_PUTLONG(h2nl(ipl->lease_server_addr), cp);  /* place the address */
    if (dhcp_debug) {
	buginf("\nDHCP: SRequest- Server ID option: %i",
	       ipl->lease_server_addr);
    }

    if (ipl->client_addr) {
	/* DHCP requested IP address & lease */
	*cp++ = DHCO_IPADDR;        /* Requested IP address option */
	*cp++ = DHCO_LEN_LONG;      /* Option length */
	DHCP_PUTLONG(h2nl(ipl->client_addr), cp);  /* place the address */
	if (dhcp_debug) {
	    buginf("\nDHCP: SRequest- Requested IP addr option: %i",
		   ipl->client_addr);
	}
    } else {
	if (dhcp_debug) {
	    buginf("\nDHCP: SRequest No IP addr to place");
	}
    }

    *cp++ = DHCO_LEASE;           /* Requested Lease len option */
    *cp++ = DHCO_LEN_LONG;        /* Option length */
    DHCP_PUTLONG(h2nl(ipl->lease_secs), cp); /* lease request secs */
    if (dhcp_debug) {
	buginf("\nDHCP: SRequest placed lease len option: %u",
	       ipl->lease_secs);
    }  /* if dhcp_debug */

    *cp++ = DHCO_END;

    dhcp_len = ((uint) (cp - &dhcp->dhc_op));


    if (dhcp_debug) {
	buginf("\nDHCP: SRequest: %u bytes", dhcp_len);
    }

    /*
     * See if we have the server in the configured server list.
     * Else probably we are requesting after selecting from
     * a random offering, and this type should be requested
     * as broadcast packet. Per the spec.
     */
     
    dserv = &dhcps_table[0];           /* Point at the first server */
    for (count = 0; count < MAX_DHCP_SERVERS; dserv++, count++) {
	if ((dserv->flags & DHCPS_VALID) &&  /* valid server entry ? */
	    (dserv->addr == ipl->lease_server_addr)) {  /* this server ? */

	    send_broadcast_request = FALSE;
	    break;
	}                      /* if server entry valid */
    }                          /* for server table */

    if (send_broadcast_request) {
	dhcp_send_pack(pak, dhcp, DHCP_SEND_BROADCAST,
		       NULL, NULL, dhcp_len);
    } else {
	dhcp_send_pack(pak, dhcp, DHCP_SEND_UNICAST,
		       NULL, ipl, dhcp_len);
    }
    retbuffer(pak);
    return (DHCP_RET_OK);
}

/*
 * dhcp_send_release
 *
 * send a dhcp release packet to the lease server
 */
uint dhcp_send_release (iplease_type *ipl)
{
    paktype *pak;
    dhcptype *dhcp;
    udptype *udp;
    uchar *cp;
    uint dhcp_len;

    SAVE_CALLER();

    if (!ipl) {
	if (dhcp_debug) {
	    buginf("\nDHCP: SRelease for NULL ipl ? from %X", caller());
	}
	return (DHCP_RET_FAIL);
    }
    ipl->retry_count++;

    if (dhcp_debug) {
	buginf("\nDHCP: SRelease attempt # %u for entry:",
	       ipl->retry_count);
	print_lease_entry(TRUE, ipl);
    }

    pak = dhcp_setup_xmit_pkt(&udp, &dhcp);
    if (NULL == pak) {
	return (DHCP_RET_FAIL);
    }

    PUTLONG(&dhcp->dhc_xid, h2nl(ipl->my_xid));  /* transaction id */
    PUTLONG(&dhcp->dhc_ciaddr, h2nl(ipl->client_addr));

    /*
     * Now start placing options
     */
    cp = &dhcp->dhc_options[0];

    /* DHCP message type option... A MUST */
    *cp++ = DHCO_MSG_TYPE;
    *cp++ = DHCO_LEN_OCTET;
    *cp++ = DHCPM_RELEASE;           /* it's a DHCP release message */
    
    /* DHCP selected server identifier */
    *cp++ = DHCO_SRVR_ID;            /* Server ID option */
    *cp++ = DHCO_LEN_LONG;           /* Option length */
    DHCP_PUTLONG(h2nl(ipl->lease_server_addr), cp);  /* place the address */
    if (dhcp_debug) {
	buginf("\nDHCP: SRelease placed Server ID option: %i",
	       ipl->lease_server_addr);
    }

    /* DHCP client identifier....A must for us */
    *cp++ = DHCO_CLIENT_ID;
    dhcp_len = strlen(ipl->client_id) + 1;  /* strlen doesn't count 0 */
    if ((!dhcp_len) || (dhcp_len > DHCP_MAX_STR)) {
	if (dhcp_debug) {
	    buginf("\nDHCP: SRelease: Bad len %u for client id string: %s",
		   ipl->client_id, dhcp_len);
	}
	/* this is probably fatal for us. Should we bail out ? */
	retbuffer (pak);
	return (DHCP_RET_FAIL);
    }
    sstrncpy(cp+2, ipl->client_id, dhcp_len);
    *cp++ = (uchar) dhcp_len;        /* option length */
    *cp = DHCO_CLIENT_ID_TYPE;       /* We use CL-ID type 1 */
    cp += dhcp_len;                  /* skip to next field */

    *cp++ = DHCO_END;

    dhcp_len = ((uint) (cp - &dhcp->dhc_op));

    if (dhcp_debug) {
	buginf("\nDHCP: SRelease: %u bytes", dhcp_len);
    }

    dhcp_send_pack(pak, dhcp, DHCP_SEND_UNICAST,
		   NULL, ipl, dhcp_len);
    retbuffer(pak);
    return (DHCP_RET_OK);
}

/*
 * dhcp_send_decline
 *
 *   client's only check the address after they have
 *   received an Ack. Once their, they may send back
 *   a decline. Both servers I am testing against, don't
 *   look at declines. They commit their bindings right
 *   after sending Acks.
 */
uint dhcp_send_decline (dhcp_values *dhc, iplease_type *ipl)
{
    if (dhcp_debug) {
	buginf("\nDHCP: At the moment we don't send DHCP declines");
    }
    return (DHCP_RET_FAIL);
}


static int dhcp_send_pack (paktype *pak, dhcptype *dhcp, int opcode,
			   dhcp_server_type *dserv,
			   iplease_type *ipl,
			   uint dhcp_len)
{

    paktype *copy;
    idbtype *odb;
    udptype *udp;
    uchar *cp;
    ipaddrtype local_ip_addr;

    cp = dhcp_len + &dhcp->dhc_op;

#ifdef DHCP_LENGTH_FILL
    if (dhcp_len < 576) {
	memset(cp, 0, 576 - dhcp_len);
    }
    dhcp_len = 576;
#endif

    /*
     * Set up the UDP fields.
     */
    udp = (udptype *) ((char *) dhcp - UDPHEADERBYTES);
    PUTSHORT(&udp->sport, h2n(DHCPC_PORT));  /* this could be any port */
    PUTSHORT(&udp->dport, h2n(DHCPS_PORT));  /* this has to be server port */
    PUTSHORT(&udp->length, h2n(UDPHEADERBYTES + dhcp_len));
    PUTSHORT(&udp->checksum,0);

    /*
     * Use the ipaddress of the requesting idb or
     * that of its unnumbered leader, as the source
     * address for our dhcp packets. Allows a DHCP
     * server to determine, which subnet/pool to use
     * for address allocation.
     */
    local_ip_addr = (ipaddrtype) 0;
    if (ipl && ipl->idb && ipl->idb->ip_enabled) {
	local_ip_addr = (ipl->idb->ip_address) ?
	    (ipl->idb->ip_address) :
		((ipl->idb->ip_unnumbered) ?
		 (ipl->idb->ip_unnumbered->ip_address) : (ipaddrtype) 0);
    }

    if ((DHCP_SEND_UNICAST == opcode) &&
	((dserv && (!(dserv->flags & DHCPS_DEFAULT))) ||
	 (ipl && ipl->lease_server_addr))) {

	ipaddrtype server;

	server = (dserv) ? dserv->addr : ipl->lease_server_addr;

	if (!local_ip_addr) {
	    local_ip_addr = ip_best_local_address(server, TRUE);
	}

	PUTSHORT(&dhcp->dhc_flags, 0); /* reset b'cast bit */
	/*
	 * giaddr should have our address, if we are hoping to
	 * get a unicast back.
	 */

	PUTLONG(&dhcp->dhc_giaddr, (long) local_ip_addr);
	copy = pak_duplicate(pak);
	if (copy) {

	    ipwrite(copy,
		    IPHEADERBYTES(NOPT) + UDPHEADERBYTES + dhcp_len,
		    UDP_PROT, local_ip_addr, server, NULL,
		    NULL, &ipdefaultparams);

	    if (dhcp_debug) {
		buginf(" Unicast to %i from %i", server, local_ip_addr);
	    }              /* if dhcp_debug */
	    return (DHCP_RET_OK);
	}
	return (DHCP_RET_FAIL);
    }

    /* if broadcast */

    /*
     * Broadcast on all interfaces a DHCP discover request for
     * this lease request except the interface which we are
     * servicing
     */
    FOR_ALL_SWIDBS(odb) {
	if ((!interface_up(odb)) || is_ddr(odb->hwptr) ||
	    ((ipl) && (odb == ipl->idb)) ||
	    !reg_invoke_proto_on_swidb(LINK_IP, odb)) {
	    continue;
	}

	PUTLONG(&dhcp->dhc_giaddr, (long) ip_ouraddr(odb->hwptr));
	copy = pak_duplicate(pak);
	if (copy) {
	    ipwrite(copy,
		    IPHEADERBYTES(NOPT) + UDPHEADERBYTES + dhcp_len,
		    UDP_PROT,
		    local_ip_addr,
		    DHCP_default_server , odb,
		    NULL, &ipdefaultparams);
	    if (dhcp_debug) {
		buginf("\n            B'cast on %s interface from %i",
		       odb->namestring, local_ip_addr);
	    }   /* if dhcp_debug */
	}       /* if copy */
    }           /* for all swidbs */
    return (DHCP_RET_OK);
}
