/* $Id: iso_chksum.c,v 3.2 1995/11/17 17:56:11 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/util/iso_chksum.c,v $
 *------------------------------------------------------------------
 * iso_chksum.c -- ISO Checksum routines
 *
 * March 1989, Kirk Lougheed
 *
 * Copyright (c) 1989-1994 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: iso_chksum.c,v $
 * Revision 3.2  1995/11/17  17:56:11  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:43:07  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:12:52  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * ISO Checksum routines.  The ISO checksum requirement is that for a 
 * checksum of array a[], for length L, the following holds.
 *
 *	1. sum(a[i]) mod 255 = 0
 *	2. sum((L-i+1) * a[i]) = 0
 *
 * Nakassis establish a upper limit, MODX, which is the maximum number
 * of sums that may be taken without overflowing 32-bit arithmetic.
 * This is used to reduce the number of times the MOD operation must
 * be taken. 
 *
 * In RFC-1008, McCoy gave C versions of Nakassis's algorithm.  The
 * following routines are based on that code, modified to change the
 * checksum position argument, k, from ones-origin offset to a pointer.
 */
#include "master.h"

#define MODX 4102		/* max number of one-byte sums that can
				   be taken without overflowing 32-bits */

/*
 * verify_iso_checksum
 * returns TRUE is checksum is correct.  A checksum of 0 means the
 * checksum was not supplied by the sender and the packet should be
 * accepted as is.
 */
boolean verify_iso_checksum(
     unsigned char mess[],	/* the TPDU to be checksummed */
     int      len,		/* length to be checksummed */
     unsigned char csp[])	/* checksum pointer */

{ 
    int c0, c1;
    unsigned char *p,*p1,*p2,*p3 ;

    if ((csp[0] == 0) && (csp[1] == 0))
        return(TRUE);

    p = mess;
    p3 = mess + len;

    c0 = 0;
    c1 = 0; 
    p1 = mess;
    while (p1 < p3) {
	/* outer sum accumulation loop */
	p2 = p1 + MODX;
	if (p2 > p3)
	    p2 = p3 ;
	for (p = p1; p < p2; p++) {
	    /* inner sum accumulation loop */
	    c0 = c0 + (*p);
	    c1 = c1 + c0;
	}
	/* adjust accumulated sums to mod 255 */
	c0 = c0%255;
	c1 = c1%255;
	p1 = p2;
    }

/*    printf("c0 = %d, c1 = %d\n", c0, c1); */

    return((c0==0) && (c1==0));
}



/*
 * compute_iso_checksum
 * Compute the ISO checksum of an octet string, and put it in the specified 
 * place in the string.
 */

void compute_iso_checksum(
     unsigned char mess[],      /* the octet string to be checksummed */
     int len,
     unsigned char csp[])	/* checksum pointer */

{ 
    int iq,
    ir,
    c0,
    c1;
    unsigned char *p,*p1,*p2,*p3;

    p = mess; 
    p3 = mess + len;

    /* insert zeros for checksum octets */
    csp[0] = 0x00;
    csp[1] = 0x00;

    c0 = 0;
    c1 = 0; 
    p1 = mess;
    while (p1 < p3) {
	/* outer sum accumulation loop */
	p2 = p1 + MODX; 
	if (p2 > p3)
	    p2 = p3;
	for (p = p1; p < p2; p++) {
	    /* inner sum accumulation loop */
	    c0 = c0 + (*p); 
	    c1 = c1 + c0;
	}
	/* adjust accumulated sums to mod 255 */
	c0 = c0%255;
	c1 = c1%255;
	p1 = p2;
    }

    /* compute and insert checksum octets */
    iq = ((len-((csp-mess)+1))*c0 - c1)%255;
    if (iq <= 0)
        iq = iq + 255;
    csp[0] = iq;

    ir = (510 - c0 - iq);
    if (ir > 255)
        ir = ir - 255;
    csp[1] = ir;

}
