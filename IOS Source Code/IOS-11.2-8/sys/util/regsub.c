/* $Id: regsub.c,v 3.2.62.1 1996/03/18 22:31:00 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/util/regsub.c,v $
 *------------------------------------------------------------------
 * regsub.c - Regular expression support
 *
 * Copyright (c) 1994,1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: regsub.c,v $
 * Revision 3.2.62.1  1996/03/18  22:31:00  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.2  1996/03/07  11:00:12  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.2.26.1  1996/02/20  21:45:24  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.2  1995/11/17  17:56:38  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:43:42  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:13:23  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * regsub
 *
 *	Copyright (c) 1986 by University of Toronto.
 *	Written by Henry Spencer.  Not derived from licensed software.
 *
 *	Permission is granted to anyone to use this software for any
 *	purpose on any computer system, and to redistribute it freely,
 *	subject to the following restrictions:
 *
 *	1. The author is not responsible for the consequences of use of
 *		this software, no matter how awful, even if they arise
 *		from defects in it.
 *
 *	2. The origin of this software must not be misrepresented, either
 *		by explicit claim or by omission.
 *
 *	3. Altered versions must be plainly marked as such, and must not
 *		be misrepresented as being the original software.
 */
#include "master.h"
#include "regexp.h"
#include <ciscolib.h>
#include <string.h>

#ifndef CHARBITS
#define	UCHARAT(p)	((int)*(unsigned char *)(p))
#else
#define	UCHARAT(p)	((int)*(p)&CHARBITS)
#endif

/*
 - regsub - perform substitutions after a regexp match
 */
int regsub(regexp *prog, char *source, char *dest, int maxlen)
{
	register char *src;
	register char *dst;
	register char c;
	register int no;
	register int len;

	if (prog == NULL || source == NULL || dest == NULL) {
		regerror("NULL parm to regsub");
		return -1;
	}
	if (UCHARAT(prog->program) != MAGIC) {
		regerror("damaged regexp fed to regsub");
		return -1;
	}

	src = source;
	dst = dest;
	while ((c = *src++) != '\0' && ((dst-dest) < maxlen)) {
		if (c == '&')
			no = 0;
		else if (c == '\\' && '0' <= *src && *src <= '9')
			no = *src++ - '0';
		else
			no = -1;

		if (no < 0) {	/* Ordinary character. */
			if (c == '\\' && (*src == '\\' || *src == '&'))
				c = *src++;
			*dst++ = c;
		} else if (prog->startp[no] != NULL && prog->endp[no] != NULL) {
			len = prog->endp[no] - prog->startp[no];
			if (((dst-dest)+len) >= maxlen) {
                            regerror("pattern substitution too long");
			    return (-1);
			}
	/* Don't know why first +1 is in there, but adding second, and 
           adding 1 byte to the malloc to have adequate space for the null */
			sstrncpy(dst, prog->startp[no], len+1+1);
			dst += len;
		}
	}
	if ((c == '\0') && ((dst-dest) < maxlen)) {
	    *dst++ = '\0';
	    return (dst-dest);
	} else {
	    regerror("pattern substitution too long");
	    return (-1);
	}
}
