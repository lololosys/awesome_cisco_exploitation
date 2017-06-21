/* $Id: rvip_debug_flags.h,v 3.2.60.1 1996/04/03 22:49:13 jwelder Exp $
 * $Source: /release/112/cvs/Xsys/if/rvip_debug_flags.h,v $
 *------------------------------------------------------------------
 * rvip_debug_flags.h -- Debugging flag declarations for RVIP
 *
 * July 1995, David Getchell
 *
 * Copyright (c) 1996 by Cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: rvip_debug_flags.h,v $
 * Revision 3.2.60.1  1996/04/03  22:49:13  jwelder
 * CSCdi47348:  lingering references to rvip
 * Branch: California_branch
 *
 * Revision 3.2  1995/11/17  09:31:02  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:52:30  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:03:27  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/07/19  19:31:51  getchell
 * Placeholder files for RSP VIP support
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * We get included (multiple times) from RVIP code to define
 * debugging flags. For this, we use some macros that get defined
 * by debug_macros.h. What definitions the macros get depends on
 * which magic flags have been set by the file that includes us.
 */
#include "../ui/debug_macros.h"

/* Now define the actual flags and the array that points to them */
DEBUG_ARRDECL(rvip_debug_arr)

DEBUG_FLAG(rvip_pkt_debug, DEBUG_RVIP_PKTS,
	   "VIP packets")
DEBUG_FLAG(rvip_debug,DEBUG_RVIP,"VIP network interface")

DEBUG_ARRDONE
