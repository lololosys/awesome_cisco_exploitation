/* $Id: filesys_registry.h,v 3.2 1995/11/17 09:07:30 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/filesys/filesys_registry.h,v $
 *------------------------------------------------------------------
 * filesys_registry.h -- Registration Points for file system support.
 *
 *
 * July 1995, Yan Ke
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: filesys_registry.h,v $
 * Revision 3.2  1995/11/17  09:07:30  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:31:37  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  20:56:59  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/08/23  17:04:31  yanke
 * Placeholders
 *
 * CSCdi39151:  modulized local/remote file systems
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#ifndef __FILESYS_REGISTRY_H__
#define __FILESYS_REGISTRY_H__

extern long filesys_regcode;

#include "../h/registry.h"
#include "../filesys/filesys_registry.regh"

#endif  /* __FILESYS_REGISTRY_H__ */
