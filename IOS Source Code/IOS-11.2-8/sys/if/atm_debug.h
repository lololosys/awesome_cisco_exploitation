/* $Id: atm_debug.h,v 3.2 1995/11/17 09:25:50 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/if/atm_debug.h,v $
 *------------------------------------------------------------------
 * if/atm_debug.h: DEBUG command specific vars. & constants
 *
 * November, 1993, Rick Li Fo Sjoe
 *
 * Copyright (c) 1993-1994 by cisco Systems, Inc.
 * All rights reserved.
 * 
 * Longer description here.
 *------------------------------------------------------------------
 * $Log: atm_debug.h,v $
 * Revision 3.2  1995/11/17  09:25:50  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:47:49  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:50:41  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/* Function to fire up AIP debugging */
extern void atm_debug_init(void);

/*
 * The actual debugging flags are defined in atm_debug_flags.h.
 * We include that file twice, once to define the flags themselves
 * and once to define the indices that the parser uses.
 */
#include "../if/atm_debug_flags.h"
#define __DECLARE_DEBUG_NUMS__
#include "../if/atm_debug_flags.h"
