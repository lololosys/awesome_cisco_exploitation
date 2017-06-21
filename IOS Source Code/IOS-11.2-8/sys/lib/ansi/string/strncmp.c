/* $Id: strncmp.c,v 3.1.10.1 1996/03/18 20:49:36 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/lib/ansi/string/strncmp.c,v $
 *------------------------------------------------------------------
 * strncmp.c -- character string comparison routine
 *
 * January 1996, Mark D. Baushke
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: strncmp.c,v $
 * Revision 3.1.10.1  1996/03/18  20:49:36  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.1.2.1  1996/03/07  08:19:53  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.1  1996/03/05  03:40:06  mdb
 * placeholder for DeadKingOnAThrone_branch
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
FUNCTION
        strncmp---character string compare

ANSI_SYNOPSIS
        #include <string.h>
        int strncmp(const char *a, const char * b, size_t length);

DESCRIPTION
        strncmp compares up to length characters from the string at a
        to the string at b.

RETURNS
        If *a sorts lexicographically after *b, strncmp returns a
        number greater than zero.  If the two strings are equivalent,
        strncmp returns zero.  If *a sorts lexicographically before
        *b, strncmp returns a number less than zero.

PORTABILITY
        strncmp is ANSI C.

        strncmp requires no supporting OS subroutines.
*/

#include <string.h>

int
strncmp (const char *s1, const char *s2, size_t n)
{
    if ((!s1 && s2) || (s1 && !s2)) /* no match if only one ptr is NULL */
        return((int)(s1 - s2));
    if ((s1 == s2) || (n == 0)) /* match if ptrs same or length is 0 */
        return(0);

    while (n-- != 0 && *s1 == *s2) {
        if (n == 0 || *s1 == '\0' || *s2 == '\0')
            break;
        s1++;
        s2++;
    }

    return (*(unsigned char *) s1) - (*(unsigned char *) s2);
}

#if 0
/*
 * strncmp
 * case sensitive string match for n characters
 * returns an integer <,=,> 0 depending on whether s1 is <,=,> s2
 */

int strncmp (const char *s1, const char *s2, size_t n)
{
    if ((!s1 && s2) || (s1 && !s2)) /* no match if only one ptr is NULL */
        return((int)(s1 - s2));
    if ((s1 == s2) || (n == 0)) /* match if ptrs same or length is 0 */
        return(0);
    do {
        if (*s1 != *s2++)
            return (*s1 - *--s2);
        if (*s1++ == '\0')
            break;
    } while (--n != 0);
    return (0);
}
#endif
