/* $Id: strcspn.c,v 3.1.10.1 1996/03/18 20:49:31 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/lib/ansi/string/strcspn.c,v $
 *------------------------------------------------------------------
 * strcspn.c -- searches a string for characters which are NOT in the
 *              second string
 *
 * January 1996, Mark D. Baushke
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: strcspn.c,v $
 * Revision 3.1.10.1  1996/03/18  20:49:31  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.1.2.1  1996/03/07  08:19:49  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.1  1996/03/05  03:39:57  mdb
 * placeholder for DeadKingOnAThrone_branch
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
FUNCTION
        strcspn---count chars not in string

ANSI_SYNOPSIS
        size_t strcspn(const char *s1, const char *s2);

DESCRIPTION
        This function computes the length of the initial part of the
        string pointed to by s1 which consists entirely of characters
        NOT from the string pointed to by s2 (excluding the
        terminating null character).

RETURNS
        strcspn returns the length of the substring found.

PORTABILITY
        strcspn is ANSI C.

        strcspn requires no supporting OS subroutines.
*/

#include <string.h>

/*
 * strcspn - find length of initial segment of s1 consisting entirely
 * of characters not from s2
 */

size_t
strcspn (char *s1, char *s2)
{
    char *scan2;
    int count;

    count = 0;
    for (; *s1 != '\0'; s1++) {
        for (scan2 = s2; *scan2 != '\0';)       /* ++ moved down. */
            if (*s1 == *scan2++)
                return(count);
        count++;
    }
    return(count);
}
