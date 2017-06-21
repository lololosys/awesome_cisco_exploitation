/* $Id: strncpy.c,v 3.1.10.1 1996/03/18 20:49:38 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/lib/ansi/string/strncpy.c,v $
 *------------------------------------------------------------------
 * strncpy.c -- counted copy string
 *
 * January 1996, Mark D. Baushke
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: strncpy.c,v $
 * Revision 3.1.10.1  1996/03/18  20:49:38  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.1.2.1  1996/03/07  08:19:55  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.1  1996/03/05  03:40:08  mdb
 * placeholder for DeadKingOnAThrone_branch
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
FUNCTION
	strncpy---counted copy string

ANSI_SYNOPSIS
	#include <string.h>
	char *strncpy(char *dst, const char *src, size_t length);

DESCRIPTION
	strncpy copies not more than length characters from the
	the string pointed to by src (including the terminating
	null character) to the array pointed to by dst.  If the
	string pointed to by src is shorter than length
	characters, null characters are appended to the destination
	array until a total of length characters have been
	written.

RETURNS
	This function returns the initial value of dst.

PORTABILITY
strncpy is ANSI C.

strncpy requires no supporting OS subroutines.

QUICKREF
	strncpy ansi pure
*/

#include <string.h>

char *
strncpy (char *dst, const char *src, size_t n)
{
    char *dscan;
    const char *sscan;
    size_t count;

    dscan = dst;
    sscan = src;
    count = n;
    /*
     * Copy the source to the dest until there is a NULL
     */
    while (count > 0) {
	--count;
	if ((*dscan++ = *sscan++) == '\0')
	    break;
    }
    /*
     * Pad the rest of dest with NULLs
     */
    while (count-- > 0)
	*dscan++ = '\0';

    return dst;
}
