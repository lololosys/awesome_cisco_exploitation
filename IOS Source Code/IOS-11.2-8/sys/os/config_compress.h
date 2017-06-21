/* $Id: config_compress.h,v 3.2 1995/11/17 18:46:10 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/os/config_compress.h,v $
 *------------------------------------------------------------------
 * config_compress.h
 *
 * September 1993, Stig Thormodsrud
 *
 * Copyright (c) 1993-1994 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: config_compress.h,v $
 * Revision 3.2  1995/11/17  18:46:10  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:44:53  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:54:20  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define NVCOMPRESS_MULTIPLIER 3

extern boolean compress_config_enable;
extern long configbufsize;

long config_compress (uchar *, long , uchar *, long);
long config_decompress (uchar *, long, uchar *, long);

