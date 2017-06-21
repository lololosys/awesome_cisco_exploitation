/* $Id: module_externs.h,v 3.2 1995/11/17 18:49:43 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/os/module_externs.h,v $
 *------------------------------------------------------------------
 * module_extern.h - External symbols for module load facility
 *
 * January 11, 1993, Kevin Paul Herbert
 *
 * Copyright (c) 1993-1994 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: module_externs.h,v $
 * Revision 3.2  1995/11/17  18:49:43  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:47:02  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:57:04  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __MODULE_EXTERN_H__
#define __MODULE_EXTERN_H__

/*
 * Function prototypes
 */

extern boolean module_load_file(char *);

#endif __MODULE_EXTERN_H__
