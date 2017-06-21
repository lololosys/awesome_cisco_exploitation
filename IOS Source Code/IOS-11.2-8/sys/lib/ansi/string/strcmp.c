/* $Id: strcmp.c,v 3.1.10.1 1996/03/18 20:49:28 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/lib/ansi/string/strcmp.c,v $
 *------------------------------------------------------------------
 * strcmp.c -- compares two strings
 *
 * December 1995, Mark D. Baushke
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: strcmp.c,v $
 * Revision 3.1.10.1  1996/03/18  20:49:28  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.1.2.1  1996/03/07  08:19:45  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.1  1996/03/05  03:39:50  mdb
 * placeholder for DeadKingOnAThrone_branch
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
FUNCTION
        strcmp---character string compare

ANSI_SYNOPSIS
        #include <string.h>
        int strcmp(const char *a, const char *b);

DESCRIPTION
        strcmp compares the string at a to the string at b.

RETURNS
        If *a sorts lexicographically after *b, strcmp returns a
        number greater than zero.  If the two strings match, strcmp
        returns zero.  If *a sorts lexicographically before *b, strcmp
        returns a number less than zero.

PORTABILITY
        strcmp is ANSI C.

        strcmp requires no supporting OS subroutines.
*/

#include <string.h>

/*
 * strcmp
 * case sensitive string match
 * returns an integer <,=,> 0 depending on whether s1 is <,=,> s2
 */

int
strcmp (const char *s1, const char *s2)
{
    /* XXX on systems where NULL != 0x0 the next return will fail */
    if ((!s1 && s2) || (s1 && !s2)) /* no match if only one ptr is NULL */
        return (int) (s1 - s2); /* if one of these is NULL it will be the
                                   lesser of the two values and therefore
                                   we'll get the proper sign in the int */

    if (s1 == s2)               /* match if both ptrs the same (e.g. NULL) */
        return 0;

    while (*s1 == *s2++)
        if (*s1++ == '\0')
             return 0;

    return (*(unsigned char *)s1 - *(unsigned char *)--s2);
}
