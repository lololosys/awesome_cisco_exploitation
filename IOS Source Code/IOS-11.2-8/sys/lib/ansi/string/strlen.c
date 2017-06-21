/* $Id: strlen.c,v 3.1.10.1 1996/03/18 20:49:34 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/lib/ansi/string/strlen.c,v $
 *------------------------------------------------------------------
 * strlen.c -- return string length
 *
 * December 1995, Mark D. Baushke
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: strlen.c,v $
 * Revision 3.1.10.1  1996/03/18  20:49:34  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.1.2.1  1996/03/07  08:19:51  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.1  1996/03/05  03:40:02  mdb
 * placeholder for DeadKingOnAThrone_branch
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
FUNCTION
        strlen---character string length

ANSI_SYNOPSIS
        #include <string.h>
        size_t strlen(const char *str);

DESCRIPTION
        The strlen function works out the length of the string
        starting at *str by counting chararacters until it reaches a
        NULL character.

RETURNS
        strlen returns the character count.

PORTABILITY
        strlen is ANSI C.

        strlen requires no supporting OS subroutines.

*/

#include <string.h>

size_t
strlen (const char *str)
{
    const char *start = str;

    if (str == NULL)
        return(0);

    while (*str)
        str++;

    return(str - start);
}
