/* $Id: pt_debug_flags.h,v 3.2 1995/11/17 18:59:34 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/pt/pt_debug_flags.h,v $
 *------------------------------------------------------------------
 * pt_debug_flags.h -- Debugging flag declarations for PT
 *
 * May 1993, John Bashinski
 *
 * Copyright (c) 1993-1994 by Cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: pt_debug_flags.h,v $
 * Revision 3.2  1995/11/17  18:59:34  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:01:11  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:34:56  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * We get included (multiple times) from PT code to define
 * debugging flags. For this, we use some macros that get defined
 * by debug_macros.h. What definitions the macros get depends on
 * which magic flags have been set by the file that includes us.
 */
#include "../ui/debug_macros.h"

/* Now define the actual flags and the array that points to them */
DEBUG_ARRDECL(pt_debug_arr)

DEBUG_FLAG(translate_debug,DEBUG_TRANSLATE,"Protocol Translation")

DEBUG_ARRDONE
