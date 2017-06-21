/* $Id: dlsw_debug_flags.h,v 3.2 1995/11/17 09:03:10 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/dlsw/dlsw_debug_flags.h,v $
 *------------------------------------------------------------------
 * Debugging flag declarations for DLSw
 *
 * July 1994, Frank Bordonaro 
 *
 * Copyright (c) 1993-1995 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: dlsw_debug_flags.h,v $
 * Revision 3.2  1995/11/17  09:03:10  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:23:51  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  20:55:29  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/06/07  20:26:55  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/*
 * We get included (multiple times) from DLSw code to define
 * debugging flags. For this, we use some macros that get defined
 * by debug_macros.h. What definitions the macros get depends on
 * which magic flags have been set by the file that includes us.
 */
#include "../ui/debug_macros.h"

/* Now define the actual flags and the array that points to them */
DEBUG_ARRDECL(dlsw_debug_arr)

DEBUG_FLAG(dlsw_core_debug,DEBUG_DLSW_CORE,"DLSw Core")
DEBUG_FLAG(dlsw_csm_debug,DLSW_DEBUG_CSM,"DLSw Reachability")
DEBUG_FLAG(dlsw_local_debug,DLSW_DEBUG_LOCAL,"DLSw Local Circuit")
DEBUG_FLAG(dlsw_peer_debug,DLSW_DEBUG_PEER,"DLSw Peer")

DEBUG_ARRDONE
