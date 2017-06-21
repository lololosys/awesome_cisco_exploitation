/* $Id: novell_debug.h,v 3.2 1995/11/17 19:23:06 gstovall Exp $
 * $Source: /swtal/cherf.111/ios/sys/xns/novell_debug.h,v $
 *------------------------------------------------------------------
 * novell_debug.h -- Debugging declarations for Novell IPX
 *
 * May 1993, John Bashinski
 *
 * Copyright (c) 1993-1994 by Cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: novell_debug.h,v $
 * Revision 3.2  1995/11/17  19:23:06  gstovall
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:59:03  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:28:38  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "../clns/isis_nlsp_debug.h"

/* Function to fire up Novell debugging */
extern void novell_debug_init(void);

/*
 * The actual debugging flags are defined in novell_debug_flags.h.
 * We include that file twice, once to define the flags themselves
 * and once to define the indices that the parser uses.
 */
#include "../xns/novell_debug_flags.h"
#define __DECLARE_DEBUG_NUMS__
#include "../xns/novell_debug_flags.h"
