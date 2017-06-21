/* $Id: vines_ipc.h,v 3.2 1995/11/17 17:57:57 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/vines/vines_ipc.h,v $
 *------------------------------------------------------------------
 * vines_ipc.h - Support for the vines InterProcess Commications (IPC)
 *               protocol.
 *
 * February 1993, David Hampton
 *
 * Copyright (c) 1993-1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: vines_ipc.h,v $
 * Revision 3.2  1995/11/17  17:57:57  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:44:46  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/07/24  22:32:56  hampton
 * Further work to parts of VINES to make it use the passive timer macros
 * for all its timers.  It no longer have any direct references to the
 * system clock.  [CSCdi37584]
 *
 * Revision 2.1  1995/06/07 23:14:30  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Common external variables
 */
extern boolean vinesipc_debug;


/**********************************************************************
 *
 *	   VINES INTERPROCESS COMMUNICATION (IPC) PROTOCOL
 *
 **********************************************************************/

/*
 * Vines IPC header structures.
 */
struct vinesipctype_ {			/* reliable */
    ushort srcport;
    ushort dstport;
    uchar  type;
    uchar  control;
    ushort local_id;
    ushort remote_id;
    ushort sequence;
    ushort ack;
    ushort length;			/* length or error code */
    uchar  data[0];
};

typedef struct vinesipc2type_ {		/* unreliable, datagram only */
    ushort srcport;
    ushort dstport;
    uchar  type;
    uchar  control;
    uchar  data[0];
} vinesipc2type;


/**********************************************************************
 *
 *	    VINES SEQUENCED PACKET PROTOCOL (SPP) PROTOCOL
 *
 **********************************************************************/

/*
 * Vines Sequenced Packet Protocol (SPP) header structure definition.
 */
typedef struct vinesspptype_ {
    ushort srcport;
    ushort dstport;
    uchar  type;
    uchar  control;
    ushort local_id;
    ushort remote_id;
    ushort sequence;
    ushort ack;
    ushort window;
    uchar  data[0];
} vinesspptype;


/**********************************************************************
 *
 *	       VINES SHARED TRANSPORT LAYER DEFINITIONS
 *
 **********************************************************************/

/*
 * IPC port number ranges
 */
#define VIPC_PORT_UNKNOWN	   0x0000  /* unused */
#define VIPC_PORT_FIRST_WKP	   0x0001
#define VIPC_PORT_LAST_WKP	   0x01FF
#define VIPC_PORT_FIRST_AVAIL	   0x0200
#define VIPC_PORT_LAST_AVAIL	   0xFFFE
#define VIPC_PORT_TRANSIENT	   0xFFFF  /* unused */

/*
 * IPC well known port numbers
 */
#define VIPC_PORT_ECHO		   0x0001
#define VIPC_PORT_ROUTER	   0x0002
#define VIPC_PORT_BACKEND	   0x0003
#define VIPC_PORT_MAIL		   0x0004
#define VIPC_PORT_FTP		   0x0005
#define VIPC_PORT_FILE		   0x0006
#define VIPC_PORT_SERVER	   0x0007
#define VIPC_PORT_BKGND		   0x0009
#define VIPC_PORT_ST		   0x000f
#define VIPC_PORT_TALK		   0x0011
#define VIPC_PORT_NM		   0x0012
#define VIPC_PORT_VS		   0x0013
#define VIPC_PORT_TUNNEL_IP	   0x0014
#define VIPC_PORT_TUNNEL_ARP	   0x0015
#define VIPC_PORT_STDA		   0x0016
#define VIPC_PORT_STDA_LISTEN	   0x0017
#define VIPC_PORT_NSM		   0x0019
#define VIPC_PORT_TUNNEL_ATALK	   0x001F
#define VIPC_PORT_TUNNEL_IPX	   0x0021
#define VIPC_PORT_VINES_ASSISTANT  0x015A
#define VIPC_PORT_BIG_EAR_LISTENER 0x0176  /* decimal 374 */
	/* See Vines Observer, Volume 4, Issue 2 */

/*
 * Other IPC port numbers
 */
#define VIPC_PORT_MAXCOUNTED	   0x001A  /* For cisco counts */
#define VIPC_PORT_CISCO_PROXY	   0x01FF  /* cisco hack */

#define FOR_ALL_VIPC_PORTS_COUNTED() \
    for (i = VIPC_PORT_FIRST_WKP; i < VIPC_PORT_MAXCOUNTED; i++)

/*
 * IPC and SPP type numbers
 */
#define VIPC_DATAGRAM	    0 /* IPC only - unreliable	*/
#define VIPC_DATA	    1 /*	    w/retrans	*/
#define VIPC_ERROR	    2 /* IPC only - errors	*/
#define VIPC_DISC	    3 /*	    disconnect	*/
#define VIPC_PROBE	    4 /*	    probe	*/
#define VIPC_ACK	    5 /*	    acknowledge */
#define VIPC_LAST	    5

/*
 * IPC and SPP control values
 */
#define VIPC_CTL_ACK	0x80
#define VIPC_CTL_BOMEOM 0x60
#define VIPC_CTL_EOM	0x40
#define VIPC_CTL_BOM	0x20
#define VIPC_CTL_ABORT	0x10
#define VIPC_CTL_NONE	0x00

/*
 * IPC and SPP connection values
 */
#define VIPC_CONN_FIRST	0x0001
#define VIPC_CONN_LAST	0xFFFE

/*
 * IPC and SPP miscellaneous values
 */
#define VIPC_MAXDATA_ONE   1450
#define VIPC_MAXDATA       5800


/**********************************************************************
 *
 *			LOCAL DATA DTRUCTURES
 *
 **********************************************************************/

/*
 * Vines proxy structure
 *
 * This structure is used to keep track of packets that the cisco is
 * currently "proxying" for.  This is a really gross, ugly, hack, but
 * mabye it can prevent future problems.
 */
typedef struct vinesproxytype_ {
    struct vinesproxytype_ *next;
    vinesipcsocket *proxy_socket;	/* what server was it sent to */
    vinesipcsocket *query_socket;	/* what client sent it        */
    ushort          query_from_port;    /* (port in the socket may change) */
    ushort          query_to_port;      /* (port in the socket may change) */
} vinesproxytype;

/*
 * IPC 'Connection' structure
 */
#define VIPC_RETRY_COUNT     10

enum SOCKETSTATE {
    SS_NULL,		/* unknown */
    SS_CONNECTING,	/* connection in progress */
    SS_CONNECTED,	/* connected, data transfer state */
    SS_IDLE,		/* in idle state */
    SS_DEAD		/* dead, waiting to be reclaimed */
    };

struct vinesipcsocket_ {
    vinesipcsocket *next;
    enum SOCKETSTATE state;

    ulong     remote_net;
    ushort    remote_host;
    ushort    remote_id;
    ushort    local_id;
    ushort    last_remote_port;
    ushort    last_local_port;
    ushort    last_rcvd_seq;	/* seq number of last received packet  */
    ushort    last_send_seq;	/* seq number of next outgoing packet  */
    ushort    last_send_ack;	/* last ack number sent to remote side */
    ushort    next_send_ack;	/* next ack number to send to remote   */
 
    ulong     servermetric;	/* in ticks */
    ulong     lasthop;		/* in ticks */
    ulong     bias;		/* in ticks */
    ulong     totalcost;	/* sum of above in msec */
    ulong     need_metric;      /* need metric notification for last hop */

    queuetype retransQ;
    paktype   *reassembly;	/* packet to build reassembled frame */
    
    mgd_timer next_ack_time;
    mgd_timer next_idle_time;
    mgd_timer next_retrans_time;

    vinesidbtype *last_idb_in;
    int       last_hop_count;
    boolean   last_was_broadcast;
    int       last_netrpc_id;
};

#define VIPC_NEED_NOTHING      0x0000
#define VIPC_NEED_LAST_HOP     0x0001	/* Need last hop informaion */
#define VIPC_NEED_VALID_METRIC 0x0002	/* Server metric is invalid */

#define VIPC_DEFAULT_METRIC    (2 * V_METRIC_1200)
				/* Default til real metric learned */

#define retransmit_count unspecified[0]
#define retransmit_time  gp_timer

#define FOR_ALL_VIPC_SOCKETS(socket) \
    for ((socket) = vines_socketQ.qhead; (socket); (socket) = (socket)->next)

#define FOR_ALL_VIPC_SOCKETS_W_NEXT(socket, next) \
    for ((socket) = vines_socketQ.qhead; (socket); (socket) = next)

#define FOR_ALL_VIPC_SOCKETS_W_PREV(socket, prev) \
    (prev) = (vinesipcsocket *)&vines_socketQ.qhead; \
    (socket) = vines_socketQ.qhead; \
    for ( ; (socket); (prev) = (socket), (socket) = (socket)->next)

typedef void (vipc_upcall)(vinesipcsocket *, uchar *, int);

/*
 * IPC 'Port' structure.  This mainteins the name of the port's owner,
 * whether or not it uses the RPC layer, and what upcall to make for the
 * various packets.
 */
typedef struct vipc_port_info_ {
    struct vipc_port_info_ *next;

    ushort        number;
    char         *owner;
    boolean       uses_netrpc;
    vipc_upcall  *raw_ipc;
} vipc_port_info;

#define FOR_ALL_VIPC_PORTS(port) \
    for ((port) = vipc_portQ.qhead; (port); (port) = (port)->next)

#define FOR_ALL_VIPC_PORTS_W_PREV(port, prev) \
    (prev) = NULL; \
    (port) = vipc_portQ.qhead; \
    for ((port) = vipc_portQ.qhead; (port); (prev) = (port), (port) = (port)->next)

/***********************************************************************
 *
 *			       INLINES
 *
 ***********************************************************************/

static inline paktype *vipc_getbuffer (int length)
{
    paktype *pak;

    pak = vip_getbuffer(sizeof(vinesipctype) + length);
    if (pak) {
	pak->session_start = pak->transport_start + sizeof(vinesipctype);
	pak->datagramsize = sizeof(vinesipctype) + length;
    }
    return(pak);
}

static inline paktype *vipc_short_getbuffer (int length)
{
    paktype *pak;

    pak = vip_getbuffer(sizeof(vinesipc2type) + length);
    if (pak) {
	pak->session_start = pak->transport_start + sizeof(vinesipc2type);
	pak->datagramsize = sizeof(vinesipc2type) + length;
    }
    return(pak);
}

/*
 * vmetric_to_ipc
 *
 * Convert an internal metric into its millisecond value.
 */
static inline int vmetric_to_ipc (ushort metric)
{
    return(metric * 125 / 10);
}

/*
 * vipc_send_datagram
 *
 * Send an unsequenced, unacknowledged message.  This routine is normally
 * used to send broadcast messages.
 */
extern vinesidbtype *vipc_send_datagram_common(paktype *, ulong, ushort, ulong,
					       ushort, ushort, ushort, int);

static inline vinesidbtype *vipc_send_datagram (
    paktype *pak,
    ulong dnet,
    ushort dhost,
    ushort dport,
    ushort sport,
    int ttl)
{
    return(vipc_send_datagram_common(pak, dnet, dhost, vines_network,
				     vines_host, dport, sport, ttl));
}

static inline vinesidbtype *vipc_send_datagram_alt_src (
    paktype *pak,
    ulong  dnet,
    ushort dhost,
    ulong  snet,
    ushort shost,
    ushort dport,
    ushort sport,
    int ttl)
{
    return(vipc_send_datagram_common(pak, dnet, dhost, snet, shost, dport,
				     sport, ttl));
}

/***********************************************************************
 *
 *
 ***********************************************************************/

/*
 * Vines Globals
 */
extern ulong vines_proxied[];	/* proxied to real server */

/*
 * Vines Function Declarations
 */

/* vines_ipc.c */
extern vinesipcsocket *vipc_create_conn(ulong, ushort, ushort, ushort, boolean);
extern vinesipcsocket *vipc_get_conn(ushort, ushort, boolean);
extern vinesipcsocket *vipc_get_conn_by_address(ulong, ushort);
extern void vipc_kill_conn(vinesipcsocket *, char *, boolean);
extern void vipc_user_clear_conn(parseinfo *csb);
extern void vipc_send_broadcast_datagram(paktype *, vinesidbtype *, vinesidbtype *, ushort, ushort, int, int);
extern vinesidbtype *vipc_send_data(vinesipcsocket *, paktype *, ushort, ushort, int);
extern vinesidbtype *vipc_send_probe(vinesipcsocket *);
extern boolean vipc_proxy_packet(vinesipcsocket *, paktype *);
extern void vines_proxy_reply(vinesipcsocket *, uchar *, int);
extern void vipc_display_timer(void);
extern void vines_flush_proxy_queue(vinesipcsocket *);
extern vipc_port_info *vipc_find_port(ushort);
extern vipc_port_info *vipc_create_port(ushort, char *, boolean, vipc_upcall *);
extern void vipc_destroy_port(vipc_port_info *);
extern void vipc_destroy_port_number(ushort);
extern vinesproxytype *vines_create_proxy(vinesipcsocket *, vinesipcsocket *);
extern void vines_process_connections(void);
extern void vipc_ack_timer_expired(mgd_timer *);
extern void vipc_idle_timer_expired(mgd_timer *);
extern void vipc_retrans_timer_expired(mgd_timer *);
extern void vipc_traffic_init(void);
extern void vipc_traffic_checkpoint(void);
extern void vipc_display_traffic(void);
extern char *vipc_portname(ushort);
extern void vipc_network_error(vinesiptype *, vinesipctype *);

/* vines_netrpc.c */
extern boolean vines_netrpc_receive(vinesipcsocket*, uchar *, int);
extern uchar *display_vinespak_netrpc(uchar *, int *, void (*fn)(const char *fmt, ...));

/* vines_services.c */
extern void vsvc_display_timer(void);
extern void vsvc_show_services(parseinfo *csb);
