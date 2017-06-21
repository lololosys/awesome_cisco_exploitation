/* $Id: isgraph.c,v 3.1.10.1 1996/03/18 20:48:24 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/lib/ansi/ctype/isgraph.c,v $
 *------------------------------------------------------------------
 * isgraph.c -- determine if argument is a printable sans space char
 *
 * December 1995, Mark D. Baushke
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: isgraph.c,v $
 * Revision 3.1.10.1  1996/03/18  20:48:24  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.1.2.1  1996/03/07  08:19:01  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.1  1996/03/05  02:40:17  mdb
 * placeholder for DeadKingOnAThrone_branch
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
FUNCTION
        isgraph---printable character predicates

ANSI_SYNOPSIS
        #include <ctype.h>
        int isgraph(int c);

DESCRIPTION
        isgraph is a macro which classifies ASCII integer values by
        table lookup.  It is a predicate returning non-zero for
        printable characters excluding the space character (0x20), and
        0 for other character arguments.  It is defined only when
        isascii(c) is true or c is EOF.

        You can use a compiled subroutine instead of the macro
        definition by undefining either macro using `#undef isgraph'.

RETURNS
        isgraph behaves identically to isprint, except that the space
        character (0x20) is excluded.

PORTABILITY
        isgraph is ANSI C.

        No supporting OS subroutines are required.
*/

#include <ctype.h>
#undef isgraph

int
isgraph (int i)
{
    unsigned char c = i;

    return(isascii(c) && ((_ctype_ + 1)[c] & (_P|_U|_L|_N)));
}
