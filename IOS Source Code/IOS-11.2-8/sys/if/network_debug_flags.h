/* $Id: network_debug_flags.h,v 3.2 1995/11/17 09:30:33 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/if/network_debug_flags.h,v $
 *------------------------------------------------------------------
 * network_debug_flags.h -- Debugging flag declarations for interface
 *
 * May 1993, John Bashinski
 *
 * Copyright (c) 1993-1995 by Cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: network_debug_flags.h,v $
 * Revision 3.2  1995/11/17  09:30:33  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:51:36  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:54:14  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * We get included (multiple times) from interface code to define
 * debugging flags. For this, we use some macros that get defined
 * by debug_macros.h. What definitions the macros get depends on
 * which magic flags have been set by the file that includes us.
 */
#include "../ui/debug_macros.h"

/* Now define the actual flags and the array that points to them */
DEBUG_ARRDECL(network_debug_arr)

DEBUG_FLAG(broadcast_debug,DEBUG_BROADCAST,"Broadcast packet")
DEBUG_FLAG(custom_debug,DEBUG_CUSTOM,"Custom output queueing")
DEBUG_FLAG(pkt_debug,DEBUG_PKTTRACE,"Packet")
DEBUG_FLAG(priority_debug,DEBUG_PRIORITY,"Priority output queueing")
DEBUG_FLAG(fair_debug,DEBUG_FAIR_QUEUE,"Fair output queueing")

DEBUG_ARRDONE
