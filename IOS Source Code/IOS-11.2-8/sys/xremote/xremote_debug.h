/* $Id: xremote_debug.h,v 3.2 1995/11/17 19:26:58 gstovall Exp $
 * $Source: /swtal/cherf.111/ios/sys/xremote/xremote_debug.h,v $
 *------------------------------------------------------------------
 * xremote_debug.h -- Debugging declarations for XRemote
 *
 * May 1993, John Bashinski
 *
 * Copyright (c) 1993-1994 by Cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: xremote_debug.h,v $
 * Revision 3.2  1995/11/17  19:26:58  gstovall
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  14:01:48  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:35:29  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/* Function to fire up XRemote debugging */
extern void xr_debug_init(void);

/*
 * The actual debugging flags are defined in xr_debug_flags.h.
 * We include that file twice, once to define the flags themselves
 * and once to define the indices that the parser uses.
 */
#include "../xremote/xremote_debug_flags.h"
#define __DECLARE_DEBUG_NUMS__
#include "../xremote/xremote_debug_flags.h"
