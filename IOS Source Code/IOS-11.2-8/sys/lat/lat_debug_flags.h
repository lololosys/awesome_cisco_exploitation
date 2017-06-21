/* $Id: lat_debug_flags.h,v 3.2 1995/11/17 17:33:33 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/lat/lat_debug_flags.h,v $
 *------------------------------------------------------------------
 * lat_debug_flags.h -- Debugging flag declarations for LAT
 *
 * May 1993, John Bashinski
 *
 * Copyright (c) 1993-1994 by Cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: lat_debug_flags.h,v $
 * Revision 3.2  1995/11/17  17:33:33  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:26:30  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:24:56  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * We get included (multiple times) from LAT code to define
 * debugging flags. For this, we use some macros that get defined
 * by debug_macros.h. What definitions the macros get depends on
 * which magic flags have been set by the file that includes us.
 */
#include "../ui/debug_macros.h"

/* Now define the actual flags and the array that points to them */
DEBUG_ARRDECL(lat_debug_arr)

DEBUG_FLAG(lat_debug,DEBUG_LAT,"LAT event")
DEBUG_FLAG(latpkt_debug,DEBUG_LATP,"LAT packet")
DEBUG_FLAG(latfltr_debug,DEBUG_LATF,"LAT filtering")

DEBUG_ARRDONE
