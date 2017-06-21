/* $Id: xns_debug.h,v 3.2 1995/11/17 19:25:27 gstovall Exp $
 * $Source: /swtal/cherf.111/ios/sys/xns/xns_debug.h,v $
 *------------------------------------------------------------------
 * xns_debug.h -- Debugging declarations for XNS
 *
 * May 1993, John Bashinski
 *
 * Copyright (c) 1993-1995 by Cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: xns_debug.h,v $
 * Revision 3.2  1995/11/17  19:25:27  gstovall
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  14:00:43  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:30:40  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef	__XNS_DEBUG_H__
#define	__XNS_DEBUG_H__

/* Function to fire up XNS debugging */
extern void xns_debug_init(void);

/*
 * The actual debugging flags are defined in xns_debug_flags.h.
 * We include that file twice, once to define the flags themselves
 * and once to define the indices that the parser uses.
 */
#include "../xns/xns_debug_flags.h"
#define __DECLARE_DEBUG_NUMS__
#include "../xns/xns_debug_flags.h"

#endif
