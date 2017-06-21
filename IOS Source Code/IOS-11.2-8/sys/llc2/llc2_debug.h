/* $Id: llc2_debug.h,v 3.2 1995/11/17 17:43:58 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/llc2/llc2_debug.h,v $
 *------------------------------------------------------------------
 * llc2_debug.h -- Debugging declarations for LLC2
 *
 * May 1993, John Bashinski
 *
 * Copyright (c) 1993-1994 by Cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: llc2_debug.h,v $
 * Revision 3.2  1995/11/17  17:43:58  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:35:38  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:41:57  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/* Function to fire up LLC2 debugging */
extern void llc2_debug_init(void);

/*
 * The actual debugging flags are defined in llc2_debug_flags.h.
 * We include that file twice, once to define the flags themselves
 * and once to define the indices that the parser uses.
 */
#include "../llc2/llc2_debug_flags.h"
#define __DECLARE_DEBUG_NUMS__
#include "../llc2/llc2_debug_flags.h"
