/* $Id: ispunct.c,v 3.1.10.1 1996/03/18 20:48:27 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/lib/ansi/ctype/ispunct.c,v $
 *------------------------------------------------------------------
 * ispunct.c -- determine if argument is a punctuation character
 *
 * December 1995, Mark D. Baushke
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ispunct.c,v $
 * Revision 3.1.10.1  1996/03/18  20:48:27  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.1.2.1  1996/03/07  08:19:05  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.1  1996/03/05  02:40:20  mdb
 * placeholder for DeadKingOnAThrone_branch
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
FUNCTION
        ispunct---punctuation character predicate

ANSI_SYNOPSIS
        #include <ctype.h>
        int ispunct(int c);

DESCRIPTION

        ispunct is a macro which classifies ASCII integer values by
        table lookup.  It is a predicate returning non-zero for
        printable punctuation characters, and 0 for other characters.
        It is defined only when isascii(c) is true or c is EOF.

        You can use a compiled subroutine instead of the macro
        definition by undefining the macro using `#undef ispunct'.

RETURNS
        ispunct returns non-zero if c is a printable punctuation
        character (isgraph(c) && !isalnum(c)).

PORTABILITY
        ispunct is ANSI C.

        No supporting OS subroutines are required.
*/

#include <ctype.h>
#undef ispunct

int
ispunct (int i)
{
    unsigned char c = i;

    return(isascii(c) && ((_ctype_ + 1)[c] & _P));
}
