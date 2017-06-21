/* $Id: cfg_tacacs.h,v 3.3.20.2 1996/04/20 06:41:26 lol Exp $
 * $Source: /release/112/cvs/Xsys/servers/cfg_tacacs.h,v $
 *------------------------------------------------------------------
 * C F G _ T A C A C S . H
 *
 * Copyright (c) 1992-1997 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_tacacs.h,v $
 * Revision 3.3.20.2  1996/04/20  06:41:26  lol
 * CSCdi45246:  want tacacs-server retransmit 0 - only try once
 * Branch: California_branch
 *
 * Revision 3.3.20.1  1996/04/19  15:29:18  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commit
 *
 * Revision 3.1.2.3  1996/04/02  19:32:47  carrel
 * cvs fixes
 *
 * Revision 3.1.2.2  1996/02/28  06:14:55  lol
 * Branch: IosSec_branch
 * TACACS+ processing.
 *
 * Revision 3.1.2.1  1996/02/21  03:58:56  che
 * Branch: IosSec_branch
 * Sync to 11.1 Mainline (02/20/96)
 *
 * Revision 3.3  1996/02/01  23:34:26  widmer
 * CSCdi45325:  Change Keyword/Set to use Keyword_Id macro
 *
 * Revision 3.2  1995/11/17  18:50:43  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:11:26  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/04  01:56:43  tli
 * CSCdi36508: Rationalize parser file placement, part I
 * Rationalize cls dev dlsw dspu filesys irix lat les lex llc2 parser pgen
 * pt servers smf smrp snmp src-36-c1000 src-4k-c4000 src-68-c3000
 * src-68-c4000 src-c4000 src-rsp srt tcl ts ui
 *
 * Revision 2.1  1995/06/07  22:14:06  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * NOTE:  All tacacs-server commands between here and "tacacs_server
 *        extended" will be cut out of the parse chain if
 *        tacacs_extended is not set.
 */
/******************************************************************
 * tacacs-server notify { connections | enable | logout | slip} [always]
 * no tacacs-server notify [{ connections | enable | logout | slip } [always]]
 *
 * OBJ(int,1) = TACACS_NOTECONNECT, TACACS_NOTEENA, TACACS_NOTESLIP,
 *                 TACACS_NOTELOGOUT 
 * OBJ(int,2) = TRUE is "always" keyword parsed, FALSE otherwise.
 */
EOLS	(conf_tacacs_notify_eol, tacacs_service, TACACS_NOTE);

KEYWORD_ID (conf_tacacs_notify_always, conf_tacacs_notify_eol,
	    conf_tacacs_notify_eol,
	    OBJ(int,2), TRUE,
	    "always", "Notify even if user isn't logged in", PRIV_CONF);

NVGENS	(conf_tacacs_notify_nvcheck, conf_tacacs_notify_always,
	 tacacs_service, TACACS_NOTE); 

KEYWORD_ID (conf_tacacs_notify_slip, conf_tacacs_notify_nvcheck, no_alt,
	    OBJ(int,1), TACACS_NOTESLIP,
	    "slip", "Initiation of SLIP and PPP sessions", PRIV_CONF);

KEYWORD_ID (conf_tacacs_notify_logout, conf_tacacs_notify_nvcheck,
	    conf_tacacs_notify_slip,
	    OBJ(int,1), TACACS_NOTELOGOUT,
	    "logout", "User Logouts/quits", PRIV_CONF);

KEYWORD_ID (conf_tacacs_notify_enable, conf_tacacs_notify_eol,
	    conf_tacacs_notify_logout,
	    OBJ(int,1), TACACS_NOTEENA,
	    "enable", "Use of the 'enable' command", PRIV_CONF);

KEYWORD_ID (conf_tacacs_notify_conn, conf_tacacs_notify_nvcheck,
	    conf_tacacs_notify_enable,
	    OBJ(int,1), TACACS_NOTECONNECT,
	    "connections", "User-initiated TCP connections", PRIV_CONF);
	    
KEYWORD	(conf_tacacs_notify, conf_tacacs_notify_conn, no_alt,
	 "notify", "Notify TACACS servers of significant events", PRIV_CONF);

/******************************************************************
 * tacacs-server authenticate { connections | enable | logout | slip }
 *         [always] [access-lists]
 * no tacacs-server authenticate [{ connections | enable | logout | slip }
 *         [always] [access_lists]]
 *
 * OBJ(int,1) = TACACS_ASKCONNECT, TACACS_ASKENA, TACACS_ASKSLIP
 * OBJ(int,2) = TRUE is "always" keyword parsed, FALSE otherwise.
 * OBJ(int,3) = TRUE is "access-lists" keyword parsed, FALSE otherwise.
 */

pdecl (conf_tacacs_auth_opts);

EOLS	(conf_tacacs_auth_eol, tacacs_service, TACACS_ASK);

KEYWORD_ID (conf_tacacs_auth_acl_kwd, conf_tacacs_auth_opts, NONE,
	    OBJ(int,3), TRUE,
	    "access-lists", "Request and install SLIP & PPP access lists",
	    PRIV_CONF);
ASSERT (conf_tacacs_auth_acl, conf_tacacs_auth_acl_kwd, NONE,
	((GETOBJ(int,3) == 0) && (GETOBJ(int,1) == TACACS_ASKSLIP)));

KEYWORD_ID (conf_tacacs_auth_always_kwd, conf_tacacs_auth_opts, NONE,
	    OBJ(int,2), TRUE,
	    "always", "Authenticate even if user isn't logged in", PRIV_CONF);
TESTVAR	(conf_tacacs_auth_always, conf_tacacs_auth_always_kwd, NONE, NONE, NONE,
	 conf_tacacs_auth_acl, OBJ(int,2), 0);

NOP (conf_tacacs_auth_opts, conf_tacacs_auth_always, conf_tacacs_auth_eol);

NVGENS	(conf_tacacs_auth_nvcheck, conf_tacacs_auth_opts,
	 tacacs_service, TACACS_ASK); 

KEYWORD_ID (conf_tacacs_auth_slip, conf_tacacs_auth_nvcheck, no_alt,
	    OBJ(int,1), TACACS_ASKSLIP,
	    "slip", "Initiation of SLIP and PPP sessions", PRIV_CONF);

KEYWORD_ID (conf_tacacs_auth_logout, conf_tacacs_auth_eol,
	    conf_tacacs_auth_slip,
	    OBJ(int,1), 0,
	    "logout",
	    NULL,
	    PRIV_CONF | PRIV_HIDDEN);

KEYWORD_ID (conf_tacacs_auth_enable, conf_tacacs_auth_eol,
	    conf_tacacs_auth_logout,
	    OBJ(int,1), TACACS_ASKENA,
	    "enable", "Use of the 'enable' command", PRIV_CONF);

KEYWORD_ID (conf_tacacs_auth_conn, conf_tacacs_auth_nvcheck,
	    conf_tacacs_auth_enable,
	    OBJ(int,1), TACACS_ASKCONNECT,
	    "connections", "User-initiated TCP connections", PRIV_CONF);
	    
KEYWORD	(conf_tacacs_auth_kwd, conf_tacacs_auth_conn, conf_tacacs_notify,
	 "authenticate", "Types of connections requiring authentication",
	 PRIV_CONF);
ASSERT	(conf_tacacs_auth, conf_tacacs_auth_kwd, NONE,
	 ((tacacs_extended) && (old_access_control)));

/******************************************************************
 * tacacs-server extended
 * no tacacs-server extended
 */
EOLS	(conf_tacacs_ext_eol, tacacs_service, TACACS_EXTENDED);
KEYWORD	(conf_tacacs_ext, conf_tacacs_ext_eol, conf_tacacs_auth,
	 "extended", "Enable extended TACACS", PRIV_CONF);

/******************************************************************
 * tacacs_server optional-passwords
 * no tacacs_server optional-passwords
 */
EOLS	(conf_tacacs_pass_eol, tacacs_service, TACACS_PASSWORDOPT);
KEYWORD (conf_tacacs_pass_kwd, conf_tacacs_pass_eol, conf_tacacs_ext,
	 "optional-passwords",
	 "The first TACACS request can be made without password verification",
	 PRIV_CONF);
TEST_BOOLEAN(conf_tacacs_pass, conf_tacacs_pass_kwd, NONE, NONE,
	     old_access_control);

LINK_TRANS(conf_tacacs_extend_here, conf_tacacs_pass);

/******************************************************************
 * tacacs_server directed-request [ restricted ]
 * no tacacs_server directed-request
 */
EOLS	(conf_tacacs_directed_req_eol, tacacs_service,
	 TACACS_DIRECTED_REQUEST);
KEYWORD_ID (conf_tacacs_directed_restricted, conf_tacacs_directed_req_eol,
	    conf_tacacs_directed_req_eol, OBJ(int,1), TRUE,
	    "restricted", "restrict queries to directed request servers only",
	    PRIV_CONF);

NVGENS (conf_tacacs_directed_nvgen, conf_tacacs_directed_restricted,
	tacacs_service, TACACS_DIRECTED_REQUEST);

KEYWORD (conf_tacacs_directed_req, conf_tacacs_directed_nvgen,
	 conf_tacacs_extend_here, 
	 "directed-request",
	 "Allow user to specify tacacs server to use with `@server'",
	 PRIV_CONF);

/******************************************************************
 * tacacs login-timeout <seconds>
 * no tacacs login-timeout [<seconds>]
 */
PARAMS_KEYONLY (conf_tacacs_login_time, conf_tacacs_directed_req,
                "login-timeout", OBJ(int,1), 1, 300,
                tacacs_service, TACACS_LOGIN_TIMEOUT,
                "Max seconds to wait for login/password via TACACS",
                "Max seconds to wait", PRIV_CONF);

TEST_BOOLEAN(conf_tacacs_login, conf_tacacs_directed_req,
            conf_tacacs_login_time, NONE,
            old_access_control);

/******************************************************************
 * tacacs-server timeout <seconds>
 * no tacacs-server timeout [<seconds>]
 */
PARAMS_KEYONLY (conf_tacacs_time, conf_tacacs_login,
		"timeout", OBJ(int,1), 1, 1000,
		tacacs_service, TACACS_TIMEOUTVALUE,
		"Time to wait for a TACACS server to reply",
		"Wait time (default 5 seconds)", PRIV_CONF);

/******************************************************************
 * tacacs-server retransmit <count>
 * no tacacs-server retransmit [<count>]
 */
PARAMS_KEYONLY (conf_tacacs_retrans_kwd, conf_tacacs_time,
		"retransmit", OBJ(int,1), 0, 100,
		tacacs_service, TACACS_RETRANSMIT,
		"Search iterations of the TACACS server list",
		"Number of times to search the TACACS list (default 2)",
		PRIV_CONF);

TEST_BOOLEAN(conf_tacacs_retrans, conf_tacacs_retrans_kwd, conf_tacacs_time,
             NONE, old_access_control);

/******************************************************************
 * tacacs-server last-resort { password | succeed }
 * no  tacacs-server last-resort [{ password | succeed }]
 */
EOLS	(conf_tacacs_last_eol, tacacs_service, TACACS_LAST);
KEYWORD_ID (conf_tacacs_last_succ, conf_tacacs_last_eol, no_alt,
	    OBJ(int,1), TACACS_LAST_SUCCEED,
	    "succeed", "Access to privileged level is granted", PRIV_CONF);
KEYWORD_ID (conf_tacacs_last_pass, conf_tacacs_last_eol, conf_tacacs_last_succ,
	    OBJ(int,1), TACACS_LAST_PASSWORD,
	    "password", "The 'enable' password must be provided", PRIV_CONF);
NOPREFIX (conf_tacacs_last_noprefix, conf_tacacs_last_pass,
	  conf_tacacs_last_eol);
KEYWORD_ID(conf_tacacs_last_kwd, conf_tacacs_last_noprefix, NONE,
	   OBJ(int,1), TACACS_LAST_FAIL,
	   "last-resort", "Define TACACS action if no server responds",
	   PRIV_CONF);
TEST_BOOLEAN(conf_tacacs_last, conf_tacacs_last_kwd, NONE, conf_tacacs_retrans,
	     old_access_control);

/******************************************************************
 * tacacs-server attempts <count>
 * no tacacs-server attempts [<count>]
 */
PARAMS_KEYONLY (conf_tacacs_attempt, conf_tacacs_last,
		"attempts", OBJ(int,1), 1, 1000, 
		tacacs_service, TACACS_ATTEMPTS,
		"Number of login attempts via TACACS",
		"Number of login attempts", PRIV_CONF);

/******************************************************************
 * tacacs-server host {<ipaddr> | <name>} [ single-connection ] [port <n> ] [ timeout <n> ] [ key <string> ]
 * no tacacs-server host {<ipaddr> | <name>}
 *
 * OBJ(string,1) -- per server key
 * OBJ(int,1) -- per server single-connection flag
 * OBJ(int,2) -- per server timeout value
 * OBJ(int,3) -- per server port number
 */

EOLS	(conf_tacacs_host_eol, tacacs_service, TACACS_HOST);

TEXT(conf_tacacs_keytext, conf_tacacs_host_eol, no_alt,
     OBJ(string,1), "Text for this server's key");

KEYWORD(conf_tacacs_key, conf_tacacs_keytext, conf_tacacs_host_eol,
	"key", "per-server encryption key (overrides default)",
	PRIV_CONF);

NUMBER(conf_tacacs_timeoutval, conf_tacacs_key, conf_tacacs_host_eol,
       OBJ(int,2), 1, 1000,
       "Timeout value in seconds to wait for server to reply");

KEYWORD(conf_tacacs_timeout, conf_tacacs_timeoutval, conf_tacacs_key,
	"timeout", 
	"Time to wait for this TACACS server to reply (overrides default)",
	PRIV_CONF);

NUMBER(conf_tacacs_portnum, conf_tacacs_timeout, conf_tacacs_timeout,
       OBJ(int,3), 1, 10000,
       "Port number");

KEYWORD(conf_tacacs_port, conf_tacacs_portnum, conf_tacacs_timeout,
	"port", 
	"TCP port for TACACS+ server (default is 49)",
	PRIV_CONF);

KEYWORD_OR(conf_tacacs_single, conf_tacacs_port, conf_tacacs_port,
	   OBJ(int,1), TACACS_SINGLE_CONNECTION,
	   "single-connection",
	   "Multiplex all packets over a single tcp connection to server"
	   " (for CiscoSecure)",
	   PRIV_CONF);

TEST_BOOLEAN(conf_tacacs_old, conf_tacacs_host_eol, conf_tacacs_single, 
	     no_alt,  old_access_control);

IPADDR_NAME (conf_tacacs_host_addr, conf_tacacs_old, conf_tacacs_host_eol,
	 OBJ(paddr,1), "IP address of TACACS server");
NVGENS	(conf_tacacs_host_nvgen, conf_tacacs_host_addr, tacacs_service, 
	 TACACS_HOST);
KEYWORD	(conf_tacacs_host, conf_tacacs_host_nvgen, conf_tacacs_attempt,
	 "host", "Specify a TACACS server", PRIV_CONF);

/******************************************************************/
KEYWORD	(conf_tacacs, conf_tacacs_host, ALTERNATE,
	 "tacacs-server", "Modify TACACS query parameters", PRIV_CONF);

#undef	ALTERNATE
#define	ALTERNATE	conf_tacacs
