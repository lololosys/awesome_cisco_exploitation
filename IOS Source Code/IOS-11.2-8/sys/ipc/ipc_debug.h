/* $Id: ipc_debug.h,v 3.2 1995/11/17 17:28:41 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/ipc/ipc_debug.h,v $
 *------------------------------------------------------------------
 * ipc_debug.h - Headers for routines used for IPC debugging.
 *
 * March 1995, Jim Hayes
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ipc_debug.h,v $
 * Revision 3.2  1995/11/17  17:28:41  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:59:45  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:05:34  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/06/07  21:02:35  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
 
extern void ipc_init_debug(void);

/*
 * The actual debugging flags are defined in ipc_debug_flags.h.
 * We include that file twice, once to define the flags themselves
 * and once to define the indices that the parser uses.
 */

#include "ipc_debug_flags.h"
#define __DECLARE_DEBUG_NUMS__
#include "ipc_debug_flags.h"

extern boolean ipc_debug_blocking;
extern boolean ipc_debug_buffers;
extern boolean ipc_debug_events;
extern boolean ipc_debug_packets;
extern boolean ipc_debug_ports;
extern boolean ipc_debug_seats;

