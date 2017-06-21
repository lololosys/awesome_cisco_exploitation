/* $Id: arap_strdiacasecmp.h,v 3.2 1995/11/17 08:38:58 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/arap/arap_strdiacasecmp.h,v $
 *------------------------------------------------------------------
 * strdiacasecmp.h
 *
 * October 1993, Tim Kolar
 *
 * Copyright (c) 1993-1994 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: arap_strdiacasecmp.h,v $
 * Revision 3.2  1995/11/17  08:38:58  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  10:53:52  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:05:08  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

int strdiacasecmp(unsigned char *, unsigned char *);
int strndiacasecmp(unsigned char *, unsigned char *, int);
