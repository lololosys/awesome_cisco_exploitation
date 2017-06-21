/* $Id: firstrbit.c,v 3.1.10.1 1996/03/18 20:51:08 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/lib/cisco/src/firstrbit.c,v $
 *------------------------------------------------------------------
 * firstrbit.c -- Return the bit number of the first bit set from right to left
 *
 * February 1996, Mark D. Baushke
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: firstrbit.c,v $
 * Revision 3.1.10.1  1996/03/18  20:51:08  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.1.2.1  1996/03/07  08:20:35  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.1  1996/03/06  11:22:31  mdb
 * placeholder for DeadKingOnAThrone_branch
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include <ciscolib.h>

/*
 * firstrbit
 * Return the bit number of the first bit set from right to left
 */

int
firstrbit (unsigned long word)
{
    int i;

    for (i = 0; i < 32; i++)
	if ((1 << i) & word)
	    return(i);
    return(-1);
}
