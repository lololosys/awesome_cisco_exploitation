/* $Id: vpn_debug_flags.h,v 1.2.40.1 1996/04/27 06:34:29 syiu Exp $
 * $Source: /release/112/cvs/Xsys/vpn/vpn_debug_flags.h,v $
 *------------------------------------------------------------------
 * vpn_debug_flags.h   --- debugging support for VPN
 *
 * January 1996, Shoou Yiu
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: vpn_debug_flags.h,v $
 * Revision 1.2.40.1  1996/04/27  06:34:29  syiu
 * Branch: California_branch
 * Commit Dial1_branch
 *
 * Revision 1.2.2.3  1996/03/19  03:36:49  tkolar
 * Branch: Dial1_branch
 * Clean up after tunnels when they close connections, and after
 * developers when they fat-finger commits.
 *
 * Revision 1.2.2.2  1996/03/12  04:55:34  tkolar
 * Branch: Dial1_branch
 * Vastly expand L2F comments, add two new debugging variables, and make
 * wraparound sequence numbers fully understood.
 *
 * Revision 1.2.2.1  1996/02/13  23:07:49  tkolar
 * Branch: Dial1_branch
 * Checkpoint VPN.
 *
 * Revision 1.2  1996/01/31  18:38:48  tkolar
 * Branch: Dial1_branch
 * Get all of the VPN code into the repository, with the right names
 * and in the right places.
 *
 * Revision 1.1  1996/01/29  18:09:14  tkolar
 * Populate the VPN directory.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * We get included (multiple times) from VPN code to define
 * debugging flags. For this, we use some macros that get defined
 * by debug_macros.h. What definitions the macros get depends on
 * which magic flags have been set by the file that includes us.
 */
#include "../ui/debug_macros.h"

/* Now define the actual flags and the array that points to them */
DEBUG_ARRDECL(vpn_debug_arr)

DEBUG_FLAG(l2f_event_debug, DEBUG_VPN_L2F_EVENT, "L2F protocol events")
DEBUG_FLAG(l2f_packet_debug, DEBUG_VPN_L2F_PACKET, "L2F protocol packets")
DEBUG_FLAG(l2f_error_debug, DEBUG_VPN_L2F_ERROR, "L2F protocol errors")
DEBUG_FLAG(vpn_event_debug,DEBUG_VPN_EVENT,"VPN events")
DEBUG_FLAG(vpn_error_debug,DEBUG_VPN_ERROR,"VPN errors")
DEBUG_FLAG(vpn_packet_debug,DEBUG_VPN_PACKET,"VPN packet")


DEBUG_ARRDONE
