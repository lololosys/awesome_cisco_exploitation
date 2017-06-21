/* $Id: isdn_registry.h,v 3.2 1995/11/17 17:40:48 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/isdn/isdn_registry.h,v $
 *------------------------------------------------------------------
 * isdn_registry.h -- Registration Points for isdn support
 *
 * March 1995, Bibek Das
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: isdn_registry.h,v $
 * Revision 3.2  1995/11/17  17:40:48  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:13:45  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:15:37  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#ifndef __ISDN_REGISTRY_H__
#define __ISDN_REGISTRY_H__

extern long isdn_regcode;

#include "../h/registry.h"
#include "../isdn/isdn_registry.regh"

#endif  /* __ISDN_REGISTRY_H__ */
