/* $Id: comp_debug_flags.h,v 3.2 1995/11/17 18:05:51 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/x25/comp_debug_flags.h,v $
 *------------------------------------------------------------------
 * COMP_DEBUG_FLAGS.H
 *
 * 10/29/93 Bill Thomas
 *
 * Copyright (c) 1993-1994 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: comp_debug_flags.h,v $
 * Revision 3.2  1995/11/17  18:05:51  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:52:07  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:20:41  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * We get included (multiple times) from COMPRESS code to define
 * debugging flags. For this, we use some macros that get defined
 * by debug_macros.h. What definitions the macros get depends on
 * which magic flags have been set by the file that includes us.
 */
#include "../ui/debug_macros.h"

/* Now define the actual flags and the array that points to them */
DEBUG_ARRDECL(compress_debug_arr)

DEBUG_FLAG(compress_debug,DEBUG_COMPRESS,"COMPRESS")

DEBUG_ARRDONE
