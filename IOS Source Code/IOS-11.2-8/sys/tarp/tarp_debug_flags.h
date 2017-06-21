/* $Id: tarp_debug_flags.h,v 3.2 1995/11/17 18:44:44 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/tarp/tarp_debug_flags.h,v $
 *------------------------------------------------------------------
 * TARP (Target ID address Resolution Protocol)
 *
 * October 1995, Arun Sastry
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: tarp_debug_flags.h,v $
 * Revision 3.2  1995/11/17  18:44:44  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:33:10  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "../ui/debug_macros.h"

/* Define the actual flags and the array that points to them */
DEBUG_ARRDECL(tarp_debug_arr)

DEBUG_FLAG(tarp_pkt_debug, DEBUG_TARP_PACKETS,"TARP packet info")
DEBUG_FLAG(tarp_event_debug, DEBUG_TARP_EVENTS, "TARP events")

DEBUG_ARRDONE
