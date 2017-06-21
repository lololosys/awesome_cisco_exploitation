/* $Id: nhrp_debug_flags.h,v 3.2.62.1 1996/03/28 03:31:31 bcole Exp $
 * $Source: /release/111/cvs/Xsys/wan/nhrp_debug_flags.h,v $
 *------------------------------------------------------------------
 * Next Hop Resolution Protocol.
 *
 * October 1995, Bruce Cole
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: nhrp_debug_flags.h,v $
 * Revision 3.2.62.1  1996/03/28  03:31:31  bcole
 * CSCdi51494:  NHRP: Prefix entries not appearing in cache in many cases
 * Branch: California_branch
 *
 * Bring 11.1 fixes forward...
 *
 * Revision 3.2  1995/11/17  18:03:41  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:50:18  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:36:19  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/10/06  21:55:06  bcole
 * Placeholder files for 11.1 NHRP work.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * We get included (multiple times) to define
 * debugging flags. For this, we use some macros that get defined
 * by debug_macros.h. What definitions the macros get depends on
 * which magic flags have been set by the file that includes us.
 */

#include "../ui/debug_macros.h"

/* Now define the actual flags and the array that points to them */
DEBUG_ARRDECL(nhrp_debug_arr)

DEBUG_FLAG(nhrp_debug,DEBUG_NHRP,"NHRP protocol")
DEBUG_FLAG(nhrp_debug_opt,DEBUG_NHRP_OPT,"NHRP option processing")
DEBUG_FLAG(nhrp_debug_cache,DEBUG_NHRP_CACHE,"NHRP cache operations")
DEBUG_FLAG(nhrp_debug_rate,DEBUG_NHRP_RATE,"NHRP rate limiting")

DEBUG_ARRDONE
