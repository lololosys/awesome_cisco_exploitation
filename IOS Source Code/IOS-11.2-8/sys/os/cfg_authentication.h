/* $Id: cfg_authentication.h,v 3.4.22.3 1996/06/26 22:44:12 billw Exp $
 * $Source: /release/112/cvs/Xsys/os/cfg_authentication.h,v $
 *------------------------------------------------------------------
 * cfg_authentication.h -- Parser commands for authentication cmds
 *
 * September 1994, Dave Carrel
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cfg_authentication.h,v $
 * Revision 3.4.22.3  1996/06/26  22:44:12  billw
 * CSCdi59376:  AAA local prompts not configurable
 * Branch: California_branch
 *
 * Revision 3.4.22.2  1996/05/07  18:06:04  snyder
 * CSCdi56844:  spellink errors
 * Branch: California_branch
 *
 * privilage -> privilege
 *
 * Revision 3.4.22.1  1996/04/19  15:24:28  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commit
 *
 * Revision 3.1.2.4  1996/04/16  03:55:49  che
 * Branch: IosSec_branch
 * Code changes mandated by code review comments.
 *
 * Revision 3.1.2.3  1996/03/05  04:43:56  che
 * Branch: IosSec_branch
 * Kerberos Phase 2 commit.
 *
 * Revision 3.1.2.2  1996/02/21  03:48:13  che
 * Branch: IosSec_branch
 * Sync to 11.1 Mainline (02/20/96)
 *
 * Revision 3.1.2.1  1995/12/17  02:11:46  che
 * Branch: IosSec_branch
 * Sync to Arkansas
 *
 * Revision 3.4  1996/01/24  05:43:42  fox
 * CSCdi47394:  connection keyword in aaa accounting/authorization is
 * hidden
 * Disable keyword 'connection' for 'aaa authorization'
 * Unhide keyword 'connection' for 'aaa accounting'
 *
 * Revision 3.3  1995/12/07  17:45:21  widmer
 * CSCdi45134:  Replace occurences of Ifelse with appropriate Test* macros
 *
 * Revision 3.2  1995/11/17  18:45:03  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:44:06  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/11/08  21:18:27  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.2  1995/07/26  14:14:55  billw
 * CSCdi37036:  No AAA Accounting records generated for system events
 * generate "system" records for startup, shutdown, and change in the
 * state of "aaa accounting system" configuration.
 *
 * Revision 2.1  1995/07/02  10:50:55  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize os, hes, les, if, and ibm.
 *
 * Revision 2.1  1995/06/07  22:01:45  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * [no] aaa new-model
 * [no] aaa authentication ...
 * [no] aaa authorization ...
 * [no] aaa accounting ...
 */

/*
 * [no] aaa authorization net|exec tacacs+|radius||if-authenticated|local|none
 * [no] aaa authorization connection tacacs+|if-authenticated|local|none
 * [no] aaa authorization command # tacacs+|if-authenticated|local|none
 *
 * [no] aaa accounting net|connection|exec|system tacacs+|radius
 * [no] aaa accounting command # tacacs+
 *
 * Note:  connection is currently not a valid keyword for aaa authorization
 */

/*
 * The aaa commands that build a method list end up using the parser in
 * a slightly strange way.  Each integer (ie GETOBJ(int,10)) in the csb
 * reflects a particular method, and the value specifies the position that
 * this method occupied in the overall list being formed.
 */

pdecl	(conf_author_method_opts);
EOLS   	(conf_author_eol, aaa_command, PARSER_AUTHOR_LIST);

EVAL	(conf_author_method_none_eval, conf_author_method_opts,
	 PARSED_METH_NONE = count++);
KEYWORD	(conf_author_method_none_kwd, conf_author_method_none_eval, NONE,
	 "none", "No authorization (always succeeds).", PRIV_CONF);
ASSERT	(conf_author_method_none, conf_author_method_none_kwd, NONE,
	 (!PARSED_METH_NONE && (GETOBJ(int,1) == PARSER_AUTHOR_LIST)));

EVAL	(conf_author_method_krb5_instance_eval, conf_author_method_opts,
	 PARSED_METH_KRB5 = count++);
KEYWORD	(conf_author_method_krb5_instance_kwd, 
	 conf_author_method_krb5_instance_eval, NONE,
	 "krb5-instance", "Use Kerberos instance privilege maps.", PRIV_CONF);
ASSERT	(conf_author_method_krb5_instance, conf_author_method_krb5_instance_kwd,
	 conf_author_method_none, (!PARSED_METH_KRB5 && kerberos5_is_installed 
	  && (GETOBJ(int,1) == PARSER_AUTHOR_LIST) 
	  && (GETOBJ(int,2) == AAA_AUTHOR_TYPE_SHELL)));

EVAL	(conf_author_method_local_eval, conf_author_method_opts,
	 PARSED_METH_LOCAL = count++);
KEYWORD	(conf_author_method_local_kwd, conf_author_method_local_eval, NONE,
	 "local", "Use local database.", PRIV_CONF);
ASSERT	(conf_author_method_local, conf_author_method_local_kwd,
	 conf_author_method_krb5_instance,
	 (!PARSED_METH_LOCAL && (GETOBJ(int,1) == PARSER_AUTHOR_LIST)));

EVAL	(conf_author_method_ifauthen_eval, conf_author_method_opts,
	 PARSED_METH_IFNEEDED = count++);
KEYWORD	(conf_author_method_ifauthen_kwd, conf_author_method_ifauthen_eval,
	 NONE, "if-authenticated", "Succeed if user has authenticated.",
	 PRIV_CONF);
ASSERT	(conf_author_method_ifauthen, conf_author_method_ifauthen_kwd,
	 conf_author_method_local,
	 (!PARSED_METH_IFNEEDED && (GETOBJ(int,1) == PARSER_AUTHOR_LIST)));

EVAL	(conf_author_method_radius_eval, conf_author_method_opts,
	 PARSED_METH_RADIUS = count++);
KEYWORD	(conf_author_method_radius_kwd, conf_author_method_radius_eval, NONE,
	 "radius", "Use RADIUS data for authorization", PRIV_CONF);
ASSERT	(conf_author_method_radius, conf_author_method_radius_kwd,
	 conf_author_method_ifauthen,
	 (!PARSED_METH_RADIUS && radius_is_installed));

EVAL	(conf_author_method_tacacs_eval, conf_author_method_opts,
	 PARSED_METH_TACPLUS = count++);
KEYWORD	(conf_author_method_tacacs_kwd, conf_author_method_tacacs_eval, NONE,
	 "tacacs+", "Use TACACS+.", PRIV_CONF);
ASSERT	(conf_author_method_tacacs, conf_author_method_tacacs_kwd,
	 conf_author_method_radius,
	 (!PARSED_METH_TACPLUS && tacacsplus_is_installed));

ASSERT	(conf_author_list_end, conf_author_eol, NONE, count > 1);
TESTVAR	(conf_author_method_opts, conf_author_method_tacacs, NONE, NONE, NONE,
         conf_author_list_end, OBJ(int,10), 0);
EVAL	(conf_author_list, conf_author_method_opts, count = 1);

/* FOR ACCOUNTING ONLY -- find out if start, stop and waiting are required */
KEYWORD_ID(conf_author_stop_only, conf_author_list, no_alt,
	   OBJ(int,4), AAA_ACCT_ACT_STOP_ONLY,
	   "stop-only", "Record stop when service terminates.", PRIV_CONF);
KEYWORD_ID(conf_author_start_wait, conf_author_list, conf_author_stop_only,
	   OBJ(int,4), AAA_ACCT_ACT_WAIT_START,
	   "wait-start",
	   "Same as start-stop but wait for start-record commit.", PRIV_CONF);
KEYWORD_ID(conf_author_start_stop, conf_author_list, conf_author_start_wait,
	   OBJ(int,4), AAA_ACCT_ACT_START_STOP,
	   "start-stop", "Record start and stop without waiting", PRIV_CONF);
TESTVAR	(conf_author_acct, conf_author_start_stop, conf_author_list,
	 NONE, NONE, NONE, OBJ(int,1), PARSER_ACCT_LIST);

NOPREFIX(conf_author_nopre, conf_author_acct, conf_author_eol);
NUMBER	(conf_author_cmd_lvl, conf_author_nopre, no_alt,
	 OBJ(int,3), PRIV_MIN, PRIV_MAX, "Enable level");
TESTVAR	(conf_author_cmd_lvl_ifelse, conf_author_cmd_lvl,
	 conf_author_nopre, NONE, NONE, NONE, OBJ(int,2), AAA_AUTHOR_TYPE_CMD);
NVGENS	(conf_author_nvgen, conf_author_cmd_lvl_ifelse,
	 aaa_command, PARSER_AUTHOR_LIST);

/*
 * [no] aaa accounting nested
 */

EOLS    (conf_acct_nested_eol, aaa_command, PARSER_ACCT_NESTED);
KEYWORD (conf_acct_nested_kwd, conf_acct_nested_eol, NONE, "nested", 
	 "When starting PPP from EXEC, generate NETWORK records before EXEC-STOP record.",
	 PRIV_CONF);

/*
 * [no] aaa accounting suppress null-username
 */

EOLS    (conf_acct_nullu_eol, aaa_command, PARSER_ACCT_SUPPRESS_NULL_USER);
KEYWORD (conf_acct_nullu_kwd, conf_acct_nullu_eol, NONE, "null-username", 
	 "Do not generate accounting records for users with a null-username",
	 PRIV_CONF);
KEYWORD (conf_acct_suppress_kwd, conf_acct_nullu_kwd, conf_acct_nested_kwd, 
	 "suppress", 
	 "Do not generate accounting records for a specific type of user.",
	 PRIV_CONF);

KEYWORD_ID(conf_author_sys_kwd, conf_author_nvgen, conf_acct_suppress_kwd,
	   OBJ(int,2), AAA_AUTHOR_TYPE_SYSTEM,
	   "system", "For System events.", PRIV_CONF);
TESTVAR	(conf_author_sys, conf_author_sys_kwd, NONE, NONE, NONE, no_alt,
	 OBJ(int,1), PARSER_ACCT_LIST);

/*
 * KEYWORD_ID(conf_author_conn, conf_author_nvgen, conf_author_sys,
 *	   OBJ(int,2), AAA_AUTHOR_TYPE_CONN,
 *	   "connection", "For outbound connections. (telnet, rlogin)",
 *	   PRIV_CONF);
 */

KEYWORD_ID(conf_acct_conn, conf_author_nvgen, conf_author_sys,
	   OBJ(int,2), AAA_AUTHOR_TYPE_CONN,
	   "connection", "For outbound connections. (telnet, rlogin)",
	   PRIV_CONF);

TESTVAR	(conf_author_conn, conf_acct_conn, conf_author_sys,
	 NONE, NONE, NONE, OBJ(int,1), PARSER_ACCT_LIST);

/*
 * connection is currently not valid for aaa authorization
 *  When this support is finally added, remove everything from this comment
 *  to the next comment above and uncomment out the original KEYWORD_ID
 *  macro above.
 */

KEYWORD_ID(conf_author_net, conf_author_nvgen, conf_author_conn,
	   OBJ(int,2), AAA_AUTHOR_TYPE_NET,
	   "network", "For network services. (PPP, SLIP, ARAP)", PRIV_CONF);

KEYWORD_ID(conf_author_conf_command, conf_author_eol, conf_author_net,
	   OBJ(int,2), PARSER_AUTHOR_CONFIG_CMD,
	   "config-commands", "For configuration mode commands.", PRIV_CONF);
	  
TESTVAR(conf_author_config, conf_author_conf_command, conf_author_net,
	NONE, NONE, NONE, OBJ(int,1), PARSER_AUTHOR_LIST);
/*
 * config-commands is not valid for aaa accounting
 */

KEYWORD_ID(conf_author_cmd, conf_author_nvgen, conf_author_config,
	   OBJ(int,2), AAA_AUTHOR_TYPE_CMD,
	   "commands", "For exec (shell) commands.", PRIV_CONF);

KEYWORD_ID(conf_author_shell, conf_author_nvgen, conf_author_cmd,
	   OBJ(int,2), AAA_AUTHOR_TYPE_SHELL,
	   "exec", "For starting an exec (shell).", PRIV_CONF);

/* Both authorization and accounting walk the same tree above. */
KEYWORD_ID(conf_acct, conf_author_shell, no_alt,
	   OBJ(int,1), PARSER_ACCT_LIST,
	   "accounting", "Accounting configurations parameters.", PRIV_CONF);
KEYWORD_ID(conf_author, conf_author_shell, conf_acct,
	   OBJ(int,1), PARSER_AUTHOR_LIST,
	   "authorization", "Authorization configurations parameters.",
	   PRIV_CONF);

/*
 * [no] aaa authentication <login|enable|ppp|arap> default|<name> <list>
 * 	Where <list> is 1, 2, 3 or 4 of:
 *	For ppp:	if-needed|krb5|radius|tacacs+|local|none
 *	For arap:	{guest|auth-guest}|tacacs+|local|line
 *	For login:	{krb5-telnet}|krb5|tacacs+|radius|local|line|enable|none
 *	For enable:	krb5|tacacs|local|line|enable|none
 */
pdecl	(conf_authen_method_opts);
EOLS	(conf_authen_list_eol, aaa_command, PARSER_AUTHEN_LISTS);

EVAL	(conf_authen_method_krb5_telnet_eval, conf_authen_method_opts,
	 PARSED_METH_KRB5_TELNET = count++);
KEYWORD	(conf_authen_method_krb5_telnet_kwd, 
	 conf_authen_method_krb5_telnet_eval, NONE, "krb5-telnet",
	 "Allow logins only if already authenticated via Kerberos V Telnet.",
	 PRIV_CONF);
ASSERT	(conf_authen_method_krb5_telnet, conf_authen_method_krb5_telnet_kwd, 
	 NONE, ((count == 1) && (GETOBJ(int,1) == AAA_SVC_LOGIN) && !PARSED_METH_KRB5_TELNET));

EVAL	(conf_authen_method_authguest_eval, conf_authen_method_opts,
	 PARSED_METH_AUTHGUEST = count++);
KEYWORD	(conf_authen_method_authguest_kwd, conf_authen_method_authguest_eval,
	 NONE, "auth-guest",
	 "Allow guest logins only if already authenticated to exec.",
	 PRIV_CONF);
ASSERT	(conf_authen_method_authguest, conf_authen_method_authguest_kwd, 
	 conf_authen_method_krb5_telnet, ((count == 1) && (GETOBJ(int,1) == AAA_SVC_ARAP) && !PARSED_METH_AUTHGUEST));

EVAL	(conf_authen_method_guest_eval, conf_authen_method_opts,
	 PARSED_METH_GUEST = count++);
KEYWORD	(conf_authen_method_guest_kwd, conf_authen_method_guest_eval, NONE,
	 "guest", "Allow guest logins.", PRIV_CONF);
ASSERT	(conf_authen_method_guest, conf_authen_method_guest_kwd,
	 conf_authen_method_authguest,
	 ((count == 1) && (GETOBJ(int,1) == AAA_SVC_ARAP) && !PARSED_METH_GUEST));

EVAL	(conf_authen_method_ifneed_eval, conf_authen_method_opts,
	 PARSED_METH_IFNEEDED = count++);
KEYWORD	(conf_authen_method_ifneed_kwd, conf_authen_method_ifneed_eval, NONE,
	 "if-needed", "Only authenticate if needed.", PRIV_CONF);
ASSERT	(conf_authen_method_ifneed, conf_authen_method_ifneed_kwd,
	 conf_authen_method_guest,
	 ((count == 1) && (GETOBJ(int,1) == AAA_SVC_PPP) && !PARSED_METH_IFNEEDED));

EVAL	(conf_authen_method_none_eval, conf_authen_method_opts,
	 PARSED_METH_NONE = count++);
KEYWORD	(conf_authen_method_none_kwd, conf_authen_method_none_eval, NONE,
	 "none", "NO authentication.", PRIV_CONF);
ASSERT	(conf_authen_method_none, conf_authen_method_none_kwd,
	 conf_authen_method_ifneed,
	 ((GETOBJ(int,1) != AAA_SVC_ARAP) && (!PARSED_METH_NONE)));

EVAL	(conf_authen_method_enable_eval, conf_authen_method_opts,
	 PARSED_METH_ENABLE = count++);
KEYWORD	(conf_authen_method_enable_kwd, conf_authen_method_enable_eval, NONE,
	 "enable", "Use enable password for authentication.", PRIV_CONF);
ASSERT	(conf_authen_method_enable, conf_authen_method_enable_kwd,
	 conf_authen_method_none, (!PARSED_METH_ENABLE &&
				   (GETOBJ(int,1) != AAA_SVC_ARAP) &&
				   (GETOBJ(int,1) != AAA_SVC_PPP)));

EVAL	(conf_authen_method_line_eval, conf_authen_method_opts,
	 PARSED_METH_LINE = count++);
KEYWORD	(conf_authen_method_line_kwd, conf_authen_method_line_eval, NONE,
	 "line", "Use line password for authentication.", PRIV_CONF);
ASSERT	(conf_authen_method_line, conf_authen_method_line_kwd,
	 conf_authen_method_enable,
	 ((GETOBJ(int,1) != AAA_SVC_PPP) && (!PARSED_METH_LINE)));

EVAL	(conf_authen_method_local_eval, conf_authen_method_opts,
	 PARSED_METH_LOCAL = count++);
KEYWORD	(conf_authen_method_local_kwd, conf_authen_method_local_eval, NONE,
	 "local", "Use local username authentication.", PRIV_CONF);
ASSERT	(conf_authen_method_local, conf_authen_method_local_kwd,
	 conf_authen_method_line,
	 ((GETOBJ(int,1) != AAA_SVC_ENABLE) && (!PARSED_METH_LOCAL)));

EVAL	(conf_authen_method_radius_eval, conf_authen_method_opts,
	 PARSED_METH_RADIUS = count++);
KEYWORD	(conf_authen_method_radius_kwd, conf_authen_method_radius_eval, NONE,
	 "radius", "Use RADIUS authentication.", PRIV_CONF);
ASSERT	(conf_authen_method_radius, conf_authen_method_radius_kwd,
	 conf_authen_method_local,
	 ((GETOBJ(int,1) != AAA_SVC_ARAP) && (!PARSED_METH_RADIUS) &&
	  radius_is_installed));

EVAL	(conf_authen_method_tacacs_eval, conf_authen_method_opts,
	 PARSED_METH_TACPLUS = count++);
KEYWORD	(conf_authen_method_tacacs_kwd, conf_authen_method_tacacs_eval, NONE,
	 "tacacs+", "Use TACACS+ authentication.", PRIV_CONF);
ASSERT	(conf_authen_method_tacacs, conf_authen_method_tacacs_kwd,
	 conf_authen_method_radius,
	 (!PARSED_METH_TACPLUS && tacacsplus_is_installed));

EVAL	(conf_authen_method_krb5_eval, conf_authen_method_opts,
	 PARSED_METH_KRB5 = count++);
KEYWORD	(conf_authen_method_krb5_kwd, conf_authen_method_krb5_eval, NONE,
	 "krb5", "Use Kerberos 5 authentication.", PRIV_CONF);
ASSERT	(conf_authen_method_krb5, conf_authen_method_krb5_kwd,
	 conf_authen_method_tacacs,
	 ((GETOBJ(int,1) != AAA_SVC_ARAP) && (GETOBJ(int,1) != AAA_SVC_ENABLE)
	 && (!PARSED_METH_KRB5) && kerberos5_is_installed));

/*
 * noprefix stuff and bookkeeping.
 */
IFELSE	(conf_authen_list_end, conf_authen_list_eol, NONE, count > 1);
ASSERT	(conf_authen_method_opts, conf_authen_method_krb5,
	 conf_authen_list_end, ((count <= MAX_ACC_METHS) && !PARSED_METH_NONE &&
				!PARSED_METH_LOCAL));
EVAL	(conf_authen_list_reset, conf_authen_method_opts, count = 1);
NOPREFIX(conf_authen_list_nopre, conf_authen_list_reset, conf_authen_list_eol);

/*
 * get "default" or a name.
 */
SET	(conf_authen_list_str_set, conf_authen_list_nopre, OBJ(int,2), 2);
STRING	(conf_authen_list_str, conf_authen_list_str_set, no_alt,
	 OBJ(string,1), "Named authentication list.");
TESTVAR	(conf_authen_list_str_assert, NONE, conf_authen_list_str, NONE, NONE,
	 NONE, OBJ(int,1), AAA_SVC_ENABLE);
TESTVAR	(conf_authen_list_def_terminate, NONE, conf_authen_list_str_assert,
	 NONE, NONE, NONE, OBJ(int,2), 1);
KEYWORD_ID (conf_authen_list_def, conf_authen_list_nopre,
	    conf_authen_list_def_terminate, OBJ(int,2), 1,
	    "default", "The default authentication list.", PRIV_CONF);
NVGENS	(conf_authen_list_nvgen, conf_authen_list_def, aaa_command,
	 PARSER_AUTHEN_LISTS);


/*
 * authentications lists for NASI.
 */
KEYWORD_ID(conf_authen_nasi, conf_authen_list_nvgen, no_alt,
	   OBJ(int,1), AAA_SVC_NASI,
	   "nasi", "Set authentication lists for NASI.", PRIV_CONF);
/*
 * authentications lists for arap.
 */
KEYWORD_ID(conf_authen_arap, conf_authen_list_nvgen, conf_authen_nasi,
	   OBJ(int,1), AAA_SVC_ARAP,
	   "arap", "Set authentication lists for arap.", PRIV_CONF);
/*
 * authentications lists for ppp.
 */
KEYWORD_ID(conf_authen_ppp, conf_authen_list_nvgen, conf_authen_arap,
	   OBJ(int,1), AAA_SVC_PPP,
	   "ppp", "Set authentication lists for ppp.", PRIV_CONF);
/*
 * authentications list for enable.
 */
KEYWORD_ID(conf_authen_enable, conf_authen_list_nvgen, conf_authen_ppp,
	   OBJ(int,1), AAA_SVC_ENABLE,
	   "enable", "Set authentication list for enable.", PRIV_CONF);
/*
 * authentications lists for logins.
 */
KEYWORD_ID(conf_authen_login, conf_authen_list_nvgen, conf_authen_enable,
	   OBJ(int,1), AAA_SVC_LOGIN,
	   "login", "Set authentication lists for logins.", PRIV_CONF);

EOLS(conf_authen_prompt_eol, aaa_command, PARSER_AUTHEN_PROMPTS);
STRING(conf_authen_promptext3, conf_authen_prompt_eol, no_alt,
	 OBJ(string,1), "Text of prompt");
NOPREFIX(conf_authen_promptext2, conf_authen_promptext3,
	 conf_authen_prompt_eol);
NVGENS	(conf_authen_promptext, conf_authen_promptext2, aaa_command,
	 PARSER_AUTHEN_PROMPTS);
KEYWORD_ID(conf_authen_uprompt, conf_authen_promptext, conf_authen_login,
	   OBJ(int,1), AAA_PROMPT_USER,
	 "username-prompt", "Text to use when prompting for a username",
	 PRIV_CONF);
KEYWORD_ID(conf_authen_pprompt, conf_authen_promptext, conf_authen_uprompt,
	   OBJ(int,1), AAA_PROMPT_PASSWORD,
	 "password-prompt", "Text to use when prompting for a password",
	 PRIV_CONF);

EOLS	(conf_authen_local_o_eol, aaa_command, PARSER_AUTHEN_LOCAL_O);
KEYWORD	(conf_authen_local_o, conf_authen_local_o_eol, conf_authen_pprompt,
	 "local-override", "Local accounts always get checked first.",
	 PRIV_CONF);

KEYWORD (conf_authen_kwd, conf_authen_local_o, conf_author, "authentication",
	 "Authentication configurations parameters.", PRIV_CONF);
TEST_BOOLEAN(conf_authen, NONE, conf_authen_kwd, no_alt, old_access_control);

EOLS	(conf_aaa_newmod_eol, aaa_command, PARSER_AUTHEN_NEW_MODEL);
KEYWORD	(conf_aaa_newmod, conf_aaa_newmod_eol, conf_authen,
	 "new-model", "Enable NEW access control commands and functions."
	 "(Disables OLD commands.)", PRIV_CONF);

KEYWORD	(conf_aaa, conf_aaa_newmod, ALTERNATE,
	 "aaa", "Authentication, Authorization and Accounting.", PRIV_CONF);

#undef	ALTERNATE
#define ALTERNATE	conf_aaa
