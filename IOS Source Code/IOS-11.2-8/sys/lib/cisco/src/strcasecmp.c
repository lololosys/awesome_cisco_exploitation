/* $Id: strcasecmp.c,v 3.1.10.1 1996/03/18 20:51:25 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/lib/cisco/src/strcasecmp.c,v $
 *------------------------------------------------------------------
 * strcasecmp.c -- case insensitive character string compare
 *
 * December 1995, Mark D. Baushke
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: strcasecmp.c,v $
 * Revision 3.1.10.1  1996/03/18  20:51:25  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.1.2.2  1996/03/12  22:48:44  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 * bugfix catch the end of string correctly.
 *
 * Revision 3.1.2.1  1996/03/07  08:20:44  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.1  1996/03/05  04:47:32  mdb
 * placeholder for DeadKingOnAThrone_branch
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
FUNCTION
	strcasecmp---case insensitive character string compare
	
ANSI_SYNOPSIS
	#include <ciscolib.h>
	int strcasecmp(const char *a, const char *b);

DESCRIPTION
	strcasecmp compares the string at a to the string at b in a
	case-insensitive manner.

RETURNS 
	If *a sorts lexicographically after *b (after both are
	converted to uppercase), strcasecmp returns a number greater
	than zero.  If the two strings match, strcasecmp returns zero.
	If *a sorts lexicographically before *b, strcasecmp returns a
	number less than zero.

PORTABILITY
	strcasecmp is in the Berkeley Software Distribution.

	strcasecmp requires no supporting OS subroutines. It uses
	the ANSI C function toupper().
*/

#include <string.h>
#include <ctype.h>
#include <ciscolib.h>

/*
 * ustrcmp
 * case insensitive string match
 * returns an integer <,=,> 0 depending on whether s1 is <,=,> s2
 */

int
strcasecmp (const char *s1, const char *s2)
{
    const unsigned char *us1 = (const unsigned char *) s1;
    const unsigned char *us2 = (const unsigned char *) s2;

    if ((!s1 && s2) || (s1 && !s2)) /* no match if only one ptr is NULL */
	return (int) (s1 - s2);	/* if one of these is NULL it will be the
				   lesser of the two values and therefore
				   we'll get the proper sign in the int */

    if (s1 == s2)		/* match if both ptrs the same (e.g. NULL) */
	return 0;

    while (*us1 != '\0' && toupper(*us1) == toupper(*us2)) {
	us1++;
	us2++;
    }

    return(toupper(*us1) - toupper(*us2));
}
