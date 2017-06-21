/* $Id: comp_debug.h,v 3.2 1995/11/17 18:05:50 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/x25/comp_debug.h,v $
 *------------------------------------------------------------------
 * COMP_DEBUG.H
 *
 * 10/29/93 Bill Thomas
 *
 * Copyright (c) 1993-1994 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: comp_debug.h,v $
 * Revision 3.2  1995/11/17  18:05:50  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:52:05  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:20:39  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/* Function to fire up COMPRESS debugging */
extern void compress_debug_init(void);

/*
 * The actual debugging flags are defined in comp_debug_flags.h.
 * We include that file twice, once to define the flags themselves
 * and once to define the indices that the parser uses.
 */
#include "../x25/comp_debug_flags.h"
#define __DECLARE_DEBUG_NUMS__
#include "../x25/comp_debug_flags.h"
