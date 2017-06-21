/* $Id: spx_debug_flags.h,v 3.2 1995/11/17 19:24:50 gstovall Exp $
 * $Source: /swtal/cherf.111/ios/sys/xns/spx_debug_flags.h,v $
 *------------------------------------------------------------------
 * spx_debug_flags.h   Sequenced Packet Exchange Protocol  (debug
 *                     flags include file)
 *
 * September 1995, Syed Mohammad Irfan Ashraf
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: spx_debug_flags.h,v $
 * Revision 3.2  1995/11/17  19:24:50  gstovall
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  14:00:15  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:40:04  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/09/26  11:57:17  irfan
 * Sequenced Packet Exchange Protocol: holding area
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * We get included (multiple times) from SPX code to define
 * debugging flags. For this, we use some macros that get defined
 * by debug_macros.h. What definitions the macros get depends on
 * which magic flags have been set by the file that includes us.
 */
#include "../ui/debug_macros.h"

/* Now define the actual flags and the array that points to them */

DEBUG_ARRDECL(spx_debug_arr)

DEBUG_FLAG(spx_debug, DEBUG_NOVELLSPX, "Sequenced Packet Exchange Protocol")

DEBUG_ARRDONE
