/* $Id: wcstombs.c,v 3.1.10.1 1996/03/18 20:49:13 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/lib/ansi/stdlib/wcstombs.c,v $
 *------------------------------------------------------------------
 * wcstombs.c -- converts a wide char string to a multibyte char string.
 *
 * Month Year, Your Name
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: wcstombs.c,v $
 * Revision 3.1.10.1  1996/03/18  20:49:13  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.1.2.1  1996/03/07  08:19:35  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.1  1996/03/05  03:22:46  mdb
 * placeholder for DeadKingOnAThrone_branch
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include <stdlib.h>

size_t
wcstombs (char *s, const wchar_t *pwcs, size_t n)
{
    int count = 0;

    if (n != 0) {
	do {
	    if ((*s++ = (char) *pwcs++) == 0)
		break;
	    count++;
	} while (--n != 0);
    }

    return count;
}
