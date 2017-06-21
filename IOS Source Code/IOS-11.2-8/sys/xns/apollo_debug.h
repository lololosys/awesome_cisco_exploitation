/* $Id: apollo_debug.h,v 3.2 1995/11/17 19:20:37 gstovall Exp $
 * $Source: /swtal/cherf.111/ios/sys/xns/apollo_debug.h,v $
 *------------------------------------------------------------------
 * apollo_debug.h -- Debugging declarations for Apollo Domain
 *
 * May 1993, John Bashinski
 *
 * Copyright (c) 1993-1994 by Cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: apollo_debug.h,v $
 * Revision 3.2  1995/11/17  19:20:37  gstovall
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:57:01  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:26:45  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/* Function to fire up Apollo debugging */
extern void apollo_debug_init(void);

/*
 * The actual debugging flags are defined in apollo_debug_flags.h.
 * We include that file twice, once to define the flags themselves
 * and once to define the indices that the parser uses.
 */
#include "../xns/apollo_debug_flags.h"
#define __DECLARE_DEBUG_NUMS__
#include "../xns/apollo_debug_flags.h"
