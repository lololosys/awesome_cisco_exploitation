/* $Id: bstun_debug_flags.h,v 3.2 1995/11/17 08:49:04 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/bstun/bstun_debug_flags.h,v $
 *------------------------------------------------------------------
 * bstun_debug_flags.h -- Debugging flag declarations for BSTUN
 *
 * August 1994, James Balestriere
 *
 * Copyright (c) 1994-1995 by cisco Systems, Inc.
 * Prepared by Metaplex.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: bstun_debug_flags.h,v $
 * Revision 3.2  1995/11/17  08:49:04  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:04:23  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:14:27  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "../ui/debug_macros.h"

DEBUG_ARRDECL(bstun_debug_arr)

DEBUG_FLAG(bstun_debug,DEBUG_BSTUN,"BSTUN: Block Serial Tunnel")
DEBUG_FLAG(bstun_debug_event,DEBUG_BSTUN_EVENT,"BSTUN Events")
DEBUG_FLAG(bstun_debug_packet,DEBUG_BSTUN_PACKET,"BSTUN Packet")

DEBUG_ARRDONE

