/* $Id: xremote_chain.c,v 3.5.10.1 1996/05/21 10:12:14 thille Exp $
 * $Source: /release/112/cvs/Xsys/xremote/xremote_chain.c,v $
 *------------------------------------------------------------------
 * Xremote parse chains
 *
 * May 1993, Robert Widmer
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: xremote_chain.c,v $
 * Revision 3.5.10.1  1996/05/21  10:12:14  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.5  1996/02/22  14:40:42  widmer
 * CSCdi49281:  make all parser_extension_request static const
 * make parser_extension_request typedef const
 * make all parser_extension_request static
 *
 * Revision 3.4  1996/02/01  06:13:37  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.3  1995/12/17  18:42:55  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.2  1995/11/17  19:26:52  gstovall
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  14:01:45  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/07/02  01:44:45  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize xns, atalk, vines, decnet, xremote, x25, unix, wan, tn3270,
 * clns, cdp
 *
 * Revision 2.1  1995/06/07  23:35:26  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include "ttysrv.h"
#include "config.h"
#include "../ui/debug.h"
#include "parser.h"
#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "parser_defs_xremote.h"
#include "../xremote/xremote.h"
#include "../xremote/xremote_debug.h"


/*
 * Parse chains for Xremote exec commands
 */
#define	ALTERNATE	NONE
#include "exec_xremote.h"
LINK_POINT(xremote_exec_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chains for Xremote debug commands
 */
#define	ALTERNATE	NONE
#include "exec_debug_xremote.h"
LINK_POINT(xremote_debug_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chains for Xremote show commands
 */
#define ALTERNATE       NONE
#include "exec_show_xremote.h"
LINK_POINT(xremote_show_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chains for Xremote config commands
 */
#define	ALTERNATE	NONE
#include "cfg_xremote.h"
LINK_POINT(xremote_config_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chain registration array for Xremote
 */
static parser_extension_request xremote_chain_init_table[] = {
    { PARSE_ADD_EXEC_CMD, &pname(xremote_exec_commands) },
    { PARSE_ADD_DEBUG_CMD, &pname(xremote_debug_commands) },
    { PARSE_ADD_SHOW_CMD, &pname(xremote_show_commands) },
    { PARSE_ADD_CFG_ROUTING_CMD, &pname(xremote_config_commands) },
    { PARSE_LIST_END, NULL }
};


/*
 * xremote_parser_init - Initialize Xremote parser support
 */
void xremote_parser_init (void)
{
    parser_add_command_list(xremote_chain_init_table, "xremote");
}
