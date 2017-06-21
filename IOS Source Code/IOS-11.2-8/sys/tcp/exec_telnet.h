/* $Id: exec_telnet.h,v 3.1.2.2 1996/08/12 16:08:09 widmer Exp $
 * $Source: /release/112/cvs/Xsys/tcp/exec_telnet.h,v $
 *------------------------------------------------------------------
 * E X E C _ T E L N E T . H
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_telnet.h,v $
 * Revision 3.1.2.2  1996/08/12  16:08:09  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.1.2.1  1996/04/15  21:23:23  bcole
 * CSCdi54649:  IP Host Modularity, IP Automatic Stub Routing feature
 * commit
 * Branch: California_branch
 *
 * Revision 3.1  1996/04/15  01:40:28  bcole
 * Placeholders for 11.2 features.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * telnet  <host> [<port>]
 *
 * <host>	= OBJ(string,1)
 * <port>	= OBJ(int,1)
 *		OBJ(int,2) = TRUE  if <ipaddr> is set
 *			   FALSE if <ipaddr> is not set
 *		OBJ(int,3) = TRUE  if /debug is set
 *			   FALSE if /debug is not set
 *		OBJ(int,4) = TRUE  if /line is set
 *			   FALSE if /line is not set
 *		OBJ(int,5) = TRUE  if /stream is set
 *			   FALSE if /stream is not set
 *		OBJ(int,6) = TRUE  if /route is set
 *			   FALSE if /route is not set
 *		OBJ(int,7) = TRUE  if /noecho is set
 *			   FALSE is /noecho is not set
 *		OBJ(int,8) = Internal flags
 *		OBJ(int,9) = TRUE if TN3270 command
 *			   FALSE if TELNET command
 * OBJ(idb,1) = interface for /source-interface switch
 */

EOLNS	(exec_telnet_eol, telnet_connect_command);
EOLS    (exec_connect_eol, connect_command, CMD_CONNECT_DEFAULT);
EOLNS	(exec_rlogin_eol, connect_command); /* not rlogin_connect_command */
LINK_TRANS(exec_rlogin_return, exec_rlogin_eol);

TESTVAR	(exec_rlogin_eol_test, exec_rlogin_eol, exec_telnet_eol,
	 NONE, NONE, NONE, OBJ(int,9), CMD_CONNECT_RLOGIN);
TESTVAR	(exec_connect_eol_test, exec_connect_eol, exec_rlogin_eol_test,
	 NONE, NONE, NONE, OBJ(int,9), CMD_CONNECT_DEFAULT);

/* keyword */

pdecl(exec_telnet_keyword);
pdecl(exec_telnet_route_addr_opt);
#define	TELNET_ROUTE_SET	1<<0
#define	TELNET_STREAM_SET	1<<1
#define	TELNET_LINE_SET		1<<2
#define	TELNET_DEBUG_SET	1<<3
#define	TELNET_NOECHO_SET	1<<4

LINK_TRANS(exec_telnet_return_here, exec_telnet_keyword);
LINK_TRANS(exec_telnet_opts_extend_here, exec_connect_eol_test);

INTERFACE(exec_telnet_source_name, exec_telnet_keyword, no_alt,
	   OBJ(idb,1), IFTYPE_ANYSUB);
KEYWORD	(exec_telnet_source, exec_telnet_source_name,
	 exec_telnet_opts_extend_here,
         "/source-interface", "Specify source interface", PRIV_USER);

SET	(exec_telnet_route_set, exec_telnet_keyword, OBJ(int,6), TRUE);

/* optional 8th address */
IPADDR_NAME(exec_telnet_route_addr8, exec_telnet_route_set,
	    exec_telnet_route_set,
	    OBJ(paddr,8), "Next host in route");

/* optional 7th address */
IPADDR_NAME(exec_telnet_route_addr7, exec_telnet_route_addr8,
	    exec_telnet_route_set,
	    OBJ(paddr,7), "Next host in route");

/* optional 6th address */
IPADDR_NAME(exec_telnet_route_addr6, exec_telnet_route_addr7,
	    exec_telnet_route_set,
	    OBJ(paddr,6), "Next host in route");

/* optional 5th address */
IPADDR_NAME(exec_telnet_route_addr5, exec_telnet_route_addr6,
	    exec_telnet_route_set,
	    OBJ(paddr,5), "Next host in route");

/* optional 4th address */
IPADDR_NAME(exec_telnet_route_addr4, exec_telnet_route_addr5,
	    exec_telnet_route_set,
	    OBJ(paddr,4), "Next host in route");

/* optional 3rd address */
IPADDR_NAME(exec_telnet_route_addr3, exec_telnet_route_addr4,
	    exec_telnet_route_set,
	    OBJ(paddr,3), "Next host in route");

/* optional 2nd address */
IPADDR_NAME(exec_telnet_route_addr2, exec_telnet_route_addr3,
	    exec_telnet_route_set,
	    OBJ(paddr,2), "Next host in route");

/* required 1st address */
IPADDR_NAME(exec_telnet_route_addr1, exec_telnet_route_addr2,
	    no_alt,
	    OBJ(paddr,1), "Next host in route");

GENERAL_KEYWORD(exec_telnet_route, exec_telnet_route_addr1,
		exec_telnet_source,
		"/route:", "Enable telnet source route mode", PRIV_USER,
		CSBOFF(OBJ(int,8)), TELNET_ROUTE_SET, 2,
		(KEYWORD_WS_OK | KEYWORD_NO_WS_OK |
		 KEYWORD_HELP_CHECK | KEYWORD_OR_SET | KEYWORD_TEST));

SET	(exec_telnet_debug_set, exec_telnet_keyword, OBJ(int,3), TRUE);
KEYWORD_ORTEST(exec_telnet_debug, exec_telnet_debug_set, exec_telnet_route,
	       OBJ(int,8), TELNET_DEBUG_SET,
	       "/debug", "Enable telnet debugging mode", PRIV_USER);

SET	(exec_telnet_stream_set, exec_telnet_keyword, OBJ(int,5), TRUE);
KEYWORD_ORTEST(exec_telnet_stream, exec_telnet_stream_set, NONE,
	       OBJ(int,8), TELNET_STREAM_SET,
	       "/stream", "Enable stream processing", PRIV_USER);

SET	(exec_telnet_noecho_set, exec_telnet_keyword, OBJ(int,7), TRUE);
KEYWORD_ORTEST(exec_telnet_noecho, exec_telnet_noecho_set, exec_telnet_stream,
	       OBJ(int,8), TELNET_NOECHO_SET,
	       "/noecho", "Disable local echo", PRIV_USER);


SET	(exec_telnet_line_set, exec_telnet_keyword, OBJ(int,4), TRUE);
KEYWORD_ORTEST(exec_telnet_line, exec_telnet_line_set, exec_telnet_noecho,
	       OBJ(int,8), TELNET_LINE_SET,
	       "/line", "Enable telnet line mode", PRIV_USER);

TESTVAR (exec_telnet_keyword, NONE, exec_telnet_line,
         NONE, NONE, exec_telnet_debug, OBJ(int,9), CMD_CONNECT_TN3270);

/* port */

/* Note "tacacs-ds" -> 49 is wrong, but needed for backwards compatibility */
KEYWORD_ID_MM (exec_telnet_tacacs, exec_telnet_keyword, exec_telnet_keyword,
	       OBJ(int, 1), TACACS_PORT,
	       "tacacs-ds", NULL, PRIV_CONF|PRIV_NOHELP, 7);
TCP_PORT (exec_telnet_port, exec_telnet_keyword,
	  exec_telnet_tacacs, OBJ(int, 1));
LINK_TRANS(exec_telnet_port_return, exec_telnet_port);

/* <host> */

/* Use a string here instead of IPADDR_NAME because
 * connect_command wants to do the lookup of names and
 * increments a reference count if one is found.
 */
STRING	(exec_telnet_ipaddr, exec_telnet_port, exec_connect_eol_test,
	 OBJ(string,1), "IP address or hostname of a remote system");
LINK_TRANS(exec_telnet_entry, exec_telnet_ipaddr);

/* connect */
/*******************************************************/
KEYWORD_ID(exec_connect, exec_telnet_ipaddr, ALTERNATE,
	   OBJ(int,9), CMD_CONNECT_DEFAULT,
	   "connect", "Open a terminal connection",
	   PRIV_USER | PRIV_INTERACTIVE);

/* telnet */
KEYWORD_ID(exec_telnet, exec_telnet_ipaddr, exec_connect,
	   OBJ(int,9), CMD_CONNECT_TELNET,
	   "telnet", "Open a telnet connection",
	   PRIV_USER | PRIV_INTERACTIVE);

#undef	ALTERNATE
#define	ALTERNATE	exec_telnet
