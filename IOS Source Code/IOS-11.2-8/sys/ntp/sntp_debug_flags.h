/* $Id: sntp_debug_flags.h,v 3.1.2.2 1996/06/25 22:55:31 dkatz Exp $
 * $Source: /release/112/cvs/Xsys/ntp/sntp_debug_flags.h,v $
 *------------------------------------------------------------------
 * Debug flags for SNTP
 *
 * June 1996, Dave Katz
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: sntp_debug_flags.h,v $
 * Revision 3.1.2.2  1996/06/25  22:55:31  dkatz
 * CSCdi61119:  Commit SNTP Code
 * Branch: California_branch
 * Add rudimentary clock selection logic.  Get rid of most static storage.
 * Keep track of all broadcast servers.
 *
 * Revision 3.1.2.1  1996/06/23  04:40:26  dkatz
 * CSCdi61119:  Commit SNTP Code
 * Branch: California_branch
 * Add an SNTP client.  This is not included in shipping IOS images.
 *
 * Revision 3.1  1996/06/22  21:14:56  dkatz
 * *** empty log message ***
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/*
 * We get included (multiple times) from SNTP code to define
 * debugging flags. For this, we use some macros that get defined
 * by debug_macros.h. What definitions the macros get depends on
 * which magic flags have been set by the file that includes us.
 */
#include "../ui/debug_macros.h"

/* Now define the actual flags and the array that points to them */
DEBUG_ARRDECL(sntp_debug_arr)

DEBUG_FLAG(sntp_clockadjust_debug,DEBUG_SNTP_ADJUST,"SNTP clock adjustments")
DEBUG_FLAG(sntp_packets_debug,DEBUG_SNTP_PACKETS,"SNTP packets")
DEBUG_FLAG(sntp_select_debug,DEBUG_SNTP_SELECT,"SNTP server selection")

DEBUG_ARRDONE
