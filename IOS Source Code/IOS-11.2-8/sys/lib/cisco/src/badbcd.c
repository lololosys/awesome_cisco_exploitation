/* $Id: badbcd.c,v 3.1.10.1 1996/03/18 20:50:47 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/lib/cisco/src/badbcd.c,v $
 *------------------------------------------------------------------
 * badbcd.c -- return non-zero if BCD string has a bad entry
 *
 * March 1996, Mark D. Baushke
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: badbcd.c,v $
 * Revision 3.1.10.1  1996/03/18  20:50:47  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.1.2.1  1996/03/07  08:20:21  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.1  1996/03/06  11:22:28  mdb
 * placeholder for DeadKingOnAThrone_branch
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include <ciscolib.h>

/*
 * badbcd
 * Return true if BCD string has a bad entry
 */

int
badbcd (unsigned char *s, long len)
{
    long octets;

    octets = len / 2;
    while (octets > 0) {
	if (((*s >> 4) & 0xf) > 9 || (*s & 0xf) > 9)
	    return(1);
	s++;
	octets--;
    }
    if (len & 0x1 && ((*s >> 4) & 0xf) > 9)
	return(1);
    return(0);
}
