/* $Id: isis_registry.h,v 3.1 1995/11/09 11:09:50 shaker Exp $
 * $Source: /swtal/cherf.111/ios/sys/clns/isis_registry.h,v $
 *------------------------------------------------------------------
 * ISIS registry include files
 *
 * August 1995, Dave Katz
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: isis_registry.h,v $
 * Revision 3.1  1995/11/09  11:09:50  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/09/23  21:37:49  dkatz
 * CSCdi38306:  ISIS shouldnt run SPF on leaf route changes
 * Calculate routes from only changed LSPs if the inter-router topology
 * stays constant.  Don't gratuitously install IP routes that will
 * be immediately replaced, as this causes large-scale cache invalidation.
 * Eliminate multiple IP forwarding table traversals.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __ISIS_REGISTRY_H__
#define __ISIS_REGISTRY_H__

#include "../h/registry.h"
#include "../clns/isis_registry.regh"

#endif /* __ISIS_REGISTRY_H__ */
