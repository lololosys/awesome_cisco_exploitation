/* 
 * $Id: cfg_appn_cos.h,v 3.1.40.2 1996/09/05 19:25:51 mregan Exp $
 * $Source: /release/112/cvs/Xsys/appn/cfg_appn_cos.h,v $
 *------------------------------------------------------------------
 *
 * Configuration Commands for APPN
 *
 * August 1994, Dave McCowan
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 *
 * $Log: cfg_appn_cos.h,v $
 * Revision 3.1.40.2  1996/09/05  19:25:51  mregan
 * CSCdi67560:  Cfg-ing class of service (COS) can cause errors
 * Branch: California_branch
 * Correct configurable value for COSs.
 *
 * Revision 3.1.40.1  1996/08/12  16:00:14  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.1  1996/02/01  04:56:45  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 1: Create copies of the files to be moved in their new location.
 *
 * Revision 3.3  1995/12/01  15:56:48  widmer
 * CSCdi44724:  Remove nv_top argument to parser_add_mode()
 * Clean up "no" handling at beginning of modes
 * Fix appn modes
 * Removed unneded pdecl's
 *
 * Revision 3.2  1995/11/17  18:42:09  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:49:36  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:01:25  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 * $Endlog$
*/



/******************************************************************
 * appn cos <cosname>
 *    ...subcommands...
 */


/*
 *   exit
 */
EOLNS	(appn_cos_exit_eol, exit_appn_command);
KEYWORD	(appn_cos_exit_kwd, appn_cos_exit_eol, NONE,
	 "exit", "Exit from appn-cos configuration mode", PRIV_CONF);
TESTVAR	(appn_cos_exit, NONE, appn_cos_exit_kwd, NONE, NONE, ALTERNATE,
	 sense, FALSE);
#undef  ALTERNATE
#define ALTERNATE appn_cos_exit

/*
 *   [no] complete
 */
EOLS      (cfg_appn_cos_comp_eol, appn_command, APPN_COS_COMP);
KEYWORD_ID(cfg_appn_cos_comp, cfg_appn_cos_comp_eol, ALTERNATE,
           appn_subcommand_type, APPN_COS_COMP,
           "complete", "Complete COS definition.", PRIV_CONF);
#undef  ALTERNATE
#define ALTERNATE cfg_appn_cos_comp


/* [no] tg-row <index> weight <w> byte <l> <h> time <l> <h>
 *         capacity <l> <h> delay <l> <h> security <l> <h>
 *         user1 <l> <h> user2 <l> <h> user3 <l> <h>
 */
EOLS      (cfg_appn_cos_tg_eol, appn_command, APPN_COS_TG);
NUMBER    (cfg_appn_cos_user3_hi, cfg_appn_cos_tg_eol, no_alt,
           OBJ(int,18), 0, 255, APPN_COS_USER3_HI_VAL_HELP);
NUMBER    (cfg_appn_cos_user3_low, cfg_appn_cos_user3_hi, no_alt,
           OBJ(int,17), 0, 255, APPN_COS_USER3_LOW_VAL_HELP);
KEYWORD   (cfg_appn_cos_user3_key, cfg_appn_cos_user3_low,
           no_alt,
           APPN_COS_USER3_KEY, 
           APPN_COS_USER3_KEY_HELP, PRIV_CONF);
NUMBER    (cfg_appn_cos_user2_hi, cfg_appn_cos_user3_key, no_alt,
           OBJ(int,16), 0, 255, APPN_COS_USER2_HI_VAL_HELP);
NUMBER    (cfg_appn_cos_user2_low, cfg_appn_cos_user2_hi, no_alt,
           OBJ(int,15), 0, 255, APPN_COS_USER2_LOW_VAL_HELP);
KEYWORD   (cfg_appn_cos_user2_key, cfg_appn_cos_user2_low,
           no_alt,
           APPN_COS_USER2_KEY, 
           APPN_COS_USER2_KEY_HELP, PRIV_CONF);
NUMBER    (cfg_appn_cos_user1_hi, cfg_appn_cos_user2_key, no_alt,
           OBJ(int,14), 0, 255, APPN_COS_USER1_HI_VAL_HELP);
NUMBER    (cfg_appn_cos_user1_low, cfg_appn_cos_user1_hi, no_alt,
           OBJ(int,13), 0, 255, APPN_COS_USER1_LOW_VAL_HELP);
KEYWORD   (cfg_appn_cos_user1_key, cfg_appn_cos_user1_low,
           no_alt,
           APPN_COS_USER1_KEY, 
           APPN_COS_USER1_KEY_HELP, PRIV_CONF);
NUMBER    (cfg_appn_cos_security_hi_other, cfg_appn_cos_user1_key, no_alt,
           OBJ(int,12), 0, 255, APPN_COS_SECURITY_HI_VAL_HELP);
KEYWORD_ID(cfg_appn_cos_security_hi_rad, cfg_appn_cos_user1_key,
           cfg_appn_cos_security_hi_other,
           OBJ(int,12), AP_SEC_GUARDED_RADIATION,
           APPN_PORT_SECUR_RAD, 
           APPN_PORT_SECUR_RAD_KEY_HELP, PRIV_CONF);
KEYWORD_ID(cfg_appn_cos_security_hi_enc, cfg_appn_cos_user1_key,
           cfg_appn_cos_security_hi_rad,
           OBJ(int,12), AP_SEC_ENCRYPTED,
           APPN_PORT_SECUR_ENC, 
           APPN_PORT_SECUR_ENC_KEY_HELP, PRIV_CONF);
KEYWORD_ID(cfg_appn_cos_security_hi_gua, cfg_appn_cos_user1_key,
           cfg_appn_cos_security_hi_enc,
           OBJ(int,12), AP_SEC_GUARDED_CONDUIT,
           APPN_PORT_SECUR_GUA, 
           APPN_PORT_SECUR_GUA_KEY_HELP, PRIV_CONF);
KEYWORD_ID(cfg_appn_cos_security_hi_sec, cfg_appn_cos_user1_key,
           cfg_appn_cos_security_hi_gua,
           OBJ(int,12), AP_SEC_SECURE_CONDUIT,
           APPN_PORT_SECUR_SEC, 
           APPN_PORT_SECUR_SEC_KEY_HELP, PRIV_CONF);
KEYWORD_ID(cfg_appn_cos_security_hi_und, cfg_appn_cos_user1_key,
           cfg_appn_cos_security_hi_sec,
           OBJ(int,12), AP_SEC_UNDERGROUND_CABLE,
           APPN_PORT_SECUR_UND, 
           APPN_PORT_SECUR_UND_KEY_HELP, PRIV_CONF);
KEYWORD_ID(cfg_appn_cos_security_hi_pub, cfg_appn_cos_user1_key,
           cfg_appn_cos_security_hi_und,
           OBJ(int,12), AP_SEC_PUBLIC_SWITCHED_NETWORK,
           APPN_PORT_SECUR_PUB, 
           APPN_PORT_SECUR_PUB_KEY_HELP, PRIV_CONF);
KEYWORD_ID(cfg_appn_cos_security_hi_non, cfg_appn_cos_user1_key,
           cfg_appn_cos_security_hi_pub,
           OBJ(int,12), AP_SEC_NONSECURE,
           APPN_PORT_SECUR_NON, 
           APPN_PORT_SECUR_NON_KEY_HELP, PRIV_CONF);
NUMBER    (cfg_appn_cos_security_low_other, cfg_appn_cos_security_hi_non, no_alt,
           OBJ(int,11), 0, 255, APPN_COS_SECURITY_LOW_VAL_HELP);
KEYWORD_ID(cfg_appn_cos_security_low_rad, cfg_appn_cos_security_hi_non,
           cfg_appn_cos_security_low_other,
           OBJ(int,11), AP_SEC_GUARDED_RADIATION,
           APPN_PORT_SECUR_RAD, 
           APPN_PORT_SECUR_RAD_KEY_HELP, PRIV_CONF);
KEYWORD_ID(cfg_appn_cos_security_low_enc, cfg_appn_cos_security_hi_non,
           cfg_appn_cos_security_low_rad,
           OBJ(int,11), AP_SEC_ENCRYPTED,
           APPN_PORT_SECUR_ENC, 
           APPN_PORT_SECUR_ENC_KEY_HELP, PRIV_CONF);
KEYWORD_ID(cfg_appn_cos_security_low_gua, cfg_appn_cos_security_hi_non,
           cfg_appn_cos_security_low_enc,
           OBJ(int,11), AP_SEC_GUARDED_CONDUIT,
           APPN_PORT_SECUR_GUA, 
           APPN_PORT_SECUR_GUA_KEY_HELP, PRIV_CONF);
KEYWORD_ID(cfg_appn_cos_security_low_sec, cfg_appn_cos_security_hi_non,
           cfg_appn_cos_security_low_gua,
           OBJ(int,11), AP_SEC_SECURE_CONDUIT,
           APPN_PORT_SECUR_SEC, 
           APPN_PORT_SECUR_SEC_KEY_HELP, PRIV_CONF);
KEYWORD_ID(cfg_appn_cos_security_low_und, cfg_appn_cos_security_hi_non,
           cfg_appn_cos_security_low_sec,
           OBJ(int,11), AP_SEC_UNDERGROUND_CABLE,
           APPN_PORT_SECUR_UND, 
           APPN_PORT_SECUR_UND_KEY_HELP, PRIV_CONF);
KEYWORD_ID(cfg_appn_cos_security_low_pub, cfg_appn_cos_security_hi_non,
           cfg_appn_cos_security_low_und,
           OBJ(int,11), AP_SEC_PUBLIC_SWITCHED_NETWORK,
           APPN_PORT_SECUR_PUB, 
           APPN_PORT_SECUR_PUB_KEY_HELP, PRIV_CONF);
KEYWORD_ID(cfg_appn_cos_security_low_non, cfg_appn_cos_security_hi_non,
           cfg_appn_cos_security_low_pub,
           OBJ(int,11), AP_SEC_NONSECURE,
           APPN_PORT_SECUR_NON, 
           APPN_PORT_SECUR_NON_KEY_HELP, PRIV_CONF);

KEYWORD   (cfg_appn_cos_security_key, cfg_appn_cos_security_low_non,
           no_alt,
           APPN_COS_SECURITY_KEY, 
           APPN_COS_SECURITY_KEY_HELP, PRIV_CONF);

NUMBER    (cfg_appn_cos_delay_hi_other, cfg_appn_cos_security_key, no_alt,
           OBJ(int,10), 0, 255, APPN_COS_DELAY_HI_VAL_HELP);
KEYWORD_ID(cfg_appn_cos_delay_hi_max, cfg_appn_cos_security_key,
           cfg_appn_cos_delay_hi_other,
           OBJ(int,10), AP_PROP_DELAY_MAXIMUM,
           APPN_PORT_DELAY_MAX, 
           APPN_PORT_DELAY_MAX_KEY_HELP, PRIV_CONF);
KEYWORD_ID(cfg_appn_cos_delay_hi_sat, cfg_appn_cos_security_key,
           cfg_appn_cos_delay_hi_max,
           OBJ(int,10), AP_PROP_DELAY_SATELLITE,
           APPN_PORT_DELAY_SAT, 
           APPN_PORT_DELAY_SAT_KEY_HELP, PRIV_CONF);
KEYWORD_ID(cfg_appn_cos_delay_hi_pack, cfg_appn_cos_security_key,
           cfg_appn_cos_delay_hi_sat,
           OBJ(int,10), AP_PROP_DELAY_PKT_SWITCHED_NET,
           APPN_PORT_DELAY_PACK, 
           APPN_PORT_DELAY_PACK_KEY_HELP, PRIV_CONF);
KEYWORD_ID(cfg_appn_cos_delay_hi_phone, cfg_appn_cos_security_key,
           cfg_appn_cos_delay_hi_pack,
           OBJ(int,10), AP_PROP_DELAY_TELEPHONE,
           APPN_PORT_DELAY_PHONE, 
           APPN_PORT_DELAY_PHONE_KEY_HELP, PRIV_CONF);
KEYWORD_ID(cfg_appn_cos_delay_hi_lan, cfg_appn_cos_security_key,
           cfg_appn_cos_delay_hi_phone,
           OBJ(int,10), AP_PROP_DELAY_LAN,
           APPN_PORT_DELAY_LAN, 
           APPN_PORT_DELAY_LAN_KEY_HELP, PRIV_CONF);
KEYWORD_ID(cfg_appn_cos_delay_hi_min, cfg_appn_cos_security_key,
           cfg_appn_cos_delay_hi_lan,
           OBJ(int,10), AP_PROP_DELAY_MINIMUM,
           APPN_PORT_DELAY_MIN, 
           APPN_PORT_DELAY_MIN_KEY_HELP, PRIV_CONF);
NUMBER    (cfg_appn_cos_delay_low_other, cfg_appn_cos_delay_hi_min, no_alt,
           OBJ(int,9), 0, 255, APPN_COS_DELAY_LOW_VAL_HELP);
KEYWORD_ID(cfg_appn_cos_delay_low_max, cfg_appn_cos_delay_hi_min,
           cfg_appn_cos_delay_low_other,
           OBJ(int,9), AP_PROP_DELAY_MAXIMUM,
           APPN_PORT_DELAY_MAX, 
           APPN_PORT_DELAY_MAX_KEY_HELP, PRIV_CONF);
KEYWORD_ID(cfg_appn_cos_delay_low_sat, cfg_appn_cos_delay_hi_min,
           cfg_appn_cos_delay_low_max,
           OBJ(int,9), AP_PROP_DELAY_SATELLITE,
           APPN_PORT_DELAY_SAT, 
           APPN_PORT_DELAY_SAT_KEY_HELP, PRIV_CONF);
KEYWORD_ID(cfg_appn_cos_delay_low_pack, cfg_appn_cos_delay_hi_min,
           cfg_appn_cos_delay_low_sat,
           OBJ(int,9), AP_PROP_DELAY_PKT_SWITCHED_NET,
           APPN_PORT_DELAY_PACK, 
           APPN_PORT_DELAY_PACK_KEY_HELP, PRIV_CONF);
KEYWORD_ID(cfg_appn_cos_delay_low_phone, cfg_appn_cos_delay_hi_min,
           cfg_appn_cos_delay_low_pack,
           OBJ(int,9), AP_PROP_DELAY_TELEPHONE,
           APPN_PORT_DELAY_PHONE, 
           APPN_PORT_DELAY_PHONE_KEY_HELP, PRIV_CONF);
KEYWORD_ID(cfg_appn_cos_delay_low_lan, cfg_appn_cos_delay_hi_min,
           cfg_appn_cos_delay_low_phone,
           OBJ(int,9), AP_PROP_DELAY_LAN,
           APPN_PORT_DELAY_LAN, 
           APPN_PORT_DELAY_LAN_KEY_HELP, PRIV_CONF);
KEYWORD_ID(cfg_appn_cos_delay_low_min, cfg_appn_cos_delay_hi_min,
           cfg_appn_cos_delay_low_lan,
           OBJ(int,9), AP_PROP_DELAY_MINIMUM,
           APPN_PORT_DELAY_MIN, 
           APPN_PORT_DELAY_MIN_KEY_HELP, PRIV_CONF);
KEYWORD   (cfg_appn_cos_delay_key, cfg_appn_cos_delay_low_min,
           no_alt,
           APPN_COS_DELAY_KEY, 
           APPN_COS_DELAY_KEY_HELP, PRIV_CONF);
NUMBER    (cfg_appn_cos_capacity_hi, cfg_appn_cos_delay_key, no_alt,
           OBJ(int,8), 0, 255, APPN_COS_CAPACITY_HI_VAL_HELP);
NUMBER    (cfg_appn_cos_capacity_low, cfg_appn_cos_capacity_hi, no_alt,
           OBJ(int,7), 0, 255, APPN_COS_CAPACITY_LOW_VAL_HELP);
KEYWORD   (cfg_appn_cos_capacity_key, cfg_appn_cos_capacity_low,
           no_alt,
           APPN_COS_CAPACITY_KEY, 
           APPN_COS_CAPACITY_KEY_HELP, PRIV_CONF);
NUMBER    (cfg_appn_cos_time_hi, cfg_appn_cos_capacity_key, no_alt,
           OBJ(int,6), 0, 255, APPN_COS_TIME_HI_VAL_HELP);
NUMBER    (cfg_appn_cos_time_low, cfg_appn_cos_time_hi, no_alt,
           OBJ(int,5), 0, 255, APPN_COS_TIME_LOW_VAL_HELP);
KEYWORD   (cfg_appn_cos_time_key, cfg_appn_cos_time_low,
           no_alt,
           APPN_COS_TIME_KEY, 
           APPN_COS_TIME_KEY_HELP, PRIV_CONF);
NUMBER    (cfg_appn_cos_byte_hi, cfg_appn_cos_time_key, no_alt,
           OBJ(int,4), 0, 255, APPN_COS_BYTE_HI_VAL_HELP);
NUMBER    (cfg_appn_cos_byte_low, cfg_appn_cos_byte_hi, no_alt,
           OBJ(int,3), 0, 255, APPN_COS_BYTE_LOW_VAL_HELP);
KEYWORD   (cfg_appn_cos_byte_key, cfg_appn_cos_byte_low,
           no_alt,
           APPN_COS_BYTE_KEY, 
           APPN_COS_BYTE_KEY_HELP, PRIV_CONF);
NUMBER    (cfg_appn_cos_tg_weight_val, cfg_appn_cos_byte_key, no_alt,
           OBJ(int,2), 0, 255, APPN_COS_TG_WEIGHT_VAL_HELP);
KEYWORD   (cfg_appn_cos_tg_weight, cfg_appn_cos_tg_weight_val,
           no_alt,
           APPN_COS_WEIGHT_KEY, 
           APPN_COS_TG_WEIGHT_KEY_HELP, PRIV_CONF);
NOPREFIX  (cfg_appn_cos_tg_noprefix, cfg_appn_cos_tg_weight, 
           cfg_appn_cos_tg_eol);
NUMBER    (cfg_appn_cos_tg_index, cfg_appn_cos_tg_noprefix, no_alt,
           OBJ(int,1), 1, 8, APPN_COS_TG_INDEX_VAL_HELP);
NVGENS    (cfg_appn_cos_tg_nvgen, cfg_appn_cos_tg_index,
           appn_command, APPN_COS_TG);
KEYWORD_ID(cfg_appn_cos_tg, cfg_appn_cos_tg_nvgen,ALTERNATE,
           appn_subcommand_type, APPN_COS_TG,
           APPN_COS_TG_KEY, 
           APPN_COS_TG_KEY_HELP, PRIV_CONF);
#undef  ALTERNATE
#define ALTERNATE cfg_appn_cos_tg

/*
 * [no] node-row <index> weight <w> congestion <l> <h> 
 *         route-additional-resistance <l> <h>
 */
EOLS      (cfg_appn_cos_node_eol, appn_command, APPN_COS_NODE);
NUMBER    (cfg_appn_cos_route_hi, cfg_appn_cos_node_eol, no_alt,
           OBJ(int,6), 0, 255, APPN_COS_ROUTE_HI_VAL_HELP);
NUMBER    (cfg_appn_cos_route_low, cfg_appn_cos_route_hi, no_alt,
           OBJ(int,5), 0, 255, APPN_COS_ROUTE_LOW_VAL_HELP);
KEYWORD   (cfg_appn_cos_route_key, cfg_appn_cos_route_low,
           no_alt,
           APPN_COS_ROUTE_KEY, 
           APPN_COS_ROUTE_KEY_HELP, PRIV_CONF);
KEYWORD_ID(cfg_appn_cos_congest_hi_yes, cfg_appn_cos_route_key,
           no_alt,
           OBJ(int,4), AP_YES,
           APPN_COS_YES_KEY, 
           APPN_COS_CONGEST_HI_YES_KEY_HELP, PRIV_CONF);
KEYWORD_ID(cfg_appn_cos_congest_hi_no, cfg_appn_cos_route_key,
           cfg_appn_cos_congest_hi_yes,
           OBJ(int,4), AP_NO,
           APPN_COS_NO_KEY, 
           APPN_COS_CONGEST_HI_NO_KEY_HELP, PRIV_CONF);
KEYWORD_ID(cfg_appn_cos_congest_low_yes, cfg_appn_cos_congest_hi_no,
           no_alt,
           OBJ(int,3), AP_YES,
           APPN_COS_YES_KEY, 
           APPN_COS_CONGEST_LOW_YES_KEY_HELP, PRIV_CONF);
KEYWORD_ID(cfg_appn_cos_congest_low_no, cfg_appn_cos_congest_hi_no,
           cfg_appn_cos_congest_low_yes,
           OBJ(int,3), AP_NO,
           APPN_COS_NO_KEY, 
           APPN_COS_CONGEST_LOW_NO_KEY_HELP, PRIV_CONF);
KEYWORD   (cfg_appn_cos_congest_key, cfg_appn_cos_congest_low_no,
           no_alt,
           APPN_COS_CONGEST_KEY, 
           APPN_COS_CONGEST_KEY_HELP, PRIV_CONF);
NUMBER    (cfg_appn_cos_node_weight_val, cfg_appn_cos_congest_key, no_alt,
           OBJ(int,2), 0, 255, APPN_COS_NODE_WEIGHT_VAL_HELP);
KEYWORD   (cfg_appn_cos_node_weight, cfg_appn_cos_node_weight_val,
           no_alt,
           APPN_COS_WEIGHT_KEY, 
           APPN_COS_NODE_WEIGHT_KEY_HELP, PRIV_CONF);
NOPREFIX  (cfg_appn_cos_node_noprefix, cfg_appn_cos_node_weight, 
           cfg_appn_cos_node_eol);
NUMBER    (cfg_appn_cos_node_index, cfg_appn_cos_node_noprefix, no_alt,
           OBJ(int,1), 1, 8, APPN_COS_NODE_INDEX_VAL_HELP);
NVGENS    (cfg_appn_cos_node_nvgen, cfg_appn_cos_node_index,
           appn_command, APPN_COS_NODE);
KEYWORD_ID_MM (cfg_appn_cos_node, cfg_appn_cos_node_nvgen,ALTERNATE,
           appn_subcommand_type, APPN_COS_NODE,
           APPN_COS_NODE_KEY, 
           APPN_COS_NODE_KEY_HELP, PRIV_CONF, 3);
#undef  ALTERNATE
#define ALTERNATE cfg_appn_cos_node

/*
 * [no] transmission-priority <priority>
 */
EOLS	  (cfg_appn_cos_prior_eol, appn_command, APPN_COS_PRIOR);
KEYWORD_ID(cfg_appn_cos_prior_low, cfg_appn_cos_prior_eol,
           no_alt,
           OBJ(int,1), AP_LOW,
           APPN_COS_PRIOR_LOW, 
           APPN_COS_PRIOR_LOW_KEY_HELP, PRIV_CONF);
KEYWORD_ID(cfg_appn_cos_prior_med, cfg_appn_cos_prior_eol,
           cfg_appn_cos_prior_low,
           OBJ(int,1), AP_MEDIUM,
           APPN_COS_PRIOR_MED, 
           APPN_COS_PRIOR_MED_KEY_HELP, PRIV_CONF);
KEYWORD_ID(cfg_appn_cos_prior_high, cfg_appn_cos_prior_eol,
           cfg_appn_cos_prior_med,
           OBJ(int,1), AP_HIGH,
           APPN_COS_PRIOR_HIGH, 
           APPN_COS_PRIOR_HIGH_KEY_HELP, PRIV_CONF);
KEYWORD_ID(cfg_appn_cos_prior_net, cfg_appn_cos_prior_eol,
           cfg_appn_cos_prior_high,
           OBJ(int,1), AP_NETWORK,
           APPN_COS_PRIOR_NET, 
           APPN_COS_PRIOR_NET_KEY_HELP, PRIV_CONF);
NOPREFIX  (cfg_appn_cos_prior_noprefix, cfg_appn_cos_prior_net, 
           cfg_appn_cos_prior_eol);
NVGENS    (cfg_appn_cos_prior_nvgen, cfg_appn_cos_prior_noprefix,
           appn_command, APPN_COS_PRIOR);
KEYWORD_ID(cfg_appn_cos_prior, cfg_appn_cos_prior_nvgen,ALTERNATE,
           appn_subcommand_type, APPN_COS_PRIOR,
           APPN_COS_PRIOR_KEY, 
           APPN_COS_PRIOR_KEY_HELP, PRIV_CONF);
#undef  ALTERNATE
#define ALTERNATE cfg_appn_cos_prior
