/* $Id: udp_debug_flags.h,v 3.2 1995/11/17 09:37:21 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/ip/udp_debug_flags.h,v $
 *------------------------------------------------------------------
 * udp_debug_flags.h -- Debugging flag declarations for UDP
 *
 * May 1993, John Bashinski
 *
 * Copyright (c) 1993-1994 by Cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: udp_debug_flags.h,v $
 * Revision 3.2  1995/11/17  09:37:21  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:59:25  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:02:15  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * We get included (multiple times) from UDP code to define
 * debugging flags. For this, we use some macros that get defined
 * by debug_macros.h. What definitions the macros get depends on
 * which magic flags have been set by the file that includes us.
 */
#include "../ui/debug_macros.h"

/* Now define the actual flags and the array that points to them */
DEBUG_ARRDECL(udp_debug_arr)

DEBUG_FLAG(udp_debug,DEBUG_UDP,"UDP packet")

DEBUG_ARRDONE
