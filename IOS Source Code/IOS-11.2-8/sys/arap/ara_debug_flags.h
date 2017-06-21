/* $Id: ara_debug_flags.h,v 3.2 1995/11/17 08:38:10 hampton Exp $
 * $Source: /vws/aam/cherf.111/ios/sys/arap/ara_debug_flags.h,v $
 *------------------------------------------------------------------
 * ara_debug_flags.h -- Debugging flag declarations for ARA
 *
 * September 1993, Tim Kolar
 *
 * Copyright (c) 1993-1994 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ara_debug_flags.h,v $
 * Revision 3.2  1995/11/17  08:38:10  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  10:53:12  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:04:12  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * We get included (multiple times) from ARA code to define
 * debugging flags. For this, we use some macros that get defined
 * by debug_macros.h. What definitions the macros get depends on
 * which magic flags have been set by the file that includes us.
 */
#include "../ui/debug_macros.h"

/* Now define the actual flags and the array that points to them */
DEBUG_ARRDECL(ara_debug_arr)

DEBUG_FLAG(arapmnp4_debug, DEBUG_ARAP_MNP4, "ARAP MNP4")
DEBUG_FLAG(arapv42bis_debug, DEBUG_ARAP_V42BIS, "ARAP V.42bis")
DEBUG_FLAG(arapinternal_debug, DEBUG_ARAP_INTERNAL, "ARAP internal packet")
DEBUG_FLAG(arapmemory_debug, DEBUG_ARAP_MEMORY, "ARAP memory")

DEBUG_ARRDONE
