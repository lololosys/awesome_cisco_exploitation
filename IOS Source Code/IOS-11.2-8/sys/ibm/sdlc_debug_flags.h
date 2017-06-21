/* $Id: sdlc_debug_flags.h,v 3.2.60.1 1996/05/23 06:42:44 pmorton Exp $
 * $Source: /release/112/cvs/Xsys/ibm/sdlc_debug_flags.h,v $
 *------------------------------------------------------------------
 * sdlc_debug_flags.h -- Debugging flag declarations for SDLC
 *
 * May 1993, John Bashinski
 *
 * Copyright (c) 1993-1996 by Cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: sdlc_debug_flags.h,v $
 * Revision 3.2.60.1  1996/05/23  06:42:44  pmorton
 * CSCdi58502:  Fix for CSCdi42369 broke FRAD image
 * Branch: California_branch
 * Use 'debug sdlc qllc' instead of 'debug qllc sdlc'
 *
 * Revision 3.2  1995/11/17  09:23:53  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:46:06  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:48:09  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * We get included (multiple times) from SDLC code to define
 * debugging flags. For this, we use some macros that get defined
 * by debug_macros.h. What definitions the macros get depends on
 * which magic flags have been set by the file that includes us.
 */
#include "../ui/debug_macros.h"

/* Now define the actual flags and the array that points to them */
DEBUG_ARRDECL(sdlc_debug_arr)

DEBUG_FLAG(sdlc_debug,DEBUG_SDLC,"SDLC link")
DEBUG_FLAG(sdllc_debug,DEBUG_SDLLC,"SDLLC media translation")
DEBUG_FLAG(stun_debug,DEBUG_STUN,"Serial TUNnel")
DEBUG_FLAG(sdlc_qllc_debug, DEBUG_SDLC_QLLC, "SDLC QLLC direct conversion")
DEBUG_ARRDONE
