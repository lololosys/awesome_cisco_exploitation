/* $Id: tarp_chain.c,v 3.6.10.1 1996/05/21 10:05:41 thille Exp $
 * $Source: /release/112/cvs/Xsys/tarp/tarp_chain.c,v $
 *------------------------------------------------------------------
 * TARP (Target ID Address Resolution Protocol) parser support
 *
 * October, 1995 Arun Sastry
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: tarp_chain.c,v $
 * Revision 3.6.10.1  1996/05/21  10:05:41  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.6  1996/02/22  14:38:52  widmer
 * CSCdi49281:  make all parser_extension_request static const
 * make parser_extension_request typedef const
 * make all parser_extension_request static
 *
 * Revision 3.5  1996/02/01  06:10:43  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.4  1996/01/29  07:30:15  hampton
 * Remove unnecessary includes of parser definition files.  [CSCdi47717]
 *
 * Revision 3.3  1995/12/17  18:39:00  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.2  1995/11/17  18:44:39  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:33:07  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include "config.h"
#include "parser.h"
#include "packet.h"
#include "interface_private.h"
#include "../ui/debug.h"
#include "../if/network.h"
#include "tarp.h"
#include "tarp_debug.h"

#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "parser_defs_tarp.h"
#include "../parser/parser_defs_parser.h"


/*
 * Parse chains for EXEC commands
 */
#define	ALTERNATE	NONE
#include "exec_tarp_originate.h"
LINK_POINT(tarp_exec_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chains for CLEAR commands
 */
#define ALTERNATE NONE
#include "exec_clear_tarp.h"
LINK_POINT(tarp_clear_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chains for TARP deubg commands
 */
#define	ALTERNATE	NONE
#include "exec_debug_tarp.h"
LINK_POINT(tarp_debug_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chains for TARP config commands
 */
#define ALTERNATE       NONE
#include "cfg_tarp.h"
LINK_POINT(tarp_config_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chains for top-level exec commands
 */
#define ALTERNATE NONE
#include "exec_show_tarp.h"
LINK_POINT(tarp_exec_show_commands, ALTERNATE);
#undef ALTERNATE


/*
 * Parse chains for TARP interface commands
 */
#define	ALTERNATE	NONE
#include "cfg_int_tarp.h"
LINK_POINT(tarp_interface_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chain registration array for TARP
 */
static parser_extension_request tarp_chain_init_table[] = {
    { PARSE_ADD_EXEC_CMD, &pname(tarp_exec_commands) },
    { PARSE_ADD_CLEAR_CMD, &pname(tarp_clear_commands) },
    { PARSE_ADD_DEBUG_CMD, &pname(tarp_debug_commands) },
    { PARSE_ADD_CFG_ROUTING_CMD, &pname(tarp_config_commands) },
    { PARSE_ADD_SHOW_CMD, &pname(tarp_exec_show_commands) },
    { PARSE_ADD_CFG_INT_AS_CMD, &pname(tarp_interface_commands) },
    { PARSE_LIST_END, NULL }
};


/*
 * tarp_parser_init - Initialize TARP parser support
 */
void tarp_parser_init (void)
{
    parser_add_link_point(PARSE_ADD_CFG_TARP, "tarp",
			  &pname(conf_tarp_extend_here));
    parser_add_command_list(tarp_chain_init_table, "tarp");

}

