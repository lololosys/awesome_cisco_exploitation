/* $Id: qllc_debug_flags.h,v 3.2.62.2 1996/05/23 06:42:23 pmorton Exp $
 * $Source: /release/112/cvs/Xsys/srt/qllc_debug_flags.h,v $
 *------------------------------------------------------------------
 * QLLC: define debug flags
 *
 * April, 1994          Phil Morton
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: qllc_debug_flags.h,v $
 * Revision 3.2.62.2  1996/05/23  06:42:23  pmorton
 * CSCdi58502:  Fix for CSCdi42369 broke FRAD image
 * Branch: California_branch
 * Use 'debug sdlc qllc' instead of 'debug qllc sdlc'
 *
 * Revision 3.2.62.1  1996/05/22  05:00:50  pmorton
 * CSCdi42369:  sdlc-qllc conversion, no auto-reconnect
 * Branch: California_branch
 * 1. When qllc link state changes propagate to SDLC-QLLC conversion (SNX)
 * 2. Add 'debug qllc sdlc' to track this
 * 3. Correct SNX state names
 *
 * Revision 3.2  1995/11/17  18:55:14  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:31:35  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:01:08  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#include "../ui/debug_macros.h"

/* Now define the actual flags and the array that points to them */
DEBUG_ARRDECL(qllc_debug_arr)

DEBUG_FLAG(lnx_debug,DEBUG_LNX,"qllc to llc/sdlc conversion")
DEBUG_FLAG(qllc_test_debug,DEBUG_QLLC_TEST,"qllc test special images")
DEBUG_FLAG(lnx_error_debug,DEBUG_QLLC_ERROR,"qllc conversion error")
DEBUG_FLAG(qllc_state_debug,DEBUG_QLLC_STATE,"qllc state transitions")
DEBUG_FLAG(qllc_event_debug,DEBUG_QLLC_EVENT,"qllc event")
DEBUG_FLAG(qllc_packet_debug,DEBUG_QLLC_PACKET,"qllc packet")
DEBUG_FLAG(qllc_timer_debug,DEBUG_QLLC_TIMER,"qllc timer")
DEBUG_FLAG(qllc_x25_debug,DEBUG_QLLC_X25,"qllc x.25 events")

DEBUG_ARRDONE
