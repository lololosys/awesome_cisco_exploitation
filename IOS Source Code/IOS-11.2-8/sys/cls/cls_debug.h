/* $Id: cls_debug.h,v 3.2 1995/11/17 08:54:45 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/cls/cls_debug.h,v $
 *------------------------------------------------------------------
 * cls_debug.h -- Debugging declarations for CLS
 *
 * April 1993, Steve Berl
 *
 * Copyright (c) 1994 by Cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cls_debug.h,v $
 * Revision 3.2  1995/11/17  08:54:45  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:10:43  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:19:32  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

extern void cls_debug_init(void);	/* Function to fire up CLS debugging */

/*
 * The actual debugging flags are defined in cls_debug_flags.h.
 * We include that file twice, once to define the flags themselves
 * and once to define the indices that the parser uses.
 */
#include "../cls/cls_debug_flags.h"
#define __DECLARE_DEBUG_NUMS__
#include "../cls/cls_debug_flags.h"
