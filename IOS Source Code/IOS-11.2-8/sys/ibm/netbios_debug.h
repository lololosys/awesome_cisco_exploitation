/* $Id: netbios_debug.h,v 3.2 1995/11/17 09:23:29 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/ibm/netbios_debug.h,v $
 *------------------------------------------------------------------
 * netbios_debug.h -- Debugging declarations for NETBIOS
 *
 * May 1993, John Bashinski
 *
 * Copyright (c) 1993-1994 by Cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: netbios_debug.h,v $
 * Revision 3.2  1995/11/17  09:23:29  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:45:45  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:47:37  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/* Function to fire up NETBIOS debugging */
extern void netbios_debug_init(void);

/*
 * The actual debugging flags are defined in netbios_debug_flags.h.
 * We include that file twice, once to define the flags themselves
 * and once to define the indices that the parser uses.
 */
#include "../ibm/netbios_debug_flags.h"
#define __DECLARE_DEBUG_NUMS__
#include "../ibm/netbios_debug_flags.h"
