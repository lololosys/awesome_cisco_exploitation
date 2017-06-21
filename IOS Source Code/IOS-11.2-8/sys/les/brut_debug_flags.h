/* $Id: brut_debug_flags.h,v 3.2 1995/11/17 17:36:35 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/les/brut_debug_flags.h,v $
 *------------------------------------------------------------------
 * brut_debug_flags.h -- Debugging flag declarations for BRUT
 *
 * May 1993, John Bashinski
 *
 * Copyright (c) 1993-1994 by Cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: brut_debug_flags.h,v $
 * Revision 3.2  1995/11/17  17:36:35  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:29:30  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:30:38  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * We get included (multiple times) from BRUT code to define
 * debugging flags. For this, we use some macros that get defined
 * by debug_macros.h. What definitions the macros get depends on
 * which magic flags have been set by the file that includes us.
 */
#include "../ui/debug_macros.h"

/* Now define the actual flags and the array that points to them */
DEBUG_ARRDECL(brut_debug_arr)

DEBUG_FLAG(brut_debug_receive,DEBUG_HUB_RCV,"DECHub90 Hub management receive message")
DEBUG_FLAG(brut_debug_transmit,DEBUG_HUB_TX,"DECHub90 Hub management transmit message")
DEBUG_FLAG(brut_trace_debug,DEBUG_HUB_TRC,"DECHub90 Hub management message trace")

DEBUG_ARRDONE

