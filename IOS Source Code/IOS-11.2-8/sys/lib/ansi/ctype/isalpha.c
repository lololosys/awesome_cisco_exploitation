/* $Id: isalpha.c,v 3.1.10.1 1996/03/18 20:48:16 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/lib/ansi/ctype/isalpha.c,v $
 *------------------------------------------------------------------
 * isalpha.c -- argument is an alphabetic character
 *
 * December 1995, Mark D. Baushke
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: isalpha.c,v $
 * Revision 3.1.10.1  1996/03/18  20:48:16  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.1.2.1  1996/03/07  08:18:57  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.1  1996/03/05  02:40:13  mdb
 * placeholder for DeadKingOnAThrone_branch
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
FUNCTION
        isalpha---alphabetic character predicate

ANSI_SYNOPSIS
        #include <ctype.h>
        int isalpha(int c);

DESCRIPTION
        isalpha is a macro which classifies ASCII integer values by table
        lookup.  It is a predicate returning non-zero when c represents an
        alphabetic ASCII character, and 0 otherwise.  It is defined only when
        isascii(c) is true or c is EOF.

        You can use a compiled subroutine instead of the macro definition by
        undefining the macro using `#undef isalpha'.

RETURNS
        isalpha returns non-zero if c is a letter (A--Z or
        a--z).

PORTABILITY
        isalpha is ANSI C.

        No supporting OS subroutines are required.
*/

#include <ctype.h>
#undef isalpha

int
isalpha (int i)
{
    unsigned char c = i;

    return(isascii(c) && ((_ctype_ + 1)[c] & (_U|_L)));
}
