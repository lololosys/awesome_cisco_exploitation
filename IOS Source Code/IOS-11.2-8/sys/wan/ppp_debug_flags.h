/* $Id: ppp_debug_flags.h,v 3.3.32.2 1996/07/30 21:01:50 dblair Exp $
 * $Source: /release/112/cvs/Xsys/wan/ppp_debug_flags.h,v $
 *------------------------------------------------------------------
 * ppp_debug_flags.h -- Debugging flag declarations for PPP
 *
 * May 1993, John Bashinski
 *
 * Copyright (c) 1993-1996 by Cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ppp_debug_flags.h,v $
 * Revision 3.3.32.2  1996/07/30  21:01:50  dblair
 * CSCdi57980:  debug ppp multilink is too busy
 * Branch: California_branch
 * Clean up multilink debugs
 *
 * Revision 3.3.32.1  1996/04/27  06:37:57  syiu
 * Branch: California_branch
 * Commit Dial1_branch
 *
 * Revision 3.3.2.1  1996/01/16  15:45:31  dblair
 * CSCdi42375:  Move Multilink PPP into PPP subsystem
 * Branch: Dial1_branch
 *
 * Revision 3.3  1996/01/11  10:18:34  billw
 * CSCdi46705:  debug ppp chap should be debug ppp authentication
 * Just do it.  Retain "hidden" "debug ppp chap" for now.
 *
 * Revision 3.2  1995/11/17  18:04:17  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:50:45  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/09/20  22:39:22  dblair
 * CSCdi40745:  PPP Multilink feature commit
 *
 * Revision 2.1  1995/06/07  23:19:20  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * We get included (multiple times) from PPP code to define
 * debugging flags. For this, we use some macros that get defined
 * by debug_macros.h. What definitions the macros get depends on
 * which magic flags have been set by the file that includes us.
 */
#include "../ui/debug_macros.h"

/* Now define the actual flags and the array that points to them */
DEBUG_ARRDECL(ppp_debug_arr)

DEBUG_FLAG(ppp_chap_debug,DEBUG_PPP_CHAP,"PPP authentication")
DEBUG_FLAG(ppp_error_debug,DEBUG_PPP_ERROR,"PPP protocol errors")
DEBUG_FLAG(ppp_negotiation_debug,DEBUG_PPP_NEGOTIATION,"PPP protocol negotiation")
DEBUG_FLAG(ppp_packet_debug,DEBUG_PPP_PACKET,"PPP packet display")
DEBUG_FLAG(multilink_debug,DEBUG_MULTILINK,"Multilink activity")
DEBUG_FLAG(multilink_detailed_debug,DEBUG_MULTILINK_DETAILED,
	   "Detailed multilink activity")
DEBUG_FLAG(multilink_events_debug, DEBUG_MULTILINK_EVENTS, "Multilink events")
DEBUG_FLAG(multilink_data_debug, DEBUG_MULTILINK_DATA, "First bytes of multilink packet")

DEBUG_ARRDONE
