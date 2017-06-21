/* $Id: atoo.c,v 3.1.10.1 1996/03/18 20:50:45 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/lib/cisco/src/atoo.c,v $
 *------------------------------------------------------------------
 * atoo.c -- ASCII to octal value.  Returns -1 if illegal number.
 *
 * February 1996, Mark D. Baushke
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: atoo.c,v $
 * Revision 3.1.10.1  1996/03/18  20:50:45  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.1.2.1  1996/03/07  08:20:19  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.1  1996/03/05  04:47:04  mdb
 * placeholder for DeadKingOnAThrone_branch
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include <ciscolib.h>


/*
 * atoo
 * ASCII to octal value.  Returns -1 if illegal number.
 */

long
atoo (const char *s)
{
    long i = 0;
    char c;
    const char *startptr;

    if (!s)
	return(-1);
    s = deblank(s);
    startptr = s;
    c = *s++;
    while ((c >= '0') && (c <= '7')) {
	i = ((i << 3) | (c - '0'));
	c = *s++;
    }
    if (s == startptr || (!term_char(c)))
	return(-1);
    return(i);
}
