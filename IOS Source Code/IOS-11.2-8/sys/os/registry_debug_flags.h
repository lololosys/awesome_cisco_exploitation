/* $Id: registry_debug_flags.h,v 3.2 1995/11/17 18:51:09 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/os/registry_debug_flags.h,v $
 *------------------------------------------------------------------
 * registry_debug_flags.h -- Debugging flag declarations for registries
 *
 * November 1993, Scott Mackie
 *
 * Copyright (c) 1993-1994 by Cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: registry_debug_flags.h,v $
 * Revision 3.2  1995/11/17  18:51:09  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:47:50  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:58:12  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * We get included (multiple times) from registry code to define
 * debugging flags. For this, we use some macros that get defined
 * by debug_macros.h. What definitions the macros get depends on
 * which magic flags have been set by the file that includes us.
 */
#include "../ui/debug_macros.h"

/* Now define the actual flags and the array that points to them */
DEBUG_ARRDECL(registry_debug_arr)

DEBUG_FLAG(registry_miss_debug,DEBUG_REGISTRY_MISS,"Service Miss")
DEBUG_FLAG(registry_trace_debug,DEBUG_REGISTRY_TRACE,"Function Trace")

DEBUG_ARRDONE
