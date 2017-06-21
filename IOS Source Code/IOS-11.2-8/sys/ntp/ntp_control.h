/* $Id: ntp_control.h,v 3.2.62.1 1996/04/16 19:03:10 rchandra Exp $
 * $Source: /release/112/cvs/Xsys/ntp/ntp_control.h,v $
 *------------------------------------------------------------------
 * ntp_control.h:  NTP control messages
 *
 * Dave Katz, November 1992
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: ntp_control.h,v $
 * Revision 3.2.62.1  1996/04/16  19:03:10  rchandra
 * CSCdi54830:  IP features commit
 * Branch: California_branch
 *
 * Revision 3.2  1995/11/17  17:50:58  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:41:01  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:44:59  hampton
 * Bump version numbers from 1.x to 2.x.
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
 * ntp_control.h - definitions related to NTP mode 6 control messages
 */

#include "ntp_types.h"

struct ntp_control {
	u_char li_vn_mode;		/* leap, version, mode */
	u_char r_m_e_op;		/* response, more, error, opcode */
	u_short sequence;		/* sequence number of request */
	u_short status;			/* status word for association */
	u_short associd;		/* association ID */
	u_short offset;			/* offset of this batch of data */
	u_short count;			/* count of data in this packet */
	u_char data[(480 + MAX_MAC_LEN)]; /* data + auth */
};

/*
 * Length of the control header, in octets
 */
#define	CTL_HEADER_LEN		12
#define	CTL_MAX_DATA_LEN	468


/*
 * Limits and things
 */
#define	CTL_MAXTRAPS	3		/* maximum number of traps we allow */
#define	CTL_TRAPTIME	(60*60)		/* time out traps in 1 hour */
#define	CTL_MAXAUTHSIZE	64		/* maximum size of an authen'ed req */

/*
 * Decoding for the r_m_e_op field
 */
#define	CTL_RESPONSE	0x80
#define	CTL_ERROR	0x40
#define	CTL_MORE	0x20
#define	CTL_OP_MASK	0x1f

#define	CTL_ISRESPONSE(r_m_e_op)	(((r_m_e_op) & 0x80) != 0)
#define	CTL_ISMORE(r_m_e_op)	(((r_m_e_op) & 0x20) != 0)
#define	CTL_ISERROR(r_m_e_op)	(((r_m_e_op) & 0x40) != 0)
#define	CTL_OP(r_m_e_op)	((r_m_e_op) & CTL_OP_MASK)

/*
 * Opcodes
 */
#define	CTL_OP_UNSPEC		0
#define	CTL_OP_READSTAT		1
#define	CTL_OP_READVAR		2
#define	CTL_OP_WRITEVAR		3
#define	CTL_OP_READCLOCK	4
#define	CTL_OP_WRITECLOCK	5
#define	CTL_OP_SETTRAP		6
#define	CTL_OP_ASYNCMSG		7
#define	CTL_OP_UNSETTRAP	31

/*
 * {En,De}coding of the system status word
 */
#define	CTL_SST_TS_UNSPEC	0	/* time source unspecified */
#define	CTL_SST_TS_ATOM		1	/* time source calibrated atomic */
#define	CTL_SST_TS_LF		2	/* time source VLF or LF radio */
#define	CTL_SST_TS_HF		3	/* time source HF radio */
#define	CTL_SST_TS_UHF		4	/* time source UHF radio */
#define	CTL_SST_TS_LOCAL	5	/* time source LOCAL */
#define	CTL_SST_TS_NTP		6	/* time source NTP */
#define	CTL_SST_TS_UDPTIME	7	/* time source UDP/TIME */
#define	CTL_SST_TS_WRSTWTCH	8	/* time source is wristwatch */
#define	CTL_SST_TS_TELEPHONE	9	/* time source is telephone modem */
#define CTL_SST_TS_PPS		0x20	/* time source is PPS signal */

#define	CTL_SYS_MAXEVENTS	15

#define	CTL_SYS_STATUS(li, source, nevnt, evnt) \
		(((((unsigned short)(li))<< 14)&0xc000) | \
		(((source)<<8)&0x3f00) | \
		(((nevnt)<<4)&0x00f0) | \
		((evnt)&0x000f))

#define	CTL_SYS_LI(status)	(((status)>>14) & 0x3)
#define	CTL_SYS_SOURCE(status)	(((status)>>8) & 0x3f)
#define	CTL_SYS_NEVNT(status)	(((status)>>4) & 0xf)
#define	CTL_SYS_EVENT(status)	((status) & 0xf)

/*
 * {En,De}coding of the peer status word
 */
#define	CTL_PST_CONFIG		0x80
#define	CTL_PST_AUTHENABLE	0x40
#define	CTL_PST_AUTHENTIC	0x20
#define	CTL_PST_REACH		0x10
#define	CTL_PST_UNSPEC		0x08

#define	CTL_PST_SEL_REJECT	0	/* rejected */
#define	CTL_PST_SEL_SANE	1	/* passed sanity checks */
#define	CTL_PST_SEL_CORRECT	2	/* passed correctness checks */
#define	CTL_PST_SEL_SELCAND	3	/* passed candidate checks */
#define	CTL_PST_SEL_SYNCCAND	4	/* passed outlyer checks */
#define	CTL_PST_SEL_DISTSYSPEER	5	/* selected, distance exceeded */
#define	CTL_PST_SEL_SYSPEER	6	/* selected */
#define	CTL_PST_SEL_PPS		7	/* selected, pps signal override */

#define	CTL_PEER_MAXEVENTS	15

#define	CTL_PEER_STATUS(status, nevnt, evnt) \
		((((status)<<8) & 0xff00) | \
		(((nevnt)<<4) & 0x00f0) | \
		((evnt) & 0x000f))

#define	CTL_PEER_STATVAL(status)(((status)>>8) & 0xff)
#define	CTL_PEER_NEVNT(status)	(((status)>>4) & 0xf)
#define	CTL_PEER_EVENT(status)	((status) & 0xf)

/*
 * {En,De}coding of the clock status word
 */
#define	CTL_CLK_OKAY		0
#define	CTL_CLK_NOREPLY		1
#define	CTL_CLK_BADFORMAT	2
#define	CTL_CLK_FAULT		3
#define	CTL_CLK_PROPAGATION	4
#define	CTL_CLK_BADDATE		5
#define	CTL_CLK_BADTIME		6

#define	CTL_CLK_STATUS(status, event) \
		((((status)<<8) & 0xff00) | \
		((event) & 0x00ff))

/*
 * Error code responses returned when the E bit is set.
 */
#define	CERR_UNSPEC	0
#define	CERR_PERMISSION	1
#define	CERR_BADFMT	2
#define	CERR_BADOP	3
#define	CERR_BADASSOC	4
#define	CERR_UNKNOWNVAR	5
#define	CERR_BADVALUE	6
#define	CERR_RESTRICT	7

#define	CERR_NORESOURCE	CERR_PERMISSION	/* wish there was a different code */


/*
 * System variables we understand
 */
#define	CS_LEAP		1
#define	CS_STRATUM	2
#define	CS_PRECISION	3
#define	CS_ROOTDELAY	4
#define	CS_ROOTDISPERSION	5
#define	CS_REFID	6
#define	CS_REFTIME	7
#define	CS_POLL		8
#define	CS_PEERID	9
#define	CS_OFFSET	10
#define	CS_DRIFT	11
#define	CS_COMPLIANCE	12
#define	CS_CLOCK	13
#define	CS_LEAPIND	14
#define	CS_LEAPWARNING	15
#define	CS_PROCESSOR	16
#define	CS_SYSTEM	17
#define	CS_KEYID	18
#define	CS_REFSKEW	19
#define CS_VARLIST	20

#define	CS_MAXCODE	CS_VARLIST

/*
 * Peer variables we understand
 */
#define	CP_CONFIG	1
#define	CP_AUTHENABLE	2
#define	CP_AUTHENTIC	3
#define	CP_SRCADR	4
#define	CP_SRCPORT	5
#define	CP_DSTADR	6
#define	CP_DSTPORT	7
#define	CP_LEAP		8
#define	CP_HMODE	9
#define	CP_STRATUM	10
#define	CP_PPOLL	11
#define	CP_HPOLL	12
#define	CP_PRECISION	13
#define	CP_ROOTDELAY	14
#define	CP_ROOTDISPERSION	15
#define	CP_REFID	16
#define	CP_REFTIME	17
#define	CP_ORG		18
#define	CP_REC		19
#define	CP_XMT		20
#define	CP_REACH	21
#define	CP_VALID	22
#define	CP_TIMER	23
#define	CP_DELAY	24
#define	CP_OFFSET	25
#define	CP_DISPERSION	26
#define	CP_KEYID	27
#define	CP_FILTDELAY	28
#define	CP_FILTOFFSET	29
#define	CP_PMODE	30
#define	CP_RECEIVED	31
#define	CP_SENT		32
#define	CP_FILTERROR	33
#define	CP_FLASH	34
#define CP_DISP		35
#define CP_VARLIST	36

#define	CP_MAXCODE	CP_VARLIST

/*
 * Clock variables we understand
 */
#define	CC_TYPE		1
#define	CC_TIMECODE	2
#define	CC_POLL		3
#define	CC_NOREPLY	4
#define	CC_BADFORMAT	5
#define	CC_BADDATA	6
#define	CC_FUDGETIME1	7
#define	CC_FUDGETIME2	8
#define	CC_FUDGEVAL1	9
#define	CC_FUDGEVAL2	10
#define	CC_FLAGS	11
#define	CC_DEVICE	12
#define CC_VARLIST	13

#define	CC_MAXCODE	CC_VARLIST

/*
 * Definition of the structure used internally to hold trap information.
 * ntp_request.c wants to see this.
 */
struct ctl_trap {
	struct sockaddr_in tr_addr;	/* address of trap recipient */
	struct interface *tr_localaddr;	/* interface to send this through */
	u_long tr_settime;		/* time trap was set */
	u_long tr_count;		/* async messages sent to this guy */
	u_long tr_origtime;		/* time trap was originally set */
	u_long tr_resets;		/* count of resets for this trap */
	u_short tr_sequence;		/* trap sequence id */
	u_char tr_flags;		/* trap flags */
	u_char tr_version;		/* version number of trapper */
};

/*
 * Flag bits
 */
#define	TRAP_INUSE	0x1		/* this trap is active */
#define	TRAP_NONPRIO	0x2		/* this trap is non-priority */
#define	TRAP_CONFIGURED	0x4		/* this trap was configured */

/*
 * Types of things we may deal with
 * shared between ntpq and library
 */
#define	TYPE_SYS	1
#define	TYPE_PEER	2
#define	TYPE_CLOCK	3
