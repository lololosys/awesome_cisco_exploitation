/* $Id: pt_auto_chain.c,v 3.6.10.1 1996/05/21 10:00:48 thille Exp $
 * $Source: /release/112/cvs/Xsys/pt/pt_auto_chain.c,v $
 *------------------------------------------------------------------
 * Parse chains for PT Autocomand
 *
 * November 1994, Eman Schaffe
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: pt_auto_chain.c,v $
 * Revision 3.6.10.1  1996/05/21  10:00:48  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.6  1996/02/22  14:37:17  widmer
 * CSCdi49281:  make all parser_extension_request static const
 * make parser_extension_request typedef const
 * make all parser_extension_request static
 *
 * Revision 3.5  1996/02/01  06:08:06  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.4  1996/01/29  07:29:28  hampton
 * Remove unnecessary includes of parser definition files.  [CSCdi47717]
 *
 * Revision 3.3  1995/12/17  18:36:23  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.2  1995/11/17  18:59:25  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:01:03  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/07/04  01:56:31  tli
 * CSCdi36508: Rationalize parser file placement, part I
 * Rationalize cls dev dlsw dspu filesys irix lat les lex llc2 parser pgen
 * pt servers smf smrp snmp src-36-c1000 src-4k-c4000 src-68-c3000
 * src-68-c4000 src-c4000 src-rsp srt tcl ts ui
 *
 * Revision 2.1  1995/06/07  22:34:47  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include "interface_private.h"
#include "packet.h"
#include "config.h"
#include "../ui/debug.h"
#include "parser.h"

#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "../parser/parser_defs_lat.h"
#include "../pt/parser_defs_pt.h"
#include "../pt/pt_autocommand.h"


/*
 * Parse chains for AUTOCOMMAND translate out commands
 */
#define       ALTERNATE       NONE
#include "cfg_translate_autocommand_out.h"
LINK_POINT(autocommand_out_translate_commands, ALTERNATE);
#undef        ALTERNATE

/*
 * Parse chain registration array for PT AUTOCOMMAND
 */
static parser_extension_request pt_autocommand_parse_init_table[] = {
    { PARSE_ADD_CFG_TRANS_OUT_CMD, &pname(autocommand_out_translate_commands) 
      }, 
	{ PARSE_ADD_CFG_TRANS_OUT_RETURN, 
	  (dynamic_transition *) &pname(trans_out_autocommand_return) },
	{ PARSE_LIST_END, NULL }
};

void pt_autocommand_parser_init (void)
{
    parser_add_command_list(pt_autocommand_parse_init_table, "pt_autocommand");
}
