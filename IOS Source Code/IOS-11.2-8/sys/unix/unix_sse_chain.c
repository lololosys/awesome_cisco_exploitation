/* $Id: unix_sse_chain.c,v 3.4.20.1 1996/05/21 10:08:04 thille Exp $
 * $Source: /release/112/cvs/Xsys/unix/unix_sse_chain.c,v $
 *------------------------------------------------------------------
 * unix_sse_chain.c - Registration for UNIX SSE commands
 *
 * Nov 1994 Tony Li
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: unix_sse_chain.c,v $
 * Revision 3.4.20.1  1996/05/21  10:08:04  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.4  1996/01/29  07:30:52  hampton
 * Remove unnecessary includes of parser definition files.  [CSCdi47717]
 *
 * Revision 3.3  1995/12/17  18:40:31  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.2  1995/11/17  17:55:53  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:42:49  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/07/02  01:45:29  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize xns, atalk, vines, decnet, xremote, x25, unix, wan, tn3270,
 * clns, cdp
 *
 * Revision 2.1  1995/06/07  23:12:31  hampton
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


/*
 * Parse chains for top-level exec commands
 */
#define ALTERNATE NONE
#include "exec_test_sse_unix.h"
LINK_POINT(unix_sse_test_commands, ALTERNATE);
#undef ALTERNATE

/*
 * Parse chain registration array for SSE commands
 */

static parser_extension_request unix_sse_chain_init_table[] = {
    { PARSE_ADD_TEST_SSE_CMD, &pname(unix_sse_test_commands) },
    { PARSE_LIST_END, NULL }
};

/*
 * unix_sse_chain_init - Initialize parse chains for SSE commands
 * 
 * This routine is called at system startup to register the parse chains
 * for SSE commands.
 */

void unix_sse_chain_init (void)
{
    parser_add_command_list(unix_sse_chain_init_table, "unix_sse");
}
