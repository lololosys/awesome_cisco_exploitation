/* $Id: cfg_int_csu_dsu.h,v 3.2 1996/03/29 03:49:32 sagarwal Exp $
 * $Source: /release/111/cvs/Xsys/les/cfg_int_csu_dsu.h,v $
 *------------------------------------------------------------------
 * C F G _ I N T _ C S U _ D S U . H
 *
 * March 1996, Johnny Chan
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_int_csu_dsu.h,v $
 * Revision 3.2  1996/03/29  03:49:32  sagarwal
 * CSCdi46172:  Add Boa support to 11.1 mainline
 *
 * Revision 3.1  1996/03/28  20:06:10  sagarwal
 * Placeholder files for 2500 Derivative with integrated CSU/DSU
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/* forward declarations */
pdecl(ci_csu_dsu_timeslots_range_num1);
pdecl(ci_csu_dsu_timeslots_range_num1w);

/***************************************************************
 * [no] service-module 56k clock rate {auto | 2.4 | 4.8 |
 *				       9.6 | 19.2 | 38.4 | 56 | 64}
 *
 * OBJ(int,1) = 56k 
 * OBJ(int,2) = auto | 2.4 | 4.8 | 9.6 | 19.2 | 38.4 | 56 | 64
*/
EOLS  (ci_csu_dsu_clk_rate_eol, csu_dsu_service_module_command, CSU_DSU_CLK_RATE);

KEYWORD_ID(ci_csu_dsu_clk_rate_64, ci_csu_dsu_clk_rate_eol,
           no_alt, OBJ(int,2), CSU_DSU_CLK_RATE_64, "64",
           "64000",
           PRIV_CONF);

KEYWORD_ID(ci_csu_dsu_clk_rate_56, ci_csu_dsu_clk_rate_eol,
           ci_csu_dsu_clk_rate_64, OBJ(int,2), CSU_DSU_CLK_RATE_56, "56",
           "56000",
           PRIV_CONF);

KEYWORD_ID(ci_csu_dsu_clk_rate_38_4, ci_csu_dsu_clk_rate_eol,
           ci_csu_dsu_clk_rate_56, OBJ(int,2), CSU_DSU_CLK_RATE_38_4, "38.4",
           "38400",
           PRIV_CONF);

KEYWORD_ID(ci_csu_dsu_clk_rate_19_2, ci_csu_dsu_clk_rate_eol,
           ci_csu_dsu_clk_rate_38_4, OBJ(int,2), CSU_DSU_CLK_RATE_19_2, "19.2",
           "19200",
           PRIV_CONF);

KEYWORD_ID(ci_csu_dsu_clk_rate_9_6, ci_csu_dsu_clk_rate_eol,
           ci_csu_dsu_clk_rate_19_2, OBJ(int,2), CSU_DSU_CLK_RATE_9_6, "9.6",
           "9600",
           PRIV_CONF);

KEYWORD_ID(ci_csu_dsu_clk_rate_4_8, ci_csu_dsu_clk_rate_eol,
           ci_csu_dsu_clk_rate_9_6, OBJ(int,2), CSU_DSU_CLK_RATE_4_8, "4.8",
           "4800",
           PRIV_CONF);

KEYWORD_ID(ci_csu_dsu_clk_rate_2_4, ci_csu_dsu_clk_rate_eol,
           ci_csu_dsu_clk_rate_4_8, OBJ(int,2), CSU_DSU_CLK_RATE_2_4, "2.4",
           "2400",
           PRIV_CONF);

KEYWORD_ID(ci_csu_dsu_clk_rate_auto, ci_csu_dsu_clk_rate_eol,
           ci_csu_dsu_clk_rate_2_4, OBJ(int,2), CSU_DSU_CLK_RATE_AUTO, "auto",
           "Auto mode",
           PRIV_CONF);

NOPREFIX (ci_csu_dsu_clk_rate_nocheck, ci_csu_dsu_clk_rate_auto,
          ci_csu_dsu_clk_rate_eol);

NVGENS(ci_csu_dsu_clk_rate_nvgen, ci_csu_dsu_clk_rate_nocheck,
       csu_dsu_service_module_command, CSU_DSU_CLK_RATE);

KEYWORD(ci_csu_dsu_serv_clk_rate, ci_csu_dsu_clk_rate_nvgen, no_alt,
        "rate", "56K Module clock rate", PRIV_CONF);

IFELSE (ci_csu_dsu_clk_rate_56k_chk, ci_csu_dsu_serv_clk_rate,
        no_alt, (GETOBJ(int,1) == CSU_DSU_SW56_OPT));

/***************************************************************
 * [no] service-module t1 remote-alarm-enable 
 *
 * OBJ(int,1) = t1
 */

EOLS  (ci_csu_dsu_remote_alarm_ena_eol, csu_dsu_service_module_command, CSU_DSU_REMOTE_ALRM_ENA);

NVGENS(ci_csu_dsu_remote_alarm_ena_nvgen, ci_csu_dsu_remote_alarm_ena_eol,
       csu_dsu_service_module_command, CSU_DSU_REMOTE_ALRM_ENA);

KEYWORD(ci_csu_dsu_remote_alarm_ena, ci_csu_dsu_remote_alarm_ena_nvgen, no_alt,
        "remote-alarm-enable", "Remote/Yellow alarm enable", PRIV_CONF);

IFELSE (ci_csu_dsu_remote_alarm_t1_chk, ci_csu_dsu_remote_alarm_ena,
        no_alt, (GETOBJ(int,1) == CSU_DSU_FT1_OPT));

/***************************************************************
 * [no] service-module t1 timeslots {all | <range 1-24>}
 *				    [speed {56 | 64}]			     
 *
 * OBJ(int,1) = t1
 * OBJ(int,2) = all | range
 * OBJ(int,3) = range bit map <range 1-24>, 0 for all
 * OBJ(int,4) = 56 | 64 | Not-selected
 * OBJ(int,5) = temporary variable
 * OBJ(int,6) = temporary variable
 */
EOLS  (ci_csu_dsu_timeslots_eol, csu_dsu_service_module_command, CSU_DSU_TIMESLOTS);

KEYWORD_ID(ci_csu_dsu_timeslots_64, ci_csu_dsu_timeslots_eol,
           no_alt, OBJ(int,4), CSU_DSU_TS_SPEED_64, "64",
           "64K",
           PRIV_CONF);

KEYWORD_ID(ci_csu_dsu_timeslots_56, ci_csu_dsu_timeslots_eol,
           ci_csu_dsu_timeslots_64, OBJ(int,4), CSU_DSU_TS_SPEED_56, "56",
           "56K",
           PRIV_CONF);

KEYWORD (ci_csu_dsu_timeslots_speed, ci_csu_dsu_timeslots_56, 
	 ci_csu_dsu_timeslots_eol, "speed", "Fraction timeslots speed", PRIV_CONF);

SET (ci_csu_dsu_ts_speed_set_def, ci_csu_dsu_timeslots_speed,
     OBJ(int,4), CSU_DSU_TS_SPEED_NOSEL);

CHARACTER (ci_csu_dsu_timeslots_range_num4, ci_csu_dsu_timeslots_range_num1w, 
	   ci_csu_dsu_ts_speed_set_def, ',');

EVAL (ci_csu_dsu_timeslots_range_num3a, ci_csu_dsu_timeslots_range_num4, SETOBJ(int,3) =
      GETOBJ(int,3) | csu_dsu_make_bit_field(GETOBJ(int,5), GETOBJ(int,6)));

GENERAL_NUMBER (ci_csu_dsu_timeslots_range_num3, ci_csu_dsu_timeslots_range_num3a, 
	        no_alt, OBJ(int,6), 1, 24, "List of T1-channel timeslots is set for 24",
                NUMBER_DEC| NUMBER_WS_OK | NUMBER_NO_WS_OK | NUMBER_HELP_CHECK);

WHITESPACE (ci_csu_dsu_timeslots_range_num1w, ci_csu_dsu_timeslots_range_num1, 
	    ci_csu_dsu_timeslots_range_num1);

WHITESPACE (ci_csu_dsu_timeslots_range_num3w, ci_csu_dsu_timeslots_range_num3, 
	    ci_csu_dsu_timeslots_range_num3);

CHARACTER (ci_csu_dsu_timeslots_range_num2a, ci_csu_dsu_timeslots_range_num1w, 
	   ci_csu_dsu_ts_speed_set_def, ',');

CHARACTER (ci_csu_dsu_timeslots_range_num2, ci_csu_dsu_timeslots_range_num3w, 
           ci_csu_dsu_timeslots_range_num2a, '-');

EVAL (ci_csu_dsu_timeslots_range_num1a, ci_csu_dsu_timeslots_range_num2,
      SETOBJ(int,3) =
           GETOBJ(int,3) | csu_dsu_make_bit_field(GETOBJ(int,5), GETOBJ(int,5)));

GENERAL_NUMBER (ci_csu_dsu_timeslots_range_num1, ci_csu_dsu_timeslots_range_num1a, no_alt, 
	        OBJ(int,5), 1, 24, "List of timeslots which comprise the T1-group",
                NUMBER_DEC| NUMBER_WS_OK | NUMBER_NO_WS_OK | NUMBER_HELP_CHECK);

SET (ci_csu_dsu_timeslots_range_num0, ci_csu_dsu_timeslots_range_num1, OBJ(int,3), 0);

KEYWORD_ID (ci_csu_dsu_timeslots_all, ci_csu_dsu_ts_speed_set_def, 
	    ci_csu_dsu_timeslots_range_num0, OBJ(int,2), CSU_DSU_TS_TYPE_ALL,
            "all", "All timeslots", PRIV_CONF);

SET (ci_csu_dsu_timeslots_type_def, ci_csu_dsu_timeslots_all, OBJ(int,2), 
     CSU_DSU_TS_TYPE_RANGE);

NOPREFIX (ci_csu_dsu_timeslots_nocheck, ci_csu_dsu_timeslots_type_def,
          ci_csu_dsu_timeslots_eol);

NVGENS(ci_csu_dsu_timeslots_nvgen, ci_csu_dsu_timeslots_nocheck,
       csu_dsu_service_module_command, CSU_DSU_TIMESLOTS);

KEYWORD(ci_csu_dsu_timeslots, ci_csu_dsu_timeslots_nvgen, 
	ci_csu_dsu_remote_alarm_t1_chk,
        "timeslots", "Fraction timeslots", PRIV_CONF);

IFELSE (ci_csu_dsu_ts_t1_chk, ci_csu_dsu_timeslots,
        ci_csu_dsu_remote_alarm_t1_chk, (GETOBJ(int,1) == CSU_DSU_FT1_OPT));

/***************************************************************
 * [no] service-module t1 lbo {none | -7.5db | -15db}
 *
 * OBJ(int,1) = t1
 * OBJ(int,2) = none | -7.5db | -15db
 */
EOLS  (ci_csu_dsu_lbo_eol, csu_dsu_service_module_command, CSU_DSU_LBO);

KEYWORD_ID(ci_csu_dsu_lbo_15db, ci_csu_dsu_lbo_eol,
           NONE, OBJ(int,2), CSU_DSU_LBO_15DB, "-15db",
           "-15 DB",
           PRIV_CONF);

KEYWORD_ID(ci_csu_dsu_lbo_7_5db, ci_csu_dsu_lbo_eol,
           ci_csu_dsu_lbo_15db, OBJ(int,2), CSU_DSU_LBO_7_5_DB, "-7.5db",
           "-7.5 DB",
           PRIV_CONF);

KEYWORD_ID(ci_csu_dsu_lbo_none, ci_csu_dsu_lbo_eol,
           ci_csu_dsu_lbo_7_5db, OBJ(int,2), CSU_DSU_LBO_NONE, "none",
           "None",
           PRIV_CONF);

NOPREFIX (ci_csu_dsu_lbo_nocheck, ci_csu_dsu_lbo_none,
          ci_csu_dsu_lbo_eol);

NVGENS(ci_csu_dsu_lbo_nvgen, ci_csu_dsu_lbo_nocheck,
       csu_dsu_service_module_command, CSU_DSU_LBO);

KEYWORD(ci_csu_dsu_lbo, ci_csu_dsu_lbo_nvgen, ci_csu_dsu_ts_t1_chk,
        "lbo", "Line Build Out format", PRIV_CONF);

IFELSE (ci_csu_dsu_lbo_t1_chk, ci_csu_dsu_lbo,
        ci_csu_dsu_ts_t1_chk, (GETOBJ(int,1) == CSU_DSU_FT1_OPT));

/***************************************************************
 * [no] service-module t1 linecode {b8zs | ami}
 *
 * OBJ(int,1) = t1
 * OBJ(int,2) = b8zs | ami
 */
EOLS  (ci_csu_dsu_linecode_eol, csu_dsu_service_module_command, CSU_DSU_LINECODE);

KEYWORD_ID(ci_csu_dsu_linecode_ami, ci_csu_dsu_linecode_eol,
           NONE, OBJ(int,2), CSU_DSU_LINECODE_AMI, "ami",
           "AMI format",
           PRIV_CONF);

KEYWORD_ID(ci_csu_dsu_linecode_b8zs, ci_csu_dsu_linecode_eol,
           ci_csu_dsu_linecode_ami, OBJ(int,2), CSU_DSU_LINECODE_B8ZS, "b8zs",
           "B8ZS format",
           PRIV_CONF);

NOPREFIX (ci_csu_dsu_linecode_nocheck, ci_csu_dsu_linecode_b8zs,
          ci_csu_dsu_linecode_eol);

NVGENS(ci_csu_dsu_linecode_nvgen, ci_csu_dsu_linecode_nocheck,
       csu_dsu_service_module_command, CSU_DSU_LINECODE);

KEYWORD(ci_csu_dsu_linecode, ci_csu_dsu_linecode_nvgen, ci_csu_dsu_lbo_t1_chk,
        "linecode", "Line coding format", PRIV_CONF);

IFELSE (ci_csu_dsu_linecode_t1_chk, ci_csu_dsu_linecode,
        ci_csu_dsu_lbo_t1_chk, (GETOBJ(int,1) == CSU_DSU_FT1_OPT));

/***************************************************************
 * [no] service-module t1 framing {esf | sf}
 *
 * OBJ(int,1) = t1
 * OBJ(int,2) = esf | sf
 */
EOLS  (ci_csu_dsu_framing_eol, csu_dsu_service_module_command, CSU_DSU_FRAMING);

KEYWORD_ID(ci_csu_dsu_framing_sf, ci_csu_dsu_framing_eol,
           NONE, OBJ(int,2), CSU_DSU_FRAMING_SF, "sf",
           "SF format",
           PRIV_CONF);

KEYWORD_ID(ci_csu_dsu_framing_esf, ci_csu_dsu_framing_eol,
           ci_csu_dsu_framing_sf, OBJ(int,2), CSU_DSU_FRAMING_ESF, "esf",
           "ESF format",
           PRIV_CONF);

NOPREFIX (ci_csu_dsu_framing_nocheck, ci_csu_dsu_framing_esf,
          ci_csu_dsu_framing_eol);

NVGENS(ci_csu_dsu_framing_nvgen, ci_csu_dsu_framing_nocheck,
       csu_dsu_service_module_command, CSU_DSU_FRAMING);

KEYWORD(ci_csu_dsu_framing, ci_csu_dsu_framing_nvgen, 
	ci_csu_dsu_linecode_t1_chk,
        "framing", "Framing format", PRIV_CONF);

IFELSE (ci_csu_dsu_framing_t1_chk, ci_csu_dsu_framing,
        ci_csu_dsu_linecode_t1_chk, (GETOBJ(int,1) == CSU_DSU_FT1_OPT));

/***************************************************************
 * [no] service-module t1 remote-loopback {full | payload}
 *                                        {<cr> | alternate | v54}
 * [no] service-module 56k remote-loopback
 *
 * OBJ(int,1) = 56k | t1
 * OBJ(int,2) = full | fractional 
 * OBJ(int,3) = <cr> | alternate | v54
 */
EOLS  (ci_csu_dsu_rmt_lpback_full_eol, csu_dsu_service_module_command, CSU_DSU_RMT_LPBACK_T1_FULL);

EOLS  (ci_csu_dsu_rmt_lpback_fr_eol, csu_dsu_service_module_command, CSU_DSU_RMT_LPBACK_T1_FR);

KEYWORD_ID(ci_csu_dsu_rmt_lpback_v54, ci_csu_dsu_rmt_lpback_fr_eol,
           ci_csu_dsu_rmt_lpback_fr_eol, OBJ(int,3), CSU_DSU_RMT_LPBACK_TYPE_V54, "v54",
           "V54 mode",
           PRIV_CONF);

IFELSE (ci_csu_dsu_rmt_lpback_fr_chk, ci_csu_dsu_rmt_lpback_v54,
        ci_csu_dsu_rmt_lpback_full_eol, (GETOBJ(int,2) == CSU_DSU_RMT_LPBACK_MODE_FRACT));

IFELSE (ci_csu_dsu_rmt_lpback_full_chk, ci_csu_dsu_rmt_lpback_full_eol,
        ci_csu_dsu_rmt_lpback_fr_eol, (GETOBJ(int,2) == CSU_DSU_RMT_LPBACK_MODE_FULL));

KEYWORD_ID(ci_csu_dsu_rmt_lpback_alt, ci_csu_dsu_rmt_lpback_full_chk,
           ci_csu_dsu_rmt_lpback_fr_chk, OBJ(int,3), CSU_DSU_RMT_LPBACK_TYPE_ALT, "alternate",
           "ALTERNATE mode",
           PRIV_CONF);

SET (ci_csu_dsu_rmt_lpback_t1_set_def1, ci_csu_dsu_rmt_lpback_alt, 
     OBJ(int,3), CSU_DSU_RMT_LPBACK_TYPE_DEF);

NOPREFIX (ci_csu_dsu_rmt_lpback_fr_nocheck, ci_csu_dsu_rmt_lpback_t1_set_def1,
          ci_csu_dsu_rmt_lpback_fr_eol);

NVGENS(ci_csu_dsu_rmt_lpback_fr_nvgen, ci_csu_dsu_rmt_lpback_fr_nocheck,
       csu_dsu_service_module_command, CSU_DSU_RMT_LPBACK_T1_FR);

KEYWORD_ID(ci_csu_dsu_rmt_lpback_fract, ci_csu_dsu_rmt_lpback_fr_nvgen,
           NONE, OBJ(int,2), CSU_DSU_RMT_LPBACK_MODE_FRACT, "payload",
           "Payload T1", PRIV_CONF);

NOPREFIX (ci_csu_dsu_rmt_lpback_full_nocheck, ci_csu_dsu_rmt_lpback_t1_set_def1,
          ci_csu_dsu_rmt_lpback_full_eol);

NVGENS(ci_csu_dsu_rmt_lpback_full_nvgen, ci_csu_dsu_rmt_lpback_full_nocheck,
       csu_dsu_service_module_command, CSU_DSU_RMT_LPBACK_T1_FULL);

KEYWORD_ID(ci_csu_dsu_rmt_lpback_full, ci_csu_dsu_rmt_lpback_full_nvgen,
           ci_csu_dsu_rmt_lpback_fract, OBJ(int,2), CSU_DSU_RMT_LPBACK_MODE_FULL, "full",
           "Full T1", PRIV_CONF);

EOLS  (ci_csu_dsu_rmt_lpback_56k_eol, csu_dsu_service_module_command, CSU_DSU_RMT_LPBACK_SW56);

SET (ci_csu_dsu_rmt_lpback_56k_set_def1, ci_csu_dsu_rmt_lpback_56k_eol, 
     OBJ(int,2), CSU_DSU_RMT_LPBACK_MODE_DEF);

NOPREFIX (ci_csu_dsu_rmt_lpback_56k_nocheck, ci_csu_dsu_rmt_lpback_56k_set_def1,
          ci_csu_dsu_rmt_lpback_56k_eol);

NVGENS(ci_csu_dsu_rmt_lpback_56k_nvgen, ci_csu_dsu_rmt_lpback_56k_nocheck,
       csu_dsu_service_module_command, CSU_DSU_RMT_LPBACK_SW56);

IFELSE (ci_csu_dsu_rmt_lpback_56k_chk, ci_csu_dsu_rmt_lpback_56k_nvgen,
        ci_csu_dsu_rmt_lpback_full, (GETOBJ(int,1) == CSU_DSU_SW56_OPT));

KEYWORD(ci_csu_dsu_rmt_lpback, ci_csu_dsu_rmt_lpback_56k_chk, 
	ci_csu_dsu_framing_t1_chk,
        "remote-loopback", "Remote Loopback/RL disable", PRIV_CONF);

/***************************************************************
 * [no] service-module 56k switched-carrier {att | sprint | other}
 *
 * OBJ(int,1) = 56k 
 * OBJ(int,2) = att | sprint | other 
 */
EOLS  (ci_csu_dsu_sw_carrier_eol, csu_dsu_service_module_command, CSU_DSU_SW_CARRIER);

KEYWORD_ID(ci_csu_dsu_sw_carrier_other, ci_csu_dsu_sw_carrier_eol,
           NONE, OBJ(int,2), CSU_DSU_SW_CARRIER_OTHER, "other",
           "OTHER",
           PRIV_CONF);

KEYWORD_ID(ci_csu_dsu_sw_carrier_sprint, ci_csu_dsu_sw_carrier_eol,
           ci_csu_dsu_sw_carrier_other, OBJ(int,2), CSU_DSU_SW_CARRIER_SPRINT, "sprint",
           "SPRINT",
           PRIV_CONF);

KEYWORD_ID(ci_csu_dsu_sw_carrier_att, ci_csu_dsu_sw_carrier_eol,
           ci_csu_dsu_sw_carrier_sprint, OBJ(int,2), CSU_DSU_SW_CARRIER_ATT, "att",
           "AT&T",
           PRIV_CONF);

NOPREFIX (ci_csu_dsu_sw_carrier_nocheck, ci_csu_dsu_sw_carrier_att,
          ci_csu_dsu_sw_carrier_eol);

NVGENS(ci_csu_dsu_sw_carrier_nvgen, ci_csu_dsu_sw_carrier_nocheck,
       csu_dsu_service_module_command, CSU_DSU_SW_CARRIER);

KEYWORD(ci_csu_dsu_sw_carrier, ci_csu_dsu_sw_carrier_nvgen, 
	ci_csu_dsu_rmt_lpback,
        "switched-carrier", "Service module long distance carrier", PRIV_CONF);

IFELSE (ci_csu_dsu_sw_carrier_56k_chk, ci_csu_dsu_sw_carrier,
        ci_csu_dsu_rmt_lpback, (GETOBJ(int,1) == CSU_DSU_SW56_OPT));

/***************************************************************
 * [no] service-module 56k network-type {dds | switched}
 *
 * OBJ(int,1) = 56k | t1
 * OBJ(int,2) = dds | switched  for 56k
 */
EOLS  (ci_csu_dsu_nwktype_eol, csu_dsu_service_module_command, CSU_DSU_NWKTYPE);


KEYWORD_ID(ci_csu_dsu_nwktype_dds, ci_csu_dsu_nwktype_eol,
           NONE, OBJ(int,2), CSU_DSU_NWKTYPE_DDS, "dds",
           "DDS mode",
           PRIV_CONF);

KEYWORD_ID(ci_csu_dsu_nwktype_switched, ci_csu_dsu_nwktype_eol,
           ci_csu_dsu_nwktype_dds, OBJ(int,2), CSU_DSU_NWKTYPE_SWITCHED, "switched",
           "Switched mode",
           PRIV_CONF);

NOPREFIX (ci_csu_dsu_nwktype_nocheck, ci_csu_dsu_nwktype_switched,
          ci_csu_dsu_nwktype_eol);

NVGENS(ci_csu_dsu_nwktype_nvgen, ci_csu_dsu_nwktype_nocheck,
       csu_dsu_service_module_command, CSU_DSU_NWKTYPE);

KEYWORD(ci_csu_dsu_nwktype, ci_csu_dsu_nwktype_nvgen, no_alt,
        "network-type", "Service module Network Type/DACS mode", PRIV_CONF);

ASSERT (ci_csu_dsu_nwktype_chk, ci_csu_dsu_nwktype,
        ci_csu_dsu_sw_carrier_56k_chk,
        reg_invoke_csu_dsu_installed(CHK_SW56_4W_CSU_DSU, csb->interface->hwptr)); 

/***************************************************************
 * [no] service-module t1 data-coding {normal | inverted}
 * [no] service-module 56k data-coding {normal | scrambled}
 *
 * OBJ(int,1) = 56k | t1
 * OBJ(int,2) = normal | inverted  for t1
 *      or
 * OBJ(int,2) = normal | scrambled for 4 wires 56K only
 */
EOLS  (ci_csu_dsu_dcode_eol, csu_dsu_service_module_command, CSU_DSU_DCODE);

KEYWORD_ID(ci_csu_dsu_dcode_scrambled, ci_csu_dsu_dcode_eol,
           NONE, OBJ(int,2), CSU_DSU_DCODE_SCRAMBLED, "scrambled",
           "Scrambled data decoding",
           PRIV_CONF);

KEYWORD_ID(ci_csu_dsu_dcode_inverted, ci_csu_dsu_dcode_eol,
           NONE, OBJ(int,2), CSU_DSU_DCODE_INVERTED, "inverted",
           "Inverted data decoding",
           PRIV_CONF);

IFELSE (ci_csu_dsu_dcode_scr_56k_chk, ci_csu_dsu_dcode_scrambled,
        ci_csu_dsu_dcode_inverted, (GETOBJ(int,1) == CSU_DSU_SW56_OPT));

KEYWORD_ID(ci_csu_dsu_dcode_normal, ci_csu_dsu_dcode_eol,
           ci_csu_dsu_dcode_scr_56k_chk, OBJ(int,2), CSU_DSU_DCODE_NORMAL, "normal",
           "Normal data decoding",
           PRIV_CONF);

NOPREFIX (ci_csu_dsu_dcode_nocheck, ci_csu_dsu_dcode_normal,
          ci_csu_dsu_dcode_eol);

NVGENS(ci_csu_dsu_dcode_nvgen, ci_csu_dsu_dcode_nocheck,
       csu_dsu_service_module_command, CSU_DSU_DCODE);

KEYWORD(ci_csu_dsu_dcode, ci_csu_dsu_dcode_nvgen, no_alt,
       "data-coding", "Service module DCE line coding", PRIV_CONF);

ASSERT (ci_csu_dsu_dcode_chk, ci_csu_dsu_dcode,
        ci_csu_dsu_nwktype_chk,
        reg_invoke_csu_dsu_installed(CHK_FT1_OR_SW56_4W_CSU_DSU, csb->interface->hwptr)); 

/***************************************************************
 * [no] service-module t1 clock soruce {line | internal}
 * [no] service-module 56k clock soruce {line | internal} 
 *
 * OBJ(int,1) = 56k | t1
 * OBJ(int,2) = line | internal for T1 and 4 wires 56K only
 */
EOLS  (ci_csu_dsu_clocksource_eol, csu_dsu_service_module_command, CSU_DSU_CLK_SRC);

KEYWORD_ID(ci_csu_dsu_clk_src_line, ci_csu_dsu_clocksource_eol,
           NONE, OBJ(int,2), CSU_DSU_CLK_SRC_LINE, "line",
           "Recover clock from line",
           PRIV_CONF);

KEYWORD_ID(ci_csu_dsu_clk_src_internal, ci_csu_dsu_clocksource_eol,
           ci_csu_dsu_clk_src_line, OBJ(int,2), CSU_DSU_CLK_SRC_INT, "internal",
           "Use adapter internal clock",
           PRIV_CONF);

NOPREFIX (ci_csu_dsu_clk_src_nocheck, ci_csu_dsu_clk_src_internal,
          ci_csu_dsu_clocksource_eol);

NVGENS(ci_csu_dsu_clk_src_nvgen, ci_csu_dsu_clk_src_nocheck,
       csu_dsu_service_module_command, CSU_DSU_CLK_SRC);

KEYWORD(ci_csu_dsu_serv_clk_src, ci_csu_dsu_clk_src_nvgen, 
	ci_csu_dsu_clk_rate_56k_chk,
        "source", "Module clock source", PRIV_CONF);

KEYWORD(ci_csu_dsu_serv_clock, ci_csu_dsu_serv_clk_src, no_alt,
       "clock", "Service module clock source", PRIV_CONF);

ASSERT (ci_csu_dsu_clk_src_chk, ci_csu_dsu_serv_clock,
        ci_csu_dsu_dcode_chk,
        reg_invoke_csu_dsu_installed(CHK_FT1_OR_SW56_4W_CSU_DSU, csb->interface->hwptr)); 

/***************************************************************
 * [no] service-module t1 
 * [no] service-module 56k 
 *
 * OBJ(int,1) = 56k | t1
 */

SET (ci_csu_dsu_serv_t1_set, ci_csu_dsu_clk_src_chk, OBJ(int,1), CSU_DSU_FT1_OPT);

KEYWORD(ci_csu_dsu_serv_t1, ci_csu_dsu_serv_t1_set, no_alt,
        "t1", "T1", PRIV_CONF);

ASSERT (ci_csu_dsu_t1_chk, ci_csu_dsu_serv_t1,  no_alt,
        (csb->interface->hwptr->csu_dsu_type & CSU_DSU_FT1));

SET (ci_csu_dsu_serv_56k_set, ci_csu_dsu_clk_src_chk, OBJ(int,1), CSU_DSU_SW56_OPT);

KEYWORD(ci_csu_dsu_serv_56k, ci_csu_dsu_serv_56k_set, no_alt,
        "56k", "56K switching", PRIV_CONF);

ASSERT (ci_csu_dsu_56k_chk, ci_csu_dsu_serv_56k, ci_csu_dsu_t1_chk,
        (csb->interface->hwptr->csu_dsu_type & CSU_DSU_SW56));

KEYWORD(ci_csu_dsu, ci_csu_dsu_56k_chk, ALTERNATE, 
 	"service-module", "Service module configuration subcommands", PRIV_CONF);

ASSERT (ci_csu_dsu_int, ci_csu_dsu, ALTERNATE,
        ((csb->interface->hwptr->status & IDB_SERIAL) &&
         ((csb->interface->hwptr->csu_dsu_type & CSU_DSU_FT1) ||
	  (csb->interface->hwptr->csu_dsu_type & CSU_DSU_SW56)))); 

#undef	ALTERNATE
#define	ALTERNATE	ci_csu_dsu_int
