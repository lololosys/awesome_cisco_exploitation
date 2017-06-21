/* $Id: tcpinput.c,v 3.10.4.10 1996/08/28 13:17:18 thille Exp $
 * $Source: /release/112/cvs/Xsys/tcp/tcpinput.c,v $
 *------------------------------------------------------------------
 * tcpinput.c -- routines which handle an incoming TCP datagram
 *
 * November 1985, Kirk Lougheed
 *
 * Copyright (c) 1985-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: tcpinput.c,v $
 * Revision 3.10.4.10  1996/08/28  13:17:18  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.10.4.9  1996/08/16  19:10:10  ahh
 * CSCdi63890:  %SYS-2-NOBLOCK: event dismiss with blocking disabled
 * Cannot block in IP Input context, so drop incoming SYN if we can't
 * respond reliably.
 * Branch: California_branch
 *
 * Revision 3.10.4.8  1996/08/10  01:46:38  jenny
 * CSCdi65660:  TCP hung in CLOSEWAIT after window closes
 * Branch: California_branch
 * Reset ONLY if we receive data after user had already closed
 * the connection.
 *
 * Revision 3.10.4.7  1996/08/09  22:31:11  jenny
 * CSCdi65660:  TCP hung in CLOSEWAIT after window closes
 * Branch: California_branch
 * . Receiver reset connection if packet received after user close.
 * . Always send zero-window probe if send window shrunk to zero and
 *   no unACKed packet in retransmission queue.
 *
 * Revision 3.10.4.6  1996/07/29  22:40:01  dawnli
 * CSCdi63787:  Router reponds to telnets destined for broadcast address
 * Branch: California_branch
 *
 * Revision 3.10.4.5  1996/06/28  23:30:17  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.10.4.4  1996/06/18  01:49:13  hampton
 * Split the monolithic traffic array into smaller per-protocol traffic
 * arrays.  [CSCdi59224]
 * Branch: California_branch
 *
 * Revision 3.10.4.3  1996/04/15  21:23:53  bcole
 * CSCdi54649:  IP Host Modularity, IP Automatic Stub Routing feature
 * commit
 * Branch: California_branch
 *
 * Revision 3.10.4.2  1996/03/28  00:16:10  jenny
 * CSCdi51402:  TCP Path-MTU Discovery needs to be dynamic
 * Branch: California_branch
 * Implemented TCP PMTU dynamic discovery and PMTU timers (RFC 1191).
 * Moved ip tcp parser command into a seperated parser chain in TCP for
 * modularity.
 *
 * Revision 3.10.4.1  1996/03/18  22:13:44  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.6.2.3  1996/03/16  07:41:10  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.6.2.2  1996/03/07  10:53:04  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.6.2.1  1996/02/20  21:21:26  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.10  1996/03/09  22:57:48  widmer
 * CSCdi47890:  HTTP server doesnt work
 * Add HTTP server
 *
 * Revision 3.9  1996/02/13  10:15:58  irfan
 * CSCdi47190:  bus error in telnet_input
 * ahh thinks tcp maybe giving us bad length when repeat FINs received.
 *
 * Revision 3.8  1996/01/29  11:39:06  billw
 * CSCdi46038:  vty access-class takes precedence over translate
 * access-class
 * Always use SYNC_LISTEN for incoming PT tcp sessions, and let the PT
 * process itself do the access check.
 *
 * Revision 3.7  1996/01/24  20:41:16  jenny
 * CSCdi47046:  TCP/IP need code clean up (part 3)
 * . removed ip/tuba.c, ip/tuba_default.c and ip/tuba.h
 * . removed all includes for ip/tuba.h.
 * . removed more TUBA related code.
 *
 * Revision 3.6  1996/01/22  07:29:20  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.5  1995/12/28  18:29:17  jenny
 * CSCdi46011:  TCP/IP need to do code clean up.
 * This is the first patch of code changes for TCP/IP code cleanup.
 * . removed all references to TUBA_IP and TUBA_CLNS.
 * . renamed open_tuba_socket() to ip_open_socket().
 * . renamed tcp_tuba_open() to tcp_open().
 * . pass addrtype * instead of ipaddrtype for local and foreight addresses
 *   to tcp_open() for feature portability.
 *
 * Revision 3.4  1995/12/13  04:24:44  ahh
 * CSCdi44230:  Remove dead code
 * Bring out your dead... *clang* (RSRB/TCP fast-switching)
 *
 * Revision 3.3  1995/11/21  21:51:26  jenny
 * CSCdi39128:  TCP needs to do fast retransmit
 * Implemted TCP fast retransmit and recovery algorithm. Fixed a bug in
 * TCP where an out-of-sequence FIN packet with no data got dropped during
 * processing.
 *
 * Revision 3.2  1995/11/17  18:49:09  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:36:02  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.9  1995/11/08  21:32:11  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.8  1995/08/18  02:58:23  ahh
 * CSCdi38935:  TCP: TCP Listener should use new process primitives
 * Use process and list primitives for connection queueing support.
 * Clean up some crufties, and delete some conditionally-compiled
 * debug code.
 *
 * Revision 2.7  1995/07/27  07:30:51  hampton
 * Rename insque/remque to lw_insert/lw_remove. [CSCdi37745]
 *
 * Revision 2.6  1995/07/12 23:45:39  hampton
 * Convert TCP to fully use the passive timers macros instead of
 * referencing the system clock directly.  [CSCdi37015]
 *
 * Revision 2.5  1995/07/10 22:11:42  ahh
 * CSCdi36259:  RLOGIN: pending output not flushed correctly (regress)
 * Code review comments.
 *
 * Revision 2.4  1995/06/22  05:51:17  ahh
 * CSCdi36259:  RLOGIN: pending output not flushed correctly (regress)
 *
 * Revision 2.3  1995/06/20  22:02:04  ahh
 * CSCdi31398:  Rlogin sending window size packet twice
 * Eliminate race conditions in rlogin's urgent-data processing.
 *
 * Revision 2.2  1995/06/17  06:26:41  ahh
 * CSCdi35734:  TCP: Long overdue code reorganization
 * Separate tcp.h into public and private header files.  Shuffle functions
 * around into more logical groupings.  Rename functions to meet naming
 * conventions (say goodbye to StudlyCaps).  Delete unneeded externs.
 * Trim off old log messages.
 *
 * Revision 2.1  1995/06/07  23:06:07  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <ciscolib.h>
#include <string.h>
#include "interface_private.h"
#include "packet.h"
#include "../ip/ip_registry.h"
#include "../ui/debug.h"
#include "../ip/ip.h"
#include "tcp.h"
#include "tcpinternal.h"
#include "tcpdriver.h"
#include "tcp_debug.h"
#include "../ip/ipoptions.h"
#include "../iprouting/route.h"
#include "../util/md5.h"
#include "logger.h"
#include "../tcp/msg_tcp.c"		/* Not a typo, see logger.h */


/*
 * Check to see if the usable window is non-zero.
 */

void tcp_check_window (tcbtype *tcb)
{
    if (tcp_usable_window(tcb)) {
	tcb->events |= TCPEV_WINDOW;
	if (tcb->activate)
	    tcb->activate(tcb, tcb->rxhandle);
	tcp_wakeup_process(tcb);
    }
}

/*
 * tcp_discardsegment:
 * Discard the specified segment.
 */

void tcp_discardsegment (paktype *packet, char *errstr)
{
    if (tcp_debug) {
	addrtype addr;
	ulong rcvnxt, rcvwnd;
	tcbtype *tcb;
	tcptype *tcp = tcp_get_hdr(packet, NULL);

	ip_extract_addr(packet, &addr, SOURCE);
	tcb = find_tcb(tcp->sourceport, tcp->destinationport, 
		       &addr, tcp->syn, FALSE);

	rcvnxt = tcb ? tcb->rcvnxt : 0L;
	rcvwnd = tcb ? tcb->rcvwnd : 0L;
	buginf("\nTCP%t: bad seg from %s -- %s: seq %lu ack %lu rcvnxt %lu rcvwnd %lu",
	       (tcb ? tcb->ttynum : 0), address_string(&addr),
	       (errstr ? errstr : "reason unknown"),
	       tcp->sequencenumber, tcp->acknowledgementnumber,
	       rcvnxt, rcvwnd);
    }
    retbuffer(packet);
}

static boolean tcp_checkmd5 (tcbtype *tcb, curpkttype *curpkt)
{
    uchar *md5;
    uchar buf[MD5_LEN];
    ushort saved_checksum;
    tcptype *tcp;
    iphdrtype *ip;

    tcp = curpkt->tcpptr;
    ip = (iphdrtype *) ipheadstart(curpkt->pakptr);
    if (curpkt->segdataoffset > TCPHEADERBYTES) {
	md5 = tcp_findoption(curpkt->tcpptr, MD5OPTION);
	if (md5) {
	    /* verify string */
	    if (md5[1] != MD5OPT_BYTES) {
		tcp_discardsegment(curpkt->pakptr, "MD5 string corrupt");
		return(FALSE);
	    }
#ifdef TCPVERBOSE_MD5
	    if (tcp_debug)
		buginf("\nTCP%t: received md5 string: "
		       "%02x %02x %02x %02x %02x %02x %02x %02x "
		       "%02x %02x %02x %02x %02x %02x %02x %02x",
		       tcb->ttynum, 
		       md5[2+0], md5[2+1], md5[2+2], md5[2+3],
		       md5[2+4], md5[2+5], md5[2+6], md5[2+7],
		       md5[2+8], md5[2+9], md5[2+10], md5[2+11],
		       md5[2+12], md5[2+13], md5[2+14], md5[2+15]);
#endif
	    saved_checksum = tcp->checksum;
	    tcp->checksum = 0;
	    tcp_md5digest(buf, ip->srcadr, ip->dstadr, TCP_PROT,
			  ip->tl - (ip->ihl << 2), tcp,
			  (uchar *) tcp + curpkt->segdataoffset,
			  tcb->md5_key, tcb->md5_keylen);
#ifdef TCPVERBOSE_MD5
	    if (tcp_debug)
		buginf("\nTCP%t: calculated md5 string: "
		       "%02x %02x %02x %02x %02x %02x %02x %02x "
		       "%02x %02x %02x %02x %02x %02x %02x %02x",
		       tcb->ttynum, 
		       buf[0], buf[1], buf[2], buf[3],
		       buf[4], buf[5], buf[6], buf[7],
		       buf[8], buf[9], buf[10], buf[11],
		       buf[12], buf[13], buf[14], buf[15]);
#endif
	    tcp->checksum = saved_checksum;
	    if (bcmp(buf, &md5[2], MD5_LEN) != 0) {
		tcp_discardsegment(curpkt->pakptr, "Invalid MD5 string");
		errmsg(&msgsym(BADAUTH, TCP), "Invalid",
		       ip->srcadr, tcp->sourceport,
		       ip->dstadr, tcp->destinationport,
		       tcp->rst ? " (RST)" : "");
		return(FALSE);
	    }
	} else {
	    tcp_discardsegment(curpkt->pakptr, "no MD5 string");
	    errmsg(&msgsym(BADAUTH, TCP), "No", ip->srcadr, tcp->sourceport,
		   ip->dstadr, tcp->destinationport, tcp->rst ? " (RST)" : "");
	    return(FALSE);
	}
    } else {
	tcp_discardsegment(curpkt->pakptr, "no MD5 string");
	errmsg(&msgsym(BADAUTH, TCP), "No", ip->srcadr, tcp->sourceport,
	       ip->dstadr, tcp->destinationport, tcp->rst ? " (RST)" : "");
	return(FALSE);
    }
    return(TRUE);
}


/*
 * Copy interesting bits of a TCB
 */

static void tcp_clone_tcb (tcbtype *tcb, tcbtype *newtcb)
{
    /*
     * Note that the flags get copied.  This means
     * that we can tell a TCB that was created via
     * a passive open by testing for TCB_GENTCBS.
     */
    newtcb->tty = tcb->tty;
    newtcb->ttynum = tcb->ttynum;
    newtcb->rcvwnd = newtcb->maxrcvwnd = tcb->maxrcvwnd;
    newtcb->localport = tcb->localport;
    newtcb->localhost = tcb->localhost;
    newtcb->flags = tcb->flags;
    newtcb->md5_keylen = tcb->md5_keylen;
    bcopy(tcb->md5_key, newtcb->md5_key, tcb->md5_keylen);
    newtcb->pid = tcb->pid;
    newtcb->param.tos = tcb->param.tos;

    newtcb->state = LISTEN;
    newtcb->events |= tcp_set_events[LISTEN];
    newtcb->events &= tcp_clear_events[LISTEN];
}

/*
 * tcp_process_fin
 * Handle a FIN
 */

static
void tcp_process_fin (tcbtype *tcb, ulong acknumber)
{
    if (tcb->bad_rst_count > TCP_RST_SECURITY_COUNT) {
	if (tcp_debug)
	    buginf("\nTCP%t: ignoring valid FIN due to storm", tcb->ttynum);
	return;
    }

    if (tcp_debug)
	buginf("\nTCP%t: FIN processed", tcb->ttynum);
    tcb->rcvnxt = tcb->rcvnxt + 1;
    
    tcp_sendacksegment(tcb);
    switch (tcb->state) {
    case SYNRCVD: 
    case ESTAB:
	tcp_changestate(tcb, CLOSEWAIT);
	tcb->lsterr = FOREIGN_RESET;
	break;
    case FINWAIT1: 
	if (acknumber != tcb->sndnxt) {
	    tcp_changestate(tcb, CLOSING);
	    break;
	}
	/* else fall through to FINWAIT2 case. */
    case FINWAIT2: 
	/* Stop the FINWAIT2-hang timeout */
	if (tcp_timerrunning(tcb, TMEWAITTIMEOUT))
	    tcp_stoptimer(tcb, TMEWAITTIMEOUT);
	tcp_changestate(tcb, TIMEWAIT);
	tcp_starttimer(tcb, TMEWAITTIMEOUT, tcb->timewait_time);
	/* Delayed ack timer already stopped from call to tcp_sendacksegment. */
	break;
    case TIMEWAIT: 
        if (!tcp_timerrunning(tcb, TMEWAITTIMEOUT))
	    tcp_starttimer(tcb, TMEWAITTIMEOUT, tcb->timewait_time);
	break;
    default: 
	break;
    }
}

/*
 * tcp_process_urg
 * Process the urgent data in the incoming segment somehow.
 */

static
void tcp_process_urg (tcbtype *tcb, curpkttype *curpkt, tcptype *tcp)
{
    /*
     * If we know the OOB byte then set it. Otherwise,
     * we go into urgent mode but set it to be -1.
     */
    if (tcp->urgentpointer <= curpkt->seglen) {
	tcb->oobptr = &((uchar *)tcp)[curpkt->segdataoffset +
				      tcp->urgentpointer - 1];
	tcb->oobbyte = *tcb->oobptr;
    } else if (tcb->rcvup == 0) {
	tcb->oobptr = NULL;
	tcb->oobbyte = -1;
    }
    tcb->rcvup = tcp->sequencenumber + tcp->urgentpointer;
}

/*
 * tcp_editurg
 * Edit out the urgent byte from this packet.
 * rlogin can't handle its "out-of-band" commands being in the data stream.
 */

static
void tcp_editurg (tcbtype *tcb, tcptype *tcp, paktype *pak, int dataoffset)
{
    int i, bytes;
    uchar *oobptr;

    bytes = pak->length - tcp->urgentpointer;
    oobptr = &((uchar *)tcp)[dataoffset + tcp->urgentpointer - 1];
    if (tcp_debug)
	buginf("\nTCP%t: removing urg byte %02x at %x, tcp %x urg %d bytes %d"
	       " len %d",
	       tcb->ttynum, oobptr[0], oobptr, tcp, tcp->urgentpointer, bytes,
	       pak->length);
    /*
     * Do the copy by hand since it is probably only going to be
     * a few bytes (actually, it will probably be nothing).
     */
    for (i = 0; i < bytes; i++)
	oobptr[i] = oobptr[i+1];
    pak->length--;
    if (pak->length == 0) {
	/*
	 * If segment is now empty, start the ACK timer since the app
	 * will never see this data and generate an ACK for us.
	 */
	if (!TIMER_RUNNING(tcb->timers[ACKTIMEOUT]))
	    tcp_starttimer(tcb, ACKTIMEOUT, tcb->ackholdtime);
    }
}

/*
 * tcp_qsegment:
 * Place incoming segment buffer on segment or save Queue, depending on its
 * relation to rcvnxt.  If the segment begins beyond rcvnxt then store it on
 * the save queue for later processing.
 * If the segment can be processed then:
 * Make dataptr and length of packet buffer refer only to actual
 * Tcp data for user.
 * Adjust for overlapping data by changing dataptr and length
 * of packet buffer.
 * Update Tcb state to reflect accepted segment.
 * Acknowledge data received back to remote side.
 */

static
void tcp_qsegment (tcbtype *tcb, curpkttype *curpkt, tcptype *tcp)
{
    paktype *packet, *pkt, *tmp;
    long diff;
    tcptype *tcp2;
    ulong seqnum;
    boolean do_driver_input;

    /*
     * Don't queue a segment with zero or fewer (!) bytes
     */
    if (curpkt->seglen <= 0 && curpkt->tcpptr->fin == 0) {
	retbuffer(curpkt->pakptr);
	return;
    }
    packet = curpkt->pakptr;
    seqnum = curpkt->tcpptr->sequencenumber;

    /*
     * Remove input queueing information in case we hold this for a while.
     */
    clear_if_input(packet);

    /*
     * Check if this segment begins before or at rcvnxt.
     */
    if (!tcp_seqinrange(tcb->rcvnxt - curpkt->seglen, seqnum, tcb->rcvnxt)) {

	/*
	 * Segment begins past rcvnxt, so make sure there is room in
	 * the save queue.
	 */
	if ((tcb->qsaved >= QUEUEMAX) || (tcb->flags & TCB_IDLEUSER)) {
	    retbuffer(packet);
	    return;
	}
	tcb->pakinorder++;

	packet->us_seglen = curpkt->seglen;
	packet->us_segdataoffset = curpkt->segdataoffset;

	/*
	 * Check for a FIN that arrives out of sequence, and complain a bit
	 */
	if (tcp_debug)
	    if (tcp->fin)
		buginf("\nTCP%t: FIN with sequence %u, out of order",
		       tcb->ttynum, seqnum);

 	tmp = (paktype *) tcb->q[SAVEQUEUE].qtail;
	if (tmp)
	    tcp = tcp_get_hdr(tmp, NULL);
	else
	    tcp = NULL;
	if (QUEUEEMPTY(&tcb->q[SAVEQUEUE]) ||
	    (tcp && tcp_seqinrange(tcp->sequencenumber, seqnum,
				   tcb->rcvnxt + tcb->rcvwnd))) {
	    enqueue(&tcb->q[SAVEQUEUE], packet);
	    tcb->qsaved++;
	    /* 
	     * Ack the out-of-sequence packet right away.
	     */
	    tcp_sendacksegment(tcb);
	    tcp_stoptimer(tcb, ACKTIMEOUT);
	    return;
	}
	
	/*
	 * If this packet belongs *before* the beginning of the first
	 * queued segment, make it the new "head" of the queue. Ack
	 * the out-of-sequence packet right away.
	 */ 

	tcp = tcp_get_hdr((paktype *)tcb->q[SAVEQUEUE].qhead, NULL);

	if (tcp_seqinrange(tcb->rcvnxt, seqnum, tcp->sequencenumber)) {
	    requeue(&tcb->q[SAVEQUEUE], packet);
	    tcb->qsaved++;
	    tcp_sendacksegment(tcb);
	    tcp_stoptimer(tcb, ACKTIMEOUT);
	    return;
	}

	/*
	 * If we get this far, the segment belongs in the middle of the
	 * queue someplace...
	 *
	 * Find where in the queue the packet belongs.  We already know
	 * that it isn't at the head or tail of it - so no special
	 * cases must be handled. Ack the out-of-sequence packet right 
	 * away.
	 *
	 */

	for (pkt = (paktype *) tcb->q[SAVEQUEUE].qhead; 
	     pkt->next != NULL; pkt = pkt->next) {
	    tcp = tcp_get_hdr(pkt, NULL);
	    tcp2 = tcp_get_hdr(pkt->next, NULL);
	    if (tcp_seqinrange(tcp->sequencenumber, seqnum,
			       tcp2->sequencenumber)) {
		packet->next = pkt->next;
		pkt->next = packet;
		tcb->qsaved++;
	    	tcp_sendacksegment(tcb);
	    	tcp_stoptimer(tcb, ACKTIMEOUT);
		return;
	    }
	}
	
	/* We couldn't find a home for it, so dump it... */

	retbuffer(packet);
	return;
    }

    /*
     * Segment on or overlapping rcvnxt
     * Trim front of window, check again for valid length
     */
    diff = (long) (tcb->rcvnxt - tcp->sequencenumber);
    packet->dataptr += curpkt->segdataoffset + diff;
    packet->length = curpkt->seglen - diff;
    if (packet->length <= 0 && curpkt->tcpptr->fin == 0) {
	retbuffer(packet);
	return;
    }

    /*
     * The tcpLength and dataoffset fields will be used as ptrs into the data
     * field and don't take into account the header length anymore.
     * Check for segment larger than rcvwnd unless this packet contains a FIN.
     * Absorb a too large FIN packet rather than risk losing data.
     */
    if ((packet->length > tcb->rcvwnd) && !tcp->fin)
	packet->length = tcb->rcvwnd;		/* trim too long packet */
    tcb->rcvnxt += packet->length;
    if (tcp->fin) 		/* adjust ACK for FIN */
	tcp_process_fin(tcb, tcp->acknowledgementnumber);
    if (tcp->urg && (tcb->flags & TCB_EDITURG) &&
	(tcp->urgentpointer <= curpkt->seglen))
	tcp_editurg(tcb, tcp, packet, curpkt->segdataoffset);

    if (packet->length == 0) {	/* Nothing there, let's go... */
	retbuffer(packet);		/* flush unwanted datagram */
	return;
    }
    tcb->bytesincount += packet->length;	/* keep total bytes input */
    tcb->pakindata++;				/* count data packets */
    tcp_wakeup_process(tcb);

    /*
     * If we are an idled connection, allow two datagrams to be queued.
     * Pretend to read all the rest.  This allows output notification to
     * work and gives the user a clue as to what triggered notification.
     * Don't discard a packet containing a FIN!
     */
    if ((tcb->flags & TCB_IDLEUSER) && (tcb->qinput >= 2)) {
	retbuffer(packet);		/* flush unwanted datagram */
	do_driver_input = FALSE;
    } else {
	tcb->unreadbytes += packet->length;	/* note bytes pending */
	tcp_changercvwnd(tcb, -packet->length);	/* adjust window */
	enqueue(&tcb->q[SEGQUEUE], packet);
	tcb->qinput++;			/* count datagrams enqueued */
	do_driver_input = TRUE;
    }

    /*
     * Check to see if this new segment allows any of the saved segments
     * to be processed.
     */
    if (!QUEUEEMPTY(&tcb->q[SAVEQUEUE]) && !(tcb->flags & TCB_IDLEUSER))
	tcp_processsavedsegments(tcb);


    /*
     * Drive those that need it
     */
    if (do_driver_input && tcb->rxnotify) {
	/*
	 * This magic code is used to determine if the tcpdriver
	 * caused an ack to be sent when it read the data.  If
	 * not we send one.  Perverse?
	 */
	if (!TIMER_RUNNING(tcb->timers[ACKTIMEOUT]))
	    tcp_starttimer(tcb, ACKTIMEOUT, tcb->ackholdtime);
	(*tcb->rxnotify)(tcb, tcb->rxhandle);
	if (TIMER_RUNNING(tcb->timers[ACKTIMEOUT]))
	    tcp_sendacksegment(tcb);
	return;
    }

    if (tcp->fin)
	return;

    /*
     * Start delayed ACK timer
     */
    if (!TIMER_RUNNING(tcb->timers[ACKTIMEOUT]))
	tcp_starttimer(tcb, ACKTIMEOUT, tcb->ackholdtime);
}

/*
 * tcp_handleoptions:
 * Processes options present in incoming Tcp packet.
 * Assumes that options are present.
 * Returns TRUE if MSS option was seen and processed.
 *
 * Parameters:
 * tcb     Ptr to tcb of current connection
 * curpkt  Ptr to rec. for current packet
 */

static
void tcp_handleoptions (tcbtype *tcb, curpkttype *curpkt)
{
    int i;
    tcptype *p;
    int size, len;
    int option, optlen;
    boolean done;

    i = 0;
    p = curpkt->tcpptr;
    len = curpkt->segdataoffset - TCPHEADERBYTES;
    done = FALSE;
    while (!done && i < len) {
	switch (option = p->data[i]) {
	case ENDOPTIONLIST:
	    done = TRUE;
	    break;

	case NOPOPTION:
	    i++;
	    break;

	case MAXSEGSIZEOPTION:
	    i++;
	    if (p->data[i] != MSSOPT_BYTES) {	/* validate length */
		done = TRUE;
		break;
	    }
	    i++;
	    size = GETSHORT(&p->data[i]);
	    i += sizeof(ushort);
	    tcp_mss(tcb, size);
	    break;

	case MD5OPTION:
	    if (tcb->flags & TCB_MD5) {
		optlen = p->data[i] - 2;	/* adjust for opt + len */
		i++;
		i += optlen;
		break;
	    }
	    /*FALLTHROUGH*/
	default:	/* Silently consume the option we don't support */
	    i++;
	    optlen = p->data[i] - 2;	/* adjust for opt + len */
	    if (optlen <= 0) {
		done = TRUE;	/* XXX perhaps should reset conn */
		break;
	    }
	    i++;
	    if (tcp_debug)
		buginf("\nTCP%t: Connection to %s:%d, ignoring option %d",
		       tcb->ttynum, address_string(&tcb->foreignhost),
		       tcb->foreignport, option);
	    i += optlen;
	    break;
	}
    }
}

/*
 * tcp_handleother:
 * Check fin and urg bits of incoming segment as a fcn. of sendAckedFlag.
 * Handle any options present.
 * Queue any data in the segment for the receiver.
 *
 * Parameters:
 * tcb     Ptr to tcb of current connection
 * curpkt  Ptr to rec. for current packet
 * sendAckedFlag
 */

void tcp_handleother (tcbtype *tcb, curpkttype *curpkt, int sendAckedFlag)
{
    tcptype *tcp = curpkt->tcpptr;
    boolean gotmss = FALSE;

    if (tcp->fin)
	if (sendAckedFlag)
	    tcp_changestate(tcb,CLOSEWAIT);
	else {
	    tcp_discardsegment(curpkt->pakptr, 
			       "FIN set and our SYN not yet acked.");
	    return;
	}
    if (tcp->urg) {
	tcp_process_urg(tcb, curpkt, tcp);
	if (tcp_debug)
	    buginf("\nTCP: urgent data rcvd state %s, rcvup %lu",
		tcp_statestring(tcb->state), tcb->rcvup);
    }
    if (curpkt->segdataoffset > TCPHEADERBYTES) {
	tcp_handleoptions(tcb, curpkt);
	if (tcp_findoption(curpkt->tcpptr, MAXSEGSIZEOPTION))
	    gotmss = TRUE;
    }

    if (tcp->syn && !gotmss) {
	/* Other side SYN'ed but didn't send an MSS.  Fake it. */
	tcp_mss(tcb, TCP_DEFAULT_MSS);
    }

    if (curpkt->seglen > 0)
	tcp_qsegment(tcb, curpkt, tcp);
    else
	retbuffer(curpkt->pakptr);
}

/*
 * Coalesce tcp data in queue, given the max size of members in that queue.
 * Coalescing is done by traversing the queue, looking first for a 'packable'
 * member that can pack more data to it. When a member is found, will 
 * lookahead at the next member and pack its data to the first member, 
 * if possible, and that next member is then removed from the queue. 
 * This continues until the original first member is packed or the whole 
 * queue is traversed. This coalescing is repeated for the next 
 * 'packable' member, until the whole queue is traversed.  
 * Note there is no interrupt interlocking. 
 */

static
void tcp_coalesce_queue (queuetype *qptr, tcbtype *tcb, int max_size)
{
   paktype *pak;
   int count = 0;
   int bytes_merged = 0;
   int remaining_size;

   pak = (paktype *)qptr->qhead;

   while (pak) {
  
      /*
       * The max size is the caller's requested maximum packing size for a 
       * pak. Since this maximum may actually be larger than the pak's 
       * absolute size, the remaining size is calculated as the minimum
       * of the two sizes minus the the actual pak's length.
       */
      remaining_size = min((max_size - pak->length), 
                           ((pak_center(pak) + pak_get_pool_size(pak)) - 
                            (pak->dataptr + pak->length)));


      /*
       * So long as there is space remaining in the current member and
       * the next member fits into the current member's remaining size,
       * merge to the current member and remove that next member. Removal of 
       * subsequent data also has the effect of keeping the ip and tcp header 
       * of the original current member only. 
       * Don't merge any members that have urgent data in them - tcp_getstring 
       * needs the original sequence number to know that there is urgent data 
       * in that packet to notify application level.
       */
      while ((remaining_size > 0) && pak->next && !(TcpPkt(pak->next)->urg)
           && ((pak->next->length) <= remaining_size)){
       
           /*
            * Copy to the current member 
            */
           bcopy(pak->next->dataptr, (pak->dataptr + pak->length),
                 pak->next->length);
            
           /*
            * Count the packets and bytes merged
            */
           count++;
           bytes_merged +=  pak->next->length;   

           /*
            * Update current member's size and remaining size
            */
           remaining_size -=  pak->next->length;
           pak->length += pak->next->length;
           datagram_done((paktype *) remqueue(qptr, pak->next, pak));
      }
      pak = pak->next;
   }

   if (tcp_debug)
     buginf("\nTTY%t packets merged=%d bytes merged=%d", 
            tcb->ttynum, count, bytes_merged);

}

/*
 * tcp_newconn
 * Queue up a new connection on a TCB's parent.
 */

static void
tcp_newconn (tcbtype *tcb)
{
    tcbtype *parent = tcb->parent;

    list_enqueue(&parent->connq, &tcb->connq_element, tcb);
    if (!LIST_ELEMENT_QUEUED(&parent->newconn_element))
	list_enqueue(&tcp_newconn_list, &parent->newconn_element, parent);
    tcp_wakeup_process(parent);
}

/*
 * check_rst_n_bogus
 * (Check RST and bogus packets)
 * Returns TRUE if incoming segment ACKs an invalid sequence number
 * (out of our send sequence space) or the segment contains RST. 
 */
static boolean
check_rst_n_bogus (tcbtype *tcb, tcptype *tcp, curpkttype *curpkt, 
		    int tcp_state, boolean *ack_ok)  
{
    /*
     * Check the Ack bit. 
     */
    *ack_ok = FALSE;
    if (tcp->ack) {
	/*
	 * If ACKed number is totally out of our sequence space, send 
	 * RST to remote if the incoming packet didn't already have RST.
	 * Otherwise, silently drop the packet. We will either be in
	 * sync with remote when we get a valid packet or the connection
	 * will timeout eventually.
	 */
	if (!tcp_seqinrange(tcb->iss + 1, tcp->acknowledgementnumber, 
			    tcb->sndnxt)) {
	    if (!tcp->rst) {
		tcp_sendcontrolsegment(tcb, tcb->localport, tcb->foreignport,
				       &tcb->foreignhost,
				       tcp->acknowledgementnumber,
				       FALSE, 0, TRUE, FALSE);
	    }
	    tcp_discardsegment(curpkt->pakptr, "Bad ACK in SYNSENT");
	    return(TRUE);
	} else {
	    /*
	     * Make sure the ACKed sequence number is also within our
	     * send window.
	     */
	    if (tcp_seqinrange(tcb->snduna, tcp->acknowledgementnumber,
			       tcb->sndnxt)) {
		*ack_ok = TRUE;
	    }
	}
    }

    /*
     * Check the Rst bit.
     */

    if (tcp->rst) {
	/*
	 * Drop the connection upon receiving a RST packet only if the
	 * RST packet contains valid ACK number. Otherwise, it could 
	 * be a spoofed packet, just sliently drop it. The connections
	 * will be in sync again once we receive a valid packet from 
	 * remote or it will time out.
	 */ 
	if (*ack_ok)
	    tcp_closeconnection(tcb, curpkt, REJECTED, "remote sent RST");
	else
	    tcp_discardsegment(curpkt->pakptr, "Rst bit set.");
	return(TRUE);
    }
    return(FALSE);
}


/*
 * tcp_inputsegment
 * Checksum, demultiplex, and deliver a single TCP segment
 */

void tcp_inputsegment (paktype *pak)
{
    iphdrtype *ip;
    curpkttype currentpacket;
    curpkttype *curpkt;
    tcptype *tcp;
    tcbtype *tcb;
    int len;
    addrtype addr;
    ulong old_usable;
    iphdrtype *curpkt_ip;
    boolean discarded_pak; 	/* True if segment is dropped either because 
				   of an invalid ACK number or it's a RST pak */
    boolean acceptableAck;	/* Signals whether ACK numbers in segment are 
				   acceptable. */
    int status;

    ip = (iphdrtype *)ipheadstart(pak);

    tcp_traffic.inputs++;
    curpkt = &currentpacket;

    /*
     * Determine which network layer gave us the packet.
     */
    if (pak->linktype == LINK_IP) {
	len = (ip->ihl << 2);
	tcp = (tcptype *)((char *)ip + len);
	len = n2h(ip->tl) - len;
	if (CheckSum((uchar *)tcp, len, ip->srcadr, ip->dstadr, ip->prot)) {
	    if (tcp_debug)
		buginf("\nTCP: checksum failure <%i,%d> <%i,%d>",
		       ip->srcadr, tcp->sourceport,
		       ip->dstadr, tcp->destinationport);
	    IP_DISPOSE(tcp, checksumerr, pak);
	    return;
	}
	addr.type = ADDR_IP;
	addr.ip_addr = ip->srcadr;
    } else {
	if (tcp_debug)
	    buginf("\nTCP: packet discarded, invalid linktype %d", 
		   pak->linktype);
	retbuffer(pak);
	return;
    }

    if (!pak || (pak->flags & (PAK_BROADCAST|PAK_MULTICAST))) {
	/*
	 * Allow http config to broadcast addr.   XXX
	 */
      if (tcp->destinationport != WWW_PORT) {
	    retbuffer(pak);
	    return;
	}
    }

    /* Don't accept connection requests (or anything else)
     * until the system is fully initialized.
     */
    if (!systeminit_flag && tcp->syn && !tcp->ack) {
	tcp_discardsegment(pak, "System not ready");
	return;
    }

    /*
     * For little endians, byte swap
     */
    tcp->sourceport = n2h(tcp->sourceport);
    tcp->destinationport = n2h(tcp->destinationport);
    tcp->sequencenumber = n2hl(tcp->sequencenumber);
    tcp->acknowledgementnumber = n2hl(tcp->acknowledgementnumber);
    tcp->window = n2h(tcp->window);
    tcp->urgentpointer = n2h(tcp->urgentpointer);

    /*
     * Find a tcp connection that exists or a wild listener.
     */
    tcb = find_tcb(tcp->sourceport, tcp->destinationport, 
		   &addr, tcp->syn, TRUE);
#ifdef TCPVERBOSE
    if (tcp_debug) {
	buginf("\nTCP: input %s(%d) -> %d %s, seq %lu, ack %lu, win %u",
	       address_string(&addr), tcp->sourceport, 
	       tcp->destinationport, (tcp->syn ? "SYN" : ""), 
	       tcp->sequencenumber, tcp->acknowledgementnumber, tcp->window);
    }
#endif
    if (!tcb) {
	/*
	 * In the cisco tcp architecture, there should always be a "wild"
	 * listener available for accepting SYN packets.  If not, this is a
	 * temporary condition caused by multiple SYNs arriving before a new
	 * wild listener can be set up.  In this case, do nothing - catch the
	 * correct action for the packet on the next retransmission.
	 */
	if (tcp->syn) {
	    tcp_discardsegment(pak, "No wild listener");
	    return;
	}
	tcp_replytoconnectionlesspacket(pak);
	tcp_traffic.noport++;
	return;
    }
#ifdef TCPVERBOSE
    if (tcp_debug) {
	buginf("\nTCP: sndnxt %lu, rcvnxt %lu, qi %d, qo %d, qs %d, brs %lu",
	       tcb->sndnxt, tcb->rcvnxt, tcb->qinput, tcb->qoutput,
	       tcb->qsaved, tcb->brs);
	buginf("\nTCP: snduna %lu, sndwnd %u, sndwl1 %lu, sndwl2 %lu",
	       tcb->snduna, tcb->sndwnd, tcb->sndwl1, tcb->sndwl2);
	buginf("\nTCP: rcvwnd %u, del %u, max %u, add %u, unread %d",
	       tcb->rcvwnd, tcb->delrcvwnd, tcb->maxrcvwnd,
	       tcb->addwnd, tcp_unread(tcb));
    }
#endif
    pak->dataptr = (uchar *) tcp;	/* start of TCP header */
    pak->length = len;			/* TCP pkt length */

    /* Junk the segment now if closed. */
    if (tcb->state == CLOSED) {
	tcp_discardsegment(pak, "CLOSED state");
	return;
    }

    curpkt->foreignhost = addr;
    curpkt->foreignport = tcp->sourceport;
    curpkt->pakptr = pak;
    curpkt->tcpptr = tcp;
    curpkt->seglen = pak->length - (tcp->dataoffset << 2);
    curpkt->segprc = 0;
    curpkt->segdataoffset = tcp->dataoffset << 2;

    /*
     * Check the MD5 signature if necessary.
     */
    if ((tcb->flags & TCB_MD5) && (tcp_checkmd5(tcb, curpkt) == FALSE)) {
	return;
    }

    /*
     * Check if keepalives are enabled.
     * Pretend that they are enabled if we are in a condition we could get
     * stuck in very easily (out of order segments have been received)
     */
    if (tcb->flags & TCB_KEEPALIVE ||
	(tcb->qsaved > 0 && (tcb->flags & TCB_PASSIVEOPEN) &&
	 tcb->qinput == 0 && !TIMER_RUNNING(tcb->timers[RETRANSTIMEOUT]))) {
	tcp_starttimer(tcb, KEEPALIVETIMEOUT, tcb->maxaliveticks);
    }
    /*
     * Be safe and reset the keepalive timeout count EVERY time we get
     * a packet, whether or not we think these timers are in use!
     */
    tcb->timerexp[KEEPALIVETIMEOUT] = 0;

    /*
     * Reset the black hat timer to zero.
     */
    if (tcb->bad_rst_count && CLOCK_OUTSIDE_INTERVAL(tcb->bad_rst_timestamp,
						     TCP_RST_SECURITY_WINDOW))
	tcb->bad_rst_count = 0;

    old_usable = tcp_usable_window(tcb);

    if (tcppkt_debug)
	print_tcp(tcb, pak, tcp, 'I', (pak->linktype == LINK_IP) ? ip->id : 0);

    /* Header prediction */
    if (tcb->state == ESTAB &&
	tcp->ack && !tcp->syn && !tcp->fin && !tcp->rst && !tcp->urg &&
	tcp->sequencenumber == tcb->rcvnxt &&
	tcp->window && tcp->window == tcb->sndwnd &&
	tcb->sndnxt == tcb->sndmax) {
	if (curpkt->seglen == 0) {
	    if (GT(tcp->acknowledgementnumber, tcb->snduna) &&
		LEQ(tcp->acknowledgementnumber, tcb->sndmax) &&
		tcb->sndcwnd >= tcb->sndwnd) {
		/*
		 * this is a pure ack for outstanding data.
		 */
		tcb->snduna = tcp->acknowledgementnumber;
		if (GT(tcp->sequencenumber, tcb->sndwl1) &&
		    LEQ(tcp->sequencenumber, tcb->rcvnxt)) {
		    tcb->sndwnd = tcp->window;
		    tcb->sndwl1 = tcp->sequencenumber;
		    tcb->sndwl2 = tcp->acknowledgementnumber;
		}
		retbuffer(curpkt->pakptr);
		if (tcb->snduna == tcb->sndmax) {
		    tcp_stoptimer(tcb, RETRANSTIMEOUT);
		    tcb->flags &= ~TCB_TIMINGOUT;
		}

		tcp_removeackedsendsegments(tcb);
		tcb->pakincount++;
		if (old_usable == 0)
		    tcp_check_window(tcb);

		return;
	    }
	} else if (tcp->acknowledgementnumber == tcb->snduna &&
		   QUEUEEMPTY(&tcb->q[SAVEQUEUE]) &&
		   curpkt->seglen <= tcb->rcvwnd) {
	    paktype *packet = curpkt->pakptr;

	    /*
	     * this is a pure, in-sequence data packet
	     * with nothing on the reassembly queue and
	     * and we have enough buffer space to take it.
	     */

	    /* This is tcp_qsegment() simplified considerably. */

	    /*
	     * Remove input queueing information in case we hold this
	     * for a while.
	     */
	    clear_if_input(packet);

	    packet->dataptr += curpkt->segdataoffset;
	    packet->length = curpkt->seglen;
	    tcb->bytesincount += packet->length;
	    tcb->pakindata++;
	    tcb->rcvnxt += packet->length;
	    tcp_wakeup_process(tcb);

	    /*
	     * If we are an idled connection, allow two datagrams to be queued.
	     * Pretend to read all the rest.  This allows output notification
	     * to work and gives the user a clue as to what triggered
	     * notification.
	     * Don't discard a packet containing a FIN!
	     */
	    if ((tcb->flags & TCB_IDLEUSER) && (tcb->qinput >= 2)) {
		retbuffer(packet);	/* flush unwanted datagram */
	    } else {
		tcb->unreadbytes += packet->length;
		/* Fast path reduce receive window */
		if (packet->length > tcb->rcvwnd)
		    tcb->rcvwnd = 0;
		else
		    tcb->rcvwnd -= packet->length;
		enqueue(&tcb->q[SEGQUEUE], packet);

                /* 
                 * If input queue is non-zero and has reached maximum, 
                 * kick in the TCP packet coalescing operation
                 */ 
                if (tcb->qinput_max && 
                    ((&tcb->q[SEGQUEUE])->count >= tcb->qinput_max))
		    tcp_coalesce_queue(&tcb->q[SEGQUEUE], tcb,
				       tcb->maxsegmentsize);
		tcb->qinput++;

		/*
		 * Drive those that need it
		 */
		if (tcb->rxnotify) {
		    /*
		     * This magic code is used to determine if the tcpdriver
		     * caused an ack to be sent when it read the data.  If
		     * not we send one.  Perverse?
		     */
		    if (!TIMER_RUNNING(tcb->timers[ACKTIMEOUT]))
			tcp_starttimer(tcb, ACKTIMEOUT, tcb->ackholdtime);
		    (*tcb->rxnotify)(tcb, tcb->rxhandle);
		    if (TIMER_RUNNING(tcb->timers[ACKTIMEOUT]))
			tcp_sendacksegment(tcb);
		    tcb->pakincount++;
		    if (old_usable == 0)
			tcp_check_window(tcb);

		    return;
		}
	    }

	    /*
	     * Start delayed ACK timer
	     */
	    if (!TIMER_RUNNING(tcb->timers[ACKTIMEOUT]))
		tcp_starttimer(tcb, ACKTIMEOUT, tcb->ackholdtime);
	    tcb->pakincount++;
	    if (old_usable == 0)
		tcp_check_window(tcb);
	    return;
	}
    }

    switch (tcb->state) {
    case LISTEN: {
	idbtype *idb;		/* idb for setting initial SRTT */
	ipaddrtype dummy;	/* used for getting an IDB */

	/*
	 * Check for a Rst.
	 */
	if (tcp->rst) {
	    tcp_discardsegment(curpkt->pakptr, "RST while in LISTEN");
	    break;
	}

	/*
	 * Check for an Ack.
	 */
	if (tcp->ack) {
	    if (tcp_debug) {
		buginf("\nTCP: input %s(%d) -> %d: ACK while in LISTEN",
		       address_string(&curpkt->foreignhost),
		       tcp->sourceport, tcp->destinationport);
	    }
	    tcp_replytoconnectionlesspacket(curpkt->pakptr);
	    break;
	}

	/*
	 * Check for a Syn. 
	 */
	if (tcp->syn) {
	    iphdrtype *curpkt_ip = (iphdrtype *)ipheadstart(curpkt->pakptr);

	    /*
	     * If there are no queueing elements available, we can't
	     * send a packet reliably.  We cannot block here to wait
	     * for elements to become available, so give up.
	     */
	    if (element_num_elements() == 0) {
		/* No hope. */
		tcp_discardsegment(curpkt->pakptr, "No elements");
		break;
	    }

	    /*
	     * Check if access is allowed for this port
	     */
	    if (!tcp_access_check(tcb, tcp->sourceport,
	        reg_invoke_ipalias_port(curpkt_ip->dstadr,
	        tcp->destinationport),
	        curpkt->foreignhost.ip_addr)) {
	        tcp_replytoconnectionlesspacket(curpkt->pakptr);
	        break;
	    }

	    /*
	    * Process precedence
	    */
	    if (curpkt->segprc > tcb->prc)
	        if (tcb->flags & TCB_HIGHERPRC)
	            tcb->prc = curpkt->segprc;
	        else {
	            tcp_sendcontrolsegment(tcb, tcp->destinationport,
	                                   tcp->sourceport,
	                                   &curpkt->foreignhost,
	                                   tcp->acknowledgementnumber,
	                                   FALSE, 0, TRUE, FALSE);
	            tcp_discardsegment(curpkt->pakptr, "Bad precedence");
	        break;
	    }

	    /*
	     * Process any incoming options.  Copy options that we
	     * need to remember into an holding place.  If this dies
	     * send a reset to the source and dump the packet.
	     */
	    if (curpkt_ip->ihl > btol(MINIPHEADERBYTES)) {
	        tcb->options = malloc(sizeof(ipoptiontype));
	        if (tcb->options) {
	            if (!ipopt_copytcb(tcb->options, curpkt->pakptr)) {
	            /*
	            * Oh Bother!  couldn't deal with the options
	            * for some reason -- hopefully the lower layers
	            * have told somebody about it.  Send a reset
	            * to the other side and drop the packet.  Be
	            * careful because we will reuse this tcb for
	            * the wild listen.  We do not release it.
	            */
			free(tcb->options);
			tcb->options = NULL;
			tcp_replytoconnectionlesspacket(curpkt->pakptr);
			break;
	    	    }
		} else {
		    /* Couldn't get any space for the option bits in the
		     * tcb block.  We could dump it on the ground here
		     * and send a reset back because we can't deal with
		     * it, but for now we will let it in.  We will punt
		     * any options (thrown away) including source route
		     * and security but not to worry.  If we are secured
		     * an appropriate security will be inserted 
		     * automatically on the way out.
		     */
	    	}
	    }

	    if (tcb->flags & TCB_GENTCBS) {
		tcbtype *newtcb;
		int error;
		ipoptiontype *ipopts = NULL;

		/*
		 * Make a new TCB for the incoming connection.
		 * It will be this new TCB that moves from the
		 * LISTEN state to ESTAB (and beyond) while the
		 * original TCB stays in LISTEN, ready for more
		 * connections (up to connqlimit).
		 */
		if (LIST_FULL(&tcb->connq)) {
		    tcp_discardsegment(curpkt->pakptr,
				       "connection queue limit reached");
		    break;
		}

		/* Make a new copy of the options. */
		if (tcb->options) {
		    ipopts = malloc(sizeof(ipoptiontype));
		    if (ipopts == NULL) {
			tcp_discardsegment(curpkt->pakptr,
					   "no memory to accept connection");
			break;
		    }
		    bcopy(tcb->options, ipopts, sizeof(ipoptiontype));
		}

		newtcb = tcp_create(tcb->tty, &error, ipopts);
		if (newtcb == NULL) {
		    tcp_discardsegment(curpkt->pakptr,
				       "no memory to accept connection");
		    break;
		}
		tcp_clone_tcb(tcb, newtcb);
		newtcb->parent = tcb;
		newtcb->internal_flags |= TCB_INT_WAITING;
		/* All further manipulations will be on the new TCB. */
		tcb = newtcb;
	    }

	    /*
	     * Adjust for sequence number of syn bit.
	     */
	    tcb->irs = tcp->sequencenumber;
	    tcp->sequencenumber++;
	    tcb->brs = tcp->sequencenumber;
	    tcb->rcvnxt = tcp->sequencenumber;

	    /*
	     * Remember foreign host and port.  Use same local host address as
	     * selected by foreign side -- until now we wouldn't know which of
	     * our addresses would be optimal.  If local port is wild carded,
	     * set the local port.
	     */
	    if (!(tcb->flags & TCB_GENTCBS))	/* if not a genned TCB... */
		tcb_unqueue(tcb);		/* remove from listener */
	    tcb->foreignport = curpkt->foreignport;
	    address_copy(&tcb->foreignhost, &curpkt->foreignhost);
	    curpkt_ip = (iphdrtype *) ipheadstart(curpkt->pakptr);
	    tcb->localhost.type = ADDR_IP;
	    tcb->localhost.ip_addr = curpkt_ip->dstadr;
	    if (ip_aliasaddress(tcb->localhost.ip_addr))
		tcb->flags |= TCB_ALIAS;

	    /*
	     * Revise guess of SRTT if we're using a slow interface (< 100 
	     * Kbit). Our default guess is good enough for fast LAN's.
	     */
	    dummy = curpkt->foreignhost.ip_addr;
	    idb = reg_invoke_ip_first_hop(&dummy, dummy, IP_PATH_DEFFLAGS);
	    if (idb && idb->visible_bandwidth < 100) {
		tcb->srtt = TCP_SLOWRTT;
		tcb->rtto = TCP_SLOWRTO;
	    }

	    if (tcb->localport == 0)
		tcb->localport = curpkt->tcpptr->destinationport;

	    /*
	     * Add to queue of TCB's
	     */
	    tcb_enqueue(tcb);

	    /*
	     * SYNCLISTEN can get set during access-list checking for certain
	     * types of connection (ie protocol translation.)  When this
	     * happens, higher level code is given the repsonsibility of
	     * finishing the establishment of the connection.
	     * (If they call close_connection() instead of finishing the
	     * open with tcp_establish(), the source host should get a
	     * "connection refused" sort of message.)
	     */
	    if (tcb->flags & TCB_SYNCLISTEN) {
		/* Save where we were, and bug out.  Listener will
		 * call tcp_establish() later.
		 */
		tcb->saved_curpkt = malloc(sizeof(curpkttype));
		if (tcb->saved_curpkt == NULL) {
		    tcp_replytoconnectionlesspacket(curpkt->pakptr);
		    tcb->flags &= ~TCB_SYNCLISTEN;
		    return;
		}
		bcopy(curpkt, tcb->saved_curpkt, sizeof(curpkttype));
		tcp_changestate(tcb, SYNRCVD);

		/*
		 * If this was a generated TCB, tell the upper layer
		 * that there's a connection waiting to get established.
		 */
		if (tcb->flags & TCB_GENTCBS)
		    tcp_newconn(tcb);

		return;
	    }
	    
	    /*
	     * Send an ACK of the SYN and change our state.  Process any data.
	     */
	    status = tcp_sendsynsegment(tcb, tcp_selectiss(), TRUE,
					tcb->rcvnxt);
	    if (status != OK) {
		/* 
		 * The SYN+ACK packet wasn't sent.
		 */
		if (tcb->flags & TCB_GENTCBS) {
		    /* 
		     * Blow away the tcb if no one knows about it yet.
		     */
		    tcp_async_cleanup(tcb, tcb->lsterr);
		}
		return;
	    }
	    tcp_changestate(tcb, SYNRCVD);
	    tcp_handleother(tcb, curpkt, FALSE);
	    break;
	}

	/* 
	 * Any other control or text-bearing segment
	 * (not containing Syn) must have an Ack and thus would
	 * be discarded by the Ack processing.  An incoming Rst
	 * segment could not be valid, since it could not have
	 * been sent in response to anything sent by this 
	 * incarnation of the connection.  So it is unlikely to
	 * get to here.  Drop the segment and return.
	 */
	tcp_discardsegment(curpkt->pakptr, "Should never have gotten here!");
    }

    case SYNSENT: {
 	discarded_pak = check_rst_n_bogus(tcb, tcp, curpkt, SYNSENT, 
					  &acceptableAck);
	/*
	 * Incoming segment has been discarded already.
	 */ 
	if (discarded_pak)
	    break;
	if (tcp->ack) {
	    if (curpkt->segprc != tcb->prc) {
		tcp_sendcontrolsegment(tcb, tcb->localport, tcb->foreignport,
				       &tcb->foreignhost,
				       tcp->acknowledgementnumber, FALSE, 0,
				       TRUE, FALSE);
		tcp_discardsegment(curpkt->pakptr, "Bad precedence");
		break;
	    }
	} else {
	    if (curpkt->segprc > tcb->prc) {
		if (tcb->flags & TCB_HIGHERPRC)
		    tcb->prc = curpkt->segprc;
		else {
		    tcp_sendcontrolsegment(tcb, tcb->localport,
					   tcb->foreignport,
					   &tcb->foreignhost,
					   0, TRUE,
					   tcp->sequencenumber+curpkt->seglen,
					   TRUE, FALSE);
		    tcp_discardsegment(curpkt->pakptr, "Bad precedence");
		    break;
		}
	    }
	}

	/*
	 * Check the Syn bit.
	 * This step should be reached only if the Ack is ok, or there
	 * is no Ack, and the segment did not contain a Rst.
	 */

	if (tcp->syn) {
	    tcb->irs = tcp->sequencenumber;
	    tcp->sequencenumber++;
	    /* Adjust for sequence number of syn bit. */
	    tcb->brs = tcp->sequencenumber;
	    tcb->rcvnxt = tcp->sequencenumber;
	    if (acceptableAck) {
		tcb->snduna = tcp->acknowledgementnumber;
		tcb->sndwnd = tcp->window;
		tcb->sndwl1 = tcp->sequencenumber;
		tcb->sndwl2 = tcp->acknowledgementnumber;
	    }
	    /*
	     * Process an incoming security options.  All we are doing here
	     * is locking our SECINSERT down to a specific level.  Anything
	     * else is ignored.
	     */
	    curpkt_ip = (iphdrtype *)ipheadstart(curpkt->pakptr);

	    if (ip_secopt_bytes || curpkt_ip->ihl > btol(MINIPHEADERBYTES)) {
		if (!ipopt_checktcb(&tcb->options, curpkt->pakptr)) {
		    /*
		     * Oh Bother!  couldn't deal with the options
		     * for some reason hopefully the lower layers
		     * have told somebody about it.  Send a reset
		     * to the other side and drop the packet.
		     */
		    if (tcb->options) {
			free(tcb->options);
			tcb->options = NULL;
		    }
		    tcp_replytoconnectionlesspacket(curpkt->pakptr);
		    tcp_changestate(tcb, CLOSED);
		    tcb->lsterr = ABORTED;
		    break;
		}
	    }

	    if (tcp_seqinrange(tcb->iss + 1, tcb->snduna, tcb->sndnxt)) {
		tcp_changestate(tcb, ESTAB);	/* Syn has been acked. */
#if 0
		if (modem_debug)
		    tcp_putbyte(tcb, 0, FALSE);
		else
#endif
		tcp_sendacksegment(tcb);
		tcp_handleother(tcb, curpkt, TRUE);

		/*
		 * If this was a generated TCB, tell the upper layer
		 * that there's a connection here (except if this was
		 * a synchronized-listen TCB in which case the upper
		 * layer was notified earlier).
		 */
		if ((tcb->flags & TCB_GENTCBS) &&
		    !(tcb->flags & TCB_SYNCLISTEN))
		    tcp_newconn(tcb);
		break;
	    } else {
		tcp_changestate(tcb, SYNRCVD);/* Rely on Retransmission to send
					     syn segment out again. */
		tcp_handleother(tcb, curpkt, FALSE);
		break;
	    }
	}

	/*
	 * If neither Syn or Rst then drop the segment.
	 */

	tcp_discardsegment(curpkt->pakptr,
			   "Couldn't process packet in SYNSENT state.");
	break;
    }

    default: {
	boolean seqok;		/* TRUE if good sequence number */
	boolean ackok;		/* TRUE if acceptable ack */
	char *errstr;		/* ptr to discard message string */

	/*
	 * Check sequence number.
	 */
	seqok = FALSE;
	ackok = FALSE;		/* Assume ACK is not acceptable */
	errstr = "bad sequence number";
	if (curpkt->seglen == 0) {
	    if (tcb->rcvwnd == 0) {
		if (tcp->sequencenumber == tcb->rcvnxt)
		    seqok = TRUE;
	    } else {
		if (tcp_seqinrange(tcb->rcvnxt, tcp->sequencenumber,
				   tcb->rcvnxt + tcb->rcvwnd - 1)) {
		    seqok = TRUE;
		} else if (tcp->sequencenumber == tcb->rcvnxt-1 && tcp->rst) {
		    /* Check for a RST of a keepalive packet */
		    seqok = ackok = TRUE;
		}
	    }
	} else {
	    if (tcb->rcvwnd != 0) {
		/*
		 * Acceptable if either the start or the end of the segment is
		 * within the current receive window.
		 */
		if ((tcp_seqinrange(tcb->rcvnxt, tcp->sequencenumber,
				    tcb->rcvnxt + tcb->rcvwnd - 1)) ||
		    (tcp_seqinrange(tcb->rcvnxt,
				    tcp->sequencenumber + curpkt->seglen - 1,
				    tcb->rcvnxt + tcb->rcvwnd - 1))) {
		    seqok = TRUE;
		} else
		    errstr = "outside window";
	    } else {
		errstr = "window closed";
		/*
		 * treat the ack as acceptable if the sequence number is
		 * that of a reasonable zero-window probe.
		 */
		if (tcp->sequencenumber == tcb->rcvnxt)
		    ackok = TRUE;
	    }	
	}

	if (seqok) {		/* valid sequence number ? */
	    ackok = TRUE;	/* Ack always potentially valid if seq ok */
	    /*
	     * Check the Rst bit. 
	     */
	    if (tcp->rst) {
  		enum tcp_state last_state;

		if (tcb->bad_rst_count > TCP_RST_SECURITY_COUNT) {
		    tcp_discardsegment(curpkt->pakptr,
				       "Ignored valid RST due to storm");
		    break;
		}

		if (tcp_debug)
		    buginf("\nTCP%t: RST received, Closing connection",
			   tcb->ttynum);
		last_state = tcb->state;
		tcp_changestate(tcb, CLOSED);
		tcb->lsterr = FOREIGN_RESET;
		retbuffer(curpkt->pakptr);
		tcp_stoptimer(tcb, RETRANSTIMEOUT);

		if (tcb->flags & TCB_APP_CLOSED ||
		    (last_state == SYNRCVD && (tcb->flags & TCB_GENTCBS))) {
		    /*
		     * The app has already closed and forgotten about
		     * this connection, or its a child that never got
		     * established and never got queued so clean up now.
		     */
		    tcp_async_cleanup(tcb, tcb->lsterr);
		    tcb = NULL;
		}
		break;
	    }
	
	    /*
	     * Check the Syn bit.
	     */
	    if (tcp->syn) {
		tcp_sendcontrolsegment(tcb, tcb->localport, tcb->foreignport,
				       &tcb->foreignhost, 0, TRUE,
				       tcp->sequencenumber + curpkt->seglen,
				       TRUE, FALSE);
		tcp_closeconnection(tcb, curpkt, ABORTED,
				    "SYN rcvd while ESTAB");
		break;
	    }
	}

	if (ackok) {		/* process ack sometimes even if bad seq */
	    /*
	     * Check the Ack field.
	     */
	    if (!tcp->ack) {
		tcp_discardsegment(curpkt->pakptr, "No ack in packet header!");
		break;
	    }
	    switch (tcb->state) {
	    case SYNRCVD: 
		if (tcp_seqinrange(tcb->snduna, tcp->acknowledgementnumber,
				   tcb->sndnxt)) {
		    tcp_changestate(tcb, ESTAB);

		    /*
		     * If this was a generated TCB, tell the upper layer
		     * that there's a connection here (except if this was
		     * a synchronized-listen TCB in which case the upper
		     * layer was notified earlier).
		     */
		    if ((tcb->flags & TCB_GENTCBS) &&
			!(tcb->flags & TCB_SYNCLISTEN))
			tcp_newconn(tcb);
		    /* Continue processing in case Estab. */
		} else {
		    tcp_sendcontrolsegment(tcb, tcb->localport,
					   tcb->foreignport, &tcb->foreignhost,
					   tcp->acknowledgementnumber,
					   FALSE, 0, TRUE, FALSE);
		    tcp_discardsegment(curpkt->pakptr, 
				       "Bad ACK number in state SYNRCVD.");
		    goto done_default;
		}

	    case ESTAB:
	    case FINWAIT1:
	    case FINWAIT2:
	    case CLOSEWAIT:
	    case CLOSING:
		if (tcp->acknowledgementnumber == tcb->snduna) {
		    /*
		     * This is a pure ACK and if the window info didn't 
		     * change, it could be a duplicated ACK for a missing 
		     * packet. Reset dupacks if we don't have outstanding
		     * data to be ACKed. 
		     */
		    if (curpkt->seglen == 0 && tcp->window == tcb->sndwnd) {
			if (!TIMER_RUNNING(tcb->timers[RETRANSTIMEOUT]))
				tcb->dupacks = 0;
			/* 
			 * If we have outstanding data (not a window probe),
			 * this is a completely duplicate ACK, the ACK is 
			 * the biggest we've seen and the number of the
			 * same duplicated ACK is TCP_MAXDUPACKS, assume a 
			 * packet has been dropped and retransmit it. 
		         */
			else if (++tcb->dupacks == TCP_MAXDUPACKS) {
			    tcp_fast_retransmit(tcb, tcp);
        		    retbuffer(curpkt->pakptr);
			    goto done_default;
			}
			break;
		    } else {
		    	tcb->dupacks = 0; 
		    	if (tcppkt_debug && tcplineno &&
			    tcplineno == tcb->ttynum &&
			    tcb->sndwnd != tcp->window)
			    buginf("\nTCP%t: old ACK changes window %d to %d",
			           tcb->ttynum, tcb->sndwnd, tcp->window);
		    	tcb->sndwnd = tcp->window;  /* might open window! */
		    }
		} else {
		    tcb->dupacks = 0; 
		    if (tcp_seqinrange(tcb->snduna, tcp->acknowledgementnumber,
				       tcb->sndnxt)) {
			/* new acknowledgement */
			tcb->snduna = tcp->acknowledgementnumber;

			/*
			 * Slow start and congestion avoidance.
			 * A new ack has come in, so try opening
			 * the send window a bit more.  If we are
			 * below the slow start threshold, then
			 * open the window by one packet. Otherwise,
			 * do the congestion avoidance additive increase
			 * to "probe" for more bandwidth.
			 */
			{
			    uint cwnd = tcb->sndcwnd;
			    uint incr, mss = tcb->maxsegmentsize;

			    if (cwnd > tcb->sndssthresh)
				incr = mss * mss / cwnd + mss / 8;
			    else
				incr = mss;
			    tcb->sndcwnd = min(cwnd + incr, MAXTCPWINDOW);
#ifdef TCPVERBOSE
			    if (tcp_debug && cwnd != tcb->sndcwnd)
				buginf("\nTCP%t: congestion window changes"
				       " to %d (+%d)", tcb->ttynum,
				       tcb->sndcwnd, incr);
#endif
			}

			/*
			 * Only change the window based on more recent
			 * information.  We determine this by keeping the
			 * sequence number.  If the previous sequence number
			 * is what we expect and the ack is reasonable then
			 * believe the advertised window.
			 *
			 */
			if (tcp_seqinrange(tcb->sndwl1, tcp->sequencenumber,
					   tcb->rcvnxt)) {
			    tcb->sndwnd = tcp->window;
			    tcb->sndwl1 = tcp->sequencenumber;
			    tcb->sndwl2 = tcp->acknowledgementnumber;
			}
		    } else {
			if (tcp_seqinrange(tcb->snduna - 1000000, /* XXX */
					   tcp->acknowledgementnumber,
					   tcb->snduna)) {
			    /* old ack, do nothing */
			} else {
			    /* acknowledgement for unsent data */
			    tcp_sendacksegment(tcb);
			    tcp_discardsegment(curpkt->pakptr,
					   "Bad ack number in state >= Estab");
			    goto done_default;
			}
		    }
		}

		if (tcb->state == FINWAIT1) {
		    if (tcb->snduna == tcb->sndnxt) {	/* Fin acknowledged */
			tcp_changestate(tcb, FINWAIT2);
			/*
			 * Start a timer so we don't sit in FINWAIT2 forever.
			 */
			tcp_starttimer(tcb, TMEWAITTIMEOUT, LENFINWAIT2TIMEOUT);
		    }
		} else {
		    if (tcb->state != FINWAIT2) {
			if (tcb->state == CLOSING) {
			    if (tcb->snduna == tcb->sndnxt) {
				/* Fin acknowledged */
				tcp_changestate(tcb, TIMEWAIT);
				if (!tcp_timerrunning(tcb, TMEWAITTIMEOUT))
				    tcp_starttimer(tcb, TMEWAITTIMEOUT,
					       tcb->timewait_time);
			    } else {
				tcp_discardsegment(curpkt->pakptr,
					"snduna != sndnxt in state CLOSING");
				goto done_default;
			    }
			}
		    }
		}
		break;

	    case LASTACK:
		if (!tcp_seqinrange(tcb->snduna, tcp->acknowledgementnumber,
				    tcb->sndnxt)) {
		    tcp_sendacksegment(tcb);
		    tcp_discardsegment(curpkt->pakptr,
				       "state LASTACK - no data");
		} else {
		    tcb->snduna = tcp->acknowledgementnumber;
		    if (tcb->snduna == tcb->sndnxt) {
			if (tcp_debug)
			    buginf("\nTCP%t: FIN acked", tcb->ttynum);
			tcp_changestate(tcb, CLOSED);    /* Fin acknowledged */
			if (tcp_timerrunning(tcb, TMEWAITTIMEOUT))
			    tcp_stoptimer(tcb, TMEWAITTIMEOUT);

			/*
			 * Our FIN has been acked.  Since the TCB can
			 * only have gotten to LASTACK state due to a
			 * call to tcp_close(), it is safe to destroy
			 * the TCB here.  Any task referencing a TCB
			 * after closing it deserves to lose.
			 */
			tcp_async_cleanup(tcb, -1);
			tcb = NULL;
                    }
		    retbuffer(curpkt->pakptr);
		}
		goto done_default;

	    case TIMEWAIT:
		if (tcp->fin) {
		    tcp_sendacksegment(tcb);
		    if (!tcp_timerrunning(tcb, TMEWAITTIMEOUT))
		        tcp_starttimer(tcb, TMEWAITTIMEOUT,
				       tcb->timewait_time);
		}
		tcp_discardsegment(curpkt->pakptr,
				   "state TIMEWAIT - no data.");
		goto done_default;

	    default:
		/* do nothing */
		break;
	    }
	} /* if (ackok) */

	if (!seqok) {
	    if (tcb->state == SYNRCVD) {
		if ((tcp->ack) &&  
		    (!tcp_seqinrange(tcb->iss + 1, tcp->acknowledgementnumber,
				     tcb->sndnxt))) {
		    tcp_discardsegment(curpkt->pakptr, 
				       "Bad ACK and seq in SYNRCVD");
		    /*
		     * ACK is on, but ACKed number is totally out of our 
		     * sequence space, send RST to remote if the incoming 
		     * packet didn't already  have RST. 
		     */
		    if (!tcp->rst) 
			tcp_sendcontrolsegment(tcb, tcb->localport,
					       tcb->foreignport, 
					       &tcb->foreignhost,
					       tcp->acknowledgementnumber,
					       FALSE, 0, TRUE, FALSE);
		    break;
		} 
	    }

	    if (!tcp->rst) {
		tcp_sendacksegment(tcb);
	    }

	    if (tcp->rst || tcp->fin) {
		GET_TIMESTAMP(tcb->bad_rst_timestamp);
		tcb->bad_rst_count++;
	    }

	    tcp_discardsegment(curpkt->pakptr, errstr);
	    break;
	}

	/*
	 * Check the Urg bit. 
	 */

	if (tcp->urg) {
	    switch (tcb->state) {
	    case ESTAB: 
	    case FINWAIT1: 
	    case FINWAIT2: 
		tcp_process_urg(tcb, curpkt, tcp);
		break;

	    default: 
		/*
		 * This should not occur since a FIN has been received from
		 * the remote side.  Ignore the URG.
		 */
		break;
	    }
 	    if (tcp_debug)
		buginf("\nTCP%t: urgent data: rcvnxt=%lu, rcvup=%lu, data=%d",
		       tcb->ttynum, tcb->rcvnxt, tcb->rcvup, curpkt->seglen);
	}

	/*
	 * Process the segment text.
	 */
	switch (tcb->state) {
	case ESTAB: 
	    tcp_qsegment(tcb, curpkt, tcp);
	    break;
	case FINWAIT1: 
	case FINWAIT2: 
	    /*
	     * Reset if we receive data after user had already closed
	     * the connection.
	     */
	    if (curpkt->seglen > 0  && (tcb->flags & TCB_APP_CLOSED)) {
		tcp_discardsegment(curpkt->pakptr, "Application closed");
		tcp_sendcontrolsegment(tcb, tcb->localport, tcb->foreignport, 
				       &tcb->foreignhost,
				       tcp->acknowledgementnumber,
				       FALSE, 0, TRUE, FALSE);
		/* 
		 * Application has closed, reset has been sent to remote,
		 * connection no need to hang around till FINWAIT2 timer expired.
		 */
		tcp_async_cleanup(tcb, -1);
		tcb = NULL;
		return;
	    }
	    tcp_qsegment(tcb, curpkt, tcp);
	    break;
	default:
	    retbuffer(curpkt->pakptr);
	    break;
	}
done_default:
	break;
    }
    }

    if (tcb) {		/* TCB may have gone away in above processing */
	tcp_removeackedsendsegments(tcb);
	tcb->pakincount++;
	if (old_usable == 0)
	    tcp_check_window(tcb);
    }

    return;
}

/*
 * tcp_quench
 * Handle an ICMP quench message 
 */

void tcp_quench (void *ipopaque, ipaddrtype source)
{
    iphdrtype *ip;
    tcptype *tcp;
    tcbtype *tcb;	/* Let's hear it for easily-confused variable names. */
    addrtype laddr, faddr;
    int index;
    ushort lport, fport;

    ip = ipopaque;
    tcp = (tcptype *) ipdatastart_iph(ip);
    lport = tcp->sourceport;
    fport = tcp->destinationport;
    laddr.type = faddr.type = ADDR_IP;
    laddr.ip_addr = source;
    faddr.ip_addr = ip->dstadr;

    if (tcp_debug)
	buginf("\nTCP: source quench, sport %d dport %d saddr %i syn %d",
	       lport, fport, source, tcp->syn);

    /*
     * Congestion avoidance.
     * A gateway is complaining about one of our connections.
     * Find it and reduce its congestion window to one packet.
     */
    index = tcbhash(lport, fport, laddr);
    for (tcb = tcbhashtable[index]; tcb; tcb = tcb->next) {
	if (tcb->foreignhost.ip_addr != faddr.ip_addr ||
	    /* inp->inp_socket == 0 || */
	    (lport && tcb->localport != lport) ||
	    (laddr.ip_addr && tcb->localhost.ip_addr != laddr.ip_addr) ||
	    (fport && tcb->foreignport != fport)) {
	    continue;
	}
	tcb->sndcwnd = tcb->maxsegmentsize;	/* Close congestion window */
	if (tcp_debug)
	    buginf("\nTCP%t: quench closes congestion window to %d",
		   tcb->ttynum, tcb->sndcwnd);
    }
}

/*
 * tcp_processsavedsegments:
 * Processes any segments in the SaveQueue that can be relative
 * to the current value of rcvnxt.
 */

void tcp_processsavedsegments (tcbtype *tcb)
{
    paktype *packet;
    long diff;
    tcptype *tcp;

    while (!QUEUEEMPTY(&tcb->q[SAVEQUEUE])) {
	packet = (paktype *) tcb->q[SAVEQUEUE].qhead;
	tcp = tcp_get_hdr(packet, tcb);
	if (!tcp_seqinrange(tcb->rcvnxt - tcb->maxrcvwnd, tcp->sequencenumber,
			    tcb->rcvnxt)) {
	    if (tcp_seqinrange(tcb->rcvnxt, tcp->sequencenumber,
			       tcb->rcvnxt + tcb->maxrcvwnd))
	        return;
	    /*
	     * packet was neither overlapping rcvnxt, or in the range of
	     * packets that we should expect to be on the save queue.  It
	     * must be an old packet that is still here for unobvious reasons,
	     * and ought to be thrown away!
	     */
	    packet = dequeue(&tcb->q[SAVEQUEUE]);
	    tcb->qsaved--;
	    retbuffer(packet);
	    continue;

	}
	packet = dequeue(&tcb->q[SAVEQUEUE]);
	tcb->qsaved--;

	/*
	 * The following is the same basic code as tcp_qsegment.
	 * Remember:  unspecified[0] = curpkt->seglen
	 * 	      unspecified[1] = curpkt->segdataoffset
	 *	      diff is the size of prefix to discard	
	 */
	tcp = tcp_get_hdr(packet, tcb);
	diff = (long) (tcb->rcvnxt - tcp->sequencenumber);
	if ((diff > packet->us_seglen) ||
	    ((diff == packet->us_seglen) && !tcp->fin)) {
	    retbuffer(packet); /* already processed this data */
	    continue;
	}
	packet->dataptr += packet->us_segdataoffset + diff;
	packet->length = packet->us_seglen - diff;

	/*
	 * Check for segment larger than rcvwnd.  If a too large segment
	 * contains a FIN, accept it rather than risk losing data.
	 */
	if ((packet->length > tcb->rcvwnd) && !tcp->fin)
	    packet->length = tcb->rcvwnd;	/* trim too long packet */
	tcb->unreadbytes += packet->length;	/* note bytes pending */
	tcb->bytesincount += packet->length;	/* keep total bytes input */
	tcb->pakindata++;			/* count data packets */
	tcp_changercvwnd(tcb, -packet->length);
	tcb->rcvnxt += packet->length;
	if (tcp->fin) 		/* adjust ACK for FIN */
	    tcp_process_fin(tcb, tcp->acknowledgementnumber);
	if (tcp->urg && (tcb->flags & TCB_EDITURG) &&
	    (tcp->urgentpointer <= packet->length)) {
	    tcp_editurg(tcb, tcp, packet, tcp->dataoffset << 2);
	    tcb->unreadbytes--;
	}
	if (packet->length == 0) {
	    retbuffer(packet);
	} else {
	    enqueue(&tcb->q[SEGQUEUE], packet);
	    tcb->qinput++;
	}
	tcb->qinput++;
    }
}

/*
 * tcp_removeackedsendsegments:
 * Removes acked send segments from the retransmission queue.
 *
 * NOTE: only entire segments are removed from the queue.
 */

void tcp_removeackedsendsegments (tcbtype *tcb)
{
    paktype *packet;
    paktype *headptr;
    tcptype *tp;
    int del, len;
    long sendtime;
    sys_timestamp send_timestamp;
    elementtype *element;
    elementtype *nextelement;
    boolean sendnext, removed, nonempty, lots_queued;

    /*
     * Scan retransmit queue until empty or we find a packet we cannot
     * remove.  Note that the retransmit queue is a queue of queueing
     * elements pointing to packet buffers.
     */
    len = 0;
    TIMER_STOP(send_timestamp);
    sendnext = FALSE;
    removed = FALSE;
    nonempty = FALSE;
    element = (elementtype *) tcb->q[RETRANSQUEUE].qhead;
    headptr = element ? element->pakptr : NULL;
    if (element)
	nonempty = TRUE;
    while (element) {
	nextelement = element->next;
	packet = element->pakptr;
	tp = tcp_get_hdr(packet, tcb);
	del = (tp->syn || tp->fin) ? 1 : 0;
	len = packet->length - IPHEADERBYTES(tcb->options) - 
	      (tp->dataoffset << 2);
	/*
	 * the maximum the send window can ever be is 65535, so in theory,
	 * we will never have a packet on the RxQueue that is "older" than
	 * 65536 bytes from the next ack we are expecting.  This will probably
	 * need to change if we ever implement large TCP windows.
	 */
	if (tcp_seqinrange(tcb->snduna - (MAXTCPWINDOW + 1),
			   tp->sequencenumber + len + del, tcb->snduna)) {
	    paktype *pak;

	    pak = pak_unqueue(&tcb->q[RETRANSQUEUE], packet);
	    if (tcb->qoutput >= QUEUEMAX)
		lots_queued = TRUE;
	    else
		lots_queued = FALSE;
	    tcb->qoutput--;
	    if (lots_queued && tcb->qoutput < QUEUEMAX) {
		tcb->events |= TCPEV_RTQUEUE;
		if (tcb->activate)
		    tcb->activate(tcb, tcb->rxhandle);
		tcp_wakeup_process(tcb);
	    }

	    /*
	     * Remember RTT only for a unretransmitted packet at the head of
	     * the queue.  Set flag to send next packet right away if we've
	     * retransmitted that first packet.
	     */
	    if (pak && headptr == packet) { /* Packet at head of queue? */
		if (packet->us_retransno == 0)
		    COPY_TIMESTAMP(packet->us_sendtime, send_timestamp);  /* not retransmitted */
		else
		    sendnext = TRUE;
	    }
	    removed = TRUE;

	    /* Prevent the refcount on the packet from going bananas
	     * by only freeing it if pak_unqueue gave us a good pointer.
	     */
	    if (pak)
		datagram_done(packet);
	} else
	    break;
	element = nextelement;
    }

    /*
     * Fake a window event to kick the TCP Driver awake now that the
     * retransmission queue is empty.  This is necessary because our
     * criteria for sending window probes is screwed up.  tcp_putsetup()
     * will only initiate probes when the retransmission queue is empty.
     * An application sending bytes and then sleeping when the usable
     * window closes won't keep banging away at tcp_putsetup() until
     * all outstanding segments have been acked, so we wake it up now
     * to start the probes.  The only reason probes work for everything
     * else is because tcpputBLOCK is broken and wakes the process
     * (typically looping inside tcp_putbyte()) at every scheduling
     * interval.  The process will try again and again and again,
     * eventually calling tcp_putsetup() with an empty retransmission
     * queue.
     */
    if (nonempty && QUEUEEMPTY(&tcb->q[RETRANSQUEUE]) && tcb->sndwnd == 0) {
	tcb->events |= TCPEV_WINDOW;
	if (tcb->activate)
	    tcb->activate(tcb, tcb->rxhandle);
	tcp_wakeup_process(tcb);
    }

    /*
     * If we were probing a zero window, and the window has now opened,
     * we want to restart any retransmission timeout calculations (reset krtt),
     * as well as essentially restarting the retransmission of the probe
     * packet using timeout rules rather than probe rules.
     * If we are restarting transmission because a 0 window has opened,
     * also pretend that the packet hasn't been retransmitted
     * at all yet.  Use the SNDWNDTIMEOUT instead of RXTIMEOUT, since it
     * is already running anyway.
     */
    if (TIMER_RUNNING(tcb->timers[SNDWNDTIMEOUT]) && tcb->sndwnd != 0) {
	if (tcp_debug)
	    buginf("\nTCP%t: probed window opened, resetting RTT",
		   tcb->ttynum);
	tcp_stoptimer(tcb, SNDWNDTIMEOUT);
	tcb->krtt = 0;
	if (!QUEUEEMPTY(&tcb->q[RETRANSQUEUE])) {
	    packet = (paktype *) (((elementtype *)
				 tcb->q[RETRANSQUEUE].qhead)->pakptr);
	    if (packet) {
		GET_TIMESTAMP(packet->us_sendtime);
		packet->us_retransno = 0;
	    }
	    /*
	     * TCB is expected still being there after returning from
	     * tcp_removeackedsendsegments(). Make sure that the TCB is
	     * not freed in tcp_retransmit() in the case where the
	     * connection is considered dead.
	     */
	    tcp_retransmit(tcb, FALSE);
	}
    }

    /*
     * We only have more to do if the ACK actually removed some data from
     * our output queue.  If all we did was get an ack to a zero window
     * probe (for example) that doesn't actually ACK any data, we are
     * already done!
     */
    if (!removed)
	return;


    /*
     * Recalculate retransmission interval if the ACK was for an
     * unretransmitted packet.  Remember the extremes and limit
     * ourself to a reasonable range.  Clear the krtt so that
     * senders use calculated Round Trip Timeout instead of the
     * exponential backoff value of KRTT (Karn Algorithm).
     */
    if (TIMER_RUNNING(send_timestamp)) {
	sendtime = ELAPSED_TIME(send_timestamp);
	if (sendtime < tcb->minrtt)
	    tcb->minrtt = sendtime;
	if (sendtime < MINRTT)
	    sendtime = MINRTT;
	if (sendtime > tcb->maxrtt)
	   tcb->maxrtt = sendtime;
	if (sendtime > MAXRTT)
	    sendtime = MAXRTT;

	/*
	 * Begin Van Jacobson Round Trip Timeout and RTTO variance estimation
	 */
	sendtime -= (tcb->srtt >> 3);
	tcb->srtt += sendtime;
	if (sendtime < 0)
	    sendtime = -sendtime;
	sendtime -= (tcb->srtv >> 2);
	tcb->srtv += sendtime;
	tcb->rtto = (tcb->srtt >> 2) + (tcb->srtv << 1);
#if 0
	if (tcp_debug)
	    buginf("\nTCP%t: SRTT=%d, RTTO=%d, RTV=%d, KRTT=%d",
		   tcb->ttynum, (tcb->srtt >> 3), tcb->rtto, tcb->srtv,
		   tcb->krtt);
#endif
	tcb->krtt = 0;
	tcb->ackholdtime = (tcb->srtt/2);
	if (tcb->ackholdtime > MINRTT)
	    tcb->ackholdtime = MINRTT;
    }

    /*
     * Stop the retransmit timer if queue has emptied.
     * If we've removed packets that have been retransmitted, restart
     * the timer to send the packet at the current top of queue.
     */
    if (QUEUEEMPTY(&tcb->q[RETRANSQUEUE])) {
	tcp_stoptimer(tcb, RETRANSTIMEOUT);
	tcp_stoptimer(tcb, GIVEUPTIMEOUT);
	tcb->flags &= ~TCB_TIMINGOUT;
	if ((tcb->flags & TCB_OUTPUTPENDING) && !onintstack())
	    tcp_putpush(tcb, TRUE, FALSE);	/* send pending packet... */
    } else {
	/*
	 * If there are additional packets in the retransmit queue that have
	 * already been sent once, we want to restart the retransmit timer
	 * to rx them at the appropriate time, which is its original
	 * transmission time plus the newly calculated rtto.
	 */
	packet = (paktype *) (((elementtype *)
			      tcb->q[RETRANSQUEUE].qhead)->pakptr);
	sendtime = (tcb->krtt ? tcb->krtt : tcb->rtto) -
	    ELAPSED_TIME(packet->us_sendtime);

	if (sendtime < 0)
	    sendtime = 0;

        if (tcppkt_debug && tcplineno && tcplineno == tcb->ttynum)
	    buginf("\nTCP%t: restart retransmission in %d", 
		   tcb->ttynum, sendtime);

	tcp_starttimer(tcb, RETRANSTIMEOUT, sendtime);
	if (tcb->giveuptime)
	     tcp_starttimer(tcb, GIVEUPTIMEOUT, tcb->giveuptime);
    }
}

/*
 * tcp_mss
 *	Set the MSS for a connection.
 */

void tcp_mss (tcbtype *tcb, int size)
{
    ushort segsize;
    idbtype *idb;

    if (size == 0) {		/* Calculating MSS for offer. */
	/*
	 * Determine maximum segment size.  Use 536 default if there is a
	 * gateway between us, unless we have a small IP MTU on the interface.
	 * If local destination, base size on network MTU.
	 * If MSS is larger than the maximum receive window for this proto-
	 * connection, decrease the MSS to be equal to the receive window
	 * size.
	 */


	idb = samecable(tcb->foreignhost.ip_addr);
	if (idb) {
	    /* On the same wire as our destination. */
	    segsize = idb->ip_mtu - IPHEADERBYTES(tcb->options) - 
	    TCPHEADERBYTES - tcp_optionbytes(tcb, FALSE);
	} else {
	    ipaddrtype junk;

	    /*
	     * Going through a router to get to our destination.
	     * Start out with a reasonable first guess for MSS
	     * and enable path MTU discovery.
	     */
	    junk = tcb->foreignhost.ip_addr;
	    idb = reg_invoke_ip_first_hop(&junk, junk, IP_PATH_DEFFLAGS);
	    if (idb)
		segsize = idb->ip_mtu - IPHEADERBYTES(tcb->options) -
			  TCPHEADERBYTES - tcp_optionbytes(tcb, FALSE);
	    else
		segsize = TCP_DEFAULT_MSS;
	    if (tcp_pathmtu)
		tcp_pmtu_on(tcb);
	    else
		segsize = min(segsize, TCP_DEFAULT_MSS);
	}
	if (segsize > tcb->maxrcvwnd)
	    segsize = tcb->maxrcvwnd;
    } else {		/* Adjusting MSS based on offer from other side. */
	/* 
	 * If offer is sane, use the smaller of the two as MSS but
	 * remember the MSS offered by the peer.
	 */
	if (size > (IPHEADERBYTES(tcb->options) + TCPHEADERBYTES)) {
	    segsize = min(tcb->maxsegmentsize, size);
	    tcb->peer_mss = size;
	} else 
	    tcb->peer_mss = segsize = TCP_DEFAULT_MSS;
	if (tcp_debug)
	    buginf("\nTCP%t: Connection to %s:%d, received MSS %d, MSS is %d",
		   tcb->ttynum, address_string(&tcb->foreignhost),
		   tcb->foreignport, size, segsize);
    }
    tcb->maxsegmentsize = tcb->sndcwnd = segsize;
}
