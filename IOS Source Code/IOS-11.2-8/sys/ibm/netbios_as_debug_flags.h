/* $Id: netbios_as_debug_flags.h,v 3.2 1995/11/17 09:23:21 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/ibm/netbios_as_debug_flags.h,v $
 *------------------------------------------------------------------
 * netbios_as_debug_flags.h -- Debugging flag declarations for NBF
 *
 * September 1995, Shoou Yiu
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: netbios_as_debug_flags.h,v $
 * Revision 3.2  1995/11/17  09:23:21  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:45:38  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:02:08  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/09/21  00:22:41  syiu
 * Initial placeholder for NBF (NetBIOS Frames) support.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * We get included (multiple times) from NETBIOS code to define
 * debugging flags. For this, we use some macros that get defined
 * by debug_macros.h. What definitions the macros get depends on
 * which magic flags have been set by the file that includes us.
 */
#include "../ui/debug_macros.h"

/* Now define the actual flags and the array that points to them */
DEBUG_ARRDECL(netbios_as_debug_arr)

DEBUG_FLAG(netbios_name_cache_debug,DEBUG_NETBIOS_NAMEC,"NetBIOS name cache tracing")
DEBUG_FLAG(netbios_error_debug,DEBUG_NETBIOS_ERROR,"NetBIOS protocol errors")
DEBUG_FLAG(netbios_packet_debug,DEBUG_NETBIOS_PACKET,"NetBIOS packet display")


DEBUG_ARRDONE
