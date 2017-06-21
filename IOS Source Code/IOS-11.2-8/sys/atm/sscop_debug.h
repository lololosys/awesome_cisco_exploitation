/* $Id: sscop_debug.h,v 3.2 1995/11/17 08:47:22 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/atm/sscop_debug.h,v $
 *------------------------------------------------------------------
 * atm/sscop_debug.h: DEBUG command specific vars. & constants
 *
 * November, 1993,  Percy P. Khabardar 
 *
 * Copyright (c) 1993-1994 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Longer description here.
 *------------------------------------------------------------------
 * $Log: sscop_debug.h,v $
 * Revision 3.2  1995/11/17  08:47:22  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:01:17  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:13:00  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/* Function to fire up SSCOP debugging */
extern void sscop_debug_init(void);

/*
 * The actual debugging flags are defined in sscop_debug_flags.h.
 * We include that file twice, once to define the flags themselves
 * and once to define the indices that the parser uses.
 */
#include "../atm/sscop_debug_flags.h"
#define __DECLARE_DEBUG_NUMS__
#include "../atm/sscop_debug_flags.h"
