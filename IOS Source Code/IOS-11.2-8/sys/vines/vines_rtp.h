/* $Id: vines_rtp.h,v 3.3.18.1 1996/05/11 16:17:42 thille Exp $
 * $Source: /release/112/cvs/Xsys/vines/vines_rtp.h,v $
 *------------------------------------------------------------------
 * vines_rtp.h  --  Definitions for Vines RTP protocol.
 *
 * February 1990, David Hampton
 *
 * Copyright (c) 1990-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: vines_rtp.h,v $
 * Revision 3.3.18.1  1996/05/11  16:17:42  thille
 * CSCdi51397:  Duplicate strings in vines code
 * Branch: California_branch
 * Clean up some duplicate literals and save 1140 bytes in gs7-j-m.
 *
 * Revision 3.3  1996/02/08  20:14:10  thille
 * CSCdi48347: format string in inlines, code size impact
 * Pass 1 - Clean up some literal strings in inlines, duplicate literals,
 * etc.  11,356 bytes saved on gs7-j-m, 9336 bytes saved on igs-j-l
 *
 * Revision 3.2  1995/11/17  17:58:33  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:45:11  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:15:02  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/**********************************************************************
 *
 *			   Pre VINES 5.50
 *			 PACKET DATA FORMATS
 *
 **********************************************************************/

/* 
 * Vines RTP header structure definition.
 */
struct vrtp_type_ {
    uchar op;
    uchar node;
    uchar controller;
    uchar machine;
    uchar data[0];
};

#define VRTP_REQUEST	 1
#define VRTP_UPDATE	 4
#define VRTP_RESPONSE	 5
#define VRTP_REDIRECT	 6
#define VRTP_LAST	 7

/*
 * The following two definitions are the same as the definitions for
 * the VINES SRTP protocol.  The code utiliizes this knowledge to
 * perform as single test on the neighbor node type, instead of a
 * specific test for each version.  If these definitions change, the
 * code must be searched for all of these tests.
 */
#define VRTP_NODE_CLIENT 1
#define VRTP_NODE_ROUTER 2

#define VRTP_CONT_SINGLE 0
#define VRTP_CONT_MULTI	 1

#define VRTP_MACH_SLOW	   0
#define VRTP_MACH_FAST	   1
#define VRTP_MACH_TCPIP	   2
#define VRTP_COMPATABILITY 4

/*
 * Structure definition for the data portion of an update/response message.
 */
typedef struct vrtp_entry_ {
    uchar nnet[V_NETBYTES];
    ushort metric;
} vrtp_entry;

#define VRTP_MAX_ENTRIES  200

/*
 * Structure definition for the data portion of an redirect message.
 */
struct vrtp_redirecttype_ {
    ushort version;
    uchar  macaddrlen;
    uchar  riflen;
    uchar  dstnet[V_NETBYTES];
    ushort dsthost;
    ushort dstmetric;
    uchar  dstnode;
    uchar  dstcontroller;
    uchar  dstmachine;
    uchar  pad1;
    uchar  nbrnet[V_NETBYTES];
    ushort nbrhost;
    ushort nbrmetric;
    uchar  nbrnode;
    uchar  nbrcontroller;
    uchar  nbrmachine;
    uchar  pad2;
    uchar  mac[IEEEBYTES];
    uchar  rif[0];
};

#define V_REDIR_DATAMAX 30		/* potential RIF */

/*
 * Definitions for the `roll call' timer.  This is the timer in 5.50
 * implementations to insure that a 4.11 server on the other side of a
 * WAN link is still present.  Once an hour, it will send a simple
 * request for information to see if the other side is alive.  This
 * allows for recovery of a failed link to start automatically after this
 * time interval, instead of requiring a manual restart to start
 * recovery.  The initial time is used to kickstart things on WAN links
 * like Frame Relay where the other site might not see link changes on
 * this end.  Whenever the first packet is received from the other end
 * and the neighbor is created, usually from a RTP request, start with a
 * really short request time so that the full topology will be learned
 * quickly.
 */

#define VRTP_RC_INITIAL_TIME	(5 * ONESEC)
#define VRTP_RC_TIME		(ONEHOUR)
#define VRTP_RC_RETRY_TIME	(30 * ONESEC)

typedef enum vrtp_rc_states_ {
    VRTP_RC_POLL    = 0,
    VRTP_RC_RETRY1  = 1,
    VRTP_RC_RETRY2  = 2,
    VRTP_RC_RETRY3  = 3,
    VRTP_RC_GIVE_UP = 4
} vrtp_rc_states;
    

/**********************************************************************
 *
 *		     VINES RTP Metric Manipulation
 *
 **********************************************************************/

/*
 * vmetric_to_rtp
 *
 * Convert a metric from its internal form to RTP form.
 */
static inline ushort vmetric_to_rtp (ushort metric)
{
    if (metric == VINES_INFINITY)
	return(VINES_INFINITY);
    if (metric <= 0x000F)
	return(1);
    return((metric & 0xFFF0) >> 4);
}

/*
 * vmetric_from_rtp
 *
 * Convert a metric from RTP form to internal form.
 */
static inline ushort vmetric_from_rtp (ushort metric)
{
    if (metric > 0x0FFF)
	return(VINES_INFINITY);
    return((metric & 0x0FFF) << 4);
}

/*
 * vrtp_buginf_metric
 *
 * Pretty print a line for an entry in an RTP packet.  This is called for
 * both transmitted and received RTP packets.
 */
static inline void vrtp_buginf_metric (boolean debug, ulong net, ushort metric)
{
    const char *tag; 
    char  buffer[40];

    if (!debug) {
	return;

    if (!vines_valid_net(net) || !vmetric_valid(metric))
	tag = common_str_invalid;
    } else if (metric == VINES_INFINITY) {
	tag = common_str_unreachable;
    } else {
	sprintf(buffer, vines_seconds_msg, vmetric_to_seconds(metric));
	tag = buffer;
    }
    buginf(vines_network_metric_msg, net, metric, tag);
}


/**********************************************************************
 *
 *		    VRTP Timer Queue Manipulation
 *
 **********************************************************************/

/*
 * The `roll call' timer.
 */
static inline void vrtp_rc_timer_initial_enqueue (vinespathtype *path)
{
    path->rc_state = VRTP_RC_POLL;
    /* Don't enqueue this timer on a snapshot interface */
    if ((path->idb != NULL) &&
	(reg_invoke_snapshot_enabled(path->idb->phys_idb))) {
	return;
    }
    mgd_timer_start(&path->rc_timer, VRTP_RC_INITIAL_TIME);
}

static inline void vrtp_rc_timer_enqueue (vinespathtype *path)
{
    path->rc_state = VRTP_RC_POLL;
    /* Don't enqueue this timer on a snapshot interface */
    if ((path->idb != NULL) &&
	(reg_invoke_snapshot_enabled(path->idb->phys_idb))) {
	return;
    }
    mgd_timer_start(&path->rc_timer, VRTP_RC_TIME);
}

static inline void vrtp_rc_timer_enqueue_retry (vinespathtype *path)
{
    /* Don't enqueue this timer on a snapshot interface */
    if ((path->idb != NULL) &&
	(reg_invoke_snapshot_enabled(path->idb->phys_idb))) {
	path->rc_state = VRTP_RC_POLL;
	return;
    }
    /* do not reset the state here */
    mgd_timer_start(&path->rc_timer, VRTP_RC_RETRY_TIME);
}


/**********************************************************************
 *
 *		    Miscellaneous VRTP Inlines
 *
 **********************************************************************/

/*
 * vrtp_getbuffer
 *
 * Allocate a packet buffer and set it up for use by the RTP code.
 */
static inline paktype *vrtp_getbuffer (int length)
{
    paktype *pak;

    pak = vip_getbuffer(sizeof(vrtp_type) + length);
    if (pak) {
	pak->session_start = pak->transport_start + sizeof(vrtp_type);
	pak->datagramsize = sizeof(vrtp_type) + length;
    } else {
	vroute_no_buffer_out++;
    }
    return(pak);
}


/**********************************************************************
 *
 *			      PROTOTYPES
 *
 **********************************************************************/

/* vines_rtp.c */
extern void vrtp_init(void);
extern void vrtp_send_pak(paktype *, vinespathtype *, int);
extern void vrtp_route_update(ulong, ushort, vinespathtype *, boolean *);
extern void vrtp_display_global(void);
extern void vrtp_display_interface(vinesidbtype *);
extern void vrtp_display_neighbor(vinesneighbortype *);
extern void vrtp_display_timer(void);
extern uchar *vrtp_decode_packet(uchar *, int *, void (*fn )(const char *, ...));
extern void vrtp_send_request(vinesidbtype *, vinespathtype *);
extern void vrtp_send_update(vinesidbtype *, vinespathtype *, boolean, boolean, boolean, uchar);
extern void vrtp_rcv_request(paktype *, vinespathtype *);
extern void vrtp_rcv_response(paktype *, vinespathtype *);
extern void vrtp_send_redirect(paktype *, routingdecision *);
extern void vrtp_rcv_redirect(paktype *, vinespathtype *);
extern void vrtp_send_periodic(vinesidbtype *);
extern void vrtp_process_message(paktype *);
extern void vrtp_rc_timer_expired(vinespathtype *path);
