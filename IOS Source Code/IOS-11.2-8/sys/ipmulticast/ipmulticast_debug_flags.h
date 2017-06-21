/* $Id: ipmulticast_debug_flags.h,v 3.2.58.3 1996/09/13 22:52:44 lwei Exp $
 * $Source: /release/112/cvs/Xsys/ipmulticast/ipmulticast_debug_flags.h,v $
 *------------------------------------------------------------------
 * ipmulticast_debug_flags.h - Debugging flag declarations for IP multicast 
 *                             routing.
 *
 * November, 1993.
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: ipmulticast_debug_flags.h,v $
 * Revision 3.2.58.3  1996/09/13  22:52:44  lwei
 * CSCdi69140:  Fix per code review comments to CSCdi68465
 * Branch: California_branch
 *
 * Revision 3.2.58.2  1996/09/10  05:31:43  lwei
 * CSCdi68465:  RP information is too difficult to change in last-hop
 *              routers
 * Branch: California_branch
 *
 * Revision 3.2.58.1  1996/04/03  23:19:13  dino
 * CSCdi51598:  Problem of interoperability with switches
 * Branch: California_branch
 *
 * Revision 3.2  1995/11/17  17:29:56  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:01:14  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:03:53  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/*
 * We get included (multiple times) from IP routing code to define
 * debugging flags. For this, we use some macros that get defined
 * by debug_macros.h. What definitions the macros get depends on
 * which magic flags have been set by the file that includes us.
 */
#include "../ui/debug_macros.h"

/* 
 * Now define the actual flags and the array that points to them.
 */
DEBUG_ARRDECL(ipmulticast_debug_arr)

DEBUG_FLAG(igmp_debug, DEBUG_IGMP, "IGMP")
DEBUG_FLAG(cgmp_debug, DEBUG_CGMP, "CGMP")
DEBUG_FLAG(dvmrp_debug, DEBUG_DVMRP, "DVMRP")
DEBUG_FLAG(pim_debug, DEBUG_PIM, "PIM")
DEBUG_FLAG(mroute_debug, DEBUG_MROUTE, "IP multicast routing")
DEBUG_FLAG(mcache_debug, DEBUG_MCACHE, "IP multicast fast-switching")
DEBUG_FLAG(sd_debug, DEBUG_SD, "Session Directory (SD)")
DEBUG_FLAG(pim_autorp_debug, DEBUG_AUTORP, "PIM Auto-RP")

DEBUG_ARRDONE
    
