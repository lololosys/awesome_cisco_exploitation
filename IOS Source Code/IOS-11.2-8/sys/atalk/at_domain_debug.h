/* $Id: at_domain_debug.h,v 3.2 1995/11/17 08:42:09 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/atalk/at_domain_debug.h,v $
 *------------------------------------------------------------------
 * Support for debugging of AppleTalk Remapping
 *
 * September 1995, Steven Lin
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: at_domain_debug.h,v $
 * Revision 3.2  1995/11/17  08:42:09  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  10:56:31  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  20:49:41  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/09/28  00:25:30  slin
 * Placeholders for AppleTalk Code Modularization
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
#ifndef __AT_DOMAIN_DEBUG_H__
#define __AT_DOMAIN_DEBUG_H__

extern atalkdebugtype *atalkdomain_debug;
extern atalkdebugtype *atalkremap_debug;

enum {
    DEBUG_ATALK_DOMAIN,
    DEBUG_ATALK_REMAP,
    DEBUG_ATALK_DOMAIN_MAX
};

extern void atdomain_debug_init(void);

#endif
