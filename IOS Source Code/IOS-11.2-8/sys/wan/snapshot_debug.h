/* $Id: snapshot_debug.h,v 3.2 1995/11/17 18:04:58 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/wan/snapshot_debug.h,v $
 *------------------------------------------------------------------
 * snapshot_debug.h -- Debugging declarations for Dial on demand
 *
 * April 1994, Joel P. Bion
 *
 * Copyright (c) 1994 by Cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: snapshot_debug.h,v $
 * Revision 3.2  1995/11/17  18:04:58  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:51:20  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:20:06  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef _SNAPSHOT_DEBUG_H_
#define _SNAPSHOT_DEBUG_H_

/* Function to fire up Snapshot debugging */
extern void snapshot_debug_init(void);

/*
 * The actual debugging flags are defined in snapshot_debug_flags.h.
 * We include that file twice, once to define the flags themselves
 * and once to define the indices that the parser uses.
 */
#include "../wan/snapshot_debug_flags.h"
#define __DECLARE_DEBUG_NUMS__
#include "../wan/snapshot_debug_flags.h"

#endif
