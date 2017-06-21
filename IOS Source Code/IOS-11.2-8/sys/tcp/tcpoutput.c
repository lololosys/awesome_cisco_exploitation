/* $Id: tcpoutput.c,v 3.13.4.7 1996/08/07 09:02:55 snyder Exp $
 * $Source: /release/112/cvs/Xsys/tcp/tcpoutput.c,v $
 *------------------------------------------------------------------
 * tcpoutput.c -- TCP output routines
 *
 * November 1985, Kirk Lougheed
 *
 * Copyright (c) 1985-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: tcpoutput.c,v $
 * Revision 3.13.4.7  1996/08/07  09:02:55  snyder
 * CSCdi65343:  more things that should be declared const
 *              char * [] -> char *const []
 *              char *    -> const char []
 *              savings 17,496 data space, 1,852 image space
 * Branch: California_branch
 *
 * Revision 3.13.4.6  1996/07/23  13:30:02  thille
 * CSCdi63329: Centralize logging for getbuffer() and pak_duplicate()
 * failures
 * Branch: California_branch
 * Shamelessly rip off Mr. Katz' code from CSCdi36037.  Provide
 * centralized logging for getbuffer() and pak_duplicate() failures and
 * then rip out the 9 bazillion different messages that where in the
 * various callers to report the failure.
 *
 * Revision 3.13.4.5  1996/06/28  23:30:20  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.13.4.4  1996/06/18  01:49:17  hampton
 * Split the monolithic traffic array into smaller per-protocol traffic
 * arrays.  [CSCdi59224]
 * Branch: California_branch
 *
 * Revision 3.13.4.3  1996/04/20  20:55:07  lol
 * CSCdi52836:  T+ tcp open timeouts are not accurate enough
 * Branch: California_branch
 *
 * Revision 3.13.4.2  1996/04/10  17:32:38  jenny
 * CSCdi53387:  TCP should not perform repacketization before conn
 * established.
 * Branch: California_branch
 *
 * Revision 3.13.4.1  1996/03/18  22:13:47  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.9.2.3  1996/03/16  07:41:14  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.9.2.2  1996/03/07  10:53:07  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.9.2.1  1996/02/20  21:21:29  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.13  1996/03/08  00:43:13  lol
 * CSCdi50738:  no timeouts in T+
 *         Implement T+ timeouts for a customer who just won't wait. This
 *         code becomes obsolete in California.
 *
 * Revision 3.12  1996/02/07  16:16:07  widmer
 * CSCdi48285:  Remove function name from bad_parser_subcommand
 *
 * Revision 3.11  1996/01/29  07:30:24  hampton
 * Remove unnecessary includes of parser definition files.  [CSCdi47717]
 *
 * Revision 3.10  1996/01/24  20:41:20  jenny
 * CSCdi47046:  TCP/IP need code clean up (part 3)
 * . removed ip/tuba.c, ip/tuba_default.c and ip/tuba.h
 * . removed all includes for ip/tuba.h.
 * . removed more TUBA related code.
 *
 * Revision 3.9  1996/01/22  07:29:22  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.8  1996/01/15  20:47:29  jenny
 * CSCdi46849:  TCP repacketization should coalesce data no more than TCP
 * MSS.
 *
 * Revision 3.7  1996/01/11  02:33:56  jenny
 * CSCdi46513:  TCP/IP need code clean up (part 2)
 * . removed ip_tuba.c and clns_tuba.c.
 * . removed lots of tuba related registry stubs and code.
 *
 * Revision 3.6  1995/12/28  18:29:20  jenny
 * CSCdi46011:  TCP/IP need to do code clean up.
 * This is the first patch of code changes for TCP/IP code cleanup.
 * . removed all references to TUBA_IP and TUBA_CLNS.
 * . renamed open_tuba_socket() to ip_open_socket().
 * . renamed tcp_tuba_open() to tcp_open().
 * . pass addrtype * instead of ipaddrtype for local and foreign addresses
 *   to tcp_open() for future portability.
 *
 * Revision 3.5  1995/12/20  18:55:03  jenny
 * CSCdi13385:  TCP should re-packetize on retransmission
 * Do data coalescent from multiple packets into 1 packet so we may send up
 * to tcb->maxsegmentsize of data during retransmission.
 *
 * Revision 3.4  1995/12/17  06:14:01  mmcneali
 * CSCdi43825:  Cosmetic fix.
 *
 * Revision 3.3  1995/12/13  04:24:46  ahh
 * CSCdi44230:  Remove dead code
 * Bring out your dead... *clang* (RSRB/TCP fast-switching)
 *
 * Revision 3.2  1995/11/21  21:51:30  jenny
 * CSCdi39128:  TCP needs to do fast retransmit
 * Implemted TCP fast retransmit and recovery algorithm. Fixed a bug in
 * TCP where an out-of-sequence FIN packet with no data got dropped during
 * processing.
 *
 * Revision 3.1  1995/11/09  13:36:05  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/08/23  16:31:23  ahh
 * CSCdi39158:  TCP: resets to yourself trigger looping packet detection
 * Clear the PAK_SENDSELF bit in the packet we reuse for sending
 * back the reset.
 *
 * Revision 2.2  1995/07/12  23:45:42  hampton
 * Convert TCP to fully use the passive timers macros instead of
 * referencing the system clock directly.  [CSCdi37015]
 *
 * Revision 2.1  1995/06/17 06:29:11  ahh
 * CSCdi35734:  TCP: Long overdue code reorganization
 * Separate tcp.h into public and private header files.  Shuffle functions
 * around into more logical groupings.  Rename functions to meet naming
 * conventions (say goodbye to StudlyCaps).  Delete unneeded externs.
 * Trim off old log messages.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <ciscolib.h>
#include "interface_private.h"
#include "packet.h"
#include "../ip/ip_registry.h"
#include "../ui/debug.h"
#include "../ip/ip.h"
#include "ttysrv.h"
#include "tcp.h"
#include "tcpinternal.h"
#include "tcpdriver.h"
#include "tcp_debug.h"
#include "config.h"
#include "parser.h"
#include "../parser/parser_defs_tcpip.h"
#include "../ip/ip_media.h"
#include "../ip/ipoptions.h"
#include "../iprouting/route.h"
#include "../clns/clns.h"
#include "../pt/translate.h"
#include "mgd_timers.h"
#include "../util/md5.h"
#include "logger.h"
#include "../tcp/msg_tcp.c"		/* not a typo, see logger.h */

#define TCPTEST
#ifdef TCPTEST
/*
 * Special code for testing the behavior of TCP.  This code get called
 * just before ipwrite, and may dstroy, delete, delay, or otherwise do
 * nasty things to the packet.  Normal behavior has this routine just
 * return the pointer to the packet.  If we dispose of the packet in
 * some other way, return NULL.
 */

static int tcptest_tty;
static int tcptest_delpaks;	/* delete the next N packets */
static int tcptest_delrandom;	/* delete every Nth packet */
static int tcptest_pakcount;	/* counter for above. */
static int tcptest_delaypaks;	/* delay each packet for specified ms */

static int tcptest_delpaks_cnt;	/* delete the next N packets */
static int tcptest_delrandom_cnt;/* delete every Nth packet */

static pid_t tcptest_delay_pid;
static queuetype tcptest_delay_Q;
mgd_timer tcptest_delay_timer;

static process tcptest_delay_proc (void)
{
    paktype *pak;
    iphdrtype *ip;

    while (TRUE) {
	process_sleep_on_timer(&tcptest_delay_timer);
	pak = pak_dequeue(&tcptest_delay_Q);
	ip  = (iphdrtype *) ipheadstart(pak);

	ipwrite(pak, pak->length, TCP_PROT, ip->srcadr, ip->dstadr, NULL,
		NULL, NULL);
	buginf("\ntcp: delayed packet sent");
    }
}

static
paktype *tcptest (tcbtype *tcb, paktype *pak)
{
    iphdrtype *ip;
    tcptype *tp;

    ip = (iphdrtype *) ipheadstart(pak);
    tp = tcp_get_hdr(pak, tcb);

    if (tcptest_delpaks) {
	tcptest_delpaks--;
	tcptest_delpaks_cnt++;
	buginf("\ntcp%t: Packet dropped %d to go", tcb->ttynum,
	       tcptest_delpaks);
	datagram_done(pak);
	return(NULL);
    }
    if (tcptest_delrandom) {
	if (++tcptest_pakcount == tcptest_delrandom) {
	    tcptest_delrandom_cnt++;
	    tcptest_pakcount = 0;
	    buginf("\ntcp%t: Random packet dropped, seq %u", 
		   tcb->ttynum, tp->sequencenumber);
	    datagram_done(pak);
	    return(NULL);
	}
    }
    if (tcptest_delaypaks) {
	if (tcptest_delay_pid == 0) {
	    queue_init(&tcptest_delay_Q, 0);
	    tcptest_delay_pid = process_create(tcptest_delay_proc,
					       "TCPtestPktDelay", NORMAL_STACK,
					       PRIO_NORMAL);
	}

	TIMER_START(pak->inputtime, tcptest_delaypaks);

	/*
	 * Fill in necessary IP info now, since this will be on an
	 * rxqueue too.  Code copied from ipwrite.
	 */
	ip->version = IPVERSION;	/* IP version number */
	ip->tos = ipdefaultparams.tos;		/* type of service */
	ip->tl = pak->length;		/* length of packet */
	ip->id = ipdefaultparams.ident;		/* packet ID */
	ip->fo = 0;			/* no fragment offset */
	ip->ipreserved = 0;		/* reserved field */
	ip->dontfragment = ipdefaultparams.dontfragment; /* may fragment? */
	ip->morefragments = FALSE;	/* no more fragments */
	ip->prot = TCP_PROT;		/* protocol type */
	ip->ihl = 5; /* header length */
	ip->ttl = ipdefaultparams.ttl;              /* time to live */
	ip->srcadr = tcb->localhost.ip_addr; /* source address */
	ip->dstadr = tcb->foreignhost.ip_addr; /* dest address */
	clear_if_input(pak);	/* ensure we are sourcing */
	pak->if_output = NULL;
	if (pak_enqueue(&tcptest_delay_Q, pak) == NULL)
	    datagram_done(pak);		/* oh well */
	if (!mgd_timer_running(&tcptest_delay_timer))
	    mgd_timer_set_soonest(&tcptest_delay_timer, pak->inputtime);
	return(NULL);
    }
    return(pak);
}

void test_tcp (parseinfo *csb)
{
    int n = GETOBJ(int, 1);

    switch (csb->which) {
    case TEST_TCP_RANDOM:
	tcptest_delrandom = n;
	tcptest_pakcount = 0;
	break;
    case TEST_TCP_LINE:
	tcptest_tty = n;
	break;
    case TEST_TCP_DROP:
	tcptest_delpaks = n;
	break;
    case TEST_TCP_DELAY:
	tcptest_delaypaks = n;
	break;
    default:
	bad_parser_subcommand(csb, csb->which);
	break;
    }
}
#else
void test_tcp (parseinfo *csb)
{
}
#endif

/*
 * tcp_sendip
 * Sends designated packet to IP layer.
 *
 * Security raises it's ugly head.  If we are secured then we have to
 * find the real start of the data field.  We assume that all appropriate
 * options are in the option packet including security.  We can not
 * let ipwrite default the insertion.
 *
 * Parameters:
 * tcb     Ptr to tcb of current connection
 * packet  Ptr to packet buffer to send to IP
 * fhost   IP address to send packet to 
 */

void tcp_sendip (tcbtype *tcb, paktype *packet, ipaddrtype fhost)
{
    iphdrtype *ip;
    tcptype *p;	/* ptr to actual TCP packet. */
    uchar ihl;

    ip = (iphdrtype *)ipheadstart(packet);

    if (ip->ihl) {
	ihl = ip->ihl << 2;
	if (tcb->options &&
	    (IPHEADERBYTES(tcb->options) != ihl)) {
	    /*
	     * IP options in generated packet differ from those
	     * constructed in the tcb.  Update our tcb copy so that
	     * ipbuild_head() doesn't set the IP header length
	     * to the wrong value.
	     */
	    ipopt_copytcb(tcb->options, packet);
	}
    } else
	ihl = IPHEADERBYTES(tcb->options);
    p = (tcptype *) (ipheadstart(packet) + ihl);
    p->checksum = 0;
    p->checksum = CheckSum((uchar *)p,
			   (packet->length - ihl),
			   h2nl(tcb->localhost.ip_addr),
			   h2nl(fhost), TCP_PROT);
    if (packet->us_retransno != 0) {
	tcp_traffic.retrans++;
	tcb->pakoutretrans++;		/* retransmitting */
	tcb->param.ident = ip->id;	/* reuse the IP ID field */
	if (tcppkt_debug)
	    print_tcp(tcb, packet, p, 'R', tcb->param.ident);
    } else {
	int bytes = packet->length - (IPHEADERBYTES(tcb->options) +
				      (p->dataoffset << 2));
	if (bytes != 0) {
	    tcb->bytesoutcount += bytes;
	    tcb->pakoutdata++;
	}
        tcb->pakoutcount++;
	tcb->param.ident = tcb->ipcount++;	/* form next IP ID */
	if (tcppkt_debug)
	    print_tcp(tcb, packet, p, 'O', tcb->param.ident);
    }

    /*
     * Clear all packet flags except the ones we still need.
     * Historical note:  before the addition of the buffer_flags
     * field in paktype, this code would save the PAK size bits
     * and the permanent flag (PAK_PERMANENT), but now it doesn't.
     */
    packet->ip_flags &= PAKOPT_SECSTRIP;
    if (ip_aliasaddress(fhost))
	packet->flags |= PAK_ALIAS;
   
    if (tcb->tty)
	tcb->param.log = !(tcb->tty->capabilities & LOGGING);

    packet->if_output = NULL;  /* so we recompute route */
#ifdef TCPTEST
    if (tcb->ttynum == tcptest_tty)
	packet = tcptest(tcb, packet);
    if (packet) {
#endif
    ipwrite(packet, packet->length, TCP_PROT, tcb->localhost.ip_addr, 
	    fhost, NULL, tcb->options, &tcb->param);
#ifdef TCPTEST
    }
#endif
    tcp_stoptimer(tcb, ACKTIMEOUT);
}

/*
 * tcp_repacketize
 * Try to coalesce data from multiple packets into 1 packet so we 
 * may send up to tcb->maxsegmentsize of data during retransmission.
 */ 
static 
paktype *tcp_repacketize (tcbtype *tcb, paktype *packet)
{
    paktype *next_pak;
    tcptype *tp, *next_tp;
    iphdrtype *ip;
    elementtype *element;
    uchar *copy_from, *copy_to;
    int usable_len; 
    int overhead, optbytes;
    int data_len, next_data_len;
    int buffer_size, buffer_taken;
    boolean new_ip_id = FALSE;


    /*
     * Don't do data coalescent if connection is not fully establised. 
     */
    if (tcb->state < ESTAB) 
	return(packet);
	
    /*
     * Don't do data coalescent if the packet doesn't have it's dataptr
     * pointer initialized. This probably means the packet wasn't 
     * allocated via tcp_putsetup().  
     */
    if (packet->dataptr == NULL)
	return(packet);

    /*
     * Stop data repackization if packet reference count is invalid.
     * Complain so that we know there is a problem. 
     */
    if (packet->refcount < 1) {
	errmsg(&msgsym(BADREFCNT, TCP), tcb->ttynum, packet, 
		address_string(&tcb->localhost), tcb->localport, 
		address_string(&tcb->foreignhost), tcb->foreignport, 
		tcb->state);
	return(packet);
    }

    /*
     * Get the first packet in the retransmission queue. If this is the
     * only packet in the queue, don't need to do anything. 
     */
    element = (elementtype *) tcb->q[RETRANSQUEUE].qhead;
    if (element->next == NULL)
	return (packet);
    else {
	/*
	 * Buffer allocated for the packet begins at packet center
	 * (data_area + ENCAPBYTES). dataptr points to one byte beyond 
	 * last user data in the packet. Calculate the buffer space that's 
	 * taken in the packet just in case the network_start does not 
	 * begin right after packet center. 
	 *
	 * buffer_size	- Max size packet to send. This is the minimum of
	 *		  TCP MSS for the connection and number of bytes
	 *		  initially allocated for the packet.
	 * buffer_taken - number of bytes that's already used in the packet.
	 * overhead	- number of bytes for TCP/IP header.
	 * usable_len	- number of bytes available for data copy.
	 * data_len	- number of bytes in raw user data (excluding header).
	 */
	optbytes = tcp_optionbytes(tcb, FALSE);
	overhead = IPHEADERBYTES(tcb->options) + TCPHEADERBYTES + optbytes;
	buffer_size = pak_get_pool_size(packet);
	buffer_size = min(tcb->maxsegmentsize, buffer_size);
	buffer_taken = packet->dataptr - pak_center(packet); 
	if ((usable_len = (buffer_size - buffer_taken)) <= 0)
	    return(packet);
	data_len = packet->length - overhead; 
	tp = tcp_get_hdr(packet, tcb);
	ip = (iphdrtype *)ipheadstart(packet);
	element = element->next;	/* skip the first packet */
    }

    while (element) {
	next_pak = element->pakptr;

	/*
	 * Stop data repackization if dataptr is not initialized in
	 * the next packet. 
	 */
	if (next_pak->dataptr == NULL)
	    break;

	/*
	 * Stop data repackization if packet reference count is invalid.
	 * Complain so that we know there is a problem. 
	 */
	if (next_pak->refcount < 1) {
	    errmsg(&msgsym(BADREFCNT, TCP), tcb->ttynum, next_pak,
		    address_string(&tcb->localhost), tcb->localport, 
		    address_string(&tcb->foreignhost), tcb->foreignport, 
		    tcb->state);
	    break;
	}

	next_data_len = next_pak->length - overhead; 
	/*
	 * Copy data from next packet to the first packet if we have 
	 * enough room for the entire packet. 
	 */
	if (usable_len >= next_data_len) {
	    element = element->next;
	    next_pak = pak_unqueue(&tcb->q[RETRANSQUEUE], next_pak);
    	    next_tp = tcp_get_hdr(next_pak, tcb);
	    /*
	     * Pick up any flags in TCP header that may be set in the
	     * packet we are copying from.
	     */
    	    tp->urg |= next_tp->urg;
    	    tp->ack |= next_tp->ack;
    	    tp->psh |= next_tp->psh;
    	    tp->rst |= next_tp->rst;
    	    tp->fin |= next_tp->fin;
    	    copy_from = (uchar *)((iphdrtype *)ipheadstart(next_pak))+ overhead;
    	    copy_to = packet->dataptr;
    	    bcopy(copy_from, copy_to, next_data_len);
	    /*
	     * ajust the pointers in the first packet to reflect new
	     * data added to it. 
	     */
	    if (next_tp->urg)
		tp->urgentpointer = h2nl(next_tp->urgentpointer + data_len);
    	    packet->dataptr += next_data_len;
    	    packet->length += next_data_len;
    	    packet->datagramsize += next_data_len;
	    usable_len -= next_data_len;
	    data_len += next_data_len;
	    /*
	     * Make sure to decrement the packet count in retransmission
	     * queue and free the packet that we've already copied data 
	     * from.
	     */
	    tcb->qoutput--;
	    datagram_done(next_pak);
	    new_ip_id = TRUE;
	} else 
	    break; 
    }
    /* 
     * Use a new IP ID for the repacketized packet.
     */
    if (new_ip_id) {
	ip->id = tcb->ipcount++;
	if (tcp_debug) {
	    buginf("\nTCP%t: Data repacketized, seq %u, sent %d byte", 
		    tcb->ttynum, tp->sequencenumber, 
		    (packet->length - overhead));
	}
    }
    return (packet);
}

/*
 * tcp_retransmit
 * Do the delicate retransmission dance.
 * Return FALSE if the connection has been timed out, TRUE otherwise.
 */
boolean tcp_retransmit (tcbtype *tcb, boolean ok_to_free)
{
    paktype *packet;
    tcptype *tp;
    long retries, elapsedticks;
    long maxtime;
    char *type;
    iphdrtype *ip;

    /*
     * We examine only the first packet in the retransmit queue.
     * Set up pointers, counts, and elapsed times.
     */
    packet = (paktype *) (((elementtype *)tcb->q[RETRANSQUEUE].qhead)->pakptr);
    if (!packet)
	return(TRUE);
	
    packet = tcp_repacketize(tcb, packet);
    tp  = tcp_get_hdr(packet, tcb);
    ip = (iphdrtype *)ipheadstart(packet);

    retries = ++packet->us_retransno;
    elapsedticks = ELAPSED_TIME(packet->us_sendtime);

    /*
     * If we've been trying to send this datagram for a long time without
     * any response, we assume the remote side has died and declare
     * a timeout.  Timeout SYN packets with tcp_synwait value.  For all other
     * packets we use 10 times the maximum krtt value, limited below
     * by seven minutes and above by fifteen minutes.
     */
    if (tp->syn) {
	maxtime = ONESEC * tcb->tcb_synwait;
    } else {
	maxtime = 10 * ((tcb->srtt >> 3) << MAXTMOSHIFT);
	if (maxtime < (7*ONEMIN))
	    maxtime = 7*ONEMIN;
	else if (maxtime > (15*ONEMIN))
	    maxtime = 15*ONEMIN;
    }

    /*
     * If we haven't had a response lately, flush the redirect cache and
     * any possibly invalid AR cache entries.  We may be able to salvage
     * this connection by finding a new gateway or by discovering that the
     * other side has changed its hardware address since we last talked.
     */
    if (((elapsedticks >= maxtime/2) || (elapsedticks >= ONEMIN)) &&
	(tcb->flags & TCB_TIMINGOUT) == 0) {
	ip_redirect_flush_cache(NULL);
	ip_arp_invalidate(ip->dstadr);
	tcb->flags |= TCB_TIMINGOUT;
    }

    /*
     * If no response for too long, timeout the connection unless
     * we have disabled timeouts or the send window is zero.
     */
    if ((elapsedticks > maxtime) && (tcb->flags & TCB_RTRANSTMO) &&
	(tp->syn != FALSE || tcb->sndwnd != 0)) {
	enum tcp_state last_state;

	last_state = tcb->state;	/* Save the state */
	tcp_deallocatetcb(tcb, TIMEDOUT);

	/*
	 * Don't try to free the TCB unless the caller allowes us to.
	 * If the TCB can't be freed now, it will be freed later either
	 * when the timer expires again or application does an explicit
	 * close on the connection.
	 */
	if (ok_to_free) {
	    if (((tcb->flags & TCB_GENTCBS) && last_state < ESTAB) ||
		tcb->flags & TCB_APP_CLOSED) {
		/*
		 * If this was a generated TCB and hasn't reached 
		 * established state yet, or it has been closed already 
		 * (we're timing out a FIN to a dead host) clean it up
		 * here and now.
		 */
		tcp_async_cleanup(tcb, -1);
	    }
	}
	return(FALSE);
    }

    /*
     * We want to resend the datagram.  Update the window and ACK fields.
     * Indicate another retransmission and reset the timer.  We do exponential
     * backoff for the retransmissions.
     */
    if (tp->syn == FALSE) {
	tp->window = h2n(tcb->rcvwnd);
	tp->acknowledgementnumber = h2nl(tcb->rcvnxt);
	tp->ack = TRUE;
	if (tcb->flags & TCB_MD5) {
	    uchar *md5;
	    int hdrbytes = IPHEADERBYTES(tcb->options); 
	    int optbytes = (tp->dataoffset << 2) - TCPHEADERBYTES;

	    /* Need to rewrite signature */
	    md5 = tcp_findoption(tp, MD5OPTION);
	    if (md5) {
		tp->checksum = 0;
		tcp_md5digest(&md5[2], tcb->localhost.ip_addr,
			      tcb->foreignhost.ip_addr,
			      TCP_PROT, packet->length - hdrbytes,
			      tp, &tp->data[optbytes],
			      tcb->md5_key, tcb->md5_keylen);
	    }
	}
    }
    tcb->krtt = (tcb->rtto << (min(retries, MAXTMOSHIFT)));
    if (TIMER_RUNNING(tcb->timers[SNDWNDTIMEOUT])) {
	type = "probe";
	maxtime = min(tcb->krtt, LENSNDWNDTIMEOUT);
	tcp_starttimer(tcb, SNDWNDTIMEOUT, maxtime);
    } else {
	type = "timeout";
	maxtime = tcb->krtt;
	tcp_starttimer(tcb, RETRANSTIMEOUT, maxtime);
    }
    
    /*
     * Do not send the initial pak, call pak_duplicate to make a copy. Lower
     * layers (X.25, LAPB) may still be holding onto the datagram and have
     * sequenced encapsulation information on the front of the datagram.
     */
    packet = pak_duplicate(packet);
    if (!packet)
	return(TRUE);

    /*
     * Congestion avoidance.
     * There was a timeout, so set the slow-start threshold to half
     * the current window size (but at least 2 packets) and set the
     * congestion window to 1 packet.  When new acks come back,
     * slow-start will open the window up to the slow-start threshold
     * and after that the window will open in small chunks.
     */
    {
	unsigned int thresh = min(tcb->sndwnd, tcb->sndcwnd) / 2;
#ifdef TCPVERBOSE
	ushort cwnd = tcb->sndcwnd;
	ushort ssthresh = tcb->sndssthresh;
#endif

	if (thresh / tcb->maxsegmentsize < 2)
	    thresh = 2 * tcb->maxsegmentsize;
	tcb->sndcwnd = tcb->maxsegmentsize;
	tcb->sndssthresh = thresh;
#ifdef TCPVERBOSE
	if (tcp_debug && (cwnd != tcb->sndcwnd ||
			  ssthresh != tcb->sndssthresh))
	    buginf("\nTCP%t: congestion window changes"
		   " to %d, threshold set to %d", tcb->ttynum, tcb->sndcwnd,
		   tcb->sndssthresh);
#endif
    }

    /*
     * Complain if the retransmitted packet has refcount of zero, so that 
     * we know there is a problem.
     */
    
    if (packet->refcount < 1) {
	errmsg(&msgsym(BADREFCNT, TCP), tcb->ttynum, packet,
		address_string(&tcb->localhost), tcb->localport, 
		address_string(&tcb->foreignhost), tcb->foreignport, 
		tcb->state);
    }

    tcp_sendip(tcb, packet, tcb->foreignhost.ip_addr);
    if (tcp_debug || (tcppkt_debug && tcplineno && tcplineno == tcb->ttynum)) {
	if ((retries >= 3) || tcppkt_debug) {
	    buginf("\nTCP%t: %s #%d - timeout is %d ms, seq %lu",
		   tcb->ttynum, type, retries, maxtime, tp->sequencenumber);
	    if (tcb->tty == NULL)
		buginf("\nTCP: (%d) -> %s(%d)", tcb->localport,
		       address_string(&tcb->foreignhost), 
		       tcb->foreignport);
	}
    }
    return(TRUE);
}

/*
 * tcp_fast_retransmit
 * Fast retransmit a missing packet so that our send window can be 
 * opened up again.
 */ 
void tcp_fast_retransmit (tcbtype *tcb, tcptype *tcp)
{
    paktype *send_pak;
    tcptype *send_tcp;
    ulong onxt;
    u_int thresh;

    /*
     * Get the first packet in the retransmit queue.
     */
    send_pak = (paktype *)(((elementtype *)tcb->q[RETRANSQUEUE].qhead)->pakptr);
    if (! send_pak) { 
        if (tcp_debug || tcppkt_debug)
            buginf("\nTCP%t: no packet in rexmit Q, lport %d, fport %d", 
		   tcb->ttynum, tcb->localport, tcb->foreignport);
        return; 
    }

    /*
     * Do not send the initial pak, call pak_duplicate to make a copy.
     */
    send_pak = pak_duplicate(send_pak);
    if (! send_pak) {
        return; 
    }

    /*
     * Kludge sndnxt & the congestion window so we send only this one
     * packet. Save current sndnxt as onxt and set sndnxt to the missing
     * packet sequence number. Set the slow-start threshold to half the 
     * current window size (but at least 2 packets). If this packet fills 
     * the only hole in the receiver's sequence space, the next real ack 
     * will fully open our window. 
     */
    onxt = tcb->sndnxt;
    tcb->sndnxt = tcp->acknowledgementnumber;
    thresh = min(tcb->sndwnd, tcb->sndcwnd) / 2; 
    if (thresh / tcb->maxsegmentsize < 2)
	thresh = 2 * tcb->maxsegmentsize;
    tcb->sndssthresh = thresh;
    tcb->sndcwnd = tcb->maxsegmentsize;

    /*
     * Resend the missing packet. Update the window and ACK fields.
     */
    send_tcp  = tcp_get_hdr(send_pak, tcb);
    send_tcp->window = h2n(tcb->rcvwnd);
    send_tcp->acknowledgementnumber = h2nl(tcb->rcvnxt);
    send_tcp->ack = TRUE;
    tcp_stoptimer(tcb, RETRANSTIMEOUT);

    if (tcb->flags & TCB_MD5) {
        uchar *md5;
        int hdrbytes = IPHEADERBYTES(tcb->options); 
        int optbytes = (send_tcp->dataoffset << 2) - TCPHEADERBYTES;
        /* 
         * Need to rewrite signature 
         */
        md5 = tcp_findoption(send_tcp, MD5OPTION);
        if (md5) {
            send_tcp->checksum = 0;
            tcp_md5digest(&md5[2], tcb->localhost.ip_addr,
            		  tcb->foreignhost.ip_addr, TCP_PROT, 
            		  send_pak->length - hdrbytes, send_tcp, 
            		  &send_tcp->data[optbytes], tcb->md5_key, 
            		  tcb->md5_keylen);
        }
    }

    tcp_sendip(tcb, send_pak, tcb->foreignhost.ip_addr);

    tcp_starttimer(tcb, RETRANSTIMEOUT, tcb->krtt ? tcb->krtt : tcb->rtto);

    /*
     * Set the congestion window to the slow start thresh times number
     * of out-of-sequence packets the peer has ACKed and set sndnxt back
     * to where it was prior to the retransmission of missing the packet.
     */
    tcb->sndcwnd = tcb->sndssthresh + tcb->maxsegmentsize * tcb->dupacks;
    if (GT(onxt, tcb->sndnxt))
        tcb->sndnxt = onxt;

    if (tcp_debug || tcppkt_debug)
        buginf("\nTCP%t: Fast retransmitted one packet, seq %u",
               tcb->ttynum, send_tcp->sequencenumber);
}

/*
 * tcp_initpacketbuffer:
 * Initialize packet buffer.
 * Intended for use with outgoing Tcp packets.
 *
 * Assumes that doffset is a multiple of 4.
 * Note: the urg and urgentpointer fields of the packet are set at
 * actual send time.
 *
 * If the connection has been declared idle, advertise a zero window.
 * "Shrinking" the window shouldn't confuse the foreign TCP too much,
 * since we will continue to accept the datagrams in transit.
 */

static const char finrstsyn_str[] = "\nTCP: sending %s, seq %u, ack %u";

static
void tcp_initpacketbuffer (
    ushort lPort, ushort fPort,	/* Local and foreign ports. */
    int tcplen,			/* Length of Tcp packet. */
    int doffset,		/* Offset in bytes of data field. */
    paktype *packet,		/* Packet buffer to init. */
    ulong seqNum, ulong ackNum,	/* Sequence number and ack number resp. */
    int ack,			/* Equivalent packet bit fields. */
    int psh,
    int rst,
    int syn,
    int fin,
    ulong rcvwnd,		/* Receive window size. */
    boolean rcvidle,		/* TRUE if connection is idle */
    ipoptiontype *options,	/* IP Level options */
    tcbtype *tcb)               /* If NULL, replying to connectionless packet*/
{
    tcptype *ptr;		/* Ptr to Tcp packet to init. */
    iphdrtype *ip;

    ip = (iphdrtype *)ipheadstart(packet);

    packet->length = tcplen;	/* packet length (TCP portion only) */
    if (tcb)
	ip->ihl = 0;
    ptr = tcp_get_hdr(packet, tcb);
    ptr->sourceport = h2n(lPort);
    ptr->destinationport = h2n(fPort);
    ptr->sequencenumber = h2nl(seqNum);    
    ptr->acknowledgementnumber = h2nl(ackNum);
    ptr->dataoffset = doffset >> 2;
#ifdef M_I86	/* microsoft C can't have bitfields crossing byte boundries */
    ptr->reserved1 = 0;
    ptr->reserved2 = 0;
#else
    ptr->reserved = 0;
#endif
    ptr->urg = 0;
    ptr->ack = ack;
    ptr->psh = psh;
    ptr->rst = rst;
    ptr->syn = syn;
    ptr->fin = fin;
    rcvwnd = TCP_TRUNC_WINDOW(rcvwnd);
#ifdef ZEROWINDOW
    ptr->window = rcvidle ? 0 : h2n(rcvwnd);
#else
    ptr->window = h2n(rcvwnd);
#endif
    ptr->checksum = 0;
    ptr->urgentpointer = 0;
    if (tcp_debug) {
	if (fin)
	    buginf(finrstsyn_str, "FIN", seqNum, ackNum);
	if (rst)
	    buginf(finrstsyn_str, "RST", seqNum, ackNum);
	if (syn)
	    buginf(finrstsyn_str, "SYN", seqNum, ackNum);
    }
}

/*
 * tcp_inithdr
 * Initialize the tcp headers of a packet to their default values based on
 * the tcb.  Various field in the tcb are arranged so that they can be bcopied
 * into the packet.  This routine replaces tcp_initpacketbuffer, most of the
 * time.  At the moment, this assumes network byte order = host byte order.
 */
void tcp_inithdr (tcbtype *tcb, tcptype *tcp)
{
    tcb->hrcvwnd = TCP_TRUNC_WINDOW(tcb->rcvwnd);
    bcopy(&tcb->localport, tcp, TCPHEADERBYTES); /* set most of the header */
}

/*
 * tcp_replytoconnectionlesspacket:
 * Replies to a packet sent to a non-existent TCP connection (i.e. a
 * Closed connection).  This routine is reusing the packet buffer
 * that came in.  We force the system to think that it is a fresh
 * buffer, but we try to save and use any options that might have
 * been in the original packet.
 *
 * Way back when the company was very small, we thought we had lost some IP
 * routers (the only kind then) that were the first to be shipped to Finland.
 * This was somewhat alarming, since in those days the usual fate of stuff
 * lost on its way to Finland was to end up in the USSR.  Routers were a
 * controlled technology then and the military boys took a dim view of the
 * commies having them.
 *  
 * Well, the routers did turn up, but in case we ever had routers go missing
 * again, Len and I installed the port 1999 hack.  There are two parts.
 * The first was to have the no such connection code check for TCP port 1999.
 * It would then fill in the data portion of the RST packet with the five
 * byte string "cisco" and send the datagram back to the originator.
 * The other part was to have our telnet code remove and print out the data
 * portion of any RST packet it received.
 *  
 * The idea was to be able to conclusively identify a router as being a cisco
 * router.  No version information was ever included since this would be a
 * very blatant security hole (e.g. oh, it's version 6.2, you need to do this
 * to break in).  We have never documented or advertised this functionality.
 * The function has been in the code since Release 6 or so.
 *  
 *  Kirk
 */

void tcp_replytoconnectionlesspacket (paktype *packet)
{
    iphdrtype *ip = (iphdrtype *)ipheadstart(packet);
    tcptype *p;
    ipaddrtype srcadr = n2hl(ip->srcadr);
    ipaddrtype dstadr = n2hl(ip->dstadr);
    int seglen, len, tcphdrlen, olen, otcphdrlen;
    ipoptiontype *ipopts = NULL;
    addrtype local_addr, remote_addr;
    boolean try_md5;

    /*
     * This packet is being reused, so we don't care if it has
     * already gone through ip_sendself() to get here.
     */
    packet->flags &= ~PAK_SENDSELF;

    if (packet->linktype == LINK_IP) {
	p = TcpPkt(packet);

	/*
	 * Don't send RST's in response to RST's or for datagrams not aimed
	 * at one of our addresses.
	 */
	if ((p->rst == TRUE) || !ip_ouraddress(dstadr)) {
	    retbuffer(packet);
	    return;
	}

	/*
	 * Make an attempt to use any options on the packet.  If it doesn't
	 * work, don't worry.  (Be happy.)  We were about to reset the
	 * connection anyway.  The (first) IP Security Kludge will cover for us.
	 */
	ipopts = malloc(sizeof(ipoptiontype));
	if (ipopts)
	    if (!ipopt_copytcb(ipopts,packet)) {
		/*
		 * Oops, we can't copy the options.  Well, we were going to
		 * send a RST anyway, so don't worry.  Security will be 
		 * taken care of by the automagic SECINSERT.
		 */
		free(ipopts);
		ipopts = NULL;
	    }
    } else {
	if (tcp_debug)
	    buginf("\nTCP: invalid link type %d", packet->linktype);
	retbuffer(packet);	
	return;
    }

    /*
     * If the incoming packet has an MD5 option, try to send
     * a reset containing the right MD5 signature so that the
     * remote side will take the reset and clean itself up.
     */
    try_md5 = (tcp_findoption(p, MD5OPTION) != NULL);

    otcphdrlen = olen = len = tcphdrlen = TCPHEADERBYTES;
    if (try_md5) {
	/*
	 * Set aside space for the options (being optimistic)
	 */
	tcphdrlen += (MD5OPT_BYTES + 3) & ~3;
	len += (MD5OPT_BYTES + 3) & ~3;
    }
    if (p->ack) {
	tcp_initpacketbuffer(p->destinationport, p->sourceport,
			     tcphdrlen, tcphdrlen, packet,
			     p->acknowledgementnumber, 0, FALSE,
			     FALSE, TRUE, FALSE, FALSE, 0, FALSE, ipopts,
			     NULL);
    } else {
	seglen = packet->length - (p->dataoffset << 2);
	if (p->syn) {		/* a SYN takes up a segment slot */
	    seglen += 1;
	    if (p->destinationport == TCP_ID_PORT) {
		len += 5;
		bcopy("cisco", (uchar *) p + TCPHEADERBYTES, 5);
	    }
	}
	tcp_initpacketbuffer(p->destinationport, p->sourceport,
			     tcphdrlen, tcphdrlen, packet,
			     p->acknowledgementnumber,
			     p->sequencenumber + seglen, TRUE,
			     FALSE, TRUE, FALSE, FALSE, 0, FALSE, ipopts,
			     NULL);
    }

    if (try_md5) {
	local_addr.type = remote_addr.type = ADDR_IP;
	local_addr.ip_addr = dstadr;
	remote_addr.ip_addr = srcadr;
	if (!tcp_md5_connectionless_write(&remote_addr, &local_addr, p, p, len,
					  ipopts)) {
	    /*
	     * Can't write the option, so reset the length fields.
	     */
	    tcphdrlen = otcphdrlen;
	    len = olen;
	    p->dataoffset = tcphdrlen >> 2;
	}
    }

    tcp_traffic.reset++;
    p->checksum = 0;
    if (packet->linktype == LINK_IP) {
	packet->length = len + IPHEADERBYTES(ipopts);
	ip->tl = packet->length;
	p->checksum = CheckSum((uchar *) p, len, h2nl(dstadr), h2nl(srcadr),
			       TCP_PROT);
	packet->flags &= ~PAK_ALIAS; /* Clear aliasness of original pak */
	ipwrite(packet, ip->tl, TCP_PROT, dstadr, srcadr, NULL,
		ipopts, &ipdefaultparams);

	if (tcp_debug)
	    buginf("\nTCP: sent RST to %i:%d from %i:%d",
		   srcadr, p->destinationport, dstadr, p->sourceport);
	if (ipopts)
	    free(ipopts);
    }
}

/*
 * tcp_sendsynsegment:
 * Issue a Syn segment to IP.
 *
 * Currently no data is sent with this segment.
 */

int tcp_sendsynsegment (
    tcbtype *tcb,		/* Ptr to tcb of current connection. */
    ulong iss,			/* Initial sequence number. */
    boolean ackFlag,		/* Indicates if should send an Ack. */
    ulong ack)			/* Ack number to send. */
{
    paktype *packet;
    tcptype *p;
    int bytes;
    int hdrbytes, optbytes;

    tcp_mss(tcb, 0);

    /*
     * Allocate and initialize the buffer.
     */
    optbytes = tcp_optionbytes(tcb, TRUE);
    hdrbytes = IPHEADERBYTES(tcb->options); 
    bytes = hdrbytes + TCPHEADERBYTES + optbytes;
    packet = getbuffer(bytes);
    if (packet == NULL) {
	tcp_changestate(tcb, CLOSED);
	tcb->lsterr = NO_MEMORY;
	return(NO_MEMORY);
    }
    packet->linktype = LINK_IP;
    tcp_initpacketbuffer(tcb->localport, tcb->foreignport,
			 TCPHEADERBYTES + optbytes,
			 TCPHEADERBYTES + optbytes, packet, iss, ack,
			 ackFlag, FALSE, FALSE, TRUE, FALSE,
			 tcb->rcvwnd, FALSE, tcb->options, tcb);

    p = (tcptype *) (packet->network_start + hdrbytes);

    tcp_startoptions(tcb, &p->data[0]);
    tcp_writeoption(tcb, MAXSEGSIZEOPTION, (uchar *) &tcb->maxsegmentsize, 2);
    if (tcb->flags & TCB_MD5) {
	uchar md5[MD5_LEN];

	tcp_md5digest(md5, tcb->localhost.ip_addr, tcb->foreignhost.ip_addr,
		      TCP_PROT, bytes - hdrbytes,
		      p, NULL, tcb->md5_key, tcb->md5_keylen);
	tcp_writeoption(tcb, MD5OPTION, md5, sizeof(md5));
    }
    tcp_endoptions(tcb);

    if (tcp_debug)
	buginf("\nTCP%t: Connection to %s:%d, advertising MSS %d", tcb->ttynum,
	       address_string(&tcb->foreignhost), tcb->foreignport,
	       tcb->maxsegmentsize);

    /*
     * Put packet on retransmit queue, then send it for the first time.
     */
    tcb->snduna = tcb->iss = iss;
    tcb->sndmax = tcb->sndnxt = iss + 1;
    while (!pak_enqueue(&tcb->q[RETRANSQUEUE], packet))
	edisms((blockproc *) elementBLOCK, 0);
    tcp_retrans_sanity_check(tcb, packet);
    packet->length = bytes;
    packet->us_retransno = 0;		/* set retransmission number */
    GET_TIMESTAMP(packet->us_sendtime);	/* time the packet was sent */
    pak_lock(packet);			/* Put a lock on the packet */
    tcp_sendip(tcb, packet, tcb->foreignhost.ip_addr);
    tcb->qoutput++;
    tcp_starttimer(tcb, RETRANSTIMEOUT, (tcb->krtt ? tcb->krtt : tcb->rtto));
    if (tcb->giveuptime)
	tcp_starttimer(tcb, GIVEUPTIMEOUT, tcb->giveuptime);
    return(OK);
}

/*
 * tcp_sendcontrolsegment:
 * Sends a TCP control packet to IP/CLNS.
 * Control packet here means no data is sent.
 */

void tcp_sendcontrolsegment (
    tcbtype *tcb,		/* Ptr to tcb of current connection. */
    ushort sPort,		/* source port. */
    ushort fPort,		/* destination port. */
    addrtype *fHost,		/* destination host. */
    ulong seq, 			/* Sequence number to use. */
    int ackFlag,		/* Indicates whether to send an ack. */
    ulong ack,			/* Ack number to use. */
    int rst,			/* Value of rst bit field in packet. */
    int fin)			/* Value of fin bit field in packet. */
{
    paktype *packet;
    int bytes;
    tcptype *tcp;
    int optbytes, hdrbytes;

    optbytes = tcp_optionbytes(tcb, FALSE);
    hdrbytes = IPHEADERBYTES(tcb->options); 
    bytes = hdrbytes + TCPHEADERBYTES + optbytes;
    packet = getbuffer(bytes);
    if (packet == NULL)
	return;
    packet->linktype = LINK_IP;
    tcp_initpacketbuffer(sPort, fPort, TCPHEADERBYTES + optbytes,
			 TCPHEADERBYTES + optbytes,
			 packet, seq, ack, ackFlag, FALSE, rst, FALSE,
			 fin, tcb->rcvwnd, (tcb->flags & TCB_IDLEUSER),
			 tcb->options, tcb);

    tcp = (tcptype *) (packet->network_start + hdrbytes);

    tcp_startoptions(tcb, &tcp->data[0]);
    if (tcb->flags & TCB_MD5) {
	uchar md5[MD5_LEN];

	tcp_md5digest(md5, tcb->localhost.ip_addr, tcb->foreignhost.ip_addr,
		      TCP_PROT, bytes - hdrbytes,
		      tcp, NULL, tcb->md5_key, tcb->md5_keylen);
	tcp_writeoption(tcb, MD5OPTION, md5, sizeof(md5));
    }
    tcp_endoptions(tcb);

    if (rst)
        tcp_traffic.reset++;
    packet->length = bytes;
    packet->us_retransno = 0;
    GET_TIMESTAMP(packet->us_sendtime);
    tcp_sendip(tcb, packet, tcb->foreignhost.ip_addr);
}

/*
 * tcp_sendacksegment:
 * Send an Ack segment to IP/CLNS.  We do this a lot, so it should be quick.
 */

void tcp_sendacksegment (tcbtype *tcb)
{
    paktype *packet;
    int bytes;
    tcptype *tcp;
    iphdrtype *ip;
    int hdrbytes, optbytes;

    optbytes = tcp_optionbytes(tcb, FALSE);
    hdrbytes = IPHEADERBYTES(tcb->options); 
    bytes = hdrbytes + TCPHEADERBYTES + optbytes;
    packet = getbuffer(bytes);
    if (packet == NULL)
	return;
    ip = (iphdrtype *)ipheadstart(packet);
    ip->ihl = 0;
    tcp = (tcptype *)(packet->network_start + hdrbytes);
    tcp_inithdr(tcb, tcp);
    tcp->dataoffset = ((tcp->dataoffset << 2) + optbytes) >> 2;

    /*
     * If send window has shrunk to zero, pull sequencenumber to the
     * edge of closed window (snduna) to avoid potential ACK war with
     * the peer.
     */
    if (tcb->sndwnd == 0)
	tcp->sequencenumber = tcb->snduna;

    tcp_startoptions(tcb, &tcp->data[0]);
    if (tcb->flags & TCB_MD5) {
	uchar md5[MD5_LEN];

	tcp_md5digest(md5, tcb->localhost.ip_addr, tcb->foreignhost.ip_addr,
		      TCP_PROT, bytes - hdrbytes,
		      tcp, NULL, tcb->md5_key, tcb->md5_keylen);
	tcp_writeoption(tcb, MD5OPTION, md5, sizeof(md5));
    }
    tcp_endoptions(tcb);

    packet->linktype = LINK_IP;
    packet->length = bytes;
    packet->us_retransno = 0;
    GET_TIMESTAMP(packet->us_sendtime);
    tcp_sendip(tcb, packet, tcb->foreignhost.ip_addr);
}

/*
 * tcp_sendfin
 * Send a (possibly empty) packet with the FIN bit set.
 */

void tcp_sendfin (tcbtype *tcb)
{
    paktype *pak;
    iphdrtype *ip;

    (void) tcp_putpush(tcb, FALSE, FALSE);	/* Finish current segment */

    if (tcb->curoutputpak == NULL) {
	int overhead;

	overhead = IPHEADERBYTES(tcb->options) + TCPHEADERBYTES +
	   	   tcp_optionbytes(tcb, FALSE);
	while ((pak = getbuffer(overhead)) == NULL) {
	    process_sleep_for(ONESEC);
	}
	tcb->curoutputpak = pak;
	ip = (iphdrtype *)ipheadstart(pak);
	ip->ihl = 0;
        pak->us_seglen = 0;
	pak->length = 0;
	pak->dataptr = pak->network_start + overhead;
	pak->linktype = LINK_IP;
    }
    (void) tcp_putpush(tcb, TRUE, TRUE);
    if (tcp_debug)
	buginf("\nTCP%t: sending FIN", tcb->ttynum);
}
