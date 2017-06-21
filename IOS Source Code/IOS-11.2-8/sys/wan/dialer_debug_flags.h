/* $Id: dialer_debug_flags.h,v 3.3.54.2 1996/07/19 19:50:32 wmay Exp $
 * $Source: /release/112/cvs/Xsys/wan/dialer_debug_flags.h,v $
 *------------------------------------------------------------------
 * dialer_debug_flags.h -- Debugging flag declarations for dialer
 *
 * May 1993, John Bashinski
 *
 * Copyright (c) 1993-1996 by Cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: dialer_debug_flags.h,v $
 * Revision 3.3.54.2  1996/07/19  19:50:32  wmay
 * CSCdi60728:  ISDN dialer holdQ not handled correctly - do not allocate
 * holdq until needed, and pass the holdq correctly as calls and callbacks
 * are made.  Redo a little callback code to deal with the holdq better and
 * rename some of the structures.  Add a debug dialer holdq (hidden).
 * Branch: California_branch
 *
 * Revision 3.3.54.1  1996/04/27  06:36:11  syiu
 * Branch: California_branch
 * Commit Dial1_branch
 *
 * Revision 3.3.16.1  1996/02/23  21:31:23  lbustini
 * Branch: Dial1_branch
 * Move dialer fields out of the idb.
 *
 * Revision 3.3  1995/11/29  01:59:03  dblair
 * CSCdi41333:  DDR Callback and Multilink PPP clean up
 *
 * Revision 3.2  1995/11/17  18:01:43  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:48:34  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:17:26  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * We get included (multiple times) from dialer code to define
 * debugging flags. For this, we use some macros that get defined
 * by debug_macros.h. What definitions the macros get depends on
 * which magic flags have been set by the file that includes us.
 */
#include "../ui/debug_macros.h"

/* Now define the actual flags and the array that points to them */
DEBUG_ARRDECL(dialer_debug_arr)

DEBUG_FLAG(dialer_debug,DEBUG_DIALER,"Dial on demand events")
DEBUG_FLAG(dialer_packet_debug,DEBUG_DIALER_PACKET,"Dial on demand packets")
DEBUG_FLAG(dialer_detailed_debug,DEBUG_DIALER_DETAILED,
	   "Dial on demand detailed messages")
DEBUG_FLAG(dialer_holdq_debug, DEBUG_DIALER_HOLDQ,
	   "dialer holdq messages")

DEBUG_ARRDONE
