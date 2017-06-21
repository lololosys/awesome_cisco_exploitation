/* $Id: sscop_debug_flags.h,v 3.2.60.1 1996/07/29 23:38:50 jwjang Exp $
 * $Source: /release/112/cvs/Xsys/atm/sscop_debug_flags.h,v $
 *------------------------------------------------------------------
 * atm/sscop_debug_flags.h: DEBUG command specific vars. & constants
 *
 * November 1993,   Percy P. Khabardar 
 *
 * Copyright (c) 1996-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Longer description here.
 *------------------------------------------------------------------
 * $Log: sscop_debug_flags.h,v $
 * Revision 3.2.60.1  1996/07/29  23:38:50  jwjang
 * CSCdi62882:  SSCOP: when vp-tunnel is shutdown the peers SSCOP remains
 * active
 * Branch: California_branch
 *
 * Revision 3.2  1995/11/17  08:47:24  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:01:18  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:13:02  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "../ui/debug_macros.h"

/* Now define the actual flags and the array that points to them */
DEBUG_ARRDECL(sscop_debug_arr)

DEBUG_FLAG(sscop_debugSuspend, DEBUG_SSCOP_SUSPEND, "SSCOP Suspend")
DEBUG_FLAG(sscop_debugError, DEBUG_SSCOP_ERROR, "SSCOP Error")
DEBUG_FLAG(sscop_debugPacket, DEBUG_SSCOP_PAK, "SSCOP Packets")
DEBUG_FLAG(sscop_debugEvent, DEBUG_SSCOP_EVENT, "SSCOP Events")

DEBUG_ARRDONE
