/* $Id: lane_debug.c,v 3.2.58.2 1996/07/25 09:49:05 snyder Exp $
 * $Source: /release/112/cvs/Xsys/lane/lane_debug.c,v $
 *------------------------------------------------------------------
 * lane_debug.c: LANE DEBUG command code
 *
 * October 1994, Chris Parrott
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: lane_debug.c,v $
 * Revision 3.2.58.2  1996/07/25  09:49:05  snyder
 * CSCdi63981:  eliminate unused code, fix bugs, make a common routine
 *              1. create a routine call generic_debug_all and
 *                 make most debugs use it
 *              2. eliminate if (*_debug_inited) return, most never set
 *              the var
 *                 to TRUE making the code useless.
 *              3. declare some constant arrays const
 *              4. fix bugs found along the way
 *              5. save 2768 bytes from image, 660 out of data section
 * Branch: California_branch
 *
 * Revision 3.2.58.1  1996/05/09  14:32:21  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.2.86.1  1996/04/27  07:06:16  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.2.76.2  1996/04/15  09:06:39  shj
 * CSCdi52532:  LANE invalidates protocol fast caches too often
 * Branch: LE_Cal_V112_0_2_branch
 * Don't do bump_cache_version for dying LE-ARP's unless data-direct path
 * was actually in use.  Also add some debugging to see dying LE-ARP's.
 *
 * Revision 3.2.76.1  1996/04/08  01:56:43  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.2.64.1  1996/03/22  22:54:05  rlowe
 * Non-sync of Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.2.48.2  1996/03/05  06:42:44  rlowe
 * Apply LANE content to merge branch.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 3.2.48.1  1996/03/03  21:23:24  rlowe
 * Apply SYNALC content to merge branch.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 3.2.36.1  1996/02/27  21:10:05  cakyol
 * non sync sync of LE_Cal_V111_0_16_branch to V111_1_0_3 yielding
 * LE_Cal_V111_1_0_3_branch
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 3.2.14.1  1996/01/11  01:46:10  cakyol
 * non sync sync of LE_Calif_branch to V111_0_16 yielding
 *     LE_Cal_V111_0_16_branch
 *
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 3.2.10.1  1996/01/03  22:52:37  cakyol
 * - divided up debug categories to: all, events and packets
 *   for the LECS.  There were too many debugging messages
 *   for a single debug flag.
 *
 * - updated some of the outdated comments.
 *
 * Branch: LE_Calif_branch
 *
 * Revision 3.2  1995/11/17  17:50:18  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:22:50  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1.64.1  1996/02/03  07:07:01  rlowe
 * Non-sync sync of Synalc_catchup_branch to V111_0_5.
 * Branch: Synalc_catchup_pretal_branch
 *
 * Revision 2.1.62.1  1996/01/19  21:56:44  sgarcia
 * CSCdi47203:  sync 11.1 catchup branch to 11.0 ATMFLASH_V11_951214 tag
 * of Synalc2_
 * Branch: Synalc_catchup_branch
 * Patch 11.0 Synalc2_LE2_V110_1_0_3_branch diffs between tags
 * Synalc2_LE2_V110_1_0_3_arizona_arkansas_split_951103 and
 * ATMFLASH_V11_951214
 * into 11.1 branch.
 *
 * Revision 2.1.44.1  1995/12/01  16:01:13  bbenson
 * CSCdi44768:  TOPO change debug message should output on toggle
 * Branch: Synalc2_LE2_V110_1_0_3_branch
 * Added new debug switch to the client for topo debugging.
 *
 * Revision 2.1  1995/06/07  21:19:10  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "sys_registry.h"
#include "../ui/debug.h"
#include "../lane/lane_debug.h"

/* Declare the initialized text for the debugging array */
#define __DECLARE_DEBUG_ARR__
#include "lane_debug_flags.h"

/*
 * lec_debug_all
 *
 * Is called to turn on all debugging flags for the LAN emulation
 * Client.
 */
void lec_debug_all (boolean flag)
{
    *(lane_debug_arr[DEBUG_LANE_LEC_PACKET].var) = flag;
    *(lane_debug_arr[DEBUG_LANE_LEC_STATE].var) = flag;
    *(lane_debug_arr[DEBUG_LANE_LEC_SIGNAL].var) = flag;
    *(lane_debug_arr[DEBUG_LANE_LEC_LE_ARP].var) = flag;
    *(lane_debug_arr[DEBUG_LANE_LEC_TOPO].var) = flag;
}

void lecs_debug_all (boolean flag)
{
    *(lane_debug_arr [DEBUG_LANE_LECS].var) = flag;
    *(lane_debug_arr [DEBUG_LANE_LECS_PACKETS].var) = flag;
}

/*
 * lane_debug_any
 * 
 * This routine can be called to find out if any "lane debug" commands
 * have been issued.
 */
boolean lane_debug_any (void)
{
    int i;

    for (i = 0; lane_debug_arr[i].var != (boolean *) NULL; i++) {
	if (*(lane_debug_arr[i].var))
	    return(TRUE);
    }
    return(FALSE);
}

/*
 * lane_debug_all
 * 
 * This routine is registered to be called whenever anybody issues
 * a "debug all" or "undebug all" command... or whenever you want to
 * set the state of all the lane debug flags at once. The argument is
 * TRUE for "debug all", FALSE for "undebug all".
 */
void lane_debug_all (boolean flag)
{
    generic_debug_all(lane_debug_arr, flag);
}

/*
 * lane_debug_show
 *
 * Is called to display the values of all the LANE
 * debugging variables.
 */
static void lane_debug_show (void)
{
    debug_show_flags(&(lane_debug_arr[0]), "Generic LANE");
}

/*
 * lane_debug_init
 *
 * Is called at ATM subsystem startup. It registers the routines 
 * to maintain and display the LANE debug flags, and makes sure
 * the flags are set to the appropriate values depending on
 * whether "debug all" is in effect when LANE is started.
 */
void lane_debug_init (void)
{

    /* Register for "debug all" and "show debug" events. A Real Programmer
     * would be prepared to do something with the error code returned
     * from registry_add_list. Oh, well...
     */
    (void) reg_add_debug_all(lane_debug_all, "lane_debug_all");
    (void) reg_add_debug_show(lane_debug_show, "lane_debug_show");

    /* Make sure the debug flags are right at startup. If "debug all"
     * is in effect when LANE is initialized, we want to turn on
     * all the LANE debugging right now.
     */
    lane_debug_all(debug_all_p());
}
