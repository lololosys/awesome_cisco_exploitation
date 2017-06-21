/* $Id: atm_dxi_debug_flags.h,v 3.2 1995/11/17 18:00:37 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/wan/atm_dxi_debug_flags.h,v $
 *------------------------------------------------------------------
 * Support for ATM-DXI encapsulation and RFC1483 support
 *
 * October 1994, Shankar Natarajan
 *
 * Copyright (c) 1994 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: atm_dxi_debug_flags.h,v $
 * Revision 3.2  1995/11/17  18:00:37  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:47:42  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:16:52  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
/*
 * We get included (multiple times) from atm-dxi code to define
 * debugging flags. For this, we use some macros that get defined
 * by debug_macros.h. What definitions the macros get depends on
 * which magic flags have been set by the file that includes us.
 */
#include "../ui/debug_macros.h"

/* Now define the actual flags and the array that points to them */
DEBUG_ARRDECL(atm_dxi_debug_arr)

DEBUG_FLAG(atm_dxi_debug_packet,DEBUG_ATM_DXI_PACKET,"atm-dxi packets")
DEBUG_FLAG(atm_dxi_events_debug,DEBUG_ATM_DXI_EVENTS,"atm-dxi events")

DEBUG_ARRDONE
