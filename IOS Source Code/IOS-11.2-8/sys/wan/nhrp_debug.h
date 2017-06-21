/* $Id: nhrp_debug.h,v 3.2 1995/11/17 18:03:40 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/wan/nhrp_debug.h,v $
 *------------------------------------------------------------------
 * Next Hop Resolution Protocol.
 *
 * October 1995, Bruce Cole
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: nhrp_debug.h,v $
 * Revision 3.2  1995/11/17  18:03:40  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:50:18  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:36:17  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/10/06  21:55:06  bcole
 * Placeholder files for 11.1 NHRP work.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * The actual debugging flags are defined in nhrp_debug_flags.h.
 * We include that file twice, once to define the flags themselves
 * and once to define the indices that the parser uses.
 */
#include "../wan/nhrp_debug_flags.h"
#define __DECLARE_DEBUG_NUMS__
#include "../wan/nhrp_debug_flags.h"
