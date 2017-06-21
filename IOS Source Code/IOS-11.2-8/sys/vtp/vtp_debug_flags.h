/* $Id: vtp_debug_flags.h,v 1.1.44.1 1996/05/09 14:48:30 rbadri Exp $
 * $Source: /release/112/cvs/Xsys/vtp/vtp_debug_flags.h,v $
 *------------------------------------------------------------------
 * vtp_debug_flags.h: DEBUG command specific vars. & constants
 *
 * January 1996,   Percy P. Khabardar
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Longer description here.
 *------------------------------------------------------------------
 * $Log: vtp_debug_flags.h,v $
 * Revision 1.1.44.1  1996/05/09  14:48:30  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 1.1.74.1  1996/04/27  07:38:32  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.62.1  1996/04/08  02:42:23  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.50.1  1996/03/22  09:45:00  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.34.1  1996/03/05  05:37:32  rlowe
 * Apply LANE content to merge branch.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1.14.1  1996/02/27  21:20:26  cakyol
 * non sync sync of LE_Cal_V111_0_16_branch to V111_1_0_3 yielding
 * LE_Cal_V111_1_0_3_branch
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 1.1.2.1  1996/01/23  00:47:17  percyk
 * Branch: LE_Cal_V111_0_16_branch
 * add default vlans at startup time.  add meat to debug files.
 *
 * Revision 1.1  1996/01/23  00:18:34  percyk
 * placeholder files for vtp debugging
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/* Function to fire up vtp debugging */
void vtp_debug_init(void);
 
/* Now define the actual flags and the array that points to them */
DEBUG_ARRDECL(vtp_debug_arr)
 
DEBUG_FLAG(vtp_debug_error, DEBUG_VTP_ERROR, "vtp error")
DEBUG_FLAG(vtp_debug_packet, DEBUG_VTP_PAK, "vtp packets")
DEBUG_FLAG(vtp_debug_event, DEBUG_VTP_EVENT, "vtp events")
 
DEBUG_ARRDONE
