/* $Id: cfg_int_ppp.h,v 3.9.10.7 1996/08/21 17:03:39 irfan Exp $
 * $Source: /release/112/cvs/Xsys/wan/cfg_int_ppp.h,v $
 *------------------------------------------------------------------
 * C F G _ I N T _ P P P . H
 *
 * Copyright (c) 1992-1997 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_int_ppp.h,v $
 * Revision 3.9.10.7  1996/08/21  17:03:39  irfan
 * CSCdi66659:  ppp max-bad-auth should be visible
 * Branch: California_branch
 * unhide the command.
 *
 * Revision 3.9.10.6  1996/05/22  19:06:56  snyder
 * CSCdi58423:  spellink errors
 * Branch: California_branch
 *
 * Connectted -> Connected
 *
 * Revision 3.9.10.5  1996/05/19  05:48:54  dblair
 * CSCdi57979:  Multilink clean up from Dial1_branch
 * Branch: California_branch
 * Optimize fragmentation to remove multiply and divides from the
 * data path.
 * Xmit fragments from the mlp_holdq_enqueue.
 * update BW when links are added and removed from bundle
 * Fix byte counts and packet counts on the virtual bundle interface
 * Clean up
 *
 * Revision 3.9.10.4  1996/05/10  01:44:21  wmay
 * CPP and PPP half-bridge commit
 * Branch: California_branch
 *
 * Revision 3.9.10.3  1996/05/06  17:03:24  fox
 * CSCdi49278:  Update PAP handling of outbound calls
 * Branch: California_branch
 * Three bug fixes in one!
 * Disable remote PAP authentication support by default.  Add 'ppp pap
 * sent-username xxx password yyy' to re-enable support if needed.  Warn
 * if sent-username/password matches entry in local database. (CSCdi49278)
 * Add 'ppp chap hostname xxx' command to allow multiple boxes to use the
 * same hostname in their CHAP Challenges.  (CSCdi44884)
 * Add 'ppp chap password yyy' command to allow a box to respond to many
 * unknown peers with the same password.  (CSCdi54088)
 *
 * Revision 3.9.10.2  1996/04/27  06:35:42  syiu
 * Branch: California_branch
 * Commit Dial1_branch
 *
 * Revision 3.6.8.5  1996/04/25  23:57:51  syiu
 * Branch: Dial1_branch
 * Dial1_branch to IosSec_postmerge_960419
 *
 * Revision 3.6.8.4  1996/04/16  23:59:46  syiu
 * Branch: Dial1_branch
 * Sync Dial1_branch to V111_1_3
 *
 * Revision 3.6.8.3  1996/04/06  21:26:51  dblair
 * Branch: Dial1_branch
 * Fix CSCdi53768 and added restart lcp when ppp multilink command
 * processed, and fix multilink not negotiated at reload problem.
 *
 * Revision 3.6.8.2  1996/03/29  15:54:48  dblair
 * Branch: Dial1_branch
 * Fix crash when clearing virtual interface and add some multilink knobs
 *
 * Revision 3.6.8.1  1996/01/16  15:45:07  dblair
 * CSCdi42375:  Move Multilink PPP into PPP subsystem
 * Branch: Dial1_branch
 *
 * Revision 3.9.10.1  1996/03/27  10:36:27  irfan
 * CSCdi36933:  Customer desires configurable PAP/CHAP retries
 * Branch: California_branch
 * Add PPP UI to make us accept more than one PAP/CHAP Authentication
 * attempt. Instead of hanging up the physical layer right away.
 *
 * Revision 3.9  1996/02/22  05:20:58  fox
 * CSCdi35908:  Should allow CHAP and PAP clients to same dialin pool
 * Add support for multiple acceptable inbound PPP authentication methods
 *
 * Revision 3.8  1996/02/18  07:23:56  fox
 * CSCdi49280:  Cant specify inbound authen only if AAA new-model enabled
 * Add TEST_MULTIPLE_FUNCS to fix 'ambiguous command' output
 *
 * Revision 3.7  1996/02/01  23:34:58  widmer
 * CSCdi45325:  Change Keyword/Set to use Keyword_Id macro
 *
 * Revision 3.6  1996/01/03  20:35:52  wmay
 * CSCdi38318:  cant specify inbound only ppp authentication - add callin
 * option to ppp authentication command
 *
 * Revision 3.5  1995/12/07  17:45:37  widmer
 * CSCdi45134:  Replace occurences of Ifelse with appropriate Test* macros
 *
 * Revision 3.4  1995/11/29  02:24:19  dblair
 * CSCdi43783:  ppp multilink command not available when aaa new-model
 * is enabled
 *
 * Revision 3.3  1995/11/29  01:58:54  dblair
 * CSCdi41333:  DDR Callback and Multilink PPP clean up
 *
 * Revision 3.2  1995/11/17  18:01:10  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:48:07  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/09/30  05:06:04  dblair
 * CSCdi41333:  DDR Callback initial commit
 * Adding Callback feature for DDR interfaces.
 *
 * Revision 2.2  1995/09/20  22:38:42  dblair
 * CSCdi40745:  PPP Multilink feature commit
 *
 * Revision 2.1  1995/07/02  01:45:55  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize xns, atalk, vines, decnet, xremote, x25, unix, wan, tn3270,
 * clns, cdp
 *
 * Revision 2.2  1995/06/21  03:12:46  gstovall
 * CSCdi35731:  Cleanup unused cruft in idb
 * Nuke some unused vectors and variables from the hwidb.
 *
 * Revision 2.1  1995/06/07  22:06:54  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/***************************************************************
 * [no] ppp multilink 
 * ppp multilink has to be last authentication config and maybe others
 *   need to be processed before multilink config.
 *   Since multilink config change may result in lcp renegotiation.
 */
EOLS(cfg_ppp_multilink_eol, ppp_command, PPP_MULTILINK);
NVGENS(cfg_ppp_multilink_nvgen, cfg_ppp_multilink_eol, ppp_command, 
       PPP_MULTILINK);
KEYWORD(cfg_ppp_multilink, cfg_ppp_multilink_nvgen, no_alt,
 	"multilink", "Make interface multilink capable", PRIV_CONF);

/***************************************************************
 * [no] ppp pap sent-username <string> password [num] <string>
 */
EOLS	(conf_ppp_pap_sentname_eol, ppp_command, PPP_PAP_SENT_NAME);
TEXT	(conf_ppp_pap_sentname_password_text, conf_ppp_pap_sentname_eol, no_alt,
	 OBJ(string,2), "Outbound PAP password");
TEST_MULTIPLE_FUNCS(conf_ppp_pap_sentname_test,
		    conf_ppp_pap_sentname_password_text,
		    no_alt, NONE);
NUMBER	(conf_ppp_pap_sentname_num, conf_ppp_pap_sentname_password_text,
	 conf_ppp_pap_sentname_test,
	 OBJ(int,1), 0, 7, "Encryption type (0=not yet encrypted)");
KEYWORD (conf_ppp_pap_sentname_password, conf_ppp_pap_sentname_num, no_alt,
	 "password", "Set outbound PAP password", PRIV_CONF);
STRING	(conf_ppp_pap_sentname_name, conf_ppp_pap_sentname_password, no_alt,
	 OBJ(string,1), "Outbound PAP username");
NOPREFIX(conf_ppp_pap_sentname_noprefix, conf_ppp_pap_sentname_name,
	 conf_ppp_pap_sentname_eol);
NVGENS	(conf_ppp_pap_sentname_nvgens, conf_ppp_pap_sentname_noprefix,
	 ppp_command, PPP_PAP_SENT_NAME);
KEYWORD	(conf_ppp_pap_sentname, conf_ppp_pap_sentname_nvgens, no_alt,
	 "sent-username", "Set outbound PAP username", PRIV_CONF);
KEYWORD	(conf_ppp_pap, conf_ppp_pap_sentname, cfg_ppp_multilink,
	 "pap", "Set PAP authentication parameters", PRIV_CONF);

/***************************************************************
 * [no] ppp direction < callin | callout | dedicated >
 */
EOLS	(conf_ppp_dir_eol, ppp_command, PPP_CALL_DIRECTION);
KEYWORD_ID(conf_ppp_dir_leased, conf_ppp_dir_eol, no_alt,
	   OBJ(int,1), PPP_NO_CALL,
	   "dedicated", "Treat connection as a dedicated line", PRIV_CONF);
KEYWORD_ID(conf_ppp_dir_callout, conf_ppp_dir_eol, conf_ppp_dir_leased,
	   OBJ(int,1), PPP_CALL_OUT,
	   "callout", "Treat connection as an initiated call", PRIV_CONF);
KEYWORD_ID(conf_ppp_dir_callin, conf_ppp_dir_eol, conf_ppp_dir_callout,
	   OBJ(int,1), PPP_CALL_IN,
	   "callin", "Treat connection as a received call", PRIV_CONF);
NOPREFIX(conf_ppp_dir_noprefix, conf_ppp_dir_callin, conf_ppp_dir_eol);
NVGENS	(conf_ppp_dir_nvgens, conf_ppp_dir_noprefix,
	 ppp_command, PPP_CALL_DIRECTION);
KEYWORD (conf_ppp_dir, conf_ppp_dir_nvgens, conf_ppp_pap,
	 "direction", "Force direction of PPP connection",
	 (PRIV_CONF|PRIV_HIDDEN));

/***************************************************************
 * [no] ppp chap splitnames
 */
EOLS	(conf_ppp_splitnames_eol, ppp_command, PPP_CHAP_SPLIT_NAMES);
KEYWORD (conf_ppp_splitnames, conf_ppp_splitnames_eol, no_alt,
	 "splitnames", "Allow peer to use two different CHAP names",
	 (PRIV_CONF|PRIV_HIDDEN));

#ifdef NOTDONE
/***************************************************************
 * [no] ppp chap < two-secrets | four-secrets >
 */
EOLS	(conf_ppp_quad_eol, ppp_command, PPP_CHAP_QUAD_SECRETS);
KEYWORD (conf_ppp_quad, conf_ppp_quad_eol, conf_ppp_splitnames,
	 "four-secrets",
	 "Extend CHAP secret based on username & call direction", PRIV_CONF);
EOLS	(conf_ppp_dual_eol, ppp_command, PPP_CHAP_DUAL_SECRETS);
KEYWORD (conf_ppp_dual, conf_ppp_dual_eol, conf_ppp_quad,
	 "two-secrets",
	 "Extend CHAP secret based on username", PRIV_CONF);
#endif

/***************************************************************
 * [no] ppp chap wait
 */
EOLS	(conf_ppp_wait_eol, ppp_command, PPP_CHAP_WAIT);
KEYWORD (conf_ppp_wait, conf_ppp_wait_eol, conf_ppp_splitnames,
	 "wait", "Wait for caller to authenticate first", PRIV_CONF);

/***************************************************************
 * [no] ppp chap refuse [callin]
 */
EOLS	(conf_ppp_refuse_callin_eol, ppp_command, PPP_REFUSE_CHAP_CALLIN);
EOLS	(conf_ppp_refuse_eol, ppp_command, PPP_REFUSE_CHAP);
KEYWORD (conf_ppp_refuse_callin, conf_ppp_refuse_callin_eol,
	 conf_ppp_refuse_eol,
	 "callin", "Refuse to authenticate on incoming call only", PRIV_CONF);
NOPREFIX(conf_ppp_refuse_noprefix, conf_ppp_refuse_callin, conf_ppp_refuse_eol);
NVGENS	(conf_ppp_refuse_nvgens, conf_ppp_refuse_noprefix,
	 ppp_command, PPP_REFUSE_CHAP);
KEYWORD (conf_ppp_refuse, conf_ppp_refuse_nvgens, conf_ppp_wait,
	 "refuse", "Refuse to authenticate using CHAP", PRIV_CONF);

/***************************************************************
 * [no] ppp chap password [num] <string>
 */
EOLS	(conf_ppp_chappw_eol, ppp_command, PPP_PASSWORD);
TEXT	(conf_ppp_chappw_pw, conf_ppp_chappw_eol, no_alt,
	 OBJ(string,1), "Default PPP authentication password");
TEST_MULTIPLE_FUNCS(conf_ppp_chappw_test, conf_ppp_chappw_pw, no_alt, NONE);
NUMBER	(conf_ppp_chappw_num, conf_ppp_chappw_pw, conf_ppp_chappw_test,
	 OBJ(int,1), 0, 7, "Encryption type (0=not yet encrypted)");
NOPREFIX(conf_ppp_chappw_noprefix, conf_ppp_chappw_num, conf_ppp_chappw_eol);
NVGENS	(conf_ppp_chappw_nvgens, conf_ppp_chappw_noprefix,
	 ppp_command, PPP_PASSWORD);
KEYWORD (conf_ppp_chappw, conf_ppp_chappw_nvgens, conf_ppp_refuse,
	 "password", "Set default CHAP password", PRIV_CONF);

/***************************************************************
 * [no] ppp chap ignoreus
 */
EOLS	(conf_ppp_ignoreus_eol, ppp_command, PPP_CHAP_IGNOREUS);
KEYWORD (conf_ppp_ignoreus, conf_ppp_ignoreus_eol, conf_ppp_chappw,
	 "ignoreus", "Ignore CHAP Challenges containing our name",
	 (PRIV_CONF|PRIV_HIDDEN));

/***************************************************************
 * [no] ppp chap hostname <string>
 */
EOLS	(conf_ppp_chapname_eol, ppp_command, PPP_CHAP_NAME);
STRING	(conf_ppp_chapname_name, conf_ppp_chapname_eol, no_alt,
	 OBJ(string,1), "Alternate CHAP hostname");
NOPREFIX(conf_ppp_chapname_noprefix, conf_ppp_chapname_name,
	 conf_ppp_chapname_eol);
NVGENS	(conf_ppp_chapname_nvgens, conf_ppp_chapname_noprefix,
	 ppp_command, PPP_CHAP_NAME);
KEYWORD	(conf_ppp_chapname, conf_ppp_chapname_nvgens, conf_ppp_ignoreus,
	 "hostname", "Set alternate CHAP hostname", PRIV_CONF);
KEYWORD	(conf_ppp_chap, conf_ppp_chapname, conf_ppp_dir,
	 "chap", "Set CHAP authentication parameters", PRIV_CONF);

/***************************************************************
 * [no] ppp use-tacacs
 */
EOLS	(conf_ppp_tacacs_eol, ppp_command, PPP_USE_TACACS);

KEYWORD_ID(conf_ppp_tacacs_sl_pap, conf_ppp_tacacs_eol, conf_ppp_tacacs_eol,
	   OBJ(int,1), TRUE,
	   "single-line", "single-line has no effect when using PAP.",
	   PRIV_CONF|PRIV_HIDDEN);
KEYWORD_ID(conf_ppp_tacacs_sl_chap, conf_ppp_tacacs_eol, conf_ppp_tacacs_eol,
	   OBJ(int,1), TRUE,
	   "single-line", "Accept username and password in the username field.",
	   PRIV_CONF);
IFELSE (conf_ppp_tacacs_sl, conf_ppp_tacacs_sl_chap, conf_ppp_tacacs_sl_pap,
	(csb->interface->hwptr->lcpfsm &&
	 csb->interface->hwptr->lcpfsm->wantoptions &&
	 auth_method_supported(csb->interface->hwptr->lcpfsm, PPP_AUTH_CHAP)));
NVGENS(conf_ppp_tacacs_nvgen, conf_ppp_tacacs_sl, ppp_command, PPP_USE_TACACS);
KEYWORD (conf_ppp_tacacs_kwd, conf_ppp_tacacs_nvgen, no_alt,
	 "use-tacacs", "Use TACACS to verify PPP Authentications",
	 PRIV_CONF);
TEST_BOOLEAN(conf_ppp_tacacs, conf_ppp_tacacs_kwd, no_alt, conf_ppp_chap,
	     old_access_control);

EOLS	(conf_ppp_auth_eol, ppp_command, PPP_AUTHENTICATION );
/***************************************************************
 * [no] ppp authentication < chap | pap > [ if-needed ] [ <list> ] [ one-time ]
 *	where <list> is: `default' or a name.
 * ONLY SEEN IF old_access_control == FALSE
 *
 * Note: this parse chain is ugly. One-time appears twice as a node
 * in the parse tree below because <list> is optional, so one-time can
 * appear after <list> or in place of it, as follows:
 *
 * ... default <cr>
 * ... default one-time <cr>
 * ... one-time <cr>
 * ... <name> <cr>
 * ... <name> one-time <cr>
 *
 */
KEYWORD_ID (conf_ppp_auth_onetime, conf_ppp_auth_eol, conf_ppp_auth_eol, 
	    OBJ(int,4), 1,
	    "one-time", "Allow use of username*OTP for one-time passwords", 
	    PRIV_CONF);
SET	(conf_ppp_auth_list_set, conf_ppp_auth_onetime, OBJ(int,2), 2);
STRING	(conf_ppp_auth_list_name, conf_ppp_auth_list_set, conf_ppp_auth_eol,
	 OBJ(string,1), "Use an authentication list with this name.");
TEST_MULTIPLE_FUNCS (conf_ppp_auth_list_term, conf_ppp_auth_list_name,
		     NONE, no_alt);
KEYWORD_ID (conf_ppp_auth_list_onetime, conf_ppp_auth_eol, 
	    conf_ppp_auth_list_term, 
	    OBJ(int,4), 1,
	    "one-time", "Allow use of username*OTP for one-time passwords", 
	    PRIV_CONF);
KEYWORD_ID (conf_ppp_auth_list_kwd, conf_ppp_auth_onetime, 
	    conf_ppp_auth_list_onetime,
	    OBJ(int,2), 1,
	    "default", "Use the default authentication list.", PRIV_CONF);
TEST_BOOLEAN(conf_ppp_auth_list, conf_ppp_auth_eol, conf_ppp_auth_list_kwd,
	     NONE, old_access_control);
/***************************************************************
 * [no] ppp authentication < chap | pap > [ if-needed ]
 * ONLY SEEN IF old_access_control == TRUE
 */
KEYWORD_ID(conf_ppp_auth_ifneed_kwd, conf_ppp_auth_eol, no_alt,
	   OBJ(int,2), PPP_AUTH_IFNEED,
	   "if-needed",
	   "Don't do PAP or CHAP if user has already authenticated.",
	   PRIV_CONF);
ASSERT	(conf_ppp_auth_ifneed_ass, conf_ppp_auth_ifneed_kwd, conf_ppp_auth_eol,
	 (csb->interface->hwptr->type == IDBTYPE_ASYNC) ||
	 (csb->interface->hwptr->type == IDBTYPE_ASYNC_GROUP));
TEST_BOOLEAN(conf_ppp_auth_ifneed, conf_ppp_auth_ifneed_ass, conf_ppp_auth_list,
	     NONE, old_access_control);

/***************************************************************
 * [no] ppp authentication < chap | pap > [callin]
 */
KEYWORD_ID(conf_ppp_auth_callin, conf_ppp_auth_ifneed, conf_ppp_auth_ifneed,
	   OBJ(int, 3), TRUE,
	   "callin", "Authenticate remote on incoming call only", PRIV_CONF);
SET (conf_ppp_auth_callin_def, conf_ppp_auth_callin, OBJ(int,3), FALSE);
/***************************************************************
 * [no] ppp authentication chap
 *
 * The funny looking TESTVAR below is to prevent a no-alt from being seen on
 * the second (or later) pass by the authentication method keywords.  Otherwise,
 * the TEST_MULTIPLE_FUNCS above will prevent the 'string' from being an
 * option with the on-line help.  Even though the string will not be displayed
 * as an option, it would still be parsed correctly on input.
 */
pdecl(conf_ppp_auth_opts);
TESTVAR(conf_ppp_auth_loop_test, no_alt, NONE, NONE, NONE, NONE, OBJ(int,9), 0);
EVAL(conf_ppp_auth_chap_eval, conf_ppp_auth_opts,
     csb->uiarray[GETOBJ(int, 9) ++] = PPP_AUTH_CHAP);
KEYWORD_ORTEST(conf_ppp_auth_chap, conf_ppp_auth_chap_eval,
	       conf_ppp_auth_loop_test,
               OBJ(int,1), PPP_AUTH_CHAP_MASK,
               "chap", "Challenge Handshake Access Protocol (CHAP)", PRIV_CONF);
/***************************************************************
 * [no] ppp authentication pap
 */
EVAL(conf_ppp_auth_pap_eval, conf_ppp_auth_opts,
     csb->uiarray[GETOBJ(int, 9) ++] = PPP_AUTH_PAP);
KEYWORD_ORTEST(conf_ppp_auth_pap, conf_ppp_auth_pap_eval, conf_ppp_auth_chap,
               OBJ(int,1), PPP_AUTH_PAP_MASK,
               "pap", "Password Authentication Protocol (PAP)", PRIV_CONF);
NOP (conf_ppp_auth_opts, conf_ppp_auth_pap, conf_ppp_auth_callin_def);
NOPREFIX(conf_ppp_auth_noprefix, conf_ppp_auth_pap, conf_ppp_auth_eol);
NVGENS  (conf_ppp_auth_nvgens, conf_ppp_auth_noprefix, ppp_command,
         PPP_AUTHENTICATION);
KEYWORD(conf_ppp_auth, conf_ppp_auth_nvgens, conf_ppp_tacacs,
	"authentication", "Set PPP link Authentication method", PRIV_CONF );

/*****************************************************************
 * ppp bridge ip
 * ppp bridge ipx [novell-ether | arpa | snap | sap]
 * ppp bridge appletalk
 */
EOLS  (conf_ppp_bridge_eol, ppp_command, PPP_BRIDGE);
KEYWORD_ID(conf_ppp_bridge_ipx_sap, conf_ppp_bridge_eol, conf_ppp_bridge_eol,
	   OBJ(int,2), ET_SAP,
	   "sap", "IEEE 802.2 on bridged ipx traffic",
	   PRIV_CONF|PRIV_SUBIF);
KEYWORD_ID(conf_ppp_bridge_ipx_snap, conf_ppp_bridge_eol,
	   conf_ppp_bridge_ipx_sap, OBJ(int,2), ET_SNAP,
	   "snap", "IEEE 802.2 SNAP on bridged ipx traffic",
	   PRIV_CONF|PRIV_SUBIF);
KEYWORD_ID(conf_ppp_bridge_ipx_arpa, conf_ppp_bridge_eol,
	   conf_ppp_bridge_ipx_snap, OBJ(int,2), ET_ARPA,
	   "arpa", "Novell Ethernet_II", PRIV_CONF|PRIV_SUBIF);
KEYWORD_ID(conf_ppp_bridge_ipx_ether, conf_ppp_bridge_eol,
	   conf_ppp_bridge_ipx_arpa, OBJ(int,2), ET_NOVELL_ETHER,
	   "novell-ether", "Novell Ethernet_802.3", PRIV_CONF|PRIV_SUBIF);
SET       (conf_ppp_bridge_ipx_set, conf_ppp_bridge_ipx_ether, OBJ(int,2),
	   ET_NOVELL_ETHER);
KEYWORD_ID_MM(conf_ppp_bridge_ipx, conf_ppp_bridge_ipx_set,
	      no_alt, OBJ(int,1), LINK_NOVELL,
	      "ipx", "Bridge IPX traffic", PRIV_CONF, 3 );
KEYWORD_ID_MM(conf_ppp_bridge_ip, conf_ppp_bridge_eol,
	      conf_ppp_bridge_ipx, OBJ(int,1), LINK_IP,
	      "ip", "Bridge IP traffic", PRIV_CONF, 2 );
KEYWORD_ID_MM(conf_ppp_bridge_atalk, conf_ppp_bridge_eol,
	      conf_ppp_bridge_ip, OBJ(int,1), LINK_APPLETALK,
	      "appletalk", "Bridge appletalk traffic", PRIV_CONF, 3 );
NVGENS  (conf_ppp_bridge_nvgens, conf_ppp_bridge_atalk, ppp_command,
	 PPP_BRIDGE);
KEYWORD (conf_ppp_bridge, conf_ppp_bridge_nvgens, conf_ppp_auth,
	 "bridge", "Enable ppp bridge translation", PRIV_CONF);

/***************************************************************
 * [no] ppp callback < accept | request  >
 */
EOLS  (conf_ppp_callback_sync_eol, ppp_command, PPP_CALLBACK);
KEYWORD_ID(conf_ppp_callback_sync_request, conf_ppp_callback_sync_eol, 
    no_alt, OBJ(int,1), PPP_CALLBACK_REQUEST,
    "request", "Request a callback", PRIV_CONF );
KEYWORD_ID(conf_ppp_callback_sync_accept, conf_ppp_callback_sync_eol, 
    conf_ppp_callback_sync_request, OBJ(int,1), PPP_CALLBACK_ACCEPT,
    "accept", "Accept a callback request", PRIV_CONF );
NOPREFIX(conf_ppp_callback_sync_noprefix, conf_ppp_callback_sync_accept, 
    conf_ppp_callback_sync_eol);
NVGENS  (conf_ppp_callback_sync_nvgens, conf_ppp_callback_sync_noprefix, ppp_command,
    PPP_CALLBACK);
KEYWORD(conf_ppp_callback_sync, conf_ppp_callback_sync_nvgens, NONE,
    "callback", "Set PPP link callback option", PRIV_CONF );
ASSERT (conf_ppp_callback_nonasync, conf_ppp_callback_sync,
	conf_ppp_bridge, (if_is_ddr(csb->interface->hwptr))); 

/***************************************************************
 * [no] ppp callback < accept | request | initiate >
 */
EOLS  (conf_ppp_callback_eol, ppp_command, PPP_CALLBACK);
KEYWORD_ID(conf_ppp_callback_initiate, conf_ppp_callback_eol, 
    no_alt, OBJ(int,1), PPP_CALLBACK_INITIATE,
    "initiate", "Initiate callback without PPP callback negotiation", 
    PRIV_CONF );
KEYWORD_ID(conf_ppp_callback_request, conf_ppp_callback_eol, 
    conf_ppp_callback_initiate, OBJ(int,1), PPP_CALLBACK_REQUEST,
    "request", "Request a callback", PRIV_CONF|PRIV_HIDDEN );
KEYWORD_ID(conf_ppp_callback_accept, conf_ppp_callback_eol, 
    conf_ppp_callback_request, OBJ(int,1), PPP_CALLBACK_ACCEPT,
    "accept", "Accept a callback request", PRIV_CONF );
NOPREFIX(conf_ppp_callback_noprefix, conf_ppp_callback_accept, 
    conf_ppp_callback_eol);
NVGENS  (conf_ppp_callback_nvgens, conf_ppp_callback_noprefix, ppp_command,
    PPP_CALLBACK);
KEYWORD(conf_ppp_callback, conf_ppp_callback_nvgens, NONE,
    "callback", "Set PPP link callback option", PRIV_CONF );
ASSERT (conf_ppp_callback_async, conf_ppp_callback, conf_ppp_callback_nonasync,
    (((csb->interface->hwptr->type == IDBTYPE_ASYNC) ||
      (csb->interface->hwptr->type == IDBTYPE_ASYNC_GROUP)) &&
     !if_is_ddr(csb->interface->hwptr)));

/***************************************************************
 *[no] ppp compression predictor
 *
 */
EOLS(conf_ppp_comp_eol, ppp_ncp_command, PPP_COMPRESSION);
KEYWORD_ID(conf_ppp_comp_pred, conf_ppp_comp_eol, no_alt,
	   OBJ(int,1), PPP_COMP_PREDICTOR,
	   "predictor", "Predictor Compression", PRIV_CONF);

/***************************************************************
 *[no] ppp compression stacker
 *
 */
KEYWORD_ID(conf_ppp_comp_stack, conf_ppp_comp_eol, conf_ppp_comp_pred,
	   OBJ(int,1), PPP_COMP_STACKER,
	   "stacker", "Stacker (LZS) Compression", PRIV_CONF);
NOPREFIX(conf_ppp_comp_noprefix, conf_ppp_comp_stack, conf_ppp_comp_eol);
NVGENS(conf_ppp_comp_nvgens, conf_ppp_comp_noprefix, ppp_ncp_command,
	PPP_COMPRESSION);
KEYWORD(conf_comp_control, conf_ppp_comp_nvgens, conf_ppp_callback_async,
	"compression", "Enable PPP Compression control negotiation",
	PRIV_CONF );

/***************************************************************
 * [no] ppp reliable-link
 * 
 */

EOLS(conf_ppp_reliable_link_eol, ppp_command, PPP_RELIABLE_LINK);
KEYWORD(conf_ppp_reliable_link, conf_ppp_reliable_link_eol,
           conf_comp_control, "reliable-link",
          "Use LAPB with PPP to provide a reliable link", PRIV_CONF);

 
/***************************************************************
 * ppp negotiation-timeout <num>
 * no ppp negotiation-timeout
 */
PARAMS_KEYONLY(conf_ppp_negtimeout, conf_ppp_reliable_link,
	       "negotiation-timeout", OBJ(int,1), 1, -1,
	       ppp_command, PPP_NEG_TIMEOUT,
	       "Negotiation timeout period",
	       "Seconds", PRIV_CONF);

/***************************************************************
 * ppp max-bad-auth <num>
 * no ppp max-bad-auth
 */
PARAMS_KEYONLY(conf_ppp_maxbadauth, conf_ppp_negtimeout,
	       "max-bad-auth", OBJ(int,1), 0, -1,
	       ppp_command, PPP_MAX_BAD_AUTHS,
	       "",
	       "", PRIV_CONF);

/***************************************************************
 * ppp restart-timer <num>
 * no ppp restart-timer
 */
PARAMS_KEYONLY(conf_ppp_restart, conf_ppp_maxbadauth,
	       "restart-timer", OBJ(int,1), 0, -1,
	       ppp_command, PPP_RESTART_TIMER,
	       "",
	       "", PRIV_CONF | PRIV_HIDDEN);

/***************************************************************
 * ppp max-failure <num>
 * no ppp max-failure
 */
PARAMS_KEYONLY(conf_ppp_maxfail, conf_ppp_restart, 
	       "max-failure", OBJ(int,1), 0, -1,
	       ppp_command, PPP_MAX_FAILURE,
	       "",
	       "", PRIV_CONF | PRIV_HIDDEN);

/***************************************************************
 * ppp max-configure <num>
 * no ppp max-configure
 */
PARAMS_KEYONLY(conf_ppp_maxconf, conf_ppp_maxfail,
	       "max-configure", OBJ(int,1), 0, -1,
	       ppp_command, PPP_MAX_CONFIGURE,
	       "",
	       "", PRIV_CONF | PRIV_HIDDEN);

/***************************************************************
 * ppp max-terminate <num>
 * no ppp max-terminate
 */
PARAMS_KEYONLY(conf_ppp_maxterm, conf_ppp_maxconf, 
	       "max-terminate", OBJ(int,1), 0, -1,
	       ppp_command, PPP_MAX_TERMINATE,
	       "",
	       "", PRIV_CONF | PRIV_HIDDEN);

/***************************************************************
 * ppp quality <percentage>
 * no ppp quality
 */
PARAMS_KEYONLY(conf_ppp_quality, conf_ppp_maxterm, 
	       "quality", OBJ(int,1), 0, 100,
	       ppp_command, PPP_LINK_QUALITY,
	       "",
	       "", PRIV_CONF|PRIV_HIDDEN);

KEYWORD	(conf_ppp, conf_ppp_quality, NONE, 
	 "ppp", "Point-to-point protocol", PRIV_CONF);

TEST_IDBSTATUS(ci_ppp_int, conf_ppp, NONE, ALTERNATE, IDB_SERIAL);

#undef	ALTERNATE
#define	ALTERNATE	ci_ppp_int
