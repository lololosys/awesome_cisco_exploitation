/* $Id: dlsw_core_debug_flags.h,v 3.2 1995/11/17 09:02:26 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/dlsw/dlsw_core_debug_flags.h,v $
 *------------------------------------------------------------------
 * All the core specific debug flags are defined in here
 * DLSw subsystem 
 *
 * Fri July 29, 1994 G. Pandian
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 * dlsw core debug flags definition 
 *------------------------------------------------------------------
 * $Log: dlsw_core_debug_flags.h,v $
 * Revision 3.2  1995/11/17  09:02:26  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:22:52  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  20:54:57  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/06/07  20:26:12  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/*
 * We get included (multiple times) from DLSW code to define
 * debugging flags. For this, we use some macros that get defined
 * by debug_macros.h. What definitions the macros get depends on
 * which magic flags have been set by the file that includes us.
 */
#include "../ui/debug_macros.h"

/* Now define the actual flags and the array that points to them */
DEBUG_ARRDECL(dlsw_core_debug_arr)
DEBUG_FLAG(dlsw_core_debug_flow, DEBUG_DLSW_CORE_FLOW, "DLSw core flow-control")
DEBUG_FLAG(dlsw_core_debug_xid, DEBUG_DLSW_CORE_XID,"DLSw core XID C/R")
DEBUG_FLAG(dlsw_core_debug_msgs, DEBUG_DLSW_CORE_MSGS,"DLSw core messages")
DEBUG_FLAG(dlsw_core_debug_state, DEBUG_DLSW_CORE_STATE,"DLSw core states")
DEBUG_ARRDONE

DEBUG_ARRDECL(dlsw_csm_debug_arr)
DEBUG_FLAG(dlsw_csm_debug_error, DEBUG_DLSW_CSM_ERRORS, "DLSw CSM errors")
DEBUG_FLAG(dlsw_csm_debug_event, DEBUG_DLSW_CSM_EVENTS,"DLSw CSM events")
DEBUG_FLAG(dlsw_csm_debug_verbose, DEBUG_DLSW_CSM_VERBOSE,"DLSw CSM details")
DEBUG_FLAG(dlsw_csm_debug_sna, DEBUG_DLSW_SNA_REACH,"DLSw CSM SNA")
DEBUG_FLAG(dlsw_csm_debug_netb, DEBUG_DLSW_NETB_REACH, "DLSw CSM NetBIOS")
DEBUG_FLAG(dlsw_csm_debug_all, DEBUG_DLSW_ALL_REACH,"DLSw CSM - All protocols")
DEBUG_ARRDONE











