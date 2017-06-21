/* $Id: brut_debug.h,v 3.2 1995/11/17 17:36:34 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/les/brut_debug.h,v $
 *------------------------------------------------------------------
 * brut_debug.h -- Debugging declarations for BRUT
 *
 * May 1993, John Bashinski
 *
 * Copyright (c) 1993-1994 by Cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: brut_debug.h,v $
 * Revision 3.2  1995/11/17  17:36:34  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:29:28  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:30:36  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/* Function to fire up BRUT debugging */
extern void brut_debug_init(void);

/*
 * The actual debugging flags are defined in brut_debug_flags.h.
 * We include that file twice, once to define the flags themselves
 * and once to define the indices that the parser uses.
 */
#include "../les/brut_debug_flags.h"
#define __DECLARE_DEBUG_NUMS__
#include "../les/brut_debug_flags.h"


