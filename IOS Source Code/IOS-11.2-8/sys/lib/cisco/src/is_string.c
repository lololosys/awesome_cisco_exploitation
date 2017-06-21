/* $Id: is_string.c,v 3.1.10.1 1996/03/18 20:51:10 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/lib/cisco/src/is_string.c,v $
 *------------------------------------------------------------------
 * is_string.c -- is this really an ASCII string?
 *
 * February 1996, Mark D. Baushke
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: is_string.c,v $
 * Revision 3.1.10.1  1996/03/18  20:51:10  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.1.2.1  1996/03/07  08:20:36  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.1  1996/03/05  04:47:22  mdb
 * placeholder for DeadKingOnAThrone_branch
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include <ciscolib.h>
#include <ctype.h>

/*
 * is_string
 * Is this an ascii string?
 * Return 0 if any byte in the supposed string is not an ascii character,
 * else, return 1.
 * The NULL string is not considered a string by this routine.
 */

int
is_string (const char *s)
{
    if (!s)
	return(0);

    while (*s) {
	if (!isascii(*s))	/* Not string if any non ascii character */
	    return(0);
	s++;
    }

    return(1);			/* All characters checked out as ascii */
}
