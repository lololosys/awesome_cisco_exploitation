/* $Id: cons_chain.c,v 3.6.10.1 1996/05/21 10:10:34 thille Exp $
 * $Source: /release/112/cvs/Xsys/x25/cons_chain.c,v $
 *------------------------------------------------------------------
 * CONS parse chains
 *
 * May 1993, Robert Widmer
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: cons_chain.c,v $
 * Revision 3.6.10.1  1996/05/21  10:10:34  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.6  1996/02/22  14:40:18  widmer
 * CSCdi49281:  make all parser_extension_request static const
 * make parser_extension_request typedef const
 * make all parser_extension_request static
 *
 * Revision 3.5  1996/02/01  06:12:48  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.4  1996/01/29  07:31:32  hampton
 * Remove unnecessary includes of parser definition files.  [CSCdi47717]
 *
 * Revision 3.3  1995/12/17  18:41:49  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.2  1995/11/17  18:05:59  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:52:13  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/07/02  01:45:05  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize xns, atalk, vines, decnet, xremote, x25, unix, wan, tn3270,
 * clns, cdp
 *
 * Revision 2.1  1995/06/07  23:20:49  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include "interface_private.h"
#include "config.h"
#include "parser.h"
#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "../x25/x25.h"
#include "../parser/parser_defs_parser.h"
#include "../x25/parser_defs_cons.h"


/*
 * Parse chains for CONS show commands
 */
#define	ALTERNATE	NONE
#include "exec_show_cons.h"
LINK_POINT(cons_show_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chains for CONS interface commands
 */
#define	ALTERNATE	NONE
#include "cfg_int_cons.h"
LINK_POINT(cons_interface_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chain registration array for CONS
 */
static parser_extension_request cons_chain_init_table[] = {
    { PARSE_ADD_SHOW_CMD, &pname(cons_show_commands) },
    { PARSE_ADD_CFG_INT_CMD, &pname(cons_interface_commands) },
    { PARSE_LIST_END, NULL }
};


/*
 * cons_parser_init - Initialize CONS parser support
 */
void cons_parser_init (void)
{
    parser_add_command_list(cons_chain_init_table, "cons");
}

