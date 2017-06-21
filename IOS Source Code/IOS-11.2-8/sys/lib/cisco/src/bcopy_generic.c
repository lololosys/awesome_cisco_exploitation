/* $Id: bcopy_generic.c,v 3.1.10.1 1996/03/18 20:50:51 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/lib/cisco/src/bcopy_generic.c,v $
 *------------------------------------------------------------------
 * bcopy_generic.c -- byte copy
 *
 * January 1996, Mark D. Baushke
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: bcopy_generic.c,v $
 * Revision 3.1.10.1  1996/03/18  20:50:51  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.1.2.1  1996/03/07  08:20:24  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.1  1996/03/05  04:47:07  mdb
 * placeholder for DeadKingOnAThrone_branch
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include <ciscolib.h>

/*
 * bcopy : direction sensitive byte copy
 */
void
bcopy (const void *src, void *dest, long bytes)
{
    unsigned char *sptr, *dptr;
    long i;

    if (src < dest) {

	/*
	 * Destination is higher. So copy backwards
	 */
	sptr = (unsigned char *)((unsigned long)src + bytes);
	dptr = (unsigned char *)((unsigned long)dest + bytes);
	for (i=0; i<bytes; i++) {
	    dptr--;
	    sptr--;
	    *dptr = *sptr;
	}
    } else {
	
	/* 
	 * Source is higher than destination. So copy forward
	 */
	sptr = (unsigned char *)src;
	dptr = (unsigned char *)dest;
	for (i=0; i<bytes; i++) {
	    *dptr++ = *sptr++;
	}
    }
}
