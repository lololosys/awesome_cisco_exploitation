/* $Id: dhcp_debug_flags.h,v 3.2 1995/11/17 09:32:32 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/ip/dhcp_debug_flags.h,v $
 *------------------------------------------------------------------
 * dhcp_debug_flags.c - DHCP proxy client debugging support..Header
 *                      include file
 *
 * Mar 10, 1994, Syed Irfan Ashraf
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: dhcp_debug_flags.h,v $
 * Revision 3.2  1995/11/17  09:32:32  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:54:24  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:56:18  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
/*
 * We get included (multiple times) from DHCP code to define
 * debugging flags. For this, we use some macros that get defined
 * by debug_macros.h. What definitions the macros get depends on
 * which magic flags have been set by the file that includes us.
 */
#include "../ui/debug_macros.h"

/* Now define the actual flags and the array that points to them */

DEBUG_ARRDECL(dhcp_debug_arr)

DEBUG_FLAG(dhcp_debug, DEBUG_DHCP, "DHCP proxy client activity")

DEBUG_ARRDONE
