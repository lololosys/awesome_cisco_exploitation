/* $Id: pcbus_debug_flags.h,v 3.2 1995/11/17 17:42:28 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/les/pcbus_debug_flags.h,v $
 *------------------------------------------------------------------
 * pcbus_debug_flags.h -- Debugging flag declarations for pcbus
 *
 * April 1994, Johnny Chan
 *
 * Copyright (c) 1994 by Cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: pcbus_debug_flags.h,v $
 * Revision 3.2  1995/11/17  17:42:28  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:34:19  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:39:37  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 * $Endlog$
 */

/*
 * We get included (multiple times) from pcbus code to define
 * debugging flags. For this, we use some macros that get defined
 * by debug_macros.h. What definitions the macros get depends on
 * which magic flags have been set by the file that includes us.
 */
#include "../ui/debug_macros.h"

/* Now define the actual flags and the array that points to them */
DEBUG_ARRDECL(pcbus_debug_arr)

DEBUG_FLAG(pcbus_debug,DEBUG_PCBUS,"PCbus network interface")
DEBUG_FLAG(pcbus_packet_debug,DEBUG_PCBUSPACKET,"PCbus network packets")

DEBUG_ARRDONE
