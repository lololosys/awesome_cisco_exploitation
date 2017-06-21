/* $Id: fddi_debug.h,v 3.2 1995/11/17 09:27:43 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/if/fddi_debug.h,v $
 *------------------------------------------------------------------
 * fddi_debug.h -- Debugging declarations for FDDI
 *
 * May 1993, John Bashinski
 *
 * Copyright (c) 1993-1994 by Cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: fddi_debug.h,v $
 * Revision 3.2  1995/11/17  09:27:43  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:49:49  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:52:19  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/* Function to fire up FDDI debugging */
extern void fddi_debug_init(void);

/*
 * The actual debugging flags are defined in fddi_debug_flags.h.
 * We include that file twice, once to define the flags themselves
 * and once to define the indices that the parser uses.
 */
#include "../if/fddi_debug_flags.h"
#define __DECLARE_DEBUG_NUMS__
#include "../if/fddi_debug_flags.h"
