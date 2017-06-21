/* $Id: cdp_debug_flags.h,v 3.2.58.1 1996/04/15 21:13:05 bcole Exp $:
 * $Source: /release/112/cvs/Xsys/cdp/cdp_debug_flags.h,v $
 *------------------------------------------------------------------
 * Debugging flag declarations for CDP
 *
 * October 1994, Arun Sastry
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cdp_debug_flags.h,v $
 * Revision 3.2.58.1  1996/04/15  21:13:05  bcole
 * CSCdi54649:  IP Host Modularity, IP Automatic Stub Routing feature
 * commit
 * Branch: California_branch
 *
 * Revision 3.2  1995/11/17  08:49:47  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:05:12  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:15:03  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "../ui/debug_macros.h"

/* Define the actual flags and the array that points to them */
DEBUG_ARRDECL(cdp_debug_arr)

DEBUG_FLAG(cdp_pkt_debug, DEBUG_CDP_PACKETS,"CDP packet info")
DEBUG_FLAG(cdp_event_debug, DEBUG_CDP_EVENTS, "CDP events")
DEBUG_FLAG(cdp_adj_debug, DEBUG_CDP_ADJ, "CDP neighbor info")
DEBUG_FLAG(cdp_ip_debug, DEBUG_CDP_IP, "CDP IP info")

DEBUG_ARRDONE

