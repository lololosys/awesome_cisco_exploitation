/* $Id: bsc_debug_flags.h,v 3.2 1995/11/17 08:48:14 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/bstun/bsc_debug_flags.h,v $
 *------------------------------------------------------------------
 * bsc_debug_flags.h - BSC Debug flags
 *
 * August 1994, James Balestriere
 *
 * Copyright (c) 1994-1995,1997 by cisco Systems, Inc.
 * Prepared by Metaplex.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: bsc_debug_flags.h,v $
 * Revision 3.2  1995/11/17  08:48:14  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:03:32  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:13:40  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "../ui/debug_macros.h"


DEBUG_ARRDECL(bsc_debug_arr)

DEBUG_FLAG(bsc_debug_event,DEBUG_BSC_EVENT,"BSC Event")
DEBUG_FLAG(bsc_debug_packet,DEBUG_BSC_PACKET,"BSC Packet")

DEBUG_ARRDONE
