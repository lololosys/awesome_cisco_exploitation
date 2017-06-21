/* $Id: image_aout.h,v 3.2 1995/11/17 17:50:17 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/machine/image_aout.h,v $
 *------------------------------------------------------------------
 * image_out.h - Definitions for a.out format files
 *
 * December 1994, Scott Mackie
 *
 * Copyright (c) 1994-1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: image_aout.h,v $
 * Revision 3.2  1995/11/17  17:50:17  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:40:26  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:43:41  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef ASMINCLUDE
extern boolean _start(int, boolean);
extern uchar etext[];
extern uchar edata[];
extern uchar end[];
extern int data_start;
#endif

#define TEXT_START     &_start
#define TEXT_END       &etext[0]
#define TEXTBASE       TEXT_START

#define DATA_START     &data_start
#define DATABASE       DATA_START
#define DATA_END       &edata[0]
#define BSSBASE        DATA_END

#define _END           &end[0]

