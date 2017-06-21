/* $Id: motsrec.c,v 3.2 1995/11/17 18:49:21 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/rommon/utils/hxldsrc/motsrec.c,v $
 *------------------------------------------------------------------
 * motsrec.c
 *
 * June 1993, Rob Clevenger
 *
 * Copyright (c) 1994 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Output in Moto Srecord format.
 *------------------------------------------------------------------
 * $Log: motsrec.c,v $
 * Revision 3.2  1995/11/17  18:49:21  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:10:30  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:43:04  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#include "stdio.h"

extern unsigned checksum;
extern unsigned long loadaddr;
extern char lo_unit, hi_unit, unit;
extern FILE *outfile;
char firstrecord = 1;

motsrecout(size,ptr) /* output a 'record' in Motorola S record format */
    register unsigned char *ptr;
    register int size;
{
    register int i, j;
    register unsigned char *cptr;

    if(size == 0) {
	fprintf(outfile,"S9030000FC\n");                /* terminator */
	return;
    }
    if(firstrecord) {
	firstrecord = 0;                                /* clear flag */
	fprintf(outfile,"\nS0090000646C2E72756EA3\n");  /* header record */
    }
    checksum = 0;
    
    if(lo_unit ^ hi_unit) size >>= 1;
    
    checksum += size + sizeof(loadaddr) + 1;
    for(i=0, cptr = (unsigned char *)&loadaddr; i < sizeof(loadaddr); i++)
	checksum += *cptr++;      /* add up bytes within the loadaddr */
    /* send Motorola preamble */
    fprintf(outfile, "S3%.2X%.8X", size + sizeof(loadaddr) + 1, loadaddr);
    for(i=0; i<size;) {
	if(hi_unit) {
	    for(j=0; j<unit && i<size; j++) {
		checksum += *ptr;
		fprintf(outfile, "%.2X", *ptr);
		loadaddr++;
		i++;
		ptr++;
	    }
	} else ptr += unit;
	if(lo_unit) {
	    for(j=0; j<unit && i<size; j++) {
		checksum += *ptr;
		fprintf(outfile, "%.2X", *ptr);
		loadaddr++;
		i++;
		ptr++;
	    }
	} else ptr += unit;
    }
    fprintf(outfile, "%.2X\n", ~checksum & 0xff);
}
