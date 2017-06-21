/* $Id: firstbit.c,v 3.1.10.1 1996/03/18 20:51:06 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/lib/cisco/src/firstbit.c,v $
 *------------------------------------------------------------------
 * firstbit.c -- Return the bit number of the first bit set from left to right
 *
 * February 1996, Mark D. Baushke
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: firstbit.c,v $
 * Revision 3.1.10.1  1996/03/18  20:51:06  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.1.2.1  1996/03/07  08:20:34  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.1  1996/03/05  04:47:20  mdb
 * placeholder for DeadKingOnAThrone_branch
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include <ciscolib.h>

/*
 * firstbit
 * Return the bit number of the first bit set from left to right
 */

int
firstbit (unsigned long word)
{
    int i;

    for (i = 31; i >= 0; i--)
	if ((1 << i) & word)
	    return(i);
    return(-1);
}
