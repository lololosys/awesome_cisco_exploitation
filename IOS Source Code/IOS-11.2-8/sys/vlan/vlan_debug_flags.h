/*--------------------------------------------------------------------------
 *
 * $Id: vlan_debug_flags.h,v 3.2 1996/01/23 08:55:33 mmcneali Exp $
 *
 * vlan/vlan_debug_flags.h
 *
 * April 1995, Martin McNealis.
 *
 * Copyright (c) 1995-1996 by Cisco Systems, Inc.
 *
 * All rights reserved.
 *
 * Debugging flag declarations for Virtual LANs.
 *
 *--------------------------------------------------------------------------
 *
 * $Log: vlan_debug_flags.h,v $
 * Revision 3.2  1996/01/23  08:55:33  mmcneali
 * CSCdi47323:  vLAN encapsulated IP SSE switched.
 *
 * Faster Baby, Faster!
 *
 *
 * $Endlog$
 */


#include "../ui/debug_macros.h"


/* Define the actual flags and the array that points to them */

DEBUG_ARRDECL(vlan_debug_arr)

DEBUG_FLAG(vlan_packet_debug,DEBUG_VLAN_PACKETS,"Virtual LAN packet information")

DEBUG_ARRDONE

