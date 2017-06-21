/* $Id: callback_debug_flags.h,v 3.2 1995/11/17 18:52:48 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/ts/callback_debug_flags.h,v $
 *------------------------------------------------------------------
 * Callback debug flags
 *
 * April 1995, Shoou Yiu
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: callback_debug_flags.h,v $
 * Revision 3.2  1995/11/17  18:52:48  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:38:13  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:08:49  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
/*
 * We get included (multiple times) from chat code to define
 * debugging flags. For this, we use some macros that get defined
 * by debug_macros.h. What definitions the macros get depends on
 * which magic flags have been set by the file that includes us.
 */
#include "../ui/debug_macros.h"

/* Now define the actual flags and the array that points to them */
DEBUG_ARRDECL(callback_debug_arr)

DEBUG_FLAG(callback_debug,DEBUG_CALLBACK,"Callback activity")

DEBUG_ARRDONE
