/* $Id: apollo_debug_flags.h,v 3.2 1995/11/17 19:20:39 gstovall Exp $
 * $Source: /swtal/cherf.111/ios/sys/xns/apollo_debug_flags.h,v $
 *------------------------------------------------------------------
 * apollo_debug_flags.h -- Debugging flag declarations for Apollo
 *
 * May 1993, John Bashinski
 *
 * Copyright (c) 1993-1994 by Cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: apollo_debug_flags.h,v $
 * Revision 3.2  1995/11/17  19:20:39  gstovall
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:57:02  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:26:47  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * We get included (multiple times) from Apollo code to define
 * debugging flags. For this, we use some macros that get defined
 * by debug_macros.h. What definitions the macros get depends on
 * which magic flags have been set by the file that includes us.
 */
#include "../ui/debug_macros.h"

/* Now define the actual flags and the array that points to them */
DEBUG_ARRDECL(apollo_debug_arr)

DEBUG_FLAG(apollo_debug,DEBUG_APOLLO,"Apollo Domain packet")
DEBUG_FLAG(apollorip_debug,DEBUG_APOLLORIP,"Apollo Domain routing")

DEBUG_ARRDONE
