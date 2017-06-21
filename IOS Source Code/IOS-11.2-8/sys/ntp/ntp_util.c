/* $Id: ntp_util.c,v 3.4.12.5 1996/08/20 19:14:06 dkatz Exp $
 * $Source: /release/112/cvs/Xsys/ntp/ntp_util.c,v $
 *------------------------------------------------------------------
 * NTP utility routines
 *
 * Dave Katz, November 1992
 *
 * Copyright (c) 1992-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: ntp_util.c,v $
 * Revision 3.4.12.5  1996/08/20  19:14:06  dkatz
 * CSCdi65483:  NTP needs modularity cleanup
 * Branch: California_branch
 * Squeeze out another 2.5KB.
 *
 * Revision 3.4.12.4  1996/08/20  03:14:02  dkatz
 * CSCdi65483:  NTP needs modularity cleanup
 * Branch: California_branch
 * Code review comments.
 *
 * Revision 3.4.12.3  1996/08/19  18:55:25  dkatz
 * CSCdi65483:  NTP needs modularity cleanup
 * Branch: California_branch
 * Break up NTP into separate subsystems.  Significantly reduce image size
 * and stack usage.  Clean up refclock interface.  Make all modules
 * compile cleanly with all error checking enabled (DUSTY is clean!)
 *
 * Revision 3.4.12.2  1996/04/16  19:03:32  rchandra
 * CSCdi54830:  IP features commit
 * Branch: California_branch
 *
 * Revision 3.4.12.1  1996/03/18  21:12:25  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.20.3  1996/03/16  07:09:46  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.3.20.2  1996/03/07  10:07:02  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.3.20.1  1996/02/20  14:49:28  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.4  1996/02/15  09:08:32  smackie
 * You can't take the address of a "register" variable. Not that the
 * "register" keyword does anything anyway with the current setting
 * of gcc optimizer we use. (CSCdi49069)
 *
 * Revision 3.3  1995/12/01  09:19:53  dkatz
 * CSCdi43035:  NTP may not sync quickly at system startup
 *
 * Revision 3.2  1995/11/17  17:51:26  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:41:25  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/06/21  08:55:53  smackie
 * Rename malloc memory pool derivatives so that they're prefixed with
 * malloc_ for consistency. (CSCdi36222)
 *
 * Revision 2.2  1995/06/16  04:28:50  dkatz
 * CSCdi35882:  Time-of-day code needs cleanup
 *
 * Revision 2.1  1995/06/07  21:46:00  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Copyright 1992 David L. Mills
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appears in all copies and
 * that both the copyright notice and this permission notice appear in
 * supporting documentation, and that the name University of Delaware
 * not be used in advertising or publicity pertaining to distribution of
 * the software without specific, written prior permission.  The
 * University of Delaware makes no representations about the suitability
 * of this software for any purpose.  It is provided "as is" without
 * express or implied warranty.
 *
 * Authors
 *
 * Dennis Ferguson <dennis@mrbill.canet.ca> (foundation code for NTP
 *      Version 2 as specified in RFC-1119)
 * Lars H. Mathiesen <thorinn@diku.dk> (adaptation of foundation code for
 *      Version 3 as specified in RFC01305)
 * Louis A. Mamakos <louie@ni.umd.edu> (support for md5-based 
 *      authentication)
 * Craig Leres <leres@ee.lbl.gov> (port to 4.4BSD operating system,
 *	ppsclock, Maganavox GPS clock driver)
 * Nick Sayer <mrapple@quack.sac.ca.us> (SunOS streams modules)
 * Frank Kardel <Frank.Kardel@informatik.uni-erlangen.de> (DCF77 clock
 *      driver, STREAMS module for DCF77, support scripts)
 * Rainer Pruy <Rainer.Pruy@informatik.uni-erlangen.de> (monitoring/trap
 *      scripts, statistics file handling)
 * Glenn Hollinger <glenn@herald.usask.ca> (GOES clock driver)
 * Kenneth Stone <ken@sdd.hp.com> (port to HPUX operating system)
 * Dave Katz <dkatz@cisco.com> (port to RS/6000 AIX operating system)
 * William L. Jones <jones@hermes.chpc.utexas.edu> (RS/6000 AIX
 *	modifications, HPUX modifications)
 * John A. Dundas III <dundas@salt.jpl.nasa.gov> (Apple A/UX port)
 * David L. Mills <mills@udel.edu> (WWVB and Austron GPS clock drivers,
 *	pps support)
 * Jeffrey Mogul <mogul@pa.dec.com> (ntptrace utility)
 * Steve Clift (clift@ml.csiro.au) OMEGA clock driver)
 */

#define CISCO

#include "master.h"
#include "ntpd.h"
#include <string.h>
#include <ciscolib.h>
#include <ctype.h>
#include "../os/clock.h"
#include "../os/clock_private.h"
#include "../parser/parser_defs_service.h"

#define CLOCK_NUMBUFS 5
#define CLOCK_BUFLENGTH 30
static char *clock_stringbuf[CLOCK_NUMBUFS];
static int clock_nextbuf;

/*
 * ntp_util_getbuf
 *
 * Get a "static" buffer for clock conversion purposes.
 *
 * These are actually dynamically allocated as needed, but then reused.
 *
 * This routine may return NULL if we're out of memory.
 */

static char *ntp_util_getbuf (void)
{
    char *result;

    result = clock_stringbuf[clock_nextbuf];
    if (result == NULL) {
	result = clock_stringbuf[clock_nextbuf] = malloc_named(CLOCK_BUFLENGTH,
							       "Clock buffer");
    }
    clock_nextbuf++;
    if (clock_nextbuf >= CLOCK_NUMBUFS)
	clock_nextbuf = 0;
    return(result);
}

/*
 * dofptoa - do the grunge work to convert an fp number to ascii
 */

char *
dofptoa(fpv, neg, ndec, msec)
	u_fp fpv;
	int neg;
	int ndec;
	int msec;
{
	register u_char *cp, *cpend;
	register u_long val;
	register short dec;
	u_char cbuf[12];
	u_char *cpdec;
	char *buf;
	char *bp;

	/*
	 * Get a string buffer before starting
	 */
	buf = ntp_util_getbuf();
	if (buf == NULL)
	    return("");			/* At least return a non-NULL string */

	/*
	 * Zero out the buffer
	 */
	memset(cbuf, 0, sizeof cbuf);

	/*
	 * Set the pointers to point at the first
	 * decimal place.  Get a local copy of the value.
	 */
	cp = cpend = &cbuf[5];
	val = fpv;

	/*
	 * If we have to, decode the integral part
	 */
	if (!(val & 0xffff0000))
		cp--;
	else {
		register u_short sv = (u_short)(val >> 16);
		register u_short tmp;
		register u_short ten = 10;

		do {
			tmp = sv;
			sv /= ten;
			*(--cp) = tmp - ((sv<<3) + (sv<<1));
		} while (sv != 0);
	}

	/*
	 * Figure out how much of the fraction to do
	 */
	if (msec) {
		dec = ndec + 3;
		if (dec < 3)
			dec = 3;
		cpdec = &cbuf[8];
	} else {
		dec = ndec;
		cpdec = cpend;
	}

	if (dec > 6)
		dec = 6;
	
	if (dec > 0) {
		do {
			val &= 0xffff;
			val = (val << 3) + (val << 1);
			*cpend++ = (u_char)(val >> 16);
		} while (--dec > 0);
	}

	if (val & 0x8000) {
		register u_char *tp;
		/*
		 * Round it. Ick.
		 */
		tp = cpend;
		*(--tp) += 1;
		while (*tp >= 10) {
			*tp = 0;
			*(--tp) += 1;
		}
	}

	/*
	 * Remove leading zeroes if necessary
	 */
	while (cp < (cpdec -1) && *cp == 0)
		cp++;
	
	/*
	 * Copy it into the buffer, asciizing as we go.
	 */
	bp = buf;
	if (neg)
		*bp++ = '-';
	
	while (cp < cpend) {
		if (cp == cpdec)
			*bp++ = '.';
		*bp++ = (char)(*cp++ + '0');
	}
	*bp = '\0';
	return buf;
}

/*
 * dolfptoa - do the grunge work of converting an l_fp number to decimal
 */
char *
dolfptoa(fpi, fpv, neg, ndec, msec)
	u_long fpi;
	u_long fpv;
	int neg;
	int ndec;
	int msec;
{
	register u_char *cp, *cpend;
	u_long work_i;
	register int dec;
	u_char cbuf[24];
	u_char *cpdec;
	char *buf;
	char *bp;

	/*
	 * Get a string buffer before starting
	 */
	buf = ntp_util_getbuf();
	if (buf == NULL)
	    return("");			/* Always return a valid pointer. */

	/*
	 * Zero the character buffer
	 */
	memset(cbuf, 0, sizeof(cbuf));

	/*
	 * Work on the integral part.  This is biased by what I know
	 * compiles fairly well for a 68000.
	 */
	cp = cpend = &cbuf[10];
	work_i = fpi;
	if (work_i & 0xffff0000) {
		register u_long lten = 10;
		register u_long ltmp;

		do {
			ltmp = work_i;
			work_i /= lten;
			ltmp -= (work_i<<3) + (work_i<<1);
			*--cp = (u_char)ltmp;
		} while (work_i & 0xffff0000);
	}
	if (work_i != 0) {
		register u_short sten = 10;
		register u_short stmp;
		register u_short swork = (u_short)work_i;

		do {
			stmp = swork;
			swork /= sten;
			stmp -= (swork<<3) + (swork<<1);
			*--cp = (u_char)stmp;
		} while (swork != 0);
	}

	/*
	 * Done that, now deal with the problem of the fraction.  First
	 * determine the number of decimal places.
	 */
	if (msec) {
		dec = ndec + 3;
		if (dec < 3)
			dec = 3;
		cpdec = &cbuf[13];
	} else {
		dec = ndec;
		if (dec < 0)
			dec = 0;
		cpdec = &cbuf[10];
	}
	if (dec > 12)
		dec = 12;
	
	/*
	 * If there's a fraction to deal with, do so.
	 */
	if (fpv != 0) {
		u_long work_f;

		work_f = fpv;
		while (dec > 0) {
			register u_long tmp_i;
			register u_long tmp_f;

			dec--;
			/*
			 * The scheme here is to multiply the
			 * fraction (0.1234...) by ten.  This moves
			 * a junk of BCD into the units part.
			 * record that and iterate.
			 */
			work_i = 0;
			M_LSHIFT(work_i, work_f);
			tmp_i = work_i;
			tmp_f = work_f;
			M_LSHIFT(work_i, work_f);
			M_LSHIFT(work_i, work_f);
			M_ADD(work_i, work_f, tmp_i, tmp_f);
			*cpend++ = (u_char)work_i;
			if (work_f == 0)
				break;
		}

		/*
		 * Rounding is rotten
		 */
		if (work_f & 0x80000000) {
			register u_char *tp = cpend;

			*(--tp) += 1;
			while (*tp >= 10) {
				*tp = 0;
				*(--tp) += 1;
			};
			if (tp < cp)
				cp = tp;
		}
	}
	cpend += dec;


	/*
	 * We've now got the fraction in cbuf[], with cp pointing at
	 * the first character, cpend pointing past the last, and
	 * cpdec pointing at the first character past the decimal.
	 * Remove leading zeros, then format the number into the
	 * buffer.
	 */
	while (cp < cpdec) {
		if (*cp != 0)
			break;
		cp++;
	}
	if (cp == cpdec)
		--cp;

	bp = buf;
	if (neg)
		*bp++ = '-';
	while (cp < cpend) {
		if (cp == cpdec)
			*bp++ = '.';
		*bp++ = (char)(*cp++ + '0');	/* ascii dependent? */
	}
	*bp = '\0';

	/*
	 * Done!
	 */
	return buf;
}


/*
 * mfptoa - Return an asciized representation of a signed long fp number
 */
char *
mfptoa(u_long fpi, u_long fpf, int ndec)
{
	int isneg;

	if (M_ISNEG(fpi, fpf)) {
		isneg = 1;
		M_NEG(fpi, fpf);
	} else
		isneg = 0;

	return dolfptoa(fpi, fpf, isneg, ndec, 0);
}

/*
 * fptoa - return an asciized representation of an s_fp number
 */

char *fptoa(fpv, ndec)
	s_fp fpv;
	int ndec;
{
	u_fp plusfp;
	int neg;

	if (fpv < 0) {
		plusfp = (u_fp)(-fpv);
		neg = 1;
	} else {
		plusfp = (u_fp)fpv;
		neg = 0;
	}

	return dofptoa(plusfp, neg, ndec, 0);
}

/*
 * calleapwhen - determine the number of seconds to the next possible
 *		 leap occurance and the last one.
 *
 * The CISCO version returns the epoch of the next leap second.
 */

/*
 * calleaptab - leaps occur at the end of December and June
 */
static const long calleaptab[10] = {
	-(JAN+FEBLEAP)*SECSPERDAY,	/* leap previous to cycle */
	(MAR+APR+MAY+JUN)*SECSPERDAY,	/* end of June */
	(MAR+APR+MAY+JUN+JUL+AUG+SEP+OCT+NOV+DEC)*SECSPERDAY, /* end of Dec */
	(MAR+APR+MAY+JUN)*SECSPERDAY + SECSPERYEAR,
	(MAR+APR+MAY+JUN+JUL+AUG+SEP+OCT+NOV+DEC)*SECSPERDAY + SECSPERYEAR,
	(MAR+APR+MAY+JUN)*SECSPERDAY + 2*SECSPERYEAR,
	(MAR+APR+MAY+JUN+JUL+AUG+SEP+OCT+NOV+DEC)*SECSPERDAY + 2*SECSPERYEAR,
	(MAR+APR+MAY+JUN)*SECSPERDAY + 3*SECSPERYEAR,
	(MAR+APR+MAY+JUN+JUL+AUG+SEP+OCT+NOV+DEC)*SECSPERDAY + 3*SECSPERYEAR,
	(MAR+APR+MAY+JUN+JUL+AUG+SEP+OCT+NOV+DEC+JAN+FEBLEAP+MAR+APR+MAY+JUN)
	    *SECSPERDAY + 3*SECSPERYEAR,	/* next after current cycle */
};

void
calleapwhen(u_long ntpdate, u_long *leaplast, u_long *leapnext)
{
	register u_long dateincycle;
	register int i;

	/*
	 * Find the offset from the start of the cycle
	 */
	dateincycle = ntpdate;
	if (dateincycle >= (u_long)(MAR1992))
		dateincycle -= (u_long)(MAR1992);
	else
		dateincycle -= MAR1900;

	while (dateincycle >= SECSPERCYCLE)
		dateincycle -= SECSPERCYCLE;

	/*
	 * Find where we are with respect to the leap events.
	 */
	for (i = 1; i < 9; i++)
		if (dateincycle < (u_long)calleaptab[i])
			break;
	
	/*
	 * i points at the next leap.  Compute the last and the next.
	 */
	*leaplast = (u_long)((long)dateincycle - calleaptab[i-1]);
	*leapnext = (u_long)(calleaptab[i] - (long)dateincycle);
}


/*
 * ranp2 - return a random integer in the range 0 .. (1<<m)-1
 */
static ulong seed = 0;
ulong ranp2(int m)
{
    l_fp curtime;
    u_int mask = (1 << m) - 1;
    ulong retval;
    clock_get_time_exact(&curtime);	/* Make it pretty durned random. */
    retval = (curtime.l_uf ^ seed) & mask;
    seed = retval >> 8;
    return(retval);
}

/*
 * ntoa - return a formatted string with an IP address
 */
char *ntoa(addr)
struct sockaddr_in *addr;
{
    char *buf;

    buf = ntp_util_getbuf();
    if (buf == NULL)
	return("");			/* Always return a valid pointer. */
    (void) sprintf(buf, "%i", addr->sin_addr.s_addr);
    return(buf);
}

static const char * const local_event_msg[] = {
    "unspecified event",	/* EVNT_UNSPEC */
    "system restart",		/* EVNT_SYSRESTART */
    "system fault",		/* EVNT_SYSFAULT */
    "sync change",		/* EVNT_SYNCCHG */
    "peer stratum change",	/* EVNT_PEERSTCHG */
    "clock reset",		/* EVNT_CLOCKRESET */
    "bad date/time",		/* EVNT_BADDATETIM */
    "clock exception"		/* EVNT_CLOCKEXCPT */
    };

static const char * const peer_event_msg[] = {
    "",				/* no event 0! */
    "IP error",			/* EVNT_PEERIPERR */
    "authentication failure",	/* EVNT_PEERAUTH */
    "unreachable",		/* EVNT_UNREACH */
    "reachable",		/* EVNT_REACH */
    "peer clock"		/* EVNT_PEERCLOCK */
    };

void report_event(event, peer)
int event;
struct peer *peer;
{
    if (ntp_event_debug) {
	if (event & PEER_EVENT) {
	    buginf("\nNTP: %s %s", ntoa(&(peer->srcadr)), 
		   peer_event_msg[event & (~PEER_EVENT)]);
	} else {
	    buginf("\nNTP: %s", local_event_msg[event]);
	}
    }
}

/*
 * mfptoms - Return an asciized signed long fp number in milliseconds
 */
char *
mfptoms(u_long fpi, u_long fpf, int ndec)
{
	int isneg;

	if (M_ISNEG(fpi, fpf)) {
		isneg = 1;
		M_NEG(fpi, fpf);
	} else
		isneg = 0;

	return dolfptoa(fpi, fpf, isneg, ndec, 1);
}

/*
 * fptoms - return an asciized s_fp number in milliseconds
 */
char *
fptoms (fpv, ndec)
	s_fp fpv;
	int ndec;
{
	u_fp plusfp;
	int neg;

	if (fpv < 0) {
		plusfp = (u_fp)(-fpv);
		neg = 1;
	} else {
		plusfp = (u_fp)fpv;
		neg = 0;
	}

	return dofptoa(plusfp, neg, ndec, 1);
}
/*
 * numtoa - return asciized network numbers store in local array space
 */
char *
numtoa (u_long num)
{
	register u_long netnum;
	register char *buf;

	netnum = ntohl(num);
	buf = ntp_util_getbuf();
	if (buf == NULL)
	    return("");

	(void) sprintf(buf, "%d.%d.%d.%d", (netnum>>24)&0xff,
	    (netnum>>16)&0xff, (netnum>>8)&0xff, netnum&0xff);

	return buf;
}

/*
 * atoint - convert an ascii string to a signed long, with error checking
 */
int
atoint (char *str, long *ival)
{
	register u_long u;
	register char *cp;
	register int isneg;
	register int oflow_digit;

	cp = str;

	if (*cp == '-') {
		cp++;
		isneg = 1;
		oflow_digit = '8';
	} else {
		isneg = 0;
		oflow_digit = '7';
	}

	if (*cp == '\0')
		return 0;

	u = 0;
	while (*cp != '\0') {
		if (!isdigit(*cp))
			return 0;
		if (u > 214748364 || (u == 214748364 && *cp > oflow_digit))
			return 0;	/* overflow */
		u = (u << 3) + (u << 1);
		u += *cp++ - '0';	/* ascii dependent */
	}

	if (isneg)
		*ival = -((long)u);
	else 
		*ival = (long)u;
	return 1;
}

/*
 * Returns TRUE if we are truly synchronized
 */
boolean ntp_clock_is_synchronized (void)
{
    return((sys_peer != NULL) && (sys_leap != LEAP_NOTINSYNC));
}
