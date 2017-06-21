/* $Id: arp_debug.h,v 3.2 1995/11/17 09:25:19 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/if/arp_debug.h,v $
 *------------------------------------------------------------------
 * arp_debug.h -- Debugging declarations for ARP
 *
 * May 1993, John Bashinski
 *
 * Copyright (c) 1993-1994 by Cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: arp_debug.h,v $
 * Revision 3.2  1995/11/17  09:25:19  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:47:26  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:50:00  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/* Function to fire up ARP debugging */
extern void arp_debug_init(void);

/*
 * The actual debugging flags are defined in arp_debug_flags.h.
 * We include that file twice, once to define the flags themselves
 * and once to define the indices that the parser uses.
 */
#include "../if/arp_debug_flags.h"
#define __DECLARE_DEBUG_NUMS__
#include "../if/arp_debug_flags.h"
