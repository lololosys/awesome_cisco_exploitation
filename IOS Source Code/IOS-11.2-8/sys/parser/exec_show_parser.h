/* $Id: exec_show_parser.h,v 3.2.62.2 1996/09/05 13:59:45 widmer Exp $
 * $Source: /release/112/cvs/Xsys/parser/exec_show_parser.h,v $
 *------------------------------------------------------------------
 * Show parser commands
 *
 * November 1993, Robert Widmer
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: exec_show_parser.h,v $
 * Revision 3.2.62.2  1996/09/05  13:59:45  widmer
 * CSCdi67994:  Remove names from parser link points
 * Branch: California_branch
 *
 * Revision 3.2.62.1  1996/08/12  16:06:22  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.2  1995/11/17  18:47:34  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:53:18  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:25:13  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

EOLNS	(show_parser_modes_eol, show_parser_modes);
KEYWORD	(show_parser_modes, show_parser_modes_eol, no_alt,
	 "modes", NULL, PRIV_HIDDEN | PRIV_ROOT);

EOLNS	(show_parser_unresolved_eol, show_parser_unresolved);
KEYWORD	(show_parser_unresolved, show_parser_unresolved_eol, show_parser_modes,
	 "unresolved", NULL, PRIV_HIDDEN | PRIV_ROOT);

#ifdef	PARSER_DEBUG_LINKS
EOLNS	(show_parser_links_eol, show_parser_links);
TEXT	(show_parser_links_name, show_parser_links_eol, show_parser_links_eol,
	 OBJ(string,1), NULL);
KEYWORD	(show_parser_links, show_parser_links_name, show_parser_unresolved,
	 "links", NULL, PRIV_HIDDEN | PRIV_ROOT);
#else	/* PARSER_DEBUG_LINKS */
NOP	(show_parser_links, show_parser_unresolved, NONE);
#endif	/* PARSER_DEBUG_LINKS */

KEYWORD	(show_parser, show_parser_links, ALTERNATE,
	 "parser", NULL, PRIV_HIDDEN | PRIV_ROOT);

#undef	ALTERNATE
#define	ALTERNATE	show_parser
