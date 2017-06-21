/* $Id: dlcsw_debug.h,v 3.2 1995/11/17 08:56:01 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/cls/dlcsw_debug.h,v $
 *------------------------------------------------------------------
 * DLC-SW Debug Flags Definition
 *
 * September 1994, Meng Lyu
 *
 * Copyright (c) 1994 by Cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: dlcsw_debug.h,v $
 * Revision 3.2  1995/11/17  08:56:01  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:12:05  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:20:39  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/* Function to fire up DLC-SW debugging */
extern void dlcsw_debug_init(void);

/*
 * The actual debugging flags are defined in dlcsw_debug_flags.h.
 * We include that file twice, once to define the flags themselves
 * and once to define the indices that the parser uses.
 */
#include "../cls/dlcsw_debug_flags.h"
#define __DECLARE_DEBUG_NUMS__
#include "../cls/dlcsw_debug_flags.h"
