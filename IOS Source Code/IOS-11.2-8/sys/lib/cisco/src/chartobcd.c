/* $Id: chartobcd.c,v 3.1.10.1 1996/03/18 20:50:58 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/lib/cisco/src/chartobcd.c,v $
 *------------------------------------------------------------------
 * chartobcdc -- convert char string to BCD
 *
 * March 1996, Mark D. Baushke
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: chartobcd.c,v $
 * Revision 3.1.10.1  1996/03/18  20:50:58  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.1.2.1  1996/03/07  08:20:29  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.1  1996/03/06  11:22:30  mdb
 * placeholder for DeadKingOnAThrone_branch
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include <ciscolib.h>

/*
 * chartobcd
 * Convert char string to BCD
 */

void
chartobcd (unsigned char *from, unsigned char *to, long len)
{
    long octets;

    octets = len / 2;
    while (octets > 0) {
	*to++ = ((from[0] - '0') << 4) | (from[1] - '0');
	from += 2;
	octets--;
    }
    if (len & 0x1)
	*to = (*from - '0') << 4;
}
