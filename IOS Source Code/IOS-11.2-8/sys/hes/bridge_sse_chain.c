/* $Id: bridge_sse_chain.c,v 3.7.10.1 1996/05/21 09:47:33 thille Exp $
 * $Source: /release/112/cvs/Xsys/hes/bridge_sse_chain.c,v $
 *------------------------------------------------------------------
 * bridge_sse_chain.c -- Parser additions for bridging
 *
 * Oct 1993 Tony Li
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: bridge_sse_chain.c,v $
 * Revision 3.7.10.1  1996/05/21  09:47:33  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.7  1996/02/22  14:33:07  widmer
 * CSCdi49281:  make all parser_extension_request static const
 * make parser_extension_request typedef const
 * make all parser_extension_request static
 *
 * Revision 3.6  1996/02/01  06:02:15  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.5  1996/01/29  07:27:32  hampton
 * Remove unnecessary includes of parser definition files.  [CSCdi47717]
 *
 * Revision 3.4  1995/12/27  18:58:51  hou
 * CSCdi45767:  Attempting to configure bridge-gr X sse causes
 *              %SYS-3-REGNONE:Registry 8 service 12 doesn't exist
 *              error message
 * - bgroup_sse parser node is removed from bgroup tree and dynamically
 *   added to the 7000 parser chain.
 *
 * Revision 3.3  1995/12/17  18:27:15  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.2  1995/11/17  09:14:29  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:37:15  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/07/02  10:51:46  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize os, hes, les, if, and ibm.
 *
 * Revision 2.1  1995/06/07  20:37:42  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include "config.h"
#include "parser.h"
#include "logger.h"
#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "../ui/debug.h"
#include "../util/range.h"

#include "../parser/parser_defs_sse.h"
#include "sse_debug.h"
#include "sse.h"

#include "../srt/parser_defs_bridge.h"

/*
 * Parse chains for SHOW BRIDGE commands
 */
#define ALTERNATE NONE
#include "exec_show_bridge_sse.h"
LINK_POINT(sse_show_bridge_commands, ALTERNATE);
#undef	ALTERNATE

/*
 * Parse chains for SSE  commands
 */
#define	ALTERNATE	NONE
#include "cfg_int_bridge_sse.h"
LINK_POINT(bridge_sse_command, ALTERNATE);
#undef	ALTERNATE



/*
 * Parse chain registration array for SSE bridging commands
 */

static parser_extension_request bridge_sse_chain_init_table[] = {
    { PARSE_ADD_SHOW_BRIDGE_CMD, &pname(sse_show_bridge_commands) },
    { PARSE_LIST_END, NULL }
};

/*
 * bridge_sse_chain_init - Initialize parse chains for SSE commands
 * 
 * This routine is called at system startup to register the parse chains
 * for SSE commands.
 */

void bridge_sse_chain_init (void)
{
    parser_add_command_list(bridge_sse_chain_init_table, "bridge_sse");
    parser_add_commands(PARSE_ADD_CFG_INT_BRIDGE,
                        &pname(bridge_sse_command),
                        "sse");
}
