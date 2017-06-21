/* $Id: ntp_debug_flags.h,v 3.2.62.1 1996/04/16 19:03:11 rchandra Exp $
 * $Source: /release/112/cvs/Xsys/ntp/ntp_debug_flags.h,v $
 *------------------------------------------------------------------
 * ntp_debug_flags.h -- Debugging flag declarations for NTP
 *
 * May 1993, John Bashinski
 *
 * Copyright (c) 1993-1996 by Cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ntp_debug_flags.h,v $
 * Revision 3.2.62.1  1996/04/16  19:03:11  rchandra
 * CSCdi54830:  IP features commit
 * Branch: California_branch
 *
 * Revision 3.2  1995/11/17  17:51:03  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:41:05  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:45:12  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * We get included (multiple times) from NTP code to define
 * debugging flags. For this, we use some macros that get defined
 * by debug_macros.h. What definitions the macros get depends on
 * which magic flags have been set by the file that includes us.
 */
#include "../ui/debug_macros.h"

/* Now define the actual flags and the array that points to them */
DEBUG_ARRDECL(ntp_debug_arr)

DEBUG_FLAG(ntp_clockadjust_debug,DEBUG_NTP_ADJUST,"NTP clock adjustments")
DEBUG_FLAG(ntp_clockparms_debug,DEBUG_NTP_PARAMS,"NTP clock parameters")
DEBUG_FLAG(ntp_event_debug,DEBUG_NTP_EVENT,"NTP events")
DEBUG_FLAG(ntp_loopfilter_debug,DEBUG_NTP_LOOPFILTER,"NTP loop filter")
DEBUG_FLAG(ntp_packets_debug,DEBUG_NTP_PACKETS,"NTP packets")
DEBUG_FLAG(ntp_sync_debug,DEBUG_NTP_SYNC,"NTP clock synchronization")
DEBUG_FLAG(ntp_select_debug, DEBUG_NTP_SELECT, "NTP clock selection")
DEBUG_FLAG(ntp_validity_debug,DEBUG_NTP_VALIDITY,"NTP peer validity")
DEBUG_FLAG(ntp_refclock_debug,DEBUG_NTP_REFCLOCK,"NTP reference clocks")

DEBUG_ARRDONE
