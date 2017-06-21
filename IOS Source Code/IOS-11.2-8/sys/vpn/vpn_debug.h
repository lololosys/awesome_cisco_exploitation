/* $Id: vpn_debug.h,v 1.2 1996/01/31 18:38:46 tkolar Exp $
 * $Source: /release/111/cvs/Xsys/vpn/vpn_debug.h,v $
 *------------------------------------------------------------------
 * vpn_debug.h --  debugging declarations for VPN
 *
 * January 1996, Shoou Yiu
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: vpn_debug.h,v $
 * Revision 1.2  1996/01/31  18:38:46  tkolar
 * Branch: Dial1_branch
 * Get all of the VPN code into the repository, with the right names
 * and in the right places.
 *
 * Revision 1.1  1996/01/29  18:09:14  tkolar
 * Populate the VPN directory.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

extern void vpn_debug_init(void);

/*
 * The actual debugging flags are defined in vpn_debug_flags.h.
 * We include that file twice, once to define the flags themselves
 * and once to define the indices that the parser uses.
 */
#include "../vpn/vpn_debug_flags.h"
#define __DECLARE_DEBUG_NUMS__
#include "../vpn/vpn_debug_flags.h"
