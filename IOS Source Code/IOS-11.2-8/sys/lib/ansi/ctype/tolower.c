/* $Id: tolower.c,v 3.1.10.1 1996/03/18 20:48:32 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/lib/ansi/ctype/tolower.c,v $
 *------------------------------------------------------------------
 * tolower.c -- translate uppercase chars into lowercase
 *
 * December 1995, Mark D. Baushke
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: tolower.c,v $
 * Revision 3.1.10.1  1996/03/18  20:48:32  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.1.2.1  1996/03/07  08:19:10  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.1  1996/03/05  02:40:25  mdb
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
        int tolower(int c);

DESCRIPTION
        tolower is a macro (when compiled by a GNU C compiler) which
        converts uppercase characters to lowercase, leaving all
        other characters unchanged.  It is only defined when c is an
        integer in the range EOF to 255.

        You can use a compiled subroutine instead of the macro
        definition by undefining this macro using `#undef tolower'.
        The macro version is not defined if the GNU C compiler is not
        in use.

RETURNS
        tolower returns the lowercase equivalent of c when it is a
        character between A and Z, and c otherwise.

PORTABILITY
        tolower is ANSI C.

        No supporting OS subroutines are required.  */

#include <ctype.h>
#undef tolower

int
tolower (int i)
{
    unsigned char c = i;
    int result;

    if (isascii(c) && isupper(c))
	result = (c) - 'A' + 'a';
    else
	result = i;

    return(result);
}
