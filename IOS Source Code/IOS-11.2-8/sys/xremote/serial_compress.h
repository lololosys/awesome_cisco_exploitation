/* $Id: serial_compress.h,v 3.2 1995/11/17 19:26:41 gstovall Exp $
 * $Source: /swtal/cherf.111/ios/sys/xremote/serial_compress.h,v $
 *------------------------------------------------------------------
 * serial_compress.h
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
 * $Log: serial_compress.h,v $
 * Revision 3.2  1995/11/17  19:26:41  gstovall
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  14:01:38  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:32:00  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

typedef struct serialctx_ {
    compress_private compress_state;
    compress_private decompress_state;
} serialctx;

char *InitCompression(void);
void FreeCompression(serialctx *);
void ResetCompressionOutput(serialctx *);
void RestartCompression(serialctx *);
int SerialCompress(serialctx *, uchar *, int, uchar *, int);
int SerialDecompress(serialctx *, uchar *, int, uchar *, int);
