/* $Id: llc2_debug_flags.h,v 3.3 1996/02/28 01:02:54 kchiu Exp $
 * $Source: /release/111/cvs/Xsys/llc2/llc2_debug_flags.h,v $
 *------------------------------------------------------------------
 * llc2_debug_flags.h -- Debugging flag declarations for LLC2
 *
 * May 1993, John Bashinski
 *
 * Copyright (c) 1993-1996 by Cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: llc2_debug_flags.h,v $
 * Revision 3.3  1996/02/28  01:02:54  kchiu
 * CSCdi48822:  llc2 windowing mechanism doesn't work properly under
 * congestion
 *
 * Revision 3.2  1995/11/17  17:44:00  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:35:39  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:42:01  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * We get included (multiple times) from LLC2 code to define
 * debugging flags. For this, we use some macros that get defined
 * by debug_macros.h. What definitions the macros get depends on
 * which magic flags have been set by the file that includes us.
 */
#include "../ui/debug_macros.h"

/* Now define the actual flags and the array that points to them */
DEBUG_ARRDECL(llc2_debug_arr)

DEBUG_FLAG(llc2_debug_err,DEBUG_LLC2_ERROR,"LLC2 Errors")
DEBUG_FLAG(llc2_debug_packet,DEBUG_LLC2_PACKET,"LLC2 Packets")
DEBUG_FLAG(llc2_debug_state,DEBUG_LLC2_STATE,"LLC2 State")
DEBUG_FLAG(llc2_debug_dynwind,DEBUG_LLC2_DYNWIND,"LLC2 (FRAS) dynamic window")
DEBUG_FLAG(llc2_debug_test,DEBUG_LLC2_TEST,"LLC2 Internal Testing")

DEBUG_ARRDONE
