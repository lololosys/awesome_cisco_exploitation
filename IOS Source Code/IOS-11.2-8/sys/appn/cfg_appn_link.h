/* 
 * $Id: cfg_appn_link.h,v 3.2.18.2 1996/08/12 16:00:16 widmer Exp $
 * $Source: /release/112/cvs/Xsys/appn/cfg_appn_link.h,v $
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
 * $Log: cfg_appn_link.h,v $
 * Revision 3.2.18.2  1996/08/12  16:00:16  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.2.18.1  1996/05/23  14:48:46  mregan
 * CSCdi45410:  Limited resource link auto-activation fails
 * Branch: California_branch
 * Fix support for auto-activate links.
 *
 * Revision 3.2  1996/02/13  19:18:16  lpereira
 * CSCdi45471:  appn/qllc pvc broken
 *
 * Revision 3.1  1996/02/01  04:56:47  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 1: Create copies of the files to be moved in their new location.
 *
 * Revision 3.3  1995/12/01  15:56:51  widmer
 * CSCdi44724:  Remove nv_top argument to parser_add_mode()
 * Clean up "no" handling at beginning of modes
 * Fix appn modes
 * Removed unneded pdecl's
 *
 * Revision 3.2  1995/11/17  18:42:15  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:49:39  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.7  1995/11/08  21:20:06  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.6  1995/08/04  01:34:52  pleung
 * CSCdi37371:  APPN link-queuing custom only up to queue 10
 *
 * Revision 2.5  1995/07/27  22:29:52  dmccowan
 * CSCdi36612:  minor problems with appn commands
 *
 * Revision 2.4  1995/07/26  01:44:15  vtseng
 * CSCdi31824:  appn link station config command needs more checking
 * Also piggyback fixes for CSCdi31836 that SAP should be divisible
 * by 2.
 *
 * Revision 2.3  1995/06/20  06:03:34  sberl
 * CSCdi35593:  Clean up cls queuing option support
 * Incorporate code review comments.
 *
 * Revision 2.2  1995/06/09  19:38:47  sberl
 * CSCdi35593:  Clean up cls queuing option support
 * Allow appn to specify the priority parameter for each link station.
 *
 * Revision 2.1  1995/06/07  22:01:29  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 * $Endlog$
*/



/******************************************************************
 * appn link-station <linkname>
 *    ...subcommands...
 */


/*
 *   exit
 */
EOLNS	(appn_link_exit_eol, exit_appn_command);
KEYWORD	(appn_link_exit_kwd, appn_link_exit_eol, NONE,
	 "exit", "Exit from appn-link-station configuration mode", PRIV_CONF);
TESTVAR	(appn_link_exit, NONE, appn_link_exit_kwd, NONE, NONE, ALTERNATE,
	 sense, FALSE);
#undef  ALTERNATE
#define ALTERNATE appn_link_exit

/*
 *   [no] complete
 */
EOLS      (cfg_appn_link_comp_eol, appn_command, APPN_LINK_COMP);
KEYWORD_ID(cfg_appn_link_comp, cfg_appn_link_comp_eol, ALTERNATE,
           appn_subcommand_type, APPN_LINK_COMP,
           "complete", "Complete link-station definition.", PRIV_CONF);
#undef  ALTERNATE
#define ALTERNATE cfg_appn_link_comp

/*
 *   [no] link-queuing priority  | custom <num>
 * OBJ(int, 1) = CLS_QUEUING_{PRIORITY,CUSTOM}
 * OBJ(int, 2) = queuing parameter
 */
EOLS	  (cfg_appn_link_queuing_eol, appn_command, APPN_LINK_QUEUING);
NUMBER     (cfg_appn_link_queuing_custom_val, cfg_appn_link_queuing_eol, no_alt,
	    OBJ(int, 2), 1,  PRIORITY_MAXTYPE-1,
	    APPN_LINK_QUEUING_CUSTOM_VAL_HELP);
KEYWORD_ID (cfg_appn_link_queuing_custom, cfg_appn_link_queuing_custom_val, 
	    no_alt,
	    OBJ(int, 1), CLS_QUEUING_CUSTOM,
	    APPN_LINK_QUEUING_CUSTOM_KEY, APPN_LINK_QUEUING_CUSTOM_KEY_HELP, PRIV_CONF);
KEYWORD_ID (cfg_appn_link_queuing_priority_low, cfg_appn_link_queuing_eol, 
	    no_alt,
	    OBJ(int, 2), PRIORITY_LOW,
	    "low", "low priority queue", PRIV_CONF);
KEYWORD_ID (cfg_appn_link_queuing_priority_normal, cfg_appn_link_queuing_eol,
	    cfg_appn_link_queuing_priority_low,
	    OBJ(int, 2), PRIORITY_NORMAL,
	    "normal", "normal priority queue", PRIV_CONF);
KEYWORD_ID (cfg_appn_link_queuing_priority_medium, cfg_appn_link_queuing_eol,
	    cfg_appn_link_queuing_priority_normal,
	    OBJ(int, 2), PRIORITY_MEDIUM,
	    "medium", "medium priority queue", PRIV_CONF);
KEYWORD_ID (cfg_appn_link_queuing_priority_high, cfg_appn_link_queuing_eol, 
	    cfg_appn_link_queuing_priority_medium,
	    OBJ(int, 2), PRIORITY_HIGH,
	    "high", "high priority queue", PRIV_CONF);
KEYWORD_ID (cfg_appn_link_queuing_priority, cfg_appn_link_queuing_priority_high, 
	    cfg_appn_link_queuing_custom, 
	    OBJ(int, 1), CLS_QUEUING_PRIORITY,
	    APPN_LINK_QUEUING_PRIORITY_KEY, APPN_LINK_QUEUING_PRIORITY_KEY_HELP, PRIV_CONF);
NOPREFIX  (cfg_appn_link_queuing_noprefix, cfg_appn_link_queuing_priority, cfg_appn_link_queuing_eol);
NVGENS    (cfg_appn_link_queuing_nvgen, cfg_appn_link_queuing_noprefix,
           appn_command, APPN_LINK_QUEUING);
KEYWORD_ID(cfg_appn_link_queuing, cfg_appn_link_queuing_nvgen,ALTERNATE,
           appn_subcommand_type, APPN_LINK_QUEUING,
           APPN_LINK_QUEUING_KEY, APPN_LINK_QUEUING_KEY_HELP, PRIV_CONF);
#undef  ALTERNATE
#define ALTERNATE cfg_appn_link_queuing

/*
 *   [no] qllc pvc|svc <x25-subaddress>
 */

EOLS	  (cfg_appn_link_qllc_eol, appn_command, APPN_LINK_CONNECT);
X121ADDR    (cfg_appn_link_qllc_svc_val, cfg_appn_link_qllc_eol, no_alt,
           OBJ(paddr,1), APPN_LINK_QLLC_SVC_VAL_HELP);
KEYWORD_ID (cfg_appn_link_qllc_svc, cfg_appn_link_qllc_svc_val, 
	   cfg_appn_link_qllc_eol,
	   OBJ(int,1), FALSE, APPN_LINK_QLLC_SVC_KEY,
	   APPN_LINK_QLLC_SVC_KEY_HELP, PRIV_CONF);
NUMBER    (cfg_appn_link_qllc_pvc_val, cfg_appn_link_qllc_eol, no_alt,
	  OBJ(int,2), 1, 4095, APPN_LINK_QLLC_PVC_VAL_HELP);
KEYWORD_ID (cfg_appn_link_qllc_pvc, cfg_appn_link_qllc_pvc_val, 
	   cfg_appn_link_qllc_svc,
	   OBJ(int,1), TRUE, APPN_LINK_QLLC_PVC_KEY,
	   APPN_LINK_QLLC_PVC_KEY_HELP, PRIV_CONF);
NVGENS    (cfg_appn_link_qllc_nvgen, cfg_appn_link_qllc_pvc,
           appn_command, APPN_LINK_QLLCADDR);
KEYWORD_ID(cfg_appn_link_qllc, cfg_appn_link_qllc_nvgen,ALTERNATE,
           appn_subcommand_type, APPN_LINK_QLLCADDR,
           APPN_LINK_QLLCADDR_KEY, 
           APPN_LINK_QLLC_KEY_HELP, PRIV_CONF);
#undef  ALTERNATE
#define ALTERNATE cfg_appn_link_qllc

/*
 *   [no] sdlc-sec-address <address>
 */

EOLS	  (cfg_appn_link_sdlc_eol, appn_command, APPN_LINK_CONNECT);
HEXDIGIT  (cfg_appn_link_sdlc_addr, cfg_appn_link_sdlc_eol, no_alt,
	  OBJ(int,1), 1, 0xFE, APPN_LINK_SDLCADDR_VAL_HELP);
NOPREFIX  (cfg_appn_link_sdlc_noprefix, cfg_appn_link_sdlc_addr, cfg_appn_link_sdlc_eol);
NVGENS    (cfg_appn_link_sdlc_nvgen, cfg_appn_link_sdlc_noprefix,
           appn_command, APPN_LINK_SDLCADDR);
KEYWORD_ID(cfg_appn_link_sdlc, cfg_appn_link_sdlc_nvgen,ALTERNATE,
           appn_subcommand_type, APPN_LINK_SDLCADDR,
           APPN_LINK_SDLCADDR_KEY, 
           APPN_LINK_SDLCADDR_KEY_HELP, PRIV_CONF);
#undef  ALTERNATE
#define ALTERNATE cfg_appn_link_sdlc

/*
 *    [no] pu_type_20
 */
EOLS	  (cfg_appn_pu_type_20_eol, appn_command, APPN_LINK_PU_TYPE_20);
KEYWORD_ID(cfg_appn_pu_type_20, cfg_appn_pu_type_20_eol,ALTERNATE,
           appn_subcommand_type, APPN_LINK_PU_TYPE_20,
           APPN_LINK_PU_TYPE_20_KEY, 
           APPN_LINK_PU_TYPE_20_KEY_HELP, PRIV_CONF);
#undef  ALTERNATE
#define ALTERNATE cfg_appn_pu_type_20
 
/*
 *    [no] dspu_name <name>
 */
EOLS	  (cfg_appn_dspu_name_eol, appn_command, APPN_LINK_DSPU);
APPN_TYPE_A_NAME (cfg_appn_dspu_name_field, cfg_appn_dspu_name_eol, no_alt,
           OBJ(string,1), APPN_LINK_DSPU_VAL_HELP);
NOPREFIX  (cfg_appn_dspu_name_noprefix, cfg_appn_dspu_name_field, 
	   cfg_appn_dspu_name_eol);
NVGENS    (cfg_appn_dspu_name_nvgen, cfg_appn_dspu_name_noprefix,
           appn_command, APPN_LINK_DSPU);
KEYWORD_ID(cfg_appn_dspu_name, cfg_appn_dspu_name_nvgen,ALTERNATE,
           appn_subcommand_type, APPN_LINK_DSPU,
           APPN_LINK_DSPU_KEY, 
           APPN_LINK_DSPU_KEY_HELP, PRIV_CONF);
#undef  ALTERNATE
#define ALTERNATE cfg_appn_dspu_name

/*
 *    [no] dlus <name>
 */
EOLS	  (cfg_appn_dlus_name_eol, appn_command, APPN_LINK_DLUS);
APPN_FQNAME (cfg_appn_dlus_name_field, cfg_appn_dlus_name_eol, no_alt,
           OBJ(string,1), APPN_LINK_DLUS_VAL_HELP);
NOPREFIX  (cfg_appn_dlus_name_noprefix, cfg_appn_dlus_name_field,
	   cfg_appn_dlus_name_eol);
NVGENS    (cfg_appn_dlus_name_nvgen, cfg_appn_dlus_name_noprefix,
           appn_command, APPN_LINK_DLUS);
KEYWORD_ID(cfg_appn_dlus_name, cfg_appn_dlus_name_nvgen,ALTERNATE,
           appn_subcommand_type, APPN_LINK_DLUS,
           APPN_LINK_DLUS_KEY, 
           APPN_LINK_DLUS_KEY_HELP, PRIV_CONF);
#undef  ALTERNATE
#define ALTERNATE cfg_appn_dlus_name

/*
 *    [no] bkup_dlus <name>
 */
EOLS	  (cfg_appn_bkup_dlus_name_eol, appn_command, APPN_LINK_BKUP_DLUS);
APPN_FQNAME (cfg_appn_bkup_dlus_name_field, cfg_appn_bkup_dlus_name_eol, no_alt,
           OBJ(string,1), APPN_LINK_BKUP_DLUS_VAL_HELP);
NOPREFIX  (cfg_appn_bkup_dlus_name_noprefix, cfg_appn_bkup_dlus_name_field,
	   cfg_appn_bkup_dlus_name_eol);
NVGENS    (cfg_appn_bkup_dlus_name_nvgen, cfg_appn_bkup_dlus_name_noprefix,
           appn_command, APPN_LINK_BKUP_DLUS);
KEYWORD_ID(cfg_appn_bkup_dlus_name, cfg_appn_bkup_dlus_name_nvgen,ALTERNATE,
           appn_subcommand_type, APPN_LINK_BKUP_DLUS,
           APPN_LINK_BKUP_DLUS_KEY, 
           APPN_LINK_BKUP_DLUS_KEY_HELP, PRIV_CONF);
#undef  ALTERNATE
#define ALTERNATE cfg_appn_bkup_dlus_name

/*
 *    [no] retry infinite|<tries> [<interval>]
 */
EOLS	  (cfg_appn_link_retry_eol, appn_command, APPN_LINK_RETRY);
NUMBER    (cfg_appn_link_retry_int, cfg_appn_link_retry_eol, 
           cfg_appn_link_retry_eol,
           OBJ(int,3), 0,32767, APPN_LINK_RETRY_TIME_HELP);
NUMBER    (cfg_appn_link_retry_tries, cfg_appn_link_retry_int, no_alt,
           OBJ(int,2), 0,255, APPN_LINK_RETRY_TRIES_HELP);
KEYWORD_ID(cfg_appn_link_retry_inf, cfg_appn_link_retry_int,
           cfg_appn_link_retry_tries,
           OBJ(int,1), TRUE,
           APPN_LINK_RETRY_INF, APPN_LINK_RETRY_INF_KEY_HELP, PRIV_CONF);
NOPREFIX  (cfg_appn_link_retry_noprefix, cfg_appn_link_retry_inf, 
           cfg_appn_link_retry_eol);
NVGENS    (cfg_appn_link_retry_nvgen, cfg_appn_link_retry_noprefix,
           appn_command, APPN_LINK_RETRY);
KEYWORD_ID(cfg_appn_link_retry, cfg_appn_link_retry_nvgen,ALTERNATE,
           appn_subcommand_type, APPN_LINK_RETRY,
           APPN_LINK_RETRY_KEY, APPN_LINK_RETRY_KEY_HELP, PRIV_CONF);
#undef  ALTERNATE
#define ALTERNATE cfg_appn_link_retry

/*
 *   [no] tg-number <num>
 */
EOLS	  (cfg_appn_link_tgnum_eol, appn_command, APPN_LINK_TGNUM);
NUMBER    (cfg_appn_link_tgnum_num, cfg_appn_link_tgnum_eol, no_alt,
           OBJ(int,3), 0, 255, APPN_LINK_TGNUM_VAL_HELP);
NOPREFIX  (cfg_appn_link_tgnum_noprefix, cfg_appn_link_tgnum_num, cfg_appn_link_tgnum_eol);
NVGENS    (cfg_appn_link_tgnum_nvgen, cfg_appn_link_tgnum_noprefix,
           appn_command, APPN_LINK_TGNUM);
KEYWORD_ID(cfg_appn_link_tgnum, cfg_appn_link_tgnum_nvgen,ALTERNATE,
           appn_subcommand_type, APPN_LINK_TGNUM,
           APPN_LINK_TGNUM_KEY, 
           APPN_LINK_TGNUM_KEY_HELP, PRIV_CONF);
#undef  ALTERNATE
#define ALTERNATE cfg_appn_link_tgnum

/*
 *    [no] call-out
 */
EOLS	  (cfg_appn_link_call_eol, appn_command, APPN_LINK_CALL);
KEYWORD_ID(cfg_appn_link_call, cfg_appn_link_call_eol,ALTERNATE,
           appn_subcommand_type, APPN_LINK_CALL,
           APPN_LINK_CALL_KEY, APPN_LINK_CALL_KEY_HELP, PRIV_CONF);
#undef  ALTERNATE
#define ALTERNATE cfg_appn_link_call

/*
 *    [no] limited-resource
 */
EOLS	  (cfg_appn_link_limit_eol, appn_command, APPN_LINK_LIMIT);
KEYWORD_ID(cfg_appn_link_limit, cfg_appn_link_limit_eol,ALTERNATE,
           appn_subcommand_type, APPN_LINK_LIMIT,
           APPN_LINK_LIMIT_KEY, APPN_LINK_LIMIT_KEY_HELP, PRIV_CONF);
#undef  ALTERNATE
#define ALTERNATE cfg_appn_link_limit

/*
 *    [no] auto-activate
 */
EOLS	  (cfg_appn_link_auto_eol, appn_command, APPN_LINK_AUTO);
KEYWORD_ID(cfg_appn_link_auto, cfg_appn_link_auto_eol,ALTERNATE,
           appn_subcommand_type, APPN_LINK_AUTO,
           APPN_LINK_AUTO_KEY, APPN_LINK_AUTO_KEY_HELP, PRIV_CONF);
#undef  ALTERNATE
#define ALTERNATE cfg_appn_link_auto

/*
 *   [no] role <role>
 */
EOLS	  (cfg_appn_link_role_eol, appn_command, APPN_LINK_ROLE);
KEYWORD_ID(cfg_appn_link_role_neg, cfg_appn_link_role_eol,
           no_alt,
           OBJ(int,1), AP_NEGOTIABLE,
           APPN_LINK_ROLE_NEG, 
           APPN_LINK_ROLE_NEG_KEY_HELP, PRIV_CONF);
KEYWORD_ID(cfg_appn_link_role_sec, cfg_appn_link_role_eol,
           cfg_appn_link_role_neg,
           OBJ(int,1), AP_SECONDARY,
           APPN_LINK_ROLE_SEC, 
           APPN_LINK_ROLE_SEC_KEY_HELP, PRIV_CONF);
KEYWORD_ID(cfg_appn_link_role_pri, cfg_appn_link_role_eol,
           cfg_appn_link_role_sec,
           OBJ(int,1), AP_PRIMARY,
           APPN_LINK_ROLE_PRI, 
           APPN_LINK_ROLE_PRI_KEY_HELP, PRIV_CONF);
NOPREFIX  (cfg_appn_link_role_noprefix, cfg_appn_link_role_pri, 
           cfg_appn_link_role_eol);
NVGENS    (cfg_appn_link_role_nvgen, cfg_appn_link_role_noprefix,
           appn_command, APPN_LINK_ROLE);
KEYWORD_ID(cfg_appn_link_role, cfg_appn_link_role_nvgen,ALTERNATE,
           appn_subcommand_type, APPN_LINK_ROLE,
           APPN_LINK_ROLE_KEY, 
           APPN_LINK_ROLE_KEY_HELP, PRIV_CONF);
#undef  ALTERNATE
#define ALTERNATE cfg_appn_link_role

/*
 *   [no] user-defined-3 <val>
 */
EOLS	  (cfg_appn_link_user3_eol, appn_command, APPN_LINK_USER3);
NUMBER    (cfg_appn_link_user3_num, cfg_appn_link_user3_eol, no_alt,
           OBJ(int,3), 0, 255, APPN_LINK_USER3_VAL_HELP);
NOPREFIX  (cfg_appn_link_user3_noprefix, cfg_appn_link_user3_num, cfg_appn_link_user3_eol);
NVGENS    (cfg_appn_link_user3_nvgen, cfg_appn_link_user3_noprefix,
           appn_command, APPN_LINK_USER3);
KEYWORD_ID(cfg_appn_link_user3, cfg_appn_link_user3_nvgen,ALTERNATE,
           appn_subcommand_type, APPN_LINK_USER3,
           APPN_LINK_USER3_KEY, 
           APPN_LINK_USER3_KEY_HELP, PRIV_CONF);
#undef  ALTERNATE
#define ALTERNATE cfg_appn_link_user3

/*
 *   [no] user-defined-2 <val>
 */
EOLS	  (cfg_appn_link_user2_eol, appn_command, APPN_LINK_USER2);
NUMBER    (cfg_appn_link_user2_num, cfg_appn_link_user2_eol, no_alt,
           OBJ(int,3), 0, 255, APPN_LINK_USER2_VAL_HELP);
NOPREFIX  (cfg_appn_link_user2_noprefix, cfg_appn_link_user2_num, cfg_appn_link_user2_eol);
NVGENS    (cfg_appn_link_user2_nvgen, cfg_appn_link_user2_noprefix,
           appn_command, APPN_LINK_USER2);
KEYWORD_ID(cfg_appn_link_user2, cfg_appn_link_user2_nvgen,ALTERNATE,
           appn_subcommand_type, APPN_LINK_USER2,
           APPN_LINK_USER2_KEY, 
           APPN_LINK_USER2_KEY_HELP, PRIV_CONF);
#undef  ALTERNATE
#define ALTERNATE cfg_appn_link_user2

/*
 *   [no] user-defined-1 <val>
 */
EOLS	  (cfg_appn_link_user1_eol, appn_command, APPN_LINK_USER1);
NUMBER    (cfg_appn_link_user1_num, cfg_appn_link_user1_eol, no_alt,
           OBJ(int,3), 0, 255, APPN_LINK_USER1_VAL_HELP);
NOPREFIX  (cfg_appn_link_user1_noprefix, cfg_appn_link_user1_num, cfg_appn_link_user1_eol);
NVGENS    (cfg_appn_link_user1_nvgen, cfg_appn_link_user1_noprefix,
           appn_command, APPN_LINK_USER1);
KEYWORD_ID(cfg_appn_link_user1, cfg_appn_link_user1_nvgen,ALTERNATE,
           appn_subcommand_type, APPN_LINK_USER1,
           APPN_LINK_USER1_KEY, 
           APPN_LINK_USER1_KEY_HELP, PRIV_CONF);
#undef  ALTERNATE
#define ALTERNATE cfg_appn_link_user1

/*
 *   [no] security <security>
 */
EOLS	  (cfg_appn_link_secur_eol, appn_command, APPN_LINK_SECUR);
KEYWORD_ID(cfg_appn_link_secur_rad, cfg_appn_link_secur_eol,
           no_alt,
           OBJ(int,1), AP_SEC_GUARDED_RADIATION,
           APPN_LINK_SECUR_RAD, 
           APPN_LINK_SECUR_RAD_KEY_HELP, PRIV_CONF);
KEYWORD_ID(cfg_appn_link_secur_enc, cfg_appn_link_secur_eol,
           cfg_appn_link_secur_rad,
           OBJ(int,1), AP_SEC_ENCRYPTED ,
           APPN_LINK_SECUR_ENC, 
           APPN_LINK_SECUR_ENC_KEY_HELP, PRIV_CONF);
KEYWORD_ID(cfg_appn_link_secur_gua, cfg_appn_link_secur_eol,
           cfg_appn_link_secur_enc,
           OBJ(int,1), AP_SEC_GUARDED_CONDUIT ,
           APPN_LINK_SECUR_GUA, 
           APPN_LINK_SECUR_GUA_KEY_HELP, PRIV_CONF);
KEYWORD_ID(cfg_appn_link_secur_sec, cfg_appn_link_secur_eol,
           cfg_appn_link_secur_gua,
           OBJ(int,1), AP_SEC_SECURE_CONDUIT ,
           APPN_LINK_SECUR_SEC, 
           APPN_LINK_SECUR_SEC_KEY_HELP, PRIV_CONF);
KEYWORD_ID(cfg_appn_link_secur_und, cfg_appn_link_secur_eol,
           cfg_appn_link_secur_sec,
           OBJ(int,1), AP_SEC_UNDERGROUND_CABLE ,
           APPN_LINK_SECUR_UND, 
           APPN_LINK_SECUR_UND_KEY_HELP, PRIV_CONF);
KEYWORD_ID(cfg_appn_link_secur_pub, cfg_appn_link_secur_eol,
           cfg_appn_link_secur_und,
           OBJ(int,1), AP_SEC_PUBLIC_SWITCHED_NETWORK,
           APPN_LINK_SECUR_PUB, 
           APPN_LINK_SECUR_PUB_KEY_HELP, PRIV_CONF);
KEYWORD_ID(cfg_appn_link_secur_non, cfg_appn_link_secur_eol,
           cfg_appn_link_secur_pub,
           OBJ(int,1), AP_SEC_NONSECURE,
           APPN_LINK_SECUR_NON, 
           APPN_LINK_SECUR_NON_KEY_HELP, PRIV_CONF);
NOPREFIX  (cfg_appn_link_secur_noprefix, cfg_appn_link_secur_non, 
           cfg_appn_link_secur_eol);
NVGENS    (cfg_appn_link_secur_nvgen, cfg_appn_link_secur_noprefix,
           appn_command, APPN_LINK_SECUR);
KEYWORD_ID(cfg_appn_link_secur, cfg_appn_link_secur_nvgen,ALTERNATE,
           appn_subcommand_type, APPN_LINK_SECUR,
           APPN_LINK_SECUR_KEY, 
           APPN_LINK_SECUR_KEY_HELP, PRIV_CONF);
#undef  ALTERNATE
#define ALTERNATE cfg_appn_link_secur

/*
 *   [no] propagation_delay <delay>
 */
EOLS	  (cfg_appn_link_delay_eol, appn_command, APPN_LINK_DELAY);
KEYWORD_ID(cfg_appn_link_delay_max, cfg_appn_link_delay_eol,
           no_alt,
           OBJ(int,1), AP_PROP_DELAY_MAXIMUM,
           APPN_LINK_DELAY_MAX, 
           APPN_LINK_DELAY_MAX_KEY_HELP, PRIV_CONF);
KEYWORD_ID(cfg_appn_link_delay_sat, cfg_appn_link_delay_eol,
           cfg_appn_link_delay_max,
           OBJ(int,1), AP_PROP_DELAY_SATELLITE,
           APPN_LINK_DELAY_SAT, 
           APPN_LINK_DELAY_SAT_KEY_HELP, PRIV_CONF);
KEYWORD_ID(cfg_appn_link_delay_pack, cfg_appn_link_delay_eol,
           cfg_appn_link_delay_sat,
           OBJ(int,1), AP_PROP_DELAY_PKT_SWITCHED_NET,
           APPN_LINK_DELAY_PACK, 
           APPN_LINK_DELAY_PACK_KEY_HELP, PRIV_CONF);
KEYWORD_ID(cfg_appn_link_delay_phone, cfg_appn_link_delay_eol,
           cfg_appn_link_delay_pack,
           OBJ(int,1), AP_PROP_DELAY_TELEPHONE,
           APPN_LINK_DELAY_PHONE, 
           APPN_LINK_DELAY_PHONE_KEY_HELP, PRIV_CONF);
KEYWORD_ID(cfg_appn_link_delay_lan, cfg_appn_link_delay_eol,
           cfg_appn_link_delay_phone,
           OBJ(int,1), AP_PROP_DELAY_LAN,
           APPN_LINK_DELAY_LAN, 
           APPN_LINK_DELAY_LAN_KEY_HELP, PRIV_CONF);
KEYWORD_ID(cfg_appn_link_delay_min, cfg_appn_link_delay_eol,
           cfg_appn_link_delay_lan,
           OBJ(int,1), AP_PROP_DELAY_MINIMUM,
           APPN_LINK_DELAY_MIN, 
           APPN_LINK_DELAY_MIN_KEY_HELP, PRIV_CONF);
NOPREFIX  (cfg_appn_link_delay_noprefix, cfg_appn_link_delay_min, 
           cfg_appn_link_delay_eol);
NVGENS    (cfg_appn_link_delay_nvgen, cfg_appn_link_delay_noprefix,
           appn_command, APPN_LINK_DELAY);
KEYWORD_ID(cfg_appn_link_delay, cfg_appn_link_delay_nvgen,ALTERNATE,
           appn_subcommand_type, APPN_LINK_DELAY,
           APPN_LINK_DELAY_KEY, 
           APPN_LINK_DELAY_KEY_HELP, PRIV_CONF);
#undef  ALTERNATE
#define ALTERNATE cfg_appn_link_delay

/*
 *   [no] cost-per-connect-time <cost>
 */
EOLS	  (cfg_appn_link_connect_eol, appn_command, APPN_LINK_CONNECT);
NUMBER    (cfg_appn_link_connect_num, cfg_appn_link_connect_eol, no_alt,
           OBJ(int,3), 0, 255, APPN_LINK_CONNECT_VAL_HELP);
NOPREFIX  (cfg_appn_link_connect_noprefix, cfg_appn_link_connect_num, cfg_appn_link_connect_eol);
NVGENS    (cfg_appn_link_connect_nvgen, cfg_appn_link_connect_noprefix,
           appn_command, APPN_LINK_CONNECT);
KEYWORD_ID(cfg_appn_link_connect, cfg_appn_link_connect_nvgen,ALTERNATE,
           appn_subcommand_type, APPN_LINK_CONNECT,
           APPN_LINK_CONNECT_KEY, 
           APPN_LINK_CONNECT_KEY_HELP, PRIV_CONF);
#undef  ALTERNATE
#define ALTERNATE cfg_appn_link_connect

/*
 *   [no] cost-per-byte <cost>
 */
EOLS	  (cfg_appn_link_byte_eol, appn_command, APPN_LINK_BYTE);
NUMBER    (cfg_appn_link_byte_num, cfg_appn_link_byte_eol, no_alt,
           OBJ(int,3), 0, 255, APPN_LINK_BYTE_VAL_HELP);
NOPREFIX  (cfg_appn_link_byte_noprefix, cfg_appn_link_byte_num, cfg_appn_link_byte_eol);
NVGENS    (cfg_appn_link_byte_nvgen, cfg_appn_link_byte_noprefix,
           appn_command, APPN_LINK_BYTE);
KEYWORD_ID(cfg_appn_link_byte, cfg_appn_link_byte_nvgen,ALTERNATE,
           appn_subcommand_type, APPN_LINK_BYTE,
           APPN_LINK_BYTE_KEY, 
           APPN_LINK_BYTE_KEY_HELP, PRIV_CONF);
#undef  ALTERNATE
#define ALTERNATE cfg_appn_link_byte


/*
 *   [no] effective-capacity <cap>
 */
EOLS	  (cfg_appn_link_effcap_eol, appn_command, APPN_LINK_EFFCAP);
NUMBER    (cfg_appn_link_effcap_num, cfg_appn_link_effcap_eol, no_alt,
           OBJ(int,3), 0, 255, APPN_LINK_EFFCAP_VAL_HELP);
NOPREFIX  (cfg_appn_link_effcap_noprefix, cfg_appn_link_effcap_num, cfg_appn_link_effcap_eol);
NVGENS    (cfg_appn_link_effcap_nvgen, cfg_appn_link_effcap_noprefix,
           appn_command, APPN_LINK_EFFCAP);
KEYWORD_ID(cfg_appn_link_effcap, cfg_appn_link_effcap_nvgen,ALTERNATE,
           appn_subcommand_type, APPN_LINK_EFFCAP,
           APPN_LINK_EFFCAP_KEY, 
           APPN_LINK_EFFCAP_KEY_HELP, PRIV_CONF);
#undef  ALTERNATE
#define ALTERNATE cfg_appn_link_effcap

#ifdef NOT_SUPPORTED

/*
 *   [no] max-rcv-btu-size <size>
 */
EOLS	  (cfg_appn_link_rcvbtu_eol, appn_command, APPN_LINK_RCVBTU);
NUMBER    (cfg_appn_link_rcvbtu_num, cfg_appn_link_rcvbtu_eol, no_alt,
           OBJ(int,1), 99, 32767, APPN_LINK_RCVBTU_VAL_HELP);
NOPREFIX  (cfg_appn_link_rcvbtu_noprefix, cfg_appn_link_rcvbtu_num, cfg_appn_link_rcvbtu_eol);
NVGENS    (cfg_appn_link_rcvbtu_nvgen, cfg_appn_link_rcvbtu_noprefix,
           appn_command, APPN_LINK_RCVBTU);
KEYWORD_ID(cfg_appn_link_rcvbtu, cfg_appn_link_rcvbtu_nvgen,ALTERNATE,
           appn_subcommand_type, APPN_LINK_RCVBTU,
           APPN_LINK_RCVBTU_KEY, 
           APPN_LINK_RCVBTU_KEY_HELP, PRIV_CONF);
#undef  ALTERNATE
#define ALTERNATE cfg_appn_link_rcvbtu

/*
 *   [no] desired-max-send-btu-size <size>
 */
EOLS	  (cfg_appn_link_sndbtu_eol, appn_command, APPN_LINK_SNDBTU);
NUMBER    (cfg_appn_link_sndbtu_num, cfg_appn_link_sndbtu_eol, no_alt,
           OBJ(int,1), 99, 32767, APPN_LINK_SNDBTU_VAL_HELP);
NOPREFIX  (cfg_appn_link_sndbtu_noprefix, cfg_appn_link_sndbtu_num, cfg_appn_link_sndbtu_eol);
NVGENS    (cfg_appn_link_sndbtu_nvgen, cfg_appn_link_sndbtu_noprefix,
           appn_command, APPN_LINK_SNDBTU);
KEYWORD_ID(cfg_appn_link_sndbtu, cfg_appn_link_sndbtu_nvgen,ALTERNATE,
           appn_subcommand_type, APPN_LINK_SNDBTU,
           APPN_LINK_SNDBTU_KEY, 
           APPN_LINK_SNDBTU_KEY_HELP, PRIV_CONF);
#undef  ALTERNATE
#define ALTERNATE cfg_appn_link_sndbtu

#endif

/*
 *   [no] verify-adjacent-node-type <type>
 */
EOLS	  (cfg_appn_link_nodtyp_eol, appn_command, APPN_LINK_NODTYP);
KEYWORD_ID(cfg_appn_link_nodtyp_nn, cfg_appn_link_nodtyp_eol,
           no_alt,
           OBJ(int,1), AP_NN,
           APPN_LINK_NODTYP_NN, 
           APPN_LINK_NODTYP_NN_KEY_HELP, PRIV_CONF);
KEYWORD_ID(cfg_appn_link_nodtyp_en, cfg_appn_link_nodtyp_eol,
           cfg_appn_link_nodtyp_nn,
           OBJ(int,1), AP_EN,
           APPN_LINK_NODTYP_EN, 
           APPN_LINK_NODTYP_EN_KEY_HELP, PRIV_CONF);
KEYWORD_ID(cfg_appn_link_nodtyp_len, cfg_appn_link_nodtyp_eol,
           cfg_appn_link_nodtyp_en,
           OBJ(int,1), AP_LEN,
           APPN_LINK_NODTYP_LEN, 
           APPN_LINK_NODTYP_LEN_KEY_HELP, PRIV_CONF);
KEYWORD_ID(cfg_appn_link_nodtyp_learn, cfg_appn_link_nodtyp_eol,
           cfg_appn_link_nodtyp_len,
           OBJ(int,1), AP_LEARN,
           APPN_LINK_NODTYP_LEARN, 
           APPN_LINK_NODTYP_LEARN_KEY_HELP, PRIV_CONF);
NOPREFIX  (cfg_appn_link_nodtyp_noprefix, cfg_appn_link_nodtyp_learn, 
           cfg_appn_link_nodtyp_eol);
NVGENS    (cfg_appn_link_nodtyp_nvgen, cfg_appn_link_nodtyp_noprefix,
           appn_command, APPN_LINK_NODTYP);
KEYWORD_ID(cfg_appn_link_nodtyp, cfg_appn_link_nodtyp_nvgen,ALTERNATE,
           appn_subcommand_type, APPN_LINK_NODTYP,
           APPN_LINK_NODTYP_KEY, 
           APPN_LINK_NODTYP_KEY_HELP, PRIV_CONF);
#undef  ALTERNATE
#define ALTERNATE cfg_appn_link_nodtyp

/*
 *    [no] cp-cp-sessions-supported
 */
EOLS	  (cfg_appn_link_cpcp_eol, appn_command, APPN_LINK_CPCP);
KEYWORD_ID(cfg_appn_link_cpcp, cfg_appn_link_cpcp_eol,ALTERNATE,
           appn_subcommand_type, APPN_LINK_CPCP,
           APPN_LINK_CPCP_KEY, APPN_LINK_CPCP_KEY_HELP, PRIV_CONF);
#undef  ALTERNATE
#define ALTERNATE cfg_appn_link_cpcp

/*
 *    [no] adjacent_cp_name <name>
 */
EOLS	  (cfg_appn_link_cpnm_eol, appn_command, APPN_LINK_CPNM);
APPN_FQNAME (cfg_appn_link_cpnm_num, cfg_appn_link_cpnm_eol, no_alt,
           OBJ(string,1), APPN_LINK_CPNM_VAL_HELP);
NOPREFIX  (cfg_appn_link_cpnm_noprefix, cfg_appn_link_cpnm_num, cfg_appn_link_cpnm_eol);
NVGENS    (cfg_appn_link_cpnm_nvgen, cfg_appn_link_cpnm_noprefix,
           appn_command, APPN_LINK_CPNM);
KEYWORD_ID(cfg_appn_link_cpnm, cfg_appn_link_cpnm_nvgen,ALTERNATE,
           appn_subcommand_type, APPN_LINK_CPNM,
           APPN_LINK_CPNM_KEY, 
           APPN_LINK_CPNM_KEY_HELP, PRIV_CONF);
#undef  ALTERNATE
#define ALTERNATE cfg_appn_link_cpnm

/*
 *    [no] lan-dest-addr <address> <sap>
 */
EOLS	  (cfg_appn_link_addr_eol, appn_command, APPN_LINK_LANADDR);
SET	(cfg_appn_link_addr_sap_def, cfg_appn_link_addr_eol, 
         OBJ(int,4), DEFAULT_SAP);
HEXDIGIT  (cfg_appn_link_addr_sap, cfg_appn_link_addr_eol,
	   cfg_appn_link_addr_sap_def, OBJ(int,4), 0x02, 0xec,
	   APPN_LINK_LANSAP_VAL_HELP);
ENADDR    (cfg_appn_link_addr_addr, cfg_appn_link_addr_sap, no_alt,
           hwaddr1, APPN_LINK_LANADDR_VAL_HELP);
NOPREFIX  (cfg_appn_link_addr_noprefix, cfg_appn_link_addr_addr, cfg_appn_link_addr_eol);
NVGENS    (cfg_appn_link_addr_nvgen, cfg_appn_link_addr_noprefix,
           appn_command, APPN_LINK_LANADDR);
KEYWORD_ID(cfg_appn_link_addr, cfg_appn_link_addr_nvgen,ALTERNATE,
           appn_subcommand_type, APPN_LINK_LANADDR,
           APPN_LINK_LANADDR_KEY, APPN_LINK_LANADDR_KEY_HELP, PRIV_CONF);
#undef  ALTERNATE
#define ALTERNATE cfg_appn_link_addr

/*
 *    [no] smds-dest-addr <address> <sap>
 */
EOLS	  (cfg_appn_link_smds_addr_eol, appn_command, APPN_LINK_SMDSADDR);
SET	(cfg_appn_link_smds_addr_sap_def, cfg_appn_link_smds_addr_eol, 
         OBJ(int,4), DEFAULT_SAP);
HEXDIGIT  (cfg_appn_link_smds_addr_sap, cfg_appn_link_smds_addr_eol,
	   cfg_appn_link_smds_addr_sap_def, OBJ(int,4), 0x02, 0xec,
	   APPN_LINK_LANSAP_VAL_HELP);
SMDSADDR    (cfg_appn_link_smds_addr_addr, cfg_appn_link_smds_addr_sap, no_alt,
           hwaddr1, APPN_LINK_SMDSADDR_VAL_HELP);
NOPREFIX  (cfg_appn_link_smds_addr_noprefix, cfg_appn_link_smds_addr_addr,
           cfg_appn_link_smds_addr_eol);
NVGENS    (cfg_appn_link_smds_addr_nvgen, cfg_appn_link_smds_addr_noprefix,
           appn_command, APPN_LINK_SMDSADDR);
KEYWORD_ID(cfg_appn_link_smds_addr, cfg_appn_link_smds_addr_nvgen,ALTERNATE,
           appn_subcommand_type, APPN_LINK_SMDSADDR,
           APPN_LINK_SMDSADDR_KEY, APPN_LINK_SMDSADDR_KEY_HELP, PRIV_CONF);
#undef  ALTERNATE
#define ALTERNATE cfg_appn_link_smds_addr

/*
 *    [no] ppp-dest-addr <sap>
 */
EOLS	  (cfg_appn_link_ppp_addr_eol, appn_command, APPN_LINK_PPPADDR);
HEXDIGIT  (cfg_appn_link_ppp_addr_sap, cfg_appn_link_ppp_addr_eol,
	   no_alt, OBJ(int,4), 0x02, 0xec,
	   APPN_LINK_LANSAP_VAL_HELP);
NOPREFIX  (cfg_appn_link_ppp_addr_noprefix, cfg_appn_link_ppp_addr_sap,
           cfg_appn_link_ppp_addr_eol);
NVGENS    (cfg_appn_link_ppp_addr_nvgen, cfg_appn_link_ppp_addr_noprefix,
           appn_command, APPN_LINK_PPPADDR);
KEYWORD_ID(cfg_appn_link_ppp_addr, cfg_appn_link_ppp_addr_nvgen,ALTERNATE,
           appn_subcommand_type, APPN_LINK_PPPADDR,
           APPN_LINK_PPPADDR_KEY, APPN_LINK_PPPADDR_KEY_HELP, PRIV_CONF);
#undef  ALTERNATE
#define ALTERNATE cfg_appn_link_ppp_addr

/*
 *   [no] fr-dest-addr <num> <sap>
 */
EOLS	  (cfg_appn_link_fr_addr_eol, appn_command, APPN_LINK_TGNUM);
SET       (cfg_appn_link_fr_addr_sap_def, cfg_appn_link_fr_addr_eol,
	   OBJ(int,4), DEFAULT_SAP);
HEXDIGIT  (cfg_appn_link_fr_addr_sap, cfg_appn_link_fr_addr_eol,
           cfg_appn_link_fr_addr_sap_def, OBJ(int,4), 0x02, 0xec,
           APPN_LINK_LANSAP_VAL_HELP);
NUMBER    (cfg_appn_link_fr_addr_num, cfg_appn_link_fr_addr_sap, no_alt,
           OBJ(int,3), 16, 1007, APPN_LINK_FRADDR_VAL_HELP);
NOPREFIX  (cfg_appn_link_fr_addr_noprefix, cfg_appn_link_fr_addr_num, 
	   cfg_appn_link_fr_addr_eol);
NVGENS    (cfg_appn_link_fr_addr_nvgen, cfg_appn_link_fr_addr_noprefix,
           appn_command, APPN_LINK_FRADDR);
KEYWORD_ID(cfg_appn_link_fr_addr, cfg_appn_link_fr_addr_nvgen,ALTERNATE,
           appn_subcommand_type, APPN_LINK_FRADDR,
           APPN_LINK_FRADDR_KEY, 
           APPN_LINK_FRADDR_KEY_HELP, PRIV_CONF);
#undef  ALTERNATE
#define ALTERNATE cfg_appn_link_fr_addr

/*
 *   [no] atm-dest-addr <num> <sap>
 */
EOLS	  (cfg_appn_link_atm_addr_eol, appn_command, APPN_LINK_ATMADDR);
SET       (cfg_appn_link_atm_addr_sap_def, cfg_appn_link_atm_addr_eol,
	   OBJ(int,4), DEFAULT_SAP);
HEXNUM    (cfg_appn_link_atm_addr_sap, cfg_appn_link_atm_addr_eol,
           cfg_appn_link_atm_addr_sap_def, OBJ(int,4),
           APPN_LINK_LANSAP_VAL_HELP);
NUMBER    (cfg_appn_link_atm_addr_num, cfg_appn_link_atm_addr_sap, no_alt,
           OBJ(int,3), 1, 4096, APPN_LINK_ATMADDR_VAL_HELP);
NOPREFIX  (cfg_appn_link_atm_addr_noprefix, cfg_appn_link_atm_addr_num, 
	   cfg_appn_link_atm_addr_eol);
NVGENS    (cfg_appn_link_atm_addr_nvgen, cfg_appn_link_atm_addr_noprefix,
           appn_command, APPN_LINK_ATMADDR);
KEYWORD_ID(cfg_appn_link_atm_addr, cfg_appn_link_atm_addr_nvgen,ALTERNATE,
           appn_subcommand_type, APPN_LINK_ATMADDR,
           APPN_LINK_ATMADDR_KEY, 
           APPN_LINK_ATMADDR_KEY_HELP, PRIV_CONF);
#undef  ALTERNATE
#define ALTERNATE cfg_appn_link_atm_addr

/*
 *    [no] port <name>
 */
EOLS	  (cfg_appn_link_port_eol, appn_command, APPN_LINK_PORT);
APPN_TYPE_A_NAME (cfg_appn_link_port_name, cfg_appn_link_port_eol, no_alt,
           OBJ(string,1), APPN_LINK_PORT_VAL_HELP);
NOPREFIX  (cfg_appn_link_port_noprefix, cfg_appn_link_port_name, 
           cfg_appn_link_port_eol);
NVGENS    (cfg_appn_link_port_nvgen, cfg_appn_link_port_noprefix,
           appn_command, APPN_LINK_PORT);
KEYWORD_ID(cfg_appn_link_port, cfg_appn_link_port_nvgen,ALTERNATE,
           appn_subcommand_type, APPN_LINK_PORT,
           APPN_LINK_PORT_KEY, APPN_LINK_PORT_KEY_HELP, PRIV_CONF);
#undef  ALTERNATE
#define ALTERNATE cfg_appn_link_port
