/* $Id: module_private.h,v 3.2 1995/11/17 18:49:46 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/os/module_private.h,v $
 *------------------------------------------------------------------
 * module_private.h - Definitions for loadable modules
 *
 * November 15, 1992, Kevin Paul Herbert
 *
 * Copyright (c) 1992-1994 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: module_private.h,v $
 * Revision 3.2  1995/11/17  18:49:46  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:47:03  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:57:06  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __MODULE_PRIVATE_H__
#define __MODULE_PRIVATE_H__

#include "module.h"

/*
 * Define in-memory representation of a loaded module
 */

typedef struct module_info_ {
    struct module_info_ *next;	/* Link to next in list */
    module_header header;	/* Saved module header */
    uchar *baseaddr[MODULE_SECTIONMAX]; /* Base addresses for each section */
} module_info;

/*
 * Define temporary I/O routines
 */

#define MODULE_MAX_FILESIZE (200000)

typedef struct module_stream_ {
    ulong filesize;		/* File size */
    ulong offset;		/* Current read offset */
    uchar buffer[MODULE_MAX_FILESIZE]; /* Data buffer */
} module_stream;

#endif __MODULE_PRIVATE_H__
