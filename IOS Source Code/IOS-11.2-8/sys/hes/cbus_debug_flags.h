/* $Id: cbus_debug_flags.h,v 3.3 1995/11/27 21:31:46 yanke Exp $
 * $Source: /swtal/cherf.111/ios/sys/hes/cbus_debug_flags.h,v $
 *------------------------------------------------------------------
 * cbus_debug_flags.h -- Debugging flag declarations for cBus
 *
 * May 1993, John Bashinski
 *
 * Copyright (c) 1993-1995 by Cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cbus_debug_flags.h,v $
 * Revision 3.3  1995/11/27  21:31:46  yanke
 * Add 'debug cbus monitor' command, add cbus protection
 * registry functions.
 *
 * CSCdi44478:  slave rsp crashes or resets during ips eoir events
 *
 * Revision 3.2  1995/11/17  09:14:49  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:37:32  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/07/13  01:20:28  fsunaval
 * CSCdi36847:  FCICMDFAILs put under debug control (debug cbus
 * cmd-timeout)
 *
 * Revision 2.1  1995/06/07  20:38:06  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * We get included (multiple times) from cBus code to define
 * debugging flags. For this, we use some macros that get defined
 * by debug_macros.h. What definitions the macros get depends on
 * which magic flags have been set by the file that includes us.
 */
#include "../ui/debug_macros.h"

/* Now define the actual flags and the array that points to them */
DEBUG_ARRDECL(cbus_debug_arr)

DEBUG_FLAG(cbus_debug,DEBUG_CBUS,"ciscoBus events")
DEBUG_FLAG(cbus_cmdretry_debug,DEBUG_CBUS_CMDRETRY,"ciscoBus command retries")
DEBUG_FLAG(cbus_cmdtimeout_debug,DEBUG_CBUS_CMDTIMEOUT,"ciscoBus command time outs")
DEBUG_FLAG(cbus_monitor_debug,DEBUG_CBUS_MONITOR,"ciscoBus monitor")

DEBUG_ARRDONE
