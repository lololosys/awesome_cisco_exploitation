/* $Id: dlcsw_debug_flags.h,v 3.2 1995/11/17 08:56:02 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/cls/dlcsw_debug_flags.h,v $
 *------------------------------------------------------------------
 * DLC-SW Debug Flags Definitions
 *
 * September 1994, Meng Lyu
 *
 * Copyright (c) 1993-1994 by Cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: dlcsw_debug_flags.h,v $
 * Revision 3.2  1995/11/17  08:56:02  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:12:08  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:20:41  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * We get included (multiple times) from DLC-SW code to define
 * debugging flags. For this, we use some macros that get defined
 * by debug_macros.h. What definitions the macros get depends on
 * which magic flags have been set by the file that includes us.
 */
#include "../ui/debug_macros.h"

/* Now define the actual flags and the array that points to them */
DEBUG_ARRDECL(dlcsw_debug_arr)

DEBUG_FLAG(dlcsw_e_debug,DEBUG_DLCSW_ERR,"DLC Switch Error")
DEBUG_FLAG(dlcsw_state_debug,DEBUG_DLCSW_STATE,"DLC Switch State")
DEBUG_FLAG(dlcsw_mess_debug,DEBUG_DLCSW_MESS,"DLC Switch Message")

DEBUG_ARRDONE
