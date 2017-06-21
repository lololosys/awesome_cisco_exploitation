/* $Id: exec_debug_parser.h,v 3.3 1996/03/09 22:56:47 widmer Exp $
 * $Source: /release/111/cvs/Xsys/parser/exec_debug_parser.h,v $
 *------------------------------------------------------------------
 * E X E C _ D E B U G _ P A R S E R . H
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_debug_parser.h,v $
 * Revision 3.3  1996/03/09  22:56:47  widmer
 * CSCdi47890:  HTTP server doesnt work
 * Add HTTP server
 *
 * Revision 3.2  1995/11/17  18:46:18  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:52:31  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/06/19  14:48:37  widmer
 * CSCdi36074:  Clean up debug parse chain alternates
 * Add keyword_debug macro to assign debug array
 *
 * Revision 2.1  1995/06/07  22:19:39  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/******************************************************************
 * debug parser privilege
 *
 */
EOLS	(debug_parser_priv_eol, debug_command, DEBUG_PARSER_PRIV);
KEYWORD	(debug_parser_priv, debug_parser_priv_eol, no_alt,
	 "privilege", "Debug parser privilege levels", PRIV_ROOT);

/******************************************************************
 * debug parser mode
 *
 */
EOLS	(debug_parser_mode_eol, debug_command, DEBUG_PARSER_MODE);
KEYWORD (debug_parser_mode, debug_parser_mode_eol, debug_parser_priv,
	 "mode", "Debug parser modes", PRIV_ROOT);

/******************************************************************
 * debug parser http
 *
 */
EOLS	(debug_parser_http_eol, debug_command, DEBUG_PARSER_HTTP);
KEYWORD (debug_parser_http, debug_parser_http_eol, debug_parser_mode,
	 "http", "Debug parser http", PRIV_ROOT);

/******************************************************************
 * debug parser help
 *
 */
EOLS	(debug_parser_help_eol, debug_command, DEBUG_PARSER_HELP);
KEYWORD (debug_parser_help, debug_parser_help_eol, debug_parser_http,
	 "help", "Debug parser help messages", PRIV_ROOT);

/******************************************************************
 * debug parser ambiguity
 *
 */
EOLS	(debug_parser_ambig_eol, debug_command, DEBUG_PARSER_AMBIG);
KEYWORD (debug_parser_ambig, debug_parser_ambig_eol, debug_parser_help,
	 "ambiguity", "Debug parser ambiguity messages", PRIV_ROOT);

/******************************************************************
 * debug parser alias
 *
 */
EOLS	(debug_parser_alias_eol, debug_command, DEBUG_PARSER_ALIAS);
KEYWORD (debug_parser_alias, debug_parser_alias_eol, debug_parser_ambig,
	 "alias", "Debug parser aliases", PRIV_ROOT);


KEYWORD_DEBUG(debug_parser, debug_parser_alias, ALTERNATE,
	      OBJ(pdb,1), parser_debug_arr,
	      "parser", "Debug parser behavior", PRIV_ROOT | PRIV_HIDDEN);

#undef	ALTERNATE
#define	ALTERNATE	debug_parser
