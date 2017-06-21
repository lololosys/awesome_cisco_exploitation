/* $Id: memchr.c,v 3.1.10.1 1996/03/18 20:49:21 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/lib/ansi/string/memchr.c,v $
 *------------------------------------------------------------------
 * memchr.c -- scan memory for a byte
 *
 * December 19995, Mark D. Baushke
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: memchr.c,v $
 * Revision 3.1.10.1  1996/03/18  20:49:21  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.1.2.1  1996/03/07  08:19:39  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.1  1996/03/05  03:39:40  mdb
 * placeholder for DeadKingOnAThrone_branch
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
FUNCTION
        memchr---find character in memory

ANSI_SYNOPSIS
        #include <string.h>
        void *memchr(const void *src, int c, size_t length);

DESCRIPTION
        This function searches memory starting at *src for the
        character c.  The search only ends with the first occurrence
        of c, or after length characters; in particular, NULL does not
        terminate the search.

RETURNS
        If the character c is found within length characters of *src,
        a pointer to the character is returned. If c is not found,
        then NULL is returned.

PORTABILITY
        memchr is ANSI C.

        memchr requires no supporting OS subroutines.
*/

#include <string.h>

void *
memchr (const void *src_void, int c_int, size_t length)
{
    const unsigned char *src = src_void;
    unsigned char c = c_int & 0xff;

    if (src != NULL) {
        while (length--) {
            if (*src == c)
                return((void *)src);
            src++;
        }
    }
    return(NULL);
}

