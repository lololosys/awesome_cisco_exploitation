/* $Id: ppp_debug.h,v 3.2 1995/11/17 18:04:15 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/wan/ppp_debug.h,v $
 *------------------------------------------------------------------
 * ppp_debug.h -- Debugging declarations for PPP
 *
 * May 1993, John Bashinski
 *
 * Copyright (c) 1993-1994 by Cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ppp_debug.h,v $
 * Revision 3.2  1995/11/17  18:04:15  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:50:44  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:19:18  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/* Function to fire up PPP debugging */
extern void ppp_debug_init(void);

/*
 * The actual debugging flags are defined in ppp_debug_flags.h.
 * We include that file twice, once to define the flags themselves
 * and once to define the indices that the parser uses.
 */
#include "../wan/ppp_debug_flags.h"
#define __DECLARE_DEBUG_NUMS__
#include "../wan/ppp_debug_flags.h"
