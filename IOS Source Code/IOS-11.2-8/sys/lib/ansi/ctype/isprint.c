/* $Id: isprint.c,v 3.1.10.1 1996/03/18 20:48:26 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/lib/ansi/ctype/isprint.c,v $
 *------------------------------------------------------------------
 * isprint.c -- determine if argument is a printable character
 *
 * December 1995, Mark D. Baushke
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: isprint.c,v $
 * Revision 3.1.10.1  1996/03/18  20:48:26  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.1.2.1  1996/03/07  08:19:03  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.1  1996/03/05  02:40:19  mdb
 * placeholder for DeadKingOnAThrone_branch
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
FUNCTION
        isprint---printable character predicates

ANSI_SYNOPSIS
        #include <ctype.h>
        int isprint(int c);

DESCRIPTION
        isprint is a macro which classifies ASCII integer values by
        table lookup.  It is a predicate returning non-zero for
        printable characters, and 0 for other character arguments.  It
        is defined only when isascii(c) is true or c is EOF.

        You can use a compiled subroutine instead of the macro
        definition by undefining either macro using `#undef isprint'
        or `#undef isgraph'.

RETURNS
        isprint returns non-zero if c is a printing character,
        (0x20--0x7E).

PORTABILITY
        isprint is ANSI C.

        No supporting OS subroutines are required.
*/

#include <ctype.h>
#undef isprint

int
isprint (int i)
{
    unsigned char c = i;

    return(isascii(c) && ((_ctype_ + 1)[c] & (_P|_U|_L|_N|_B)));
}
