/* $Id: strrchr.c,v 3.1.10.1 1996/03/18 20:49:41 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/lib/ansi/string/strrchr.c,v $
 *------------------------------------------------------------------
 * strrchr.c -- reverse search for character in string
 *
 * December 1995, Mark D. Baushke
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: strrchr.c,v $
 * Revision 3.1.10.1  1996/03/18  20:49:41  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.1.2.1  1996/03/07  08:19:57  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.1  1996/03/05  03:40:12  mdb
 * placeholder for DeadKingOnAThrone_branch
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
FUNCTION
        strrchr---reverse search for character in string

ANSI_SYNOPSIS
        #include <string.h>
        char * strrchr(const char *string, int c);

DESCRIPTION
        This function finds the last occurence of c (converted to a
        char) in the string pointed to by string (including the
        terminating null character).

RETURNS
        Returns a pointer to the located character, or a null pointer
        if c does not occur in string.

PORTABILITY
        strrchr is ANSI C.

        strrchr requires no supporting OS subroutines.
*/

#include <string.h>

char *
strrchr (const char *s, int i)
{
    const char *last;
    char c = i;

    for (last = NULL; ; ++s) {
        if (*s == c)
            last = s;
        if (*s == '\0')
            break;
    }

    return((char *) last);
}
