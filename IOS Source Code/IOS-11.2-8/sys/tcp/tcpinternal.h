/* $Id: tcpinternal.h,v 3.6.10.3 1996/08/07 09:02:53 snyder Exp $
 * $Source: /release/112/cvs/Xsys/tcp/tcpinternal.h,v $
 *------------------------------------------------------------------
 * tcpinternal.h -- internal data structures and definitions for TCP
 *
 * November 1985, Kirk Lougheed
 *
 * Copyright (c) 1985-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: tcpinternal.h,v $
 * Revision 3.6.10.3  1996/08/07  09:02:53  snyder
 * CSCdi65343:  more things that should be declared const
 *              char * [] -> char *const []
 *              char *    -> const char []
 *              savings 17,496 data space, 1,852 image space
 * Branch: California_branch
 *
 * Revision 3.6.10.2  1996/04/15  21:23:56  bcole
 * CSCdi54649:  IP Host Modularity, IP Automatic Stub Routing feature
 * commit
 * Branch: California_branch
 *
 * Revision 3.6.10.1  1996/03/28  00:16:12  jenny
 * CSCdi51402:  TCP Path-MTU Discovery needs to be dynamic
 * Branch: California_branch
 * Implemented TCP PMTU dynamic discovery and PMTU timers (RFC 1191).
 * Moved ip tcp parser command into a seperated parser chain in TCP for
 * modularity.
 *
 * Revision 3.6  1996/02/23  23:38:50  ahh
 * CSCdi47841:  Running some telnet/pad sessions over serial causes rout...
 * Allow tcp_uniqueport() to fail, and clean up some debug messages.
 *
 * Revision 3.5  1996/02/20  19:45:47  ahh
 * CSCdi49358:  TCP: malformed options cause heartburn
 * Sanity check the option lengths.
 *
 * Revision 3.4  1996/01/11  02:33:54  jenny
 * CSCdi46513:  TCP/IP need code clean up (part 2)
 * changed registry function ip_tuba_move_address to inline
 * ip_copy_address().
 * . removed all references to LINK_CLNS and ADDR_CLNS in tcp/ip source.
 * . removed function tcp_sendclns().
 * . changed registry function ip_tuba_move_address to inline
 * ip_copy_address().
 *
 * Revision 3.3  1995/12/13  04:24:46  ahh
 * CSCdi44230:  Remove dead code
 * Bring out your dead... *clang* (RSRB/TCP fast-switching)
 *
 * Revision 3.2  1995/11/21  21:51:29  jenny
 * CSCdi39128:  TCP needs to do fast retransmit
 * Implemted TCP fast retransmit and recovery algorithm. Fixed a bug in
 * TCP where an out-of-sequence FIN packet with no data got dropped during
 * processing.
 *
 * Revision 3.1  1995/11/09  13:36:04  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/08/18  02:58:25  ahh
 * CSCdi38935:  TCP: TCP Listener should use new process primitives
 * Use process and list primitives for connection queueing support.
 * Clean up some crufties, and delete some conditionally-compiled
 * debug code.
 *
 * Revision 2.3  1995/07/29  00:21:54  ahh
 * CSCdi37655:  TCP: Listener process should be started on demand
 * Start it on demand, and make it low priority.
 *
 * Revision 2.2  1995/07/12  23:45:41  hampton
 * Convert TCP to fully use the passive timers macros instead of
 * referencing the system clock directly.  [CSCdi37015]
 *
 * Revision 2.1  1995/06/17 06:29:10  ahh
 * CSCdi35734:  TCP: Long overdue code reorganization
 * Separate tcp.h into public and private header files.  Shuffle functions
 * around into more logical groupings.  Rename functions to meet naming
 * conventions (say goodbye to StudlyCaps).  Delete unneeded externs.
 * Trim off old log messages.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __TCPINTERNAL_H__
#define __TCPINTERNAL_H__

#define MAXALIVETICKS (ONEMIN)  /* do keep alive action at this interval */
#define MAXRETRYTICKS (ONEMIN)  /* try this long to get data thru */
#define MAXSYNTRYTICKS (15*ONESEC) /* try this long to get a SYN thru */
#define MAXALIVETRIES 4		/* allow this many keep alive failures */
#define MAXTMOSHIFT 5		/* maximum retransmit backoff exponent */
#define MINRTT 300		/* minimum allowed S-RTT (0.3 seconds) */
#define MAXRTT (ONEMIN)		/* maximum allowed S-RTT (60 seconds) */
#define MAXSEGLIFETIME	(2*ONEMIN)	/* maximum segment lifetime (2 mins) */

/* TCP sequence number comparison macros. */
#define	LT(a,b)		((long)((a)-(b)) < 0)
#define	LEQ(a,b)	((long)((a)-(b)) <= 0)
#define	GT(a,b)		((long)((a)-(b)) > 0)
#define	GEQ(a,b)	((long)((a)-(b)) >= 0)

/* TCP port number comparison macros. */
#define	SLT(a,b)	((short)((a)-(b)) < 0)
#define	SLEQ(a,b)	((short)((a)-(b)) <= 0)
#define	SGT(a,b)	((short)((a)-(b)) > 0)
#define	SGEQ(a,b)	((short)((a)-(b)) >= 0)

#define TCBHASHSIZE	64	/* must be power of 2 */
#define tcbhash(lp,fp,fh) \
    (nethash((lp) + ((fp) ^ ((fh).ip_addr))) & (TCBHASHSIZE-1))

/*
 * Maximum number of pending connections per TCB.
 * Somewhat arbitrary.
 */
#define TCP_MAXCONNQLIMIT	20
#define TCP_MAXDUPACKS          3


/* 
 * Default values for "ip tcp" configuration commands 
 */
#define DEF_QUEUEMAX_SYS  20	/* Default queue max for system connections */
#define DEF_QUEUEMAX_USER 5	/* Default queue max for user connections */
#define QUEUEMAX (tcb->tty ? tcp_queuemax : DEF_QUEUEMAX_SYS)
				   /* Allow 20 on standalone connections */
#define DEF_SYNWAIT	30	/* Default time to wait when sending SYN */
#define DEF_FINWAITTM	5	/* Default time waiting to reach CLOSED */
#define DEF_PATHMTU	FALSE	/* Path-MTU discovery off by default (wimp) */
#define DEF_PATHMTU_AGER 10	/* Default time for PMTU age-timer */
#define DEF_TUNNEL_ENABLE	FALSE
#define MAX_LISTENQUEUE_SIZE  20 /* Max size for listen queue */ 

/* 
 * Other TCP Path MTU Discovery Timeout values 
 */
#define MIN_PATHMTU_AGER        10	/* Min minutes for PMTU age-timer */
#define MAX_PATHMTU_AGER        30	/* Max minutes for PMTU age-timer */
#define NO_PATHMTU_AGER         0	/* Disable PMTU age-timer */

/*
 * Default receive window size.
 */
#define TCP_DEFRCVWND		(8 * MAXPACKETDATALENGTH)

/*
 * We keep a couple of otherwise unused words in the TCB to be used in
 * filling in the headers of packets we send.  These are the values.
 */
#define TCPDEFHDR1 0x5010	/* data offset, ACK flag */
#define TCPDEFHDR2 0x00000000	/* checksum and urgent ptr have 0 */


/*
 * Common tuning parameters
 */

/*
 * Initial round trip parameters. 
 * Note that SRTT is scaled by a factor of eight. 
 * SRTO is in normal units.
 */
#define TCP_INIRTT	(8*ONESEC)	/* scaled (multiplied) by 8 */
#define TCP_INIRTO	(2*ONESEC)	/* 2 times actual SRTT */ 
#define TCP_SLOWRTT	(24*ONESEC)	/* use three seconds */
#define TCP_SLOWRTO	(6*ONESEC)	/* 2 times actual SRTT */ 

/*
 * Timer constants
 */
#define TCP_TIMER_MINTICKS	100	/* shortest */
#define TCP_TIMER_MAXTICKS	5000	/* longest */

/*
 * We really should stay in TIMEWAIT state for 2*MSL (4 minutes)
 * but that's a long time to wait with memory allocated, especially
 * on a puny terminal server.  We cut this by four to 1 minute.
 */
#define LENTMEWAITTIMEOUT (MAXSEGLIFETIME/2) /* 1/2 * maximum segment lifetime */
#define LENSNDWNDTIMEOUT ONEMIN	   /* probe closed window every minutes */
#define LENFINWAIT2TIMEOUT	(10 * ONEMIN)	/* time waiting for FIN */

#define LENPMTUSHORTTIMEOUT	(2 * ONEMIN)	/* time after a successful 
						   PMTU increase */
/*
 * Default max number of TCP small servers processes.
 */
#define TCP_DEFAULT_MAX_SMALL_SERVERS	10 	/* Default process limiter */

/*
 * Macro to truncate internal 32-bit window to 16-bit size
 * used in TCP header.  This is insurance against advertising
 * a bizarre window in case the internally-maintained window
 * grows above 64k-1 (which should never happen).
 */
#define TCP_TRUNC_WINDOW(x)	(((x) > MAXTCPWINDOW) ? MAXTCPWINDOW : (x))

/* 
 * curpktttype
 * Information associated with incoming segments.
 */

#define curpkttype struct curpkttype_

curpkttype {
    addrtype foreignhost;	/* foreign host which sent the segment. */
    ushort foreignport;		/* foreign port which sent the segment. */
    paktype *pakptr;		/* ptr to segment buffer (to pak structure) */
    tcptype *tcpptr;		/* ptr to tcp header within pak structure */
    ushort seglen;		/* segment length */
    ushort segprc;		/* segment precedence */
    int segdataoffset;		/* segment data offset in bytes */
};

/*
 * We've put some code in to make TCP RST based attacks more difficult.
 * This isn't a silver bullet, but does frustrate most.
 */
#define	TCP_RST_SECURITY_COUNT	10		/* 10 out of seq RSTs */
#define	TCP_RST_SECURITY_WINDOW	(5*ONESEC)	/* within 5 seconds */


/*
 * Option code byte values for TCP options.
 */

#define ENDOPTIONLIST		0
#define NOPOPTION		1
#define MAXSEGSIZEOPTION	2
#define		MSSOPT_BYTES		4
#define WINSCALEOPTION		3		/* RFC 1323, not implemented */
#define		WSOPT_BYTES		3
#define TIMESTAMPSOPTION	8		/* RFC 1323, not implemented */
#define		TSOPT_BYTES		10
#define MD5OPTION		19
#define		MD5OPT_BYTES		18

/*
 * Useful constants
 */
#define TCPHEADERBYTES		20		/* header bytes w/o options */
#define TCP_DEFAULT_MSS		MAXPACKETDATALENGTH	/* default MSS */
#define MINTCPIPHEADERBYTES	(MINIPHEADERBYTES + TCPHEADERBYTES)
#define MAXTCPOPTIONBYTES	40
#define MINTCPOPTIONLENGTH	2


/*
 * More readable names for unspecified fields in paktype
 */

/* Field usage for packets in queue for out-of-sequence segments. */
#define us_seglen		unspecified[0]
#define us_segdataoffset	unspecified[1]

/* Field usage for packets in retransmission queue. */
#define us_retransno		unspecified[0]
#define us_sendtime		gp_timer


/***********************************************************************
 *
 *                      Externs and Prototypes
 *
 ***********************************************************************/
extern int tcp_chunksize, tcp_queuemax, tcp_synwait, tcp_finwaittm;
extern int tcp_pathmtu_ager;
extern long len_pathmtu_timeout;
extern boolean tcp_pathmtu;
extern ushort tcp_portseed;
extern tcbtype *tcbhashtable[], *tcbwildtable;
extern const ushort tcp_set_events[];
extern const ushort tcp_clear_events[];
extern boolean tcp_tracing, tcp_ready;
extern mgd_timer tcptest_delay_timer;
extern list_header tcp_newconn_list;
extern pid_t tcp_listener_pid;
extern boolean tcptunnel_enable;
extern int tcp_listenQ_max, tcp_listenQ_count;
extern ulong tcpservers_max, tcpservers_current;

/*
 * tcpinput.c
 */
extern void tcp_check_window(tcbtype *);
extern void tcp_discardsegment(paktype *, char *);
extern void tcp_handleother(tcbtype *, curpkttype *, int);
extern void tcp_processsavedsegments(tcbtype *);
extern void tcp_removeackedsendsegments(tcbtype *);
extern void tcp_mss(tcbtype *, int);

/*
 * tcpoutput.c
 */
extern void tcp_sendip(tcbtype *, paktype *, ipaddrtype);
extern boolean tcp_retransmit(tcbtype *, boolean);
extern void tcp_fast_retransmit (tcbtype *, tcptype *);
extern void tcp_inithdr(tcbtype *, tcptype *);
extern void tcp_replytoconnectionlesspacket(paktype *);
extern int tcp_sendsynsegment(tcbtype *, ulong, boolean, ulong);
extern void tcp_sendcontrolsegment(tcbtype *, ushort, ushort, addrtype *,
				   ulong, int, ulong, int, int);
extern void tcp_sendacksegment(tcbtype *);
extern void tcp_sendfin(tcbtype *);

/*
 * tcpservice.c
 */
extern process tcpdiscard_daemon(void);
extern process tcpecho_daemon(void);
extern process tcpchargen_daemon(void);
extern process tcpdaytime_daemon(void);

/*
 * tcpsupport.c
 */
extern void tcp_closeconnection(tcbtype *, curpkttype *, int, char *);
extern void tcp_pmtu_on(tcbtype *);
extern void tcp_pmtu_off(tcbtype *);
extern boolean tcp_seqinrange(ulong, ulong, ulong);
extern tcbtype *tcp_allocatetcb(void);
extern void tcp_deallocatetcb(tcbtype *, int);
extern void tcp_flushqueue(tcbtype *, int);
extern void tcp_changestate(tcbtype *, int);
extern void tcp_starttimer(tcbtype *, int, ulong);
extern void tcp_stoptimer(tcbtype *, int);
extern boolean tcp_timerrunning(tcbtype *, int);
extern void tcp_status_brief(tcbtype *);
extern tcbtype *find_tcb(ushort, ushort, addrtype *, int, boolean);
extern void tcb_enqueue(tcbtype *);
extern void tcb_unqueue(tcbtype *);
extern void print_tcp(tcbtype *, paktype *, tcptype *, char, ushort);
extern boolean tcp_filter_debug (ushort, ushort, addrtype *, addrtype *, char);
extern void tcp_md5on(tcbtype *, char *, int);
extern void tcp_md5digest(uchar *, ipaddrtype, ipaddrtype,
			  ushort, ushort, tcptype *, uchar *, uchar *, int);
extern boolean tcp_md5_connectionless_write(addrtype *, addrtype *, tcptype *,
					    tcptype *, int, ipoptiontype *);
extern uchar *tcp_findoption(tcptype *, uchar);
extern int tcp_optionbytes(tcbtype *, boolean);
extern void tcp_startoptions(tcbtype *, uchar *);
extern void tcp_writeoption(tcbtype *, uchar, uchar *, int);
extern void tcp_endoptions(tcbtype *);
extern void tcp_changercvwnd(tcbtype *, int);
extern void tcp_openwindow(tcbtype *);
extern void tcp_addwindow(tcbtype *, int);
extern ulong tcp_selectiss(void);
extern void tcp_retrans_sanity_check(tcbtype *, paktype *);
extern void tcp_wakeup_process(tcbtype *);
extern void tcp_pmtutimer_on(void);
extern void tcp_pmtutimer_off(void);
extern void init_tcptrace(void);
extern void show_tcptrace(void);
extern tcptype *tcp_get_hdr(paktype *, tcbtype *);
extern boolean clear_tcp_by_tcb(tcbtype *);
extern boolean clear_tcp_by_port_addr(addrtype *, ushort, addrtype *, ushort);
extern boolean clear_tcp_by_line(int);
extern void make_tcptrace(tcbtype *, tcptype *, ulong, iphdrtype *,
			  ulong, ulong, ulong, ulong);

/*
 * tcptop.c
 */
extern void tcp_close_children(tcbtype *);
extern void tcp_close_statechange(tcbtype *);
extern void tcp_async_cleanup(tcbtype *, int);
extern boolean tcp_putsetup(tcbtype *);
extern ulong tcp_usable_window(tcbtype *);
extern boolean tcp_putblock(tcbtype *);
extern boolean tcp_set_new_tty(conntype *, tt_soc *);
extern tcbtype * tcp_create_wild_listen (tt_soc *, ushort);
extern void tcp_clear_conn(tcbtype *);

/*
 * tcpvty.c
 */
extern process tcp_protocols(void);
extern void tcp_listen_discard_port(tcbtype *);
extern void tcp_listen_finger_port(tcbtype *);
extern void tcp_listen_echo_port(tcbtype *);
extern void tcp_listen_chargen_port(tcbtype *);
extern void tcp_listen_daytime_port(tcbtype *);
extern void tcp_listen_default(tcbtype *);
extern void tcp_idle_estab(tcbtype *);
extern boolean tcp_access_check(tcbtype *, ulong, ulong, ipaddrtype);
extern boolean tcp_access_check_finger(ulong, ulong, ipaddrtype);
extern boolean tcp_access_check_small(ulong, ulong, ipaddrtype);
extern boolean tcp_access_check_default(ulong, ulong, ipaddrtype);
extern translatetype *tcp_find_translation(ipaddrtype, ushort);

/*
 * tcp_chain.c
 */
extern void tcp_parser_init(void);

/*
 * More inlines that had to come after the extern declarations.
 */

/*
 * tcp_increment_server_count
 *
 * Bump the count of active small TCP servers
 */
static inline void tcp_increment_server_count (void)
{
    tcpservers_current++;
}

/*
 * tcp_decrement_server_count
 *
 * Drop the count of active small TCP servers
 */
static inline void tcp_decrement_server_count (void)
{
    if (tcpservers_current)
        tcpservers_current--;
}

#endif /* __TCPINTERNAL_H__ */
