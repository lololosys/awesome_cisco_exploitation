/* $Id: num_bits.c,v 3.1.10.1 1996/03/18 20:51:19 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/lib/cisco/src/num_bits.c,v $
 *------------------------------------------------------------------
 * num_bits.c -- return the number of bits set in an int
 *
 * February 1996, Mark D. Baushke
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: num_bits.c,v $
 * Revision 3.1.10.1  1996/03/18  20:51:19  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.1.2.1  1996/03/07  08:20:40  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.1  1996/03/05  04:47:28  mdb
 * placeholder for DeadKingOnAThrone_branch
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include <ciscolib.h>

/*
 * num_bits
 * return the number of bits set in an int
 */
int
num_bits (unsigned long i)
{
    int retval = 0;

    while (i) {
	retval += i & 1;
	i >>= 1;
    }

    return(retval);
}
