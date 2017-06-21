/* $Id: tcpdriver.c,v 3.13 1996/02/29 01:25:03 ahh Exp $
 * $Source: /release/111/cvs/Xsys/tcp/tcpdriver.c,v $
 *------------------------------------------------------------------
 * tcpdriver.c -- support for making TCP/IP an encapsulation
 *
 * June 1988, Greg Satz
 *
 * Copyright (c) 1989-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: tcpdriver.c,v $
 * Revision 3.13  1996/02/29  01:25:03  ahh
 * CSCdi50306:  TCP: TCP Driver stack size too small
 * Some applications' close callback routines are a bit, er, extensive.
 *
 * Revision 3.12  1996/02/08  18:02:13  ahh
 * CSCdi48380:  TCP: TCP Driver close callback needs more context
 * Have some t with your cookie.
 *
 * Revision 3.11  1996/01/24  20:41:12  jenny
 * CSCdi47046:  TCP/IP need code clean up (part 3)
 * . removed ip/tuba.c, ip/tuba_default.c and ip/tuba.h
 * . removed all includes for ip/tuba.h.
 * . removed more TUBA related code.
 *
 * Revision 3.10  1996/01/22  21:49:42  jenny
 * CSCdi45992:  XOT data is delayed in router
 * Block interrupt before checking if output queue is empty and don't
 * remove the encaps from activation list if it's not.
 *
 * Revision 3.9  1996/01/22  07:29:17  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.8  1996/01/16  03:38:00  ahh
 * CSCdi46921:  TCP: remove unused second parameter to tcp_close
 *
 * Revision 3.7  1996/01/11  02:33:52  jenny
 * CSCdi46513:  TCP/IP need code clean up (part 2)
 * . removed ip_tuba.c and clns_tuba.c.
 * . removed lots of tuba related registry stubs and code.
 *
 * Revision 3.6  1996/01/05  10:18:21  hampton
 * Move/rename the files containing the deprecated timer callback
 * routines.  [CSCdi46482]
 *
 * Revision 3.5  1995/12/28  18:29:14  jenny
 * CSCdi46011:  TCP/IP need to do code clean up.
 * This is the first patch of code changes for TCP/IP code cleanup.
 * . removed all references to TUBA_IP and TUBA_CLNS.
 * . renamed open_tuba_socket() to ip_open_socket().
 * . renamed tcp_tuba_open() to tcp_open().
 * . pass addrtype * instead of ipaddrtype for local and foreign addresses
 *   to tcp_open() for future portability.
 *
 * Revision 3.4  1995/12/13  04:24:39  ahh
 * CSCdi44230:  Remove dead code
 * Bring out your dead... *clang* (RSRB/TCP fast-switching)
 *
 * Revision 3.3  1995/11/21  08:27:27  ahh
 * CSCdi42413:  XOT: cannot tune TCP keepalive behavior
 * Allow keepalive rate to be controlled for XOT connections.
 *
 * Revision 3.2  1995/11/17  18:48:51  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:35:53  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/11/08  21:32:07  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.2  1995/06/17  06:26:35  ahh
 * CSCdi35734:  TCP: Long overdue code reorganization
 * Separate tcp.h into public and private header files.  Shuffle functions
 * around into more logical groupings.  Rename functions to meet naming
 * conventions (say goodbye to StudlyCaps).  Delete unneeded externs.
 * Trim off old log messages.
 *
 * Revision 2.1  1995/06/07  23:05:56  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "interface_private.h"
#include "packet.h"
#include "../ip/ip.h"
#include "../ip/ip_registry.h"
#include "tcp.h"
#include "tcpinternal.h"
#include "tcpdriver.h"
#include "../ui/service.h"
#include "../os/old_timer_callbacks.h"

#define TCPDRIVER_MAXSEG	1450	/* stupid hack */
#define TCPDRIVER_STACK_SIZE	1500	/* some app callbacks are pigs */

extern boolean tcpdrv_debug, tcpdrvpkt_debug;

static queuetype tcpdriverQ;		/* Master list */
static tcp_encaps *tcpdriver_activate_hd, *tcpdriver_activate_tl;
static int tcpdriver_pid;
static boolean tcpdriver_running;

/* Add a tcp_encaps to the end of the activation list.
 * This routine assumes that protection against interrupts
 * is taken care of by the caller.
 */
static void tcpdriver_addtail (tcp_encaps *item)
{
    item->activate_next = NULL;
    item->activate_prev = tcpdriver_activate_tl;
    if (tcpdriver_activate_tl)
	tcpdriver_activate_tl->activate_next = item;
    tcpdriver_activate_tl = item;
    if (tcpdriver_activate_hd == NULL)
	tcpdriver_activate_hd = item;
}

/* Remove a tcp_encaps from wherever it is on the activation list.
 * This routine assumes that protection against interrupts
 * is taken care of by the caller.
 */
static void tcpdriver_del (tcp_encaps *item)
{
    if (item->activate_prev)
	item->activate_prev->activate_next = item->activate_next;
    else
	tcpdriver_activate_hd = item->activate_next;

    if (item->activate_next)
	item->activate_next->activate_prev = item->activate_prev;
    else
	tcpdriver_activate_tl = item->activate_prev;

    item->activate_next = item->activate_prev = NULL;
}

/*
 * tcpdriverBLOCK
 * Block waiting for something to happen for the TCP encapsulation.
 */

static boolean tcpdriverBLOCK (void)
{
    tcp_encaps *t, *next;
    boolean removed_some = FALSE;
    leveltype status;

    /* unblock when any:
     *     encaps becomes dead
     *     encaps had data to send
     *     tcb moves out of ESTABLISHED state
     *     tcb has been closed
     *     a send window has opened
     *     a full retransmission queue isn't full anymore
     * One of these has happened if the structure is on the list.
     */
    status = raise_interrupt_level(NETS_DISABLE);
    for (t = tcpdriver_activate_hd; t; t = next) {
	next = t->activate_next;
	/* Remove from list. */
	tcpdriver_del(t);
	t->flags &= ~TDF_ACTIVATED;
	removed_some = TRUE;
    }
    reset_interrupt_level(status);

    return(removed_some == FALSE);
}

/* Called when something interesting happens at the TCP level */
static void tcpdriver_activate (tcbtype *tcb, tcp_encaps *t)
{
    leveltype status = raise_interrupt_level(NETS_DISABLE);
    
    /*
     * If one of the TCP-level events we care about has occurred,
     * enqueue the tcp_encaps on the activation list if it's not
     * already there.  Otherwise, check to see if any TCP driver
     * events are set -- if not, this tcp_encaps doesn't belong
     * on the activation list, so remove it if it's there.
     */
    if (t->tcb->events & TCP_ACTIVATIONS) {
	if (!(t->flags & TDF_ACTIVATED)) {
	    /* Add to tail of list. */
	    tcpdriver_addtail(t);
	    t->flags |= TDF_ACTIVATED;
	}
    } else if (!(t->events & TD_ACTIVATIONS)) {
	if (t->flags & TDF_ACTIVATED) {
	    tcpdriver_del(t);
	    t->flags &= ~TDF_ACTIVATED;
	}
    }
    reset_interrupt_level(status);
}

/* Called when something interesting happens at the TCP Driver level */
static void tcpdriver_activate_internal (tcp_encaps *t)
{
    /*
     * If one of the TCP Driver-level events we care about has
     * occurred, enqueue the tcp_encaps on the activation list
     * if it's not already there.  Otherwise, check to see if any
     * TCP events are set -- if not, this tcp_encaps doesn't belong
     * on the activation list, so remove it if it's there.
     */
    if (t->events & TD_ACTIVATIONS) {
	if (!(t->flags & TDF_ACTIVATED)) {
	    /* Add to tail of list. */
	    tcpdriver_addtail(t);
	    t->flags |= TDF_ACTIVATED;
	}
    } else if (!(t->tcb->events & TCP_ACTIVATIONS)) {
	if (t->flags & TDF_ACTIVATED) {
	    tcpdriver_del(t);
	    t->flags &= ~TDF_ACTIVATED;
	}
    }
}

static void tcpdriver_setevent(tcp_encaps *t, tcp_encaps_events event)
{
    leveltype status = raise_interrupt_level(NETS_DISABLE);

    t->events |= event;
    tcpdriver_activate_internal(t);
    reset_interrupt_level(status);
}

static void tcpdriver_clearevent(tcp_encaps *t, tcp_encaps_events event)
{
    leveltype status = raise_interrupt_level(NETS_DISABLE);

    t->events &= ~event;
    tcpdriver_activate_internal(t);
    reset_interrupt_level(status);
}

/*
 * Routine called when the deep-six timer goes off.
 */

static void tcpdriver_killme_callback (tcp_encaps *t)
{
    if (tcpdrv_debug)
	buginf("\nTCPDRV%x: forced awake", t);

    tcpdriver_setevent(t, TDEV_KILL_ME);
}

/*
 * tcpdriver_input
 * Scan data from TCP connection until "enough" is present to hand to user
 * Called by tcp_driver thread and directly from TCP code (via rxnotify).
 */

static void tcpdriver_input (tcbtype *tcb, tcp_encaps *t)
{
    int bytes;

    bytes = tcp_unread(tcb);
    while (bytes >= t->inthresh && t->state != TCPD_ABORT) {
	if (tcpdrvpkt_debug)
	    buginf("\nTCPDRV%x: readf %d bytes (Thresh %d)", t,
 		   tcp_unread(t->tcb), t->inthresh);
	(*t->readf)(t, tcb, t->inthresh, t->cookie);
	bytes = tcp_unread(tcb);
    }
}

/*
 * tcpdriver_active_open
 * Create and open a new TCP encapsulated stream on demand.
 */

static tcp_encaps *tcpdriver_active_open (
    ipaddrtype ipaddr,
    ipaddrtype sipaddr,
    ushort port,
    ulong window,
    tcp_encaps *tmp,
    int *status)
{
    tcp_encaps *t;
    addrtype faddr;
    addrtype laddr;


    if (tcpdriver_pid == 0)
	return(NULL);
    t = malloc(sizeof(tcp_encaps));
    if (t == NULL) {
	*status = NO_MEMORY;
	return(NULL);
    }
    t->timer = create_timer(tcpdriver_killme_callback, t, NULL, NULL,
			    "TCP Driver", TMT_ONCE);
    if (t->timer == NULL) {
	free(t);
	*status = NO_MEMORY;
	return(NULL);
    }

    if (tcpdrv_debug)
	buginf("\nTCPDRV%x: Active open %i:0 --> %i:%d",
	       t, sipaddr, ipaddr, port);

    faddr.type = laddr.type = ADDR_IP;
    faddr.ip_addr = ipaddr;
    laddr.ip_addr = sipaddr;
    t->tcb = tcp_open(NULL, port, &faddr, 0, (sipaddr) ? &laddr : NULL, 
		      status, NULL);
    if (t->tcb == NULL) {
	destroy_timer(t->timer);
	free(t);
	return(NULL);
    }
    if (tcpdrv_debug)
	buginf(" OK, lport %d", t->tcb->localport);

    tcp_set_rcvwnd(t->tcb, window);
    if (t->tcb->maxsegmentsize < TCPDRIVER_MAXSEG)
	t->tcb->maxsegmentsize = TCPDRIVER_MAXSEG;
    t->tcb->flags &= ~TCB_RTRANSTMO;	/* no retransmission timeout */
    if (tcp_keepalivesout)
	t->tcb->flags |= TCB_KEEPALIVE;
    t->tcb->rxnotify = (tcp_notify_cb) tcpdriver_input;
    t->tcb->rxhandle = t;
    t->tcb->activate = (tcp_activate_cb) tcpdriver_activate;
    t->tcb->timewait_time = ONESEC;
    queue_init(&t->outputq, 0);
    t->state = TCPD_ALIVE;
    t->events &= ~TDEV_NOT_ALIVE;
    t->readf = tmp->readf;
    t->closef = tmp->closef;
    t->flushf = tmp->flushf;
    t->flow_ctrl = tmp->flow_ctrl;
    t->hiwater_lodamp = 0;
    t->hiwater_hidamp = 0;
    t->lowater_lodamp = 0;
    t->lowater_hidamp = 0;
    t->cong_level = TCP_CONG_LOW;
    t->inthresh = tmp->inthresh;
    t->cookie = tmp->cookie;
    t->flags |= tmp->flags;
    enqueue(&tcpdriverQ, t);
    return(t);
}

/*
 * tcpdriver_passive_open
 * Upon TCP open, create a new TCP encapsulated stream.
 */

static tcp_encaps *tcpdriver_passive_open (
    tcbtype *tcb,
    ulong window,
    tcp_encaps *tmp)
{
    tcp_encaps *t;

    if (tcpdriver_pid == 0)
	return(NULL);
    t = malloc(sizeof(tcp_encaps));
    if (t == NULL)
	return(NULL);
    t->timer = create_timer(tcpdriver_killme_callback, t, NULL, NULL,
			    "TCP Driver", TMT_ONCE);
    if (t->timer == NULL) {
	free(t);
	return(NULL);
    }

    if (tcpdrv_debug)
	buginf("\nTCPDRV%x: Passive open %s:%d <-- %s:%d",
	       t, address_string(&tcb->localhost), tcb->localport,
	       address_string(&tcb->foreignhost), tcb->foreignport);

    queue_init(&t->outputq, 0);
    tcb->tty = NULL;			/* no console */
    tcp_set_rcvwnd(tcb, window);
    if (tcb->maxsegmentsize < TCPDRIVER_MAXSEG)
	tcb->maxsegmentsize = TCPDRIVER_MAXSEG;
    tcb->flags &= ~TCB_RTRANSTMO;	/* no retransmission timeout */
    if (tcp_keepalivesin)
	tcb->flags |= TCB_KEEPALIVE;
    tcb->rxnotify = (tcp_notify_cb) tcpdriver_input;
    tcb->rxhandle = t;
    tcb->activate = (tcp_activate_cb) tcpdriver_activate;
    tcb->timewait_time = ONESEC;
    t->tcb = tcb;
    t->state = TCPD_ALIVE;
    t->events &= ~TDEV_NOT_ALIVE;
    t->events |= TDEV_NEW_CONN;
    tcpdriver_activate_internal(t);
    t->readf = tmp->readf;
    t->closef = tmp->closef;
    t->flushf = tmp->flushf;
    t->flow_ctrl = tmp->flow_ctrl;
    t->hiwater_lodamp = 0;
    t->hiwater_hidamp = 0;
    t->lowater_lodamp = 0;
    t->lowater_hidamp = 0;
    t->cong_level = TCP_CONG_LOW;
    t->inthresh = tmp->inthresh;
    t->cookie = tmp->cookie;
    t->flags |= tmp->flags;
    enqueue(&tcpdriverQ, t);
    return(t);
}

/*
 * tcpdriver_change_timeout
 * turn on or off the retransmission timeout.
 */

static void tcpdriver_change_timeout (tcp_encaps *t, boolean retrans)
{
    if (!t)
	return;
    if (tcpdrv_debug)
	buginf("\nTCPDRV%x: %s tcp timeouts", t,
	       retrans ? "enable" : "disable");
    if (retrans)
	t->tcb->flags |= TCB_RTRANSTMO;
    else
	t->tcb->flags &= ~TCB_RTRANSTMO;
}

/*
 * tcpdriver_close_internal
 * Guts of tcpdriver_close functionality.
 */

static void tcpdriver_close_internal (tcp_encaps *t)
{
	if (tcpdrv_debug) {
	    tcbtype *tcb = t->tcb;
	    if (tcb)
		buginf("\nTCPDRV%x: %s:%d --> %s:%d Closed",
		       t, address_string(&tcb->localhost), tcb->localport,
		       address_string(&tcb->foreignhost),
		       tcb->foreignport);
	    else
		buginf("\nTCPDRV%x: (no tcb) Closed", t);
	}	
	t->state = TCPD_CLOSE;
	tcpdriver_setevent(t, TDEV_NOT_ALIVE);
	TIMER_START(t->closetime, ONESEC * 30);
}

/*
 * tcpdriver_close
 * Mark this entry as closing.  Final cleanup will happen later.
 */

static void tcpdriver_close (tcp_encaps *t)
{
    if (t) {
	tcpdriver_close_internal(t);
	/*
	 * Application is closing the connection, so don't call it back
	 * if the close is delayed for any reason.
	 */
	t->cookie = NULL;
	t->tcb->rxnotify = NULL;
    }
}

/*
 * tcpdriver_abort
 * Remember that this entry is aborted.
 */

static void tcpdriver_abort (tcp_encaps *t)
{
    if (t) {
	if (tcpdrv_debug) {
	    tcbtype *tcb = t->tcb;
	    if (tcb)
		buginf("\nTCPDRV%x: %s:%d --> %s:%d Abort",
		       t, address_string(&tcb->localhost), tcb->localport,
		       address_string(&tcb->foreignhost),
		       tcb->foreignport);
	    else
		buginf("\nTCPDRV%x: (no tcb) Abort", t);
	}	
	t->state = TCPD_ABORT;
	tcpdriver_setevent(t, TDEV_NOT_ALIVE);
    }
}

/*
 * tcpdriver_doclose
 * Close and perform all cleanups from TCP
 */

static void tcpdriver_doclose (tcp_encaps *t)
{
    paktype *pak;

    if (tcpdrv_debug) {
	if (t->tcb) {
	    tcbtype *tcb = t->tcb;
	    buginf("\nTCPDRV%x: %s:%d --> %s:%d DoClose", t,
		   address_string(&tcb->localhost), tcb->localport,
		   address_string(&tcb->foreignhost),
		   tcb->foreignport);
	} else
	    buginf("\nTCPDRV%x: (no tcb) DoClose", t);
    }	
    if (t->cookie) {
	if (tcpdrv_debug)
	    buginf(" (Cookie %x)", t->cookie);
	(*t->closef)(t, t->cookie);
    }
    if (tcpdrv_debug)
	buginf(" tcp %s", t->state == TCPD_ABORT ? "abort" : "close");
    if (t->state == TCPD_ABORT)
	tcp_abort(t->tcb);
    else
	tcp_close(t->tcb);
    while ((pak = p_dequeue(&t->outputq))) {
	pak->tcpdriver_flags &= ~TCPD_PAK_INPIPE;
	if (tcpdrv_debug)
	    buginf("\nTCPDRV%x: doclose: Queued Packet %x (released)", t, pak);
	datagram_done(pak);
    }

    if (t->flags & TDF_ACTIVATED) {
	leveltype status = raise_interrupt_level(NETS_DISABLE);
       
	tcpdriver_del(t);
	t->flags &= ~TDF_ACTIVATED;
	reset_interrupt_level(status);
    }

    if (t->tcb) {
        t->tcb->rxhandle = NULL;
        t->tcb->rxnotify = NULL;
	t->tcb->activate = NULL;
    }
    unqueue(&tcpdriverQ, t);
    if (t->timer) {
	stop_timer(t->timer);
	destroy_timer(t->timer);
    }
    free(t);
}

/*
 * tcpdriver_update
 * Update values in the TCP driver structure
 */

static void tcpdriver_update (
    tcp_encaps *t,
    tcpdriver_readf readf,
    int thresh,
    void *cookie)
{
    if (!t)
	return;
    if (readf != NULL)
	t->readf = readf;
    if (thresh != 0)
	t->inthresh = thresh;
    t->cookie = cookie;
}

/*
 * tcpdriver_unacked_output
 *
 * Return TRUE if the TCP stream attached to this driver has unacknowedged
 * output.
 */
static boolean tcpdriver_unacked_output (tcp_encaps *t)
{
    if (!t || !t->tcb)
	return(FALSE);
    return(t->tcb->snduna != t->tcb->sndnxt);
}

static void tcpdriver_do_flow_ctrl (tcp_encaps *t)
{
    ulong new_level;
    ulong depth;

    depth = QUEUESIZE(&t->outputq);
    if (depth < t->lowater_lodamp)
        new_level = TCP_CONG_LOW;
    else if (depth < t->lowater_hidamp)
        new_level = t->cong_level;
    else if (depth < t->hiwater_lodamp)
        new_level = TCP_CONG_MEDIUM;
    else if (depth < t->hiwater_hidamp)
        new_level = t->cong_level;
    else
        new_level = TCP_CONG_HIGH;

    if (new_level != t->cong_level) {
        if (new_level > t->cong_level)
            t->cong_level = new_level | TCP_CONG_INCREASE;
        else
            t->cong_level = new_level;
        (*t->flow_ctrl)(t, t->cong_level, t->cookie);
    }
}

/*
 * tcpdriver_output
 * Perform output onto the network. This is a separate routine so
 * it can be called from interrupt level and process level.  We cannot
 * do this until TCP is callable from interrupt level.
 */

static void tcpdriver_output (tcp_encaps *t)
{
    paktype *pak;
    int len;
    int start_qsize;
    leveltype status;

    /*
     * Write to the network given an output packet
     */
    start_qsize = QUEUESIZE(&t->outputq);
    while (!QUEUEEMPTY(&t->outputq)) {
	pak = (paktype *) t->outputq.qhead;

	if (tcpdrvpkt_debug)
	    buginf("\nTCPDRV%x: output pak %x (len %d) inpipe: %c", t, pak,
		   pak->datagramsize,
		   (pak->tcpdriver_flags & TCPD_PAK_INPIPE) ? 'y' : 'n');

	while (pak->datagramsize > 0) {
	    len = tcp_putstring(t->tcb, pak->datagramstart,
				pak->datagramsize);
	    if (len == 0) {
		/*
		 * Only reactivate if tcp_putstring() failed for any other
		 * reason besides the usable window closing or too many
		 * unacked segments.  When the window opens or the
		 * retransmission queue shrinks, a TCP event will
		 * kick this guy awake.
		 */
		if (tcp_usable_window(t->tcb) &&
			t->tcb->qoutput < DEF_QUEUEMAX_SYS)
		    tcpdriver_setevent(t, TDEV_OUT_PEND);
		goto blocked;
	    }
	    if (tcpdrvpkt_debug)
		buginf(" (%d)", len);
	    pak->datagramstart += len;
	    pak->datagramsize -= len;
	}
        pak = p_dequeue(&t->outputq);
	pak->tcpdriver_flags &= ~TCPD_PAK_INPIPE;
	datagram_done(pak);
    }
    if ((t->flow_ctrl) && (start_qsize != QUEUESIZE(&t->outputq)))
        tcpdriver_do_flow_ctrl(t);

    /*
     * Interrupt could've put more packets in the queue, remove the
     * encaps from the activation list only if the queue is empty. 
     */ 
    status = raise_interrupt_level(NETS_DISABLE);
    if (QUEUEEMPTY(&t->outputq))
    	tcpdriver_clearevent(t, TDEV_OUT_PEND);
    reset_interrupt_level(status);

blocked:
    if (t->tcb->curoutputpak)
	tcp_putpush(t->tcb, TRUE, FALSE);
}

/*
 * tcpdriver_oqlen(t)
 * Return queue length of this driver's output queue
 */

static ulong tcpdriver_oqlen (tcp_encaps *t)
{
    return(t ? QUEUESIZE(&t->outputq) : 0);
}

/*
 * tcpdriver_set_oqmax(t, len)
 * set the maximum output queue length for a TCP output queue
 * 0 indicates no limit
 */

static void tcpdriver_set_oqmax (tcp_encaps *t, ulong len)
{
    if (t)
	t->oqmaxlen = len;
}

/*
 * tcpdriver_get_oqmax (t)
 * get the maximum output queue length for a TCP output queue
 */

static inline ulong tcpdriver_get_oqmax (tcp_encaps *t)
{
    return(t ? t->oqmaxlen : (ulong) -1);
}

/*
 * tcpdriver_set_flow_ctrl_levels(t, hiwater, lowater)
 * set the high and low water marks for output queue flow control indications
 */

static void tcpdriver_set_flow_ctrl_levels (tcp_encaps *t,
                                            ulong hiwater, ulong lowater)
{
    if (t) {
        t->hiwater_lodamp = hiwater - TCP_DAMP_CONG;
        t->hiwater_hidamp = hiwater + TCP_DAMP_CONG;;
        t->lowater_lodamp = lowater - TCP_DAMP_CONG;
        t->lowater_hidamp = lowater + TCP_DAMP_CONG;;
    }
}

/*
 * tcpdriver_send
 * Perform output to a TCP stream. Returns 0 on success, 1 on
 * queue full, and -1 on connection not alive.
 * Can be called from interrupt level (unfortunately).
 * If the caller wanted us to prime the IP fast-switching cache,
 * then pak->if_input must be valid.
 */

static int tcpdriver_send (tcp_encaps *t, paktype *pak, boolean force)
{
    if (!t)
	return(TCPDSEND_CLOSED);

    /*
     * Check for opportunities to skip this packet.
     */
    if (t->state != TCPD_ALIVE) {
	if (tcpdrv_debug)
	    buginf("\nTCPDRV%x: send %x (len %d) failed, tcp not alive", t,
		   pak, pak->datagramsize);
	return(TCPDSEND_CLOSED);
    }

    if (t->oqmaxlen && !force && (QUEUESIZE(&t->outputq) >= t->oqmaxlen)) {
	if (tcpdrv_debug)
	    buginf("\nTCPDRV%x: send %x (len %d) failed, queue full",
		   t, pak, pak->datagramsize);
	return(TCPDSEND_QFULL);
    }

    if (pak->tcpdriver_flags & TCPD_PAK_INPIPE) {
	if (tcpdrv_debug)
	    buginf("\nTCPDRV%x: send %x (len %d) failed, already in pipe",
		   t, pak, pak->datagramsize);
	return(TCPDSEND_QFULL);
    }

    /*
     * Guess we have to queue it after all.
     */
    pak->tcpdriver_flags |= TCPD_PAK_INPIPE;
    p_enqueue(&t->outputq, pak);
    tcpdriver_setevent(t, TDEV_OUT_PEND);
    if (tcpdrvpkt_debug)
	buginf("\nTCPDRV%x: send %x (len %d) queued", t, pak,
	       pak->datagramsize);

    if (t->flow_ctrl)
        tcpdriver_do_flow_ctrl(t);

    return(TCPDSEND_QUEUED);
}

/*
 * tcpdriver_send_now
 * Queue up a packet and immediately try to trigger output.
 * 
 * Must not be used at interrupt level!
 */

static int tcpdriver_send_now (tcp_encaps *t, paktype *pak, boolean force)
{
    boolean still_queued;
    int result;

    result = tcpdriver_send(t, pak, force);
    if (result == TCPDSEND_QUEUED) {
	still_queued = TRUE;
	pak_lock(pak);
	tcpdriver_output(t);
	if (pak->datagramsize == 0)
	    still_queued = FALSE;
	datagram_done(pak);
	return(still_queued ? TCPDSEND_QUEUED : TCPDSEND_SENT);
    }
    return(result);
}

/*
 * tcpdriver_tcb
 * Return tcb pointer given handle
 */

static inline tcbtype *tcpdriver_tcb (tcp_encaps *t)
{
    return(t ? t->tcb : NULL);
}

/*
 * tcpdriver_fhost
 * return the foreign host of this stream
 */

static inline ipaddrtype tcpdriver_fhost (tcp_encaps *t)
{
    return(t ? t->tcb->foreignhost.ip_addr : 0);
}

/*
 * tcpdriver_lhost
 * return the local host ip address of this stream
 */

static inline ipaddrtype tcpdriver_lhost (tcp_encaps *t)
{
    return(t ? t->tcb->localhost.ip_addr : 0);
}

/*
 * tcpdriver_fport
 * return the foreign port of this stream
 */

static inline ushort tcpdriver_fport (tcp_encaps *t)
{
    return(t ? t->tcb->foreignport : 0);
}

/*
 * tcpdriver_lport
 * return the local port of this stream
 */

static inline ushort tcpdriver_lport (tcp_encaps *t)
{
    return(t ? t->tcb->localport : 0);
}

/*
 * tcpdriver_set_flag
 * tcpdriver_get_flag
 */

static void tcpdriver_set_flag (tcp_encaps *t, uint flag)
{
    if (t == NULL)
	return;

    t->flags |= flag;
}

static void tcpdriver_clear_flag (tcp_encaps *t, uint flag)
{
    if (t == NULL)
	return;

    t->flags &= ~flag;
}

/*
 * tcpdriver_set_tos
 * tcpdriver_get_tos
 */
static void tcpdriver_set_tos (tcp_encaps *t, uchar tos)
{
    if (t == NULL)
	return;

    if (t->tcb)
	t->tcb->param.tos = tos;
}

static uchar tcpdriver_get_tos (tcp_encaps *t)
{
    if (t == NULL)
	return(0);
    if (t->tcb == NULL)
	return(0);
    return(t->tcb->param.tos);
}

/*
 * tcpdriver_keepalive
 * Control keepalives
 */

static int
tcpdriver_keepalive (tcp_encaps *t, int op, ulong value)
{
    if (t == NULL || t->tcb == NULL)
	return(0);

    switch (op) {
    case TCPD_KP_ENABLE:
	if (value)
	    t->tcb->flags |= TCB_KEEPALIVE;
	else
	    t->tcb->flags &= ~TCB_KEEPALIVE;
	if (tcpdrv_debug)
	    buginf("\nTCPDRV%x: keepalives turned %s", t,
		   value ? "on" : "off");
	break;
 
    case TCPD_KP_INTERVAL:
	if (tcpdrv_debug)
	    buginf("\nTCPDRV%x: keepalive interval set to %d ms", t, value);
	t->tcb->maxaliveticks = value;
	break;

    case TCPD_KP_ATTEMPTS:
	if (tcpdrv_debug)
	    buginf("\nTCPDRV%x: keepalive attempts set to %d", t, value);
	t->tcb->maxalivetries = value;
	break;
    }
    return(0);
}

/*
 * tcp_driver
 * Handling reading, writing and driving for TCP data stream.
 */

static forktype tcp_driver (void)
{
    tcp_encaps *t, *next;

    while (TRUE) {
	edisms((blockproc *) tcpdriverBLOCK, 0);
	for (t = (tcp_encaps *) tcpdriverQ.qhead; t; t = next) {
	    next = t->next;

	    /*
	     * Extreme flakiness.
	     * Window opening is a true event, not a level,
	     * so we must reset it if it occurred.
	     */
	    if (t->tcb->events & TCPEV_WINDOW) {
		t->tcb->events &= ~TCPEV_WINDOW;
		tcp_wakeup_process(t->tcb);
	    }

	    /*
	     * Same for retransmission queue not-full-anymore.
	     */
	    if (t->tcb->events & TCPEV_RTQUEUE) {
		t->tcb->events &= ~TCPEV_RTQUEUE;
		tcp_wakeup_process(t->tcb);
	    }

	    /*
	     * If we are aborting, then be done.
	     */
	    if (t->state == TCPD_ABORT) {
		tcpdriver_doclose(t);
		continue;
	    }

 	    /*
 	     * If things closed underneath us, then we are as good as dead.
 	     */
 	    if ((t->state == TCPD_ALIVE) && (t->tcb->state == CLOSED)) {
 		tcpdriver_doclose(t);
  		continue;
  	    }

	    /*
	     * If we are closed but there is more work to do, then do it.
	     * However, if we have been closed longer than the 'dead wait'
	     * allowed for closing, close us down ANYWAY. Tough luck!
	     */
	    if (t->state == TCPD_CLOSE) {
		if (AWAKE(t->closetime) ||
		    ((tcp_unread(t->tcb) == 0) &&
		     (t->tcb->qsaved == 0) &&
		     (t->outputq.qhead == NULL))) {
		    tcpdriver_doclose(t);
		    continue;
		} else {
		    /*
		     * Closed, but there's still stuff to do --
		     * start the dead-wait timer now if it isn't
		     * already going.
		     */
		    if (!active_timer(t->timer))
			set_timer(t->timer, ONESEC * 30);
		}
	    }

	    /*
	     * Other side has closed down and we have less than a buffer,
	     *  - we are alive, then attempt close when we are finished.
	     *  - we are closed, then close immediately if output still
	     *    pending as it will never be processed.
	     */
	    if (t->tcb->state > ESTAB && tcp_unread(t->tcb) < t->inthresh) {
		if (t->state == TCPD_ALIVE)
		    tcpdriver_close_internal(t);
		else if (t->outputq.qhead != NULL) {
		    tcpdriver_doclose(t);
		    continue;
		}
	    }

	    /*
	     * If this is a new passively-opened connection,
	     * try to drain any data that might have arrived
	     * before tcpdriver_passive_open() was called.
	     */
	    if (t->events & TDEV_NEW_CONN) {
		/* Give the application an opportunity to take the data. */
		tcpdriver_input(t->tcb, t);

		/* Reset the event. */
		tcpdriver_clearevent(t, TDEV_NEW_CONN);
	    }

	    /*
	     * Perform as much output as possible
	     */
	    tcpdriver_output(t);
	}
    }
}

/*
 * tcpdriver_start
 * Start up the TCP driver upon request.
 */

static void tcpdriver_start (void)
{
    if (tcpdriver_pid == 0)
	tcpdriver_pid = cfork(tcp_driver, 0L, TCPDRIVER_STACK_SIZE,
			      "TCP Driver", 0);
    tcpdriver_running = TRUE;
}

/*
 * tcpdriver_init
 * Initialize TCP encapsulation
 */

void tcpdriver_init (void)
{
    tcpdriver_pid = 0;
    tcpdriver_running = FALSE;
    queue_init(&tcpdriverQ, 0);

    /*
     * Add tcpdriver registries
     */
    reg_add_ip_tcpdriver_active_open(tcpdriver_active_open,
				     "tcpdriver_active_open");
    reg_add_ip_tcpdriver_passive_open(tcpdriver_passive_open,
				      "tcpdriver_passive_open");
    reg_add_ip_tcpdriver_change_timeout(tcpdriver_change_timeout,
					"tcpdriver_change_timeout");
    reg_add_ip_tcpdriver_close(tcpdriver_close, "tcpdriver_close");
    reg_add_ip_tcpdriver_abort(tcpdriver_abort, "tcpdriver_abort");
    reg_add_ip_tcpdriver_update(tcpdriver_update, "tcpdriver_update");
    reg_add_ip_tcpdriver_unacked_output(tcpdriver_unacked_output,
					"tcpdriver_unacked_output");
    reg_add_ip_tcpdriver_oqlen(tcpdriver_oqlen, "tcpdriver_oqlen");
    reg_add_ip_tcpdriver_set_oqmax(tcpdriver_set_oqmax, "tcpdriver_set_oqmax");
    reg_add_ip_tcpdriver_set_flow_ctrl_levels(tcpdriver_set_flow_ctrl_levels,
                                              "tcpdriver_set_flow_ctrl_levels");
    reg_add_ip_tcpdriver_get_tos(tcpdriver_get_tos, "tcpdriver_get_tos");
    reg_add_ip_tcpdriver_set_tos(tcpdriver_set_tos, "tcpdriver_set_tos");

    reg_add_ip_tcpdriver_get_oqmax(tcpdriver_get_oqmax, "tcpdriver_get_oqmax");
    reg_add_ip_tcpdriver_send(tcpdriver_send, "tcpdriver_send");
    reg_add_ip_tcpdriver_send_now(tcpdriver_send_now, "tcpdriver_send_now");
    reg_add_ip_tcpdriver_tcb(tcpdriver_tcb, "tcpdriver_tcb");
    reg_add_ip_tcpdriver_fhost(tcpdriver_fhost, "tcpdriver_fhost");
    reg_add_ip_tcpdriver_lhost(tcpdriver_lhost, "tcpdriver_lhost");
    reg_add_ip_tcpdriver_fport(tcpdriver_fport, "tcpdriver_fport");
    reg_add_ip_tcpdriver_lport(tcpdriver_lport, "tcpdriver_lport");
    reg_add_ip_tcpdriver_set_flag(tcpdriver_set_flag, "tcpdriver_set_flag");
    reg_add_ip_tcpdriver_clear_flag(tcpdriver_clear_flag,
				    "tcpdriver_clear_flag");
    reg_add_ip_tcpdriver_keepalive(tcpdriver_keepalive, "tcpdriver_keepalive");
    reg_add_ip_tcpdriver_start(tcpdriver_start, "tcpdriver_start");
}
