/* $Id: rvip_debug.h,v 3.2.60.1 1996/03/18 20:14:44 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/if/rvip_debug.h,v $
 *------------------------------------------------------------------
 * rvip_debug.h -- Debugging declarations for RVIP
 *
 * Julyy 1995, David Getchell
 *
 * Copyright (c) 1995-1996 by Cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: rvip_debug.h,v $
 * Revision 3.2.60.1  1996/03/18  20:14:44  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.1  1996/01/24  22:19:07  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.2  1995/11/17  09:31:01  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:52:28  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:03:26  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/07/19  19:31:50  getchell
 * Placeholder files for RSP VIP support
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * The actual debugging flags are defined in rvip_debug_flags.h.
 * We include that file twice, once to define the flags themselves
 * and once to define the indices that the parser uses.
 */
#include "../if/rvip_debug_flags.h"
#define __DECLARE_DEBUG_NUMS__
#include "../if/rvip_debug_flags.h"
