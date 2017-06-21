/* $Id: toupper.c,v 3.1.10.1 1996/03/18 20:48:33 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/lib/ansi/ctype/toupper.c,v $
 *------------------------------------------------------------------
 * toupper.c --  translate lowercase chars into uppercase
 *
 * December 1995, Mark D. Baushke
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: toupper.c,v $
 * Revision 3.1.10.1  1996/03/18  20:48:33  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.1.2.1  1996/03/07  08:19:12  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.1  1996/03/05  02:40:26  mdb
 * placeholder for DeadKingOnAThrone_branch
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
FUNCTION
        toupper---translate characters to uppercase

ANSI_SYNOPSIS
        #include <ctype.h>
        int toupper(int c);

DESCRIPTION
        toupper is a macro (when compiled by a GNU C compiler) which
        converts lowercase characters to uppercase, leaving all
        other characters unchanged.  It is only defined when c is an
        integer in the range EOF to 255.

        You can use a compiled subroutine instead of the macro
        definition by undefining this macro using `#undef toupper'.
        The macro version is not defined if the GNU C compiler is not
        in use.

RETURNS
        toupper returns the uppercase equivalent of c when it is a
        character between a and z, and c otherwise.

PORTABILITY
        toupper is ANSI C.

        No supporting OS subroutines are required.
*/

#include <ctype.h>
#undef toupper

int
toupper (int i)
{
    unsigned char c = i;
    int result;

    if (isascii(c) && islower(c))
	result = c - 'a' + 'A';
    else
	result = i;

    return(result);
}
