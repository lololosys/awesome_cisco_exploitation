/* $Id: ntp_io.c,v 3.3.10.3 1996/08/19 18:54:54 dkatz Exp $
 * $Source: /release/112/cvs/Xsys/ntp/ntp_io.c,v $
 *------------------------------------------------------------------
 * NTP I/O routines
 *
 * Dave Katz, November 1992
 *
 * Copyright (c) 1992-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: ntp_io.c,v $
 * Revision 3.3.10.3  1996/08/19  18:54:54  dkatz
 * CSCdi65483:  NTP needs modularity cleanup
 * Branch: California_branch
 * Break up NTP into separate subsystems.  Significantly reduce image size
 * and stack usage.  Clean up refclock interface.  Make all modules
 * compile cleanly with all error checking enabled (DUSTY is clean!)
 *
 * Revision 3.3.10.2  1996/04/16  19:03:14  rchandra
 * CSCdi54830:  IP features commit
 * Branch: California_branch
 *
 * Revision 3.3.10.1  1996/03/18  21:12:14  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.3  1996/03/16  07:09:37  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.2.26.2  1996/03/07  10:06:49  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.2.26.1  1996/02/20  14:49:14  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.3  1996/02/21  10:24:45  dkatz
 * CSCdi47415:  NTP doesnt respond with right address
 * Respond with the address to which our received packets were directed.
 *
 * Revision 3.2  1995/11/17  17:51:06  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:41:08  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:45:19  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Copyright 1992 David L. Mills
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appears in all copies and
 * that both the copyright notice and this permission notice appear in
 * supporting documentation, and that the name University of Delaware
 * not be used in advertising or publicity pertaining to distribution of
 * the software without specific, written prior permission.  The
 * University of Delaware makes no representations about the suitability
 * of this software for any purpose.  It is provided "as is" without
 * express or implied warranty.
 *
 * Authors
 *
 * Dennis Ferguson <dennis@mrbill.canet.ca> (foundation code for NTP
 *      Version 2 as specified in RFC-1119)
 * Lars H. Mathiesen <thorinn@diku.dk> (adaptation of foundation code for
 *      Version 3 as specified in RFC01305)
 * Louis A. Mamakos <louie@ni.umd.edu> (support for md5-based 
 *      authentication)
 * Craig Leres <leres@ee.lbl.gov> (port to 4.4BSD operating system,
 *	ppsclock, Maganavox GPS clock driver)
 * Nick Sayer <mrapple@quack.sac.ca.us> (SunOS streams modules)
 * Frank Kardel <Frank.Kardel@informatik.uni-erlangen.de> (DCF77 clock
 *      driver, STREAMS module for DCF77, support scripts)
 * Rainer Pruy <Rainer.Pruy@informatik.uni-erlangen.de> (monitoring/trap
 *      scripts, statistics file handling)
 * Glenn Hollinger <glenn@herald.usask.ca> (GOES clock driver)
 * Kenneth Stone <ken@sdd.hp.com> (port to HPUX operating system)
 * Dave Katz <dkatz@cisco.com> (port to RS/6000 AIX operating system)
 * William L. Jones <jones@hermes.chpc.utexas.edu> (RS/6000 AIX
 *	modifications, HPUX modifications)
 * John A. Dundas III <dundas@salt.jpl.nasa.gov> (Apple A/UX port)
 * David L. Mills <mills@udel.edu> (WWVB and Austron GPS clock drivers,
 *	pps support)
 * Jeffrey Mogul <mogul@pa.dec.com> (ntptrace utility)
 * Steve Clift (clift@ml.csiro.au) OMEGA clock driver)
 */

/*
 * ntp_io.c - input/output routines for xntpd.  The socket-opening code
 *	       was shamelessly stolen from ntpd.
 */

#define CISCO

#include "master.h"
#include <ciscolib.h>
#include "packet.h"
#include "ttysrv.h"
#include "interface_private.h"
#include "../ip/ip.h"
#include "../ip/ip_registry.h"
#include "ntpd.h"
#include "ntp_refclock.h"
#include <logger.h>

/*
 * Memory allocation
 */
static struct recvbuf *local_buf;

/*
 * Other statistics of possible interest
 */
u_long packets_dropped;	/* total number of packets dropped on reception */
u_long packets_ignored;	/* packets received on wild card interface */
u_long packets_received;	/* total number of packets received */
u_long packets_sent;	/* total number of packets sent */
u_long packets_notsent;	/* total number of packets which couldn't be sent */

u_long handler_calls;	/* number of calls to interrupt handler */
u_long handler_pkts;	/* number of pkts received by handler */
u_long io_timereset;	/* time counters were reset */

/*
 * Interface stuff
 */
struct interface *any_interface;	/* pointer to default interface */
struct interface *loopback_interface;	/* point to loopback interface */
struct interface inter_list[MAX_INTERFACES];
static int ninterfaces;


/*
 * ntp_init_idb - initialize an IDB for NTP use
 */

void ntp_init_idb(idbtype *idb, boolean setup_hwidb)
{
    struct ntp_idb_info *infoptr;

    /* Skip it if NTP isn't running. */

    if (!ntp_task_initialized)
	return;

    infoptr = idb->ntp_info;
    if (infoptr)		/* Already initialized? */
	return;
    infoptr = (struct ntp_idb_info *) 
	malloc(sizeof(struct ntp_idb_info));
    if (!infoptr)
	return;

    idb->ntp_info = infoptr;
    infoptr->ntp_enabled = NTP_ENAB_RCV;
    
    inter_list[idb->if_number].port = htons(NTP_PORT);
    inter_list[idb->if_number].idb = idb;
    inter_list[idb->if_number].flags = 0;
    if (idb->hwptr->status & IDB_BCAST)
	inter_list[idb->if_number].flags |= INT_BROADCAST;
    if (idb->if_number >= ninterfaces)
	ninterfaces = idb->if_number + 1;
}

/*
 * create_sockets - create a socket for each interface plus a default
 *		    socket for when we don't know where to send
 */
static void create_sockets(void)
{
    idbtype *idb;
    
    /*
     * create pseudo-interface with wildcard address
     */
    inter_list[0].received = 0;
    inter_list[0].sent = 0;
    inter_list[0].notsent = 0;
    inter_list[0].flags = INT_BROADCAST;
    inter_list[0].idb = NULL;
    inter_list[0].port = htons(NTP_PORT);
    
    FOR_ALL_SWIDBS(idb) {
	ntp_init_idb(idb, FALSE);
    }
    any_interface = &inter_list[0];
}



/*
 * findbcastinter - find broadcast interface corresponding to address
 *
 * This routine should never need to be called; always return any_interface.
 * (Broadcast interfaces are explicitly configured.)
 */
struct interface *
    findbcastinter(addr)
struct sockaddr_in *addr;
{
    return any_interface;
}


/*
 * sendpkt - send a packet to the specified destination.
 *
 * Broadcasts are flagged with the "bcast" flag true in the sockaddr_in.
 * If the "if_bcast" flag is set in the sockaddr_in, use the interface
 * broadcast address;  otherwise, use the provided address.
 *
 * If an explicit source address is provided, use that;  otherwise, use
 * the interface address.
 */
void
    sendpkt(dest, inter, srcaddr, ttl, pkt, len)
struct sockaddr_in *dest;
struct interface *inter;
ipaddrtype srcaddr;
int ttl;
struct pkt *pkt;
int len;
{
    paktype *pak;
    uint offset;
    u_char *ptr;
    ipaddrtype dummy, destaddr, dest_or_all_ones;
    udptype *udp;
    idbtype *if_out;

    if (!ntp_running)
	return;

    if (inter == NULL)
	return;

    if_out = inter->idb;		/* A reasonable default */

    if (!ntp_sock)		/* Can't happen, but we're paranoid. */
	return;
    pak = setup_ipsocket(ntp_sock, (uint) len, &offset);
    if (pak == NULL)
	return;

    /*
     * If broadcast, pick up the interface broadcast address if we're
     * supposed to;  otherwise, use the provided address.
     */
    destaddr = dest_or_all_ones = ntohl(dest->sin_addr.s_addr);
    if (dest->bcast) {
	pak->flags |= PAK_XMITBCAST;	/* Flag it */
	if (dest->if_bcast) {
	    if (if_out == NULL) {
		if (ntp_packets_debug) {
		    buginf("\nNTP: sendpkt called with NULL interface");
		    buginf("\n     interface %x, flags %x, last_ttl %d, "
			   "received %d, sent %d, notsent %d",
			   inter, inter->flags, inter->last_ttl,
			   inter->received, inter->sent, inter->notsent);
		    bugtrace();
		}
		retbuffer(pak);
		return;
	    }
	    dest_or_all_ones = -1L;
	    destaddr = if_out->ip_broadcast;
	}
    }

    /*
     * Determine the source address for this packet.  If we're passed
     * one, use it.  If we dont have one, figure it out based on the outgoing
     * interface.  If we don't have one written down in the peer block,
     * do a first-hop lookup to determine the outgoing interface.
     */
    if (inter == any_interface && if_out == NULL) {
	dummy = destaddr;
	if_out = reg_invoke_ip_first_hop(&dummy, dummy, 
					 IP_PATH_DEFFLAGS);
    }
    if (srcaddr == IPADDR_ZERO) {	/* Nothing specified */
	if (if_out != NULL) {
	    if (if_out->ip_unnumbered) {
		srcaddr = if_out->ip_unnumbered->ip_address;
	    } else {
		srcaddr = if_out->ip_address;
	    }
	} else {
	    srcaddr = IPADDR_ZERO;	/* We'll figure it out */
	}
    }

    udp = (udptype *) (ipheadstart(pak) + IPHEADERBYTES(NOPT));
    udp->sport = inter->port;
    udp->dport = dest->sin_port;
    udp->length = htons(UDPHEADERBYTES + len);
    ptr = pak->data_area + offset;
    bcopy((u_char *) pkt, ptr, len);
    udp->checksum = 0;
    if (srcaddr != IPADDR_ZERO) {	/* If no address, punt the checksum */
	udp->checksum = htons(CheckSum((uchar *)udp, udp->length, srcaddr, 
				       destaddr, UDP_PROT));
    }

    ipwrite(pak, len + UDPHEADERBYTES + IPHEADERBYTES(NOPT), UDP_PROT, 
	    srcaddr, dest_or_all_ones, if_out, NULL, NULL);
    inter->sent++;
    packets_sent++;
}


/*
 * ntp_input_handler - process received packets
 */
void ntp_input_handler(pak, udp, ntp_pkt)
    paktype *pak;
    udptype *udp;
    struct pkt *ntp_pkt;
{
    iphdrtype *ip;
    register int if_num;
    struct ntp_idb_info *if_info;
    idbtype *idb;

    ip = (iphdrtype *)ipheadstart(pak);

    if (!ntp_running)
	return;

    if_info = pak->if_input->ntp_info;
    
    /* Skip it if the interface doesn't do NTP. */
    
    if (!if_info)
	return;
    if (!if_info->ntp_enabled)
	return;

    /* Skip it if it's a broadcast and we're not a broadcast client. */

    if (!(if_info->ntp_enabled & NTP_ENAB_BCASTCLIENT) && 
	(pak->flags & PAK_BROADCAST))
	return;

    /*
     * If not a broadcast, find the interface matching the destination address.
     * The problem is that pak->if_input points to the receiving interface,
     * not the addressed interface.  For efficiency, first check the input
     * interface.
     */
    if_num = pak->if_input->if_number;
    if (!(pak->flags & PAK_BROADCAST)) {
	if (pak->if_input->ip_address != ip->dstadr) {
	    idb = ip_ouraddress_interface(ip->dstadr);
	    if (idb) {
		if_num = idb->if_number;
	    } else {
		if_num = 0;
	    }
	}
    }
    if (!if_num)
	return;
    
    get_systime(&local_buf->recv_time);
    handler_pkts++;
    
    local_buf->recv_length = ip->tl - (ip->ihl << 2) - UDPHEADERBYTES;
    if (local_buf->recv_length > RX_BUFF_SIZE) /* Watch out for jumbograms */
	return;
    local_buf->recv_srcadr.sin_addr.s_addr = ip->srcadr;
    local_buf->recv_srcadr.sin_port = udp->sport;
    local_buf->ip_destaddr = ip->dstadr;
    bcopy(ntp_pkt, &local_buf->recv_space, local_buf->recv_length);
    
    /*
     * Got one.  Mark how and when it got here,
     * put it on the full list and do bookkeeping.
     */
    local_buf->dstadr = &inter_list[if_num];
    inter_list[if_num].received++;
    
    receive(local_buf);
}

/*
 * ntp_refclock_input_handler
 *
 * Handle the reception of a packet from a reference clock.
 */
void ntp_refclock_input_handler (struct recvbuf *rbuf)
{
    tt_soc *tty;
    ntp_refclock *refclock;

    /* Pick up all the linkages. */

    tty = MODEMS[rbuf->fd];
    refclock = tty->tty_refclock;
    if (!refclock)
	return;
    rbuf->recv_srcclock = refclock->refclock_peer;

    /* Go handle the received packet. */

    rbuf = (*refclock->io.clock_recv)(rbuf);

    /* Free the buffer if the handler hasn't stolen it. */

    if (rbuf)
	free(rbuf);
}

/*
 * ntp_refclock_input
 *
 * Receive a packet from the async process
 *
 * Called on the async process thread!
 *
 * Simply enqueue the packet for the NTP main process to see.
 */
void ntp_refclock_input (struct recvbuf *rbuf)
{
    process_enqueue(ntp_refclock_queue, rbuf);
}

/*
 * findinterface - utility used by other modules to find an interface
 *		   given an address.
 */
struct interface *
    findinterface(addr)
struct sockaddr_in *addr;
{
    register int i;
    register u_long saddr;
    
    /*
     * Just match the address portion.
     */
    saddr = addr->sin_addr.s_addr;
    for (i = 0; i < ninterfaces; i++) {
	if (inter_list[i].idb &&
	    inter_list[i].idb->ip_address == saddr)
	    return &inter_list[i];
    }
    return (struct interface *)0;
}


/*
 * io_clr_stats - clear I/O module statistics
 */
void
    io_clr_stats()
{
    packets_dropped = 0;
    packets_ignored = 0;
    packets_received = 0;
    packets_sent = 0;
    packets_notsent = 0;
    
    handler_calls = 0;
    handler_pkts = 0;
    io_timereset = current_time;
}


/*
 * ntp_init_io - initialize I/O data structures and call socket creation routine
 */
boolean ntp_init_io()
{
    local_buf = malloc(sizeof(struct recvbuf));
    if (!local_buf)
	return(FALSE);
    packets_dropped = packets_received = 0;
    packets_ignored = 0;
    packets_sent = packets_notsent = 0;
    handler_calls = handler_pkts = 0;
    io_timereset = 0;
    loopback_interface = 0;
    create_sockets();
    return(TRUE);
}
