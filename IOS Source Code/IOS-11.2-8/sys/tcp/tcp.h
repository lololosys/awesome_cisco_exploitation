/* $Id: tcp.h,v 3.10.4.6 1996/06/18 01:49:08 hampton Exp $
 * $Source: /release/112/cvs/Xsys/tcp/tcp.h,v $
 *------------------------------------------------------------------
 * tcp.h -- data structures and definitions for TCP
 *
 * November 1985, Kirk Lougheed
 *
 * Copyright (c) 1985-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: tcp.h,v $
 * Revision 3.10.4.6  1996/06/18  01:49:08  hampton
 * Split the monolithic traffic array into smaller per-protocol traffic
 * arrays.  [CSCdi59224]
 * Branch: California_branch
 *
 * Revision 3.10.4.5  1996/04/27  06:29:33  syiu
 * Branch: California_branch
 * Commit Dial1_branch
 *
 * Revision 3.7.8.3  1996/04/26  00:18:31  syiu
 * Branch: Dial1_branch
 * Dial1_branch to IosSec_postmerge_960419
 *
 * Revision 3.7.8.2  1996/04/16  23:39:42  syiu
 * Branch: Dial1_branch
 * Sync Dial1_branch to V111_1_3
 *
 * Revision 3.7.8.1  1996/01/26  20:25:59  agh
 * Branch: Dial1_branch
 * Initial Checkin of MLPVT Code.
 *
 * Revision 3.10.4.4  1996/04/19  15:33:36  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commit
 *
 * Revision 2.1.6.9  1996/04/10  23:09:20  lol
 * Branch: IosSec_branch
 *         Incorporate code review comments.
 *
 * Revision 2.1.6.8  1996/04/09  22:03:43  che
 * Branch: IosSec_branch
 * Sync to California_branch (960405)
 *
 * Revision 2.1.6.7  1996/03/19  08:47:52  che
 * Branch: IosSec_branch
 * Sync to V111_1_3
 *
 * Revision 2.1.6.6  1996/02/28  06:15:19  lol
 * Branch: IosSec_branch
 * TACACS+ processing.
 *
 * Revision 2.1.6.5  1996/02/21  04:13:17  che
 * Branch: IosSec_branch
 * Sync to 11.1 Mainline (02/20/96)
 *
 * Revision 2.1.6.4  1996/01/23  06:50:22  che
 * Branch: IosSec_branch
 * Sync with 11.1 mainline. (960122)
 *
 * Revision 2.1.6.3  1995/12/19  04:20:03  che
 * Branch: IosSec_branch
 * Re-sync to Arkansas to fix things that broke.
 *
 * Revision 2.1.6.2  1995/12/17  02:32:03  che
 * Branch: IosSec_branch
 * Sync to Arkansas
 *
 * Revision 2.1.6.1  1995/11/17  18:35:16  carrel
 * Branch: IosSec_branch
 * Sync to Arkansas
 *
 * Revision 3.10.4.3  1996/04/15  21:23:42  bcole
 * CSCdi54649:  IP Host Modularity, IP Automatic Stub Routing feature
 * commit
 * Branch: California_branch
 *
 * Revision 3.10.4.2  1996/03/28  00:16:04  jenny
 * CSCdi51402:  TCP Path-MTU Discovery needs to be dynamic
 * Branch: California_branch
 * Implemented TCP PMTU dynamic discovery and PMTU timers (RFC 1191).
 * Moved ip tcp parser command into a seperated parser chain in TCP for
 * modularity.
 *
 * Revision 3.10.4.1  1996/03/18  22:13:38  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.8.2.4  1996/03/16  07:40:58  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.8.2.3  1996/03/14  01:35:24  gstovall
 * Branch: DeadKingOnAThrone_branch
 * The King is now ready for action.
 *
 * Revision 3.8.2.2  1996/03/07  10:52:57  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.8.2.1  1996/02/20  21:21:08  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.10  1996/03/08  00:43:10  lol
 * CSCdi50738:  no timeouts in T+
 *         Implement T+ timeouts for a customer who just won't wait. This
 *         code becomes obsolete in California.
 *
 * Revision 3.9  1996/01/24  20:41:08  jenny
 * CSCdi47046:  TCP/IP need code clean up (part 3)
 * . removed ip/tuba.c, ip/tuba_default.c and ip/tuba.h
 * . removed all includes for ip/tuba.h.
 * . removed more TUBA related code.
 *
 * Revision 3.8  1996/01/16  03:37:57  ahh
 * CSCdi46921:  TCP: remove unused second parameter to tcp_close
 *
 * Revision 3.7  1995/12/28  18:29:11  jenny
 * CSCdi46011:  TCP/IP need to do code clean up.
 * This is the first patch of code changes for TCP/IP code cleanup.
 * . removed all references to TUBA_IP and TUBA_CLNS.
 * . renamed open_tuba_socket() to ip_open_socket().
 * . renamed tcp_tuba_open() to tcp_open().
 * . pass addrtype * instead of ipaddrtype for local and foreight addresses
 *   to tcp_open() for feature portability.
 *
 * Revision 3.6  1995/12/13  04:24:37  ahh
 * CSCdi44230:  Remove dead code
 * Bring out your dead... *clang* (RSRB/TCP fast-switching)
 *
 * Revision 3.5  1995/11/21  21:51:22  jenny
 * CSCdi39128:  TCP needs to do fast retransmit
 * Implemted TCP fast retransmit and recovery algorithm. Fixed a bug in
 * TCP where an out-of-sequence FIN packet with no data got dropped during
 * processing.
 *
 * Revision 3.4  1995/11/21  08:27:25  ahh
 * CSCdi42413:  XOT: cannot tune TCP keepalive behavior
 * Allow keepalive rate to be controlled for XOT connections.
 *
 * Revision 3.3  1995/11/20  23:22:10  ahh
 * CSCdi41919:  PT: cannot specify source interface for TCP connections
 * Add source-interface option to outgoing TCP translation and telnet.
 * Also convert some parser defines to enums.
 *
 * Revision 3.2  1995/11/17  18:48:27  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:35:38  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.8  1995/11/08  21:32:04  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.7  1995/08/18  02:58:21  ahh
 * CSCdi38935:  TCP: TCP Listener should use new process primitives
 * Use process and list primitives for connection queueing support.
 * Clean up some crufties, and delete some conditionally-compiled
 * debug code.
 *
 * Revision 2.6  1995/07/06  21:46:14  ahh
 * CSCdi36734:  RCP: cannot stop file transfer on a VTY
 * Consume VTY input during file transfer so the TTY code will
 * notice when someone hits the escpae char.
 *
 * Revision 2.5  1995/06/22  05:51:15  ahh
 * CSCdi36259:  RLOGIN: pending output not flushed correctly (regress)
 *
 * Revision 2.4  1995/06/20  22:02:01  ahh
 * CSCdi31398:  Rlogin sending window size packet twice
 * Eliminate race conditions in rlogin's urgent-data processing.
 *
 * Revision 2.3  1995/06/17  06:26:29  ahh
 * CSCdi35734:  TCP: Long overdue code reorganization
 * Separate tcp.h into public and private header files.  Shuffle functions
 * around into more logical groupings.  Rename functions to meet naming
 * conventions (say goodbye to StudlyCaps).  Delete unneeded externs.
 * Trim off old log messages.
 *
 * Revision 2.2  1995/06/16  04:29:43  dkatz
 * CSCdi35882:  Time-of-day code needs cleanup
 *
 * Revision 2.1  1995/06/07  23:05:37  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#ifndef __TCP_H__
#define __TCP_H__

#include "packet.h"
#include "../ip/ip.h"
#include "address.h"
#include "login.h"

/*
 * Per-packet flags
 */

#define TCPF_ENQUEUING  0x01	/* enqueuing segment on retransmission q */

/*
 * Queue enumeration type.
 */

#define NUMTCPQUEUES	3	/* Number of queues for a Tcp connection. */
#define RETRANSQUEUE	0	/* unacknowledged data */
#define SEGQUEUE	1	/* unread data */
#define SAVEQUEUE	2	/* data outside rcvnxt value */

/*
 * Timeout enumeration type.
 */

enum tcp_timers { 
    RETRANSTIMEOUT		= 0,	/* Retransmission timeout. */
    TMEWAITTIMEOUT		= 1,	/* Timeout for TimeWait state. */
    ACKTIMEOUT			= 2,	/* Time before an Ack must be sent */
    SNDWNDTIMEOUT		= 3,	/* send window probe interval */
    KEEPALIVETIMEOUT		= 4,	/* idle chatter timeout */
    GIVEUPTIMEOUT               = 5,    /* give up retransmitting */
    PMTUTIMEOUT 		= 6,	/* time since last PMTU ajustment */ 
    NUMTCPTIMEOUTS		= 7	/* Number of Tcp timeout types. */
};

/*
 * Largest possible window size.
 */
#define MAXTCPWINDOW	((ushort) ~0)	/* 65535 */

#define TCP_MAXMD5KEYLEN	(80+1)	/* semi-arbitrary */


/*
 * The Transmission Control Block
 */

typedef void (*tcp_notify_cb)(tcbtype *, void *);
typedef void (*tcp_activate_cb)(tcbtype *, void *);
typedef void (*tcp_listen_cb)(tcbtype *);

/*
 * TCP states.
 *
 * There is some code (unfortunately) which does magnitude comparisons
 * on the TCB state variable, so the actual numerical values are
 * important.
 */

enum tcp_state {
    CLOSED	= 0,
    LISTEN	= 1,
    SYNSENT 	= 2,
    SYNRCVD 	= 3,
    ESTAB	= 4,
    FINWAIT1	= 5,
    FINWAIT2	= 6,
    CLOSEWAIT	= 7,
    LASTACK	= 8,
    CLOSING	= 9,
    TIMEWAIT	= 10
};

struct tcbtype_ {
    tcbtype *next;	/* pointer to next TCB */
    ulong flags;	/* flag bits */
    int lsterr;		/* Contains error code or 0 */
    tt_soc *tty;	/* pointer to owning tty */
    pid_t pid;		/* pid of owning process */
    uint ttynum;	/* number of owning terminal */
    ipparamtype param;	/* IP parameters */
    paktype *curinputpak;/* pointer to current input packet, or NULL */
    paktype *curoutputpak;/* pointer to current output packet, or NULL */
    /*
     * the following data is arranged the same way it is in an actual packet,
     * so that we can bcopy it into the header easily.  *** BEWARE ***
     */
        ushort localport;	/* local port */
        ushort foreignport;	/* remote port */
        ulong sndnxt;	        /* send next */
        ulong rcvnxt;    	/* receive next */
        ushort tcpdefhdr1;	/* has data offset + flags */
        ushort hrcvwnd;		/* xmitted receive window (real one below) */
        ulong tcpdefhdr2;	/* has checksum, urgent ptr (0) */
    /*
     * **** END OF BEWARE
     */
    addrtype foreignhost;	/* remote host */
    addrtype localhost;		/* local host */
    sys_timestamp dallytime;	/* timer used for dallying */
    ulong ipcount;	/* Count of IP's sent (used as IP sequence no.'s) */
    enum tcp_state state;/* current state of tcp finite state machine. */
    int wakeup;		/* non-zero if tiptop process should be activated */
    ushort events;	/* events for wakeup */
    ushort prc;		/* precedence of connection. */
    ipoptiontype *options; /* relevant IP options (eg source route) */
    ushort maxsegmentsize;	/* maximum allowed segment size */
    ushort peer_mss;	/* MSS negotiated by peer */ 
    ulong unreadbytes;	/* count of unread data bytes */
    queuetype q[NUMTCPQUEUES];	/* retrans, seg, and save queues. */
    int qinput;		/* total packets queued for input */
    int qinput_max;     /* maximum in input queue for coalescing trigger */
    int qoutput;	/* total packets queued for output */
    int qsaved;		/* total packets in save queue */
    tcp_notify_cb rxnotify;	/* Pointer to rxnotify service routine */
    void *rxhandle;	/* rxnotify context handle */
    tcp_activate_cb activate;	/* Proc to call when a wakeup event occurs. */
    sys_timestamp timers[NUMTCPTIMEOUTS]; /* the event timers */
    int pending;	/* count of active timers */

    sys_timestamp bad_rst_timestamp;	/* last time we received bad RST/FIN */
    ulong bad_rst_count;		/* number of out of sequence RST/FINs */

    ulong snduna;	/* send unacknowledged */
    ulong sndwnd;	/* send window */
    ulong sndup;	/* send urgent pointer */
    ulong sndwl1;	/* segment sequence number for last window update. */
    ulong sndwl2;	/* segment ack. no. used for last window update. */
    ulong sndmax;
    ulong iss;		/* initial send sequence number. */
    ulong rcvwnd;	/* receive window size */
    ulong delrcvwnd;	/* reduction in receive window size */
    ulong maxrcvwnd;	/* maximum size of receive window */
    ulong addwnd;	/* temporary increase in receive window */
    ulong rcvup;	/* receive urgent pointer */
    short oobbyte;	/* byte of OOB data */
    uchar *oobptr;	/* pointer to OOB data */
    ulong irs;		/* receive initial sequence number */
    ulong brs;		/* bytes taken off sequence number */

    long avglen;	/* smoothed average packet length */
    long srtt;		/* smoothed round trip time (RTT) times 8 */
    long krtt;		/* new round trip time (Karn algorithm) */
    long srtv;		/* smoothed round trip time variance */
    long rtto;		/* round trip timeout */
    long minrtt;	/* smallest recorded RTT */
    long maxrtt;	/* largest recorded RTT */
    long ackholdtime;	/* time to delay ACK's for piggybacking */

    ushort sndcwnd;	/* congestion window */
    ushort sndssthresh;	/* slow start threshold */

    struct curpkttype_ *saved_curpkt;	/* saved stuff for tcb_establish() */
    ulong maxaliveticks; /* for tuning keepalives (ms) */
    ushort maxalivetries; /* for tuning keepalives */
    ulong giveuptime;	/* giveup time (ms) */

    uchar *optstart;	/* context pointer for option building */
    int optbytes;	/* context count for option building */

    ulong timewait_time;/* how long to wait in TIMEWAIT state */
    uchar dupacks; 	/* consecutive duplicated ACKs received */
    uchar msg_refcnt;	/* refcnt for queued messages for a tcb */

    uchar md5_key[TCP_MAXMD5KEYLEN];/* Password for MD5 digests */
    int md5_keylen;

    /* Statistics */

    ulong timeron[NUMTCPTIMEOUTS]; /* count of timer activations */
    ulong timerexp[NUMTCPTIMEOUTS];/* count of timer expirations */

    ulong pakincount;	/* total datagrams received */
    ulong pakoutcount;	/* total datagrams sent (no retransmits) */

    ulong pakinorder;	/* total datagrams received out of order */
    ulong pakoutretrans;/* total datagrams retransmitted */

    ulong pakindata;	/* total input datagrams bearing data bytes */
    ulong pakoutdata;	/* total output datagrams bearing data bytes */

    ulong bytesincount;	/* total data bytes received */
    ulong bytesoutcount;/* total data bytes sent */

    sys_timestamp starttime; /* when TCP connection started */

    ulong internal_flags;		/* misc flags */

    tcbtype *parent;			/* parent of this TCB */

    /*
     * Connection queueing
     * - connq holds a list of TCBs awaiting acceptance by the application
     * - connq_element provides pointers for threading this list
     * - newconn_element provides pointers for threading the children
     *   on the global tcp_newconn_list
     */
    list_header connq;			/* list of pending connections */
    list_element connq_element;		/* element for connection queueing */
    list_element newconn_element;	/* element for conn wake-up queueing */

    tcp_listen_cb listencb;		/* new connection callback */
    boolean (*access_check)(ulong, ulong, ipaddrtype);
    int tcb_synwait;                   /* per-tcb synwait value */
    sys_timestamp timeout;             /* timeout for reads/writes */
};

/*
 * Bit assignments for events
 */

#define TCPEV_IS_CLOSED		0x01
#define TCPEV_MAYBE_OPEN	0x02
#define TCPEV_NOT_LISTEN	0x04
#define TCPEV_NOT_ESTAB		0x08
#define TCPEV_MAYBE_CONNECTED	0x10
#define TCPEV_FROM_ESTAB	0x20
#define TCPEV_WINDOW		0x40
#define TCPEV_RTQUEUE		0x80

/*
 * bits in tcb->flags word
 * Update display routines if you change these.
 */

#define TCB_PASSIVEOPEN	  0x00000001	/* passive open was done */
#define TCB_ACTIVE	  0x00000002	/* connection is now active */
#define TCB_HIGHERPRC	  0x00000004	/* higher precedence allowed */
#define TCB_IDLEUSER	  0x00000008	/* user has idled the connection */
#define TCB_RTRANSTMO	  0x00000010	/* doing retransmit timeout */
#define TCB_SENDURGENT	  0x00000020	/* urgent data is being sent */
#define TCB_KEEPALIVE	  0x00000040	/* keep alive timer is running */
#define TCB_RETXMIT	  0x00000080	/* retransmitting, don't queue output */
#define TCB_ALIAS	  0x00000100	/* TCB local address is an IP alias */
#define TCB_TIMINGOUT	  0x00000200	/* connection is possibly timing out */
#define TCB_OUTPUTPENDING 0x00000400	/* output is waiting to be sent */
#define TCB_SYNCCLOSE	  0x00000800	/* wait for FIN to get acked. */
#define TCB_NAGLE	  0x00001000	/* doing nagle processing */
#define TCB_APP_CLOSED	  0x00002000	/* application has closed TCB */
#define TCB_ALWAYSPUSH	  0x00004000	/* push all packets (tn3270ism) */
#define TCB_SYNCLISTEN	  0x00008000	/* handshake for LISTEN->ESTAB */
#define TCB_FULLSEGS	  0x00010000	/* full-sized segments even if VTY */
#define TCB_PMTUDISC	  0x00020000	/* path MTU discovery capable */
#define TCB_GENTCBS	  0x00040000	/* generate tcbs as passive listener */
#define TCB_MD5		  0x00080000	/* doing MD5 hash on all segments */
#define TCB_EDITURG	  0x00100000	/* "edit" out urgent data (rlogin) */
#define TCB_PMTUAGER	  0x00200000	/* doing path MTU discovery */

#define TCB_ACTIVE_OPEN_FLAGS	(TCB_HIGHERPRC | TCB_RTRANSTMO)
#define TCB_PASSIVE_OPEN_FLAGS	(TCB_PASSIVEOPEN | TCB_RTRANSTMO)

/*
 * bits in tcb->internal_flags
 */
#define TCB_INT_PRIMITIVES	0x00000001
#define TCB_INT_WAITING		0x00000002
#define TCB_INT_NOFREE		0x00000004
#define TCB_INT_MSG_NOTIFY    	0x00000008
#define TCB_INT_WAIT_ESTAB      0x00000020

 
/*
 * signals used for TCB_INT_MSG_NOTIFY message.
 */
enum TCP_MSG_ID {
    TCP_PASSTCB_MSG = 0x80000000
};


/*
 * Properties
 */

enum {
    TCP_WINDOW_SIZE,	/* window size (ulong) */
    TCP_NO_DELAY,	/* Turn off Nagle processing (not impl.) */
    TCP_COND_ACCEPT,	/* conditional accept (int) */
    TCP_TTY,		/* tty pointer (tt_soc *) */
    TCP_IPOPTIONS,	/* ip options (ipoptiontype *) */
    TCP_MD5KEY,		/* md5 key (char [PASSWORDLEN]) */
    TCP_ACCESS_CHECK,	/* routine to call before ACKing SYN (void (*)()) */
    TCP_ADDRESS_TYPE,	/* network protocol, e.g. ADDR_IP (int) */
    TCP_PID,		/* process associated with this TCB (pid_t) */
    TCP_MSG_NOTIFY,     /* Send message to this pid containing the TCB */
    TCP_SYNWAIT         /* per tcb synwait value */
};

/*
 * These ports are used by the TS software to provide special server
 * functions based on line or rotary group number.
 */

#define PORTINTERVAL	1000		/* distance between TCP port classes */

#define LINEBASE		2000	/* telnet line ports */
#define ROTARYBASE		3000	/* telnet rotary ports */
#define STREAMLINEBASE		4000	/* raw TCP line ports */
#define STREAMROTARYBASE	5000	/* raw TCP rotary ports */
#define BINARYLINEBASE		6000	/* telnet binary mode line ports */
#define BINARYROTARYBASE	7000	/* telnet binary mode rotary ports */
#define XREMOTEBASE		8000	/* Xremote connections */
#define REVERSE_XREMOTE_LINE_BASE	9000	/* reverse XRemote telnet to a line */
#define REVERSE_XREMOTE_ROTARY_BASE	10000	/* reverse XRemote telnet to 
					   a rotary group */

#define MAXPRIVEDPORT	(REVERSE_XREMOTE_ROTARY_BASE + PORTINTERVAL)

/*
 * Useful constants
 */
#define MAXPACKETDATALENGTH	(MAXIPPACKETSIZE-40)	/* max data bytes */

#define TcpPkt(A) ((tcptype *) ipdatastart((A)))


/*
 * Return codes from tcp_do_command()
 */
enum {
    TCP_SUCCESS, TCP_NO_TTY, TCP_ACCESS_DENIED, TCP_NO_RESOURCES
};

/*
 * TCP traffic counters
 */
typedef struct tcp_traffic_t_ {
    ulong inputs;
    ulong outputs;
    ulong retrans;
    ulong checksumerr;
    ulong noport;
    ulong actopen;
    ulong pasopen;
    ulong attfail;
    ulong estrest;
    ulong reset;
} tcp_traffic_t;


/***********************************************************************
 *
 *                      Externs and Prototypes
 *
 ***********************************************************************/
extern ulong tcp_defrcvwnd;
extern connvector tcp_connvector;
#ifdef __TTYSRV_H__	/* dumb, but avoids includes of ttysrv.h everywhere */
extern vectortype TCPvector;
#endif
extern idbtype *telnet_source_idb;
extern tcp_traffic_t tcp_traffic;

/*
 * tcpinput.c
 */
extern void tcp_inputsegment(paktype *);
extern void tcp_quench(void *, ipaddrtype);

/*
 * tcpservice.c
 */
extern boolean tcpmiscBLOCK(tcbtype *);

/*
 * tcpsupport.c
 */
extern void tcp_status(tcbtype *);
extern char *tcp_statestring(int);
extern void tcp_show_traffic(void);
extern tcbtype *tcb_findport(ushort);
extern char *tcperror2string(int);
extern tcptype *tcp_get_hdr(paktype *, tcbtype *);

/*
 * tcptop.c
 */
extern tcbtype *tcp_open(tt_soc *, ushort, addrtype *, ushort, addrtype *,
			      int *, ipoptiontype *);
extern int tcp_set_rcvwnd(tcbtype *, ulong);
extern tcbtype *tcp_old_listen(tt_soc *, ushort);
extern int tcp_establish(tcbtype *);
extern void tcp_close(tcbtype *);
extern int tcp_reset(tcbtype *);
extern void tcp_abort(tcbtype *);
extern boolean tcp_putbyte(tcbtype *, uchar, int);
extern int tcp_putstring(tcbtype *, uchar *, int);
extern int tcp_putstringblock(tcbtype *, uchar *, int, int *);
extern boolean tcpputBLOCK(tcbtype *);		/* for bgp.c */
extern void tcp_idle_put(tcbtype *);
extern void tcp_sendurg(tcbtype *);
extern void tcp_putpush(tcbtype *, boolean, boolean);
extern int tcp_getstring(tcbtype *, uchar **, int *, int);
extern int tcp_getstringblock(tcbtype *, uchar **, int *, int);
extern int tcp_getbyte(tcbtype *, int *);
extern int tcp_getbyteblock(tcbtype *, int *);
extern boolean tcp_activity(tcbtype *);
extern int tcp_unread(tcbtype *);
extern boolean tcp_state_activity(tcbtype *);
extern int tcp_lsterr(tcbtype *);
extern boolean tcp_closing(tcbtype *);
extern void tcp_idle(tcbtype *, boolean);
extern ushort tcp_uniqueport(ushort);
extern boolean tcp_reservedport(ushort *);
extern void tcp_putback(tcbtype *, int);
extern tcbtype *tcp_create(tt_soc *, int *, ipoptiontype *);
extern int tcp_bind(tcbtype *, addrtype *, int);
extern int tcp_connect(tcbtype *, addrtype *, int);
extern int tcp_listen(tcbtype *, int, tcp_listen_cb);
extern tcbtype *tcp_accept(tcbtype *, int *);
extern tcbtype *tcp_accept_one(tcbtype **, int, int *);
extern int tcp_read(tcbtype *, void *, unsigned int, int *);
extern int tcp_write(tcbtype *, void *, unsigned int, int *);
extern int tcp_set_property(tcbtype *, int, void *, int);
extern int tcp_get_property(tcbtype *, int, void *, int *);

/*
 * tcpvty.c
 */
extern int tcp_do_command(int, tcbtype *, char *, boolean, boolean,
			  void *, boolean);
extern tcbtype *tty2tcb(tt_soc *, boolean);
extern void tvt_init(void);

/*
 * tcp_chain.c
 */
extern void tcp_parser_init(void);

#endif /* __TCP_H__ */
