/* $Id: islower.c,v 3.1.10.1 1996/03/18 20:48:25 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/lib/ansi/ctype/islower.c,v $
 *------------------------------------------------------------------
 * islower.c -- determine if argument is a lowercase ASCII character
 *
 * December 1995, Mark D. Baushke
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: islower.c,v $
 * Revision 3.1.10.1  1996/03/18  20:48:25  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.1.2.1  1996/03/07  08:19:02  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.1  1996/03/05  02:40:18  mdb
 * placeholder for DeadKingOnAThrone_branch
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
FUNCTION
        islower---lowercase character predicate

ANSI_SYNOPSIS
        #include <ctype.h>
        int islower(int c);

DESCRIPTION
        islower is a macro which classifies ASCII integer values by
        table lookup.  It is a predicate returning non-zero for
        minuscules (lowercase alphabetic characters), and 0 for other
        characters.  It is defined only when isascii(c) is true or c
        is EOF.

        You can use a compiled subroutine instead of the macro
        definition by undefining the macro using `#undef islower'.

RETURNS
        islower returns non-zero if c is a lowercase letter (a--z).

PORTABILITY
        islower is ANSI C.

        No supporting OS subroutines are required.
*/

#include <ctype.h>
#undef islower

int
islower (int i)
{
    unsigned char c = i;

    return(isascii(c) && ((_ctype_ + 1)[c] & _L));
}
