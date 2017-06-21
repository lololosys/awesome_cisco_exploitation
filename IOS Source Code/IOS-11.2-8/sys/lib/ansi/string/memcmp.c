/* $Id: memcmp.c,v 3.1.10.1 1996/03/18 20:49:22 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/lib/ansi/string/memcmp.c,v $
 *------------------------------------------------------------------
 * memcmp.c -- memory comparison routine
 *
 * December 1995, Mark D. Baushke
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: memcmp.c,v $
 * Revision 3.1.10.1  1996/03/18  20:49:22  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.1.2.1  1996/03/07  08:19:40  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.1  1996/03/05  03:39:42  mdb
 * placeholder for DeadKingOnAThrone_branch
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
FUNCTION
        memcmp---compare two memory areas

SYNOPSIS
        #include <string.h>
        int memcmp(const void *s1, const void *s2, size_t n);

DESCRIPTION
        This function compares not more than n characters of the
        object pointed to by s1 with the object pointed to by s2.

RETURNS
        The function returns an integer greater than, equal to or less
        than zero according to whether the object pointed to by s1 is
        greater than, equal to or less than the object pointed to by
        s2.

PORTABILITY
        memcmp is ANSI C.

        memcmp requires no supporting OS subroutines.
*/

#include <string.h>

int
memcmp (const void *m1, const void *m2, size_t bytes)
{
    const unsigned char *s1 = m1;
    const unsigned char *s2 = m2;

    if (s1 == s2)
        return(0);

    while (bytes--) {
        if (*s1++ != *s2++) {
            return(*(--s1) - *(--s2));
        }
    }
    return(0);
}
