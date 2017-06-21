/* $Id: atmtest_sig_vars.h,v 3.2.56.1 1996/05/09 14:11:55 rbadri Exp $
 * $Source: /release/112/cvs/Xsys/atm-test/atmtest_sig_vars.h,v $
 *------------------------------------------------------------------
 * atmtest_sig_vars.h -- Definitions of TCL variables used to support
 * various ATM related TCL commands, including LANE and ATM signalling.
 * [Was ../tcl/tcl_atm_vars.h]
 *
 * April 1995,  Chris Parrott
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: atmtest_sig_vars.h,v $
 * Revision 3.2.56.1  1996/05/09  14:11:55  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.2.84.1  1996/04/27  06:34:16  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.2.74.1  1996/04/08  01:44:46  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.2.62.1  1996/03/22  09:35:11  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.2.48.1  1996/03/05  05:49:04  rlowe
 * Apply LANE content to merge branch.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 3.2.36.1  1996/02/27  20:40:44  cakyol
 * non sync sync of LE_Cal_V111_0_16_branch to V111_1_0_3 yielding
 * LE_Cal_V111_1_0_3_branch
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 3.2.14.1  1996/01/11  01:44:44  cakyol
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
 * Revision 3.2  1995/11/17  08:47:52  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:03:09  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef _ATMTEST_SIG_VARS_H_
#define _ATMTEST_SIG_VARS_H_

/*
 * Vars used for the Global LANE VCD array.  Each element of this
 * array corresponds to a particular type of LANE VCC.  Each element
 * is either assigned a static value (when PVCs are being tested), or
 * a value based on what's returned by the ATM Signalling code for
 * an SVC.
 * Example:  set vcd_array( configure )  4
 */
#define LANE_VCD_ARRAY_STR		"lane_vcd_array"
#define LANE_VC_TYPE_UNKNOWN_STR	"unknown"
#define LANE_LEC_DIRECT_STR		"lec_direct"
#define LANE_LEC_DISTRIBUTE_STR		"lec_distribute"
#define LANE_LEC_CONFIGURE_STR		"lec_configure"
#define LANE_LEC_SEND_STR		"lec_send"
#define LANE_LEC_FORWARD_STR		"lec_forward"
#define LANE_LEC_DATA_STR		"lec_data"
#define LANE_LES_DIRECT_STR		"les_direct"
#define LANE_LES_DISTRIBUTE_STR		"les_distribute"
#define LANE_BUS_SEND_STR		"bus_send"
#define LANE_BUS_FORWARD_STR		"bus_forward"
#define LANE_CONFIGURE_STR		"configure"

/*
 * Specific defines for elements of the LANE VCD array 
 */
#define VCD_ARRAY_LEC_DIRECT_STR	"lane_vcd_array(lec_direct)"
#define VCD_ARRAY_LEC_DISTRIBUTE_STR	"lane_vcd_array(lec_distribute)"
#define VCD_ARRAY_LEC_CONFIGURE_STR	"lane_vcd_array(lec_configure)"
#define VCD_ARRAY_LEC_SEND_STR		"lane_vcd_array(lec_send)"
#define VCD_ARRAY_LEC_FORWARD_STR	"lane_vcd_array(lec_forward)"
#define VCD_ARRAY_LEC_DATA_STR		"lane_vcd_array(lec_data)"
#define VCD_ARRAY_LES_DIRECT_STR	"lane_vcd_array(les_direct)"
#define VCD_ARRAY_LES_DISTRIBUTE_STR	"lane_vcd_array(les_distribute)"
#define VCD_ARRAY_LES_CONFIGURE_STR	"lane_vcd_array(les_configure)"
#define VCD_ARRAY_BUS_SEND_STR		"lane_vcd_array(bus_send)"
#define VCD_ARRAY_BUS_FORWARD_STR	"lane_vcd_array(bus_forward)"
#define VCD_ARRAY_CONFIGURE_STR		"lane_vcd_array(configure)"



/*
 * Current VC TCL Global.  Things are a little sticky wrt this variable...
 * to keep track of multiple SVCs, we need to know the 'current' SVC 
 * so that the call_id can be saved/retrieved.  It _must_ be set by the 
 * user prior to running the commands to set up an SVC, but we'll default
 * it just to maintain sanity.
 */
#define LANE_CURRENT_VC			"current_vc"

/* Arbitrary */
#define LANE_CURRENT_VC_DEF		"configure"


#endif _ATMTEST_SIG_VARS_H_
