/* $Id: strcat.c,v 3.1.10.1 1996/03/18 20:49:25 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/lib/ansi/string/strcat.c,v $
 *------------------------------------------------------------------
 * strcat.c -- concatenate two non-overlapping strings
 *
 * January 1996, Mark D. Baushke
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: strcat.c,v $
 * Revision 3.1.10.1  1996/03/18  20:49:25  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.1.2.1  1996/03/07  08:19:43  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.1  1996/03/05  03:39:47  mdb
 * placeholder for DeadKingOnAThrone_branch
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
FUNCTION
        strcat---concatenate strings

ANSI_SYNOPSIS
        #include <string.h>
        char *strcat(char *dst, const char *src);

DESCRIPTION
        strcat appends a copy of the string pointed to by src
        (including the terminating null character) to the end of the
        string pointed to by dst.  The initial character of
        src overwrites the null character at the end of dst. The
        source and destination may not overlap.

RETURNS
        This function returns the initial value of dst

PORTABILITY
        strcat is ANSI C.

        strcat requires no supporting OS subroutines.
*/

#include <string.h>

char *
strcat (char *dest, const char *src)
{
    char *orig_dest = dest;

    while (*dest) {
        dest++;
    }

    while (*src) {
        *dest++ = *src++;
    }

    *dest = '\0';

    return orig_dest;
}
