/* $Id: concat.c,v 3.1.10.1 1996/03/18 20:51:03 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/lib/cisco/src/concat.c,v $
 *------------------------------------------------------------------
 * concat.c -- concatenate two strings to create a thrid string
 *
 * February 1996, Mark D. Baushke
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: concat.c,v $
 * Revision 3.1.10.1  1996/03/18  20:51:03  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.1.2.1  1996/03/07  08:20:32  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.1  1996/03/05  04:47:17  mdb
 * placeholder for DeadKingOnAThrone_branch
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include <ciscolib.h>

void
concat (const char *s1, const char *s2, char *s3)
{
    char c;

    while ((c = *s1++)) {
	*s3++ = c;			/* start with first string */
    }
    while ((*s3++ = *s2++));		/* concat second with NULL */
}
