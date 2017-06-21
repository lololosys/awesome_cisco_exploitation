/* $Id: async_debug.h,v 3.2 1995/11/17 09:25:24 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/if/async_debug.h,v $
 *------------------------------------------------------------------
 * async_debug.h -- Debugging declarations for Asyncronous interfaces
 *
 * May 1993, John Bashinski
 *
 * Copyright (c) 1993-1994 by Cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: async_debug.h,v $
 * Revision 3.2  1995/11/17  09:25:24  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:47:30  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:50:08  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/* Function to fire up async debugging */
extern void async_debug_init(void);

/*
 * The actual debugging flags are defined in async_debug_flags.h.
 * We include that file twice, once to define the flags themselves
 * and once to define the indices that the parser uses.
 */
#include "../if/async_debug_flags.h"
#define __DECLARE_DEBUG_NUMS__
#include "../if/async_debug_flags.h"
