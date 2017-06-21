/* $Id: atm_dxi_debug.h,v 3.2 1995/11/17 18:00:36 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/wan/atm_dxi_debug.h,v $
 *------------------------------------------------------------------
 * Support for ATM-DXI encapsulation and RFC1483 support
 *
 * October 1994, Shankar Natarajan
 *
 * Copyright (c) 1994 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: atm_dxi_debug.h,v $
 * Revision 3.2  1995/11/17  18:00:36  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:47:41  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:16:51  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
/* Function to fire up atm-dxi debugging */
extern void atm_dxi_debug_init(void);

/*
 * The actual debugging flags are defined in atm_dxi_debug_flags.h.
 * We include that file twice, once to define the flags themselves
 * and once to define the indices that the parser uses.
 */
#include "../wan/atm_dxi_debug_flags.h"
#define __DECLARE_DEBUG_NUMS__
#include "../wan/atm_dxi_debug_flags.h"
