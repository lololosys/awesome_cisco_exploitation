/* $Id: async_debug_flags.h,v 3.2 1995/11/17 09:25:26 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/if/async_debug_flags.h,v $
 *------------------------------------------------------------------
 * async_debug_flags.h -- Debugging flag declarations for async
 *
 * May 1993, John Bashinski
 *
 * Copyright (c) 1993-1994 by Cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: async_debug_flags.h,v $
 * Revision 3.2  1995/11/17  09:25:26  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:47:31  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:50:11  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * We get included (multiple times) from async code to define
 * debugging flags. For this, we use some macros that get defined
 * by debug_macros.h. What definitions the macros get depends on
 * which magic flags have been set by the file that includes us.
 */
#include "../ui/debug_macros.h"

/* Now define the actual flags and the array that points to them */
DEBUG_ARRDECL(async_debug_arr)

DEBUG_FLAG(async_framing_debug,DEBUG_ASYNC_FRAMING,"Async interface framing")
DEBUG_FLAG(async_packet_debug,DEBUG_ASYNC_PACKET,"Async packet I/O")
DEBUG_FLAG(async_state_debug,DEBUG_ASYNC_STATE,"Async interface state changes")

DEBUG_ARRDONE
