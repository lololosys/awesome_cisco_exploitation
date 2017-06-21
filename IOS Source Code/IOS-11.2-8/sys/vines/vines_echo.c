/* $Id: vines_echo.c,v 3.3.18.3 1996/07/24 00:56:17 slin Exp $
 * $Source: /release/112/cvs/Xsys/vines/vines_echo.c,v $
 *------------------------------------------------------------------
 * vines_echo.c -- handle Banyan Vines echo and trace functions
 *
 * June 1990, David R. Hampton
 *
 * Copyright (c) 1990-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: vines_echo.c,v $
 * Revision 3.3.18.3  1996/07/24  00:56:17  slin
 * CSCdi63865:  Rework obsolete os call in vines
 * Branch: California_branch
 *
 * Revision 3.3.18.2  1996/05/11  16:52:12  thille
 * CSCdi51398:  Duplicate strings in ping / trace
 * Branch: California_branch
 * Every 116 bytes counts.
 *
 * Revision 3.3.18.1  1996/03/18  22:31:41  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.2  1996/03/07  11:04:25  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.2.26.1  1996/02/20  21:46:24  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.3  1996/02/08  20:13:50  thille
 * CSCdi48347: format string in inlines, code size impact
 * Pass 1 - Clean up some literal strings in inlines, duplicate literals,
 * etc.  11,356 bytes saved on gs7-j-m, 9336 bytes saved on igs-j-l
 *
 * Revision 3.2  1995/11/17  17:57:43  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:44:36  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/07/07  20:39:59  hampton
 * Minor vines timer cleanup. [CSCdi36825]
 *
 * Revision 2.2  1995/06/09 13:25:03  hampton
 * Rename old scheduler calls to fit the new naming convention.
 * [CSCdi35514]
 *
 * Revision 2.1  1995/06/07 23:14:17  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <string.h>
#include <ctype.h>
#undef   toupper			/* use library function call */
#include "interface_private.h"
#include "packet.h"
#include "ttysrv.h"
#include "../ui/command.h"
#include "config.h"
#include "vines_private.h"
#include "vines_ipc.h"
#include "vines_netrpc.h"
#include "vines_services.h"
#include "vsvc_nsm.h"
#include "vsvc_nsm.rpch"
#include "subsys.h"
#include "../snmp/sr_pingmib.h"
#include "../snmp/snmp_registry.h"
#include "../ui/common_strings.h"

static int snmp_vines_echo_validate (ciscoPingEntry_t *entry);
static void snmp_vines_echo_activate (ciscoPingEntry_t *entry);

#define MAXLEN 100
#define VINES_MINECHOSIZE (sizeof(vinesiptype) + sizeof(vinesipctype))

queuetype vinesechoQ;		/* VINES protocol level echo queue */
queuetype vinestraceQ;		/* VINES protocol level trace queue */
queuetype banyantraceQ;		/* Banyan's Application level trace */


/**********************************************************************
 *
 *			     PING SUPPORT
 *
 **********************************************************************/

/*
 * vines_ping_parse
 * Parse a ping command line.  Called by SERVICE_PING_PARSE.
 * Return TRUE if positive identification of a Vines address.
 */

boolean vines_ping_parse (char *args, int hint, int *index,
			  char **cp, boolean *running)
{
    ulong net;
    ushort host;

    *running = vines_running;
    *cp = "Vines";
    *index = LINK_VINES;

    if(!vines_running)
	return(FALSE);

    if (hint < 0) {
	if (strmatch(args, "oldvines", FALSE)) {
	    *index = LINK_VINES_ECHO;
	    return(TRUE);
	}
	return(strmatch(args, "vines", FALSE));
    } else if (hint == LINK_ILLEGAL) {
	if (parse_vinesaddress(args, &net, &host))
	    return(TRUE);
    } else if ((hint == LINK_VINES) &&
	       (!args || !*args || parse_vinesaddress(args, &net, &host)))
	return(TRUE);
    return(FALSE);
}

/*
 * vines_ping
 *
 * Send a Vines IPC packet to port 1 of the destination system.  Look
 * for the returned packet from a cisco router, or for the IPC error
 * message from a Banyan Vines system.  This port was used for an
 * early echo protocol by Banyan, but has long been discontinued.
 * Supply all of the "standard" cisco ping options.
 */
void vines_ping (char *args)
{
    ulong net;
    ushort host;
    long size, repeat, timeout, hits, pacing;
    register long i;
    boolean verbose;
    long tmin = 999999999, tmax = 0, tsum = 0;
    char buff[MAXLEN];
    vinesechotype data;
    vinesipcsocket *socket;
    vipc_port_info *port;
    sys_timestamp pacing_timer;

    size = 100;				/* default datagram size */
    repeat = 5;				/* default repeat count */
    timeout = 2;			/* default timeout */
    pacing = 0;				/* default pacing */
    verbose = FALSE;			/* not verbose */

    /*
     * Get Vines address
     */
    if (!parse_vinesaddress(args, &net, &host)) {
	printf("Target Vines address: ");
	if (!rdtty(buff, MAXLEN) || !parse_vinesaddress(buff, &net, &host)) {
	    printf("\n%% Bad Vines address or host name");
	    return;
	}
	
	/*
	 * Ask for repeat count, size, and timeout interval
	 */
	if (!ping_common(&repeat, &size, (long)VINES_MINECHOSIZE,
			 (long)VINES_MAXSIZE, &timeout))
	    return;
	
	pacing = get_num_in_range("Pacing interval in seconds (0=no pacing)", 10, 0,
				  3600, pacing);

	if (pacing && (pacing < timeout)) {
	    printf("\nThe pacing interval cannot be less than the timeout interval.");
	    return;
	}

	/*
	 * Ask for verbosity
	 */
	printf("\nVerbose [n]: ");
	if (rdtty(buff, MAXLEN) && toupper(*buff) == 'Y') 
	    verbose = TRUE;
    }

    /*
     * Allocate a port for sourceing pings.
     */
    port = vipc_create_port(0, "Exec Ping", FALSE, vines_ipc_echo_sink);
    data.port = port->number;

    /*
     * Go for it.
     */
    psion(stdio);
    printf(type_escape_seq_abort_msg);
    printf(ping_sending_msg, repeat, size, vines_Vines_str,
	   common_str_empty_str);
    printf("%#z", net, host);
    printf(ping_timeout_msg, timeout);
    flush();
    for (i = 0, hits = 0; (i < repeat) && (psipending() == FALSE); i++) {
	if ((i % 70) == 0 && !verbose) {
	    printc('\n');
	    flush();
	}

	/*
	 * Create a socket structre or fail miserably.  This needs to be
	 * inside the for loop so that it will ask for the socket for
	 * each packet.  This allows the echo code to recover from a
	 * disconnect that is received on the socket it is currently
	 * using.  It also guatantees that if we have to kill a socket
	 * because of a lost packet, we will create a new one for the
	 * next ping.
	 */
	socket = vipc_create_conn(net, host, 0, 0, TRUE);
	if (!socket) {
	    printf("\nCannot allocate memory for socket structure.");
	    return;
	}

	TIMER_START(data.timeout, timeout * ONESEC);
	TIMER_START(pacing_timer, pacing * ONESEC);
	if (vines_echo_send(&socket, &data, size)) {
	    switch(data.how) {
	    case '!':		/* good reply */
		hits++;
		/*
		 * If we got a reply before the clocked ticked, turn a zero
		 * into a one to avoid dumb questions from customers.
		 */
		if (data.deltatime == 0L)
		    data.deltatime = 1L;
		tsum += data.deltatime;
		tmin = min(tmin, data.deltatime);
		tmax = max(tmax, data.deltatime);
		if (verbose)
		    printf("\n%d in %d ms", i, data.deltatime);
		break;
	    default:
		if (verbose)
		    printf("\nunknown packet type %#x", data.how);
		else
		    data.how = '?';
		break;
	    }
	    if (!verbose)
		printc(data.how);
        } else {
	    boolean timed_out = AWAKE(data.timeout);
	    if (!verbose) {
		printc('.');
	    } else {
	      printf("\n%d %s", i,
		     timed_out ? "timed out" : "send failed");
	    }
	    if (!timed_out) {
	      process_sleep_for(TIME_LEFT_SLEEPING(data.timeout));
	    }

	    /*
	     * Kill the connection so that we don't get into any problems
	     * with packets being out of sequence now that one has been
	     * lost.
	     */
	    if (socket)
		vipc_kill_conn(socket, "ping", TRUE);
	}
	flush();
	
	/*
	 * We will only see PSIPENDING if we do this
	 */
	while (getc(stdio) != -1) {
	  /* Nothing */
	}
	if (!psipending()) {
	    process_sleep_for(TIME_LEFT_SLEEPING(pacing_timer));
	}
    }
    vipc_destroy_port(port);
    if (i > 0) {
	printf("\nSuccess rate is %d percent (%d/%d)",
	    (hits*100)/i, hits, i);
	if (hits) {
	    /*
	     * If we got a reply before the clocked ticked, turn a zero
	     * into a one to avoid dumb questions from customers.
	     */
	    if (tmin == 0)
		tmin = 1;
	    printf(", round-trip min/avg/max = %d/%d/%d ms",
		tmin, tsum/hits, tmax);
	}
    }
    psioff(stdio);
}

/*
 * vines_echo_send
 *
 * Send a Vines echo to a host and block for a reply.  Returns TRUE if
 * echo reply received. False otherwise.
 */

boolean vines_echo_send (
    vinesipcsocket **socket,
    vinesechotype *echoinfo,
    int bytes)
{
    paktype *pak;
    vinesidbtype *idb;
    ushort *u;

    /*
     * Setup packet.
     */
    bytes = bytes - sizeof(vinesiptype) - sizeof(vinesipctype);
    pak = vipc_getbuffer(bytes);
    if (!pak)
	return(FALSE);
    pak->datagramsize = bytes;

    u = (ushort *)pak->session_start;
    memset(u, 0, bytes);
    *u++ = 0x1122;
    *u++ = 0x3344;
    *u++ = 0x5566;
    *u++ = 0x7788;
    *u++ = 0x99aa;
    *u++ = 0xbbcc;
    *u++ = 0xddee;
    *u++ = 0xff00;

    /*
     * Setup info to remember this packet.
     */
    echoinfo->next = NULL;
    echoinfo->active = TRUE;
    echoinfo->tty = stdio;
    GET_TIMESTAMP(echoinfo->timesent);

    /*
     * Send it.
     */
    idb = vipc_send_data(*socket, pak, VIPC_PORT_ECHO, echoinfo->port, VINES_HOPS);
    if (!idb) {
	*socket = NULL;
	return(FALSE);
    }
    
    idb->tx_echo++;

    /*
     * Wait for the response (or timeout)
     */
    p_enqueue(&vinesechoQ, echoinfo);

    /* Suspend until reply received, user interrupt or timeout */
    for (;;) {
	if (!echoinfo->active || psipending2(echoinfo->tty) ||
	    AWAKE(echoinfo->timeout))
	    break;
	process_suspend();
    }
    
    if (echoinfo->active)
	p_unqueue(&vinesechoQ, echoinfo);
    return(echoinfo->active ? FALSE : TRUE);
}

/*
 * vines_echo_bounce
 *
 * There is really no work to be done here.  All of the important
 * information is being kept in the IPC header so it is automagically
 * returned to the sender.
 */
void vines_echo_bounce (
    vinesipcsocket *socket,
    uchar *ipc_data,
    int ipc_length)
{
    paktype *reply;

    reply = vipc_getbuffer(ipc_length);
    if (!reply) {
	if (vinesipc_debug)
	    buginf("\nVINES: cannot allocate packet for response.");
	return;
    }

    bcopy(ipc_data, reply->session_start, ipc_length);
    reply->datagramsize = ipc_length;
    if (vinesipc_debug)
	buginf("\nVines: sending echo reply to %#z",
	       socket->remote_net, socket->remote_host);
    vipc_send_data(socket, reply, 0, VIPC_PORT_ECHO, VINES_HOPS);
}

/**********************************************************************
 *
 *			PROCESS ERROR PACKETS
 *
 **********************************************************************/

/*
 * vines_ipc_echo_sink
 *
 * Take the returned echo packet and check to see if it is on either
 * the echo or trace queues.
 */
void vines_ipc_echo_sink (vinesipcsocket *socket, uchar *ipc_data, int ipc_length)
{
    vinesechotype *ep;
    vinestracetype *tp;

    if (vinesipc_debug)
	buginf("\nVines: received echo reply from %#z",
	       socket->remote_net, socket->remote_host);
    for (ep = (vinesechotype *)vinesechoQ.qhead; ep; ep = ep->next)
	if (socket->last_local_port == ep->port) {
	    ep->deltatime = ELAPSED_TIME(ep->timesent);
	    ep->how = '!';
	    ep->active = FALSE;
	    p_unqueue(&vinesechoQ, ep);
	    return;
	}
    for (tp = (vinestracetype *)vinestraceQ.qhead; tp; tp = tp->next)
	if (socket->last_local_port == tp->port) {
	    tp->deltatime = ELAPSED_TIME(tp->timesent);
	    tp->net = socket->remote_net;
	    tp->host = socket->remote_host;
	    tp->how = '!';
	    tp->active = FALSE;
	    p_unqueue(&vinestraceQ, tp);
	    return;
	}
}

/*
 * vines_icp_echo_sink
 *
 * Take the returned ICP error packet and check to see if it is on
 * either the echo or trace queues. 
 */
void vines_icp_echo_sink (paktype *pak)
{
    vinesicptype *icp;
    vinesiptype *vip, *oldip;
    vinesipctype *oldipc;
    vinestracetype *tp;

    vip = (vinesiptype *)pak->network_start;
    icp = (vinesicptype *)pak->transport_start;
    oldip = (vinesiptype *)icp->oldpak;
    oldipc = (vinesipctype *)oldip->data;

    for (tp = (vinestracetype *)vinestraceQ.qhead; tp; tp = tp->next) {
	if (oldipc->srcport == tp->port) {
	    tp->deltatime = ELAPSED_TIME(tp->timesent);
	    tp->net = GET2WD(vip->ssrcnet);
	    tp->host = vip->srchost;
	    if (icp->info == VINES_ERR_HOST_UNR) {
		if (GET2WD(vip->ssrcnet) == GET2WD(oldip->ddstnet))
		    tp->how = 'H';
		else
		    tp->how = 'N';
	    } else
		tp->how = '!';
	    tp->active = FALSE;
	    p_unqueue(&vinestraceQ, tp);
	}
    }
}

/**********************************************************************
 *
 *		   VINES TRACE - CISCO STYLE (1989)
 *
 **********************************************************************/

/*
 * vines_trace_cisco
 *
 * Send a Vines IPC packet to port 1 of the destination system,
 * using an increasing hop count with each set of packets.  Look
 * for the returned packet from a cisco router, or for the IPC error
 * message from a Banyan Vines system.  This port was used for an
 * early echo protocol by Banyan, but has long been discontinued.
 * Supply all of the "standard" cisco trace options.
 */
void vines_trace_cisco (char *args)
{
    vinestracetype data;
    boolean done;               /* got to destination */
    ushort probe, nprobes;      /* current probe; number at each level */
    ushort minttl, maxttl;      /* time to live range */
    ulong seconds;              /* time to wait */
    boolean numericonly;        /* don't ask name server for symbolic name */
    boolean verbose;            /* print extra output */
    char *c, buff[MAXLEN], throw_away;          /* input buffer */
    vinesipcsocket *socket;
    ulong dstnet;
    ushort dsthost;
    vipc_port_info *port;

    /*
     * Set default parameters
     */
    nprobes = 3;
    minttl = 0;
    maxttl = 15;
    numericonly = FALSE;
    verbose = FALSE;
    seconds = 3;

    /*
     * Get Vines address or collect extended paramters.
     */
    if (!parse_vinesaddress(args, &dstnet, &dsthost)) {
        printf("\nTarget Vines address: ");
	if (!rdtty(buff, MAXLEN) ||
	    !parse_vinesaddress(buff, &dstnet, &dsthost)) {
	    printf("\n%% Bad Vines address or host name");
	    return;
	}

	printf("\nNumeric display [n]: ");
	if (rdtty(buff, MAXLEN) && !null(buff) && toupper(*buff) == 'Y')
	    numericonly = TRUE;

	if (!trace_common(buff, &seconds, &nprobes, &minttl, &maxttl, VINES_HOPS))
	    return;
    }
  
    /*
     * Create a socket structre or fail miserably.
     */
    socket = vipc_create_conn(dstnet, dsthost, 0, 0, TRUE);
    if (!socket) {
	printf("\nCannot allocate memory for socket structure.");
	return;
    }

    /*
     * All parsing is done!  Let's actually start sending some packets.
     */
    psion(stdio);
    printf("%s%s", type_escape_seq_abort_msg, tracing_the_route_to_msg);
    if (!numericonly && (c = vines_lookup_name(dstnet, dsthost)))
	printf("%s (%z)\n", c, dstnet, dsthost);
    else
	printf("%z\n", dstnet, dsthost);
    flush();
    done = FALSE;
    for (data.ttl = minttl; !done && data.ttl <= maxttl; data.ttl++) {
	data.net = VINES_UNKNOWNNET;
	printf("\n %-2u ", data.ttl);
	for (probe = 0; probe < nprobes; ++probe) {

	    /*
	     * Each probe should have its own dport so we can distinguish
	     * responses.  The first packet that actually reaches the
	     * destination must have a sequence number of zero.  As long
	     * as the remote connection ID is zero, hothing has been
	     * heard from the remote side.  Tweak the sequence number so
	     * the next packet will be zero.  (This really shouldn't be
	     * sent on a connection, but we must live with a choice made
	     * in '89.
	     */
	    port = vipc_create_port(0,"Exec Trace", FALSE, vines_ipc_echo_sink);
	    data.port = port->number;
	    TIMER_START(data.timeout, seconds * ONESEC);
	    if (socket->remote_id == 0)
		socket->last_send_seq = 0;

	    /*
	     * Actually send a packet
	     */
	    vines_ciscotrace_sendprobe(socket, &data);
	    switch(data.how) {
	    case '!':    /* success ! */
		vines_ciscotrace_showprobe(verbose, numericonly, &data, probe);
		break;
	    case 'U':   /* got to the dest - quit now */
		vines_ciscotrace_showprobe(verbose, numericonly, &data, probe);
		done = TRUE;
		break;
	    case 'N':
	    case 'H':
	    case 'P':
		printf (" !%c ", data.how);
		done = TRUE;
		break;
	    case 'I':   /* user interrupted */
		done = TRUE;
		break;
	    case 'T':   /* timeout */
		printf(" * ");
		break;
	    default:
		printf(" ? ");
		break;
	    } /* switch */
	    flush();
	    /*
	     * The following read of getc is an attempt to get the PSIPENDING
	     * set if the user types an escape when we are using a vty
	     */
	    while ((throw_away = getc(stdio)) != -1)
	        ;
	    vipc_destroy_port(port);
	} /* for  probe */
	if (data.currentnet == dstnet && data.currenthost == dsthost)
	    done = TRUE;
    } /* for  ttl */
    if (!done && verbose)
	printf("\nDestination not found inside max TTL diameter.");

    flush();
    psioff(stdio);
}

/* 
 * vines_ciscotrace_showprobe
 *
 * Display the results of a successful traceroute probe
 */

void vines_ciscotrace_showprobe (
    boolean verbose,
    boolean numericonly,
    vinestracetype *data,
    ushort probe)
{
    char *c;

    if (data->net != data->currentnet || data->host != data->currenthost) {
	if (probe)
	    printf("\n    ");
	if (!numericonly && (c = vines_lookup_name(data->net, data->host)))
	    printf("%s (%z)", c, data->net, data->host);
	else
	    printf("%z", data->net, data->host);
    }
    printf(" %lu msec", data->deltatime);
    data->currentnet = data->net;
    data->currenthost = data->host;
}

/*
 * vines_ciscotrace_sendprobe
 *
 * Send a probe packet to the traceroute destination
 */

void vines_ciscotrace_sendprobe (
    vinesipcsocket *socket,
    vinestracetype *traceinfo)
{
    paktype *pak;
    long length;

    /*
     * Setup packet.
     */
    length = 20; /* Arbitrary */
    pak = vipc_getbuffer(length);
    if (!pak)
	return;
    memset(pak->session_start, 0, length);

    /*
     * Setup info to remember this packet.
     */
    traceinfo->next = NULL;
    traceinfo->active = TRUE;
    traceinfo->tty = stdio;
    GET_TIMESTAMP(traceinfo->timesent);

    /*
     * Send it.
     */
    if (!vipc_send_data(socket, pak, VIPC_PORT_ECHO, traceinfo->port,
			traceinfo->ttl))
	return;

    /*
     * Wait for the response (or timeout)
     */
    enqueue(&vinestraceQ, traceinfo);

    /* Suspend until response received, user interrupt or timeout.
     * Response could be IPC echo response (from a cisco), IPC error
     * from a Vines server, or an ICP hops exceeded message.
     */
    for (;;) {
	if (!traceinfo->active)	/* if block flag reset, break out */
	    break;
	if (psipending2(traceinfo->tty)) { /* interrupted? */
	    traceinfo->how = 'I';
	    break;
	}
	if (AWAKE(traceinfo->timeout)) { /* if we haven't heard, give up */
	    traceinfo->how = 'T';
	    break;
	}
	process_suspend();
    }
    
    if (traceinfo->active)
        unqueue(&vinestraceQ, traceinfo);
}

/**********************************************************************
 *
 *		   VINES TRACE - BANYAN STYLE (1992)
 *
 **********************************************************************/

/*
 * vtrace_rcv_data
 *
 * The NSM Service has received a Trace Data packet and unpacked it.  It
 * is up to this routine to determine which exec process requested this
 * information, and to attach it to the proper data queue for that
 * process.
 */
void vtrace_rcv_data (
    ushort port,
    void *dummy)
{
    vnsm_trace_other *data = dummy;
    vtrace_info *traceinfo;

    for (traceinfo = banyantraceQ.qhead; traceinfo; traceinfo = traceinfo->next) {
	if (traceinfo->port == port) {
	    if (data) {
		data_enqueue(&traceinfo->new_dataQ, data);
	    } else {
		traceinfo->flags |= VTRACE_ABORT;
	    }
	    return;
	}
    }
    free(data);
}

/*
 * vtrace_lower_seq
 *
 * Compare sequence numbers.  Used to determine if a newly arrived
 * message needs to be printed or queued for later printing.
 */
static inline boolean vtrace_lower_seq (
    vnsm_trace_other *on_queue,
    vnsm_trace_other *new)
{
    return(new->sequence < on_queue->sequence);
}

/*
 * vtrace_process_response
 *
 * Process a single trace response.  The return value of this routine
 * indicates whether or not the trace has finished. The printed output of
 * this routine should be identical to what a Banyan server would print.
 * Note that the message indicating the end of the trace has no data in
 * it that needs to be printed.
 */
static boolean vtrace_process_response (vnsm_trace_other *data)
{
    vnsm_trace_neighbor *nbr = &data->neighbor;
    vnsm_trace_route *route = &data->route;
    char *svr, *gate;
    int i;

    if (route->gateway_net == 0)
	return(TRUE);

    svr  = vines_lookup_name(nbr->lnetid, VINES_SERVER);
    gate = vines_lookup_name(route->gateway_net, VINES_SERVER);
    printf("\n%10Z %15s %10Z %15s  %-4x  %-5d ", nbr->lnetid, svr,
	   route->gateway_net, gate, route->metric, nbr->interface_type);
    for (i = 0; i < nbr->sig_bytes; i++)
	printf("%02x", nbr->neighbor_addr[i]);
    return(FALSE);
}

/*
 * vtrace_cleanup
 *
 * Clean up after a trace has finished.  This routine is called after
 * sucessful traces, failed traces, etc.  It is responsible for cleaning
 * up the data queues, freeing the port, etc.
 */
static void vtrace_cleanup (vtrace_info *traceinfo)
{
    vipc_port_info *port;
    uchar *data;

    flush();
    psioff(stdio);

    unqueue(&banyantraceQ, traceinfo);
    while (traceinfo->new_dataQ.count) {
	data = data_dequeue(&traceinfo->new_dataQ);
	free(data);
    }
    while (traceinfo->saved_dataQ.count) {
	data = data_dequeue(&traceinfo->saved_dataQ);
	free(data);
    }
    port = vipc_find_port(traceinfo->port);
    vipc_destroy_port(port);
}

/*
 * vines_trace_banyan
 *
 * Send a Banyan style trace packet.  This is an application level packet
 * that is sent to the Network and Systems Managent (NSM) Service on the
 * starting server/router of the trace.  The request is then cascaded
 * from NSM Service to NSM Service, with trace information messages sent
 * back to the originating client (or in case, the exec process).
 */
void vines_trace_banyan (char *args)
{
    vnsm_trace_other *tracedata;
    vtrace_info traceinfo;
    boolean done;
    char *s, *d, buff[MAXLEN];          /* input buffer */
    ulong destination, source;
    ushort host;

    /*
     * Initialize
     */
    memset(&traceinfo, 0, sizeof(vtrace_info));
    queue_init(&traceinfo.new_dataQ, 0);
    queue_init(&traceinfo.saved_dataQ, 0);
    traceinfo.tty = stdio;
    traceinfo.print = 1;

    /*
     * Get the VINES address if they were not specified on the command
     * line.  If a destination *was* specified on the command line, then
     * the source should default to the local router.  This is to allow
     * quick access from the command line without questions.
     */
    if (!parse_vinesaddress(args, &destination, &host)) {
        printf("\nTarget Vines address: ");
	if (!rdtty(buff, MAXLEN) ||
	    !parse_vinesaddress(buff, &destination, &host)) {
	    printf("\n%% Bad Vines address or host name");
	    return;
	}
	if (host != VINES_SERVER) {
	    printf("\n%% Trace can only be performed between servers.");
	    return;
	}

	printf("\nSource Vines address: ");
	if (!rdtty(buff, MAXLEN) ||
	    !parse_vinesaddress(buff, &source, &host)) {
	    printf("\n%% Bad Vines address or host name");
	    return;
	}
	if (host != VINES_SERVER) {
	    printf("\n%% Trace can only be performed between servers.");
	    return;
	}
    } else {
	source = vines_network;
    }
  
    /*
     * All parsing is done!  Print the header
     */
    s = vines_lookup_name(source, VINES_SERVER);
    d = vines_lookup_name(destination, VINES_SERVER);
    printf("\nFrom: %10Z %15s    To: %10Z %15s\n", source, s, destination, d);
    printf("\nServer                     Gate                       metric media address");
    flush();
    psion(stdio);

    /*
     * Fire off the trace request.
     */
    traceinfo.port = vnsm_trace_send_call(source, destination);
    TIMER_START(traceinfo.timeout, VTRACE_MAXWAITTIME);
    enqueue(&banyantraceQ, &traceinfo);

    /*
     * Now just sit back and wait for the replies.  There's nothing else
     * to do as Banyan's trace is designed to cascade from one server to
     * the next.  Note that the dismiss is safe because this routine is
     * always called from the context of an exec process, never fromthe
     * context of a vines routing process.
     */
    done = FALSE;
    while (!done) {

	/* Wait for messages from the Network and Systems Management
	 * Services.  This will cause a process to block until a message
	 * arrives, a timeout occurs or an interrupt occurs.
	 */
	for (;;) {
	    if (!QUEUEEMPTY(&traceinfo.new_dataQ))
		break;
	    if (psipending2(traceinfo.tty)) {
		traceinfo.flags |= VTRACE_INTERRUPTED;
		break;
	    }
	    if (!XSLEEPING(traceinfo.timeout, VTRACE_MAXWAITTIME)) {
		traceinfo.flags |= VTRACE_TIMEOUT;
		break;
	    }
	    process_suspend();
	}

	if (traceinfo.flags) {
	    if (traceinfo.flags & VTRACE_TIMEOUT) {
		printf("\nTimed Out.");
	    } else if (traceinfo.flags & VTRACE_ABORT) {
		printf("\nProtocol Error.");
	    }
	    vtrace_cleanup(&traceinfo);
	    return;
	}
	while (!done && traceinfo.new_dataQ.count) {
	    tracedata = data_dequeue(&traceinfo.new_dataQ);
	    if (tracedata->sequence == traceinfo.print) {
		done = vtrace_process_response(tracedata);
		free(tracedata);
		traceinfo.print++;
	    } else {
		data_insertlist(&traceinfo.saved_dataQ, tracedata,
				vtrace_lower_seq);
	    }
	}
	while (!done && traceinfo.saved_dataQ.count) {
	    tracedata = traceinfo.saved_dataQ.qhead;
	    if (tracedata->sequence == traceinfo.print) {
		tracedata = data_dequeue(&traceinfo.saved_dataQ);
		done = vtrace_process_response(tracedata);
		free(tracedata);
		traceinfo.print++;
	    } else {
		break;
	    }
	}
    }
    vtrace_cleanup(&traceinfo);
}

/*
 * vines_init_echo
 *
 * Initialize the trace and echo queues.
 */
void vines_init_echo (void)
{
    queue_init(&vinesechoQ, 0);
    queue_init(&vinestraceQ, 0);
    queue_init(&banyantraceQ, 0);
}




/*
 * snmp_vines_echo_validate
 * Validate a vines echo for SNMP
 * Called via the snmp_ping_validate registry
 */

static int snmp_vines_echo_validate (ciscoPingEntry_t *entry)
{
    /*
     * there is a real problem if the protocol isn't set, or if it
     * isn't set to the same value as the case registry switch value
     */
    if (!VALID(I_ciscoPingProtocol, entry->valid) ||
	(entry->ciscoPingProtocol != D_ciscoPingProtocol_vines))
	 return (SNMP_PING_INVALID_PROTOCOL);

    /*
     * If the address isn't set, we simply continue
     * (the activate routine won't be called by snmp until an address has
     * been set and validated)
     */
    if (VALID(I_ciscoPingAddress, entry->valid) &&
	entry->ciscoPingAddress->length != ADDRLEN_VINES)
	return (SNMP_PING_INVALID_ADDRESS);

    /*
     * if the packet size is set, it better be within the legal range.
     * if the packet size is not set, we simply continue (the activation
     * routine won't be called by snmp until a packet size has been set
     * and validated)
     */
    if (VALID(I_ciscoPingPacketSize, entry->valid) &&
	((entry->ciscoPingPacketSize < VINES_MINECHOSIZE) ||
	(entry->ciscoPingPacketSize > VINES_MAXSIZE)))
	return (SNMP_PING_INVALID_SIZE);

    return (SNMP_PING_VALID);
}



/*
 * snmp_vines_echo_activate
 * Send a vines echo for SNMP
 * Called via the snmp_ping_activate registry
 */
static void snmp_vines_echo_activate (ciscoPingEntry_t *entry)
{
    ulong total_rtt, idx;
    int i;
    ulong net = 0L;
    ushort host;
    vinesechotype data;
    vinesipcsocket *socket;
    vipc_port_info *port;

    /*
     * Recover the VINES net:host from the OctetString. 
     * The first 4 bytes are the net, the last 2 are the host.
     * So to ping 30198A78:0001, use octet '30 19 8a 78 00 01'
     */
    for (i = 3; i >= 0; i--) {
	net |= entry->ciscoPingAddress->octet_ptr[i] <<
		((3 - i) * 8);
    }
    host = ((entry->ciscoPingAddress->octet_ptr[4] << 8) +
	entry->ciscoPingAddress->octet_ptr[5]);

    entry->ciscoPingSentPackets = 0;
    SET_VALID(I_ciscoPingSentPackets, entry->valid);
    entry->ciscoPingReceivedPackets = 0;
    SET_VALID(I_ciscoPingReceivedPackets, entry->valid);
    entry->ciscoPingCompleted = D_ciscoPingCompleted_false;
    SET_VALID(I_ciscoPingCompleted, entry->valid);
    total_rtt = 0;

    for (idx = 1; idx <= entry->ciscoPingPacketCount; idx++) {
	entry->ciscoPingSentPackets++;
	port = vipc_create_port(0, "Exec Ping", FALSE, vines_ipc_echo_sink);
	data.port = port->number;
	socket = vipc_create_conn(net, host, 0, 0, TRUE);
	if (!socket)
	    return;

	TIMER_START(data.timeout, entry->ciscoPingPacketTimeout);
	if (vines_echo_send(&socket, &data, entry->ciscoPingPacketSize)) {
	    if (data.how == '!') {
		entry->ciscoPingReceivedPackets++;
		if (data.deltatime == 0L)
		    data.deltatime = 1L;
		total_rtt += data.deltatime;
		if ((!VALID(I_ciscoPingMinRtt, entry->valid)) ||
			(data.deltatime < entry->ciscoPingMinRtt)) {
		    entry->ciscoPingMinRtt = data.deltatime;
		    SET_VALID(I_ciscoPingMinRtt, entry->valid);
		}
		if ((!VALID(I_ciscoPingMaxRtt, entry->valid)) ||
			(data.deltatime > entry->ciscoPingMaxRtt)) {
		    entry->ciscoPingMaxRtt = data.deltatime;
		    SET_VALID(I_ciscoPingMaxRtt, entry->valid);
		}
		entry->ciscoPingAvgRtt =
			total_rtt / entry->ciscoPingReceivedPackets;
		SET_VALID(I_ciscoPingAvgRtt, entry->valid);
	    }
        }
	if (idx < entry->ciscoPingPacketCount) {
	    process_sleep_for(entry->ciscoPingDelay);
	}
	vipc_destroy_port(port);
    }
    entry->ciscoPingCompleted = D_ciscoPingCompleted_true;
}



void register_snmp_vines_echo(void)
{
    reg_add_snmp_ping_validate(D_ciscoPingProtocol_vines,
                snmp_vines_echo_validate,
                "snmp_vines_echo_validate");

    reg_add_snmp_ping_activate(D_ciscoPingProtocol_vines,
                snmp_vines_echo_activate,
                "snmp_vines_echo_activate");
}

