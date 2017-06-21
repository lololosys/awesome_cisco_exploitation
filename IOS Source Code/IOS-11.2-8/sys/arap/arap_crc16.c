/* $Id: arap_crc16.c,v 3.2 1995/11/17 08:38:35 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/arap/arap_crc16.c,v $
 *------------------------------------------------------------------
 * CRC routines
 *
 * October 1993, Tim Kolar
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: arap_crc16.c,v $
 * Revision 3.2  1995/11/17  08:38:35  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  10:53:33  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/06/18  19:54:30  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.1  1995/06/07  20:04:38  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "arap_crc16.h"	/* function prototype declarations */

#define	CRC_16_POLY	(0xa001)
#define	CRC_POLYNOMIAL	CRC_16_POLY
#define CRC_TBL_SIZE 256

ushort mnp4_crctab[CRC_TBL_SIZE];

void make_crctab (void)
{
    unsigned int modifier;
    unsigned char bitcount;
    int c;

    for (c = 0; c < CRC_TBL_SIZE; c++) {
	modifier = c;
	for (bitcount = 0; bitcount < 8; bitcount++)
	    modifier = modifier >> 1 ^ (modifier & 0x01 ? CRC_POLYNOMIAL : 0);
	mnp4_crctab[c] = modifier;
    }
}
