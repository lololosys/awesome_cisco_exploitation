/* $Id: xdmcp.c,v 3.6 1996/01/25 10:08:29 smackie Exp $
 * $Source: /vws/aam/cherf.111/ios/sys/xremote/xdmcp.c,v $
 *------------------------------------------------------------------
 * xdmcp.c - X Display Manager Control Protocol client
 *
 * February 27, 1991, Kevin Paul Herbert
 *
 * Copyright (c) 1991-1996 by cisco Systems, Inc.
 *------------------------------------------------------------------
 * $Log: xdmcp.c,v $
 * Revision 3.6  1996/01/25  10:08:29  smackie
 * Make xdmcp.c compile again. (CSCdi47046)
 *
 * Revision 3.5  1996/01/24  20:42:56  jenny
 * CSCdi47046:  TCP/IP need code clean up (part 3)
 * . removed ip/tuba.c, ip/tuba_default.c and ip/tuba.h
 * . removed all includes for ip/tuba.h.
 * . removed more TUBA related code.
 *
 * Revision 3.4  1996/01/22  07:53:31  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.3  1996/01/11  02:35:27  jenny
 * CSCdi46513:  TCP/IP need code clean up (part 2)
 * changed registry function ip_tuba_move_address to inline
 * ip_copy_address().
 *
 * Revision 3.2  1995/11/17  19:26:43  gstovall
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  14:01:39  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/08/08  16:51:39  hampton
 * Miscellaneous changes to eliminate direct references to the system
 * clock and to convert to always using the timer macros to access timers.
 * [CSCdi38391]
 *
 * Revision 2.3  1995/08/03 07:42:51  hampton
 * EIGRP stops processing input.  Convert the IP socket input queue into a
 * watched_queue.  This allows the scheduler to see if there is input
 * waiting when the process dismisses.  [CSCdi38156]
 *
 * Revision 2.2  1995/07/14 07:03:41  hampton
 * Convert Terminal Server to fully use the passive timers macros instead
 * of referencing the system clock directly.  [CSCdi37093]
 *
 * Revision 2.1  1995/06/07 23:32:02  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Copyright 1989 Network Computing Devices, Inc., Mountain View, California.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided
 * that the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of N.C.D. not be used in advertising or
 * publicity pertaining to distribution of the software without specific,
 * written prior permission.  N.C.D. makes no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 */

#include "master.h"
#include "interface_private.h"
#include "packet.h"
#include "ttysrv.h"
#include "../ip/ip.h"
#include "Xproto.h"
#include "os.h"
#include "Xdmcp.h"
#include "xremote.h"
#include "registry.h"
#include "../ip/ip_registry.h"

#define DEBUG (xrxdm_debug)

static long		    defaultKeepaliveDormancy = XDM_DEF_DORMANCY;

static void xdm_send_packet(xrcontext *);
static void xdm_receive_packet(xrcontext *);
static void xdm_timeout(xrcontext *);
static void xdm_send_query_msg(xrcontext *);
static void xdm_recv_willing_msg(xrcontext *, ushort *, int, ipaddrtype,
  				 ipaddrtype, ushort);
static void xdm_send_request_msg(xrcontext *);
static void xdm_recv_accept_msg(xrcontext *, ushort *, int);
static void xdm_recv_decline_msg(xrcontext *, ushort *, int);
static void xdm_send_manage_msg(xrcontext *);
static void xdm_recv_refuse_msg(xrcontext *, ushort *, int);
static void xdm_recv_failed_msg(xrcontext *, ushort *, int);
static void xdm_send_keepalive_msg(xrcontext *);
static void xdm_recv_alive_msg(xrcontext *, ushort *, int);

extern boolean xrxdm_debug;

/* 
 * initialize XDMCP; create the socket, compute the display
 * number, set up the state machine
 */

boolean xdm_init (xrcontext *ctx, ipaddrtype addr, xdmcp_states state)
{
    addrtype faddr;

    faddr.type = ADDR_IP;
    faddr.ip_addr = addr;
    ctx->xdmSocket = open_ipsocket(UDP_PROT, NULL, &faddr, 0, 
				   XDM_UDP_PORT, SOCK_ANYSRC | SOCK_INTERR);
    if (!ctx->xdmSocket) {
	return(FALSE);
    }

    ctx->xdmState = state;
    ctx->xdmTimeOutRtx = 0;
    ctx->xdmKeepaliveDormancy = defaultKeepaliveDormancy;
    xdm_send_packet(ctx);
    return(TRUE);
}

/*
 * xdm_shutdown - Return XDM data structures
 */

void xdm_shutdown (xrcontext *ctx)
{
    reg_invoke_ip_close_socket(ctx->xdmSocket);
    ctx->xdmSocket = NULL;
    ctx->xdmState = XDM_OFF;
}

/*
 * Hooks for clients connecting and disconnecting
 *
 * When Xremote opens and closes TCP connections, it calls us with a pointer
 * to the client structure. We remember the client structure pointer so
 * that we can break all connections when the display manager exits.
 */

void xdm_tcp_open (xrcontext *ctx, struct client *client)
{
    if (ctx->xdmState != XDM_AWAIT_MANAGE_RESPONSE) {
	return;
    }

    if (DEBUG) {
	buginf("\nXDM%t: Changing state to XDM_RUN_SESSION", ctx->tty->ttynum);
    }

    ctx->xdmState = XDM_RUN_SESSION;
    ctx->xdmSessionClient = client;

    ctx->xdmSocket->status &= ~SOCK_INTERR; /* Disable pseudo-interrupts */
}

void xdm_tcp_close (xrcontext *ctx, struct client *client)
{
    if (((ctx->xdmState != XDM_RUN_SESSION && 
	 ctx->xdmState != XDM_AWAIT_ALIVE_RESPONSE) ||
	 ctx->xdmSessionClient != client)){
	return;
    }

    if (DEBUG) {
	buginf("\nXDM%t: Closing down, state %x", ctx->tty->ttynum,
	       ctx->xdmState);
    }

    xremote_reset(ctx, "Display manager closed connection");
}


boolean xdmBLOCK (xrcontext *ctx)
{
    if (ctx->xdmState != XDM_OFF) {
	if (!process_is_queue_empty(ctx->xdmSocket->inq))
	    return(FALSE);		/* Don't block if network input */

	if ((ctx->xdmSocket->status & SOCK_INTERR) && psipending2(ctx->tty))
	    return(FALSE);		/* Get out on an interrupt */

	if (TIMER_RUNNING_AND_AWAKE(ctx->xdmTimeOutTime))
	    return(FALSE);		/* Don't block if a pending timeout */

	if (ctx->stopReason)
	    return(FALSE);		/* Get out on errors */
    }
    return(TRUE);		/* Let the caller block */
}

void xdm_wakeup (xrcontext *ctx)
{
    if (ctx->xdmState == XDM_OFF)
	return;

    xdm_receive_packet(ctx);	/* Receive any pending packets */

    if (ctx->xdmState == XDM_RUN_SESSION && 
	ELAPSED_TIME(ctx->idletime) < defaultKeepaliveDormancy * ONESEC) {
	ctx->xdmKeepaliveDormancy = defaultKeepaliveDormancy;
	TIMER_START(ctx->xdmTimeOutTime, ctx->xdmKeepaliveDormancy * ONESEC);
    }

    if (TIMER_RUNNING_AND_AWAKE(ctx->xdmTimeOutTime)) {
    	if (ctx->xdmState == XDM_RUN_SESSION) {
	    ctx->xdmState = XDM_KEEPALIVE;
	    xdm_send_packet(ctx);
    	} else {
	    xdm_timeout(ctx);
	}
    }
}

/*
 * This routine should be called from the routine that drives the
 * user's host menu when the user selects a host
 */

static void
xdm_select_host (
     xrcontext *ctx,
     ipaddrtype mgraddr,
     ipaddrtype locaddr,
     ushort port)
{
    ipsocktype *soc = ctx->xdmSocket;

    ip_copy_address(mgraddr, &soc->faddress);
    ip_copy_address(locaddr, &soc->laddress);
    soc->fport = h2n(port);
    soc->status &= ~SOCK_ANYSRC;

    ctx->xdmState = XDM_START_CONNECTION;

    xdm_send_packet(ctx);
}

/*
 * !!! this routine should be replaced by a routine that adds
 * the host to the user's host menu. the current version just
 * selects the first host to respond with willing message.
 */

static void
xdm_add_host (
     xrcontext *ctx,
     ipaddrtype mgraddr,
     ipaddrtype locaddr,
     ushort port,
     int hostlen,
     uchar *hostptr,
     int statlen,
     ushort *statptr)
{
    xdm_select_host(ctx, mgraddr, locaddr, port);
}

/*
 * A message is queued on the socket; read it and
 * do the appropriate thing
 */

static void
xdm_receive_packet (xrcontext *ctx)
{
    paktype *pak;
    iphdrtype *ip;
    udptype *udp;
    ushort *ptr;
    int params;
    xdmOpCode opcode;

    while (TRUE) {
	pak = read_ipsocket(ctx->xdmSocket, FALSE, 0, &udp, &ptr);

	if (!pak)
	  return;

	ip = (iphdrtype *)ipheadstart(pak);

	ctx->xdmTimeOutRtx = 0;	    /* reset retransmission backoff */

	/* Validate the message header */

	if (udp->length >= 6) {
	    if (n2h(*ptr++) == XDM_PROTOCOL_VERSION) {
		opcode = n2h(*ptr++);		/* Get the opcode */
		params = n2h(*ptr++);		/* And the parameters length */

		if (udp->length >= params + 6) {
		    
		    if (DEBUG)
		      buginf("\nXDM%t: Received type %x from %i",
			     ctx->tty->ttynum, opcode, n2hl(ip->srcadr));

		    switch (opcode) {
		      case WILLING:
			xdm_recv_willing_msg(ctx, ptr, params, 
					     n2hl(ip->srcadr),
					     pak->if_input->ip_address,
					     n2h(udp->sport));
			break;
		      case UNWILLING:
			xremote_reset(ctx, 
			      "Display manager unwilling to manage display");
			break;
		      case ACCEPT:
			xdm_recv_accept_msg(ctx, ptr, params);
			break;
		      case DECLINE:
			xdm_recv_decline_msg(ctx, ptr, params);
			break;
		      case REFUSE:
			xdm_recv_refuse_msg(ctx, ptr, params);
			break;
		      case FAILED:
			xdm_recv_failed_msg(ctx, ptr, params);
			break;
		      case ALIVE:
			xdm_recv_alive_msg(ctx, ptr, params);
			break;
		      default:
			break;
		    }
		}
	    }
	}
	datagram_done(pak);
    }
}

/*
 * send the appropriate message given the current state
 */

static void
xdm_send_packet (xrcontext *ctx)
{
    /*
     * Set up a retransmission timeout. We do this before calling the
     * actual send routine to allow the send routine to set a shorter timeout
     * to handle buffer unavailability.
     */

    TIMER_START(ctx->xdmTimeOutTime,
	min(XDM_MIN_RTX * (1 << ctx->xdmTimeOutRtx), XDM_MAX_RTX) * ONESEC);

    if (DEBUG)
      buginf("\nXDM%t: Sending packet, state %x", ctx->tty->ttynum,
	     ctx->xdmState);

    switch (ctx->xdmState) {
      case XDM_QUERY:
      case XDM_BROADCAST:
      case XDM_INDIRECT:
	xdm_send_query_msg(ctx);
	break;
      case XDM_START_CONNECTION:
	xdm_send_request_msg(ctx);
	break;
      case XDM_MANAGE:
	xdm_send_manage_msg(ctx);
	break;
      case XDM_KEEPALIVE:
	xdm_send_keepalive_msg(ctx);
	break;
      default:
	break;
    }
}

/*
 * Timeout waiting for an XDMCP response.
 */

static void xdm_timeout (xrcontext *ctx)
{
    ctx->xdmTimeOutRtx++;

    if (DEBUG)
      buginf("\nXDM%t: Timeout, counter = %d, state = %x", ctx->tty->ttynum,
	     ctx->xdmTimeOutRtx, ctx->xdmState);

    if (ctx->xdmState == XDM_AWAIT_ALIVE_RESPONSE && 
	ctx->xdmTimeOutRtx >= XDM_KA_RTX_LIMIT) {
	xremote_reset(ctx, "too many XDM keepalive retransmissions");
	return;
    } else {
	if (ctx->xdmTimeOutRtx >= XDM_RTX_LIMIT) {
	    xremote_reset(ctx, "too many XDM retransmissions");
	    return;
	}
    }

    switch (ctx->xdmState) {
      case XDM_COLLECT_QUERY:
	ctx->xdmState = XDM_QUERY;
	break;
      case XDM_COLLECT_BROADCAST_QUERY:
	ctx->xdmState = XDM_BROADCAST;
	break;
      case XDM_COLLECT_INDIRECT_QUERY:
	ctx->xdmState = XDM_INDIRECT;
	break;
      case XDM_AWAIT_REQUEST_RESPONSE:
	ctx->xdmState = XDM_START_CONNECTION;
	break;
      case XDM_AWAIT_MANAGE_RESPONSE:
	ctx->xdmState = XDM_MANAGE;
	break;
      case XDM_AWAIT_ALIVE_RESPONSE:
	ctx->xdmState = XDM_KEEPALIVE;
	break;
      default:
	break;
    }
    xdm_send_packet(ctx);
}

static void xdm_send_query_msg (xrcontext *ctx)
{
    paktype *pak;
    uchar *start;
    uchar *ptr;

    pak = setup_ipsocket(ctx->xdmSocket, XDM_QUERY_MAX, (uint *)&start);

    /* If we didn't get a packet, try this again in a half second */

    if (!pak) {
	TIMER_START(ctx->xdmTimeOutTime, ONESEC/2);
	return;
    }
      
    start += (ulong)pak->data_area;	/* Point to packet start */
    ptr = start;			/* Make a work copy */

    *((ushort *)ptr)++ = h2n(XDM_PROTOCOL_VERSION);

    switch(ctx->xdmState) {
      case XDM_QUERY:
	*((ushort *)ptr)++ = h2n(QUERY);
	ctx->xdmState = XDM_COLLECT_QUERY;
	break;
      case XDM_BROADCAST:
	*((ushort *)ptr)++ = h2n(BROADCAST_QUERY);
	ctx->xdmState = XDM_COLLECT_BROADCAST_QUERY;
	break;
      case XDM_INDIRECT:
	*((ushort *)ptr)++ = h2n(INDIRECT_QUERY);
	ctx->xdmState = XDM_COLLECT_INDIRECT_QUERY;
	break;
      default:
	break;
    }

    /*
     * Fill in the remainder of the message. For now, we do not send
     * out any authentication names, so our count is always one byte,
     * and we send a single byte of zero.
     */

    *((ushort *)ptr)++ = h2n(1);	/* One more byte in message */
    *ptr++ = 0;				/* Specify no authentication names */

    write_ipsocket(ctx->xdmSocket, pak, ptr - start); 
}

static void
xdm_recv_willing_msg (xrcontext *ctx,
		      ushort *ptr,
		      int length,
		      ipaddrtype mgraddr,
		      ipaddrtype locaddr,
		      ushort port)
{
    int authlen;
    int hostlen;
    uchar *hostptr;
    int statlen;

    if (length < 6)
      return;				/* Find at least three empty
					   ARRAY8 items */

    authlen = n2h(*ptr);		/* Pick up length of authentication
					   name array */
    length -= authlen + 2;		/* Update the remaining length */
    (uchar *)ptr += authlen + 2;	/* Point to hostname field */
    if (length < 2)
      return;				/* Get out if malformed message */

    hostlen = n2h(GETSHORT(ptr));	/* Get the length of the hostname */
    length -= hostlen + 2;		/* Update the remaining length */
    hostptr = (uchar *)((ushort *)++ptr); /* Point to hostname */
    (uchar *)ptr += hostlen;		/* Point to the status field */
    if (length < 2)
      return;				/* Get out if malformed message */

    statlen = n2h(GETSHORT(ptr));	/* Get the length of status field */
    length -= statlen + 2;
    ptr++;
    if (length < 0)
      return;				/* Get out if malformed message */

    /*
     * At this point, hostlen contains the length of the hostname field,
     * hostptr points to the hostname, statlen points to the length of
     * the status field, and ptr points to the status field.
     */

    switch (ctx->xdmState) {
      case XDM_COLLECT_QUERY:
	xdm_select_host(ctx, mgraddr, locaddr, port);
	break;
      case XDM_COLLECT_BROADCAST_QUERY:
      case XDM_COLLECT_INDIRECT_QUERY:
	xdm_add_host(ctx, mgraddr, locaddr, port,
		     hostlen, hostptr, statlen, ptr);
	break;
      default:
	break;
    }
}

static void xdm_send_request_msg (xrcontext *ctx)
{
    paktype *pak;
    uchar *start;
    uchar *ptr;

    pak = setup_ipsocket(ctx->xdmSocket, XDM_REQUEST_MAX, (uint *)&start);

    if (!pak) {
	TIMER_START(ctx->xdmTimeOutTime, ONESEC/2);
	return;
    }

    start += (ulong)pak->data_area;	/* Point to packet start */
    ptr = start;			/* Make a work copy */

    *((ushort *)ptr)++ = h2n(XDM_PROTOCOL_VERSION);
    *((ushort *)ptr)++ = h2n(REQUEST);
    *((ushort *)ptr)++ = h2n(19);	/* Set length */

    /* Fill in the parameters of the message */

    *((ushort *)ptr)++ = h2n(2000 + ctx->tty->ttynum); /* Display number */

    *ptr++ = 1;				/* Length of address family list */
    *ptr++ = 0;				/* Set the padding byte */
    *ptr++ = X_FAMILY_INTERNET;		/* Set the family code */

    *ptr++ = 1;				/* Set the count of addresses */
    *((ushort *)ptr)++ = h2n(4);	/* Set length of an IP address */
    PUTLONG(ptr, ctx->xdmSocket->laddress.ip_addr); /* Set IP address */
    ptr += 4;				/* Point past it */

    *((ushort *)ptr)++ = 0;		/* No authentication name */
    *((ushort *)ptr)++ = 0;		/* No authentication data */
    *ptr++ = 0;				/* No authorization names */
    *ptr++ = 0;				/* No display ID length MSB */
    *ptr++ = 0;				/* No display ID length LSB */

    write_ipsocket(ctx->xdmSocket, pak, ptr - start);
    ctx->xdmState = XDM_AWAIT_REQUEST_RESPONSE;
}

static void xdm_recv_accept_msg (xrcontext *ctx, ushort *ptr, int length)
{
    if (ctx->xdmState != XDM_AWAIT_REQUEST_RESPONSE)
	return;

    /* All we care about in this message is the session ID from the
     * accept message, so we just check that the message contains at
     * least that. Since we do not support authentication, there is no
     * reason to look farther in the packet.
     */

    if (length < 4)
      return;

    ctx->xdmSessionID = n2hl(GETLONG(ptr));
    ctx->xdmState = XDM_MANAGE;
    xdm_send_packet(ctx);
}

static void xdm_recv_decline_msg (xrcontext *ctx, ushort *ptr, int length)
{
    int statlen;

    statlen = n2h(*ptr++);		/* Get length of status field */
    if (statlen > length)
      return;				/* Ignore invalid counts */
    xremote_reset(ctx, "Received decline message");
}

static void xdm_send_manage_msg (xrcontext *ctx)
{
    paktype *pak;
    uchar *start;
    ushort *ptr;

    pak = setup_ipsocket(ctx->xdmSocket, XDM_MANAGE_MAX, (uint *)&start);

    /* If we didn't get a packet, try this again in a half second */

    if (!pak) {
	TIMER_START(ctx->xdmTimeOutTime, ONESEC/2);
	return;
    }

    start += (ulong)pak->data_area;
    ptr = (ushort *)start;

    /* Fill in the xdmcp header */

    *ptr++ = h2n(XDM_PROTOCOL_VERSION);
    *ptr++ = h2n(MANAGE);
    *ptr++ = h2n(8);

    /* Fill in the parameters */

    PUTLONG(ptr, h2nl(ctx->xdmSessionID)); /* Set the session ID */
    ((uchar *)ptr) += 4;		/* Point past it */
    *ptr++ = h2n(2000 + ctx->tty->ttynum); /* Set display number */
    *ptr++ = 0;				/* Set empty display class */

    write_ipsocket(ctx->xdmSocket, pak, ((uchar *)ptr) - start);
    ctx->xdmState = XDM_AWAIT_MANAGE_RESPONSE;
}

static void xdm_recv_refuse_msg (xrcontext *ctx, ushort *ptr, int length)
{
    ulong refusedSessionID;

    if (ctx->xdmState != XDM_AWAIT_MANAGE_RESPONSE)
      return;

    if (length != 4)
      return;

    refusedSessionID = n2hl(GETLONG(ptr));	/* Extract session ID */

    if (refusedSessionID == ctx->xdmSessionID) {
	ctx->xdmState = XDM_START_CONNECTION;
	xdm_send_packet(ctx);
    }
}

static void xdm_recv_failed_msg (xrcontext *ctx, ushort *ptr, int length)
{
    ulong failedSessionID;
    int statlen;

    if (ctx->xdmState != XDM_AWAIT_MANAGE_RESPONSE)
      return;

    if (length < 6)
      return;				/* Ensure minimum length message */

    failedSessionID = n2hl(GETLONG(ptr)); /* Extract session ID */

    statlen = n2h(*ptr++);		/* Get the status length */

    if (statlen + 6 > length)
      return;				/* Check message length */

    if (failedSessionID == ctx->xdmSessionID) {
	printf("\nXDMCP: Received failed message: %*s", statlen, ptr);
    }
}

static void xdm_send_keepalive_msg (xrcontext *ctx)
{
    paktype *pak;
    uchar *start;
    ushort *ptr;

    pak = setup_ipsocket(ctx->xdmSocket, XDM_KEEPALIVE_MAX, (uint *)&start);
    if (!pak) {
	TIMER_START(ctx->xdmTimeOutTime, ONESEC/2);
	return;
    }
    
    start += (ulong)pak->data_area;
    ptr = (ushort *)start;

    /* Fill in the xdmcp header */

    *ptr++ = h2n(XDM_PROTOCOL_VERSION);
    *ptr++ = h2n(KEEPALIVE);
    *ptr++ = h2n(6);

    *ptr++ = h2n(2000 + ctx->tty->ttynum);	/* Set display number */
    PUTLONG(ptr, h2nl(ctx->xdmSessionID)); /* Set the session ID */
    ptr += 2;

    write_ipsocket(ctx->xdmSocket, pak, ((uchar *)ptr) - start);
    ctx->xdmState = XDM_AWAIT_ALIVE_RESPONSE;
}
 
static void xdm_recv_alive_msg (xrcontext *ctx, ushort *ptr, int length)
{
    boolean sessionRunning;
    ulong aliveSessionID;

    if (ctx->xdmState != XDM_AWAIT_ALIVE_RESPONSE)
      return;

    if (length != 5)
	return;

    sessionRunning = (*((uchar *)ptr))++;
    aliveSessionID = n2hl(GETLONG(ptr));

    if (sessionRunning /* && aliveSessionID == ctx->xdmSessionID */ ) {
	/* backoff dormancy period */
	ctx->xdmState = XDM_RUN_SESSION;
	if (ELAPSED_TIME(ctx->idletime) > ctx->xdmKeepaliveDormancy * ONESEC)
	  ctx->xdmKeepaliveDormancy = min(ctx->xdmKeepaliveDormancy << 1, 
					  XDM_MAX_DORMANCY);
	TIMER_START(ctx->xdmTimeOutTime, ctx->xdmKeepaliveDormancy * ONESEC);
    } else {
	xremote_reset(ctx, "XDM Alive response indicates session dead");
    }
}
