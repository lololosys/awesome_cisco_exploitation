/* $Id: atmsig_debug_flags.h,v 3.2 1995/11/17 08:46:10 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/atm/atmsig_debug_flags.h,v $
 *------------------------------------------------------------------
 * if/atmsig_debug.h: DEBUG command specific vars. & constants
 *
 * November, 1993,   Percy P. Khabardar 
 *
 * Copyright (c) 1993-1995 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Longer description here.
 *------------------------------------------------------------------
 * $Log: atmsig_debug_flags.h,v $
 * Revision 3.2  1995/11/17  08:46:10  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  10:59:56  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:11:48  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "../ui/debug_macros.h"

/* Now define the actual flags and the array that points to them */
DEBUG_ARRDECL(atmsig_debug_arr)

DEBUG_FLAG(atmSig_debugAll, DEBUG_ATMSIG_ALL, "ATM Signalling All")
DEBUG_FLAG(atmSig_debugIe, DEBUG_ATMSIG_IE, "ATM Signalling Info Elements")
DEBUG_FLAG(atmSig_debugError, DEBUG_ATMSIG_ERROR, "ATM Signalling Error")
DEBUG_FLAG(atmSig_debugPacket, DEBUG_ATMSIG_PAK, "ATM Signalling Packets")
DEBUG_FLAG(atmSig_debugEvent, DEBUG_ATMSIG_EVENT, "ATM Signalling Events")
DEBUG_FLAG(atmSmap_debugError, DEBUG_ATMSIG_SMAP_ER, 
                                       "ATM Signalling Static Map Errors")
DEBUG_FLAG(atmSmap_debugEvent, DEBUG_ATMSIG_SMAP_EV, 
                                       "ATM Signalling Static Map Events")
DEBUG_FLAG(atmSmap_debugAll, DEBUG_ATMSIG_SMAP_ALL,
                                       "ATM Signalling Static Map All")

DEBUG_ARRDONE
