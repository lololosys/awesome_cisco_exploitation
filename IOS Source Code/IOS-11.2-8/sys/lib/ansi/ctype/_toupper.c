/* $Id: _toupper.c,v 3.1.10.1 1996/03/18 20:48:14 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/lib/ansi/ctype/_toupper.c,v $
 *------------------------------------------------------------------
 * _toupper.c --  translate lowercase chars into uppercase
 *
 * December 1995, Mark D. Baushke
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: _toupper.c,v $
 * Revision 3.1.10.1  1996/03/18  20:48:14  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.1.2.1  1996/03/07  08:18:54  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.1  1996/03/05  02:40:11  mdb
 * placeholder for DeadKingOnAThrone_branch
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
FUNCTION
        _toupper---translate characters to uppercase

ANSI_SYNOPSIS
        #include <ctype.h>
        int _toupper(int c);

DESCRIPTION
        _toupper is a macro which converts lowercase characters to
        uppercase, but should only be used when c is known to be a
        lowercase character.

        You can use a compiled subroutine instead of the macro
        definition by undefining this macro using `#undef _toupper'.

	The _toupper macro is intended for use when c is already known
	to be a lowercase character. If the application does not know
	that for certain, then the toupper() macro/function should be
	used instead.

RETURNS
        _toupper returns the uppercase equivalent of c when it is a
        character between a and z. If c is not one of these
        characters, the behavior of _toupper is undefined.

PORTABILITY
        _toupper is not recommended for portable programs.

        No supporting OS subroutines are required.
*/

#include <ctype.h>
#undef _toupper

/*
 * The _toupper macro does NOT check islower(c), but the function call
 * might as well. The behavior is undefined.
 */
int
_toupper (int i)
{
    unsigned char c = i;
    int result;

    if (isascii(c) && islower(c))
	result = c - 'a' + 'A';
    else
	result = i;

    return(result);
}
