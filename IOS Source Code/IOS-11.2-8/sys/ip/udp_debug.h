/* $Id: udp_debug.h,v 3.2 1995/11/17 09:37:19 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/ip/udp_debug.h,v $
 *------------------------------------------------------------------
 * udp_debug.h -- Debugging declarations for UDP
 *
 * May 1993, John Bashinski
 *
 * Copyright (c) 1993-1994 by Cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: udp_debug.h,v $
 * Revision 3.2  1995/11/17  09:37:19  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:59:23  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:02:12  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/* Function to fire up UDP debugging */
extern void udp_debug_init(void);

/*
 * The actual debugging flags are defined in udp_debug_flags.h.
 * We include that file twice, once to define the flags themselves
 * and once to define the indices that the parser uses.
 */
#include "../ip/udp_debug_flags.h"
#define __DECLARE_DEBUG_NUMS__
#include "../ip/udp_debug_flags.h"
