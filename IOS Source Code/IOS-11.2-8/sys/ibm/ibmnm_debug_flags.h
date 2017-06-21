/* $Id: ibmnm_debug_flags.h,v 3.2 1995/11/17 09:22:37 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/ibm/ibmnm_debug_flags.h,v $
 *------------------------------------------------------------------
 * ibmnm_debug_flags.h -- Debugging flag declarations for LNM
 *
 * May 1993, John Bashinski
 *
 * Copyright (c) 1993-1994 by Cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ibmnm_debug_flags.h,v $
 * Revision 3.2  1995/11/17  09:22:37  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:44:56  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:46:49  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * We get included (multiple times) from LNM code to define
 * debugging flags. For this, we use some macros that get defined
 * by debug_macros.h. What definitions the macros get depends on
 * which magic flags have been set by the file that includes us.
 */
#include "../ui/debug_macros.h"

/* Now define the actual flags and the array that points to them */
DEBUG_ARRDECL(ibmnm_debug_arr)

DEBUG_FLAG(lnm_e_debug,DEBUG_LM_E,"Lan Network Manager Event")
DEBUG_FLAG(lnm_llc_debug,DEBUG_LM_LLC,"Lan Network Manager LLC frame")
DEBUG_FLAG(lnm_mac_debug,DEBUG_LM_MAC,"Lan Network Manager MAC frame")

DEBUG_ARRDONE
