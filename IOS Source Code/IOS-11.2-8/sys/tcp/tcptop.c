/* $Id: tcptop.c,v 3.15.4.11 1996/09/10 01:07:46 snyder Exp $
 * $Source: /release/112/cvs/Xsys/tcp/tcptop.c,v $
 *------------------------------------------------------------------
 * tcptop.c -- top level TCP routines
 *
 * 13-November-1985, Kirk Lougheed
 *
 * Copyright (c) 1985-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: tcptop.c,v $
 * Revision 3.15.4.11  1996/09/10  01:07:46  snyder
 * CSCdi68568:  more constant opportuniites
 *              140 from image, 52 from data
 * Branch: California_branch
 *
 * Revision 3.15.4.10  1996/08/28  13:17:22  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.15.4.9  1996/08/09  22:31:16  jenny
 * CSCdi65660:  TCP hung in CLOSEWAIT after window closes
 * Branch: California_branch
 * . Receiver reset connection if packet received after user close.
 * . Always send zero-window probe if send window shrunk to zero and
 *   no unACKed packet in retransmission queue.
 *
 * Revision 3.15.4.8  1996/06/28  23:30:23  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.15.4.7  1996/06/05  23:11:59  ahh
 * CSCdi59587:  TCP connections to translate not reset if max users exce...
 * If the connection hasn't been established yet (and never will be),
 * send a reset.
 * Branch: California_branch
 *
 * Revision 3.15.4.6  1996/05/17  18:24:44  vandys
 * CSCdi54618:  Line hung in carrier dropped state
 * Branch: California_branch
 * Handle TCP drop of AAA client under TTY session
 *
 * Revision 3.15.4.5  1996/05/04  02:07:50  wilber
 * Branch: California_branch
 *
 * Name Access List
 *
 * Revision 3.15.4.4  1996/04/27  06:29:48  syiu
 * Branch: California_branch
 * Commit Dial1_branch
 *
 * Revision 3.7.2.3  1996/04/26  00:19:00  syiu
 * Branch: Dial1_branch
 * Dial1_branch to IosSec_postmerge_960419
 *
 * Revision 3.7.2.2  1996/04/16  23:40:33  syiu
 * Branch: Dial1_branch
 * Sync Dial1_branch to V111_1_3
 *
 * Revision 3.7.2.1  1996/01/26  20:26:04  agh
 * Branch: Dial1_branch
 * Initial Checkin of MLPVT Code.
 *
 * Revision 3.15.4.3  1996/04/19  15:33:39  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commit
 *
 * Revision 2.2.6.9  1996/04/10  23:09:24  lol
 * Branch: IosSec_branch
 *         Incorporate code review comments.
 *
 * Revision 2.2.6.8  1996/04/09  22:04:03  che
 * Branch: IosSec_branch
 * Sync to California_branch (960405)
 *
 * Revision 2.2.6.7  1996/03/19  08:48:10  che
 * Branch: IosSec_branch
 * Sync to V111_1_3
 *
 * Revision 2.2.6.6  1996/02/28  06:15:27  lol
 * Branch: IosSec_branch
 * TACACS+ processing.
 *
 * Revision 2.2.6.5  1996/02/21  04:13:43  che
 * Branch: IosSec_branch
 * Sync to 11.1 Mainline (02/20/96)
 *
 * Revision 2.2.6.4  1996/01/23  06:51:01  che
 * Branch: IosSec_branch
 * Sync with 11.1 mainline. (960122)
 *
 * Revision 2.2.6.3  1995/12/19  04:20:23  che
 * Branch: IosSec_branch
 * Re-sync to Arkansas to fix things that broke.
 *
 * Revision 2.2.6.2  1995/12/17  02:32:23  che
 * Branch: IosSec_branch
 * Sync to Arkansas
 *
 * Revision 2.2.6.1  1995/11/17  18:35:57  carrel
 * Branch: IosSec_branch
 * Sync to Arkansas
 *
 * Revision 3.15.4.2  1996/03/28  00:16:26  jenny
 * CSCdi51402:  TCP Path-MTU Discovery needs to be dynamic
 * Branch: California_branch
 * Implemented TCP PMTU dynamic discovery and PMTU timers (RFC 1191).
 * Moved ip tcp parser command into a seperated parser chain in TCP for
 * modularity.
 *
 * Revision 3.15.4.1  1996/03/18  22:13:56  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.11.2.3  1996/03/16  07:41:21  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.11.2.2  1996/03/07  10:53:17  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.11.2.1  1996/02/20  21:21:35  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.15  1996/03/08  00:49:58  jenny
 * CSCdi51015:  TCP may block unintentionally and may abort data write in
 * CLOSEWAIT
 * Make sure all blocking conditions are checked in CLOSEWAIT state and
 * do not return error to application when data are sent in this state.
 *
 * Revision 3.14  1996/03/08  00:43:27  lol
 * CSCdi50738:  no timeouts in T+
 *         Implement T+ timeouts for a customer who just won't wait. This
 *         code becomes obsolete in California.
 *
 * Revision 3.13  1996/02/23  23:38:56  ahh
 * CSCdi47841:  Running some telnet/pad sessions over serial causes rout...
 * Allow tcp_uniqueport() to fail, and clean up some debug messages.
 *
 * Revision 3.12  1996/01/24  20:41:27  jenny
 * CSCdi47046:  TCP/IP need code clean up (part 3)
 * . removed ip/tuba.c, ip/tuba_default.c and ip/tuba.h
 * . removed all includes for ip/tuba.h.
 * . removed more TUBA related code.
 *
 * Revision 3.11  1996/01/22  19:11:39  jenny
 * CSCdi46967:  crash in tcp_putsetup
 * Make sure checking for blocking condition in both ESTAB and CLOSEWAIT
 * states. Allow writing to TCP in CLOSEWAIT state as well.
 *
 * Revision 3.10  1996/01/22  07:29:28  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.9  1996/01/16  16:53:11  ahh
 * CSCdi46924:  TCP: listener process can hang if callback does not accept
 * Throw the new connection away if the app refuses to take it.
 *
 * Revision 3.8  1996/01/16  03:38:04  ahh
 * CSCdi46921:  TCP: remove unused second parameter to tcp_close
 *
 * Revision 3.7  1996/01/11  02:34:01  jenny
 * CSCdi46513:  TCP/IP need code clean up (part 2)
 * . removed ip_tuba.c and clns_tuba.c.
 * . removed lots of tuba related registry stubs and code.
 *
 * Revision 3.6  1995/12/29  01:22:01  ahh
 * CSCdi46011:  TCP/IP need to do code clean up.
 * Fix some tab damage.
 *
 * Revision 3.5  1995/12/28  18:29:30  jenny
 * CSCdi46011:  TCP/IP need to do code clean up.
 * This is the first patch of code changes for TCP/IP code cleanup.
 * . removed all references to TUBA_IP and TUBA_CLNS.
 * . renamed open_tuba_socket() to ip_open_socket().
 * . renamed tcp_tuba_open() to tcp_open().
 * . pass addrtype * instead of ipaddrtype for local and foreign addresses
 *   to tcp_open() for future portability.
 *
 * Revision 3.4  1995/12/13  04:24:52  ahh
 * CSCdi44230:  Remove dead code
 * Bring out your dead... *clang* (RSRB/TCP fast-switching)
 *
 * Revision 3.3  1995/11/21  19:12:06  jenny
 * CSCdi43984:  TCP could be a CPUHOG if connection started from a cron job
 * Do edisms if our send window is full and peer is only half closed.
 *
 * Revision 3.2  1995/11/17  18:49:25  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:36:13  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.14  1995/11/08  21:32:19  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.13  1995/09/20  05:03:09  ahh
 * CSCdi40709:  TCP: off-by-1 error can lead to missed event
 * Make the event detection match the triggering code in ack processing.
 *
 * Revision 2.12  1995/08/18  02:58:30  ahh
 * CSCdi38935:  TCP: TCP Listener should use new process primitives
 * Use process and list primitives for connection queueing support.
 * Clean up some crufties, and delete some conditionally-compiled
 * debug code.
 *
 * Revision 2.11  1995/07/29  00:21:58  ahh
 * CSCdi37655:  TCP: Listener process should be started on demand
 * Start it on demand, and make it low priority.
 *
 * Revision 2.10  1995/07/27  07:30:58  hampton
 * Rename insque/remque to lw_insert/lw_remove. [CSCdi37745]
 *
 * Revision 2.9  1995/07/12 23:45:50  hampton
 * Convert TCP to fully use the passive timers macros instead of
 * referencing the system clock directly.  [CSCdi37015]
 *
 * Revision 2.8  1995/07/06 21:46:15  ahh
 * CSCdi36734:  RCP: cannot stop file transfer on a VTY
 * Consume VTY input during file transfer so the TTY code will
 * notice when someone hits the escpae char.
 *
 * Revision 2.7  1995/06/22  05:51:20  ahh
 * CSCdi36259:  RLOGIN: pending output not flushed correctly (regress)
 *
 * Revision 2.6  1995/06/20  22:02:14  ahh
 * CSCdi31398:  Rlogin sending window size packet twice
 * Eliminate race conditions in rlogin's urgent-data processing.
 *
 * Revision 2.5  1995/06/17  06:26:49  ahh
 * CSCdi35734:  TCP: Long overdue code reorganization
 * Separate tcp.h into public and private header files.  Shuffle functions
 * around into more logical groupings.  Rename functions to meet naming
 * conventions (say goodbye to StudlyCaps).  Delete unneeded externs.
 * Trim off old log messages.
 *
 * Revision 2.4  1995/06/16  04:29:53  dkatz
 * CSCdi35882:  Time-of-day code needs cleanup
 *
 * Revision 2.3  1995/06/14  19:22:57  vandys
 * CSCdi35265:  Should use intfc description if no line description
 * Don't reference tty->location directly; pass tt_soc to a
 * function, let it take from either tt_soc or IDB.
 *
 * Revision 2.2  1995/06/09  13:21:47  hampton
 * Rename old scheduler calls to fit the new naming convention.
 * [CSCdi35514]
 *
 * Revision 2.1  1995/06/07 23:06:20  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "interface_private.h"
#include "packet.h"
#include "logger.h"
#include "../tcp/msg_tcp.c"		/* Not a typo, see logger.h */
#include "sys_registry.h"
#include "../ip/ip_registry.h"
#include "../h/cs_registry.regh"
#include "../ui/debug.h"
#include "../os/buffers.h"
#include "../ip/ip.h"
#include "tcp.h"
#include "tcpinternal.h"
#include "tcpdriver.h"
#include "tcp_debug.h"
#include "ttysrv.h"
#include "connect.h"
#include "../ip/ipoptions.h"
#include "logger.h"
#include "access.h"
#include "../ip/ipaccess.h"
#include "config.h"
#include "telnet.h"
#include "name.h"
#include "../iprouting/route.h"
#include "../xremote/xremote.h"
#include "../ui/service.h"
#include "../util/md5.h"
#include "../os/free.h"
#include "../if/network.h"

int tcp_chunksize;
ulong tcp_defrcvwnd;
int tcp_finwaittm = DEF_FINWAITTM;
int tcp_queuemax = DEF_QUEUEMAX_USER;
int tcp_synwait = DEF_SYNWAIT;
boolean tcp_pathmtu = DEF_PATHMTU;
int tcp_pathmtu_ager;		/* PMTU age timer in unit of minutes */
long len_pathmtu_timeout;	/* PMTU age timer in unit of msecs */
int tcp_listenQ_max = MAX_LISTENQUEUE_SIZE;
int tcp_listenQ_count = 0;

/* A group of TCBs on which to wait for connections */
struct tcp_listen_group {
    tcbtype	**tcbs;		/* pointer to TCB pointers */
    int		count;		/* number of TCBS */
};

list_header tcp_newconn_list;
pid_t tcp_listener_pid;
static struct tcp_listen_group tcp_listener_group;

static ulong tcp_0window = 0, tcp_nobuffer = 0, tcp_queuefull = 0;

/*
 * tcp_open
 * Create a TCP connection to a remote host.
 * On success, returns pointer to TCB for connection.
 * On failure, returns NULL with error code in status.
 */
tcbtype *tcp_open (
    tt_soc *tty,		/* pointer to associated terminal block */
    ushort fport,		/* foreign port */
    addrtype *fhost,		/* foreign address */
    ushort lport,		/* local port (zero if don't care) */
    addrtype *lhost,		/* local address (NULL if don't care) */
    int *status,		/* error code on failure */
    ipoptiontype *ipopts)	/* ip options to use (source route) */
{
    tcbtype *tcb;

    /* Create the TCB and mark it as an old-style TCB. */
    tcb = tcp_create(tty, status, ipopts);
    if (tcb == NULL)
	return(NULL);
    tcb->internal_flags &= ~TCB_INT_PRIMITIVES;

    /* Bind the local address if requested. */
    if (lport || lhost) {
	*status = tcp_bind(tcb, lhost, lport);
	if (*status != OK) {
	    tcp_close(tcb);
	    return(NULL);
	}
    }

    /* Connect to the remote host. */
    *status = tcp_connect(tcb, fhost, fport);
    if (*status != OK) {
	tcp_close(tcb);
	return(NULL);
    }

    return(tcb);
}

/*
 * tcpopenBLOCK
 * Block until a TCP connection is open, failed, or timed out
 */

static
boolean tcpopenBLOCK (tcbtype *tcb)
{
    if (tcb->tty && (psiset(tcb->tty) || (tcb->tty->statbits & CARDROP)))
	return(FALSE);		/* user aborted the open */
    if (tcb->state == ESTAB || tcb->state == CLOSED ||
	tcb->state == TIMEWAIT || tcb->state == CLOSEWAIT)
	return(FALSE);	/* connection open, refused, timed out, or closed */
    return(TRUE);
}

static
void tcp_idle_open (tcbtype *tcb)
{
    if (!tcb->pid) {
	/* wait for connection to open */
	edisms((blockproc *) tcpopenBLOCK, (ulong) tcb);
	return;
    }

    while (TRUE) {
	process_wait_for_event();
	if (tcpopenBLOCK(tcb))
	    continue;
	return;
    }
}

/*
 * tcp_set_rcvwnd
 *
 * Enlarge the receive window.
 */

int tcp_set_rcvwnd (tcbtype *tcb, ulong window)
{
    if (tcb == NULL)
	return(-1);

    /* Truncate since we don't support huge windows yet. */
    window = TCP_TRUNC_WINDOW(window);

    if (window > tcb->maxrcvwnd) {
	if (tcb->rcvwnd)	/* add in now if window open */
	    tcb->rcvwnd += window - tcb->maxrcvwnd;
	else			/* add in when window opens */
	    tcb->delrcvwnd += window - tcb->maxrcvwnd;
	tcb->maxrcvwnd = window;
    }

    return(0);
}

/*
 * tcplistenBLOCK
 * Wait until the TCB state changes from LISTEN
 */

static boolean tcplistenBLOCK (tcbtype *tcb)
{
    if (tcb->flags & TCB_SYNCLISTEN)
	return(FALSE);
    else
	return((tcb->events & TCPEV_NOT_LISTEN) ? FALSE : TRUE);
}

static
void tcp_idle_listen (tcbtype *tcb)
{
    if (!tcb->pid) {
	/* wait for connection to open */
	edisms((blockproc *) tcplistenBLOCK, (ulong) tcb);
	return;
    }

    while (TRUE) {
	process_wait_for_event();	/* wait for connection to open */
	if (tcplistenBLOCK(tcb))
	    continue;
	return;
    }
}


/*
 * tcp_create_wild_listen
 * Creates a TCB for listen.
 *
 * Returns pointer to a TCB on success.  Caller must verify TCB state.
 * Returns NULL if unable to create TCB or net service not yet available.
 */

tcbtype *
tcp_create_wild_listen (
    tt_soc *tty,        /* pointer to associated terminal block */
    ushort lport)       /* local port */
{
    tcbtype *tcb;

    /*
     * Create and initialize the TCB.
     */
    if (!systeminit_flag)
	return(NULL);
    tcb = tcp_allocatetcb();	/* get a blank TCB */
    if (tcb == NULL)		/* quit now if no memory */
	return(NULL);
    tcb->delrcvwnd = 0;		/* don't use a temporary 0 window */
    tcb->tty = tty;		/* our controlling terminal, if any */
    tcb->foreignport = 0;	/* any foreign port */
    tcb->foreignhost.type = ADDR_ILLEGAL; /* any foreign address */
    tcb->localport = lport;	/* set local port */
    tcb->localhost.type = ADDR_ILLEGAL;	/* best local address */
    tcb->state = LISTEN;	/* we are listening */
    tcb->events |= tcp_set_events[LISTEN];
    tcb->events &= tcp_clear_events[LISTEN];
    tcb->flags |= TCB_PASSIVEOPEN; /* we started out passive */
    tcb->internal_flags |= TCB_INT_WAIT_ESTAB; /* want to be notified of
						  problem with new connections
						  before they are in ESTAB */
    tcb_enqueue(tcb);
    return(tcb); 
}


/*
 * tcp_init_rtto
 * Initialize the round trip timeout values for a new TCB.
 */

static void
tcp_init_rtto (tcbtype *tcb)
{
    idbtype *idb;
    ipaddrtype dummy; 

    dummy = tcb->foreignhost.ip_addr;

    idb = reg_invoke_ip_first_hop(&dummy, dummy, IP_PATH_DEFFLAGS);
    if (idb && idb->visible_bandwidth < 100) {
        tcb->srtt = TCP_SLOWRTT;
        tcb->rtto = TCP_SLOWRTO;
    }

    /* Been a while since tcp_allocatetcb() */
    GET_TIMESTAMP(tcb->starttime);
}


/*
 * tcp_old_listen
 * Listen for a TCP connection.
 *
 * This function will block until a connection is formed with remote.
 */

tcbtype *
tcp_old_listen (
    tt_soc *tty,	/* pointer to associated terminal block */
    ushort lport)	/* local port */
{ 
    tcbtype *tcb;

    tcb = tcp_create_wild_listen(tty, lport);
    if (tcb == NULL)
	return(NULL);    
    process_get_pid(&tcb->pid);
    /*
     * Block until we form a connection of some sort.
     */
    tcp_idle_listen(tcb);

    tcp_init_rtto(tcb);
    return(tcb);			/* return tcb pointer */
}

static
int wait_for_estab(tcbtype *tcb)
{
    /* Wait for connection to become established before proceeding */
    if (tcb->state < ESTAB) {
	if (tcppkt_debug)
	    buginf("\ntcp0: W %s %s:%d %s:%d estabBLOCK",
		   tcp_statestring(tcb->state),
		   address_string(&tcb->foreignhost), tcb->foreignport,
		   address_string(&tcb->localhost), tcb->localport);

	TIMER_START(tcb->dallytime, 30*ONESEC);
	tcp_idle_estab(tcb);
    }
    if ((tcb->state != ESTAB && tcb->state != CLOSEWAIT) ||
	mempool_is_empty(MEMPOOL_CLASS_LOCAL)) {
	if (tcppkt_debug)
	    buginf("\ntcp0: T %s %s:%d %s:%d early close",
		   tcp_statestring(tcb->state),
		   address_string(&tcb->foreignhost), tcb->foreignport,
		   address_string(&tcb->localhost), tcb->localport);
	return(-1);
    }
    return(0);
}

/*
 * tcp_establish
 *	Used on a synchronized-listen TCB to accept the connection
 * and move the TCB through SYNRCVD to ESTAB.
 * The caller must close the TCB if this function fails.
 */

int tcp_establish (tcbtype *tcb)
{
    curpkttype *curpkt = tcb->saved_curpkt;
    boolean blocking_verboten;
    int rc;

    if (curpkt == NULL)
	return(-1);

    /*
     * Send an ACK of the SYN.  Process any data.
     */
    rc = tcp_sendsynsegment(tcb, tcp_selectiss(), TRUE, tcb->rcvnxt);
    if (rc != OK) {
	/* The SYN+ACK packet wasn't sent. */
	return(-1);
    }
    tcp_handleother(tcb, curpkt, FALSE);

    tcp_removeackedsendsegments(tcb);
    tcb->pakincount++;

    free(curpkt);
    tcb->saved_curpkt = NULL;

    blocking_verboten = blocking_disabled;
    if (blocking_disabled && (tcb->flags & TCB_SYNCLISTEN))
	blocking_disabled = FALSE;  /* XXX this really sucks. */
    rc = wait_for_estab(tcb);
    blocking_disabled = blocking_verboten;

    return(rc);
}

/*
 * tcp_add_listen_tcb
 * tcp_rem_listen_tcb
 *
 * TCB list maintenance routines
 */
static int
tcp_add_listen_tcb (tcbtype *tcb)
{
    tcbtype **tcbs;
    int i;

    /* Allocate a new array of tcb pointers. */
    tcbs = malloc(sizeof(tcbtype *) * (tcp_listener_group.count + 1));
    if (tcbs == NULL)
	return(NO_MEMORY);

    /* Copy the tcb pointers to the new array, then add the new one. */
    for (i = 0; i < tcp_listener_group.count; i++)
	tcbs[i] = tcp_listener_group.tcbs[i];
    tcbs[i] = tcb;

    /* Throw away the old array. */
    if (tcp_listener_group.tcbs)
	free(tcp_listener_group.tcbs);
    tcp_listener_group.tcbs = tcbs;
    tcp_listener_group.count++;

    return(OK);
}

static int
tcp_rem_listen_tcb (tcbtype *tcb)
{
    tcbtype **tcbs;
    int i;

    /* Allocate a new array of tcbs. */
    tcbs = malloc(sizeof(tcbtype *) * (tcp_listener_group.count - 1));
    if (tcbs == NULL)
	return(NO_MEMORY);

    /* Copy up to but not including the victim. */
    for (i = 0; i < tcp_listener_group.count; i++) {
	if (tcp_listener_group.tcbs[i] == tcb)
	    break;		/* found the victim, stop copying */
	tcbs[i] = tcp_listener_group.tcbs[i];
    }
    /* Copy after the victim to the end. */
    for (i = i; i < tcp_listener_group.count - 1; i++)
	tcbs[i] = tcp_listener_group.tcbs[i + 1];
 
    /* Throw away the old array. */
    if (tcp_listener_group.tcbs)
	free(tcp_listener_group.tcbs);
    tcp_listener_group.tcbs = tcbs;
    tcp_listener_group.count--;

    return(OK);
}

/*
 * tcp_close_children
 * Close the children of a listening TCB (internal use).
 */

void tcp_close_children (tcbtype *tcb)
{
    tcbtype *tcbtemp;
    int i;

    /* Throw away any queued connections. */
    while ((tcbtemp = list_dequeue(&tcb->connq)))
	tcp_close(tcbtemp);
    /* Throw away any SYNRCVD TCB's floating out there. */
    for (i = 0; i < TCBHASHSIZE; i++) {
	tcbtype *pred;

	pred = (tcbtype *) &tcbhashtable[i];
	while (pred->next) {
	    if (pred->next->state == SYNRCVD && pred->next->parent == tcb) {
		tcbtemp = pred->next;
		pred->next = tcbtemp->next;
		tcbtemp->next = NULL;
		tcp_close(tcbtemp);
	    } else
		pred = pred->next;
	}
    }
}

/*
 * tcp_close_statechange
 * Do the right thing for a close depending on the state of the TCB
 */

void tcp_close_statechange (tcbtype *tcb)
{
    switch (tcb->state) {
    case LISTEN: 
    case SYNSENT: 
	tcp_deallocatetcb(tcb, LOCAL_RESET);
	break;

    case SYNRCVD: 
	if ((tcb->flags & TCB_SYNCLISTEN) && (tcb->iss == 0)) {
	    /*
	     * If the connection hasn't become established yet
	     * and never will be because we haven't ACKed the other
	     * side's SYN and sent our own, reset the connection
	     * instead of trying for a graceful close.
	     */
	    tcp_reset(tcb);
	} else {
	    if (QUEUEEMPTY(&tcb->q[SEGQUEUE]) &&
		QUEUEEMPTY(&tcb->q[RETRANSQUEUE])) {
		tcp_changestate(tcb, FINWAIT1);
	    }
	    tcp_sendfin(tcb);
	}
	break;

    case ESTAB: 
	tcp_changestate(tcb, FINWAIT1);
	tcp_sendfin(tcb);
	break;

    case CLOSEWAIT: 
	tcp_changestate(tcb, LASTACK);
	tcp_sendfin(tcb);
	/*
	 * Start a timer so we won't be in LASTACK state forever.
	 */
	tcp_starttimer(tcb, TMEWAITTIMEOUT, LENFINWAIT2TIMEOUT);
	break;

    default:
	/* do nothing */
	break;
    }
}

/*
 * tcp_close
 * Normal close of a TCP connection.
 */

void tcp_close (tcbtype *tcb)
{
    if (tcb->internal_flags & TCB_INT_PRIMITIVES) {
	/* Make sure tcp_listener won't handle this TCB anymore. */
	if (tcb->listencb)
	    tcp_rem_listen_tcb(tcb);
    
	tcp_close_children(tcb);
	if (LIST_ELEMENT_QUEUED(&tcb->newconn_element))
	    list_remove(&tcp_newconn_list, &tcb->newconn_element, NULL);
    }

    tcp_close_statechange(tcb);

    /*
     * Mark the TCB as closed, so asynchronous-close code can feel
     * free to throw the TCB away.  By calling tcp_close(), the
     * application has forgotten about this TCB.
     */
    tcb->flags |= TCB_APP_CLOSED;
    tcb->pid = 0;

    /*
     * Free everything now if not connected.
     * Otherwise, LASTACK and TIMEWAIT processing will free the TCB
     * and related structures.
     */
    if (tcb->state < ESTAB) {
	tcp_async_cleanup(tcb, -1);
	tcb = NULL;	/* paranoia */
    }
}

/*
 * tcp_async_close
 *
 * Clean up a connection after closing it asynchronously.
 */

void tcp_async_cleanup (tcbtype *tcb, int why)
{
    tcp_deallocatetcb(tcb, why);
    if (tcp_debug)
	buginf("\nTCB%08lx destroyed", tcb);
    if (!(tcb->internal_flags & TCB_INT_NOFREE))
	free(tcb);
}

/*
 * tcp_reset
 * Reset a connection.
 */

int tcp_reset (tcbtype *tcb)
{
    switch (tcb->state) {
    case SYNRCVD:
	/* Send an ACK to the peer's SYN along with the RST. */
	tcp_sendcontrolsegment(tcb, tcb->localport, tcb->foreignport,
			       &tcb->foreignhost, tcb->sndnxt, TRUE,
			       tcb->rcvnxt, TRUE, FALSE);
	tcp_deallocatetcb(tcb, LOCAL_RESET);
	break;
    case ESTAB:
    case FINWAIT1:
    case FINWAIT2:
    case CLOSEWAIT:
	tcp_sendcontrolsegment(tcb, tcb->localport, tcb->foreignport,
			       &tcb->foreignhost, tcb->sndnxt, FALSE,
			       0, TRUE, FALSE);
	tcp_deallocatetcb(tcb, LOCAL_RESET);
	break;
    case CLOSED:
	tcp_deallocatetcb(tcb, -1);
	break;
    default: 
	tcp_deallocatetcb(tcb, LOCAL_RESET);
	break;
    }

    return(OK);
}

/*
 * tcp_abort
 * Abort a TCP connection, i.e., close but don't dally.
 */

void tcp_abort (tcbtype *tcb)
{
    /*
     * If the receiver's window is closed and there is something
     * on the retransmission queue (very likely), usurp the first
     * segment's sequence number for that of the RST to ensure
     * that the other side will process it rather than rejecting
     * as past the right edge of its window.  Actually, there is
     * only one sequence number it will take, and that's the one
     * we'll be using.
     */
    if (tcb->sndwnd == 0 && !QUEUEEMPTY(&tcb->q[RETRANSQUEUE])) {
	paktype *pak;

	pak = ((elementtype *) tcb->q[RETRANSQUEUE].qhead)->pakptr;
	if (pak) {
	    tcptype *tcp;

	    tcp = tcp_get_hdr(pak, tcb);
	    tcb->sndnxt = tcp->sequencenumber;
	}
    }

    tcp_reset(tcb);
    tcb->pid = 0;
    tcp_async_cleanup(tcb, -1);
}


/*
 * tcp_clear_conn
 *
 * Clears a TCP connection. First send a RST to remote, and cleanup
 * the local TCB as if a RST is received from remote as well. 
 * Deallocate and free the TCB if it's at a state that application
 * had already closed the connection, otherwise, just change the 
 * state to CLOSED for the local TCB, the connection will be closed
 * by the applications. 
 */
void 
tcp_clear_conn (tcbtype *tcb)
{

    tcp_sendcontrolsegment(tcb, tcb->localport, tcb->foreignport,
			   &tcb->foreignhost, tcb->sndnxt, FALSE, 
			   tcb->rcvnxt, TRUE, FALSE);
    tcp_changestate(tcb, CLOSED);
    tcb->lsterr = LOCAL_RESET;
    tcp_stoptimer(tcb, RETRANSTIMEOUT);

    if (tcb->flags & TCB_APP_CLOSED) {
	/*
	 * Application has done a close already forgotten about the 
	 * connection, so clean up now.
	 */
	tcp_async_cleanup(tcb, tcb->lsterr);
	tcb = NULL;
    }
}
    

/*
 * tcp_putsetup
 * Set up an output buffer for a TCP connection.
 * Returns FALSE if no available buffers or if window is closed.
 * On a FALSE return, tcb->dallytime is set if we failed because of lack
 * of buffers.
 */

boolean tcp_putsetup (tcbtype *tcb)
{
    paktype *pak;
    ulong size;
    long guess, overhead;
    iphdrtype *ip;
    int optbytes;

    TIMER_STOP(tcb->dallytime);
    if (tcb->curoutputpak) {
	return(tcb->curoutputpak->length != 0);
    }

    /*
     * If the offered window (sndwnd) is closed (zero) and there is nothing
     * in the retransmit queue, send just one byte to try opening the window.
     */
    size = tcp_usable_window(tcb);
    if (size == 0) {
	/*
	 * If the usable window is too small and there is nothing in our
	 * retransmit queue, start a timer to periodically query the remote
	 * host to see if its receive window has grown.  Let the system
	 * transmit one byte into the 0 window.  Note that this byte will
	 * be held for one RTT before being transmitted the first time,
	 * in hopes of receiving an ACK with a better window
	 */
	if (QUEUEEMPTY(&tcb->q[RETRANSQUEUE])) {
#ifdef TCPVERBOSE
	    if (tcp_debug)
		buginf("\nTCP%t: putsetup: probing window.", tcb->ttynum);
#endif
	    size = 1;
	    tcp_starttimer(tcb, SNDWNDTIMEOUT, tcb->rtto);
	} else {
#ifdef TCPVERBOSE
	    if (tcp_debug)
		buginf("\nTCP%t: putsetup: window closed.", tcb->ttynum);
#endif
	    if (!schedflag)
		tcp_0window++;
	    return(FALSE);
	}
    } else
	tcp_stoptimer(tcb, SNDWNDTIMEOUT);	/* Stop send window timer */

    /*
     * If we have many packets pending for transmission, wait until
     * they are sent before sending any more. This prevents us from
     * overrunning our buffer queue before the window closes.
     */
    if (tcb->qoutput >= QUEUEMAX) {
	if (!schedflag)
	    tcp_queuefull++;
#ifdef TCPVERBOSE
	if (tcp_debug)
	    buginf("\nTCP%t: putsetup: output queue full.", tcb->ttynum);
#endif
	return(FALSE);
    }

    /*
     * Limit packet size to less than the maximum segment size.  Further
     * limit it to our best guess based on associated tty of the maximum
     * output bytes.  On tcp connections without a tty (BGP, X25 switch),
     * get a MSS sized packet.
     */
    if (tcb->tty && !(tcb->flags & TCB_FULLSEGS)) {
	guess = outguess(tcb->tty);		/* make a good guess */
	if (size > guess)			/* let that guess bound us */
	    size = guess;
    }
    if (size > tcb->maxsegmentsize)	/* we are also bounded by MSS */
	size = tcb->maxsegmentsize;
    if (size < 1) {
	errmsg(&msgsym(BUFFER, TCP), tcb->ttynum);
	return(FALSE);
    }
    optbytes = tcp_optionbytes(tcb, FALSE);
    if (optbytes > MAXTCPOPTIONBYTES) {
	errmsg(&msgsym(TOOBIG, TCP), tcb->ttynum, optbytes);
	return(FALSE);
    }
    overhead = IPHEADERBYTES(tcb->options) + TCPHEADERBYTES + optbytes;
    pak = getbuffer(overhead + size);
    if (pak == NULL) {
	if (!schedflag)
	    tcp_nobuffer++;
	TIMER_START(tcb->dallytime, 2*ONESEC);
#ifdef TCPVERBOSE
	if (tcp_debug)
	    buginf("\nTCP%t: putsetup: no buffer.", tcb->ttynum);
#endif
	return(FALSE);
    }
    tcb->curoutputpak = pak;
    ip = (iphdrtype *)ipheadstart(pak);
    ip->ihl = 0;
    pak->us_seglen = size;
    pak->us_segdataoffset = 0;
    pak->linktype = LINK_IP;
    pak->length = size;
    pak->dataptr = pak->network_start + overhead;
    return(TRUE);
}

/*
 * tcp_usable_window
 * Return the number of bytes in the send window, taking into account
 * SWS avoidance.
 */

ulong tcp_usable_window (tcbtype *tcb)
{
    ulong r_edge, size;
    ulong win;

    /*
     * The following is Clark's SWS avoidance.  The usable window size is
     * the # of bytes between the sequence number of the next byte to be sent
     * (tcb->sndnxt) and the right window edge (tcb->sndwnd + tcb->snduna).
     * If the usable window is less than 25% of the offered window size
     * (tcb->sndwnd), set the usable window to zero.
     */

    win = min(tcb->sndwnd, tcb->sndcwnd);
    r_edge = win + tcb->snduna;
    if (LEQ(r_edge, tcb->sndnxt)) {
	size = 0;
    } else {
	size = r_edge - tcb->sndnxt;
	if (size < tcb->maxsegmentsize && size < (win >> 2))
	    size = 0;
    }

    return(size);
}

/*
 * tcp_putblock - Determine if an output request would block
 */

boolean tcp_putblock (tcbtype *tcb)
{
    if (tcb->curoutputpak && tcb->curoutputpak->length)
	return(FALSE);
    /*
     * Allow the dallytimer to expire to give the system a chance 
     * to recover from buffer shortage.
     */
    if (TIMER_RUNNING_AND_SLEEPING(tcb->dallytime))
	return(TRUE);
    return(!tcp_putsetup(tcb));
}

/*
 * tcp_putbyte
 * Write a byte to a net connection.  Returns TRUE on success, FALSE if
 * there is no buffer room and the user did not specify blocking, or if
 * the connection is dead.
 */

boolean tcp_putbyte (
    tcbtype *tcb,
    uchar ch,
    int blockflag)
{
    paktype *pak = tcb->curoutputpak;

    if (blocking_disabled && blockflag)
	errmsg(&msgsym(PUTBYTE, TCP), tcb->ttynum);

    while (pak == NULL) {
	if (tcp_putsetup(tcb) == TRUE)
	    pak = tcb->curoutputpak;
	else {
	    enum tcp_state state = tcb->state;
	    if (!blockflag ||
		(state != ESTAB && state != CLOSEWAIT) ||
		psipending())
		return(FALSE);
	    else {
		tcp_idle_put(tcb);
	    }
	}
    }
    *pak->dataptr++ = ch;
    if (--pak->length == 0) {
	(void) tcp_putpush(tcb, (tcb->flags & TCB_ALWAYSPUSH) ? TRUE : FALSE,
			   FALSE);
    }
    return(TRUE);
}

/*
 * tcp_putstring
 * Write a string to a net connection.  Returns number of bytes that were
 * actually written before we run out of buffers or have to block.
 * We may write multiple packets.
 * This function doesn't block.
 */

int tcp_putstring (tcbtype *tcb, uchar *p, int len)
{
    paktype *pak;
    int left = len;

    while (left != 0) {
	if (!tcb->curoutputpak && tcp_putsetup(tcb) == FALSE) {
	    return(len - left);		/* we would block */
	}
	pak = tcb->curoutputpak;
	if (left <= pak->length) {	/* Last chunk of data */
	    bcopy(p, pak->dataptr, left);
	    pak->dataptr += left;
	    pak->length -= left;
	    left = 0;
	} else {			/* All but last */
	    bcopy(p, pak->dataptr, pak->length);
	    p += pak->length;
	    pak->dataptr += pak->length;
	    left -= pak->length;
	    pak->length = 0;
	}
	if (pak->length == 0)
	    (void) tcp_putpush(tcb, FALSE, FALSE);
    }
    return(len);
}

/*
 * tcp_piggybacked_tty
 *
 * Return TRUE if this TCB is piggybacking on another TCB's tty
 * (most likely so it can catch aborts on a VTY).
 */

static boolean
tcp_piggybacked_tty (tcbtype *tcb)
{
    conntype *conn;

    if (tcb->tty == NULL)
	return(FALSE);

    /*
     * Check if there is a connection block, and if it's TCB
     * does not point back at this one.
     */
    conn = tcb->tty->conntop;
    if (conn && conn->tcb && (conn->tcb != tcb))
	return(TRUE);

    return(FALSE);
}

/*
 * tcp_putstringblock
 *
 * Like tcp_putstring, but do not return until we have either sent
 * out all the data we requested to send or we have an error condition
 * on the connection.
 */

int tcp_putstringblock (tcbtype *tcb, uchar *p, int len, int *error)
{
    int orig, i;
    int throw_away;

    for (orig = len; (i = tcp_putstring(tcb, p, len)) < len; p+= i,len -= i) {
	tcp_idle_put(tcb);

	if (tcb->tty) {
	    if (tcp_piggybacked_tty(tcb)) {
		/*
		 * The following getc() is an attempt to get PSIPENDING flag
		 * set if the user types an escape on the vty associated with
		 * (but not owned by) this connection.
		 */
		do {
		    throw_away = getc(tcb->tty);
		} while (throw_away != -1);
	    }

	    if (psipending2(tcb->tty)) {
		*error = LOCAL_RESET;
		return(0);
	    }
	}

	if (tcp_closing(tcb)) {
	    *error = ABORTED;
	    return(0);
	}
    }

    if (tcb->state == ESTAB || tcb->state == CLOSEWAIT) {
	*error = OK;
	return(orig);
    } else {
	*error = ABORTED;
	return(0);
    }
}
/*
 * tcpputBLOCK
 * Event dismiss for TCP put byte operation.  We blocked because the window
 * was too small (SWS prevention) or because we ran out of buffers.
 * Rather than figure out the buffer situation, we activate every couple of
 * seconds to try again.
 */

boolean tcpputBLOCK (tcbtype *tcb)
{
    /*
     * Activate if connection is dead.
     */
    if (tcb->state != ESTAB && tcb->state != CLOSEWAIT)
	return(FALSE);

    /*
     * If some other process is writing to this TCP connection, then
     * it may have satisfied our buffer wait.
     */
    if (tcb->curoutputpak && tcb->curoutputpak->length)
	return(FALSE);

    /*
     * Do a timed block if we blocked because of no buffers.
     * Give the system a chance to recover.
     */
    if (TIMER_RUNNING_AND_SLEEPING(tcb->dallytime))
	return(TRUE);

    if (tcp_usable_window(tcb)) {
	/*
	 * We had buffers, but our window was closed or output queue
	 * filled. Activate if the window becomes open and output queue
	 * drained.
	 */
	if (tcb->qoutput < QUEUEMAX)
	    return(FALSE);
    } else if (QUEUEEMPTY(&tcb->q[RETRANSQUEUE])) {
	/*
	 * Send window closed but we have nothing in the retransmission
	 * queue. Unblock so we can send zero-window probes.
	 */
	return(FALSE);
    }

    return(TRUE);
}

void tcp_idle_put (tcbtype *tcb)
{
    if (!tcb->pid) {
	edisms((blockproc *) tcpputBLOCK, (ulong) tcb);
	return;
    }

    process_watch_timer(&tcb->dallytime, ENABLE);
    while (TRUE) {
	process_wait_for_event();
	if (tcpputBLOCK(tcb))
	    continue;
	process_watch_timer(&tcb->dallytime, DISABLE);
	return;
    }
}

/*
 * tcpelementBLOCK
 * Event dismiss for TCP packet enqueue operation.  We blocked because 
 * we are out of the queuing elements. Unblock when either the queuing
 * elements are available again or TCB state changed to CLOSED.
 */
static boolean
tcpelementBLOCK (tcbtype *tcb)
{
    if (tcb->state == CLOSED)
	return(FALSE);
    return(elementBLOCK());
}


/*
 * tcp_sendurg
 * Set the most recent byte put into the buffer as last urgent byte.
 * Uses BSD definition of urgent, so make sure that in telnet sync
 * you mark the IAC, not the next byte.
 */

void tcp_sendurg (tcbtype *tcb)
{
    paktype *pak;
    int bytes;

    pak = tcb->curoutputpak;
    if (pak == NULL)
	bytes = 0;
    else
	bytes = pak->us_seglen - pak->length;
    tcb->sndup = tcb->sndnxt + bytes;
}

/*
 * tcp_putpush
 * Finish up current output buffer and initiate packet transmission.
 * Assumes that tcp_putsetup has checked window size and performed SWS
 * avoidance.  Does *not* have anything to do with the PSH bit!
 * This function may block.
 */

void tcp_putpush (tcbtype *tcb, boolean pushflag, boolean finflag)
{
    paktype *pak;
    tcptype *tcp;
    int bytes;
    int optbytes;

    pak = tcb->curoutputpak;
    /*
     * Return if there is nothing to send out, or if the packet
     * there is already being enqueued here in another context.
     */
    if (pak == NULL || (pak->tcp_flags & TCPF_ENQUEUING)) {
	return;
    }

    bytes = pak->us_seglen - pak->length;
    if ((bytes == 0) && (finflag == FALSE)) {
        return;
    }

    /*
     * Check whether we should not send the packet now due to Nagle
     * slow packet avoidance algorithms (rfc896).  Set outputpending
     * instead.  Always try to send full packets.
     */
    if ((tcb->flags & TCB_NAGLE) && (pak->length > 0) &&
		TIMER_RUNNING(tcb->timers[RETRANSTIMEOUT]) &&
		(finflag == FALSE)) {
	tcb->flags |= TCB_OUTPUTPENDING;
	return;
    }
    tcb->flags &= ~TCB_OUTPUTPENDING;

    /*
     * We have a buffer with data to send.  Remove from TCB and initialize
     * the TCP specified fields. 
     */
    if (tcp_chunksize && tcb->rcvwnd == 0)
	tcp_addwindow(tcb, 10);	/* FIXME -- what the hell's this for? */

    tcp = (tcptype *)(pak->network_start + IPHEADERBYTES(tcb->options)); 
    tcp_inithdr(tcb, tcp);
    tcp->psh = pushflag;
    tcb->sndnxt += bytes;
    if (GT(tcb->sndnxt, tcb->sndmax))
	tcb->sndmax = tcb->sndnxt;
    if (finflag) {
	tcb->sndnxt++;
	tcp->fin = TRUE;
    }

    /*
     * If we are sending urgent data, indicate so.
     */
    if (tcb->sndup != 0) {
	tcp->urgentpointer = h2nl(tcb->sndup - tcp->sequencenumber);
	tcp->urg = TRUE;
	if (tcp_seqinrange(n2hl(tcp->sequencenumber), tcb->sndup,
			   (n2hl(tcp->sequencenumber) + bytes))) {
	    tcb->sndup = 0;
	}
    } else
	tcp->urg = FALSE;

    /*
     * Adjust dataoffset if necessary.
     */
    optbytes = tcp_optionbytes(tcb, FALSE);
    if (optbytes)
	tcp->dataoffset = ((tcp->dataoffset << 2) + optbytes) >> 2;

    /*
     * Do any options.
     * Assume the space is there waiting for us.
     */
    tcp_startoptions(tcb, &tcp->data[0]);
    if (tcb->flags & TCB_MD5) {
	uchar md5[MD5_LEN];

	tcp_md5digest(md5, tcb->localhost.ip_addr, tcb->foreignhost.ip_addr,
		      TCP_PROT, bytes + TCPHEADERBYTES + optbytes,
		      tcp, &tcp->data[optbytes],
		      tcb->md5_key, tcb->md5_keylen);
	tcp_writeoption(tcb, MD5OPTION, md5, sizeof(md5));
    }
    tcp_endoptions(tcb);

    /*
     * Put packet on retransmit queue, then send it.
     * Also protect against tcp_putpush() being called by another
     * task on the same TCB in case we actually have to block here.
     * NOTE: this flag must be set before the first opportunity
     * for a task reschedule!
     */
    pak->tcp_flags |= TCPF_ENQUEUING; /* Starting to enqueue this packet */
    while (!pak_enqueue(&tcb->q[RETRANSQUEUE], pak)) {
	critical_background_wake();
	edisms((blockproc *) tcpelementBLOCK, (ulong)tcb);
	/*
	 * If we returned from block routine because the TCB is closed,
	 * probably due to receiving a RST, don't enqueue the packet
	 * and just return. The TCB will be blown away in tcp_close().
	 */
	if (tcb->state == CLOSED)
	    return;
    }
    pak->tcp_flags &= ~TCPF_ENQUEUING;	/* Done enqueuing this packet */
    tcp_retrans_sanity_check(tcb, pak);

    /*
     * Now that we have the packet on the retransmission queue, we can
     * safely clear tcb->curoutputpak. Otherwise, another process writing
     * to this TCP connection might get a new buffer, fill it, and write
     * it out before our block completed. Note that we must clear it
     * before we re-use the length and unspecified[] fields in the
     * packet.
     */

    tcb->curoutputpak = NULL;		/* Clear current packet pointer */

    pak->length = bytes + IPHEADERBYTES(tcb->options) + 
		  TCPHEADERBYTES + optbytes;
    pak->us_retransno = 0;		/* init retransmit number */
    GET_TIMESTAMP(pak->us_sendtime);	/* time the packet was sent */
    tcb->qoutput++;
    if (!TIMER_RUNNING(tcb->timers[SNDWNDTIMEOUT])) { /* don't immediately xmit probes */
	pak_lock(pak);		/* Lock the packet */
	tcp_sendip(tcb, pak, tcb->foreignhost.ip_addr);
	if (!TIMER_RUNNING(tcb->timers[RETRANSTIMEOUT])) {
	    tcp_starttimer(tcb, RETRANSTIMEOUT,
			   tcb->krtt ? tcb->krtt : tcb->rtto);
	    if (tcb->giveuptime)
		 tcp_starttimer(tcb, GIVEUPTIMEOUT, tcb->giveuptime);
       }
    }
}

/*
 * tcp_getstring
 * Get a chunk of data from the TCP input stream.  Return status code.
 * We usually return a pointer to the next TCP segment.  Care is taken
 * to separate urgent and non-urgent data.
 */

int tcp_getstring (
    tcbtype *tcb,
    uchar **datastart,
    int *datacount,
    int datamax)
{
    paktype *packet;
    tcptype *tcp;
    boolean urgentgone = FALSE;

    /* Claim to have read nothing until we actually do. */
    *datastart = NULL;
    *datacount = 0;

    /*
     * Get current input buffer.  Discard it if it has been emptied.
     * If there was an urgent boudary in that segment, we may have
     * a partial buffer.
     */
    packet = tcb->curinputpak;
    if (packet && (packet->length <= 0)) {
	datagram_done(packet);
	tcb->curinputpak = NULL;
	packet = NULL;
    }

    /*
     * If no partial buffer, attempt to get something off the segment
     * queue.  If nothing there, process the save queue as a last ditch
     * effort to scrounge up data.
     */
    if (packet == NULL) {
	packet = dequeue(&tcb->q[SEGQUEUE]);
	if (packet == NULL) {
	    if (!QUEUEEMPTY(&tcb->q[SAVEQUEUE])) {
	        tcp_processsavedsegments(tcb); 
		if (!QUEUEEMPTY(&tcb->q[SEGQUEUE]))
		    packet = dequeue(&tcb->q[SEGQUEUE]);
	    }
	    if (packet == NULL) {
	        return((tcb->state == ESTAB) ? NO_DATA : END_OF_FILE);
	    }
	}

	/*
	 * On idle connections we will sometimes discard data.  This causes
	 * the brs (byte received sequence number) variable to get out of
 	 * sync with sequence numbers.  We reset brs here and update other
	 * variables.
	 */
	tcp = tcp_get_hdr(packet, tcb);
	tcb->brs = tcp->sequencenumber;
	tcb->qinput--;
	tcb->curinputpak = packet;
	tcb->unreadbytes -= packet->length;
        tcb->delrcvwnd += packet->length;
	if (tcb->delrcvwnd >= min((tcb->maxrcvwnd/2), tcb->maxsegmentsize))
	    tcp_openwindow(tcb);
    }

    /*
     * Check for urgent data boundary in this segment.  Special processing
     * to ensure the data block returned is all normal or all urgent.
     */
    if (tcb->rcvup) {
	/*
	 * If we overlap the end of urgent data, trim off the urgent
	 * data, clear urgent pointer and pass that block to caller.
	 * If no urgent data in this packet, clear urgent pointer and
	 * fall through.
         */
	if (GEQ((tcb->brs + packet->length), tcb->rcvup)) {
	    long urgcnt;
	    urgcnt = (long) tcb->rcvup - tcb->brs;
	    if (urgcnt > 0) {
		if (datamax < urgcnt)
		    urgcnt = datamax;
		else
		    tcb->rcvup = 0;
		*datastart = (uchar *) packet->dataptr;
		packet->dataptr += urgcnt;
		*datacount = urgcnt;
		packet->length -= urgcnt;
		tcb->brs += (ulong) urgcnt;
		return(URGENT_DATA);
	    } else
		tcb->rcvup = 0;
	}
    }

    /*
     * Easy and usual case -- no urgent data boundary in this segment.
     */
    if (datamax > packet->length)
	datamax = packet->length;
    *datastart = (uchar *) packet->dataptr;
    *datacount = datamax;
    tcb->brs += (ulong) datamax;
    packet->length -= datamax;
    packet->dataptr += datamax;
    return((tcb->rcvup || urgentgone) ? URGENT_DATA : OK);
}

/*
 * tcp_getstringblock
 * Same as tcp_getstring but blocks until datamax bytes are read
 * or an error occurs.
 */

int tcp_getstringblock (tcbtype *tcb, uchar **datastart, int *datacount,
			int datamax)
{
    int status;
    int throw_away;

    while ((status = tcp_getstring(tcb, datastart, datacount, datamax))
         == NO_DATA) {
        edisms((blockproc *) tcpmiscBLOCK, (ulong) tcb);

	if (tcb->tty) {
	    if (tcp_piggybacked_tty(tcb)) {
		/*
		 * The following getc() is an attempt to get PSIPENDING flag
		 * set if the user types an escape on the vty associated with
		 * (but not owned by) this connection.
		 */
		do {
		    throw_away = getc(tcb->tty);
		} while (throw_away != -1);
	    }

	    if (psipending2(tcb->tty))
		return(LOCAL_RESET);
	}
    }

    return(status);
}

/*
 * tcp_getbyte
 * Return next byte from a TCP data stream, or -1 if none available.
 * Jacket for more general tcp_getstring() routine.
 */

int tcp_getbyte (tcbtype *tcb, int *status)
{
    uchar *ptr;
    int count;

    *status = tcp_getstring(tcb, &ptr, &count, 1);
    if ((*status == OK) || (*status == URGENT_DATA))
	return((int)*ptr);
    else
	return(-1);
}

int tcp_getbyteblock (tcbtype *tcb, int *status)
{
    int c;

    c = tcp_getbyte(tcb, status);
    while (*status == NO_DATA) {
        edisms((blockproc *)tcpmiscBLOCK, (ulong) tcb);

	if ((tcb->tty) && (psiset(tcb->tty))) {
	    *status = LOCAL_RESET;
	    return(-1);
	}

        c = tcp_getbyte(tcb, status);
    }

    return(c);
}

/*
 * tcp_activity
 * Returns TRUE if there was TCP protocol activity for this TCB
 * that warrants activating the owning process.
 *
 * The TCB wakeup flag is set on the following events:
 *	- connection changes state
 *	- new data is received
 *	- a previously closed send window opens
 */

boolean tcp_activity (tcbtype *tcb)
{
    if (tcp_unread(tcb) != 0)	 /* unread data pending */
	return(TRUE);
    return(tcp_state_activity(tcb));
}

/*
 * tcp_unread
 * Return number of unread bytes pending on a TCB.
 * Intended for very high level (protocol independent) routines.
 */

int tcp_unread (tcbtype *tcb)
{
    int bytes;
    paktype *pak;

    bytes = tcb->unreadbytes;		/* bytes on segment queue */
    pak = tcb->curinputpak;		/* current input packet */
    if (pak != NULL)			/* reading something now? */
	bytes += pak->length;		/* yes, count those bytes, too */
    return(bytes);			/* return total unread byte count */
}

/*
 * tcp_state_activity
 *
 * The TCB wakeup flag is set on the following events:
 *      - connection changes state
 *      - new data is received
 *      - a previously closed send window opens
 */
boolean tcp_state_activity (tcbtype *tcb)
{
    if (tcb->wakeup) {		/* generic change of state flag */
	tcb->wakeup = 0;	/* clear the wakeup flag */
	return(TRUE);
    }
    if (tcb->state > ESTAB)	/* we are dead or dying, seek burial */
	return(TRUE);
    return(FALSE);
}


/*
 * tcp_lsterr
 * Return last error code for the TCB
 * Intended for very high level (protocol independent) routines.
 */

int tcp_lsterr (tcbtype *tcb)
{
    return(tcb->lsterr);
}

/*
 * tcp_closing
 * Return TRUE if tcb state is not ESTAB, FALSE otherwise
 * Intended for very high level (protocol independent) routines.
 */

boolean tcp_closing (tcbtype *tcb)
{
    if (tcb->state != ESTAB) {
	if (tcb->state == CLOSED)	/* if totally shut down, quit */
	    return(TRUE);
	if (tcb->qsaved)
  	    return(FALSE);
	if (tcb->state == SYNRCVD && tcb->flags & TCB_SYNCLISTEN)
	    return(FALSE);
	if (!tcp_unread(tcb))		/* if closing, quit if no bytes left*/
	    return(TRUE);
    }
    return(FALSE);			/* Closing, but still has data */
}

/*
 * tcp_idle
 * Used to toggle the TCB_IDLEUSER flag.  When set, we will accept the first
 * datagram sent, but pretend to "read" and acknowledge succeeding datagrams.
 * Ideally we should set our window to zero and refuse further data, but too
 * many systems (4.2BSD, TOPS-20 6.1) drop such connections.
 */

void tcp_idle (tcbtype *tcb, boolean flag)
{
    if (flag == TRUE)			/* change flag state */
	tcb->flags |= TCB_IDLEUSER;
    else
	tcb->flags &= ~TCB_IDLEUSER;

    tcb->addwnd = 0;
    tcb->delrcvwnd = 0;
    if (telnet_idle0window && flag == TRUE)
	tcb->rcvwnd = 0;
    else
	tcb->rcvwnd = tcb->maxrcvwnd;
    tcp_sendacksegment(tcb);
}

/*
 * tcp_uniqueport
 * Determine a unique, non-privileged local port.
 * Insert the provided number in the low nine bits of the 16 bit port number.
 * Return 0 if no port could be allocated.
 */

ushort tcp_uniqueport (ushort ttynum)
{
    ushort port, lastport, firstport;
    tcbtype *tcb;

    if (ttynum)
	firstport = lastport = port = (tcp_portseed << 9) | ttynum;
    else	
	firstport = lastport = port = tcp_portseed;

    while (TRUE) {
	/* Check if we have wrapped around our number space. */
	if (SLT(lastport, firstport) && SGEQ(port, firstport)) {
	    /* Yikes! Out of ports. */
	    return(0);
	}

	lastport = port;

	if (ttynum)
	    port = (tcp_portseed++ << 9) | ttynum;
	else
	    port = tcp_portseed++;		/* Just try the next one. */

	if (port < MAXPRIVEDPORT) {		/* No privileged ports. */
	    if (!ttynum)
		tcp_portseed = MAXPRIVEDPORT;	/* Skip past them all. */
	    continue;
	}

	tcb = tcb_findport(port);
	if (tcb == NULL)
	    return(port);
    }
}

/*
 * tcp_reservedport
 * Pick a reserved local port number (one from 512 to 1023)
 */

boolean tcp_reservedport (ushort *localport)
{
    while (*localport >= IPPORT_RESERVED / 2) {
	if (tcb_findport(*localport) == NULL)
	    return(TRUE);
	else
	    (*localport)--;
    }
    return(FALSE);
}

/*
 * tcp_set_new_tty
 *
 * Attempt to set a new tty for a connection. This service routine is used
 * to check out a new terminal for the rotary hunt function, when the terminal
 * originally selected does not respond. If this routine returns TRUE, the
 * operation was done successfully; FALSE indicates that access checking
 * prohibits use of this terminal.
 */

boolean tcp_set_new_tty (conntype *conn, tt_soc *tty)
{
    tcbtype *tcb = (tcbtype *) conn->tcb;

    if (check_access(tty, tcb->foreignhost.ip_addr,
		     tcb->foreignport, tcb->localport, FALSE)) {
	tcb->tty = tty;
	tcb->ttynum = tty->ttynum;
	return(TRUE);
    }
    return(FALSE);
}

/*
 * tcp_putback - Mark some data read as unread
 *
 * This routine is used to mark data read as unread. This can be done
 * after a call to tcp_getstring() if not all of the data was read. Note
 * that the count must never exceed the amount of data returned in the
 * very last TCP read.
 *
 */

void tcp_putback (tcbtype *tcb, int count)
{
    if (tcb->curinputpak) {	/* Session still active, data there? */
	tcb->curinputpak->length += count; /* Update length */
	tcb->curinputpak->dataptr -= count; /* And count */
    }
}

/*
 * show_tcpbrief_guts
 *
 * Show brief information about one TCB.  Returns TRUE if
 * the caller should keep looping, FALSE otherwise.
 */

static const char show_tcpb_header[] =
    "TCB       Local Address           Foreign Address        (state)";

static
boolean show_tcpbrief_guts (tcbtype *tcb, boolean all, boolean *hdr_set)
{
    boolean newstyle;

    if (automore_quit())
	return(FALSE);

    newstyle = (tcb->internal_flags & TCB_INT_PRIMITIVES);

    if (newstyle)
	mem_lock(tcb);
	   
    /*
     * If this TCB has a handle and it's a child waiting to
     * be dequeued from its parent handle (by tcp_accept()),
     * then don't show it since we'll show it in the loop
     * below when we hit the parent.
     */
    if (all && newstyle && (tcb->internal_flags & TCB_INT_WAITING) &&
	LIST_ELEMENT_QUEUED(&tcb->connq_element)) {
	free(tcb);	/* un-mem_lock() */
	return(TRUE);
    }

    if (!*hdr_set) {
	automore_header(show_tcpb_header);
	*hdr_set = TRUE;
    }

    tcp_status_brief(tcb);

    if (newstyle && LIST_SIZE(&tcb->connq)) {
	list_element *el;
	tcbtype *tcbtemp;

	/*
	 * Use this loop instead of FOR_ALL_DATA_IN_LIST() to guard
	 * against the possibility of list element being removed
	 * while the display is paused at the --More-- prompt.
	 */
	el = LIST_HEAD_ELEMENT(&tcb->connq);
	while (el) {
	    if (automore_quit()) {
		free(tcb);	/* un-mem_lock() */
		return(FALSE);
	    }

	    tcbtemp = LIST_GET_DATA(el);
	    mem_lock(tcbtemp);
	    tcp_status_brief(tcbtemp);
	    el = LIST_NEXT_ELEMENT(el);
	    free(tcbtemp);	/* un-mem_lock() */
	}
    }
    if (newstyle)
	free(tcb);	/* un-mem_lock() */
    return(TRUE);
}

/*
 * show_tcpbrief
 *
 * Show brief information about all TCBs a la netstat.
 */

static
void show_tcpbrief (boolean all)
{
    int i, j;
    tcbtype *tcb;
    boolean hdr_set;

    automore_enable(NULL);
    hdr_set = FALSE;
    for (i = 0; i < TCBHASHSIZE; i++) {
	for (j = 0; (tcb = queuepick(&tcbhashtable[i], j)); j++) {
	    if (!show_tcpbrief_guts(tcb, all, &hdr_set))
		break;
	}
    }

    if (all) {
	for (j = 0; (tcb = queuepick(&tcbwildtable, j)); j++) {
	    if (!show_tcpbrief_guts(tcb, all, &hdr_set))
		break;
	}
    }
    automore_disable();
}

/*
 * show_tcptcb
 * Show TCP info by TCB address.
 */
static
void show_tcptcb (tcbtype *tcb)
{
    tcbtype *tmp_tcb;
    int i;
 
    for (i = 0; i < TCBHASHSIZE; i++) {
	for (tmp_tcb = tcbhashtable[i]; tmp_tcb; tmp_tcb = tmp_tcb->next) {
	    if (tmp_tcb == tcb) {
	 	tcp_status(tcb);
		return;
	    }
	}
    }	

    printf("\nTCB 0x%x not found", tcb);
}


/*
 * show_tcpstatus
 * Display all the TCB's for the specified lines, or all lines.
 */

void show_tcpstatus (parseinfo *csb)
{
    int i, j, line;
    tt_soc *tty;
    conntype *conn;
    int firstline, lastline;
    tcbtype *tcb;

    if (GETOBJ(int, 2)) {
	show_tcpbrief(GETOBJ(int, 3));
	return;
    }

    tcb = (tcbtype *)(GETOBJ(int, 4));
    if (tcb) {
	show_tcptcb(tcb);
	return;
    }

    line = GETOBJ(int,1);
    automore_enable(NULL);
    if (tcp_debug)
	printf("\n tcp block reasons:  0 window %u, queue full %u, no buf %u",
	       tcp_0window, tcp_queuefull, tcp_nobuffer);
    if (line == -1) {
	firstline = 0;
	lastline = ALLlines;
    } else {
	firstline = line;
	lastline = line + 1;
    }
    if (line == -1) {
	for (i = 0; i < TCBHASHSIZE; i++) {
 	    for (j = 0;
		 (tcb = queuepick(&tcbhashtable[i], j));
		 j++) {
		if (tcb->tty == NULL || tcp_piggybacked_tty(tcb)) {
		    mem_lock(tcb);
		    printf("\n\nStand-alone TCP connection %s host %s",
			   (tcb->flags & TCB_PASSIVEOPEN) ? "from" : "to",
			   address_string(&tcb->foreignhost));
		    tcp_status(tcb);
		    free(tcb);	/* un-mem_lock() */
		}
	    }
	}
    }
		
    for (i = firstline; i < lastline; i++) {
	if (automore_quit())
	    break;
	tty = MODEMS[i];
	if (tty == NULL)
	    continue;
	conn = tty->conntop;
	if (conn && conn->tcb && (conn->v == &tcp_connvector)) {
	    mem_lock(conn);
	    mem_lock(conn->tcb);
	    printf("\n\ntty%t, virtual tty from host ",i);
	    (void) print_host(conn);
	    tcp_status((tcbtype *)conn->tcb);
	    free(conn->tcb);	/* un-mem_lock() */
	    free(conn);		/* un-mem_lock() */
	}
	for (j = 0; (conn = queuepick(&tty->connhead, j)); j++) { 
	    if (automore_quit())
		break;
	    if (conn->tcb && conn->address.ip_addr) {
 		if (!validmem(conn) || !validmem(conn->tcb) ||
 		    !validlink(conn->link))
 		    return;
		mem_lock(conn);
		mem_lock(conn->tcb);
 		printf("\n\ntty%t",i);
 		if (get_tty_location(tty))
 		    printf(" (%s)", get_tty_location(tty));
 		printf(", connection %d to host ",conn->number);
 		(void) print_host(conn);
 		tcp_status((tcbtype *)conn->tcb);
		free(conn->tcb);	/* un-mem_lock() */
		free(conn);		/* un-mem_lock() */
	    }
	}
	reg_invoke_xremote_show_tcp(tty);
    }
    automore_disable();
}

/*
 * tcp_create
 * Create a TCB
 */

tcbtype *tcp_create (tt_soc *tty, int *error, ipoptiontype *ipopts)
{
    tcbtype *tcb;

    if (!tcp_ready) {
	errmsg(&msgsym(NOTREADY, TCP), "tcp_create");
	*error = ABORTED;
	return(NULL);
    }

    /*
     * Create and initialize the TCB.
     */
    tcb = tcp_allocatetcb();
    if (tcb == NULL) {
	*error = NO_MEMORY;
	return(NULL);
    }

    /*
     * Set up some specific parameters for this connection.
     * Remember tty pointer if we're associated with a terminal line.
     */
    if (tty) {
	tcb->tty = tty;
	tcb->ttynum = tt2number(tty);
    }
    tcb->options = ipopts;

    *error = OK;	/* be optimistic */

#ifdef NOTYET
    /* Nagle processing should be on by default. */
    tcb->flags |= TCB_NAGLE;
#endif
    tcb->internal_flags |= TCB_INT_PRIMITIVES;

    if (tcp_debug)
	buginf("\nTCB%08lx created", tcb);

    return(tcb);
}

/*
 * tcp_bind
 * Bind a local address and port to a TCB
 */

int tcp_bind (tcbtype *tcb, addrtype *lhost, int lport)
{
    tcbtype *tcbx;

    if (tcb == NULL)
	return(ABORTED);

    /*
     * If a local port was specified look to see if such a tcb already exists.
     * If so abort with a reasonable error.
     */
    if (lport != 0) {
	if (tcb->foreignport) {
	    int i;

	    for (i = 0; i < TCBHASHSIZE; i++) {
		for (tcbx = tcbhashtable[i]; tcbx; tcbx = tcbx->next) {
		    if (tcbx->state != CLOSED && tcbx->state != TIMEWAIT &&
			tcbx->localport == lport &&
			ip_match_address(&tcbx->foreignhost,
					   &tcb->foreignhost))
			return(INUSE);
		}
	    }
	} else {
	    for (tcbx = tcbwildtable; tcbx; tcbx = tcbx->next) {
		if (tcbx->localport == lport) {
		    if (tcbx->foreignhost.type != ADDR_ILLEGAL) {
			if (ip_match_address(&tcbx->foreignhost,
					       &tcb->foreignhost))
			    return(INUSE);
		    } else
			return(INUSE);
		} else if (lport == -1 && tcbx->localport == 0) {
		    return(INUSE);
		}
	    } 
	}
    }

    /*
     * This is a bit of a hack that relies on TCP ports being
     * 16-bit values.  Only tcp_protocols() should be passing
     * a value of -1 for lport as it is the only "very wild"
     * listener in the system.
     */
    if (lport == -1)
	tcb->localport = 0;	/* very wild listener */
    else {
	tcb->localport = lport ? lport : tcp_uniqueport(0);
	if (tcb->localport == 0)
	    return(INUSE);
    }
    if (lhost)
	address_copy(&tcb->localhost, lhost);

    if (tcp_debug)
	buginf("\nTCB%08lx bound to %s.%d", tcb,
	       address_string(&tcb->localhost), tcb->localport);

    return(OK);
}

/*
 * tcp_connect
 * Connect a TCB to a foreign address and port.
 */

int tcp_connect (tcbtype *tcb, addrtype *fhost, int fport)
{
    ulong iss;
    idbtype *idb;
    ipoptiontype *saved_opts;
    ipoptiontype *ipopts;
    int status;

    if (tcb == NULL)
	return(ABORTED);

    if (tcb->tty && tcp_keepalivesout)
	tcb->flags |= TCB_KEEPALIVE;

    /*
     * Check if we have a chance of making a connection.
     */
    idb = NULL;				/* don't yet know probable interface */
    ipopts = tcb->options;

    if (!ipopts) {
	ipaddrtype dummy = fhost->ip_addr;	/* don't clobber fhost */

	idb = reg_invoke_ip_first_hop(&dummy, dummy, IP_PATH_DEFFLAGS);
	if (idb == NULL)
	    return(UNREACHABLE);
    } else if (ip_secopt_bytes &&
	!ipopt_findopt(ipopts->ipopts, IPOPT_BASSEC, ipopts->length)) {
	/*
	 * We are secured and we have options, but we do not have a basic
	 * security option.  Check to see if we have enough space for the
	 * default option insert.
	 */
	if (ipopts->length + SECURITY_SIZE > MAXIPOPTIONBYTES) {
	    return(OPTSTOOBIG);
	} else {
	    uchar *src, *dst;
	    int i;

	    /*
	     * Size is just fine.  Insert the default security
	     * option.  This option is BASSEC,SECURITY_SIZE,SECINSERT,0 and
	     * is not likely to change.  (SECURITY_SIZE is 4).
	     *
	     * To be compatible with BFEs we have to put the security
	     * option at the very front.  This means we have to ripple
	     * the current option to the back to make room.
	     */
	    src = &ipopts->ipopts[ipopts->length - 1];
	    dst = &ipopts->ipopts[ipopts->length - 1 + SECURITY_SIZE];
	    for (i = ipopts->length; i > 0; i--)
		*dst-- = *src--;
	    ip_add_bso_opt(ipopts);
	    dst = &ipopts->ipopts[ipopts->length];
	    for (i = ipopts->paddedlength - ipopts->length; i; i--)
		*dst++ = PADDING;
	}
    }

    /* Set up the foreign host and port.
     */
    address_copy(&tcb->foreignhost, fhost);
    tcb->foreignport = fport;

    /* Set up the local host and port if they haven't already
     * been specified.
     */
    if (tcb->localhost.type == ADDR_ILLEGAL) {
	tcb->localhost.ip_addr = ip_best_local_address(fhost->ip_addr, FALSE);
	tcb->localhost.type = ADDR_IP;
    }
    if (tcb->localport == 0) {
	tcb->localport = tcp_uniqueport(0);
	if (tcb->localport == 0)
	    return(INUSE);
    }

    /*
     * Revise guess of SRTT if we're using a slow interface (< 100 Kbit).
     * Our default guess is good enough for fast LAN's.
     */
    if (idb && idb->visible_bandwidth < 100) {
	tcb->srtt = TCP_SLOWRTT;
	tcb->rtto = TCP_SLOWRTO;
    }

    /*
     * Enqueue TCB, pick an Initial Sequence Number, send the original SYN.
     */
#ifdef TCPVERBOSE
    if (tcp_debug)
	buginf("\nTCP: opening %s(%d) -> %s(%d)", 
	       address_string(&tcb->localhost), tcb->localport, 
	       address_string(&tcb->foreignhost), tcb->foreignport);
#endif
    tcb_enqueue(tcb);
    iss = tcp_selectiss();
    status = tcp_sendsynsegment(tcb, iss, FALSE, 0L);
    if (status != OK) {
	/*
	 * The SYN packet wasn't sent out. Propagate the error 
	 * back to caller.
	 */
	return(status);
    }
    tcp_changestate(tcb, SYNSENT);

    /*
     * Block until something interesting happens.
     */
    if (tcb->tty)
	psion(tcb->tty);		/* allow interrupts */
    tcp_idle_open(tcb);			/* wait for connection to open */
    if (tcb->state == ESTAB) {		/* if ESTAB, then we win! */
	if (tcb->tty)
	    psioff(tcb->tty);		/* disallow interrupts */
	if (tcp_debug)
	    buginf("\nTCB%08lx connected to %s.%d", tcb,
		   address_string(&tcb->foreignhost), tcb->foreignport);
	return(OK);			/* return okey-dokey */
    }
    if (tcb->tty) {
	if (psiset(tcb->tty) || (tcb->tty->statbits & CARDROP))
	    tcb->lsterr = LOCAL_RESET;	/* user aborted the open */
	psioff(tcb->tty);		/* disallow interrupts */
	if (tcb->curinputpak)
	    ttyprintf(tcb->tty, "%*s... ", tcb->curinputpak->length,
		      tcb->curinputpak->dataptr);
    }

    /*
     * TCB cleanup normally deletes the IP options that
     * the application allocated and gave to TCP to dispense with.
     * This is the one case where we want to hold on to the options.
     * Ugly but effective.
     */
    saved_opts = tcb->options;
    tcb->options = NULL;
    tcp_reset(tcb);
    tcb->options = saved_opts;
    return(tcb->lsterr);		/* return last error to caller */
}

/*
 * tcp_get_next_parent
 *
 * Get the next parent with 1 or more connections pending.
 * If there is only one connection left, remove the parent
 * from the global list.  Make an attempt to be fair by
 * moving the parent to the end of the list.
 * The linear search through the group list is not too good.
 *
 * It also might be nice to thread incoming connections for a
 * particular group in the order they arrive.  This would require
 * there to be a back-pointer in the TCB to the group containing it
 * (which would disallow having a TCB in multiple groups).
 */

static tcbtype *
tcp_get_next_parent (struct tcp_listen_group *m)
{
    int i;
    tcbtype *tcb;
    list_element *el;

    FOR_ALL_DATA_IN_LIST(&tcp_newconn_list, el, tcb) {
	for (i = 0; i < m->count; i++) {
	    if (m->tcbs[i] == tcb) {
		list_remove(&tcp_newconn_list, &tcb->newconn_element, NULL);
		list_enqueue(&tcp_newconn_list, &tcb->newconn_element, tcb);
		return(tcb);
	    }
	}
    }
    return(NULL);
}

/*
 * tcp_listener
 * Service task to provide context in which to call the accept function
 * for users who don't want or need another task.
 */

static process
tcp_listener (void)
{
    tcbtype *tcb, *ntcb;
    int size, error;

    while (1) {
	process_wait_for_event();

	while ((tcb = tcp_get_next_parent(&tcp_listener_group))) {
	    /* Check if a connection is pending and call the callback. */
	    if (tcp_debug)
		buginf("\nTCB%08lx callback", tcb);

	    size = LIST_SIZE(&tcb->connq);
	    blocking_disabled = TRUE;	/* blocking is not allowed */
	    (*tcb->listencb)(tcb);
	    blocking_disabled = FALSE;	/* blocking is allowed */
	    if (LIST_SIZE(&tcb->connq) == size) {
		/*
		 * Callback didn't accept the connection!
		 * Accept for it, throwing the new connection away
		 * so this process will eventually relinquish
		 * the processor.
		 */
		ntcb = tcp_accept(tcb, &error);
		if (ntcb)
		    errmsg(&msgsym(ACCEPT, TCP), ntcb->foreignhost.ip_addr,
			   ntcb->foreignport, ntcb->localhost.ip_addr,
			   ntcb->localport);
	    }
	}
    }
}

/*
 * tcp_listen
 * Allow incoming connections for a TCB.
 */

int tcp_listen (tcbtype *tcb, int qlimit, tcp_listen_cb func)
{
    if (tcb == NULL)
	return(ABORTED);

    if (tcb->tty && tcp_keepalivesin)
	tcb->flags |= TCB_KEEPALIVE;

    /* Set up the TCB to listen for incoming connections. */
    tcb->state = LISTEN;		/* we are listening */
    tcb->events |= tcp_set_events[LISTEN];
    tcb->events &= tcp_clear_events[LISTEN];
    tcb->flags &= ~TCB_ACTIVE_OPEN_FLAGS;
    tcb->flags |= TCB_PASSIVE_OPEN_FLAGS | TCB_GENTCBS;
    tcb->connq.maximum = min(qlimit, TCP_MAXCONNQLIMIT);
    if (func) {
	int error;

	tcb->listencb = func;
	error = tcp_add_listen_tcb(tcb);
	if (error != OK)
	    return(error);

	if (!tcp_listener_pid) {
	    if (tcp_debug)
		buginf("\nTCP: starting TCP Listener process");
	    tcp_listener_pid = process_create(tcp_listener, "TCP Listener",
					      NORMAL_STACK, PRIO_LOW);
	}
	tcb->pid = tcp_listener_pid;
    }
    tcb_enqueue(tcb);

    if (tcp_debug)
	buginf("\nTCB%08lx listening with queue %d", tcb, tcb->connq.maximum);

    return(OK);
}

/*
 * Accept functions
 */

static
boolean tcpacceptBLOCK (tcbtype *tcb)
{
    if (tcb->tty && psiset(tcb->tty))
	return(FALSE);		/* user aborted the accept */
    if (LIST_SIZE(&tcb->connq) > 0)
	return(FALSE);		/* connection to accept */
    return(TRUE);
}

static void
tcp_idle_accept (tcbtype *tcb)
{
    if (!tcb->pid) {
	edisms((blockproc *) tcpacceptBLOCK, (ulong) tcb);
	return;
    }

    while (TRUE) {
	process_wait_for_event();
	if (tcpacceptBLOCK(tcb))
	    continue;
	return;
    }
}

/*
 * tcp_accept_common
 */

static
tcbtype *tcp_accept_common (tcbtype *tcb, int *error)
{
    idbtype *idb;
    tcbtype *newtcb;
    ipaddrtype dummy;

    /* Dequeue the new TCB and mark it as not waiting anymore. */
    newtcb = list_dequeue(&tcb->connq);
    if (newtcb == NULL)
	return(NULL);
    if (LIST_SIZE(&tcb->connq) == 0)
	list_remove(&tcp_newconn_list, &tcb->newconn_element, NULL);
    newtcb->internal_flags &= ~TCB_INT_WAITING;
    if (tcb->listencb) {
	/*
	 * The tcp_listener process owns the parent TCB, so reset the
	 * pid of the child.  Whatever is picking up the child TCB
	 * can tcp_set_property(tcb, TCP_PID, ...) if it wants to use
	 * the new scheduler.
	 */
	newtcb->pid = 0;
    }

    /* adjust rtt estimates */
    dummy = newtcb->foreignhost.ip_addr;

    idb = reg_invoke_ip_first_hop(&dummy, dummy, IP_PATH_DEFFLAGS);
    if (idb && idb->visible_bandwidth < 100) {
	newtcb->srtt = TCP_SLOWRTT;
	newtcb->rtto = TCP_SLOWRTO;
    }
    /* Been a while since tcp_allocatetcb() */
    GET_TIMESTAMP(newtcb->starttime);

    if (tcp_debug)
	buginf("\nTCB%08lx accepting %08lx from %s.%d", newtcb->parent, newtcb,
	       address_string(&newtcb->foreignhost), newtcb->foreignport);

    return(newtcb);
}

/*
 * tcp_accept
 * Accept a pending incoming connection on a TCB.
 */

tcbtype *tcp_accept (tcbtype *tcb, int *error)
{
    if (tcb == NULL) {
	*error = ABORTED;
	return(NULL);
    }

    /* Don't block unless we have to. */
    if (LIST_SIZE(&tcb->connq) == 0) {
	/*
	 * Don't block if this TCB has a listen callback.
	 */
	if (tcb->listencb)
	    return(NULL);

	/* Wait for other side to SYN */
	if (tcb->tty)
	    psion(tcb->tty);		/* allow interrupts */
	tcp_idle_accept(tcb);
	if (tcb->tty) {
	    int ubreak = psiset(tcb->tty);

	    psioff(tcb->tty);
	    if (ubreak) {
		tcb->lsterr = *error = LOCAL_RESET;
		return(NULL);		/* user aborted the accept */
	    }
	}
    }

    return(tcp_accept_common(tcb, error));
}

/*
 * tcp_accept_one
 * Accept a pending incoming connection on one of a set of TCBs.
 */

tcbtype *tcp_accept_one (tcbtype **tcbs, int count, int *error)
{
    struct tcp_listen_group tcp_listen_group;
    tcbtype *tcb;

    if (tcbs == NULL) {
	*error = ABORTED;
	return (NULL);
    }

    tcp_listen_group.tcbs = tcbs;
    tcp_listen_group.count = count;

    /*
     * Look for a queued connection before sleeping.
     */
    while ((tcb = tcp_get_next_parent(&tcp_listen_group)) == NULL)
	process_wait_for_event();

    return(tcp_accept_common(tcb, error));
}

/*
 * tcp_read
 * Read bytes from a TCB.
 */

int tcp_read (tcbtype *tcb, void *buf, unsigned int n, int *error)
{
    uchar *tcpbuf = NULL;
    int nbyte = 0;
    int result;
    int total;
    char *cbuf = buf;

    if (tcb == NULL) {
	*error = ABORTED;
	return(-1);
    }

    /* Block until data arrives or an error occurs. */
    while ((result = tcp_getstring(tcb, &tcpbuf, &nbyte, n)) == NO_DATA)
	edisms((blockproc *) tcpmiscBLOCK, (ulong) tcb);
    total = nbyte;

    switch (result) {
    case OK:
    case URGENT_DATA:
	/* Copy the first load of data. */
	if (cbuf) {
	    bcopy(tcpbuf, cbuf, nbyte);
	    cbuf += nbyte;
	}

	/* Continue reading if caller wants more data. */
	if (nbyte < n) {
	    n -= nbyte;
	    do {
		result = tcp_getstring(tcb, &tcpbuf, &nbyte, n);
		if (result == OK || result == URGENT_DATA) {
		    if (cbuf) {
			bcopy(tcpbuf, cbuf, nbyte);
			cbuf += nbyte;
		    }
		    total += nbyte;
		    n -= nbyte;
		}
	    } while (n && (result == OK || result == URGENT_DATA));
	}
	*error = result;
	return(total);
    case END_OF_FILE:
	*error = ABORTED;
	return(0);
    default:
	*error = result;
	return(-1);
    }
}

/*
 * tcp_write
 * Write bytes to a TCB.
 */

int tcp_write (tcbtype *tcb, void *buf, unsigned int n, int *error)
{
    int written;
    int to_write;
    char *cbuf = buf;

    if (tcb == NULL) {
	*error = ABORTED;
	return(-1);
    }

    /* Connection is not open anymore, so don't even try to write. */
    if ((tcb->state != ESTAB) && (tcb->state != CLOSEWAIT)) {
	*error = tcb->lsterr;
	return(-1);
    }

    *error = OK;

    to_write = n;
    while (to_write) {
	written = tcp_putstring(tcb, cbuf, to_write);
	if (written < to_write)
	    tcp_idle_put(tcb);
	to_write -= written;
	cbuf += written;
	if (tcb->state != ESTAB && tcb->state != CLOSEWAIT) {
	    *error = tcb->lsterr;
	    return(-1);		/* return (n - to_write) ? */
	}
    }

    tcp_putpush(tcb, FALSE, FALSE);
    return(n);
}

/*
 * tcp_set_property
 * Set window size, etc. a la setsockopt.
 */

int tcp_set_property (tcbtype *tcb, int prop, void *value, int valuelen)
{
    if (tcp_debug)
	buginf("\nTCB%08lx setting property %d %x", tcb, prop, value);

    if (tcb == NULL)
	return(ABORTED);

    switch (prop) {
    case TCP_WINDOW_SIZE:
	tcp_set_rcvwnd(tcb, *(ulong *) value);
	break;
#ifdef NOTYET
    case TCP_NO_DELAY:
	if (*(int *) value)
	    tcb->flags |= TCB_NAGLE;
	else
	    tcb->flags &= ~TCB_NAGLE;
	break;
#endif
    case TCP_COND_ACCEPT:
	if (*(int *) value)
	    tcb->flags |= TCB_SYNCLISTEN;
	else
	    tcb->flags &= ~TCB_SYNCLISTEN;
	break;
    case TCP_TTY:
	tcb->tty = *(tt_soc **) value;
	tcb->ttynum = tt2number(*(tt_soc **) value);
	break;
    case TCP_IPOPTIONS:
	tcb->options = *(ipoptiontype **) value;
	break;
    case TCP_MD5KEY:
	tcp_md5on(tcb, (char *) value, valuelen);
	break;
    case TCP_ACCESS_CHECK:
	tcb->access_check = (boolean (*)(ulong, ulong, ipaddrtype)) value;
	break;
    case TCP_PID:
	tcb->pid = *(pid_t *) value;
	break;
    case TCP_MSG_NOTIFY:
	if (*(ulong *) value)
	    tcb->internal_flags |= TCB_INT_MSG_NOTIFY;
	else
	    tcb->internal_flags &= ~TCB_INT_MSG_NOTIFY;
	break;
    case TCP_SYNWAIT:
	tcb->tcb_synwait = *(int *) value;
	break;
    default:
	return(ABORTED);
    }

    return(OK);
}

/*
 * tcp_get_property
 * Get window size, etc. a la getsockopt.
 */

int tcp_get_property (tcbtype *tcb, int prop, void *value, int *valuelen)
{
    if (tcb == NULL)
	return(ABORTED);

    switch (prop) {
    case TCP_WINDOW_SIZE:
	*(ulong *) value = tcb->rcvwnd;
	*valuelen = sizeof(tcb->rcvwnd);
	break;
#ifdef NOTYET
    case TCP_NO_DELAY:
	*(int *) value = (tcb->flags & TCB_NAGLE) ? 1 : 0;
	*valuelen = sizeof(int);
	break;
#endif
    case TCP_COND_ACCEPT:
	*(int *) value = (tcb->flags & TCB_SYNCLISTEN) ? 1 : 0;
	*valuelen = sizeof(int);
	break;
    case TCP_TTY:
	*(tt_soc **) value = tcb->tty;
	*valuelen = sizeof(tcb->tty);
	break;
    case TCP_IPOPTIONS:
	*(ipoptiontype **) value = tcb->options;
	*valuelen = sizeof(tcb->options);
	break;
    case TCP_ACCESS_CHECK:
	*(boolean (**)(ulong, ulong, ipaddrtype)) value = tcb->access_check;
	break;
    case TCP_PID:
	*(pid_t *) value = tcb->pid;
	*valuelen = sizeof(tcb->pid);
	break;
    case TCP_MSG_NOTIFY:
	*(ulong *) value = (tcb->internal_flags & TCB_INT_MSG_NOTIFY) ? 1 : 0;
	*valuelen = sizeof(ulong);
	break;
    case TCP_SYNWAIT:
	*(int *) value = tcb->tcb_synwait;
	*valuelen = sizeof(tcb->tcb_synwait);
	break;
    default:
	return(ABORTED);
    }

    return(OK);
}
