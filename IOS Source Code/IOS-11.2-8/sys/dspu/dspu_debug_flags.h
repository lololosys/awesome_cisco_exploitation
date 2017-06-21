/* $Id: dspu_debug_flags.h,v 3.2 1995/11/17 09:04:48 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/dspu/dspu_debug_flags.h,v $
 *------------------------------------------------------------------
 * Down Stream PU support
 * 
 * Mar. 1994, Vivian Tseng
 *
 * Copyright (c) 1994-1995,1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: dspu_debug_flags.h,v $
 * Revision 3.2  1995/11/17  09:04:48  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:28:11  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:28:34  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "../ui/debug_macros.h"

/* Now define the actual flags and the array that points to them */
DEBUG_ARRDECL(dspu_debug_arr)

DEBUG_FLAG(dspu_debug_state,      DEBUG_DSPU_STATE,      "DSPU State") 
DEBUG_FLAG(dspu_debug_trace,      DEBUG_DSPU_TRACE,      "DSPU Trace")
DEBUG_FLAG(dspu_debug_packets,    DEBUG_DSPU_PACKETS,    "DSPU Packet")
DEBUG_FLAG(dspu_debug_activation, DEBUG_DSPU_ACTIVATION, "DSPU Activation")

DEBUG_FLAG(sna_debug_state,      DEBUG_SNA_STATE,      "SNA State") 
DEBUG_FLAG(sna_debug_trace,      DEBUG_SNA_TRACE,      "SNA Trace")
DEBUG_FLAG(sna_debug_packets,    DEBUG_SNA_PACKETS,    "SNA Packet")
DEBUG_FLAG(sna_debug_activation, DEBUG_SNA_ACTIVATION, "SNA Activation")

DEBUG_ARRDONE
