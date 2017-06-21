/* $Id: cfg_int_atm_synalc.h,v 3.1.66.3 1996/07/02 23:44:30 sakumar Exp $
 * $Source: /release/112/cvs/Xsys/src-68-alc/cfg_int_atm_synalc.h,v $
 *------------------------------------------------------------------
 * cfg_int_atm_synalc.h -- ATM configuration commands specific to
 *			   Synergy ATM line card.
 * (patterned after cfg_int_atm_c4000.h)
 *
 * September 28, 1995, Sanjaya Kumar
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cfg_int_atm_synalc.h,v $
 * Revision 3.1.66.3  1996/07/02  23:44:30  sakumar
 * CSCdi60668:  PVC (RFC1483) support needs to be added to Cat5K ATM Module
 * Branch: California_branch
 *
 * Incorporated CLI change requests for PVC config and throttle config.
 *
 * Revision 3.1.66.2  1996/06/18  16:43:57  sakumar
 * CSCdi60668:  PVC (RFC1483) support needs to be added to Cat5K ATM Module
 * Branch: California_branch
 *
 * Revision 3.1.66.1  1996/05/09  14:41:05  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.1.94.1  1996/04/27  07:25:19  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.1.84.1  1996/04/08  02:08:53  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.1.72.1  1996/03/22  09:43:37  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.1.56.1  1996/03/03  21:24:52  rlowe
 * Apply SYNALC content to merge branch.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 3.1.54.1  1996/03/02  01:21:07  rlowe
 * Non-sync sync to mainline tag V111_1_0_3.
 * Branch: Synalc3_Cal_V111_1_0_3_branch
 *
 * Revision 3.1.38.1  1996/02/29  07:50:14  rlowe
 * First pass non-sync sync for Synergy ATM line card to V111_0_16.
 * Branch: Synalc3_Cal_V111_0_16_branch
 *
 * Revision 3.1  1995/11/09  13:24:51  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1.18.1  1996/02/03  07:08:31  rlowe
 * Non-sync sync of Synalc_catchup_branch to V111_0_5.
 * Branch: Synalc_catchup_pretal_branch
 *
 * Revision 2.1.2.1  1995/09/29  18:46:06  sakumar
 * Added support for vc-per-vp command for the Catalyst 5000 ATM module..
 * Branch: Synalc2_LE2_V110_1_0_3_branch
 *
 * Revision 2.1  1995/09/29  01:41:20  rlowe
 * Placeholders for Synergy ATM line card.
 *
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
/******************************************************************
 * ATM vc_per_vp <n>
 *
 * OBJ(int,1) = VCIs per VPIs
 */
EOLS    (ci_atm_vc_vp_eol, atm_interface_commands, ATM_CMD_VC_PER_VP);
 
NUMBER  (ci_atm_vc_vp_num, ci_atm_vc_vp_eol, no_alt, OBJ(int,1),
        32, 1024, "VCIs per VPI(32,64,128,256,512 or 1024");
NVGENS  (ci_atm_vc_vp_nvgen, ci_atm_vc_vp_num,
        atm_interface_commands, ATM_CMD_VC_PER_VP);
KEYWORD (ci_atm_vc_vp, ci_atm_vc_vp_nvgen, ALTERNATE,
        "vc-per-vp", "ATM VCIs per VPI", PRIV_CONF);

#undef  ALTERNATE
#define ALTERNATE   ci_atm_vc_vp

/*******************************************************************
 * atm remove_vlan
 * OBJ(int 1) = VLAN id
 */
EOLS (ci_atm_remove_vlan_eol, cat5k_interface_commands, ATM_CMD_REMOVE_VLAN);
NUMBER (ci_atm_remove_vlanid, ci_atm_remove_vlan_eol, no_alt, OBJ(int, 1),
	1, 1023, "VLAN-ID(1-1023)");

NVGENS  (ci_atm_remove_vlan_nvgen, ci_atm_remove_vlanid,
	 cat5k_interface_commands, ATM_CMD_REMOVE_VLAN);
KEYWORD (ci_atm_remove_vlan, ci_atm_remove_vlan_nvgen, ALTERNATE,
	 "remove_pvc_vlan", "Unbind all PVCs from VLAN", PRIV_CONF|PRIV_SUBIF);

#undef ALTERNATE
#define ALTERNATE ci_atm_remove_vlan


/*******************************************************************
 * atm traffic-shape
 * OBJ(int 1) = throttle speed
 */
EOLS (ci_atm_throttle_tsar_eol, cat5k_interface_commands, 
      ATM_CMD_THROTTLE_TSAR);
NOPREFIX (ci_atm_throttle_tsar_no_prefix, ci_atm_throttle_tsar_eol, 
          ci_atm_throttle_tsar_eol);
NUMBER (ci_atm_throttle_tsar_parm, ci_atm_throttle_tsar_no_prefix, no_alt, 
        OBJ(int, 1), 1, 155, "Peak rate(Mbps)");

NVGENS  (ci_atm_throttle_tsar_nvgen, ci_atm_throttle_tsar_parm,
         cat5k_interface_commands, ATM_CMD_THROTTLE_TSAR);

KEYWORD (ci_atm_throttle_tsar_rate, ci_atm_throttle_tsar_nvgen, NONE,
         "rate", "Set maximum interface Output Rate", PRIV_CONF);

KEYWORD (ci_atm_throttle_tsar, ci_atm_throttle_tsar_rate, ALTERNATE,
         "traffic-shape", "Set interface Output Rate parameters", PRIV_CONF);

#undef ALTERNATE
#define ALTERNATE ci_atm_throttle_tsar

