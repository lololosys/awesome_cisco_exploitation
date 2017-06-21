/* $Id: clns_debug.h,v 3.2 1995/11/17 08:51:55 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/clns/clns_debug.h,v $
 *------------------------------------------------------------------
 * clns_debug.h -- Debugging declarations for CLNS
 *
 * May 1993, John Bashinski
 *
 * Copyright (c) 1993-1994 by Cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: clns_debug.h,v $
 * Revision 3.2  1995/11/17  08:51:55  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:07:28  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:17:00  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "isis_nlsp_debug.h"

/* Function to fire up CLNS debugging */
extern void clns_debug_init(void);

/*
 * The actual debugging flags are defined in clns_debug_flags.h.
 * We include that file twice, once to define the flags themselves
 * and once to define the indices that the parser uses.
 */
#include "../clns/clns_debug_flags.h"
#define __DECLARE_DEBUG_NUMS__
#include "../clns/clns_debug_flags.h"
