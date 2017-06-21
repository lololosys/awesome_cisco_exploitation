/* $Id: nasi_debug_flags.h,v 3.2 1995/11/17 19:22:25 gstovall Exp $
 * $Source: /swtal/cherf.111/ios/sys/xns/nasi_debug_flags.h,v $
 *------------------------------------------------------------------
 * NASI_debug_flags.h  Netware Asynchronous Services Interface
 *                     (Debug flag definitions)
 *
 * Sept 1995, Syed Mohammad Irfan Ashraf
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: nasi_debug_flags.h,v $
 * Revision 3.2  1995/11/17  19:22:25  gstovall
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:58:35  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:38:50  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/09/26  12:13:06  irfan
 * Netware Asynchronous Services Interface: holding area
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * We get included (multiple times) from NASI code to define
 * debugging flags. For this, we use some macros that get defined
 * by debug_macros.h. What definitions the macros get depends on
 * which magic flags have been set by the file that includes us.
 */
#include "../ui/debug_macros.h"

/* Now define the actual flags and the array that points to them */

DEBUG_ARRDECL(nasi_debug_arr)

DEBUG_FLAG(nasi_pkt_debug, DEBUG_NASIPACKETS,
	   "NASI protocol I/O packets")
DEBUG_FLAG(nasi_activity_debug, DEBUG_NASIACTIVITY,
	   "NASI protocol activity")
DEBUG_FLAG(nasi_err_debug, DEBUG_NASIERROR,
	   "NASI protocol error")

DEBUG_ARRDONE
