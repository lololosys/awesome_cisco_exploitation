/* $Id: snapshot_chain.c,v 3.6.10.1 1996/05/21 10:09:57 thille Exp $
 * $Source: /release/112/cvs/Xsys/wan/snapshot_chain.c,v $
 *------------------------------------------------------------------
 * snapshot parse chains
 *
 * April 1994, Joel P. Bion
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: snapshot_chain.c,v $
 * Revision 3.6.10.1  1996/05/21  10:09:57  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.6  1996/02/22  14:40:07  widmer
 * CSCdi49281:  make all parser_extension_request static const
 * make parser_extension_request typedef const
 * make all parser_extension_request static
 *
 * Revision 3.5  1996/02/01  06:12:41  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.4  1996/01/29  07:31:29  hampton
 * Remove unnecessary includes of parser definition files.  [CSCdi47717]
 *
 * Revision 3.3  1995/12/17  18:41:28  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.2  1995/11/17  18:04:55  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:51:18  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/07/02  01:46:16  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize xns, atalk, vines, decnet, xremote, x25, unix, wan, tn3270,
 * clns, cdp
 *
 * Revision 2.1  1995/06/07  23:20:02  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include "config.h"
#include "interface_private.h"
#include "parser.h"
#include "../ui/debug.h"
#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "../parser/parser_defs_parser.h"
#include "parser_defs_snapshot.h"
#include "../wan/snapshot.h"
#include "../wan/snapshot_debug.h"
#include "../wan/snapshot_actions.h"

/*
 * Parse chains for Snapshot debug commands
 */
#define	ALTERNATE	NONE
#include "exec_debug_snapshot.h"
LINK_POINT(snapshot_debug_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chains for Snapshot show commands
 */
#define	ALTERNATE	NONE
#include "exec_show_snapshot.h"
LINK_POINT(snapshot_show_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chains for Snapshot clear commands
 */
#define ALTERNATE	NONE
#include "exec_clear_snapshot.h"
LINK_POINT(snapshot_clear_commands, ALTERNATE);
#undef ALTERNATE


/*
 * Parse chains for Snapshot interface commands
 */
#define	ALTERNATE	NONE
#include "cfg_int_snapshot.h"
LINK_POINT(snapshot_interface_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chain registration array for Snapshot commands
 */
static parser_extension_request snapshot_chain_init_table[] = {
    { PARSE_ADD_CLEAR_CMD, &pname(snapshot_clear_commands) },
    { PARSE_ADD_DEBUG_CMD, &pname(snapshot_debug_commands) },
    { PARSE_ADD_SHOW_CMD, &pname(snapshot_show_commands) },
    { PARSE_ADD_CFG_INT_CMD, &pname(snapshot_interface_commands) },
    { PARSE_LIST_END, NULL }
};


/*
 * snapshot_parser_init - Initialize parse structures for
 *				Snapshot dialer commands
 */
void snapshot_parser_init (void)
{
    parser_add_protoaddr(LINK_SNAPSHOT,	PMATCH_SNAPSHOT PMATCH_DGRAM,
			 "snapshot", "Snapshot routing support", 4,
			 ADDR_SNAPSHOT);
    parser_add_address(ADDR_SNAPSHOT, snapshot_addr_short_help,
		       match_snapshot_addr);
    parser_add_command_list(snapshot_chain_init_table, "snapshot");
}

