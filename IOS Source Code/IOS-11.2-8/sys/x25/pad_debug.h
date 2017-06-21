/* $Id: pad_debug.h,v 3.2 1995/11/17 18:06:45 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/x25/pad_debug.h,v $
 *------------------------------------------------------------------
 * pad_debug.h -- Debugging declarations for X.29 PAD
 *
 * May 1993, John Bashinski
 *
 * Copyright (c) 1993-1994 by Cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: pad_debug.h,v $
 * Revision 3.2  1995/11/17  18:06:45  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:53:00  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:21:27  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/* Function to fire up PAD debugging */
extern void pad_debug_init(void);

/*
 * The actual debugging flags are defined in pad_debug_flags.h.
 * We include that file twice, once to define the flags themselves
 * and once to define the indices that the parser uses.
 */
#include "../x25/pad_debug_flags.h"
#define __DECLARE_DEBUG_NUMS__
#include "../x25/pad_debug_flags.h"
