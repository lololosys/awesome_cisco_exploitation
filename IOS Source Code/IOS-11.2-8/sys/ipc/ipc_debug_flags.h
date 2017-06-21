/* $Id: ipc_debug_flags.h,v 3.2 1995/11/17 17:28:42 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/ipc/ipc_debug_flags.h,v $
 *------------------------------------------------------------------
 * ipc_debug_flags.h - debugging flags for IPC subsystem.
 *
 * March 1995, Jim Hayes
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ipc_debug_flags.h,v $
 * Revision 3.2  1995/11/17  17:28:42  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:59:47  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:05:35  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/06/07  21:02:38  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * My personal thanks to whomever cleans up the way we do debugging flags
 * around here.  Three files just to set up 6 boolean flags!?!
 */
 
/*
 * We get included (multiple times) from the IPC code to define
 * debugging flags. For this, we use some macros that get defined
 * by debug_macros.h. What definitions the macros get depends on
 * which magic flags have been set by the file that includes us.
 */

#include "../ui/debug.h"
#include "../ui/debug_macros.h"

/* Now define the actual flags and the array that points to them */

DEBUG_ARRDECL(ipc_debug_arr)

DEBUG_FLAG(ipc_debug_blocking,DEBUG_IPC_BLOCKING,"Process blocking")
DEBUG_FLAG(ipc_debug_buffers,DEBUG_IPC_BUFFERS,"Buffer usage")
DEBUG_FLAG(ipc_debug_events,DEBUG_IPC_EVENTS,"Special Events")
DEBUG_FLAG(ipc_debug_packets,DEBUG_IPC_PACKETS,"IPC packets")
DEBUG_FLAG(ipc_debug_ports,DEBUG_IPC_PORTS,"Port level transactions")
DEBUG_FLAG(ipc_debug_seats,DEBUG_IPC_SEATS,"Node level transactions")

DEBUG_ARRDONE
 
