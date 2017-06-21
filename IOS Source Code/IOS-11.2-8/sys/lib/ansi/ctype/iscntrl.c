/* $Id: iscntrl.c,v 3.1.10.1 1996/03/18 20:48:22 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/lib/ansi/ctype/iscntrl.c,v $
 *------------------------------------------------------------------
 * iscntrl.c -- argument is a control character
 *
 * December 1995, Mark D. Baushke
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: iscntrl.c,v $
 * Revision 3.1.10.1  1996/03/18  20:48:22  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.1.2.1  1996/03/07  08:18:59  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.1  1996/03/05  02:40:15  mdb
 * placeholder for DeadKingOnAThrone_branch
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
FUNCTION
        iscntrl---control character predicate

ANSI_SYNOPSIS
        #include <ctype.h>
        int iscntrl(int c);

DESCRIPTION

        iscntrl is a macro which classifies ASCII integer values by
        table lookup.  It is a predicate returning non-zero for
        control characters, and 0 for other characters.  It is defined
        only when isascii(c) is true or c is EOF.

        You can use a compiled subroutine instead of the macro
        definition by undefining the macro using `#undef iscntrl'.

RETURNS
        iscntrl returns non-zero if c is a delete character or
        ordinary control character (0x7F or 0x00--0x1F).

PORTABILITY
        iscntrl is ANSI C.

        No supporting OS subroutines are required.
*/

#include <ctype.h>
#undef iscntrl

int
iscntrl (int i)
{
    unsigned char c = i;

    return(isascii(c) && ((_ctype_ + 1)[c] & _C));
}
