/* $Id: pad_chain.c,v 3.5.10.2 1996/05/30 23:47:02 hampton Exp $
 * $Source: /release/112/cvs/Xsys/x25/pad_chain.c,v $
 *------------------------------------------------------------------
 * Pad parse chains
 *
 * May 1993, Robert Widmer
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: pad_chain.c,v $
 * Revision 3.5.10.2  1996/05/30  23:47:02  hampton
 * All commands that take access list numbers should accept either numbers
 * in the current monolithic number space, or hidden numbers in the future
 * per-protocol access list spaces.  [CSCdi59067]
 * Branch: California_branch
 *
 * Revision 3.5.10.1  1996/05/21  10:10:36  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.5  1996/02/22  14:40:22  widmer
 * CSCdi49281:  make all parser_extension_request static const
 * make parser_extension_request typedef const
 * make all parser_extension_request static
 *
 * Revision 3.4  1996/02/01  06:12:56  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.3  1995/12/17  18:41:54  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.2  1995/11/17  18:06:42  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:52:58  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/07/02  01:45:13  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize xns, atalk, vines, decnet, xremote, x25, unix, wan, tn3270,
 * clns, cdp
 *
 * Revision 2.1  1995/06/07  23:21:24  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include "config.h"
#include "../ui/debug.h"
#include "parser.h"
#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "../x25/x25.h"
#include "../x25/pad.h"
#include "../x25/pad_debug.h"
#include "parser_defs_x25.h"
#include "access.h"


/*
 * Parse chains for PAD exec commands
 */
#define	ALTERNATE	NONE
#include "exec_x3.h"
#include "exec_pad.h"
LINK_POINT(pad_exec_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chains for PAD show commands
 */
#define	ALTERNATE	NONE
#include "exec_show_pad.h"
LINK_POINT(pad_show_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chains for PAD debug commands
 */
#define	ALTERNATE	NONE
#include "exec_debug_pad.h"
LINK_POINT(pad_debug_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chains for PAD config commands
 */
#define	ALTERNATE	NONE
#include "cfg_x29.h"
LINK_POINT(x29_config_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chain registration array for PAD
 */
static parser_extension_request pad_chain_init_table[] = {
    { PARSE_ADD_EXEC_CMD, &pname(pad_exec_commands) },
    { PARSE_ADD_SHOW_CMD, &pname(pad_show_commands) },
    { PARSE_ADD_DEBUG_CMD, &pname(pad_debug_commands) },
    { PARSE_ADD_CFG_TOP_CMD, &pname(x29_config_commands) },
    { PARSE_LIST_END, NULL }
};


/*
 * pad_parser_init - Initialize PAD parser support
 */
void pad_parser_init (void)
{
    parser_add_command_list(pad_chain_init_table, "pad");
}
