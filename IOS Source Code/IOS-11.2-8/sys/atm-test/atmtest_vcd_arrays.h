/* $Id: atmtest_vcd_arrays.h,v 3.2.56.1 1996/05/09 14:11:55 rbadri Exp $
 * $Source: /release/112/cvs/Xsys/atm-test/atmtest_vcd_arrays.h,v $
 *------------------------------------------------------------------
 * atmtest_vcd_arrays.h -- Definitions of arrays of TCL variables used
 * to support testing VCCs used in connection with LAN Emulation.
 * [Was ../tcl/tcl_vcd_arrays.h]
 *
 * April 1995,  Chris Parrott
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: atmtest_vcd_arrays.h,v $
 * Revision 3.2.56.1  1996/05/09  14:11:55  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.2.84.1  1996/04/27  06:34:17  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.2.74.1  1996/04/08  01:44:47  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.2.62.1  1996/03/22  09:35:12  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.2.48.1  1996/03/05  05:49:05  rlowe
 * Apply LANE content to merge branch.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 3.2.36.1  1996/02/27  20:40:45  cakyol
 * non sync sync of LE_Cal_V111_0_16_branch to V111_1_0_3 yielding
 * LE_Cal_V111_1_0_3_branch
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 3.2.14.1  1996/01/11  01:44:45  cakyol
 * non sync sync of LE_Calif_branch to V111_0_16 yielding
 *     LE_Cal_V111_0_16_branch
 *
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 3.2.10.1  1995/12/29  01:05:34  cakyol
 * Initial commits for:
 *
 * LANE Fault Tolerant LECS Servers
 * LANE Fault Tolerant LES Servers
 * HSRP over LANE Client
 *
 * Branch: LE_Calif_branch
 *
 * Revision 3.2  1995/11/17  08:47:53  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:03:10  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef _ATMTEST_VCD_ARRAYS_H_
#define _ATMTEST_VCD_ARRAYS_H_


/*
 * The LANE VCD array is used like the other LANE variable arrays 
 * above to allow us to more consisely access all the elements in 
 * loops (e.g. to dump the array).
 */
static char *lane_vcd_array[] = {
	 LANE_VC_TYPE_UNKNOWN_STR,
	 LANE_LEC_DIRECT_STR,
	 LANE_LEC_DISTRIBUTE_STR,
	 LANE_LEC_CONFIGURE_STR,
	 LANE_LEC_SEND_STR,
	 LANE_LEC_FORWARD_STR,
	 LANE_LEC_DATA_STR,
	 LANE_LES_DIRECT_STR,
	 LANE_LES_DISTRIBUTE_STR,
	 LANE_BUS_SEND_STR,
	 LANE_BUS_FORWARD_STR,
	 LANE_CONFIGURE_STR };

#endif _ATMTEST_VCD_ARRAYS_H_
