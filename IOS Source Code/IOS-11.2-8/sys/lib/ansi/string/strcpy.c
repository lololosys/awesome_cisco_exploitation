/* $Id: strcpy.c,v 3.1.10.1 1996/03/18 20:49:30 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/lib/ansi/string/strcpy.c,v $
 *------------------------------------------------------------------
 * strcpy.c -- copies a string
 *
 * January 1996, Mark D. Baushke
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: strcpy.c,v $
 * Revision 3.1.10.1  1996/03/18  20:49:30  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.1.2.1  1996/03/07  08:19:47  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.1  1996/03/05  03:39:54  mdb
 * placeholder for DeadKingOnAThrone_branch
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
FUNCTION
        strcpy---copy string

ANSI_SYNOPSIS
        #include <string.h>
        char *strcpy(char *dst, const char *src);

DESCRIPTION
        strcpy copies the string pointed to by src (including the
        terminating null character) to the array pointed to by dst.

RETURNS
        This function returns the initial value of dst.

PORTABILITY
        strcpy is ANSI C.

        strcpy requires no supporting OS subroutines.
*/

#include <string.h>

/*
 * Differs from the router version (i.e., copytoASCIZ) in that this
 * version returns a pointer to the resulting string, whereas
 * copytoASCIZ is a void function.
 */

char *
strcpy (char *dst, const char *src)
{
    char *s = dst;

    while ((*dst++ = *src++))
	;

    return s;
}
