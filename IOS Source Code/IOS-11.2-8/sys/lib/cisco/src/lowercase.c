/* $Id: lowercase.c,v 3.1.10.1 1996/03/18 20:51:15 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/lib/cisco/src/lowercase.c,v $
 *------------------------------------------------------------------
 * lowercase.c -- convert a string to lowercase
 *
 * February 1996, Mark D. Baushke
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: lowercase.c,v $
 * Revision 3.1.10.1  1996/03/18  20:51:15  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.1.2.1  1996/03/07  08:20:38  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.1  1996/03/05  04:47:25  mdb
 * placeholder for DeadKingOnAThrone_branch
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include <ciscolib.h>
#include <ctype.h>

void
lowercase (char *s)
{
    unsigned char *str = (unsigned char *) s;

    if (!str)
	return;

    while(*str) {
	if (isupper(*str))
	    *str = _tolower(*str);
	str++;
    }
}
