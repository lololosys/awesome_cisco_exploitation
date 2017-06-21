/* $Id: channel_debug.h,v 3.2 1995/11/17 09:26:33 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/if/channel_debug.h,v $
 *------------------------------------------------------------------
 * channel_debug.h -- Debugging declarations for IBM Channel interfaces
 *
 * April 1994, William H. Palmer
 *
 * Copyright (c) 1993-1994 by Cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: channel_debug.h,v $
 * Revision 3.2  1995/11/17  09:26:33  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:48:34  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:51:07  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/* Function to fire up channel debugging */
extern void channel_debug_init(void);

/*
 * The actual debugging flags are defined in channel_debug_flags.h.
 * We include that file twice, once to define the flags themselves
 * and once to define the indices that the parser uses.
 */
#include "../if/channel_debug_flags.h"
#define __DECLARE_DEBUG_NUMS__
#include "../if/channel_debug_flags.h"
