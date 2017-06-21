/* $Id: uppercase.c,v 3.1.10.1 1996/03/18 20:51:37 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/lib/cisco/src/uppercase.c,v $
 *------------------------------------------------------------------
 * uppercase.c -- convert a string to uppercase
 *
 * February 1996, Mark D. Baushke
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: uppercase.c,v $
 * Revision 3.1.10.1  1996/03/18  20:51:37  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.1.2.1  1996/03/07  08:20:51  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.1  1996/03/05  04:47:40  mdb
 * placeholder for DeadKingOnAThrone_branch
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include <ciscolib.h>
#include <ctype.h>

void
uppercase (char *str)
{
    if (!str)
	return;
    while(*str) {
	if (islower(*str))
	    *str = _toupper(*str);
	str++;
    }
}
