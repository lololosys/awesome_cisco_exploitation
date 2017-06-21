/* $Id: apollo.h,v 3.2.62.1 1996/08/03 23:43:49 fox Exp $
 * $Source: /release/112/cvs/Xsys/xns/apollo.h,v $
 *------------------------------------------------------------------
 * apollo.h -- definitions for Apollo Domain protocol family (XNS-like)
 *
 * June 1989,   By Bill Westfield
 *
 * Copyright (c) 1989-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: apollo.h,v $
 * Revision 3.2.62.1  1996/08/03  23:43:49  fox
 * CSCdi63400:  high priority compatibility queue must die
 * Branch: California_branch
 * Remove high priority compatibility queue and activatehigh flag.
 * Convert high priority cfork'd processes to new-style processes with
 * watched queues, timers, and booleans.  Split Multilink PPP input
 * into separate high priority process.  Split load and busy calculations
 * into separate processes.  Speed up PPP input and scheduler processing.
 * Miscellaneous code changes.
 *
 * Revision 3.2  1995/11/17  19:20:28  gstovall
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:56:54  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/09/11  23:58:21  hampton
 * Convert XNS and Apollo to fully use the passive timers macros instead
 * of referencing the system clock directly.  [CSCdi39966]
 *
 * Revision 2.2  1995/06/18 02:24:18  hampton
 * Change the name of an XNS timer variable to something that makes its
 * usage clearer.  While there, remove direct references to msclock.
 * [CSCdi36038]
 *
 * Revision 2.1  1995/06/07 23:26:34  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __APOLLO_H__
#define __APOLLO_H__

/*
 * Apollo-private interface variables.
 */
typedef struct apollosb_ {
    struct idbtype_	*idb;		/* the idb w/ for this sub-block */
    ulong		apollonet;	/* our configured net */
    apollo_acclisttype	*apollo_acclist; /* Pointer to Apollo access list */
    xns_rip_t		xnsrip;		/* GRIP variables */
} apollosb;

/*
 * Apollo packet type
 */

typedef struct apollohdrtype_ {
	ulong destnode;			/* apollo node id */
	uchar pkttype;			/* apollo packet type */
	uchar padding1[3];
	ulong srcnode;			/* apollo source node id */
	uchar version;			/* packet version */
	uchar padding2;
	uchar apollolink;		/* linktype code */
	uchar padding3;
	ushort hdrlen;			/* total size of header */
	ushort hdrdatalen;		/* Length of header data */
	ushort datalen;			/* Length of data portion */	
	ushort transid;			/* Transaction Id */
	uchar routelen;			/* Route field length (always 40) */
	uchar curpos;			/* Current position (always 4) */
	uchar unknown[14];		/* ??? */
					/* standard XNS header follows */
	ushort cksum;			/* checksum */
	ushort len;			/* length in bytes */
	uchar tc;			/* transport control (hop count) */
	uchar pt;			/* packet type */
	ulong PACKED(dnet);		/* destination network */
	uchar dhost[IEEEBYTES];	/* destination host */
	ushort dsock;			/* destination socket */
	ulong PACKED(snet);		/* source network */
	uchar shost[IEEEBYTES];	/* source host */
	ushort ssock;			/* source socket */
} apollohdrtype;
 
#define apolloheadstart(ptr) ((ptr)->network_start)

typedef struct apolloechotype_ {
    struct apolloechotype_ *next;
    tt_soc *tty;
    ulong active;
    sys_timestamp timeout;
    ushort socket;
    ulong deltatime;
    uchar how;
} apolloechotype;

#define APOLLOADDRBYTES 4		/* 20 bits, stored as a long */
#define APOLLOMAXADDR 0xFFFFE		/* FFFFF is broadcast ? */
#define APOLLOMAXNET  0xFFFFFFFF	/* 32 bits */

#define APOLLOOVERHEADBYTES 40
#define APOLLODATASTART(p) (apolloheadstart(p) + APOLLOOVERHEADBYTES + XNSOVERHEADBYTES)
/*
 * APOLLO Packet types
 */

#define	APOLLO_TYPE_RIP		1	/* Routing information protocol */
#define	APOLLO_TYPE_ECHO	0x6e	/* Echo protocol */
#define	APOLLO_TYPE_ERROR	3	/* Error protocol */

/*
 * APOLLO Socket numbers
 */

#define	APOLLO_SOCK_RIP		8	/* Routing information protocol */
#define	APOLLO_SOCK_ECHO	13	/* Echo protocol */
#define APOLLO_RIP_MAXSIZE	1500	
#define	APOLLO_INFINITY	        16

#define	APOLLOHEADERBYTES	30
#define	APOLLO_MINSIZE		30
#define	APOLLO_MAXSIZE		1500
#define	APOLLO_MINECHOSIZE	(70 + 2 + sizeof(sys_timestamp))
#define	APOLLO_MULTICAST	1
#define	APOLLO_FORWMAX		10
#define APOLLO_ADDRBYTES	12

typedef struct {
    ushort	  type;
    sys_timestamp PACKED(time_sent);
} apollo_cisco_ping;

/*
 * APOLLO RIP packet definitions
 */

#define APOLLO_RIP_MAXROUTES 90


#define	APOLLO_ECHO_REQUEST	0x10
#define	APOLLO_ECHO_REPLY	0x20
#define SETECHO_REQUEST(pf) ((pf & APOLLO_TYPE_ECHO) | APOLLO_ECHO_REQUEST)
#define SETECHO_REPLY(pf) ((pf & APOLLO_TYPE_ECHO) | APOLLO_ECHO_REPLY)

/*
 * APOLLO statistic elements
 */

#define	APOLLO_INPUT		0
#define	APOLLO_LOCALDEST		1
#define	APOLLO_BCASTIN		2
#define	APOLLO_FORWARDED		3
#define	APOLLO_BCASTOUT		4
#define	APOLLO_ERRIN		5
#define	APOLLO_ERROUT		6
#define	APOLLO_FORMATERROR		7
#define	APOLLO_CKSUMERR		8
#define	APOLLO_NOTGATEWAY		9
#define	APOLLO_HOPCOUNT		10
#define	APOLLO_NOROUTE		11
#define	APOLLO_ENCAPSFAILED	12
#define	APOLLO_OUTPUT		13
#define	APOLLO_INMULT		14
#define	APOLLO_UNKNOWN		15
#define	APOLLO_FWDBRD		16
#define	APOLLO_ECHOREQIN		17
#define	APOLLO_ECHOREQOUT		18
#define	APOLLO_ECHOREPIN		19

#define	APOLLO_ECHOREPOUT		20

#define	APOLLO_MAXTRAFFIC		21

/*
 * Access lists definitions.
 * Apollo access lists are currently simple net/host based things.
 * It isn't clear that sufficient documentation exists to make access
 * lists that include protocol types or socket numbers useful.
 *
 * Apollo Access lists are named things instead of numbers.
 */
typedef struct apollo_acctype_ {
    struct apollo_acctype_ *next;
    boolean result;		/* permit or deny */
    ulong firstnet;		/* first network in range */
    ulong lastnet;		/* last network in range */
    ulong host;			/* host number */
    ulong hostmask;		/* mask for host number */
} apollo_acctype;

struct apollo_acclisttype_ {
    struct apollo_acclisttype_ *next;
    NAMEDTHINGMACRO
    queuetype q;
};

/***********************************************************************
 *
 *                      Externs and Prototypes
 *
 ***********************************************************************/

/*
 * Apollo Queues
 */
extern queuetype apollobgQ;		/* Apollo services queue */
extern queuetype apollo_accQ;		/* Queue of access lists */
extern queuetype apolloechoQ;

/*
 * Apollo Global Variables
 */
extern boolean apollo_debug, apollorip_debug;
extern boolean apollo_started;
extern ushort apollo_maxpaths;
extern ushort apollo_transid;
extern rip_routetable *apollo_table;
extern uchar apollo_bdcast[];
extern uchar apollo_etheraddr[];
extern ulong apollo_address;
extern ulong apollo_stat[];
extern ulong apollo_update_interval;

/*
 * Apollo Function Declarations
 */
extern void apollo_rxtypes_update(idbtype *);
extern boolean apollo_routeBLOCK(queuetype *);
extern boolean apollo_send(paktype *);
extern boolean apollo_send1(paktype *, uchar *);
extern boolean apollo_validcrc(paktype *);
extern boolean parse_apolloaddress(char *, ulong *, uchar *);
extern forktype apollo_fork(void);
extern process apollo_input(void);
extern forktype apollo_router(void);
extern int apollo_forus(paktype *);
extern void apollo_dispose(int, paktype *);
extern void apollo_enqueue(paktype *);
extern void apollo_forward(paktype *);
extern void apollo_init(subsystype *);
extern void apollo_majorupdate(sys_timestamp *, boolean, boolean);
extern void apollo_printpak(paktype *);
extern void apollo_rcvriprequest(paktype *, xns_riptype *);
extern void apollo_riprcvupd(paktype *, xns_riptype *,
			     rip_routetable *, boolean);
extern void apollo_sendripupdate(idbtype *, ulong, uchar *,
				 ushort, boolean);
extern void apollo_start(void);
extern void apollo_route_adjust(idbtype *);
extern void display_apollo(idbtype *);
extern void show_apollotraffic(void);
extern void apollo_echo(paktype *);
extern boolean send_apolloecho(ulong, uchar *, apolloechotype *, int);
extern void show_apolloaccess(int);
extern boolean apollo_ping_parse(char *, int, int *, char **, boolean *);
extern void apollo_ping(char *);
extern void apollo_parser_init(void);
extern void apollo_arp_display(void);
extern boolean apollo_running;
extern boolean apollo_on_idb(idbtype *);
extern void apollo_media_registry(void);

/*
 * apollo_setup.c
 */
extern void apollo_setup_global(setup_struct *);
extern void apollo_setup_global2(setup_struct *);
extern void apollo_setup_int(setup_struct *);
extern void apollo_setup_end(setup_struct *);

#endif __APOLLO_H__
