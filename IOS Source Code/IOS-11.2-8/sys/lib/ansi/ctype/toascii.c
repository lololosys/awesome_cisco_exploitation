/* $Id: toascii.c,v 3.1.10.1 1996/03/18 20:48:31 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/lib/ansi/ctype/toascii.c,v $
 *------------------------------------------------------------------
 * toascii.c -- force integers into ASCII range
 *
 * December 1995, Mark D. Baushke
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: toascii.c,v $
 * Revision 3.1.10.1  1996/03/18  20:48:31  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.1.2.1  1996/03/07  08:19:09  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.1  1996/03/05  02:40:24  mdb
 * placeholder for DeadKingOnAThrone_branch
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
FUNCTION
        toascii---force integers to ASCII range

ANSI_SYNOPSIS
        #include <ctype.h>
        int toascii(int c);

DESCRIPTION
        toascii is a macro which coerces integers to the ASCII range
        (0--127) by zeroing any higher-order bits.

        You can use a compiled subroutine instead of the macro
        definition by undefining this macro using `#undef toascii'.

RETURNS
        toascii returns integers between 0 and 127.

PORTABILITY
        toascii is not ANSI C.

        No supporting OS subroutines are required.
*/

#include <ctype.h>
#undef toascii

int
toascii (int c)
{
    return(c & 0x7F);
}
