/* $Id: decnet_debug_flags.h,v 3.2 1995/11/17 08:58:48 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/decnet/decnet_debug_flags.h,v $
 *------------------------------------------------------------------
 * decnet_debug_flags.h -- Debugging flag declarations for DECNET
 *
 * May 1993, John Bashinski
 *
 * Copyright (c) 1993-1994 by Cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: decnet_debug_flags.h,v $
 * Revision 3.2  1995/11/17  08:58:48  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:15:50  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:22:47  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * We get included (multiple times) from DECNET code to define
 * debugging flags. For this, we use some macros that get defined
 * by debug_macros.h. What definitions the macros get depends on
 * which magic flags have been set by the file that includes us.
 */
#include "../ui/debug_macros.h"

/* Now define the actual flags and the array that points to them */
DEBUG_ARRDECL(decnet_debug_arr)

DEBUG_FLAG(decnet_conn_debug,DEBUG_CONN_DECNET,"DECnet connects")
DEBUG_FLAG(decnet_debug,DEBUG_DECNET,"DECnet packets (errors)")
DEBUG_FLAG(decnet_adj_debug, DEBUG_DECNET_ADJ, "DECnet adjacencies")
DEBUG_FLAG(decnet_pkt_debug, DEBUG_DECNET_PKT, "DECnet packet forwarding")
DEBUG_FLAG(decnet_rt_debug,DEBUG_RT_DECNET,"DECnet routing")

DEBUG_ARRDONE
