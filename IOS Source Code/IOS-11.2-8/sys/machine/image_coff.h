/* $Id: image_coff.h,v 3.2 1995/11/17 17:50:19 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/machine/image_coff.h,v $
 *------------------------------------------------------------------
 * image_coff.h - Definitions for COFF format files
 *
 * December 1994, Scott Mackie
 *
 * Copyright (c) 1994 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: image_coff.h,v $
 * Revision 3.2  1995/11/17  17:50:19  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:40:27  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:43:44  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#ifndef ASMINCLUDE
extern char  __start[];
extern char   _fdata[];
extern uchar   etext[];
extern uchar   edata[];
extern uchar   end[];
extern int data_start;
#endif

#define TEXT_START &__start
#define TEXT_END &etext[0]

#define DATA_START     &data_start
#define DATA_END       &edata[0]

#define _END &end[0]
