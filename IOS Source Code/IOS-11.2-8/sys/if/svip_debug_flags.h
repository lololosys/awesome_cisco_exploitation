/* $Id: svip_debug_flags.h,v 3.1.58.2 1996/04/03 22:49:15 jwelder Exp $
 * $Source: /release/112/cvs/Xsys/if/svip_debug_flags.h,v $
 *------------------------------------------------------------------
 * svip_debug_flags.h -- Debugging flag declarations for SVIP
 *
 * November 1995, David Getchell
 *
 * Copyright (c) 1995-1996 by Cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: svip_debug_flags.h,v $
 * Revision 3.1.58.2  1996/04/03  22:49:15  jwelder
 * CSCdi47348:  lingering references to rvip
 * Branch: California_branch
 *
 * Revision 3.1.58.1  1996/03/21  22:49:18  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.1.6.1  1996/02/10  00:05:07  mbeesley
 * CSCdi48581:  Add VIP2 support to ELC_branch
 * Branch: ELC_branch
 *
 * Revision 3.1  1996/01/02  06:27:10  getchell
 * Placeholder for VIPER development
 *
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * We get included (multiple times) from SVIP code to define
 * debugging flags. For this, we use some macros that get defined
 * by debug_macros.h. What definitions the macros get depends on
 * which magic flags have been set by the file that includes us.
 */
#include "../ui/debug_macros.h"

/* Now define the actual flags and the array that points to them */
DEBUG_ARRDECL(svip_debug_arr)

DEBUG_FLAG(svip_pkt_debug, DEBUG_SVIP_PKTS,
	   "VIP2 packets")
DEBUG_FLAG(svip_debug,DEBUG_SVIP,"VIP2 network interface")

DEBUG_ARRDONE
