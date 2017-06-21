/* $Id: bscopy_generic.c,v 3.1.10.1 1996/03/18 20:50:55 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/lib/cisco/src/bscopy_generic.c,v $
 *------------------------------------------------------------------
 * bscopy_generic.c -- byte swapped copy
 *
 * January 1996, Mark D. Baushke
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: bscopy_generic.c,v $
 * Revision 3.1.10.1  1996/03/18  20:50:55  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.1.2.1  1996/03/07  08:20:27  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.1  1996/03/05  04:47:13  mdb
 * placeholder for DeadKingOnAThrone_branch
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include <ciscolib.h>

/*
 * bscopy: byte swapped word copy. Same arguments as bcopy, but
 * assumes word transfers.
 */
void
bscopy (void const *src, void *dest, long bytes)
{
    const unsigned char *sptr;
    unsigned char *dptr;
    const unsigned short *ssptr;
    unsigned short *sdptr;
    unsigned short word;
    long i;

    bytes++;			/* round up byte count */
    bytes >>= 1;		/* convert bytes to words */

    if ((unsigned long)src & 0x1) {
	/* be more careful if the source address is odd */
	if ((unsigned long)dest & 0x1) {
	    /* both addresses are odd */
	    sptr = src;
	    dptr = dest;
	    for (i = 0; i < bytes; i++) {
		word = *sptr++;
		*dptr++ = *sptr++;
		*dptr++ = word;
	    }
	}
	else {
	    /* destination is aligned source is odd */
	    sptr = src;
	    sdptr = dest;
	    for (i = 0; i < bytes; i++) {
		word = *sptr++;
		word |= (*sptr++) << 8;
		*sdptr++ = word;
	    }
	}
    }
    else if ((unsigned long)dest & 0x1) {
	/* be more careful if the destination address is odd */
	ssptr = src;
	dptr = dest;
	for (i = 0; i < bytes; i++) {
	    word = *ssptr++;
	    *dptr++ = word & 0xff;
	    *dptr++ = word >> 8;
	}
    }
    else {
	/* good both addresses are word aligned */
	ssptr = src;
	sdptr = dest;
	for (i = 0; i < bytes; i++) {
	    word = *ssptr++;
	    word = (word << 8) | (word >> 8);
	    *sdptr++ = word;
	}
    }
}
