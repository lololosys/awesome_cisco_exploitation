/* $Id: ntp_stdlib.h,v 3.1.2.1 1996/04/16 19:03:29 rchandra Exp $
 * $Source: /release/112/cvs/Xsys/ntp/ntp_stdlib.h,v $
 *------------------------------------------------------------------
 * Library extern definitions for NTP
 *
 * Dave Katz, December 1995
 *
 * Copyright (c) 1996-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ntp_stdlib.h,v $
 * Revision 3.1.2.1  1996/04/16  19:03:29  rchandra
 * CSCdi54830:  IP features commit
 * Branch: California_branch
 *
 * Revision 3.1  1996/04/15  09:24:29  dkatz
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
 * ntp_stdlib.h - Prototypes for XNTP lib.
 */
#ifdef CISCO
#include "ntp_types.h"
#else
#include <sys/types.h>

#include "ntp_types.h"
#include "ntp_string.h"
#include "l_stdlib.h"
#endif

#ifndef	P
#if defined(__STDC__) || defined(USE_PROTOTYPES)
#define P(x)	x
#else
#define	P(x)	()
#if	!defined(const)
#define	const
#endif
#endif
#endif

#if defined(__STDC__)
extern	void	msyslog		P((int, char *, ...));
#else
extern	void	msyslog		P(());
#endif

extern	void	auth_des	P((u_long *, u_char *));
extern	void	auth_delkeys	P((void));
extern	int	auth_havekey	P((u_long));
extern	int	auth_parity	P((u_long *));
extern	void	auth_setkey	P((u_long, u_long *));
extern	void	auth_subkeys	P((u_long *, u_char *, u_char *));
extern	int	authistrusted	P((u_long));
extern	int	authusekey	P((u_long, int, const char *));

extern	void	auth_delkeys	P((void));

extern	void	auth1crypt	P((u_long, u_int32 *, int));
extern	int	auth2crypt	P((u_long, u_int32 *, int));
extern	int	authdecrypt	P((u_long, u_int32 *, int));
extern	int	authencrypt	P((u_long, u_int32 *, int));
extern	int	authhavekey	P((u_long));
extern	int	authreadkeys	P((const char *));
extern	void	authtrust	P((u_long, int));
extern	void	calleapwhen	P((u_long, u_long *, u_long *));
extern	u_long	calyearstart	P((u_long));
extern	const char *clockname	P((int));
extern	int	clocktime	P((int, int, int, int, int, u_long, u_long *, u_int32 *));
#ifndef CISCO
extern	char *	emalloc		P((u_int));
#endif
extern	int	ntp_getopt	P((int, char **, char *));
extern	void	init_auth	P((void));
extern	void	init_lib	P((void));
extern	void	init_random	P((void));

#ifdef	DES
extern	void	DESauth1crypt	P((u_long, u_int32 *, int));
extern	int	DESauth2crypt	P((u_long, u_int32 *, int));
extern	int	DESauthdecrypt	P((u_long, const u_int32 *, int));
extern	int	DESauthencrypt	P((u_long, u_int32 *, int));
extern	void	DESauth_setkey	P((u_long, const u_int32 *));
extern	void	DESauth_subkeys	P((const u_int32 *, u_char *, u_char *));
extern	void	DESauth_des	P((u_int32 *, u_char *));
extern	int	DESauth_parity	P((u_int32 *));
#endif	/* DES */

#ifdef	MD5
extern	void	MD5auth1crypt	P((u_long, u_int32 *, int));
extern	int	MD5auth2crypt	P((u_long, u_int32 *, int));
extern	int	MD5authdecrypt	P((u_long, const u_int32 *, int));
extern	int	MD5authencrypt	P((u_long, u_int32 *, int));
extern	void	MD5auth_setkey	P((u_long, const u_int32 *));
#endif	/* MD5 */

extern	int	atoint		P((const char *, long *));
extern	int	atouint		P((const char *, u_long *));
extern	int	hextoint	P((const char *, u_long *));
extern	char *	humandate	P((u_long));
extern	char *	inttoa		P((long));
extern	char *	mfptoa		P((u_long, u_long, int));
extern	char *	mfptoms		P((u_long, u_long, int));
extern	char *	modetoa		P((int));
extern  const char * eventstr   P((int));
extern  const char * ceventstr  P((int));
extern	char *	statustoa	P((int, int));
extern  const char * sysstatstr P((int));
extern  const char * peerstatstr P((int));
extern	u_int32	netof		P((u_int32));
extern	char *	numtoa		P((u_int32));
extern	char *	numtohost	P((u_int32));
extern	int	octtoint	P((const char *, u_long *));
extern	u_long	ranp2		P((int));
extern	char *	refnumtoa	P((u_int32));
extern	int	tsftomsu	P((u_long, int));
extern	char *	uinttoa		P((u_long));

extern	int	decodenetnum	P((const char *, u_int32 *));

#ifndef CISCO
extern RETSIGTYPE signal_no_reset P((int, RETSIGTYPE (*func)()));
#endif /* CISCO */
