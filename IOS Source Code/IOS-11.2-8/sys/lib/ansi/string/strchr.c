/* $Id: strchr.c,v 3.1.10.1 1996/03/18 20:49:27 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/lib/ansi/string/strchr.c,v $
 *------------------------------------------------------------------
 * strchr.c -- search for a character in a string
 *
 * January 1996, Mark D. Baushke
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: strchr.c,v $
 * Revision 3.1.10.1  1996/03/18  20:49:27  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.1.2.1  1996/03/07  08:19:44  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.1  1996/03/05  03:39:48  mdb
 * placeholder for DeadKingOnAThrone_branch
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
FUNCTION
        strchr -- search for character in string

ANSI_SYNOPSIS
        #include <string.h>
        char * strchr(const char *string, int c);

DESCRIPTION
        This function finds the first occurence of c (converted to
        a char) in the string pointed to by string (including the
        terminating null character).

RETURNS
        Returns a pointer to the located character, or a null pointer
        if c does not occur in string.

PORTABILITY
        strchr is ANSI C.

        strchr requires no supporting OS subroutines.
*/

#include <string.h>

char *
strchr (const char *s, int i)
{
    char c = i;

    if (s == NULL)
        return(NULL);
    do {
        if (*s == c)
            return((char *) s);
    } while (*s++);

    return(NULL);
}
