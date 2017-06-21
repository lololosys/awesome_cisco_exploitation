/* $Id: isdn_debug_flags.h,v 3.2 1995/11/17 18:03:09 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/wan/isdn_debug_flags.h,v $
 *------------------------------------------------------------------
 * isdn_debug_flags.h -- Debugging flag declarations for ISDN
 *
 * May 1993, John Bashinski
 *
 * Copyright (c) 1993-1994 by Cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: isdn_debug_flags.h,v $
 * Revision 3.2  1995/11/17  18:03:09  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:49:49  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:18:33  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * We get included (multiple times) from ISDN code to define
 * debugging flags. For this, we use some macros that get defined
 * by debug_macros.h. What definitions the macros get depends on
 * which magic flags have been set by the file that includes us.
 */
#include "../ui/debug_macros.h"

/* Now define the actual flags and the array that points to them */
DEBUG_ARRDECL(isdn_debug_arr)

DEBUG_FLAG(isdn_debug,DEBUG_ISDN_EVENTS,"ISDN events")
DEBUG_FLAG(q921_debug,DEBUG_ISDN_Q921,"ISDN Q921 packets")
DEBUG_FLAG(q931_debug,DEBUG_ISDN_Q931,"ISDN Q931 packets")
DEBUG_FLAG(detail_debug,DEBUG_ISDN_DETAIL,"ISDN detailed info")

DEBUG_ARRDONE
