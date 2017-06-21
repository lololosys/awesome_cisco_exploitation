/* $Id: sse_debug.h,v 3.2 1995/11/17 09:20:56 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/hes/sse_debug.h,v $
 *------------------------------------------------------------------
 * sse_debug.h 
 *
 * June 1993, Tony Li
 *
 * Copyright (c) 1993-1994 by Cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: sse_debug.h,v $
 * Revision 3.2  1995/11/17  09:20:56  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:43:03  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:45:07  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Debugging
 *
 * The actual debugging flags are defined in sse_debug_flags.h.
 * We include that file twice, once to define the flags themselves
 * and once to define the indices that the parser uses.
 */

#include "../hes/sse_debug_flags.h"
#define __DECLARE_DEBUG_NUMS__
#include "../hes/sse_debug_flags.h"
