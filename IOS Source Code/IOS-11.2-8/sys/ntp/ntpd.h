/* $Id: ntpd.h,v 3.1.2.3 1996/08/20 19:14:08 dkatz Exp $
 * $Source: /release/112/cvs/Xsys/ntp/ntpd.h,v $
 *------------------------------------------------------------------
 * Private external declarations for NTP
 *
 * December 1995, Dave Katz
 *
 * Copyright (c) 1996-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ntpd.h,v $
 * Revision 3.1.2.3  1996/08/20  19:14:08  dkatz
 * CSCdi65483:  NTP needs modularity cleanup
 * Branch: California_branch
 * Squeeze out another 2.5KB.
 *
 * Revision 3.1.2.2  1996/08/19  18:55:28  dkatz
 * CSCdi65483:  NTP needs modularity cleanup
 * Branch: California_branch
 * Break up NTP into separate subsystems.  Significantly reduce image size
 * and stack usage.  Clean up refclock interface.  Make all modules
 * compile cleanly with all error checking enabled (DUSTY is clean!)
 *
 * Revision 3.1.2.1  1996/04/16  19:03:33  rchandra
 * CSCdi54830:  IP features commit
 * Branch: California_branch
 *
 * Revision 3.1  1996/04/15  09:24:30  dkatz
 * Add placeholders.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Copyright (c) David L. Mills 1992, 1993, 1994
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided
 * that the above copyright notice appears in all copies and that both the
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name University of Delaware not be used in
 * advertising or publicity pertaining to distribution of the software
 * without specific, written prior permission.  The University of Delaware
 * makes no representations about the suitability this software for any
 * purpose.  It is provided "as is" without express or implied warranty.
 *
 * Authors
 *
 * Dennis Ferguson <dennis@mrbill.canet.ca> (foundation code for NTP
 *      Version 2 as specified in RFC-1119)
 * Lars H. Mathiesen <thorinn@diku.dk> (adaptation of foundation code for
 *      Version 3 as specified in RFC-1305)
 * Louis A. Mamakos <louie@ni.umd.edu> (support for md5-based 
 *      authentication)
 * Craig Leres <leres@ee.lbl.gov> (port to 4.4BSD operating system,
 *	ppsclock, Maganavox GPS clock driver)
 * Nick Sayer <mrapple@quack.kfu.com> (SunOS streams modules)
 * Frank Kardel <Frank.Kardel@informatik.uni-erlangen.de>
 *      (PARSE (GENERIC) driver, STREAMS module for PARSE, support scripts,
 *       reference clock configuration scripts, Makefile cleanup)
 * Rainer Pruy <Rainer.Pruy@informatik.uni-erlangen.de> (monitoring/trap
 *      scripts, statistics file handling)
 * Glenn Hollinger <glenn@herald.usask.ca> (GOES clock driver)
 * Kenneth Stone <ken@sdd.hp.com> (port to HPUX operating system)
 * Dave Katz <dkatz@cisco.com> (port to RS/6000 AIX operating system)
 * William L. Jones <jones@hermes.chpc.utexas.edu> (RS/6000 AIX
 *	modifications, HPUX modifications)
 * John A. Dundas III <dundas@salt.jpl.nasa.gov> (Apple A/UX port)
 * David L. Mills <mills@udel.edu> (Spectractom WWVB, Austron GPS,
 *	Heath, ATOM, ACTS, KSI/Odetics IRIG-B clock drivers; pps support)
 * Jeffrey Mogul <mogul@pa.dec.com> (ntptrace utility)
 * Steve Clift (clift@ml.csiro.au) OMEGA clock driver)
 * Mike Iglesias (iglesias@uci.edu) (DEC Alpha changes)
 * Mark Andrews <marka@syd.dms.csiro.au> (Leitch atomic clock controller)
 * George Lindholm <lindholm@ucs.ubc.ca> (port to SunOS 5.1 operating system)
 * Jeff Johnson <jbj@chatham.usdesign.com> (massive prototyping overhaul)
 * Tom Moore <tmoore@fievel.daytonoh.ncr.com> (port to i386 svr4)
 * Piete Brooks <Piete.Brooks@cl.cam.ac.uk> (MSF clock driver, Trimble PARSE
 * 	support)
 * Karl Berry <karl@owl.HQ.ileaf.com> (syslog to file option)
 * Torsten Duwe <duwe@immd4.informatik.uni-erlangen.de> (Linux Port)
 * Paul A Vixie <vixie@vix.com> (TrueTime GPS driver)
 * Jim Jagielski <jim@jagubox.gsfc.nasa.gov> (A/UX port)
 * Ray Schnitzler <schnitz@unipress.com> (First pass at a Unixware1 port.)
 * Ajit Thyagarajan <ajit@ee.udel.edu> (IP multicast support)
 * Jeff Steinman <jss@pebbles.jpl.nasa.gov> (Datum PTS clock driver)
 * Tomoaki TSURUOKA <tsuruoka@nc.fukuoka-u.ac.jp> (TRAK clock driver)
 */

/*
 * ntpd.h - Prototypes for xntpd.
 */

#ifdef CISCO
#include "../os/clock.h"
#else
#include "ntp_syslog.h"
#endif /* CISCO */
#include "ntp_fp.h"
#include "ntp.h"
#ifndef CISCO
#include "ntp_malloc.h"
#endif /* CISCO */

#ifdef SYS_WINNT
#define exit service_exit
extern 	void 	service_exit	P((int));
/*  declare the service threads */
void  	service_main 	P((DWORD, LPTSTR *));
void  	service_ctrl	P((DWORD));
void    worker_thread	P((void *));
#endif /* SYS_WINNT */

/* ntp_config.c */
extern	void	getstartup	P((int, char **));
extern	void	getconfig	P((int, char **));

/* ntp_config.c */
extern	void	ctl_clr_stats	P((void));
extern	int	ctlclrtrap	P((struct sockaddr_in *, struct interface *, int));
extern	u_short	ctlpeerstatus	P((struct peer *));
extern	int	ctlsettrap	P((struct sockaddr_in *, struct interface *, int, int));
extern	u_short	ctlsysstatus	P((void));
extern	boolean	init_control	P((void));
extern	void	process_control	P((struct recvbuf *, int));
extern	void	report_event	P((int, struct peer *));

/* ntp_control.c */
/*
 * Structure for translation tables between internal system
 * variable indices and text format.
 */
struct ctl_var {
	u_char code;
	u_char flags;
	char *text;
};
/*
 * Flag values
 */
#define	CAN_READ	0x01
#define	CAN_WRITE	0x02

#define DEF		0x20
#define	PADDING		0x40
#define	EOV		0x80

#define	RO	(CAN_READ)
#define	WO	(CAN_WRITE)
#define	RW	(CAN_READ|CAN_WRITE)

extern  char *  add_var P((struct ctl_var **, unsigned long, int));
extern  void    free_varlist P((struct ctl_var *));
extern  void    set_var P((struct ctl_var **, char *, unsigned long, int));
extern  void    set_sys_var P((char *, unsigned long, int));

/* ntp_intres.c */
extern	void	ntp_intres	P((void));

/* ntp_io.c */
extern	struct interface *findbcastinter P((struct sockaddr_in *));
extern	struct interface *findinterface P((struct sockaddr_in *));
extern	void	freerecvbuf	P((struct recvbuf *));
extern	struct recvbuf *getrecvbufs	P((void));
extern	boolean	ntp_init_io	P((void));
extern	void	io_clr_stats	P((void));
extern	void	io_setbclient	P((void));
extern	void	io_unsetbclient	P((void));
extern	void	io_multicast_add P((u_long));
extern	void	io_multicast_del P((u_long));
extern	void	kill_asyncio	 P((void));
extern struct interface *loopback_interface;

extern	void	sendpkt		P((struct sockaddr_in *, struct interface *, ipaddrtype, int, struct pkt *, int));
#ifdef HAVE_SIGNALED_IO
extern  void	wait_for_signal P((void));
extern  void    unblock_io_and_alarm P((void));
extern	void	block_io_and_alarm P((void));
#endif

/* ntp_leap.c */
extern	void	init_leap	P((void));
extern	void	leap_process	P((void));
extern	int	leap_setleap	P((int, int));
/*
 * there seems to be a bug in the IRIX 4 compiler which prevents
 * u_char from beeing used in prototyped functions.
 * This is also true AIX compiler.
 * So give up and define it to be int. WLJ
 */
extern	int	leap_actual	P((int));

/* ntp_loopfilter.c */
extern	void	init_loopfilter	P((void));
extern	int	local_clock	P((l_fp *, struct peer *, int));
extern	void	adj_frequency	P((s_fp));
extern	void	adj_host_clock	P((void));
extern	void	loop_config	P((int, l_fp *, int));

/* refclock_atom.c */
#if defined(REFCLOCK) && defined(ATOM)
extern	int	pps_sample	P((l_fp *));
#endif /* ATOM */

/* ntp_monitor.c */
extern	void	init_mon	P((void));
extern	void	mon_start	P((int));
extern	void	mon_stop	P((int));
extern	void	monitor		P((struct recvbuf *));

/* ntp_peer.c */
extern	boolean	ntp_init_peer	P((void));
extern	struct peer *findexistingpeer P((struct sockaddr_in *, struct peer *));
#ifdef CISCO
extern	struct peer *findpeer	P((struct sockaddr_in *, struct interface *, boolean));
#else
extern	struct peer *findpeer	P((struct sockaddr_in *, struct interface *, int));
#endif /* CISCO */
extern	struct peer *findpeerbyassoc P((int));
extern	struct peer *newpeer	P((struct sockaddr_in *, struct interface *, int, int, int, int, int, u_long, int));
extern	void	peer_all_reset	P((void));
extern	void	peer_clr_stats	P((void));
extern	struct peer *peer_config P((struct sockaddr_in *, struct interface *, int, int, int, int, int, int, u_long, int));
extern	void	peer_reset	P((struct peer *));
#ifdef CISCO
extern	int	peer_unconfig	P((struct sockaddr_in *, struct interface *, boolean));
#else
extern	int	peer_unconfig	P((struct sockaddr_in *, struct interface *));
#endif /* CISCO */
extern	void	unpeer		P((struct peer *));

/* ntp_proto.c */
extern	void	transmit	P((struct peer *));
extern	void	receive		P((struct recvbuf *));
extern	void	peer_clear	P((struct peer *));
extern	int	process_packet	P((struct peer *, struct pkt *, l_fp *, int, int));
extern	void	clock_update	P((struct peer *));

/*
 * there seems to be a bug in the IRIX 4 compiler which prevents
 * u_char from beeing used in prototyped functions.
 * This is also true AIX compiler.
 * So give up and define it to be int. WLJ
 */
extern	void	poll_update	P((struct peer *, unsigned int, int));

extern	void	clear		P((struct peer *));
extern	void	clock_filter	P((struct peer *, l_fp *, s_fp, u_fp));
extern	void	clock_select	P((void));
extern	void	clock_combine	P((struct peer **, int));
extern	void	fast_xmit	P((struct recvbuf *, int, int));
extern	void	init_proto	P((void));
extern	void	proto_config	P((int, u_long));
extern	void	proto_clr_stats	P((void));

#ifdef	REFCLOCK
/* ntp_refclock.c */
extern	int	refclock_newpeer P((struct peer *));
extern	void	refclock_unpeer	P((struct peer *));
extern	void	refclock_receive P((struct peer *, l_fp *, s_fp, u_fp, l_fp *, l_fp *, int));
extern	void	refclock_leap	P((void));
#endif	/* REFCLOCK */

/* ntp_request.c */
extern	boolean	init_request	P((void));
extern	void	process_private	P((struct recvbuf *, int));

#ifndef CISCO
/* ntp_restrict.c */
extern	void	init_restrict	P((void));
extern	int	restrictions	P((struct sockaddr_in *));
extern	void	restrict	P((int, struct sockaddr_in *, struct sockaddr_in *, int, int));
#endif

/* ntp_timer.c */
extern	void	init_timer	P((void));
extern	void	timer		P((void));
extern	void	timer_clr_stats	P((void));
extern struct event timerqueue[];
extern u_long current_time;

/* ntp_unixclock.c */
extern	void	init_systime	P((void));

/* ntp_util.c */
extern	void	init_util	P((void));
extern	void	hourly_stats	P((void));
extern	void	stats_config	P((int, char *));
extern	void	record_peer_stats P((struct sockaddr_in *, int, l_fp *, s_fp, u_fp));
extern	void	record_loop_stats P((l_fp *, s_fp, int));
extern	void	record_clock_stats P((struct sockaddr_in *, char *));
extern	void	record_raw_stats P((struct sockaddr_in *, struct sockaddr_in *, l_fp *, l_fp *, l_fp *, l_fp *));
extern	void	getauthkeys	P((char *));
extern	void	rereadkeys	P((void));

#ifdef CISCO
extern boolean ntp_clockparms_debug;
extern boolean ntp_clockadjust_debug;
extern boolean ntp_packets_debug;
extern boolean ntp_loopfilter_debug;
extern boolean ntp_validity_debug;
extern boolean ntp_event_debug;
extern boolean ntp_sync_debug;
extern boolean ntp_select_debug;
extern boolean ntp_refclock_debug;

extern sys_timestamp ntp_sleep_timer;
extern ipsocktype *ntp_sock;
extern boolean ntp_running;
extern boolean ntp_task_initialized;
extern int sys_bclient;
extern s_fp sys_bdelay;
extern struct peer **assoc_hash;
extern struct peer **peer_hash;
extern int *peer_hash_count;
extern int peer_free_count;
extern struct peer *sys_peer;
extern u_char sys_leap;
extern u_char sys_stratum;
extern u_long sys_refid;
extern l_fp sys_reftime;
extern l_fp sys_offset;
extern s_fp sys_rootdelay;
extern u_fp sys_rootdispersion;
extern u_fp sys_maxd;
extern int sys_authenticate;
extern u_long sys_authdelay;
extern boolean ntp_update_calendar;
extern struct interface *any_interface;
extern boolean ntp_global_source_if;
extern ulong ntp_max_peercount;
extern s_char sys_precision;
extern long sys_clock;
extern struct interface inter_list[];
extern boolean ntp_clock_is_synced;
extern watched_queue *ntp_refclock_queue;
  
extern boolean ntp_clock_is_synchronized(void);
extern void calendar_update_process(void);
extern void ntp_clock_update(boolean);
extern void ntp_reset_clock(void);
extern void ntp_unsync(void);
extern boolean ntp_newpeer_ok(void);
extern void ntp_address_change(idbtype *, ipaddrtype, ipaddrtype, boolean,
			       boolean);
extern void ntp_int_statechange(idbtype *);
extern void ntp_flush_srcaddrs(void);
extern void ntp_nv_trust(parseinfo *);
extern boolean ntp_delete_key(ulong);
extern void ntp_nv_authkey(parseinfo *);
extern void ntp_set_drift(u_int, u_int);
extern void ntp_chain_init(void);
extern void ntp_refclock_chain_init(void);
extern void ntp_peer_nvgen(parseinfo *csb);
extern void init_ntp_process(idbtype *);
extern void authtrust(ulong, boolean);
extern void MD5auth_setkey(ulong, char *, boolean, int);
extern void init_auth(void);
extern char *ntoa(struct sockaddr_in *);
extern void clear_all(void);
extern void ntp_proto_config(int, u_long);
extern void ntp_adjust_clock_frequency(void);
extern boolean authhavekey(u_long keyid);
extern void ntp_init(subsystype *);
extern void ntp_input_handler(paktype *pak, udptype *udp, struct pkt *ntp_pkt);
extern void ntp_timer(void);
extern ulong ntp_restrictions(struct sockaddr_in *);
extern forktype ntp_async_process(tt_soc *);
extern void ntp_refclock_input(struct recvbuf *);
extern void ntp_refclock_input_handler(struct recvbuf *);
extern void ntp_inform_leap(void);
extern boolean ntp_pps_support_test(parseinfo *, ulong);

#define	IN_CLASSD(i)		(((long)(i) & 0xf0000000) == 0xe0000000)

#endif /* CISCO */
