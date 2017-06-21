/* $Id: srecord.c,v 3.2 1995/11/17 07:41:42 hampton Exp $
 * $Source: /swtal/cherf.111/ios/boot/srecord.c,v $
 *------------------------------------------------------------------
 * srecord.c -- load S-Records into memory
 *
 * August 1986, Greg Satz
 *
 * Copyright (c) 1986-1992 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: srecord.c,v $
 * Revision 3.2  1995/11/17  07:41:42  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  09:04:49  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  19:12:37  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include <sys/types.h>
#include "reason.h"
#include "extern.h"
#include "defs.h"

#define SREC_DATA	'2'	/* Data-carrying S-record */
#define	SREC_TRAILER	'8'	/* Trailer S-record */

/*
 * get an S-record.  Returns character used for handshaking.
 * pcadx is set to address part of S-record for non-data records.
 */
char srecord(pcadx, s)
	long *pcadx;
	register char *s;
{
	register long bytecount;
	register short x;
	register long adx;
	char rtype;
	short w;
	short checksum;
	char *savptr;
	short echostate = echoing;
	
	echoing = FALSE;

	puthexnum(sreccnt);

	/*
	 * Format of 'S' record:
	 *	S<type><count><address><datum>...<datum><checksum>
	 *	  digit  byte   3bytes  byte      byte    byte
 	 */

	rtype = *s++;
	if (rtype != SREC_DATA && rtype != SREC_TRAILER) {
    	    echoing = echostate;
	    return('T');
	}

	bytecount = gethexbyte(s);
	s += 2;
	/*
	 * checksum = 0; checksum += bytecount
	 */
	checksum = bytecount;	

	/* check to see that the bytecount agrees with line length */
	if (bytecount < 3 || linelength != (bytecount<<1)+4) {
		echoing = echostate;
		return('L');
	}

	/* accumulate address - 3 bytes */
	adx = 0;
	for (w = 0; w++ < 3; ) {
		adx = (adx<<8)|(x = gethexbyte(s));
		s += 2;
		checksum += x;
		bytecount--;
	}

	/* verify checksum */
	savptr = s;	/* save line ptr for rescanning */
	for (x = bytecount; x > 0 ; x--) {
		checksum += gethexbyte(s);
		s += 2;
	}

	if (++checksum & 0xFF) {
		echoing = echostate;
		return('K');
        }

	/* checksum worked - now rescan input and read data */
	if (rtype == SREC_DATA) {
		s = savptr;
		while (--bytecount) {	/* predecrement to ignore checksum */
			*(char *)adx++ = (char)gethexbyte(s);
			s += 2;
		}
	} else
		*pcadx = adx;

	sreccnt++;

	if ((sreccnt&0x3) == 0)
	    putchar('.');  /* let user know that good records are coming */

    	echoing = echostate;
	return('Y');
}
