/* $Id: cfg_http.h,v 3.2.4.1 1996/05/06 17:39:53 widmer Exp $
 * $Source: /release/112/cvs/Xsys/tcp/cfg_http.h,v $
 *------------------------------------------------------------------
 * HTTP configuration commands
 *
 * January 1996, Robert Widmer
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cfg_http.h,v $
 * Revision 3.2.4.1  1996/05/06  17:39:53  widmer
 * CSCdi56492:  Need to limit access to http server with access-list
 * Branch: California_branch
 * Add "ip http access-class" command
 *
 * Revision 3.2  1996/03/09  22:57:35  widmer
 * CSCdi47890:  HTTP server doesnt work
 * Add HTTP server
 *
 * Revision 3.1  1996/02/11  04:25:46  widmer
 * CSCdi47890:  HTTP server doesnt work
 * Add placeholder files
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

PARAMS_KEYONLY(cfg_http_access, no_alt, "access-class",
	       OBJ(int,1), MINFASTACC, MAXFASTACC,
	       http_config_command, HTTP_ACCESS,
	       "Restrict access by access-class",
	       "Access list number", PRIV_CONF);

PARAMS_KEYONLY(cfg_http_port, cfg_http_access, "port",
	       OBJ(int,1), 0, MAXUSHORT,
	       http_config_command, HTTP_PORT,
	       "HTTP port", "HTTP port", PRIV_CONF);

EOLS	(cfg_http_eol, http_config_command, HTTP_SERVER);
KEYWORD	(cfg_http_server, cfg_http_eol, cfg_http_port,
	 "server", "Enable HTTP server", PRIV_CONF);

KEYWORD	(cfg_http, cfg_http_server, ALTERNATE,
	 "http", "HTTP server configuration", PRIV_CONF);

#undef	ALTERNATE
#define	ALTERNATE	cfg_http
