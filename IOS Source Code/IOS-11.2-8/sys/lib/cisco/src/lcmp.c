/* $Id: lcmp.c,v 3.1.10.1 1996/03/18 20:51:12 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/lib/cisco/src/lcmp.c,v $
 *------------------------------------------------------------------
 * lcmp.c -- long compare routine
 *
 * December 1995, Mark D. Baushke
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: lcmp.c,v $
 * Revision 3.1.10.1  1996/03/18  20:51:12  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.1.2.1  1996/03/07  08:20:37  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.1  1996/03/05  04:47:24  mdb
 * placeholder for DeadKingOnAThrone_branch
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include <ciscolib.h>

/*
 * lcmp
 * Compare a set of longs.  Returns 0 on equal, 1 on unequal.
 */

int
lcmp (unsigned long *p1, unsigned long *p2, long longs)
{
    while (longs--) {
	if (*p1++ != *p2++)
	    return(1);
    }

    return(0);
}
