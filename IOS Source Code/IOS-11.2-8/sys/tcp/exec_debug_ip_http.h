/* $Id: exec_debug_ip_http.h,v 3.2 1996/03/09 22:57:36 widmer Exp $
 * $Source: /release/111/cvs/Xsys/tcp/exec_debug_ip_http.h,v $
 *------------------------------------------------------------------
 * exec_debug_ip_http.h -- Debugging commands for HTTP
 *
 * July 1995 Tony Li
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: exec_debug_ip_http.h,v $
 * Revision 3.2  1996/03/09  22:57:36  widmer
 * CSCdi47890:  HTTP server doesnt work
 * Add HTTP server
 *
 * Revision 3.1  1996/02/11  04:25:47  widmer
 * CSCdi47890:  HTTP server doesnt work
 * Add placeholder files
 *
 *------------------------------------------------------------------
 * $Endlog$
 */



/******************************************************************
 * debug ip http url
 *
 */
EOLS	(debug_http_uri_eol, debug_command, DEBUG_HTTP_URI);
KEYWORD	(debug_http_uri, debug_http_uri_eol, no_alt,
	 "url", "HTTP URL", PRIV_OPR);

/******************************************************************
 * debug ip http transaction
 *
 */
EOLS	(debug_http_trans_eol, debug_command, DEBUG_HTTP_TRANSACTION);
KEYWORD	(debug_http_transaction, debug_http_trans_eol, debug_http_uri,
	 "transactions", "HTTP transactions", PRIV_OPR);

/******************************************************************
 * debug ip http token
 *
 */
EOLS	(debug_http_token_eol, debug_command, DEBUG_HTTP_TOKEN);
KEYWORD	(debug_http_token, debug_http_token_eol, debug_http_transaction,
	 "tokens", "HTTP tokens", PRIV_OPR);

/******************************************************************
 * debug ip http ezsetup
 *
 */
EOLS	(debug_http_ezsetup_eol, debug_command, DEBUG_HTTP_EZSETUP);
KEYWORD	(debug_http_ezsetup, debug_http_ezsetup_eol, debug_http_token,
	 "ezsetup", "HTTP EZSetup", PRIV_OPR);

KEYWORD_DEBUG(debug_http, debug_http_ezsetup, ALTERNATE,
	      OBJ(pdb,1), http_debug_arr,
	      "http", "HTTP connections", PRIV_OPR);

#undef	ALTERNATE
#define	ALTERNATE	debug_http
