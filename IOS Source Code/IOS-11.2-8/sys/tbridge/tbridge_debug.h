/* $Id: tbridge_debug.h,v 3.2 1995/11/17 18:45:02 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/tbridge/tbridge_debug.h,v $
 *------------------------------------------------------------------
 * Transparent Bridging debug flags.
 *
 * May 1995, Tony Speakman
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: tbridge_debug.h,v $
 * Revision 3.2  1995/11/17  18:45:02  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:33:23  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:03:05  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/* Function to fire up TBRIDGE debugging */
extern void tbridge_debug_init(void);

/*
 * The actual debugging flags are defined in tbridge_debug_flags.h.
 * We include that file twice, once to define the flags themselves
 * and once to define the indices that the parser uses.
 */
#include "../tbridge/tbridge_debug_flags.h"
#define __DECLARE_DEBUG_NUMS__
#include "../tbridge/tbridge_debug_flags.h"
