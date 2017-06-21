/* $Id: wctomb.c,v 3.1.10.1 1996/03/18 20:49:14 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/lib/ansi/stdlib/wctomb.c,v $
 *------------------------------------------------------------------
 * wctomb.c -- minimal wide char to multibyte converter
 *
 * February 1996, Mark D. Baushke
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: wctomb.c,v $
 * Revision 3.1.10.1  1996/03/18  20:49:14  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.1.2.1  1996/03/07  08:19:36  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.1  1996/03/05  03:22:47  mdb
 * placeholder for DeadKingOnAThrone_branch
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/*
FUNCTION
	wctomb---minimal wide char to multibyte converter

ANSI_SYNOPSIS
	#include <stdlib.h>
	int wctomb(char *s, wchar_t wchar);

DESCRIPTION
	This is a minimal ANSI-conforming implementation of wctomb.
	The only ``wide characters'' recognized are single bytes, and
	they are ``converted'' to themselves.
	
	Each call to wctomb copies the character wchar to *s, unless s
	is a null pointer.

RETURNS
	This implementation of wctomb returns 0 if s is NULL; it
	returns 1 otherwise (reporting the length of the character
	``sequence'' generated).

PORTABILITY
	wctomb is required in the ANSI C standard.  However, the
	precise effects vary with the locale.

	wctomb requires no supporting OS subroutines.  
*/

#include <stdlib.h>

int
wctomb (char *s, wchar_t wchar)
{
    if (s == NULL)
	return 0;

    *s = (char) wchar;
    return 1;
}
