/* $Id: platform_registry.h,v 3.1 1995/11/09 12:34:20 shaker Exp $
 *------------------------------------------------------------------
 * platform_registry.h -- Platform Service Points
 *
 * Oct. 18 1995 - Julie Quan
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: platform_registry.h,v $
 * Revision 3.1  1995/11/09  12:34:20  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/10/23  16:16:02  jquan
 * CSCdi40522:  Modularity: sub_c3000 should not require sub_brut
 *
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __PLATFORM_REGISTRY_H__
#define __PLATFORM_REGISTRY_H__

extern int platform_regcode;

#include "registry.h"
#include "../les/platform_registry.regh"

#endif /* __PLATFORM_REGISTRY_H__ */
