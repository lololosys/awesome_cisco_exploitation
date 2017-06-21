/* $Id: strncasecmp.c,v 3.1.10.1 1996/03/18 20:51:29 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/lib/cisco/src/strncasecmp.c,v $
 *------------------------------------------------------------------
 * strncasecmp.c -- case insensitive character string compare
 *
 * December 1995, Mark D. Baushke
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: strncasecmp.c,v $
 * Revision 3.1.10.1  1996/03/18  20:51:29  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.1.2.1  1996/03/07  08:20:46  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.1  1996/03/05  04:47:35  mdb
 * placeholder for DeadKingOnAThrone_branch
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
FUNCTION
	strncasecmp---case insensitive character string compare
	
ANSI_SYNOPSIS
	#include <ciscolib.h>
	int strncasecmp(const char *a, const char *b, size_t length);

DESCRIPTION
	strncasecmp compares up to length characters from the string
	at a to the string at a to the string at b in a
	case-insensitive manner.

RETURNS 
	If *a sorts lexicographically after *b (after both are
	converted to uppercase), strncasecmp returns a number greater
	than zero.  If the two strings are quivalent, strncasecmp
	returns zero. If *a sorts lexicographically before *b,
	strncasecmp returns a umber less than zero.

PORTABILITY
	strncasecmp is in the Berkeley Software Distribution.

	strncasecmp requires no supporting OS subroutines. It uses
	the ANSI C function toupper().
*/

#include <ciscolib.h>
#include <string.h>
#include <ctype.h>

/*
 * strncasecmp
 * case insensitive string match
 * returns an integer <,=,> 0 depending on whether s1 is <,=,> s2
 */

int
strncasecmp (const char *s1, const char *s2, unsigned long length)
{
    const unsigned char *us1 = (const unsigned char *) s1;
    const unsigned char *us2 = (const unsigned char *) s2;

    if ((!s1 && s2) || (s1 && !s2)) /* no match if only one ptr is NULL */
	return((int) (s1 - s2));

    if ((length == 0) || (s1 == s2))
	return 0;

    while (length-- > 0 && toupper(*us1) == toupper(*us2)) {
	if (length == 0 || *us1 == '\0' || *us2 == '\0')
	    break;
	us1++;
	us2++;
    }

    return(toupper(*us1) - toupper(*us2));
}
