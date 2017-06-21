/* $Id: envmib_registry.h,v 3.1 1995/12/01 19:03:38 sdurham Exp $
 * $Source: /swtal/cherf.111/ios/sys/hes/envmib_registry.h,v $
 *------------------------------------------------------------------
 * envmib_registry.h -- Service Points for environmental monitor mib Registry
 *
 * November, 1995 Sandra Durham
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: envmib_registry.h,v $
 * Revision 3.1  1995/12/01  19:03:38  sdurham
 * CSCdi44362:  envmon is non-modular w.r.t envmon mib.
 *   Remove direct calls from envmon code to mib code. Use service points.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef	__ENVMIB_REGISTRY_H__
#define	__ENVMIB_REGISTRY_H__

#include "registry.h"
#include "../hes/envmib_registry.regh"

#endif /* __ENVMIB_REGISTRY_H__ */

