/* $Id: isascii.c,v 3.1.10.1 1996/03/18 20:48:21 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/lib/ansi/ctype/isascii.c,v $
 *------------------------------------------------------------------
 * isascii.c - character is in the net ASCII range
 *
 * December 1995, Mark D. Baushke
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: isascii.c,v $
 * Revision 3.1.10.1  1996/03/18  20:48:21  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.1.2.1  1996/03/07  08:18:58  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.1  1996/03/05  02:40:14  mdb
 * placeholder for DeadKingOnAThrone_branch
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
FUNCTION
        isascii---ASCII character predicate

ANSI_SYNOPSIS
        #include <ctype.h>
        int isascii(int c);

DESCRIPTION
        isascii is a macro which returns non-zero when c is an ASCII
        character, and 0 otherwise.  It is defined for all integer values.

        You can use a compiled subroutine instead of the macro definition by
        undefining the macro using `#undef isascii'.

RETURNS
        isascii returns non-zero if the low order byte of c is in the range
        0 to 127 (0x00--0x7F).

PORTABILITY
        isascii is not ANSI C.

        No supporting OS subroutines are required.
*/

#include <ctype.h>
#undef isascii

int
isascii (int c)
{
    return((c >= 0) && (c < 128));
}
