/* $Id: pcbus_debug.h,v 3.3 1995/11/17 17:42:26 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/les/pcbus_debug.h,v $
 *------------------------------------------------------------------
 * pcbus_debug.h -- Debugging declarations for Generic pcbus
 *
 * April 1994, Johnny Chan
 *
 * Copyright (c) 1994-1995 by Cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: pcbus_debug.h,v $
 * Revision 3.3  1995/11/17  17:42:26  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:00:40  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:34:18  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:39:34  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 * $Endlog$
 */

/*
 * The actual debugging flags are defined in pcbus_debug_flags.h.
 * We include that file twice, once to define the flags themselves
 * and once to define the indices that the parser uses.
 */
#include "../les/pcbus_debug_flags.h"
#define __DECLARE_DEBUG_NUMS__
#include "../les/pcbus_debug_flags.h"
