/* $Id: isalnum.c,v 3.1.10.1 1996/03/18 20:48:15 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/lib/ansi/ctype/isalnum.c,v $
 *------------------------------------------------------------------
 * isalnum.c -- determine if the argument is an alphanumeric character
 *
 * December 1995, Mark D. Baushke
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: isalnum.c,v $
 * Revision 3.1.10.1  1996/03/18  20:48:15  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.1.2.1  1996/03/07  08:18:55  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.1  1996/03/05  02:40:12  mdb
 * placeholder for DeadKingOnAThrone_branch
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
FUNCTION
        isalnum---alphanumeric character predicate

ANSI_SYNOPSIS
        #include <ctype.h>
        int isalnum(int c);

DESCRIPTION
        isalnum is a macro which classifies ASCII integer values by table
        lookup.  It is a predicate returning non-zero for alphabetic or
        numeric ASCII characters, and 0 for other arguments.  It is defined
        for all integer values.

        You can use a compiled subroutine instead of the macro definition by
        undefining the macro using `#undef isalnum'.

RETURNS
        isalnum returns non-zero if c is a letter (a--z or
        A--Z) or a digit (0--9).

PORTABILITY
        isalnum is ANSI C.

        No OS subroutines are required.
*/

#include <ctype.h>
#undef isalnum

int
isalnum (int i)
{
    unsigned char c = i;

    return(isascii(c) && ((_ctype_ + 1)[c] & (_U|_L|_N)));
}
