/* $Id: isspace.c,v 3.1.10.1 1996/03/18 20:48:28 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/lib/ansi/ctype/isspace.c,v $
 *------------------------------------------------------------------
 * isspace.c -- determine if the argument is a white space character
 *
 * December 1995, Mark D. Baushke
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: isspace.c,v $
 * Revision 3.1.10.1  1996/03/18  20:48:28  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.1.2.1  1996/03/07  08:19:06  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.1  1996/03/05  02:40:21  mdb
 * placeholder for DeadKingOnAThrone_branch
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
FUNCTION
        isspace---whitespace character predicate

ANSI_SYNOPSIS
        #include <ctype.h>
        int isspace(int c);

DESCRIPTION
        isspace is a macro which classifies ASCII integer values by
        table lookup.  It is a predicate returning non-zero for
        whitespace characters, and 0 for other characters.  It is
        defined only when isascii(c) is true or c is EOF.

        You can use a compiled subroutine instead of the macro
        definition by undefining the macro using `#undef isspace'.

RETURNS
        isspace returns non-zero if c is a space, tab, carriage
        return, new line, vertical tab, or formfeed (0x09--0x0D,
        0x20).

PORTABILITY
        isspace is ANSI C.

        No supporting OS subroutines are required.
*/

#include <ctype.h>
#undef isspace

int
isspace (int i)
{
    unsigned char c = i;

    return(isascii(c) && ((_ctype_ + 1)[c] & _S));
}
