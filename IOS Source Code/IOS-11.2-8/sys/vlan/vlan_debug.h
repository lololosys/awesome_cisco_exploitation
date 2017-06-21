/*--------------------------------------------------------------------------
 *
 * $Id: vlan_debug.h,v 3.2 1996/01/23 08:55:31 mmcneali Exp $
 *
 * vlan/vlan_debug.h
 *
 * April 1995, Martin McNealis.
 *
 * Copyright (c) 1995-1996 by Cisco Systems, Inc.
 *
 * All rights reserved.
 *
 * Debugging routines for Virtual LAN subsystem.
 *
 *--------------------------------------------------------------------------
 *
 * $Log: vlan_debug.h,v $
 * Revision 3.2  1996/01/23  08:55:31  mmcneali
 * CSCdi47323:  vLAN encapsulated IP SSE switched.
 *
 * Faster Baby, Faster!
 *
 *
 * $Endlog$
 */


/*
 * The actual debugging flags are defined in vlan_debug_flags.h.
 * We include that file twice, once to define the flags themselves
 * and once to define the indices that the parser uses.
 */

#include "vlan_debug_flags.h"

#define __DECLARE_DEBUG_NUMS__

#include "vlan_debug_flags.h"

/* Function to fire up vLAN debugging */

extern void vlan_debug_init(void);
