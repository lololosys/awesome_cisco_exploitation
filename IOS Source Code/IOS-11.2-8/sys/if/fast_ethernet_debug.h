/* $Id: fast_ethernet_debug.h,v 3.2 1995/11/17 09:27:29 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/if/fast_ethernet_debug.h,v $
 *------------------------------------------------------------------
 * fast_ethernet_debug.h -- Debugging declarations for fast ethernet interfaces
 *
 * November 1994, Gary Kramling
 * Adapted from channel_debug.h by William H. Palmer
 *
 * Copyright (c) 1994-1995 by Cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: fast_ethernet_debug.h,v $
 * Revision 3.2  1995/11/17  09:27:29  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:49:34  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:51:57  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/* Function to fire up fast_ethernet debugging */
extern void fast_ethernet_debug_init(void);

/*
 * The actual debugging flags are defined in fast_ethernet_debug_flags.h.
 * We include that file twice, once to define the flags themselves
 * and once to define the indices that the parser uses.
 */
#include "../if/fast_ethernet_debug_flags.h"
#define __DECLARE_DEBUG_NUMS__
#include "../if/fast_ethernet_debug_flags.h"
