/* $Id: mmc_debug_flags.h,v 3.1 1995/11/09 13:23:45 shaker Exp $
 * $Source: /swtal/cherf.111/ios/sys/src-4k-ls1010/mmc_debug_flags.h,v $
 *------------------------------------------------------------------
 * parser commands for mmc driver
 *
 * September 1995, Ram Bhide.
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: mmc_debug_flags.h,v $
 * Revision 3.1  1995/11/09  13:23:45  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/09/27  14:56:51  jwilliam
 * initial checkin
 *
 * Revision 2.2  1995/08/03  00:28:55  jjohnson
 * CSCdi38141:  template files contain bogus log information
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "../ui/debug_macros.h"

/* Now define the actual flags and the array that points to them */
DEBUG_ARRDECL(mmc_debug_arr)

DEBUG_FLAG(mmc_debug_all_flag, DEBUG_MMC_ALL, "ATM MMC All")
DEBUG_FLAG(mmc_errors_trace, DEBUG_MMC_TRACE, 
           "ATM MMC TRACE")
DEBUG_FLAG(mmc_errors_debug, DEBUG_MMC_ERROR, 
           "ATM MMC ERRORS ")

DEBUG_ARRDONE
