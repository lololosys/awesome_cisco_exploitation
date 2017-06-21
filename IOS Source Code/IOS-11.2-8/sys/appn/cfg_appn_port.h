/* 
 * $Id: cfg_appn_port.h,v 3.4.10.4 1996/08/12 16:00:19 widmer Exp $
 * $Source: /release/112/cvs/Xsys/appn/cfg_appn_port.h,v $
 *------------------------------------------------------------------
 *
 * Configuration Commands for APPN
 *
 * August 1994, Dave McCowan
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 *
 * $Log: cfg_appn_port.h,v $
 * Revision 3.4.10.4  1996/08/12  16:00:19  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.4.10.3  1996/07/08  15:46:51  dbilling
 * Respond to xid commands with xid responses.  Respond to a null
 * xid command with a null xid response.  Remove null-xid-poll
 * configuration as it is no longer necessary (CSCdi61157)
 * Branch: California_branch
 *
 * Revision 3.4.10.2  1996/06/11  14:00:43  mregan
 * CSCdi53126:  APPN needs to support VDLC
 * Branch: California_branch
 * added code review comments.
 *
 * Revision 3.4.10.1  1996/05/17  10:39:11  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.4.2.2  1996/03/30  05:51:05  mregan
 * CSCdi53126:  APPN needs to support VDLC
 * Branch: IbuMod_Calif_branch
 * add VDLC support to APPN
 *
 * Revision 3.4.2.1  1996/03/19  18:45:34  dmccowan
 * CSCdi48507:  appn port max-btu help parameters should be changed to 5107
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.4  1996/02/27  10:51:15  dcalia
 * APPN/RSRB needs a configurable bridge number (CSCdi42257)
 *
 * Revision 3.3  1996/02/18  18:28:23  pleung
 * CSCdi49290:  dynamic link station support for PU2.0 broken
 *
 * Revision 3.2  1996/02/13  19:18:17  lpereira
 * CSCdi45471:  appn/qllc pvc broken
 *
 * Revision 3.1  1996/02/01  04:56:50  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 1: Create copies of the files to be moved in their new location.
 *
 * Revision 3.3  1995/12/01  15:56:55  widmer
 * CSCdi44724:  Remove nv_top argument to parser_add_mode()
 * Clean up "no" handling at beginning of modes
 * Fix appn modes
 * Removed unneded pdecl's
 *
 * Revision 3.2  1995/11/17  18:42:21  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:49:43  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.7  1995/10/03  22:54:43  tcwinn
 * CSCdi41283:  appn port rsrb doesnt allow ring number gt 255
 * local and remote ring number can be between 1 and 4095
 *
 * Revision 2.6  1995/10/03  21:46:08  tcwinn
 * CSCdi41283:  appn port rsrb doesnt allow ring number gt 255
 * local and remote ring number now can be between 1 and 4096.
 *
 * Revision 2.5  1995/07/31  19:44:57  dmccowan
 * CSCdi37620:  max-link-stations needs to be bigger
 * increase max-link-stations from 255 to 65535.
 *
 * Revision 2.4  1995/07/27  22:29:54  dmccowan
 * CSCdi36612:  minor problems with appn commands
 *
 * Revision 2.3  1995/07/26  01:44:13  vtseng
 * CSCdi31824:  appn link station config command needs more checking
 * Also piggyback fixes for CSCdi31836 that SAP should be divisible
 * by 2.
 *
 * Revision 2.2  1995/07/19  22:32:53  vtseng
 * CSCdi31836:  appn port config command needs more checking
 *
 * Revision 2.1  1995/06/07  22:01:34  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 * $Endlog$
*/


/******************************************************************
 * appn port <portname>
 *    ...subcommands...
 */


/*
 *   exit
 */
EOLNS	(appn_port_exit_eol, exit_appn_command);
KEYWORD	(appn_port_exit_kwd, appn_port_exit_eol, NONE,
	 "exit", "Exit from appn-port configuration mode", PRIV_CONF);
TESTVAR	(appn_port_exit, NONE, appn_port_exit_kwd, NONE, NONE, ALTERNATE,
	 sense, FALSE);
#undef  ALTERNATE
#define ALTERNATE appn_port_exit

/*
 *   [no] complete
 */
EOLS      (cfg_appn_port_comp_eol, appn_command, APPN_PORT_COMP);
KEYWORD_ID(cfg_appn_port_comp, cfg_appn_port_comp_eol, ALTERNATE,
           appn_subcommand_type, APPN_PORT_COMP,
           "complete", "Complete port definition.", PRIV_CONF);
#undef  ALTERNATE
#define ALTERNATE cfg_appn_port_comp

/*
 *   [no] qllc pvc|svc <x25-subaddress>
 */

EOLS	  (cfg_appn_port_qllc_eol, appn_command, APPN_PORT_CONNECT);
X121ADDR   (cfg_appn_port_qllc_svc_val, cfg_appn_port_qllc_eol, no_alt,
           OBJ(paddr,1), APPN_PORT_QLLC_SVC_VAL_HELP);
KEYWORD_ID (cfg_appn_port_qllc_svc, cfg_appn_port_qllc_svc_val, 
	   cfg_appn_port_qllc_eol,
	   OBJ(int,1), FALSE, APPN_PORT_QLLC_SVC_KEY,
	   APPN_PORT_QLLC_SVC_KEY_HELP, PRIV_CONF);

PRINT      (cfg_appn_port_qllc_pvc_error, cfg_appn_port_qllc_eol, "PVC range error");
IFELSE     (cfg_appn_port_qllc_pvc_check, cfg_appn_port_qllc_eol, 
	   cfg_appn_port_qllc_pvc_error,
	   ( GETOBJ(int,3) > GETOBJ(int,2) ) );
NUMBER     (cfg_appn_port_qllc_pvc_high, cfg_appn_port_qllc_pvc_check, 
	   cfg_appn_port_qllc_eol,
	   OBJ(int,3), 1, 4095, APPN_PORT_QLLC_PVC_VAL_HELP);
NUMBER     (cfg_appn_port_qllc_pvc_low, cfg_appn_port_qllc_pvc_high, no_alt,
	   OBJ(int,2), 1, 4095, APPN_PORT_QLLC_PVC_VAL_HELP);
KEYWORD_ID (cfg_appn_port_qllc_pvc, cfg_appn_port_qllc_pvc_low, 
	   cfg_appn_port_qllc_svc,
	   OBJ(int,1), TRUE, APPN_PORT_QLLC_PVC_KEY,
	   APPN_PORT_QLLC_PVC_KEY_HELP, PRIV_CONF);
NVGENS    (cfg_appn_port_qllc_nvgen, cfg_appn_port_qllc_pvc,
           appn_command, APPN_PORT_QLLC);
KEYWORD_ID(cfg_appn_port_qllc, cfg_appn_port_qllc_nvgen,ALTERNATE,
           appn_subcommand_type, APPN_PORT_QLLC,
           APPN_PORT_QLLC_KEY, 
           APPN_PORT_QLLC_KEY_HELP, PRIV_CONF);
#undef  ALTERNATE
#define ALTERNATE cfg_appn_port_qllc

/*
 *   [no] sdlc-sec-address <address>
 */

EOLS	  (cfg_appn_port_sdlc_eol, appn_command, APPN_PORT_CONNECT);
HEXDIGIT  (cfg_appn_port_sdlc_addr, cfg_appn_port_sdlc_eol, no_alt,
	  OBJ(int,1), 1, 0xFE, APPN_PORT_SDLC_ADDR_VAL_HELP);
NOPREFIX  (cfg_appn_port_sdlc_noprefix, cfg_appn_port_sdlc_addr, cfg_appn_port_sdlc_eol);
NVGENS    (cfg_appn_port_sdlc_nvgen, cfg_appn_port_sdlc_noprefix,
           appn_command, APPN_PORT_SDLC);
KEYWORD_ID(cfg_appn_port_sdlc, cfg_appn_port_sdlc_nvgen,ALTERNATE,
           appn_subcommand_type, APPN_PORT_SDLC,
           APPN_PORT_SDLC_KEY, 
           APPN_PORT_SDLC_KEY_HELP, PRIV_CONF);
#undef  ALTERNATE
#define ALTERNATE cfg_appn_port_sdlc

/*
 *   [no] vdlc <ring-group> [vmac <virtual-mac>]
 */
EOLS	  (cfg_appn_port_vdlc_eol, appn_command, APPN_PORT_CONNECT);
ENADDR	(cfg_appn_port_vdlc_vaddr, cfg_appn_port_vdlc_eol, cfg_appn_port_vdlc_eol,
 	 OBJ(hwaddr,1), APPN_PORT_VDLC_VADDR_VAL_HELP);
KEYWORD_ID(cfg_appn_port_vdlc_vaddr_flag, cfg_appn_port_vdlc_vaddr, no_alt,
	 OBJ(int, 2), TRUE,
         "vmac", APPN_PORT_VDLC_VADDR_VAL_HELP,PRIV_USER);
NUMBER	(cfg_appn_port_vdlc_ring, cfg_appn_port_vdlc_vaddr_flag, cfg_appn_port_vdlc_eol,
 	 OBJ(int,1), 1, 4095, APPN_PORT_VDLC_RING_VAL_HELP);
NOPREFIX  (cfg_appn_port_vdlc_noprefix, cfg_appn_port_vdlc_ring, cfg_appn_port_vdlc_eol);
NVGENS    (cfg_appn_port_vdlc_nvgen, cfg_appn_port_vdlc_noprefix,
	   appn_command, APPN_PORT_VDLC);
KEYWORD_ID(cfg_appn_port_vdlc, cfg_appn_port_vdlc_nvgen,ALTERNATE,
	   appn_subcommand_type, APPN_PORT_VDLC,
	   APPN_PORT_VDLC_KEY, 
	   APPN_PORT_VDLC_KEY_HELP, PRIV_CONF);
#undef  ALTERNATE
#define ALTERNATE cfg_appn_port_vdlc


/*
 *   [no] rsrb-virtual-station <virtual-mac> <local-ring> <bridge-number> <remote-ring>
 */
EOLS	  (cfg_appn_port_rsrb_eol, appn_command, APPN_PORT_CONNECT);
NUMBER    (cfg_appn_port_rsrb_rring, cfg_appn_port_rsrb_eol, no_alt,
           OBJ(int,3), 1, 4095, APPN_PORT_RSRB_RRING_VAL_HELP);
NUMBER    (cfg_appn_port_rsrb_bridge_num, cfg_appn_port_rsrb_rring, no_alt,
           OBJ(int,2), 1, 15, APPN_PORT_RSRB_BRIDGE_NUM_VAL_HELP);
NUMBER	(cfg_appn_port_rsrb_lring, cfg_appn_port_rsrb_bridge_num, no_alt,
	 OBJ(int,1), 1, 4095, APPN_PORT_RSRB_LRING_VAL_HELP);
ENADDR	(cfg_appn_port_rsrb_vaddr, cfg_appn_port_rsrb_lring, no_alt,
	 OBJ(hwaddr,1), APPN_PORT_RSRB_VADDR_VAL_HELP);
NOPREFIX  (cfg_appn_port_rsrb_noprefix, cfg_appn_port_rsrb_vaddr, cfg_appn_port_rsrb_eol);
NVGENS    (cfg_appn_port_rsrb_nvgen, cfg_appn_port_rsrb_noprefix,
           appn_command, APPN_PORT_RSRB);
KEYWORD_ID(cfg_appn_port_rsrb, cfg_appn_port_rsrb_nvgen,ALTERNATE,
           appn_subcommand_type, APPN_PORT_RSRB,
           APPN_PORT_RSRB_KEY, 
           APPN_PORT_RSRB_KEY_HELP, PRIV_CONF);
#undef  ALTERNATE
#define ALTERNATE cfg_appn_port_rsrb

/*
 *    [no] service-any
 */
EOLS	  (cfg_appn_port_any_eol, appn_command, APPN_PORT_ANY);
KEYWORD_ID(cfg_appn_port_any, cfg_appn_port_any_eol,ALTERNATE,
           appn_subcommand_type, APPN_PORT_ANY,
           APPN_PORT_ANY_KEY, APPN_PORT_ANY_KEY_HELP, PRIV_CONF);
#undef  ALTERNATE
#define ALTERNATE cfg_appn_port_any

/*
 *    [no] retry infinite|<tries> [<interval>]
 */
EOLS	  (cfg_appn_port_retry_eol, appn_command, APPN_PORT_RETRY);
NUMBER    (cfg_appn_port_retry_int, cfg_appn_port_retry_eol, 
           cfg_appn_port_retry_eol,
           OBJ(int,3), 0,32767, APPN_PORT_RETRY_TIME_HELP);
NUMBER    (cfg_appn_port_retry_tries, cfg_appn_port_retry_int, no_alt,
           OBJ(int,2), 0,255, APPN_PORT_RETRY_TRIES_HELP);
KEYWORD_ID(cfg_appn_port_retry_inf, cfg_appn_port_retry_int,
           cfg_appn_port_retry_tries,
           OBJ(int,1), TRUE,
           APPN_PORT_RETRY_INF, APPN_PORT_RETRY_INF_KEY_HELP, PRIV_CONF);
NOPREFIX  (cfg_appn_port_retry_noprefix, cfg_appn_port_retry_inf, 
           cfg_appn_port_retry_eol);
NVGENS    (cfg_appn_port_retry_nvgen, cfg_appn_port_retry_noprefix,
           appn_command, APPN_PORT_RETRY);
KEYWORD_ID(cfg_appn_port_retry, cfg_appn_port_retry_nvgen,ALTERNATE,
           appn_subcommand_type, APPN_PORT_RETRY,
           APPN_PORT_RETRY_KEY, APPN_PORT_RETRY_KEY_HELP, PRIV_CONF);
#undef  ALTERNATE
#define ALTERNATE cfg_appn_port_retry

/*
 *    [no] limited-resource
 */
EOLS	  (cfg_appn_port_limit_eol, appn_command, APPN_PORT_LIMIT);
KEYWORD_ID(cfg_appn_port_limit, cfg_appn_port_limit_eol,ALTERNATE,
           appn_subcommand_type, APPN_PORT_LIMIT,
           APPN_PORT_LIMIT_KEY, APPN_PORT_LIMIT_KEY_HELP, PRIV_CONF);
#undef  ALTERNATE
#define ALTERNATE cfg_appn_port_limit

/*
 *    [no] len-connection
 */
EOLS	  (cfg_appn_port_len_eol, appn_command, APPN_PORT_LEN);
KEYWORD_ID(cfg_appn_port_len, cfg_appn_port_len_eol,ALTERNATE,
           appn_subcommand_type, APPN_PORT_LEN,
           APPN_PORT_LEN_KEY, APPN_PORT_LEN_KEY_HELP, PRIV_CONF);
#undef  ALTERNATE
#define ALTERNATE cfg_appn_port_len

/*
 *   [no] role <role>
 */
EOLS	  (cfg_appn_port_role_eol, appn_command, APPN_PORT_ROLE);
KEYWORD_ID(cfg_appn_port_role_neg, cfg_appn_port_role_eol,
           no_alt,
           OBJ(int,1), AP_NEGOTIABLE,
           APPN_PORT_ROLE_NEG, 
           APPN_PORT_ROLE_NEG_KEY_HELP, PRIV_CONF);
KEYWORD_ID(cfg_appn_port_role_sec, cfg_appn_port_role_eol,
           cfg_appn_port_role_neg,
           OBJ(int,1), AP_SECONDARY,
           APPN_PORT_ROLE_SEC, 
           APPN_PORT_ROLE_SEC_KEY_HELP, PRIV_CONF);
KEYWORD_ID(cfg_appn_port_role_pri, cfg_appn_port_role_eol,
           cfg_appn_port_role_sec,
           OBJ(int,1), AP_PRIMARY,
           APPN_PORT_ROLE_PRI, 
           APPN_PORT_ROLE_PRI_KEY_HELP, PRIV_CONF);
NOPREFIX  (cfg_appn_port_role_noprefix, cfg_appn_port_role_pri, 
           cfg_appn_port_role_eol);
NVGENS    (cfg_appn_port_role_nvgen, cfg_appn_port_role_noprefix,
           appn_command, APPN_PORT_ROLE);
KEYWORD_ID(cfg_appn_port_role, cfg_appn_port_role_nvgen,ALTERNATE,
           appn_subcommand_type, APPN_PORT_ROLE,
           APPN_PORT_ROLE_KEY, 
           APPN_PORT_ROLE_KEY_HELP, PRIV_CONF);
#undef  ALTERNATE
#define ALTERNATE cfg_appn_port_role

/*
 *   [no] user-defined-3 <val>
 */
EOLS	  (cfg_appn_port_user3_eol, appn_command, APPN_PORT_USER3);
NUMBER    (cfg_appn_port_user3_num, cfg_appn_port_user3_eol, no_alt,
           OBJ(int,3), 0, 255, APPN_PORT_USER3_VAL_HELP);
NOPREFIX  (cfg_appn_port_user3_noprefix, cfg_appn_port_user3_num, cfg_appn_port_user3_eol);
NVGENS    (cfg_appn_port_user3_nvgen, cfg_appn_port_user3_noprefix,
           appn_command, APPN_PORT_USER3);
KEYWORD_ID(cfg_appn_port_user3, cfg_appn_port_user3_nvgen,ALTERNATE,
           appn_subcommand_type, APPN_PORT_USER3,
           APPN_PORT_USER3_KEY, 
           APPN_PORT_USER3_KEY_HELP, PRIV_CONF);
#undef  ALTERNATE
#define ALTERNATE cfg_appn_port_user3

/*
 *   [no] user-defined-2 <val>
 */
EOLS	  (cfg_appn_port_user2_eol, appn_command, APPN_PORT_USER2);
NUMBER    (cfg_appn_port_user2_num, cfg_appn_port_user2_eol, no_alt,
           OBJ(int,3), 0, 255, APPN_PORT_USER2_VAL_HELP);
NOPREFIX  (cfg_appn_port_user2_noprefix, cfg_appn_port_user2_num, cfg_appn_port_user2_eol);
NVGENS    (cfg_appn_port_user2_nvgen, cfg_appn_port_user2_noprefix,
           appn_command, APPN_PORT_USER2);
KEYWORD_ID(cfg_appn_port_user2, cfg_appn_port_user2_nvgen,ALTERNATE,
           appn_subcommand_type, APPN_PORT_USER2,
           APPN_PORT_USER2_KEY, 
           APPN_PORT_USER2_KEY_HELP, PRIV_CONF);
#undef  ALTERNATE
#define ALTERNATE cfg_appn_port_user2

/*
 *   [no] user-defined-1 <val>
 */
EOLS	  (cfg_appn_port_user1_eol, appn_command, APPN_PORT_USER1);
NUMBER    (cfg_appn_port_user1_num, cfg_appn_port_user1_eol, no_alt,
           OBJ(int,3), 0, 255, APPN_PORT_USER1_VAL_HELP);
NOPREFIX  (cfg_appn_port_user1_noprefix, cfg_appn_port_user1_num, cfg_appn_port_user1_eol);
NVGENS    (cfg_appn_port_user1_nvgen, cfg_appn_port_user1_noprefix,
           appn_command, APPN_PORT_USER1);
KEYWORD_ID(cfg_appn_port_user1, cfg_appn_port_user1_nvgen,ALTERNATE,
           appn_subcommand_type, APPN_PORT_USER1,
           APPN_PORT_USER1_KEY, 
           APPN_PORT_USER1_KEY_HELP, PRIV_CONF);
#undef  ALTERNATE
#define ALTERNATE cfg_appn_port_user1

/*
 *   [no] security <security>
 */
EOLS	  (cfg_appn_port_secur_eol, appn_command, APPN_PORT_SECUR);
KEYWORD_ID(cfg_appn_port_secur_rad, cfg_appn_port_secur_eol,
           no_alt,
           OBJ(int,1), AP_SEC_GUARDED_RADIATION,
           APPN_PORT_SECUR_RAD, 
           APPN_PORT_SECUR_RAD_KEY_HELP, PRIV_CONF);
KEYWORD_ID(cfg_appn_port_secur_enc, cfg_appn_port_secur_eol,
           cfg_appn_port_secur_rad,
           OBJ(int,1), AP_SEC_ENCRYPTED ,
           APPN_PORT_SECUR_ENC, 
           APPN_PORT_SECUR_ENC_KEY_HELP, PRIV_CONF);
KEYWORD_ID(cfg_appn_port_secur_gua, cfg_appn_port_secur_eol,
           cfg_appn_port_secur_enc,
           OBJ(int,1), AP_SEC_GUARDED_CONDUIT ,
           APPN_PORT_SECUR_GUA, 
           APPN_PORT_SECUR_GUA_KEY_HELP, PRIV_CONF);
KEYWORD_ID(cfg_appn_port_secur_sec, cfg_appn_port_secur_eol,
           cfg_appn_port_secur_gua,
           OBJ(int,1), AP_SEC_SECURE_CONDUIT ,
           APPN_PORT_SECUR_SEC, 
           APPN_PORT_SECUR_SEC_KEY_HELP, PRIV_CONF);
KEYWORD_ID(cfg_appn_port_secur_und, cfg_appn_port_secur_eol,
           cfg_appn_port_secur_sec,
           OBJ(int,1), AP_SEC_UNDERGROUND_CABLE ,
           APPN_PORT_SECUR_UND, 
           APPN_PORT_SECUR_UND_KEY_HELP, PRIV_CONF);
KEYWORD_ID(cfg_appn_port_secur_pub, cfg_appn_port_secur_eol,
           cfg_appn_port_secur_und,
           OBJ(int,1), AP_SEC_PUBLIC_SWITCHED_NETWORK,
           APPN_PORT_SECUR_PUB, 
           APPN_PORT_SECUR_PUB_KEY_HELP, PRIV_CONF);
KEYWORD_ID(cfg_appn_port_secur_non, cfg_appn_port_secur_eol,
           cfg_appn_port_secur_pub,
           OBJ(int,1), AP_SEC_NONSECURE,
           APPN_PORT_SECUR_NON, 
           APPN_PORT_SECUR_NON_KEY_HELP, PRIV_CONF);
NOPREFIX  (cfg_appn_port_secur_noprefix, cfg_appn_port_secur_non, 
           cfg_appn_port_secur_eol);
NVGENS    (cfg_appn_port_secur_nvgen, cfg_appn_port_secur_noprefix,
           appn_command, APPN_PORT_SECUR);
KEYWORD_ID(cfg_appn_port_secur, cfg_appn_port_secur_nvgen,ALTERNATE,
           appn_subcommand_type, APPN_PORT_SECUR,
           APPN_PORT_SECUR_KEY, 
           APPN_PORT_SECUR_KEY_HELP, PRIV_CONF);
#undef  ALTERNATE
#define ALTERNATE cfg_appn_port_secur

/*
 *   [no] propagation_delay <delay>
 */
EOLS	  (cfg_appn_port_delay_eol, appn_command, APPN_PORT_DELAY);
KEYWORD_ID(cfg_appn_port_delay_max, cfg_appn_port_delay_eol,
           no_alt,
           OBJ(int,1), AP_PROP_DELAY_MAXIMUM,
           APPN_PORT_DELAY_MAX, 
           APPN_PORT_DELAY_MAX_KEY_HELP, PRIV_CONF);
KEYWORD_ID(cfg_appn_port_delay_sat, cfg_appn_port_delay_eol,
           cfg_appn_port_delay_max,
           OBJ(int,1), AP_PROP_DELAY_SATELLITE,
           APPN_PORT_DELAY_SAT, 
           APPN_PORT_DELAY_SAT_KEY_HELP, PRIV_CONF);
KEYWORD_ID(cfg_appn_port_delay_pack, cfg_appn_port_delay_eol,
           cfg_appn_port_delay_sat,
           OBJ(int,1), AP_PROP_DELAY_PKT_SWITCHED_NET,
           APPN_PORT_DELAY_PACK, 
           APPN_PORT_DELAY_PACK_KEY_HELP, PRIV_CONF);
KEYWORD_ID(cfg_appn_port_delay_phone, cfg_appn_port_delay_eol,
           cfg_appn_port_delay_pack,
           OBJ(int,1), AP_PROP_DELAY_TELEPHONE,
           APPN_PORT_DELAY_PHONE, 
           APPN_PORT_DELAY_PHONE_KEY_HELP, PRIV_CONF);
KEYWORD_ID(cfg_appn_port_delay_lan, cfg_appn_port_delay_eol,
           cfg_appn_port_delay_phone,
           OBJ(int,1), AP_PROP_DELAY_LAN,
           APPN_PORT_DELAY_LAN, 
           APPN_PORT_DELAY_LAN_KEY_HELP, PRIV_CONF);
KEYWORD_ID(cfg_appn_port_delay_min, cfg_appn_port_delay_eol,
           cfg_appn_port_delay_lan,
           OBJ(int,1), AP_PROP_DELAY_MINIMUM,
           APPN_PORT_DELAY_MIN, 
           APPN_PORT_DELAY_MIN_KEY_HELP, PRIV_CONF);
NOPREFIX  (cfg_appn_port_delay_noprefix, cfg_appn_port_delay_min, 
           cfg_appn_port_delay_eol);
NVGENS    (cfg_appn_port_delay_nvgen, cfg_appn_port_delay_noprefix,
           appn_command, APPN_PORT_DELAY);
KEYWORD_ID(cfg_appn_port_delay, cfg_appn_port_delay_nvgen,ALTERNATE,
           appn_subcommand_type, APPN_PORT_DELAY,
           APPN_PORT_DELAY_KEY, 
           APPN_PORT_DELAY_KEY_HELP, PRIV_CONF);
#undef  ALTERNATE
#define ALTERNATE cfg_appn_port_delay

/*
 *   [no] cost-per-connect-time <cost>
 */
EOLS	  (cfg_appn_port_connect_eol, appn_command, APPN_PORT_CONNECT);
NUMBER    (cfg_appn_port_connect_num, cfg_appn_port_connect_eol, no_alt,
           OBJ(int,3), 0, 255, APPN_PORT_CONNECT_VAL_HELP);
NOPREFIX  (cfg_appn_port_connect_noprefix, cfg_appn_port_connect_num, cfg_appn_port_connect_eol);
NVGENS    (cfg_appn_port_connect_nvgen, cfg_appn_port_connect_noprefix,
           appn_command, APPN_PORT_CONNECT);
KEYWORD_ID(cfg_appn_port_connect, cfg_appn_port_connect_nvgen,ALTERNATE,
           appn_subcommand_type, APPN_PORT_CONNECT,
           APPN_PORT_CONNECT_KEY, 
           APPN_PORT_CONNECT_KEY_HELP, PRIV_CONF);
#undef  ALTERNATE
#define ALTERNATE cfg_appn_port_connect

/*
 *   [no] cost-per-byte <cost>
 */
EOLS	  (cfg_appn_port_byte_eol, appn_command, APPN_PORT_BYTE);
NUMBER    (cfg_appn_port_byte_num, cfg_appn_port_byte_eol, no_alt,
           OBJ(int,3), 0, 255, APPN_PORT_BYTE_VAL_HELP);
NOPREFIX  (cfg_appn_port_byte_noprefix, cfg_appn_port_byte_num, cfg_appn_port_byte_eol);
NVGENS    (cfg_appn_port_byte_nvgen, cfg_appn_port_byte_noprefix,
           appn_command, APPN_PORT_BYTE);
KEYWORD_ID(cfg_appn_port_byte, cfg_appn_port_byte_nvgen,ALTERNATE,
           appn_subcommand_type, APPN_PORT_BYTE,
           APPN_PORT_BYTE_KEY, 
           APPN_PORT_BYTE_KEY_HELP, PRIV_CONF);
#undef  ALTERNATE
#define ALTERNATE cfg_appn_port_byte


/*
 *   [no] effective-capacity <cap>
 */
EOLS	  (cfg_appn_port_effcap_eol, appn_command, APPN_PORT_EFFCAP);
NUMBER    (cfg_appn_port_effcap_num, cfg_appn_port_effcap_eol, no_alt,
           OBJ(int,3), 0, 16000000, APPN_PORT_EFFCAP_VAL_HELP);
NOPREFIX  (cfg_appn_port_effcap_noprefix, cfg_appn_port_effcap_num, cfg_appn_port_effcap_eol);
NVGENS    (cfg_appn_port_effcap_nvgen, cfg_appn_port_effcap_noprefix,
           appn_command, APPN_PORT_EFFCAP);
KEYWORD_ID(cfg_appn_port_effcap, cfg_appn_port_effcap_nvgen,ALTERNATE,
           appn_subcommand_type, APPN_PORT_EFFCAP,
           APPN_PORT_EFFCAP_KEY, 
           APPN_PORT_EFFCAP_KEY_HELP, PRIV_CONF);
#undef  ALTERNATE
#define ALTERNATE cfg_appn_port_effcap

/*
 *   [no] max-rcv-btu-size <size>
 */
EOLS	  (cfg_appn_port_rcvbtu_eol, appn_command, APPN_PORT_RCVBTU);
NUMBER    (cfg_appn_port_rcvbtu_num, cfg_appn_port_rcvbtu_eol, no_alt,
           OBJ(int,1), 99, 5107, APPN_PORT_RCVBTU_VAL_HELP);
NOPREFIX  (cfg_appn_port_rcvbtu_noprefix, cfg_appn_port_rcvbtu_num, cfg_appn_port_rcvbtu_eol);
NVGENS    (cfg_appn_port_rcvbtu_nvgen, cfg_appn_port_rcvbtu_noprefix,
           appn_command, APPN_PORT_RCVBTU);
KEYWORD_ID(cfg_appn_port_rcvbtu, cfg_appn_port_rcvbtu_nvgen,ALTERNATE,
           appn_subcommand_type, APPN_PORT_RCVBTU,
           APPN_PORT_RCVBTU_KEY, 
           APPN_PORT_RCVBTU_KEY_HELP, PRIV_CONF);
#undef  ALTERNATE
#define ALTERNATE cfg_appn_port_rcvbtu

/*
 *   [no] desired-max-send-btu-size <size>
 */
EOLS	  (cfg_appn_port_sndbtu_eol, appn_command, APPN_PORT_SNDBTU);
NUMBER    (cfg_appn_port_sndbtu_num, cfg_appn_port_sndbtu_eol, no_alt,
           OBJ(int,1), 99, 5107, APPN_PORT_SNDBTU_VAL_HELP);
NOPREFIX  (cfg_appn_port_sndbtu_noprefix, cfg_appn_port_sndbtu_num, cfg_appn_port_sndbtu_eol);
NVGENS    (cfg_appn_port_sndbtu_nvgen, cfg_appn_port_sndbtu_noprefix,
           appn_command, APPN_PORT_SNDBTU);
KEYWORD_ID(cfg_appn_port_sndbtu, cfg_appn_port_sndbtu_nvgen,ALTERNATE,
           appn_subcommand_type, APPN_PORT_SNDBTU,
           APPN_PORT_SNDBTU_KEY, 
           APPN_PORT_SNDBTU_KEY_HELP, PRIV_CONF);
#undef  ALTERNATE
#define ALTERNATE cfg_appn_port_sndbtu

/*
 *   [no] reserved-inbound <num>
 */
EOLS	  (cfg_appn_port_inb_eol, appn_command, APPN_PORT_INB);
NUMBER    (cfg_appn_port_inb_num, cfg_appn_port_inb_eol, no_alt,
           OBJ(int,1), 0, 65535, APPN_PORT_INB_VAL_HELP);
NOPREFIX  (cfg_appn_port_inb_noprefix, cfg_appn_port_inb_num, cfg_appn_port_inb_eol);
NVGENS    (cfg_appn_port_inb_nvgen, cfg_appn_port_inb_noprefix,
           appn_command, APPN_PORT_INB);
KEYWORD_ID(cfg_appn_port_inb, cfg_appn_port_inb_nvgen,ALTERNATE,
           appn_subcommand_type, APPN_PORT_INB,
           APPN_PORT_INB_KEY, 
           APPN_PORT_INB_KEY_HELP, PRIV_CONF);
#undef  ALTERNATE
#define ALTERNATE cfg_appn_port_inb

/*
 *   [no] reserved-outbound <num>
 */
EOLS	  (cfg_appn_port_outb_eol, appn_command, APPN_PORT_OUTB);
NUMBER    (cfg_appn_port_outb_num, cfg_appn_port_outb_eol, no_alt,
           OBJ(int,1), 0, 65535, APPN_PORT_OUTB_VAL_HELP);
NOPREFIX  (cfg_appn_port_outb_noprefix, cfg_appn_port_outb_num, cfg_appn_port_outb_eol);
NVGENS    (cfg_appn_port_outb_nvgen, cfg_appn_port_outb_noprefix,
           appn_command, APPN_PORT_OUTB);
KEYWORD_ID(cfg_appn_port_outb, cfg_appn_port_outb_nvgen,ALTERNATE,
           appn_subcommand_type, APPN_PORT_OUTB,
           APPN_PORT_OUTB_KEY, 
           APPN_PORT_OUTB_KEY_HELP, PRIV_CONF);
#undef  ALTERNATE
#define ALTERNATE cfg_appn_port_outb

/*
 *   [no] max-link-stations <num>
 */
EOLS	  (cfg_appn_port_maxl_eol, appn_command, APPN_PORT_MAXL);
NUMBER    (cfg_appn_port_maxl_num, cfg_appn_port_maxl_eol, no_alt,
           OBJ(int,1), 1, 65535, APPN_PORT_MAXL_VAL_HELP);
NOPREFIX  (cfg_appn_port_maxl_noprefix, cfg_appn_port_maxl_num, cfg_appn_port_maxl_eol);
NVGENS    (cfg_appn_port_maxl_nvgen, cfg_appn_port_maxl_noprefix,
           appn_command, APPN_PORT_MAXL);
KEYWORD_ID(cfg_appn_port_maxl, cfg_appn_port_maxl_nvgen,ALTERNATE,
           appn_subcommand_type, APPN_PORT_MAXL,
           APPN_PORT_MAXL_KEY, 
           APPN_PORT_MAXL_KEY_HELP, PRIV_CONF);
#undef  ALTERNATE
#define ALTERNATE cfg_appn_port_maxl

/*
 *    [no] local-sap <sap-num>
 */
EOLS	  (cfg_appn_port_sap_eol, appn_command, APPN_PORT_SAP);
HEXDIGIT    (cfg_appn_port_sap_num, cfg_appn_port_sap_eol, no_alt,
           OBJ(int,1), 0x02, 0xec, APPN_PORT_SAP_VAL_HELP);
NOPREFIX  (cfg_appn_port_sap_noprefix, cfg_appn_port_sap_num, cfg_appn_port_sap_eol);
NVGENS    (cfg_appn_port_sap_nvgen, cfg_appn_port_sap_noprefix,
           appn_command, APPN_PORT_SAP);
KEYWORD_ID(cfg_appn_port_sap, cfg_appn_port_sap_nvgen,ALTERNATE,
           appn_subcommand_type, APPN_PORT_SAP,
           APPN_PORT_SAP_KEY, APPN_PORT_SAP_KEY_HELP, PRIV_CONF);
#undef  ALTERNATE
#define ALTERNATE cfg_appn_port_sap

/*
 *    [no] llc2-idle-time <milliseconds>
 */
EOLS	  (cfg_appn_port_idle_eol, appn_command, APPN_PORT_IDLE);
NUMBER    (cfg_appn_port_idle_num, cfg_appn_port_idle_eol, no_alt,
           OBJ(int,1), 1, 60000, APPN_PORT_IDLE_VAL_HELP);
NOPREFIX  (cfg_appn_port_idle_noprefix, cfg_appn_port_idle_num, cfg_appn_port_idle_eol);
NVGENS    (cfg_appn_port_idle_nvgen, cfg_appn_port_idle_noprefix,
           appn_command, APPN_PORT_IDLE);
KEYWORD_ID(cfg_appn_port_idle, cfg_appn_port_idle_nvgen,ALTERNATE,
           appn_subcommand_type, APPN_PORT_IDLE,
           APPN_PORT_IDLE_KEY, APPN_PORT_IDLE_KEY_HELP, PRIV_CONF);
#undef  ALTERNATE
#define ALTERNATE cfg_appn_port_idle

/*
 *    [no] llc2-ack-delay-time <milliseconds>
 */
EOLS	  (cfg_appn_port_llc2_delay_eol, appn_command, APPN_PORT_LLC2_DELAY);
NUMBER    (cfg_appn_port_llc2_delay_num, cfg_appn_port_llc2_delay_eol, no_alt,
           OBJ(int,1), 1, 60000, APPN_PORT_LLC2_DELAY_VAL_HELP);
NOPREFIX  (cfg_appn_port_llc2_delay_noprefix, cfg_appn_port_llc2_delay_num, cfg_appn_port_llc2_delay_eol);
NVGENS    (cfg_appn_port_llc2_delay_nvgen, cfg_appn_port_llc2_delay_noprefix,
           appn_command, APPN_PORT_LLC2_DELAY);
KEYWORD_ID(cfg_appn_port_llc2_delay, cfg_appn_port_llc2_delay_nvgen,ALTERNATE,
           appn_subcommand_type, APPN_PORT_LLC2_DELAY,
           APPN_PORT_LLC2_DELAY_KEY, APPN_PORT_LLC2_DELAY_KEY_HELP, PRIV_CONF);
#undef  ALTERNATE
#define ALTERNATE cfg_appn_port_llc2_delay

/*
 *    [no] llc2-ack-max <# I-frames>
 */
EOLS	  (cfg_appn_port_ackmax_eol, appn_command, APPN_PORT_ACKMAX);
NUMBER    (cfg_appn_port_ackmax_num, cfg_appn_port_ackmax_eol, no_alt,
           OBJ(int,1), 1, 255, APPN_PORT_ACKMAX_VAL_HELP);
NOPREFIX  (cfg_appn_port_ackmax_noprefix, cfg_appn_port_ackmax_num, cfg_appn_port_ackmax_eol);
NVGENS    (cfg_appn_port_ackmax_nvgen, cfg_appn_port_ackmax_noprefix,
           appn_command, APPN_PORT_ACKMAX);
KEYWORD_ID(cfg_appn_port_ackmax, cfg_appn_port_ackmax_nvgen,ALTERNATE,
           appn_subcommand_type, APPN_PORT_ACKMAX,
           APPN_PORT_ACKMAX_KEY, APPN_PORT_ACKMAX_KEY_HELP, PRIV_CONF);
#undef  ALTERNATE
#define ALTERNATE cfg_appn_port_ackmax

/*
 *    [no] llc2-local-window <# I-frames>
 */
EOLS	  (cfg_appn_port_lwin_eol, appn_command, APPN_PORT_LWIN);
NUMBER    (cfg_appn_port_lwin_num, cfg_appn_port_lwin_eol, no_alt,
           OBJ(int,1), 1, 127, APPN_PORT_LWIN_VAL_HELP);
NOPREFIX  (cfg_appn_port_lwin_noprefix, cfg_appn_port_lwin_num, cfg_appn_port_lwin_eol);
NVGENS    (cfg_appn_port_lwin_nvgen, cfg_appn_port_lwin_noprefix,
           appn_command, APPN_PORT_LWIN);
KEYWORD_ID(cfg_appn_port_lwin, cfg_appn_port_lwin_nvgen,ALTERNATE,
           appn_subcommand_type, APPN_PORT_LWIN,
           APPN_PORT_LWIN_KEY, APPN_PORT_LWIN_KEY_HELP, PRIV_CONF);
#undef  ALTERNATE
#define ALTERNATE cfg_appn_port_lwin

/*
 *    [no] llc2-t1-time <milliseconds>
 */
EOLS	  (cfg_appn_port_t1_eol, appn_command, APPN_PORT_T1);
NUMBER    (cfg_appn_port_t1_num, cfg_appn_port_t1_eol, no_alt,
           OBJ(int,1), 1, 60000, APPN_PORT_T1_VAL_HELP);
NOPREFIX  (cfg_appn_port_t1_noprefix, cfg_appn_port_t1_num, cfg_appn_port_t1_eol);
NVGENS    (cfg_appn_port_t1_nvgen, cfg_appn_port_t1_noprefix,
           appn_command, APPN_PORT_T1);
KEYWORD_ID(cfg_appn_port_t1, cfg_appn_port_t1_nvgen,ALTERNATE,
           appn_subcommand_type, APPN_PORT_T1,
           APPN_PORT_T1_KEY, APPN_PORT_T1_KEY_HELP, PRIV_CONF);
#undef  ALTERNATE
#define ALTERNATE cfg_appn_port_t1

/*
 *    [no] act-at-startup
 */
EOLS	  (cfg_appn_port_activate_eol, appn_command, APPN_PORT_ACTIVATE);
KEYWORD_ID(cfg_appn_port_activate, cfg_appn_port_activate_eol, ALTERNATE,
           appn_subcommand_type, APPN_PORT_ACTIVATE,
           APPN_PORT_ACTIVATE_KEY, APPN_PORT_ACTIVATE_KEY_HELP, PRIV_CONF);
#undef  ALTERNATE
#define ALTERNATE cfg_appn_port_activate
