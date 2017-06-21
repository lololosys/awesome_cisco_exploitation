/* $Id: hub_debug.h,v 3.2 1995/11/17 17:37:47 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/les/hub_debug.h,v $
 *------------------------------------------------------------------
 * hub_debug.h -- Debugging declarations for Hub repeater
 *
 * September 1994, Viet Nguyen
 *
 * Copyright (c) 1994 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Gurus: Viet Nguyen
 *------------------------------------------------------------------
 * $Log: hub_debug.h,v $
 * Revision 3.2  1995/11/17  17:37:47  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:30:34  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:31:54  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/*
 * Prevent multiple inclusion.
 */
#ifndef __HUB_DEBUG_H__
#define __HUB_DEBUG_H__

/* Function to fire up hub debugging */
extern void hub_debug_init(void);
extern void hub_parser_init(void);

/*
 * The actual debugging flags are defined in hub_debug_flags.h.
 * We include that file twice, once to define the flags themselves
 * and once to define the indices that the parser uses.
 */
#include "../les/hub_debug_flags.h"
#define __DECLARE_DEBUG_NUMS__
#include "../les/hub_debug_flags.h"

#endif
