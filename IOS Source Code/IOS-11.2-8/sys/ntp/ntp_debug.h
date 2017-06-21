/* $Id: ntp_debug.h,v 3.2 1995/11/17 17:51:01 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/ntp/ntp_debug.h,v $
 *------------------------------------------------------------------
 * ntp_debug.h -- Debugging declarations for NTP
 *
 * May 1993, John Bashinski
 *
 * Copyright (c) 1993-1994 by Cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ntp_debug.h,v $
 * Revision 3.2  1995/11/17  17:51:01  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:41:03  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:45:09  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/* Function to fire up NTP debugging */
extern void ntp_debug_init(void);

/*
 * The actual debugging flags are defined in ntp_debug_flags.h.
 * We include that file twice, once to define the flags themselves
 * and once to define the indices that the parser uses.
 */
#include "../ntp/ntp_debug_flags.h"
#define __DECLARE_DEBUG_NUMS__
#include "../ntp/ntp_debug_flags.h"
