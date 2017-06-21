/* $Id: ts_debug_flags.h,v 3.3 1996/02/23 04:12:08 tkolar Exp $
 * $Source: /release/111/cvs/Xsys/ts/ts_debug_flags.h,v $
 *------------------------------------------------------------------
 * ts_debug_flags.h -- Debugging flag declarations for TS
 *
 * May 1993, John Bashinski
 *
 * Copyright (c) 1993-1996 by Cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ts_debug_flags.h,v $
 * Revision 3.3  1996/02/23  04:12:08  tkolar
 * CSCdi49648:  Modem autoconfig break modularity rules
 * Move confmodem_debug out of the commserver subsystem.
 *
 * Revision 3.2  1995/11/17  18:56:29  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:40:15  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:33:33  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/06/07  23:10:29  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * We get included (multiple times) from TS code to define
 * debugging flags. For this, we use some macros that get defined
 * by debug_macros.h. What definitions the macros get depends on
 * which magic flags have been set by the file that includes us.
 */
#include "../ui/debug_macros.h"

/* Now define the actual flags and the array that points to them */
DEBUG_ARRDECL(ts_debug_arr)

DEBUG_FLAG(termqueue_debug,DEBUG_ENTRY,"Incoming queue entry")

DEBUG_ARRDONE
