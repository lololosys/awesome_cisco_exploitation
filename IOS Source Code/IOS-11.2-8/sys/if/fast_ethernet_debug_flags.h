/* $Id: fast_ethernet_debug_flags.h,v 3.2 1995/11/17 09:27:30 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/if/fast_ethernet_debug_flags.h,v $
 *------------------------------------------------------------------
 * fast_ethernet_debug_flags.h -- Debugging flag declarations for fast ethernet
 *
 * November 1994, Gary Kramling
 * Adapted from channel_debug_flags.h by William H. Palmer
 *
 * Copyright (c) 1994-1995 by Cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: fast_ethernet_debug_flags.h,v $
 * Revision 3.2  1995/11/17  09:27:30  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:49:36  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:51:59  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/*
 * We get included (multiple times) from fast ethernet code to define
 * debugging flags. For this, we use some macros that get defined
 * by debug_macros.h. What definitions the macros get depends on
 * which magic flags have been set by the file that includes us.
 */
#include "../ui/debug_macros.h"

/* Now define the actual flags and the array that points to them */
DEBUG_ARRDECL(fast_ethernet_debug_arr)

DEBUG_FLAG(fast_ethernet_pkt_debug, DEBUG_FAST_ETHERNET_PKTS,
	   "Fast Ethernet packets")
DEBUG_FLAG(fast_ethernet_debug, DEBUG_FAST_ETHERNET, "Fast Ethernet events")

DEBUG_ARRDONE
