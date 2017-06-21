/* $Id: v120_debug_flags.h,v 3.2 1995/11/17 18:05:23 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/wan/v120_debug_flags.h,v $
 *------------------------------------------------------------------
 * v120_debug_flags.h -- Debugging flag declarations for PPP
 *
 * August, 1995, Bill May
 *
 * Copyright (c) 1995 by Cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: v120_debug_flags.h,v $
 * Revision 3.2  1995/11/17  18:05:23  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:51:41  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:37:06  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/09/21  23:30:33  wmay
 * Placeholder for CS_Arkansas_branch
 *------------------------------------------------------------------
 * $Endlog$
 */
/*
 * We get included (multiple times) from v120 code to define
 * debugging flags. For this, we use some macros that get defined
 * by debug_macros.h. What definitions the macros get depends on
 * which magic flags have been set by the file that includes us.
 */
#include "../ui/debug_macros.h"

/* Now define the actual flags and the array that points to them */
DEBUG_ARRDECL(v120_debug_arr)
    
DEBUG_FLAG(v120_debug,DEBUG_V120_EVENT, "V120 events")
DEBUG_FLAG(v120_debug_packet, DEBUG_V120_PACKET, "V120 packet")
DEBUG_ARRDONE
