/* $Id: hub_debug_flags.h,v 3.2 1995/11/17 17:37:48 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/les/hub_debug_flags.h,v $
 *------------------------------------------------------------------
 * hub_debug_flags.h
 *
 * March 1994, Viet Nguyen
 *
 * Copyright (c) 1994 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Gurus: Viet Nguyen
 *------------------------------------------------------------------
 * $Log: hub_debug_flags.h,v $
 * Revision 3.2  1995/11/17  17:37:48  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:30:36  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:31:56  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * We get included (multiple times) from hub code to define
 * debugging flags. For this, we use some macros that get defined
 * by debug_macros.h. What definitions the macros get depends on
 * which magic flags have been set by the file that includes us.
 */
#include "../ui/debug_macros.h"

/* Now define the actual flags and the array that points to them */
DEBUG_ARRDECL(hub_debug_arr)

DEBUG_FLAG(hub_debug,DEBUG_HUB,"Hub repeater")

DEBUG_ARRDONE
