/* $Id: intelhex.c,v 3.2 1995/11/17 18:49:13 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/rommon/utils/hxldsrc/intelhex.c,v $
 *------------------------------------------------------------------
 * intelhex.c
 *
 * June 1993, Rob Clevenger
 *
 * Copyright (c) 1994 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Intel Hex output format.
 *------------------------------------------------------------------
 * $Log: intelhex.c,v $
 * Revision 3.2  1995/11/17  18:49:13  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:10:26  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:42:59  hampton
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

intelhexout(size,ptr) /* output a 'record' in intel hex format */
    register char *ptr;
    register unsigned size;
{
    register unsigned i;
    int j;
    
    checksum = 0;
    if(lo_unit ^ hi_unit) size >>= 1;
    fprintf(outfile,"\r\n:"); /* marks beginning of intel hex record */
    hexout(size);
    hexout(loadaddr >> 8);
    hexout(loadaddr & 0xff);
    hexout(0); /* intel hex data type */
    for(i=0; i<size;) {
	if(hi_unit) {
	    for(j=0; j<unit && i<size; j++) {
		hexout(*ptr++);
		loadaddr++;
		i++;
	    }
	} else ptr += unit;
	if(lo_unit) {
	    for(j=0; j<unit && i<size; j++) {
		hexout(*ptr++);
		loadaddr++;
		i++;
	    }
	} else ptr += unit;
    }
    hexout(~checksum + 1); /* two's complement form */
}

hexout(byte)
     unsigned char byte;
{
  checksum += byte;
  nibhexout(byte >> 4);
  nibhexout(byte);
}

nibhexout(byte)
     unsigned char byte;
{
  byte &= 0x0f; /* only lower four bits valid */
  if(byte > 9) byte += ('A' - 10); /* make ascii hex */
  else byte += '0';
  fprintf(outfile,"%c",byte);
}
