/* $Id: ether_debug.h,v 3.2 1995/11/17 09:26:59 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/if/ether_debug.h,v $
 *------------------------------------------------------------------
 * ether_debug.h -- Debugging declarations for General Ethernet
 *
 * May 1993, John Bashinski
 *
 * Copyright (c) 1993-1994 by Cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ether_debug.h,v $
 * Revision 3.2  1995/11/17  09:26:59  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:48:56  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:51:45  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/* Function to fire up Ethernet debugging */
extern void ether_debug_init(void);

/*
 * The actual debugging flags are defined in ether_debug_flags.h.
 * We include that file twice, once to define the flags themselves
 * and once to define the indices that the parser uses.
 */
#include "../if/ether_debug_flags.h"
#define __DECLARE_DEBUG_NUMS__
#include "../if/ether_debug_flags.h"
