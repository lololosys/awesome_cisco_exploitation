/* $Id: bri_debug_flags.h,v 3.1 1995/11/09 11:47:56 shaker Exp $
 * $Source: /swtal/cherf.111/ios/sys/if/bri_debug_flags.h,v $
 *------------------------------------------------------------------
 * bri_debug_flags.h -- Debugging flag declarations for BRI.
 *
 * March 1994, Manoj Leelanivas.
 *
 * Copyright (c) 1993-1994 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * We get included (multiple times) from BRI code to define
 * debugging flags. For this, we use some macros that get defined
 * by debug_macros.h. What definitions the macros get depends on
 * which magic flags have been set by the file that includes us.
 */
#include "../ui/debug_macros.h"

/* Now define the actual flags and the array that points to them */
DEBUG_ARRDECL(bri_debug_arr)

DEBUG_FLAG(bri_debug,DEBUG_BRI,"Basic Rate network interface")

DEBUG_ARRDONE



