/* $Id: bzero.c,v 3.1.10.1 1996/03/18 20:50:57 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/lib/cisco/src/bzero.c,v $
 *------------------------------------------------------------------
 * bzero.c - zero a block of memory
 *
 * December 1995, Mark D. Baushke
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: bzero.c,v $
 * Revision 3.1.10.1  1996/03/18  20:50:57  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.1.2.1  1996/03/07  08:20:28  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.1  1996/03/05  04:47:15  mdb
 * placeholder for DeadKingOnAThrone_branch
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include <ciscolib.h>

/*
 * The bzero(ptr, count) function should be replaced by
 * calls to memset(ptr, 0, count). 
 *
 * Note: Some versions of the gcc compiler toolchain use bzero() to
 * initialize automatic arrays and structs to zero.
 */

/*
 * bzero
 * Zero some memory, takes address and byte count
 */
void
bzero (void *dummy, long count)
{
    unsigned long *lp;
    long lcount;
    unsigned char *address = dummy;

    /* First, do a few bytes so that we're word aligned. */
    for (; count && ((long)address & 0x3); count--)
	*address++ = 0;

    /* Now do the longs. */
    lp = (unsigned long *) address;
    lcount = count >> 2;
    for (; lcount > 16; lcount -= 16) {
	*lp++ = 0;
	*lp++ = 0;
	*lp++ = 0;
	*lp++ = 0;
	*lp++ = 0;
	*lp++ = 0;
	*lp++ = 0;
	*lp++ = 0;
	*lp++ = 0;
	*lp++ = 0;
	*lp++ = 0;
	*lp++ = 0;
	*lp++ = 0;
	*lp++ = 0;
	*lp++ = 0;
	*lp++ = 0;
    }
    while (lcount) {
	*lp++ = 0;
	lcount--;
    }
    address = (unsigned char *) lp;
    count &= 0x3;

    /* Now do what's left. */
    for (; count; address++, count--)
	*address = 0;
}
