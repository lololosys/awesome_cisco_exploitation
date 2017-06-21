/* $Id: tarp_debug.h,v 3.2 1995/11/17 18:44:42 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/tarp/tarp_debug.h,v $
 *------------------------------------------------------------------
 * TARP (Target ID address Resolution Protocol)
 *
 * October 1995, Arun Sastry
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: tarp_debug.h,v $
 * Revision 3.2  1995/11/17  18:44:42  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:33:09  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/* Function to fire up TARP debugging */
extern void tarp_debug_init(void);

/*
 * The actual debugging flags are defined in tarp_debug_flags.h.
 * We include that file twice, once to define the flags themselves
 * and once to define the indices that the parser uses.
 */
#include "../tarp/tarp_debug_flags.h"
#define __DECLARE_DEBUG_NUMS__
#include "../tarp/tarp_debug_flags.h"

