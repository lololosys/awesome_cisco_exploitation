/* $Id: ts_debug.h,v 3.2 1995/11/17 18:56:27 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/ts/ts_debug.h,v $
 *------------------------------------------------------------------
 * ts_debug.h -- Debugging declarations for General terminal server
 *
 * May 1993, John Bashinski
 *
 * Copyright (c) 1993-1994 by Cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ts_debug.h,v $
 * Revision 3.2  1995/11/17  18:56:27  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:40:13  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:10:27  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/* Function to fire up TS debugging */
extern void ts_debug_init(void);

/*
 * The actual debugging flags are defined in ts_debug_flags.h.
 * We include that file twice, once to define the flags themselves
 * and once to define the indices that the parser uses.
 */
#include "../ts/ts_debug_flags.h"
#define __DECLARE_DEBUG_NUMS__
#include "../ts/ts_debug_flags.h"
