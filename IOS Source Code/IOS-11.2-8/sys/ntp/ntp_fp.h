/* $Id: ntp_fp.h,v 3.2.62.1 1996/04/16 19:03:12 rchandra Exp $
 * $Source: /release/112/cvs/Xsys/ntp/ntp_fp.h,v $
 *------------------------------------------------------------------
 * NTP fixed point arithmetic definitions
 *
 * Dave Katz, November 1992
 *
 * Copyright (c) 1992-1997 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: ntp_fp.h,v $
 * Revision 3.2.62.1  1996/04/16  19:03:12  rchandra
 * CSCdi54830:  IP features commit
 * Branch: California_branch
 *
 * Revision 3.2  1995/11/17  17:51:04  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:41:06  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/06/16  04:28:42  dkatz
 * CSCdi35882:  Time-of-day code needs cleanup
 *
 * Revision 2.1  1995/06/07  21:45:15  hampton
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
 * ntp_fp.h - definitions for NTP fixed point arithmetic
 */


#ifndef NTP_FP_H
#define NTP_FP_H
#define CISCO

#ifndef CISCO
#include <sys/types.h>
#ifndef SYS_WINNT
#include <sys/socket.h>
#include <netinet/in.h>
#endif /* SYS_WINNT */
#endif /* CISCO */

#include "ntp_types.h"

#ifdef CISCO
/*
 * NTP uses two fixed point formats.  The first (l_fp) is the "long" format
 * and is 64 bits long with the decimal between bits 31 and 32.  This
 * type is defined in ../os/clock.h since the OS clock routines make
 * use of it.  
 */

#define l_fp clock_epoch

#else

/*
 * NTP uses two fixed point formats.  The first (l_fp) is the "long" format
 * and is 64 bits long with the decimal between bits 31 and 32.  This
 * is used for time stamps in the NTP packet header (in network byte
 * order) and for internal computations of offsets (in local host byte
 * order).  We use the same structure for both signed and unsigned values,
 * which is a big hack but saves rewriting all the operators twice.  Just
 * to confuse this, we also sometimes just carry the fractional part in
 * calculations, in both signed and unsigned forms.  Anyway, an l_fp looks
 * like:
 *
 *    0			  1		      2			  3
 *    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   |			       Integral Part			     |
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   |			       Fractional Part			     |
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *
 */
typedef struct {
	union {
		u_int32 Xl_ui;
		int32 Xl_i;
	} Ul_i;
	union {
		u_int32 Xl_uf;
		int32 Xl_f;
	} Ul_f;
} l_fp;

#endif /* CISCO */

#define l_ui	Ul_i.Xl_ui		/* unsigned integral part */
#define	l_i	Ul_i.Xl_i		/* signed integral part */
#define	l_uf	Ul_f.Xl_uf		/* unsigned fractional part */
#define	l_f	Ul_f.Xl_f		/* signed fractional part */

/*
 * Fractional precision (of an l_fp) is actually the number of
 * bits in a long.
 */
#define	FRACTION_PREC	(32)


/*
 * The second fixed point format is 32 bits, with the decimal between
 * bits 15 and 16.  There is a signed version (s_fp) and an unsigned
 * version (u_fp).  This is used to represent synchronizing distance
 * and synchronizing dispersion in the NTP packet header (again, in
 * network byte order) and internally to hold both distance and
 * dispersion values (in local byte order).  In network byte order
 * it looks like:
 *
 *    0			  1		      2			  3
 *    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   |		  Integer Part	     |	   Fraction Part	     |
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *
 */
typedef int32 s_fp;
typedef u_int32 u_fp;

/*
 * A unit second in fp format.  Actually 2**(half_the_bits_in_a_long)
 */
#define	FP_SECOND	(0x10000)

/*
 * Byte order conversions
 */
#define	HTONS_FP(x)	(htonl(x))
#define	HTONL_FP(h, n)	do { (n)->l_ui = htonl((h)->l_ui); \
			     (n)->l_uf = htonl((h)->l_uf); } while (0)
#define	NTOHS_FP(x)	(ntohl(x))
#define	NTOHL_FP(n, h)	do { (h)->l_ui = ntohl((n)->l_ui); \
			     (h)->l_uf = ntohl((n)->l_uf); } while (0)
#define	NTOHL_MFP(ni, nf, hi, hf) \
	do { (hi) = ntohl(ni); (hf) = ntohl(nf); } while (0)
#define	HTONL_MFP(hi, hf, ni, nf) \
	do { (ni) = ntohl(hi); (nf) = ntohl(hf); } while (0)

/* funny ones.  Converts ts fractions to net order ts */
#define	HTONL_UF(uf, nts) \
	do { (nts)->l_ui = 0; (nts)->l_uf = htonl(uf); } while (0)
#define	HTONL_F(f, nts) do { (nts)->l_uf = htonl(f); \
				if ((f) & 0x80000000) \
					(nts)->l_i = -1; \
				else \
					(nts)->l_i = 0; \
			} while (0)

/*
 * Conversions between the two fixed point types
 */
#define	MFPTOFP(x_i, x_f)	(((x_i)<<16) | (((x_f)>>16)&0xffff))
#define	LFPTOFP(v)		MFPTOFP((v)->l_i, (v)->l_f)

#define UFPTOLFP(x, v) ((v)->l_ui = (u_fp)(x)>>16, (v)->l_uf = (x)<<16)
#define FPTOLFP(x, v)  (UFPTOLFP((x), (v)), (x) < 0 ? (v)->l_ui -= 0x10000 : 0)

/*
 * Primitive operations on long fixed point values.  If these are
 * reminiscent of assembler op codes it's only because some may
 * be replaced by inline assembler for particular machines someday.
 * These are the (kind of inefficient) run-anywhere versions.
 */
#if defined(CISCO) && defined(mc68000)
#define	M_NEG(v_i, v_f) 	/* v = -v */ \
	      asm volatile ( "\
negl %3\n\
negxl %2"\
: "=dm" ((v_i)), "=dm" ((v_f)) \
: "0" ((v_i)), "1" ((v_f)))
#else
#define	M_NEG(v_i, v_f) 	/* v = -v */ \
	do { \
		if ((v_f) == 0) \
			(v_i) = -(v_i); \
		else { \
			(v_f) = -(v_f); \
			(v_i) = ~(v_i); \
		} \
	} while(0)
#endif /* CISCO */

#ifndef CISCO				/* Not used! */
#define	M_NEGM(r_i, r_f, a_i, a_f) 	/* r = -a */ \
	do { \
		if ((a_f) == 0) { \
			(r_f) = 0; \
			(r_i) = -(a_i); \
		} else { \
			(r_f) = -(a_f); \
			(r_i) = ~(a_i); \
		} \
	} while(0)
#endif /* CISCO */

#if defined(CISCO) && defined(mc68000)
#define M_ADD(r_i, r_f, a_i, a_f) /* r += a */ \
	      asm volatile ( "\
addl %5,%1\n\
bcc 0f\n\
addql #1,%0\n\
0:\n\
addl %4,%0" \
: "=dm" ((r_i)), "=dm" ((r_f)) \
: "0" ((r_i)), "1" ((r_f)), "d" ((a_i)), "d" ((a_f)))
#else
#define M_ADD(r_i, r_f, a_i, a_f) 	/* r += a */ \
	do { \
		register u_int32 lo_tmp; \
		register u_int32 hi_tmp; \
		\
		lo_tmp = ((r_f) & 0xffff) + ((a_f) & 0xffff); \
		hi_tmp = (((r_f) >> 16) & 0xffff) + (((a_f) >> 16) & 0xffff); \
		if (lo_tmp & 0x10000) \
			hi_tmp++; \
		(r_f) = ((hi_tmp & 0xffff) << 16) | (lo_tmp & 0xffff); \
		\
		(r_i) += (a_i); \
		if (hi_tmp & 0x10000) \
			(r_i)++; \
	} while (0)
#endif /* CISCO */

#ifndef CISCO				/* Not used */
#define M_ADD3(r_ovr, r_i, r_f, a_ovr, a_i, a_f) /* r += a, three word */ \
	do { \
		register u_int32 lo_tmp; \
		register u_int32 hi_tmp; \
		\
		lo_tmp = ((r_f) & 0xffff) + ((a_f) & 0xffff); \
		hi_tmp = (((r_f) >> 16) & 0xffff) + (((a_f) >> 16) & 0xffff); \
		if (lo_tmp & 0x10000) \
			hi_tmp++; \
		(r_f) = ((hi_tmp & 0xffff) << 16) | (lo_tmp & 0xffff); \
		\
		lo_tmp = ((r_i) & 0xffff) + ((a_i) & 0xffff); \
		if (hi_tmp & 0x10000) \
			lo_tmp++; \
		hi_tmp = (((r_i) >> 16) & 0xffff) + (((a_i) >> 16) & 0xffff); \
		if (lo_tmp & 0x10000) \
			hi_tmp++; \
		(r_i) = ((hi_tmp & 0xffff) << 16) | (lo_tmp & 0xffff); \
		\
		(r_ovr) += (a_ovr); \
		if (hi_tmp & 0x10000) \
			(r_ovr)++; \
	} while (0)
#endif /* CISCO */

#if defined(CISCO) && defined(mc68000)
#define M_SUB(r_i, r_f, a_i, a_f)	/* r -= a */ \
	      asm volatile ( "\
subl %5,%1\n\
bcc 0f\n\
subql #1,%0\n\
0:\n\
subl %4,%0" \
: "=dm" ((r_i)), "=dm" ((r_f)) \
: "0" ((r_i)), "1" ((r_f)), "d" ((a_i)), "d" ((a_f)))
#else
#define M_SUB(r_i, r_f, a_i, a_f)	/* r -= a */ \
	do { \
		register u_int32 lo_tmp; \
		register u_int32 hi_tmp; \
		\
		if ((a_f) == 0) { \
			(r_i) -= (a_i); \
		} else { \
			lo_tmp = ((r_f) & 0xffff) + ((-(a_f)) & 0xffff); \
			hi_tmp = (((r_f) >> 16) & 0xffff) \
			    + (((-(a_f)) >> 16) & 0xffff); \
			if (lo_tmp & 0x10000) \
				hi_tmp++; \
			(r_f) = ((hi_tmp & 0xffff) << 16) | (lo_tmp & 0xffff); \
			\
			(r_i) += ~(a_i); \
			if (hi_tmp & 0x10000) \
				(r_i)++; \
		} \
	} while (0)
#endif /* CISCO */

#ifndef CISCO				/* Not used. */
#define	M_RSHIFTU(v_i, v_f)		/* v >>= 1, v is unsigned */ \
	do { \
		(v_f) = (u_int32)(v_f) >> 1; \
		if ((v_i) & 01) \
			(v_f) |= 0x80000000; \
		(v_i) = (u_int32)(v_i) >> 1; \
	} while (0)
#endif /* CISCO */

#if defined(CISCO) && defined(mc68000)
#define	M_RSHIFT(v_i, v_f)		/* v >>= 1, v is signed */ \
	      asm volatile ( "\
asrl #1,%0\n\
roxrl #1,%1"\
: "=d" ((v_i)), "=d" ((v_f)) \
: "0" ((v_i)), "1" ((v_f)))
#else
#define	M_RSHIFT(v_i, v_f)		/* v >>= 1, v is signed */ \
	do { \
		(v_f) = (u_int32)(v_f) >> 1; \
		if ((v_i) & 01) \
			(v_f) |= 0x80000000; \
		if ((v_i) & 0x80000000) \
			(v_i) = ((v_i) >> 1) | 0x80000000; \
		else \
			(v_i) = (v_i) >> 1; \
	} while (0)
#endif /* CISCO */

#if defined(CISCO) && defined(mc68000)
#define	M_LSHIFT(v_i, v_f)		/* v <<= 1 */ \
	      asm volatile ( "\
lsll #1,%1\n\
roxll #1,%0"\
: "=d" ((v_i)), "=d" ((v_f)) \
: "0" ((v_i)), "1" ((v_f)))
#else
#define	M_LSHIFT(v_i, v_f)		/* v <<= 1 */ \
	do { \
		(v_i) <<= 1; \
		if ((v_f) & 0x80000000) \
			(v_i) |= 0x1; \
		(v_f) <<= 1; \
	} while (0)
#endif /* CISCO */

#define	M_LSHIFT3(v_ovr, v_i, v_f)	/* v <<= 1, with overflow */ \
	do { \
		(v_ovr) <<= 1; \
		if ((v_i) & 0x80000000) \
			(v_ovr) |= 0x1; \
		(v_i) <<= 1; \
		if ((v_f) & 0x80000000) \
			(v_i) |= 0x1; \
		(v_f) <<= 1; \
	} while (0)

#if defined(CISCO) && defined(mc68000)
#define M_ADDUF(r_i, r_f, a_f) /* r += a */ \
	      asm volatile ( "\
addl %4,%1\n\
bcc 0f\n\
addql #1,%0\n\
0:"\
: "=dm" ((r_i)), "=dm" ((r_f)) \
: "0" ((r_i)), "1" ((r_f)), "d" ((a_f)))
#else
#define	M_ADDUF(r_i, r_f, uf) 		/* r += uf, uf is u_int32 fraction */ \
	M_ADD((r_i), (r_f), 0, (uf))	/* let optimizer worry about it */
#endif /* CISCO */

#define	M_SUBUF(r_i, r_f, uf)		/* r -= uf, uf is u_int32 fraction */ \
	M_SUB((r_i), (r_f), 0, (uf))	/* let optimizer worry about it */

#define	M_ADDF(r_i, r_f, f)		/* r += f, f is a LONG fraction */ \
	do { \
		if ((f) > 0) \
			M_ADD((r_i), (r_f), 0, (f)); \
		else if ((f) < 0) \
			M_ADD((r_i), (r_f), (-1), (f));\
	} while(0)

#if defined(CISCO) && defined(mc68000)
#define	M_ISNEG(v_i, v_f) 		/* v < 0 */ \
        (((long) (v_i)) < 0)
#else
#define	M_ISNEG(v_i, v_f) 		/* v < 0 */ \
	(((v_i) & 0x80000000) != 0)
#endif /* CISCO */

#define	M_ISHIS(a_i, a_f, b_i, b_f)	/* a >= b unsigned */ \
	(((u_int32)(a_i)) > ((u_int32)(b_i)) || \
	  ((a_i) == (b_i) && ((u_int32)(a_f)) >= ((u_int32)(b_f))))

#define	M_ISGEQ(a_i, a_f, b_i, b_f)	/* a >= b signed */ \
	(((int32)(a_i)) > ((int32)(b_i)) || \
	  ((a_i) == (b_i) && ((u_int32)(a_f)) >= ((u_int32)(b_f))))

#define	M_ISEQU(a_i, a_f, b_i, b_f)	/* a == b unsigned */ \
	((a_i) == (b_i) && (a_f) == (b_f))

/*
 * Operations on the long fp format
 */
#define	L_ADD(r, a)	M_ADD((r)->l_ui, (r)->l_uf, (a)->l_ui, (a)->l_uf)
#define	L_SUB(r, a)	M_SUB((r)->l_ui, (r)->l_uf, (a)->l_ui, (a)->l_uf)
#define	L_NEG(v)	M_NEG((v)->l_ui, (v)->l_uf)
#define L_ADDUF(r, uf)	M_ADDUF((r)->l_ui, (r)->l_uf, (uf))
#define L_SUBUF(r, uf)	M_SUBUF((r)->l_ui, (r)->l_uf, (uf))
#define	L_ADDF(r, f)	M_ADDF((r)->l_ui, (r)->l_uf, (f))
#define	L_RSHIFT(v)	M_RSHIFT((v)->l_i, (v)->l_uf)
#define	L_RSHIFTU(v)	M_RSHIFT((v)->l_ui, (v)->l_uf)
#define	L_LSHIFT(v)	M_LSHIFT((v)->l_ui, (v)->l_uf)
#define	L_CLR(v)	((v)->l_ui = (v)->l_uf = 0)

#define	L_ISNEG(v)	(((v)->l_ui & 0x80000000) != 0)
#define L_ISZERO(v)	((v)->l_ui == 0 && (v)->l_uf == 0)
#define	L_ISHIS(a, b)	((a)->l_ui > (b)->l_ui || \
			  ((a)->l_ui == (b)->l_ui && (a)->l_uf >= (b)->l_uf))
#define	L_ISGEQ(a, b)	((a)->l_i > (b)->l_i || \
			  ((a)->l_i == (b)->l_i && (a)->l_uf >= (b)->l_uf))
#define	L_ISEQU(a, b)	M_ISEQU((a)->l_ui, (a)->l_uf, (b)->l_ui, (b)->l_uf)

extern	char *	dofptoa		P((u_fp, int, int, int));
extern	char *	dolfptoa	P((u_long, u_long, int, int, int));

extern	int	atolfp		P((const char *, l_fp *));
extern	int	buftvtots	P((const char *, l_fp *));
extern	void	gettstamp	P((l_fp *));
extern	char *	fptoa		P((s_fp, int));
extern	char *	fptoms		P((s_fp, int));
extern	char *	fptoms		P((s_fp, int));
extern	int	hextolfp	P((const char *, l_fp *));
extern	int	mstolfp		P((const char *, l_fp *));
extern	char *	prettydate	P((l_fp *));
extern	char *	uglydate	P((l_fp *));

extern	void	get_systime	P((l_fp *));
extern	int	step_systime	P((l_fp *));
extern	int	adj_systime	P((l_fp *));

#define	lfptoa(_fpv, _ndec)	mfptoa((_fpv)->l_ui, (_fpv)->l_uf, (_ndec))
#define	lfptoms(_fpv, _ndec)	mfptoms((_fpv)->l_ui, (_fpv)->l_uf, (_ndec))

#ifndef CISCO
#define	ntoa(_sin)		numtoa((_sin)->sin_addr.s_addr)
#define	ntohost(_sin)		numtohost((_sin)->sin_addr.s_addr)
#endif /* CISCO */

#define	ufptoa(_fpv, _ndec)	dofptoa((_fpv), 0, (_ndec), 0)
#define	ufptoms(_fpv, _ndec)	dofptoa((_fpv), 0, (_ndec), 1)
#define	ulfptoa(_fpv, _ndec)	dolfptoa((_fpv)->l_ui, (_fpv)->l_uf, 0, (_ndec), 0)
#define	ulfptoms(_fpv, _ndec)	dolfptoa((_fpv)->l_ui, (_fpv)->l_uf, 0, (_ndec), 1)
#define	umfptoa(_fpi, _fpf, _ndec) dolfptoa((_fpi), (_fpf), 0, (_ndec), 0)

#endif /* NTP_FP_H */
