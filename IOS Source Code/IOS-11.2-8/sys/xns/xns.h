/* $Id: xns.h,v 3.2.60.3 1996/08/14 02:38:05 hampton Exp $
 * $Source: /release/112/cvs/Xsys/xns/xns.h,v $
 *------------------------------------------------------------------
 * xns.h -- definitions for XNS protocol family
 *
 * November 1987, Greg Satz
 *
 * Copyright (c) 1987-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: xns.h,v $
 * Revision 3.2.60.3  1996/08/14  02:38:05  hampton
 * Use static inline routines to clean up references to the access list
 * array.  [CSCdi65925]
 * Branch: California_branch
 *
 * Revision 3.2.60.2  1996/08/03  23:44:39  fox
 * CSCdi63400:  high priority compatibility queue must die
 * Branch: California_branch
 * Remove high priority compatibility queue and activatehigh flag.
 * Convert high priority cfork'd processes to new-style processes with
 * watched queues, timers, and booleans.  Split Multilink PPP input
 * into separate high priority process.  Split load and busy calculations
 * into separate processes.  Speed up PPP input and scheduler processing.
 * Miscellaneous code changes.
 *
 * Revision 3.2.60.1  1996/05/30  23:49:45  hampton
 * All commands that take access list numbers should accept either numbers
 * in the current monolithic number space, or hidden numbers in the future
 * per-protocol access list spaces.  [CSCdi59067]
 * Branch: California_branch
 *
 * Revision 3.2  1995/11/17  19:25:17  gstovall
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  14:00:35  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.6  1995/09/11  23:58:58  hampton
 * Convert XNS and Apollo to fully use the passive timers macros instead
 * of referencing the system clock directly.  [CSCdi39966]
 *
 * Revision 2.5  1995/08/25 14:12:07  hampton
 * Minor Novell IPX Cleanups.  Remove some unused variables and routines.
 * Eliminate direct references to the system clock.  [CSCdi39328]
 *
 * Revision 2.4  1995/07/12 08:30:15  dstine
 * CSCdi36963:  XNS network numbers print in hex
 *
 * Revision 2.3  1995/07/01  03:39:37  hampton
 * Minor Novell IPX Cleanups.  Remove some unused variables and routines.
 * Eliminate direct references to the system clock.  [CSCdi36668]
 *
 * Revision 2.2  1995/06/18 02:24:24  hampton
 * Change the name of an XNS timer variable to something that makes its
 * usage clearer.  While there, remove direct references to msclock.
 * [CSCdi36038]
 *
 * Revision 2.1  1995/06/07 23:30:27  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __XNS_H__
#define __XNS_H__


#include "access.h"
#include "xnstypes.h"

/*
 * XNS access lists (new numbering)
 */
#define XNS_MIN_STD_ACL   1
#define XNS_MAX_STD_ACL 100
#define XNS_MIN_EXT_ACL 101
#define XNS_MAX_EXT_ACL 200

/* Split horizen algorithem to use */
#define SPLIT_TRUE	(1)
#define SPLIT_FALSE	(2)
#define UB_FUZZY_SPLIT	(3)

/*
 * XNS Packet types
 */

#define	XNS_TYPE_RIP	(1)		/* Routing information protocol */
#define	XNS_TYPE_ECHO	(2)		/* Echo protocol */
#define	XNS_TYPE_ERROR	(3)		/* Error protocol */
#define XNS_TYPE_PEP	(4)		/* Packet Exchange Protocol */
#define XNS_TYPE_UBIPC	(8)		/* UB routing protocol */
#define ALLNETS_TEST    (13)

/*
 * XNS Socket numbers
 */

#define	XNS_SOCK_RIP	1		/* Routing information protocol */
#define	XNS_SOCK_ECHO	2		/* Echo protocol */
#define	XNS_SOCK_ERROR	3		/* Error protocol */
#define XNS_SOCK_UBIPC	0x8a00		/* UB routing protocol */

#define	XNS_INFINITY           16	/* "unreachable" */
#define	XNS_IGRP_MIN_HOPS      32	/* IGRP min tc value (after 0) */
#define	XNS_IGRP_MAX_HOPS     255	/* IGRP max tc value */
#define XNS_MAX_HOPS           15	/* XNS spec max transport control */
#define XNS_TC_HOP_MASK      0x0f	/* Isolate tc hop bits */
#define XNS_TC_RSV_MASK      0xf0	/* Isolate tc reserved bits */

/*
 * XNS Protocol constants.
 */

#define	XNSHEADERBYTES		(30)
#define	XNS_MINSIZE		(30)
#define	XNS_MAXSIZE		(1500)
#define	XNS_MINECHOSIZE		(XNSHEADERBYTES + 2 + sizeof(sys_timestamp))
#define	XNS_MULTICAST_ETHER	(1)
#define XNS_MULTICAST_TR	(0x80)
#define	XNS_FORWMAX		(10)
#define XNS_ADDRBYTES		(12)
#define XNS_ECHODATASTART 	(32)	/* IPX only */
#define	XNS_BROADCAST_NET	(-1L)

typedef struct {
    ushort	  type;
    sys_timestamp PACKED(time_sent);
} xns_cisco_ping;

typedef struct xns_rip_ {
    xnstype	type;			/* who's block is this, anyway? */
    struct idbtype_ *idb;		/* our IDB */
    ulong update_interval;		/* interval to send out updates */
    sys_timestamp updatetimer;        	/* timer for sending out updates */
    sys_timestamp lastupdate;		/* time of last full or flash update */
    int		static_rts;		/* any static routes on this idb? */
    int		changed; 		/* any changes on this idb? */
    tinybool	xns_lineup;		/* Indicates if interface up or Down */
} xns_rip_t;

typedef struct xnssb_ {
    struct idbtype_ *idb;		/* the idb which owns this block */
    ulong    xnsnet;			/* our network number */
    uchar    xns_node[6];		/* node address for this interface */
    uchar    xns_helperaddr[6];
    int      xns_enctype;		/* encapsulation to use for XNS */
    tinybool xns_hear_rip;		/* Hear RIP when box in UB XNS mode? */
    tinybool xns_fld_bcast_allnets;	/* Flood incoming bcasts w/ net==-1? */
    tinybool xns_fld_bcast_netzero;	/* Flood incoming bcast w/ net==0? */
    tinybool xns_fld_spec_allnets;	/* Flood net==-1 and specific host? */
    ulong    xns_hear_rip_list;		/* If so, believe which routes? */
    ulong    xns_helpernet;
    int      xns_helperlist;		/* access list for helper address */
    ulong    xns_accesslist;		/* outgoing access lists */
    ulong    xns_in_filterlist;		/* don't listen to any routing info */
    ulong    xns_out_filterlist;	/* don't pass on any routing info */
    ulong    xns_router_filterlist;	/* don't listen to this guy */
    xns_rip_t xnsrip;			/* real XNS grip variables */
    xns_rip_t ubxnsrip;			/* UB XNS grip variables */
} xnssb;

/*
 * XNS packet type
 */

#define XNS_NETBYTES  4
#define XNS_HOSTBYTES 6
#define XNS_SOCKBYTES 2

#define XNSMACRO \
	ushort cksum;			/* checksum */			     \
	ushort len;			/* length in bytes */		     \
	uchar tc;			/* transport control (hop count) */  \
	uchar pt;			/* packet type */		     \
 	uchar ddnet[XNS_NETBYTES];	/* destination network */	     \
	uchar dhost[IEEEBYTES];		/* destination host */	             \
	ushort dsock;			/* destination socket */	     \
 	uchar ssnet[XNS_NETBYTES];	/* source network */		     \
	uchar shost[IEEEBYTES];		/* source host */ 		     \
	ushort ssock;			/* source socket */

typedef struct xnshdrtype_ {
    XNSMACRO				/* XNS header */
} xnshdrtype;

#define xnsheadstart(ptr) ((ptr)->network_start)

#define	XNSOVERHEADBYTES	30
#define XNSDATASTART(p)  (xnsheadstart(p) + XNSOVERHEADBYTES)

/*
 * XNS RIP packet definitions
 */

#define	RIP_FLASHUPDATE		(2*ONESEC)   /* flash update throttle */

#define RIP_DEFAULT_UPDATE      (30*ONESEC)  /* Default update time */
#define RIP_AGEINTERVAL         (5*ONESEC)   /* Throttle ager runs for XNS */
/* Units for the next two items are update intervals, which may vary
 * from interface to interface.
 */
#define	XNSRIP_INVALIDTIME	3            /* Learned route timeout */
#define	NOVRIP_INVALIDTIME	3            /* Learned route timeout */

#define	RIP_HOLDDOWNTIME	3            /* How long to go (semi-)deaf */
#define RIP_STALETIME           2            /* Start to worry about route */

#define	RIP_MAXSIZE	544		/* max RIP packetsize */

#define	RIP_REQUEST	1		/* request information */
#define	RIP_REPLY	2		/* reply to information */
#define	X_RIP_REQUEST	3		/* Extended request information */
#define	X_RIP_REPLY	4		/* Extended reply to information  */
#define RIP_MAXPATHS    1
#define XNS_RIP_DELAY_TIME     45       /* Interpacket gap for RIP */

#define UB_UPDATE_INTERVAL	(25*ONESEC)   /* Interval between UB updates */
#define UB_HELLO_INTERVAL	(2*ONESEC)    /* Interval between UB hellos */

#define UB_RIP_REPLY	2		/* UB routing "rip" packet */
#define UB_RIP_HELLO	3		/* UB routing "hello" packet */

/* Speeds/delay values for UB */
#define UB_DELAY_9600   25
#define UB_DELAY_19200  19
#define UB_DELAY_56K    15
#define UB_DELAY_128K   12
#define UB_DELAY_256K   10
#define UB_DELAY_512K   6
#define UB_DELAY_1544K  2
#define UB_DELAY_10M    1
/* Lists below used to form a translation table for ub_vis_bw_to_delay */
#define UB_DELAYS       UB_DELAY_10M, UB_DELAY_1544K, UB_DELAY_512K, \
                        UB_DELAY_256K, UB_DELAY_128K, UB_DELAY_56K, \
                        UB_DELAY_19200, UB_DELAY_9600
#define UB_SPEEDS       10000000, 1544000, 512000, \
                        256000, 128000, 56000, \
                        19200, 0
/* Largest delay that makes sense in a UB packet (10 bits, but not all used) */
#define UB_MAXDELAY     999

/* Make sure new entries do NOT conflict with flags in route.h, and add comment
 * saying which new ones do!
 */

typedef struct xns_ripinfotype_ {
	uchar nnet[XNS_NETBYTES];	/* dest. network */
	ushort metric;			/* routing metric */
} xns_ripinfotype;

typedef struct xns_riptype_ {
	ushort cmd;			/* kind of packet */
	xns_ripinfotype PACKED(nets[1]); /* variable length */
} xns_riptype;

typedef struct x_ripinfotype_ {
	uchar nnet[XNS_NETBYTES];	/* dest. network */
	ushort metric;			/* routing metric */
	uchar ggwnet[XNS_NETBYTES];	/* gw net */
	uchar gwhost[XNS_HOSTBYTES];	/* gw host */
} x_ripinfotype;

typedef struct x_riptype_ {
	ushort cmd;			/* kind of packet */
	x_ripinfotype PACKED(nets[1]); 	/* variable length */
} x_riptype;

/* Path descriptors, one per possible path */

typedef struct pathentrytype_ {
    struct pathentrytype_ *next;	/* next ptr */
    ulong srcnet;			/* src net */
    uchar srchost[IEEEBYTES];		/* src host */
    ushort pad1;
    ulong uses;				/* uses */
    sys_timestamp timer;		/* per path expiry timer */
    idbtype *idb;			/* XNS physical interface */
    struct xns_rip_ *grip;		/* ptr to generic RIP variables */
} pathentrytype;

/*
 * RIP routing table definitions
 */

typedef struct xns_ripentrytype_ {
    struct xns_ripentrytype_ *next;	/* next ptr */
    ulong flags;			/* type flags */
    sys_timestamp holddownuntil;        /* hold down timer */
    sys_timestamp started;		/* when route installed */
    sys_timestamp changed;		/* time last changed; flash upd supt */
    ulong dstnet;			/* dest. net */
    ushort metric;		        /* Routing metric */
    ushort hopcount;		        /* Not necessarily == metric (UB) */
    ushort delay;		        /* Units set by upper protocol... */
    ushort nettype;                     /* What type of net is this ? */
    ulong paths;                        /* number of paths to this dest. */
    boolean valid; 
    boolean unnumbered;
    struct xns_rip_ *grip;		/* ptr to generic RIP values */
    pathentrytype *nextpath;		/* next path to use */
    pathentrytype *path_list;           /* path descriptors */
} xns_ripentrytype;

typedef xns_ripentrytype *rip_routetable[NETHASHLEN];

typedef struct xnsechotype_ {
    struct xnsechotype_ *next;
    tt_soc *tty;
    ulong active;
    sys_timestamp timeout;
    ulong deltatime;
    ushort socket;
    uchar how;
    uchar method;
    boolean echo_test;
    ushort len;
    ulong seed;
    ushort csum;
} xnsechotype;

#define	XNS_ECHO_REQUEST	1
#define	XNS_ECHO_REPLY		2


/*
 * XNS Error packet type
 */

typedef struct xnserrtype_ {
	ushort errnum;			/* error number */
	ushort errparam;		/* error parameter */
	uchar errpak[XNSHEADERBYTES];	/* error packet header */
	uchar errlev2[12];		/* this many bytes of level 2 data */
} xnserrtype;

#define	XNS_ERR_UNSPEC	0		/* Unspecified error at destination */
#define	XNS_ERR_BADSUM	1		/* Bad checksum at destination */
#define	XNS_ERR_NOSOCK	2		/* Socket nonexistant @ destination */
#define	XNS_ERR_FULL	3		/* Destination is full */
#define	XNS_ERR_TUNSPEC	0x200		/* Unspecified error in transit */
#define	XNS_ERR_TBADSUM	0x201		/* Bad checksum in transit */
#define	XNS_ERR_UNREACH 0x202		/* Destination unreachable */
#define	XNS_ERR_2OLD	0x203		/* Hop count exceeded */
#define	XNS_ERR_2BIG	0x204		/* Packet too large */
#define	XNS_ERR_CONGW	0x205		/* Congestion warning. Q about full */
#define	XNS_ERR_CONGE	0x206		/* Congestion error. Packet dropped */

#define PEPMACRO \
	ulong PACKED(pep_id);		/* transaction identifier */ \
	ushort pep_client_type;		/* client of the protocol */ 

typedef struct xnspephdrtype_ {
    XNSMACRO				/* XNS header */
    PEPMACRO				/* PEP header */
} xnspephdrtype;

/*
 * XNS statistic elements
 */

#define	XNS_INPUT		0
#define	XNS_LOCALDEST		1
#define	XNS_BCASTIN		2
#define	XNS_FORWARDED		3
#define	XNS_BCASTOUT		4
#define	XNS_ERRIN		5
#define	XNS_ERROUT		6
#define	XNS_FORMATERROR		7
#define	XNS_CKSUMERR		8
#define	XNS_NOTGATEWAY		9
#define	XNS_HOPCOUNT		10
#define	XNS_NOROUTE		11
#define	XNS_ENCAPSFAILED	12
#define	XNS_OUTPUT		13
#define	XNS_INMULT		14
#define	XNS_UNKNOWN		15
#define	XNS_FWDBRD		16
#define	XNS_ECHOREQIN		17
#define	XNS_ECHOREQOUT		18
#define	XNS_ECHOREPIN		19
#define	XNS_ECHOREPOUT		20

#define	XNS_MAXTRAFFIC		21


/***********************************************************************
 *
 *                      Externs and Prototypes
 *
 ***********************************************************************/

/*
 * XNS Queues
 */
extern queuetype xnsripQ;		/* XNS RIP socket queue */
extern queuetype xnsechoQ;		/* XNS echo queue */

/*
 * XNS Global Variables
 */
extern long xnscache_version;
extern boolean xns_ub_emulation;
extern boolean xns_purgeflag;	        /* Routing table needs purging? */
extern rip_routetable *xns_table;	/* routing table for XNS */
extern ulong xns_routes;
extern ulong xns_stat[];
extern ushort xns_maxpaths;
extern boolean xns_running;
extern boolean xns_rip_delay;           /* interpacket rip gap */
/*
 * XNS inline functions
 */
/* When redistributing from RIP (hopcount metrics) into UB routing protocol
 * (delay metrics, but keeps hopcounts), we need to guess at a delay based
 * on the hop count. For now, we just multiply by the longest delay UB
 * ever uses for any interface type... which a macro could do, but what
 * the hell.
 */
static inline ushort ub_hops_to_delay (ushort hopcount)
{
    return (UB_DELAY_9600 * hopcount);
}

/*
 * XNS Access list support.
 */
/*
 * The structure definitions and typedefs should eventually migrate here.
 * When they do, the following definitions can use the typedef instead of
 * the explicit structure name.
 */

static inline struct xnsnacctype_ *xns_get_std_acl (int number)
{
    return((struct xnsnacctype_ *)accessarray[number]);
}

static inline void xns_set_std_acl (int number, struct xnsnacctype_ *acl)
{
    accessarray[number] = (accessptr *)acl;
}

static inline struct xnseacctype_ *xns_get_ext_acl (int number)
{
    return((struct xnseacctype_ *)accessarray[number]);
}

static inline void xns_set_ext_acl (int number, struct xnseacctype_ *acl)
{
    accessarray[number] = (accessptr *)acl;
}

/*
 * XNS Function Declarations
 *     xns/xnsecho.c
 *     xns/xnsinput.c
 *     xns/xnsroute.c
 *     xns/xns_chain.c
 *     sun/xnsasm.s
 */

extern boolean parse_xnsaddress(char *, ulong *, uchar *);
extern boolean send_xnsecho(long, uchar *, xnsechotype *, int);
extern boolean xns_accesscheck(paktype *, int);
extern boolean xns_allnets_test(paktype *);
extern boolean xns_filtercheck(ulong, int);
extern boolean xns_flood(paktype *);
extern boolean xns_forme(paktype *);
extern boolean xns_forwarding(paktype *);
extern boolean xns_routeBLOCK(queuetype *);
extern boolean xns_send(paktype *);
extern boolean xns_send1(paktype *, uchar const *);
extern boolean xns_validcrc(paktype *);
extern boolean xnsechoBLOCK(xnsechotype *);
extern ushort ub_vis_bw_to_delay(ulong);
extern forktype xns_background(void);
extern forktype xns_router(void);
extern int xns_forus(paktype *);
extern ushort xns_crc1(uchar *);
extern void display_xns(idbtype *);
extern void nv_xnsaccess(parseinfo *);
extern void show_xnsaccess(int);
extern void show_xnstraffic(void);
extern void ub_hello(idbtype *, xnssb *);
extern ushort ub_gethops(ushort);
extern ushort ub_getdelay(ushort);
extern ushort ub_packmetric(ushort, ushort);
extern void xns_crc(paktype *);
extern void xns_dispose(int, paktype *);
extern void xns_echo(paktype *);
extern void xns_enqueue(paktype *);
extern void xns_error(paktype *, int, int, int);
extern void xns_forward(paktype *);
extern void xns_route_adjust(idbtype *swidb);
extern void xns_media_registry(void);
extern void xns_init_idb(idbtype *idb, boolean);
extern void xns_init(subsystype *);
extern void xns_majorupdate(sys_timestamp *, boolean, boolean);
extern void xns_qcsum(uchar *, int, int);
extern void xns_rcvrequest(paktype *, xns_riptype *);
extern void xns_riprcvupd(paktype *, xns_riptype *);
extern void xns_ubrcvupd(paktype *, xns_riptype *);
extern void xns_sendrequest(void);
extern void xns_sendupdate(idbtype *, ulong, uchar const *, ushort,
			   boolean, boolean, xns_rip_t *);
extern void xns_start(void);
extern void xns_xrcvrequest(paktype *, x_riptype *);
extern void xns_xsendupdate(idbtype *, ulong, uchar const *, ushort,
			    boolean, boolean);
extern boolean xns_ping_parse(char *, int, int *, char **, boolean *);
extern void xns_ping(char *);
extern void clear_xns_command(parseinfo *);
extern void xns_parser_init(void);

extern xnssb *xns_init_sb(xnssb *);
/*
 * Generalized RIP (grip) Prototypes
 */
extern boolean grip_interface_up(pathentrytype *);
extern boolean grip_same_idb(idbtype *, pathentrytype *);
extern xns_ripentrytype *grip_find_rt(ulong, rip_routetable *);
extern xns_ripentrytype *grip_getnetwork(idbtype *, boolean, int *,
					 xns_ripentrytype **, rip_routetable *,
					 boolean, sys_timestamp);
extern uchar *grip_first_hop(ulong, uchar *, idbtype **, xns_ripentrytype *);
extern void grip_add_rt(xns_rip_t *, ulong, ulong, uchar *, ulong,
			rip_routetable *, boolean, boolean, boolean);
extern void grip_ager(rip_routetable *, ulong *, char *, ushort, sys_timestamp *, boolean);
extern void grip_del_rt(ulong, boolean, rip_routetable *, boolean, boolean);
extern void grip_prune_idb(idbtype *, rip_routetable *, ulong *, char *,
			   boolean);
extern void grip_route_adjust(xns_rip_t *,
			      rip_routetable *,
			      int,
			      sys_timestamp *,
			      boolean,
			      ulong *,
			      boolean);
extern void grip_static_routes(rip_routetable *, xns_rip_t *, boolean, sys_timestamp *);
extern void grip_update(xns_rip_t *,
			ulong,
			ushort,
			ulong,
			uchar *,
			rip_routetable *,
			ulong *,
			char *,
			sys_timestamp *,
			ushort,
			ushort,
			ushort,
			boolean);
extern void nv_grip_route(rip_routetable *, char *);
extern void show_griproutes(long, rip_routetable *, int, ushort, int);
extern void grip_purgetable(rip_routetable *);


/*
 * xns_setup.c
 */
extern void xns_setup_global(setup_struct *);
extern void xns_setup_global2(setup_struct *);
extern void xns_setup_int(setup_struct *);
extern void xns_setup_end(setup_struct *);

/*
 * xns_test.c
 */
extern void xns_test_init(void);

#endif __XNS_H__
