/* $Id: bcdtochar.c,v 3.1.10.1 1996/03/18 20:50:48 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/lib/cisco/src/bcdtochar.c,v $
 *------------------------------------------------------------------
 * bcdtochar.c -- convert BCD string to char
 *
 * March 1996, Mark D. Baushke
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: bcdtochar.c,v $
 * Revision 3.1.10.1  1996/03/18  20:50:48  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.1.2.1  1996/03/07  08:20:22  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.1  1996/03/06  11:22:29  mdb
 * placeholder for DeadKingOnAThrone_branch
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include <ciscolib.h>

/*
 * bcdtochar
 * Convert BCD string to char
 */

void
bcdtochar (unsigned char *from, unsigned char *to, long len)
{
    long octets;

    octets = len / 2;
    while (octets > 0) {
	*to++ = ((*from >> 4) & 0xf) + '0';
	*to++ = (*from++ & 0xf) + '0';
	octets--;
    }
    if (len & 0x1)
	*to = ((*from >> 4) & 0xf) + '0';
}
