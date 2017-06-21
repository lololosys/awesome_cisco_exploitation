/* $Id: sse_debug_flags.h,v 3.2 1995/11/17 09:20:58 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/hes/sse_debug_flags.h,v $
 *------------------------------------------------------------------
 * sse_debug_flags.h -- Debugging flag declarations for IP
 *
 * June 1993, Tony Li
 *
 * Copyright (c) 1993-1994 by Cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: sse_debug_flags.h,v $
 * Revision 3.2  1995/11/17  09:20:58  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:43:04  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:45:09  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#include "../ui/debug_macros.h"

/* Now define the actual flags and the array that points to them */
DEBUG_ARRDECL(sse_debug_arr)

DEBUG_FLAG(sse_debug,DEBUG_SSE,"Silicon Switching Engine")

DEBUG_ARRDONE
