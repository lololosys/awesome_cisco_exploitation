/* $Id: os_debug.h,v 3.2 1995/11/17 18:50:10 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/os/os_debug.h,v $
 *------------------------------------------------------------------
 * os_debug.h -- Debugging declarations for General OS
 *
 * May 1993, John Bashinski
 *
 * Copyright (c) 1993-1994 by Cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: os_debug.h,v $
 * Revision 3.2  1995/11/17  18:50:10  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:47:19  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:57:27  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/* Function to fire up OS debugging */
extern void os_debug_init(void);

/*
 * The actual debugging flags are defined in os_debug_flags.h.
 * We include that file twice, once to define the flags themselves
 * and once to define the indices that the parser uses.
 */
#include "../os/os_debug_flags.h"
#define __DECLARE_DEBUG_NUMS__
#include "../os/os_debug_flags.h"
