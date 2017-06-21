/* $Id: sum.c,v 3.2 1995/11/17 07:41:56 hampton Exp $
 * $Source: /swtal/cherf.111/ios/boot/sum.c,v $
 *------------------------------------------------------------------
 * sum.c -- perform checksum over ROMS
 *
 * February 1988, Greg Satz
 *
 * Copyright (c) 1988-1993 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: sum.c,v $
 * Revision 3.2  1995/11/17  07:41:56  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  09:05:00  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  19:12:51  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "defs.h"
#include "prom2.h"
#include "reason.h"
#include "extern.h"

/*
 * Perform sumation of bytes mod 2^16
 */

void sum()
{
    register u_int cksum;
    register u_char *p;
    register int i, j, c;
    register struct cdata *cdata;
    register struct cload *cload;

    cksum = 0;
    cdata = (struct cdata *)prom2;
    for (i = 0; i < cdata->number; i++) {
	cload = (struct cload *)((u_char *)cdata + sizeof(struct cdata) +
	    (i * sizeof(struct cload)));
	p = (u_char *)cload->addr;
	for (j = 0; j < cload->len; j++) {
	    c = *p++;
	    if (cksum & 1)
		cksum = (cksum >> 1) + 0x8000;
	    else
		cksum >>= 1;
	    cksum += c;
	    cksum &= 0xFFFF;
	}
    }
    if (cksum != cdata->cksum)
	printf("Checksum mismatch: calculated %#x, stored %#x\n",
	    cksum, cdata->cksum);
}
