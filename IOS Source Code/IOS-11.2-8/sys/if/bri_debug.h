/* $Id: bri_debug.h,v 3.1 1995/11/09 11:47:55 shaker Exp $
 * $Source: /swtal/cherf.111/ios/sys/if/bri_debug.h,v $
 *------------------------------------------------------------------
 * bri_debug.h -- Debugging declarations for BRI.
 *
 * March 1994, Manoj Leelanivas.
 *
 * Copyright (c) 1993-1994 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/* Function to fire up BRI debugging */
extern void bri_debug_init(void);

/*
 * The actual debugging flags are defined in bri_debug_flags.h.
 * We include that file twice, once to define the flags themselves
 * and once to define the indices that the parser uses.
 */
#include "../if/bri_debug_flags.h"
#define __DECLARE_DEBUG_NUMS__
#include "../if/bri_debug_flags.h"


