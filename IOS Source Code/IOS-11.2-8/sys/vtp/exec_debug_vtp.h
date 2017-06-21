/* $Id: exec_debug_vtp.h,v 1.1.44.1 1996/05/09 14:48:18 rbadri Exp $
 * $Source: /release/112/cvs/Xsys/vtp/exec_debug_vtp.h,v $
 *------------------------------------------------------------------
 * exec_debug_vtp.h:  DEBUG command parsing code
 *
 * January, 1996,  Percy P. Khabardar
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All Rights Reserved.
 *
 * Debug commands for vtp.
 *------------------------------------------------------------------
 * $Log: exec_debug_vtp.h,v $
 * Revision 1.1.44.1  1996/05/09  14:48:18  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 1.1.74.1  1996/04/27  07:38:20  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.62.1  1996/04/08  02:42:09  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.50.1  1996/03/22  09:44:50  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.34.1  1996/03/05  05:37:20  rlowe
 * Apply LANE content to merge branch.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1.14.1  1996/02/27  21:20:14  cakyol
 * non sync sync of LE_Cal_V111_0_16_branch to V111_1_0_3 yielding
 * LE_Cal_V111_1_0_3_branch
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 1.1.2.1  1996/01/23  00:47:10  percyk
 * Branch: LE_Cal_V111_0_16_branch
 * add default vlans at startup time.  add meat to debug files.
 *
 * Revision 1.1  1996/01/23  00:18:31  percyk
 * placeholder files for vtp debugging
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/****************************************************************************
 * debug vtp error
 *
 */
EOLS    (debug_vtp_error_eol, debug_command, DEBUG_VTP_ERROR);
KEYWORD (debug_vtp_error, debug_vtp_error_eol, no_alt,
         "error", "vtp errors", PRIV_OPR);
 
/****************************************************************************
 * debug vtp packets
 *
 */
EOLS    (debug_vtp_packets_eol, debug_command, DEBUG_VTP_PAK);
KEYWORD (debug_vtp_packets, debug_vtp_packets_eol, debug_vtp_error,
         "packets", "vtp packets", PRIV_OPR);
 
/***************************************************************************
 * debug vtp events
 *
 */
EOLS    (debug_vtp_events_eol, debug_command, DEBUG_VTP_EVENT);
KEYWORD (debug_vtp_events, debug_vtp_events_eol, debug_vtp_packets,
         "events", "vtp events", PRIV_OPR);
 
/***************************************************************************
 * debug vtp
 *
 */
KEYWORD_DEBUG(debug_vtp, debug_vtp_events, ALTERNATE,
              OBJ(pdb,1), vtp_debug_arr,
              "vtp", "vtp", PRIV_OPR);
 
#undef  ALTERNATE
#define ALTERNATE       debug_vtp
