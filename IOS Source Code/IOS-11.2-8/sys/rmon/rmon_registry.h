/* $Id: rmon_registry.h,v 3.2 1995/11/17 18:39:14 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/rmon/rmon_registry.h,v $
 *------------------------------------------------------------------
 * Remote Monitoring MIB Support
 *
 * September 1995, Jeffrey T. Johnson
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: rmon_registry.h,v $
 * Revision 3.2  1995/11/17  18:39:14  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:03:03  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __RMON_REGISTRY_H__
#define __RMON_REGISTRY_H__

extern long rmon_regcode;

#include "registry.h"
#include "../rmon/rmon_registry.regh"

extern void init_rmon_registry(void);

#endif /* __RMON_REGISTRY_H__ */
