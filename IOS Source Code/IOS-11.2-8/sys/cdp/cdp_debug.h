/* $Id: cdp_debug.h,v 3.2 1995/11/17 08:49:46 hampton Exp $:
 * $Source: /swtal/cherf.111/ios/sys/cdp/cdp_debug.h,v $
 *------------------------------------------------------------------
 * Debugging routines for CDP
 *
 * October 1994, Arun Sastry
 *
 * Copyright (c) 1994 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cdp_debug.h,v $
 * Revision 3.2  1995/11/17  08:49:46  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:05:11  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:15:01  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/* Function to fire up CDP debugging */
extern void cdp_debug_init(void);

/*
 * The actual debugging flags are defined in cdp_debug_flags.h.
 * We include that file twice, once to define the flags themselves
 * and once to define the indices that the parser uses.
 */
#include "../cdp/cdp_debug_flags.h"
#define __DECLARE_DEBUG_NUMS__
#include "../cdp/cdp_debug_flags.h"

