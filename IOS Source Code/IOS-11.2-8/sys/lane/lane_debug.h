/* $Id: lane_debug.h,v 3.2.58.1 1996/05/09 14:32:22 rbadri Exp $
 * $Source: /release/112/cvs/Xsys/lane/lane_debug.h,v $
 *------------------------------------------------------------------
 * lane_debug.h: DEBUG command specific vars. & constants
 *
 * October, 1994, Chris Parrott
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: lane_debug.h,v $
 * Revision 3.2.58.1  1996/05/09  14:32:22  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.2.86.1  1996/04/27  07:06:17  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.2.76.1  1996/04/08  01:56:44  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.2.64.1  1996/03/22  22:54:07  rlowe
 * Non-sync of Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.2.48.1  1996/03/05  06:42:45  rlowe
 * Apply LANE content to merge branch.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 3.2.36.1  1996/02/27  21:10:06  cakyol
 * non sync sync of LE_Cal_V111_0_16_branch to V111_1_0_3 yielding
 * LE_Cal_V111_1_0_3_branch
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 3.2.14.1  1996/01/11  01:46:11  cakyol
 * non sync sync of LE_Calif_branch to V111_0_16 yielding
 *     LE_Cal_V111_0_16_branch
 *
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 3.2.10.1  1996/01/03  22:52:39  cakyol
 * - divided up debug categories to: all, events and packets
 *   for the LECS.  There were too many debugging messages
 *   for a single debug flag.
 *
 * - updated some of the outdated comments.
 *
 * Branch: LE_Calif_branch
 *
 * Revision 3.2  1995/11/17  17:50:20  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:22:52  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:19:13  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef LANE_DEBUG_H
#define LANE_DEBUG_H

/* Function to fire up AIP debugging */
extern void lane_debug_init(void);
extern void lec_debug_all(boolean flag);
extern void lecs_debug_all(boolean flag);
extern void lane_debug_all(boolean flag);

/*
 * The actual debugging flags are defined in lane_debug_flags.h.
 * We include that file twice, once to define the flags themselves
 * and once to define the indices that the parser uses.
 */
#include "../lane/lane_debug_flags.h"
#define __DECLARE_DEBUG_NUMS__
#include "../lane/lane_debug_flags.h"

#endif LANE_DEBUG_H

