/* $Id: _tolower.c,v 3.1.10.1 1996/03/18 20:48:13 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/lib/ansi/ctype/_tolower.c,v $
 *------------------------------------------------------------------
 * tolower.c -- translate uppercase chars into lowercase
 *
 * December 1995, Mark D. Baushke
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: _tolower.c,v $
 * Revision 3.1.10.1  1996/03/18  20:48:13  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.1.2.1  1996/03/07  08:18:53  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.1  1996/03/05  02:40:09  mdb
 * placeholder for DeadKingOnAThrone_branch
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
FUNCTION
        tolower---translate characters to lowercase

ANSI_SYNOPSIS
        #include <ctype.h>
        int _tolower(int c);

DESCRIPTION
        _tolower is a macro which converts uppercase characters to
        lowercase, but should only be used when c is known to be an
        uppercase character.

        You can use a compiled subroutine instead of the macro
        definition by undefining this macro using `#undef _tolower'.

RETURNS
        _tolower returns the lowercase equivalent of c when it is a
        character between A and Z. if c is not one of these
        characters, the behavior of _tolower is undefined.

PORTABILITY
        _tolower is not recommended for portable programs.

        No supporting OS subroutines are required.
*/

#include <ctype.h>
#undef _tolower

/*
 * The _tolower macro does NOT check isupper(c), but the function call
 * might as well. The behavior is undefined.
 */
int
_tolower (int i)
{
    unsigned char c = i;
    int result;

    if (isascii(c) && isupper(c))
	result = (c) - 'A' + 'a';
    else
	result = i;

    return(result);
}
