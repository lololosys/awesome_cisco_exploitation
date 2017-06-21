/* $Id: ntp_refclock.h,v 3.1.2.2 1996/08/19 18:55:13 dkatz Exp $
 * $Source: /release/112/cvs/Xsys/ntp/ntp_refclock.h,v $
 *------------------------------------------------------------------
 * Definitions for NTP reference clock support
 *
 * December 1995, Dave Katz
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ntp_refclock.h,v $
 * Revision 3.1.2.2  1996/08/19  18:55:13  dkatz
 * CSCdi65483:  NTP needs modularity cleanup
 * Branch: California_branch
 * Break up NTP into separate subsystems.  Significantly reduce image size
 * and stack usage.  Clean up refclock interface.  Make all modules
 * compile cleanly with all error checking enabled (DUSTY is clean!)
 *
 * Revision 3.1.2.1  1996/04/16  19:03:23  rchandra
 * CSCdi54830:  IP features commit
 * Branch: California_branch
 *
 * Revision 3.1  1996/04/15  09:24:28  dkatz
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
 * ntp_refclock.h - definitions for reference clock support
 */

#include "ntp_types.h"

#if defined(HAVE_BSD_TTYS)
#include <sgtty.h>
#endif /* HAVE_BSD_TTYS */

#if defined(HAVE_SYSV_TTYS)
#include <termio.h>
#endif /* HAVE_SYSV_TTYS */

#if defined(HAVE_TERMIOS)
#include <termios.h>
#endif

#if defined(STREAM)
#include <stropts.h>
#if defined(CLK)
#include <sys/clkdefs.h>
#endif /* CLK */
#endif /* STREAM */

#if !defined(SYSV_TTYS) && !defined(STREAM) & !defined(BSD_TTYS)
#define BSD_TTYS
#endif /* SYSV_TTYS STREAM BSD_TTYS */

/*
 * Macros to determine the clock type and unit numbers from a
 * 127.127.t.u address
 */
#define	REFCLOCKTYPE(srcadr)	((SRCADR(srcadr) >> 8) & 0xff)
#define REFCLOCKUNIT(srcadr)	(SRCADR(srcadr) & 0xff)

/*
 * List of reference clock names and descriptions. These must agree with
 * lib/clocktypes.c and xntpd/refclock_conf.c.
 */
struct clktype {
	int code;		/* driver "major" number */
	char *clocktype;	/* long description */
	char *abbrev;		/* short description */
};

/*
 * Configuration flag values
 */
#define	CLK_HAVETIME1	0x1
#define	CLK_HAVETIME2	0x2
#define	CLK_HAVEVAL1	0x4
#define	CLK_HAVEVAL2	0x8

#define	CLK_FLAG1	0x1
#define	CLK_FLAG2	0x2
#define	CLK_FLAG3	0x4
#define	CLK_FLAG4	0x8

#define	CLK_HAVEFLAG1	0x10
#define	CLK_HAVEFLAG2	0x20
#define	CLK_HAVEFLAG3	0x40
#define	CLK_HAVEFLAG4	0x80

/*
 * Constant for disabling event reporting in
 * refclock_receive. ORed in leap
 * parameter
 */
#define REFCLOCK_OWN_STATES	0x80

/*
 * Structure for returning clock status
 */
struct refclockstat {
	u_char	type;		/* clock type */
	u_char	flags;		/* clock flags */
	u_char	haveflags;	/* bit array of valid flags */
	u_short	lencode;	/* length of last timecode (may be longer than a char!) */
	char	*lastcode;	/* last timecode received */
	u_int32	polls;		/* transmit polls */
	u_int32	noresponse;	/* no response to poll */
	u_int32	badformat;	/* bad format timecode received */
	u_int32	baddata;	/* invalid data timecode received */
	u_int32	timereset;	/* driver resets */
	char	*clockdesc;	/* ASCII description */
	l_fp	fudgetime1;	/* configure fudge time1 */
	l_fp	fudgetime2;	/* configure fudge time2 */
	int32	fudgeval1;	/* configure fudge value1 */
	int32	fudgeval2;	/* configure fudge value2 */
	u_char	currentstatus;	/* clock status */
	u_char	lastevent;	/* last exception event */
	u_char	unused;		/* spare */
	struct	ctl_var *kv_list; /* additional variables */
};

/*
 * Reference clock I/O structure.  Used to provide an interface between
 * the reference clock drivers and the I/O module.
 */
struct refclockio {
	struct	recvbuf *(*clock_recv)(struct recvbuf *); /*process buffer */
	struct peer *srcclock;	/* pointer to peer */
};

/*
 * Structure for returning debugging info
 */
#define	NCLKBUGVALUES	16
#define	NCLKBUGTIMES	32

struct refclockbug {
	u_char	nvalues;	/* values following */
	u_char	ntimes;		/* times following */
	u_short	svalues;	/* values format sign array */
	u_int32	stimes;		/* times format sign array */
	u_int32	values[NCLKBUGVALUES]; /* real values */
	l_fp	times[NCLKBUGTIMES]; /* real times */
};

/*
 * Structure interface between the reference clock support
 * ntp_refclock.c and the driver utility routines
 */
#define MAXSTAGE	64	/* max stages in shift register */
#define BMAX		128	/* max timecode length */
#define GMT		0	/* I hope nobody sees this */
#define MAXDIAL		60	/* max length of modem dial strings */

/*
 * Line discipline flags. These require line discipline or streams
 * modules to be installed/loaded in the kernel. If specified, but not
 * installed, the code runs as if unspecified.
 */
#define LDISC_STD	0x0	/* standard */
#define LDISC_CLK	0x1	/* tty_clk \n intercept */
#define LDISC_CLKPPS	0x2	/* tty_clk \377 intercept */
#define LDISC_ACTS	0x4	/* tty_clk #* intercept */
#define LDISC_CHU	0x8	/* tty_chu */
#define LDISC_PPS	0x10	/* ppsclock */

struct ntp_refclock_ {
	struct  peer *refclock_peer;  /* Corresponding peer */
	struct	refclockio io;	/* I/O handler structure */
	void	*unitptr;	/* pointer to unit structure */
	u_long	lasttime;	/* last clock update time */
	u_char	leap;		/* leap/synchronization code */
	u_char	currentstatus;	/* clock status */
	u_char	lastevent;	/* last exception event */
	u_char	type;		/* clock type */
	char	*clockdesc;	/* clock description */
	char	lastcode[BMAX];	/* last timecode received */
	u_short	lencode;	/* length of last timecode (allow for more than 256 chars !) */

	int	year;		/* year of eternity */
	int     month;		/* month (1=Jan), NZ means day is in month */
	int	day;		/* day of year */
	int	hour;		/* hour of day */
	int	minute;		/* minute of hour */
	int	second;		/* second of minute */
	int	msec;		/* millisecond of second */
	long	usec;		/* microsecond of second (alt) */
	long    tz_offset;	/* timezone offset in seconds from UTC */
	long	latitude;	/* latitude in centiseconds, - = south */
	long	longitude;	/* longitude in centiseconds, - = west */
	int     altitude;	/* altitude in feet */
	u_int	nstages;	/* median filter stages */
	u_long	yearstart;	/* beginning of year */
	u_long	coderecv;	/* sample counter */
	l_fp	lastref;	/* last reference timestamp */
	l_fp	lastrec;	/* last local timestamp */
	l_fp	offset;		/* median offset */
	u_fp	dispersion;	/* sample dispersion */
	l_fp	filter[MAXSTAGE]; /* median filter */
	l_fp	pps_time;	/* Timestamp of latest PPS firing */
	int	pollcnt;	/* Keeps track of outstanding polls */

	/*
	 * Configuration data
	 */
	l_fp	fudgetime1;	/* fudge time1 */
	l_fp	fudgetime2;	/* fudge time2 */
	u_long	refid;		/* reference identifier */
	u_long	sloppyclockflag; /* fudge flags */
	u_long stratum_specified; /* specified stratum, or NTP_MAXSTRATUM */

	/*
	 * Status tallies
 	 */
	u_long	timestarted;	/* time we started this */
	u_long	polls;		/* polls sent */
	u_long	noreply;	/* no replies to polls */
	u_long	badformat;	/* bad format reply */
	u_long	baddata;	/* bad data reply */
};
#define METERS_TO_FEET_NUMERATOR (10000)
#define METERS_TO_FEET_DENOMINATOR (254 * 12)
#define NOPOLLS 2		/* Poll count initializer */
#define	NSAMPLES	3       /* stages of median filter */

/*
 * Structure interface between the reference clock support
 * ntp_refclock.c and particular clock drivers. This must agree with the
 * structure defined in the driver.
 */
#define	NOFLAGS	0		/* flag for null flags */

struct refclock {
	int (*clock_start)	P((int, struct peer *));
	void (*clock_shutdown)	P((int, struct peer *));
	void (*clock_poll)	P((int, struct peer *));
	void (*clock_control)	P((int, struct refclockstat *,
				    struct refclockstat *));
	void (*clock_init)	P((void));
	void (*clock_buginfo)	P((int, struct refclockbug *));
	u_long clock_flags;
	void (*clock_setup)	P((struct peer *));
};


/* Table of refclock vectors */

extern struct refclock **refclock_conf;


/*
 * Function prototypes
 */

#ifdef REFCLOCK
extern	void	refclock_buginfo P((struct sockaddr_in *,
				    struct refclockbug *));
extern	void	refclock_control P((struct sockaddr_in *,
				    struct refclockstat *,
				    struct refclockstat *));
extern	int	refclock_open	P((char *, int, int));
extern	void	refclock_transmit P((struct peer *));
extern	int	refclock_ioctl	P((int, int));
extern 	int	refclock_process P((struct refclockproc *, int, int, boolean));
extern 	int	refclock_sample P((l_fp *, struct refclockproc *, int, int));
extern	void	refclock_report	P((struct peer *, u_int));
extern	int	refclock_gtlin	P((struct recvbuf *, char *, int,
				    l_fp *));
extern	int	refclock_new_sample P((struct refclockproc *, int, int,
				       l_fp *));
extern	void	ntp_refclock_register P((struct refclock *, ulong));
extern  boolean ntp_refclock_present P((ulong));
extern  boolean ntp_any_refclocks_present P((void));

#endif /* REFCLOCK */
