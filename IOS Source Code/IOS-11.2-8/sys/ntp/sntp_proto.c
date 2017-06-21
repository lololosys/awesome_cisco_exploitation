/* $Id: sntp_proto.c,v 3.1.2.3 1996/06/25 22:55:33 dkatz Exp $
 * $Source: /release/112/cvs/Xsys/ntp/sntp_proto.c,v $
 *------------------------------------------------------------------
 * Simple NTP (SNTP) client
 *
 * June 1996, Dave Katz
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: sntp_proto.c,v $
 * Revision 3.1.2.3  1996/06/25  22:55:33  dkatz
 * CSCdi61119:  Commit SNTP Code
 * Branch: California_branch
 * Add rudimentary clock selection logic.  Get rid of most static storage.
 * Keep track of all broadcast servers.
 *
 * Revision 3.1.2.2  1996/06/23  05:46:27  dkatz
 * CSCdi61119:  Commit SNTP Code
 * Branch: California_branch
 * Validate the version number, just in case there's ever a version 4.
 *
 * Revision 3.1.2.1  1996/06/23  04:40:28  dkatz
 * CSCdi61119:  Commit SNTP Code
 * Branch: California_branch
 * Add an SNTP client.  This is not included in shipping IOS images.
 *
 * Revision 3.1  1996/06/22  21:14:59  dkatz
 * *** empty log message ***
 *
 *------------------------------------------------------------------
 * $Endlog$
 *
 * This file contains the SNTP protocol itself.  It provides a client only.
 * Server capabilities are provided by standard NTP squawkers.
 *
 * The SNTP protocol is really just plain old client-mode NTP.  The
 * difference between this and Real NTP lies in what happens when a packet
 * is received.  Basically, the time received from the server is forced
 * as the current time (slewing if possible).  Only minimal validity
 * checks are performed, and no attempt is made to choose one server over
 * another, to statistically filter the received data, or any of the other
 * fancy things that give NTP its trendy cachet.  No attempt is made to
 * adjust the frequency of the system clock;  rather, corrections are made
 * periodically and the inherent frequency stays the same.  (This means
 * that the clock will run off on its merry way if contact is lost with
 * the servers;  real NTP adjusts the frequency of the clock so that
 * reasonably accurate time is kept even when free-running.
 *
 * The client can operate in broadcast mode (listening passively for NTP
 * broadcasts on all interfaces) or in unicast mode (periodically sending
 * requests to servers).  The unicast mode provides somewhat more accurate
 * time (since it compensates for the round-trip delay to the server) and
 * is necessary if the server is not on a common subnet with the client.
 */

#define CISCO

#include "master.h"
#include <string.h>
#include <ciscolib.h>
#include "mgd_timers.h"
#include "../os/clock.h"
#include "../os/clock_private.h"
#include "ntp_fp.h"
#include "ntp.h"
#include "sntp.h"
#include "sntp_debug.h"


/*
 * sntp_server_moldy
 *
 * Returns TRUE if we haven't heard from this server in awhile.
 */
static boolean sntp_server_moldy (sntp_server *server)
{
    if (!server)
	return(FALSE);

    return(elapsed_time(&server->server_reply_time) >
	   SNTP_SERVER_DEAD_TIMEOUT);
}


/*
 * sntp_better_server
 *
 * Returns TRUE if the first server is better than the second.
 *
 * A server is better if it is at a lower stratum, or if it is at the
 * same stratum but it is configured and the second is not.
 */
static boolean sntp_better_server (sntp_server *first, sntp_server *second)
{
    if (!first)				/* Not there, it's worse. */
	return(FALSE);

    if (!second)			/* No competition, it's better. */
	return(TRUE);

    if (first->server_stratum > second->server_stratum)	/* Worse stratum. */
	return(FALSE);

    if (first->server_stratum < second->server_stratum)	/* Better stratum. */
	return(TRUE);

    if (first->server_configured && !second->server_configured)	/* Unicast. */
	return(TRUE);
    return(FALSE);
}


/*
 * sntp_select_server
 *
 * Select the time server based on some criteria.  Returns a pointer to
 * the server selected.
 */
static sntp_server *sntp_select_server (sntp_proc *proc)
{
    sntp_server *cur_server, *best_server;

    /* If our current server has been quiet for too long, drop it. */

    if (sntp_server_moldy(proc->sntp_current_server)) {
	proc->sntp_current_server = NULL;
    }

    /* Find the active server that has the lowest stratum. */

    best_server = NULL;
    for (cur_server = proc->sntp_server_queue.qhead; cur_server;
	 cur_server = cur_server->next) {

	/* Consider a server that's talked to us recently. */

	if (!sntp_server_moldy(cur_server)) {

	    /* Tentatively pick this one if it is the best so far. */

	    if (sntp_better_server(cur_server, best_server))
		best_server = cur_server;
	}
    }

    /* Found a good one.  Only replace the old one if the new one is better. */

    if (!proc->sntp_current_server ||
	sntp_better_server(best_server, proc->sntp_current_server)) {
	proc->sntp_current_server = best_server;
    }
    if (sntp_select_debug) {
	buginf("\nSNTP: Selected %i",
	       proc->sntp_current_server->server_address);
    }
    return (proc->sntp_current_server);
}


/*
 * sntp_dump_packet - dump a packet for debugging purposes
 */
static void sntp_dump_packet (struct pkt *pkt)
{
    s_fp sfp;
    u_fp ufp;
    l_fp lfp;

    buginf("\n leap %d, mode %d, version %d, stratum %d, ppoll %d",
	   PKT_LEAP(pkt->li_vn_mode), PKT_MODE(pkt->li_vn_mode),
	   PKT_VERSION(pkt->li_vn_mode), pkt->stratum, 1<<pkt->ppoll);
    sfp = NTOHS_FP(pkt->rootdelay);
    ufp = NTOHS_FP(pkt->rootdispersion);
    buginf("\n rtdel %08x, rtdsp %08x, refid %08x (%i)",
	   sfp, ufp, ntohl(pkt->refid), ntohl(pkt->refid));
    NTOHL_FP(&pkt->reftime, &lfp);
    buginf("\n ref %08x.%08x (%CC)", lfp.l_ui, lfp.l_uf, "%B", &lfp);
    NTOHL_FP(&pkt->org, &lfp);
    buginf("\n org %08x.%08x (%CC)", lfp.l_ui, lfp.l_uf, "%B", &lfp);
    NTOHL_FP(&pkt->rec, &lfp);
    buginf("\n rec %08x.%08x (%CC)", lfp.l_ui, lfp.l_uf, "%B", &lfp);
    NTOHL_FP(&pkt->xmt, &lfp);
    buginf("\n xmt %08x.%08x (%CC)", lfp.l_ui, lfp.l_uf, "%B", &lfp);
}


/*
 * sntp_poll
 *
 * Build an SNTP packet in the provided buffer for transmission
 */
void sntp_poll (struct pkt *sntp_packet, int version)
{
    memset(sntp_packet, 0, LEN_PKT_NOMAC);
    sntp_packet->li_vn_mode = PKT_LI_VN_MODE(LEAP_NOWARNING, version,
					     MODE_CLIENT);
    clock_get_time_exact(&sntp_packet->xmt);
}


/*
 * sntp_receive
 *
 * Process a received SNTP packet.
 */
void sntp_receive (sntp_proc *proc, struct pkt *sntp_packet,
		   sntp_server *server, ulong length)
{
    uchar leap_ind;
    uchar mode;
    l_fp curtime;
    l_fp offset;
    l_fp p_xmt;
    l_fp p_rec;
    l_fp p_org;
    l_fp t_21;
    l_fp t_34;
    ulong version;

    /* What time is it? */

    clock_get_time_exact(&curtime);

    if (sntp_packets_debug) {
	buginf("\nReceived SNTP packet from %i, length %d",
	       server->server_address, length);
	sntp_dump_packet(sntp_packet);
	buginf("\n inp %08x.%08x (%CC)", curtime.l_ui, curtime.l_uf, "%B",
	       &curtime);
    }

    /* Toss packets that are too short. */
    
    if (length < LEN_PKT_NOMAC)
	return;

    /* Toss packets if they bear an unknown version number. */

    version = PKT_VERSION(sntp_packet->li_vn_mode);
    if (version < NTP_OLDVERSION || version > NTP_VERSION)
	return;

    /* Give up if the server is unsynchronized. */

    leap_ind = PKT_LEAP(sntp_packet->li_vn_mode);
    if (leap_ind == LEAP_NOTINSYNC) {
	server->server_stratum = STRATUM_UNSPEC; /* Don't sync to this guy. */
	return;
    }

    /* Toss packets of the wrong mode. */

    mode = PKT_MODE(sntp_packet->li_vn_mode);
    if (mode == MODE_BROADCAST) {
	if (!proc->sntp_broadcast_client) /* Not a broadcast client. */
	    return;
    } else if (mode != MODE_SERVER) {
	return;
    }

    /* Grab the fields and byte-swap as appropriate. */

    NTOHL_FP(&sntp_packet->rec, &p_rec);
    NTOHL_FP(&sntp_packet->xmt, &p_xmt);
    NTOHL_FP(&sntp_packet->org, &p_org);

    /* Toss the packet if the transmit timestamp is zero. */

    if (p_xmt.l_ui == 0)
	return;

    /* Update the server entry. */

    server->server_stratum = sntp_packet->stratum;
    get_timestamp(&server->server_reply_time);
    if (!server->server_configured) {
	server->server_version = version;
	mgd_timer_start(&server->server_timer, SNTP_SERVER_DELETION_TIMEOUT);
    }

    /*
     * Perform server selection.  If this box is our choice, process the
     * received sample.
     */
    if (sntp_select_server(proc) != server)
	return;

    /*
     * If this is a reply and all the fields are set properly,
     * calculate the offset, accounting for the round trip delay.
     */

    if ((mode == MODE_SERVER) && (p_org.l_ui != 0) && (p_rec.l_ui != 0)) {

	/*
	 * Unicast reply.  Use the offset formula:
	 * Originate Timestamp     T1   time request sent by client
	 * Receive Timestamp       T2   time request received at server
	 * Transmit Timestamp      T3   time reply sent by server
	 * Destination Timestamp   T4   time reply received at client
	 *           offset = ((T2 - T1) + (T3 - T4)) / 2.
	 */
	t_21 = p_rec;
	L_SUB(&t_21, &p_org);
	t_34 = p_xmt;
	L_SUB(&t_34, &curtime);
	L_RSHIFT(&t_21);		/* Divide before adding. */
	L_RSHIFT(&t_34);
	offset = t_21;
	L_ADD(&offset, &t_34);
	if (sntp_clockadjust_debug) {
	    buginf("\n Delay calculated,");
	}

    } else {

	/*
	 * Broadcast packet.  The offset is the other guy's time minus
	 * our time.
	 */
	offset = p_xmt;
	L_SUB(&offset, &curtime);
	if (sntp_clockadjust_debug) {
	    buginf("\n No delay calculated,");
	}
    }
    if (sntp_clockadjust_debug) {
	buginf(" offset %08x.%08x", offset.l_i, offset.l_f);
    }

    /*
     * Got the offset.  Now, if the offset is small enough, slew the
     * clock.  Otherwise, step it.  We assume a 64 second interval
     * between packets in determining the maximum value we can allow
     * for slewing.  If the slew fails, we ignore the time sample.
     * This may happen relatively often in the face of multiple
     * servers with a moderate offset between them.
     */

    if ((offset.l_i == 0 || offset.l_i == -1) &&
	(offset.l_f < SNTP_MAX_SLEW) && (offset.l_f > -SNTP_MAX_SLEW)) {
	if (clock_slew(offset.l_f, CLOCK_SOURCE_SNTP)) { /* Slew failed! */
	    if (sntp_clockadjust_debug) {
		buginf("\n Slew failed.");
	    }
	} else {
	    if (sntp_clockadjust_debug) {
		buginf("\n Clock slewed.");
	    }
	}
    } else {				/* Can't slew */
	clock_set(&p_xmt, CLOCK_SOURCE_SNTP); /* Just step it. */
	if (sntp_clockadjust_debug) {
	    buginf("\n Clock stepped.");
	}
    }
    clock_is_now_valid();
}
	
