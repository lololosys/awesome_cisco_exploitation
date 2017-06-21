/* $Id: tn3270_chain.c,v 3.5.10.1 1996/05/21 10:06:44 thille Exp $
 * $Source: /release/112/cvs/Xsys/tn3270/tn3270_chain.c,v $
 *------------------------------------------------------------------
 * TN3270 Parse chains
 *
 * May 1993, Robert Widmer
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: tn3270_chain.c,v $
 * Revision 3.5.10.1  1996/05/21  10:06:44  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.5  1996/02/22  14:39:07  widmer
 * CSCdi49281:  make all parser_extension_request static const
 * make parser_extension_request typedef const
 * make all parser_extension_request static
 *
 * Revision 3.4  1996/02/01  06:10:46  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.3  1995/12/17  18:39:30  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.2  1995/11/17  18:51:46  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:37:38  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/07/02  01:46:38  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize xns, atalk, vines, decnet, xremote, x25, unix, wan, tn3270,
 * clns, cdp
 *
 * Revision 2.1  1995/06/07  23:08:09  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include "ttysrv.h"
#include "config.h"
#include "parser.h"
#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "../parser/parser_defs_exec.h"
#include "parser_defs_tn3270.h"

#include "tn3270.h"			/* Defines CISCO */
#include "vt_interface.h"
#include "tn3270_private.h"


/*
 * Parse chains for tn3270 exec commands
 */
#define	ALTERNATE	NONE
#include "exec_tn3270.h"
LINK_POINT(tn3270_exec_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chains for tn3270 config commands
 */
#define	ALTERNATE	NONE
#include "cfg_keymap.h"
#include "cfg_ttycap.h"
#include "cfg_tn3270.h"
LINK_POINT(tn3270_config_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chains for tn3270 show commands
 */
#define	ALTERNATE	NONE
#include "exec_show_ttycap.h"
#include "exec_show_tn3270.h"
#include "exec_show_keymap.h"
LINK_POINT(tn3270_show_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chains for tn3270 line commands
 */
#define	ALTERNATE	NONE
#include "cfg_line_keymap-type.h"
LINK_POINT(tn3270_line_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chain registration array for tn3270
 */
static parser_extension_request tn3270_chain_init_table[] = {
    { PARSE_ADD_EXEC_CMD, &pname(tn3270_exec_commands) },
    { PARSE_ADD_CFG_ROUTING_CMD, &pname(tn3270_config_commands) },
    { PARSE_ADD_SHOW_CMD, &pname(tn3270_show_commands) },
    { PARSE_ADD_LINE_TERM_CMD, &pname(tn3270_line_commands) },
    { PARSE_ADD_TELNET_EXIT_CMD,
	  (dynamic_transition *) &pname(exec_tn3270_exit) },
    { PARSE_LIST_END, NULL }
};


/*
 * tn3270_parser_init - Initialize tn3270 parser support
 */
void tn3270_parser_init (void)
{
    parser_add_command_list(tn3270_chain_init_table, "tn3270");
}
