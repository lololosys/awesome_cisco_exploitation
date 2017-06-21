/* $Id: arap_crc16.h,v 3.2 1995/11/17 08:38:36 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/arap/arap_crc16.h,v $
 *------------------------------------------------------------------
 * CRC prototypes
 *
 * October 1993, Tim Kolar
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: arap_crc16.h,v $
 * Revision 3.2  1995/11/17  08:38:36  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  10:53:34  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:04:39  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef CRC16_H
#define CRC16_H
void make_crctab(void);

extern ushort mnp4_crctab[];

#define MNP4_CRC16(c,crc) \
	    (crc>>8 ^ mnp4_crctab[(crc^c) & 0xff])
#endif	/* CRC16_H */
