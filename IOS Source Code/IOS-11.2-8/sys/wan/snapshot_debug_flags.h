/* $Id: snapshot_debug_flags.h,v 3.2 1995/11/17 18:04:59 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/wan/snapshot_debug_flags.h,v $
 *------------------------------------------------------------------
 * snapshot_debug_flags.h -- Debugging flag declarations for
 *				   snapshot
 *
 * April 1994, Joel P. Bion
 *
 * Copyright (c) 1994 by Cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: snapshot_debug_flags.h,v $
 * Revision 3.2  1995/11/17  18:04:59  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:51:21  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:20:07  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * We get included (multiple times) from Snapshot code to define
 * debugging flags. For this, we use some macros that get defined
 * by debug_macros.h. What definitions the macros get depends on
 * which magic flags have been set by the file that includes us.
 */
#include "../ui/debug_macros.h"

/* Now define the actual flags and the array that points to them */
DEBUG_ARRDECL(snapshot_debug_arr)

DEBUG_FLAG(snapshot_debug,DEBUG_SNAPSHOT,"Snapshot support")

DEBUG_ARRDONE

