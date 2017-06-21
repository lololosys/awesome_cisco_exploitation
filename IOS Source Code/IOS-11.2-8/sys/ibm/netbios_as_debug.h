/* $Id: netbios_as_debug.h,v 3.2 1995/11/17 09:23:20 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/ibm/netbios_as_debug.h,v $
 *------------------------------------------------------------------
 * netbios_as_debug.h - Debugging declarations for NBF
 *
 * September 1995, Shoou Yiu
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: netbios_as_debug.h,v $
 * Revision 3.2  1995/11/17  09:23:20  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:45:37  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:02:07  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/09/21  00:21:54  syiu
 * Initial placeholder for NBF (NetBIOS Frames) support.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

extern void netbios_as_debug_init(void);

/*
 * The actual debugging flags are defined in netbios_debug_flags.h.
 * We include that file twice, once to define the flags themselves
 * and once to define the indices that the parser uses.
 */
#include "../ibm/netbios_as_debug_flags.h"
#define __DECLARE_DEBUG_NUMS__
#include "../ibm/netbios_as_debug_flags.h"
