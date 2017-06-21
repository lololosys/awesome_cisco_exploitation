/* $Id: strcoll.c,v 3.1.10.1 1996/03/18 20:49:29 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/lib/ansi/string/strcoll.c,v $
 *------------------------------------------------------------------
 * strcoll.c -- compares two strings using the current locale
 *
 * January 1996, Mark D. Baushke
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: strcoll.c,v $
 * Revision 3.1.10.1  1996/03/18  20:49:29  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.1.2.1  1996/03/07  08:19:46  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.1  1996/03/05  03:39:52  mdb
 * placeholder for DeadKingOnAThrone_branch
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
FUNCTION
        strcoll---locale specific character string compare

ANSI_SYNOPSIS
        #include <string.h>
        int strcoll(const char *stra, const char * strb);

DESCRIPTION
        strcoll compares the string pointed to by stra to the string
        pointed to by strb, using an interpretation appropriate to the
        current LC_COLLATE state.

RETURNS
        If the first string is greater than the second string, strcoll
        returns a number greater than zero.  If the two strings are
        equivalent, strcoll returns zero.  If the first string is less
        than the second string, strcoll returns a number less than
        zero.

PORTABILITY
        strcoll is ANSI C.

        strcoll requires no supporting OS subroutines.
*/

#include <string.h>

int
strcoll (const char *a, const char *b)
{
    return strcmp(a, b);
}
