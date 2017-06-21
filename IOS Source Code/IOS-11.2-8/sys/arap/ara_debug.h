/* $Id: ara_debug.h,v 3.2 1995/11/17 08:38:08 hampton Exp $
 * $Source: /vws/aam/cherf.111/ios/sys/arap/ara_debug.h,v $
 *------------------------------------------------------------------
 * ara_debug.h -- Debugging declarations for ARA
 *
 * September 1993, Tim Kolar
 *
 * Copyright (c) 1993-1994 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ara_debug.h,v $
 * Revision 3.2  1995/11/17  08:38:08  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  10:53:11  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:04:10  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/* Function to fire up ARA debugging */
extern void ara_debug_init(void);

/*
 * The actual debugging flags are defined in ara_debug_flags.h.
 * We include that file twice, once to define the flags themselves
 * and once to define the indices that the parser uses.
 */
#include "../arap/ara_debug_flags.h"
#define __DECLARE_DEBUG_NUMS__
#include "../arap/ara_debug_flags.h"
