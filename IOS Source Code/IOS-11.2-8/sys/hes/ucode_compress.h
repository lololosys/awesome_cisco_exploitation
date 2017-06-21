/* $Id: ucode_compress.h,v 3.2 1995/11/17 09:21:34 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/hes/ucode_compress.h,v $
 *------------------------------------------------------------------
 * config_compress.h
 *
 * September 1993, Stig Thormodsrud
 *
 * Copyright (c) 1993-1994 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Portions of this module are based on public domain software and/or
 * software copyright (c) 1990 by Network Computing Devices, Inc., and
 * used with permission.
 *------------------------------------------------------------------
 * $Log: ucode_compress.h,v $
 * Revision 3.2  1995/11/17  09:21:34  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:43:38  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:45:59  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

long ucode_decompress (const uchar *, long, uchar *, long);
