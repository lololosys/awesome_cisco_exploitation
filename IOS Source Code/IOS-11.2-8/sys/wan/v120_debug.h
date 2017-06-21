/* $Id: v120_debug.h,v 3.2 1995/11/17 18:05:22 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/wan/v120_debug.h,v $
 *------------------------------------------------------------------
 * v120_debug.h -- Debugging declarations for V120
 *
 * July, 1995, Bill May
 *
 * Copyright (c) 1995 by Cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: v120_debug.h,v $
 * Revision 3.2  1995/11/17  18:05:22  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:51:40  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:37:04  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/09/21  23:30:30  wmay
 * Placeholder for CS_Arkansas_branch
 *------------------------------------------------------------------
 * $Endlog$
 */
/* Function to fire up v120 debugging */
extern void v120_debug_init(void);

/*
 * The actual debugging flags are defined in v120_debug_flags.h.
 * We include that file twice, once to define the flags themselves
 * and once to define the indices that the parser uses.
 */
#include "../wan/v120_debug_flags.h"
#define __DECLARE_DEBUG_NUMS__
#include "../wan/v120_debug_flags.h"
