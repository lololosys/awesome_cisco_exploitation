/* $Id: strncat.c,v 3.1.10.1 1996/03/18 20:49:35 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/lib/ansi/string/strncat.c,v $
 *------------------------------------------------------------------
 * strncpy.c -- copy a counted non-overlapping string
 *
 * January 1996, Mark D. Baushke
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: strncat.c,v $
 * Revision 3.1.10.1  1996/03/18  20:49:35  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.1.2.1  1996/03/07  08:19:52  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.1  1996/03/05  03:40:04  mdb
 * placeholder for DeadKingOnAThrone_branch
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
FUNCTION
        strncat---concatenate strings

ANSI_SYNOPSIS
        #include <string.h>
        char *strncat(char *dst, const char *src, size_t length);

DESCRIPTION
        strncat appends not more than length characters from the
        string pointed to by src (including the terminating null
        character) to the end of the string pointed to by dst.  The
        initial character of src overwrites the null character at the
        end of dst.  A terminating null character is always appended
        to the result

WARNINGS
        Note that a null is always appended, so that if the copy is
        limited by the length argument, the number of characters
        appended to dst is n + 1.


RETURNS
        This function returns the initial value of dst

PORTABILITY
        strncat is ANSI C.

        strncat requires no supporting OS subroutines.
*/

#include <string.h>

char *
strncat (char *dst, const char *src, size_t n)
{
    char *d = dst;
    const char *s = src;

    while (*d)
        d++;
    while (n-- != 0 && (*d++ = *s++)) {
        if (n == 0)
            *d = '\0';
    }

    return dst;
}
