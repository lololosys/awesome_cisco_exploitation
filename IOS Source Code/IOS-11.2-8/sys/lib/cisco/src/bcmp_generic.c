/* $Id: bcmp_generic.c,v 3.1.10.1 1996/03/18 20:50:50 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/lib/cisco/src/bcmp_generic.c,v $
 *------------------------------------------------------------------
 * bcmp_generic.c -- byte comparison routine
 *
 * December 1995, Mark D. Baushke
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: bcmp_generic.c,v $
 * Revision 3.1.10.1  1996/03/18  20:50:50  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.1.2.1  1996/03/07  08:20:23  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.1  1996/03/05  04:47:06  mdb
 * placeholder for DeadKingOnAThrone_branch
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include <ciscolib.h>

/*
 * bcmp returns 0 if comparison true, 1 otherwise
 */
int
bcmp (const void *s1, const void *s2, long bytes)
{
    unsigned char *c1 = (unsigned char *) s1;
    unsigned char *c2 = (unsigned char *) s2;

    if (c1 == c2)		/* same pointer? */
	return(0);

    while (bytes--) {
	if (*c1++ != *c2++) {
	    return(*(--c1) - *(--c2));
	}
    }
    return(0);
}
