/* $Id: pad_debug_flags.h,v 3.2 1995/11/17 18:06:47 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/x25/pad_debug_flags.h,v $
 *------------------------------------------------------------------
 * pad_debug_flags.h -- Debugging flag declarations for PAD
 *
 * May 1993, John Bashinski
 *
 * Copyright (c) 1993-1994 by Cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: pad_debug_flags.h,v $
 * Revision 3.2  1995/11/17  18:06:47  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:53:02  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:21:29  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * We get included (multiple times) from PAD code to define
 * debugging flags. For this, we use some macros that get defined
 * by debug_macros.h. What definitions the macros get depends on
 * which magic flags have been set by the file that includes us.
 */
#include "../ui/debug_macros.h"

/* Now define the actual flags and the array that points to them */
DEBUG_ARRDECL(pad_debug_arr)

DEBUG_FLAG(pad_debug,DEBUG_PAD,"X25 PAD")

DEBUG_ARRDONE
