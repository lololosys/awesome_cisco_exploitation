/* $Id: isupper.c,v 3.1.10.1 1996/03/18 20:48:29 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/lib/ansi/ctype/isupper.c,v $
 *------------------------------------------------------------------
 * isupper.c -- determine if argument is an uppercase character
 *
 * December 1995, Mark D. Baushke
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: isupper.c,v $
 * Revision 3.1.10.1  1996/03/18  20:48:29  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.1.2.1  1996/03/07  08:19:07  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.1  1996/03/05  02:40:22  mdb
 * placeholder for DeadKingOnAThrone_branch
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
FUNCTION
        isupper---uppercase character predicate

ANSI_SYNOPSIS
        #include <ctype.h>
        int isupper(int c);

DESCRIPTION
        isupper is a macro which classifies ASCII integer values by
        table lookup.  It is a predicate returning non-zero for
        uppercase letters (A--Z), and 0 for other characters.  It is
        defined only when isascii(c) is true or c is EOF.

        You can use a compiled subroutine instead of the macro
        definition by undefining the macro using `#undef isupper'.

RETURNS
        isupper returns non-zero if c is a uppercase letter (A-Z).

PORTABILITY
        isupper is ANSI C.

        No supporting OS subroutines are required.
*/

#include <ctype.h>
#undef isupper

int
isupper (int i)
{
    unsigned char c = i;

    return(isascii(c) && ((_ctype_ + 1)[c] & _U));
}
