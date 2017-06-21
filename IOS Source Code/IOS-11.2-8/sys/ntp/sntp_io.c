/* $Id: sntp_io.c,v 3.1.2.2 1996/06/25 22:55:32 dkatz Exp $
 * $Source: /release/112/cvs/Xsys/ntp/sntp_io.c,v $
 *------------------------------------------------------------------
 * I/O Interface for SNTP
 *
 * June 1996, Dave Katz
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: sntp_io.c,v $
 * Revision 3.1.2.2  1996/06/25  22:55:32  dkatz
 * CSCdi61119:  Commit SNTP Code
 * Branch: California_branch
 * Add rudimentary clock selection logic.  Get rid of most static storage.
 * Keep track of all broadcast servers.
 *
 * Revision 3.1.2.1  1996/06/23  04:40:26  dkatz
 * CSCdi61119:  Commit SNTP Code
 * Branch: California_branch
 * Add an SNTP client.  This is not included in shipping IOS images.
 *
 * Revision 3.1  1996/06/22  21:14:57  dkatz
 * *** empty log message ***
 *
 *------------------------------------------------------------------
 * $Endlog$
 *
 * This file contains the I/O-specfic routines for SNTP.  IP packets are
 * built and parsed here, and the grubby details of the SNTP socket are
 * handled here.
 */

#define CISCO

#include "master.h"
#include "packet.h"
#include "interface_private.h"
#include "../ip/ip.h"
#include "../ip/ip_registry.h"
#include "../os/clock.h"
#include "ntp_fp.h"
#include "ntp.h"
#include "sntp.h"
#include "sntp_debug.h"


/*
 * sntp_init_io
 *
 * Initialize I/O for SNTP.
 *
 * Returns TRUE if everything went OK.
 */
boolean sntp_init_io (sntp_proc *proc)
{
    proc->sntp_sock = open_ipsocket(UDP_PROT, NULL, NULL, NTP_PORT,
				    NTP_PORT, SOCK_ANYSRC);
    if (proc->sntp_sock) {
	process_watch_queue(proc->sntp_sock->inq, ENABLE, RECURRING);
	return(TRUE);
    } else {
	return(FALSE);
    }
}


/*
 * sntp_shutdown_io
 *
 * Clean up before deleting the process.
 */
void sntp_shutdown_io (sntp_proc *proc)
{
    close_ipsocket(proc->sntp_sock);
}


/*
 * sntp_send
 *
 * Send a packet to a server.
 */
void sntp_send (sntp_proc *proc, sntp_server *server)
{
    paktype *pak;
    uint offset;
    idbtype *if_out;
    ipaddrtype srcaddr, destaddr;
    struct pkt *pktptr;
    udptype *udp;

    /* Get a buffer. */

    pak = setup_ipsocket(proc->sntp_sock, LEN_PKT_NOMAC, &offset);
    if (pak == NULL)
	return;

    /* Determine the outgoing interface and source address. */

    destaddr = server->server_address;
    if_out = reg_invoke_ip_first_hop(&destaddr, destaddr, IP_PATH_DEFFLAGS);
    if (if_out) {
	if (if_out->ip_unnumbered) {
	    srcaddr = if_out->ip_unnumbered->ip_address;
	} else {
	    srcaddr = if_out->ip_address;
	}
    } else {
	srcaddr = IPADDR_ZERO;	/* We'll figure it out */
    }

    /* Build the UDP header. */

    udp = (udptype *) (ipheadstart(pak) + IPHEADERBYTES(NOPT));
    udp->sport = NTP_PORT;
    udp->dport = NTP_PORT;
    udp->length = htons(UDPHEADERBYTES + LEN_PKT_NOMAC);

    /* Build the packet payload. */

    pktptr = (struct pkt *) (pak->data_area + offset);
    sntp_poll(pktptr, server->server_version);
    if (sntp_packets_debug) {
	buginf("\nSending SNTP packet to %i", destaddr);
	buginf("\n xmt %08x.%08x (%CC)", pktptr->xmt.l_ui, pktptr->xmt.l_uf,
	       "%B", &pktptr->xmt);
    }

    /* Checksum the packet. */

    udp->checksum = 0;
    if (srcaddr != IPADDR_ZERO) {	/* If no address, punt the checksum */
	udp->checksum = htons(CheckSum((uchar *)udp, udp->length, srcaddr, 
				       destaddr, UDP_PROT));
    }

    ipwrite(pak, LEN_PKT_NOMAC + UDPHEADERBYTES + IPHEADERBYTES(NOPT),
	    UDP_PROT, srcaddr, destaddr, if_out, NULL, NULL);
}


/*
 * sntp_readpkt
 *
 * Receive an incoming SNTP packet and process it.
 */
void sntp_readpkt (sntp_proc *proc)
{
    paktype *pak;
    iphdrtype *ip;
    udptype *udp;
    struct pkt *pktptr;
    ulong data_length;
    sntp_server *server;

    /* Fetch the packets. */

    while ((pak = read_ipsocket(proc->sntp_sock, FALSE, 0, &udp, &pktptr))) {
	ip = iphdrtype_start(pak);
	data_length = ip->tl - ipheadsize(ip) - UDPHEADERBYTES;

	/* Find the server. */

	server = sntp_find_server(proc, ip->srcadr);

	/* Broadcast? */

	if (pak->flags & (PAK_BROADCAST | PAK_MULTICAST)) {

	    /*
	     * If broadcast mode is enabled and there is no server,
	     * create one.
	     */
	    if (proc->sntp_broadcast_client && !server) {
		server = sntp_create_server(proc, ip->srcadr);
	    }
	}

	/* Process the packet. */
	
	if (server)
	    sntp_receive(proc, pktptr, server, data_length);

	datagram_done(pak);
    }
}
