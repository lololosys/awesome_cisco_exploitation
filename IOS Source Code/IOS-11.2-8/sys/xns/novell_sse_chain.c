/* $Id: novell_sse_chain.c,v 3.4.20.1 1996/05/21 10:11:41 thille Exp $
 * $Source: /release/112/cvs/Xsys/xns/novell_sse_chain.c,v $
 *------------------------------------------------------------------
 * novell_sse_chain.c -- Novell based SSE commands
 *
 * William H. Palmer, December 1993 
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: novell_sse_chain.c,v $
 * Revision 3.4.20.1  1996/05/21  10:11:41  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.4  1996/01/29  07:32:01  hampton
 * Remove unnecessary includes of parser definition files.  [CSCdi47717]
 *
 * Revision 3.3  1995/12/17  18:42:39  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.2  1995/11/17  19:23:16  gstovall
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:59:11  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/07/02  01:43:04  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize xns, atalk, vines, decnet, xremote, x25, unix, wan, tn3270,
 * clns, cdp
 *
 * Revision 2.1  1995/06/07  23:28:49  hampton
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
#include "../hes/sse_debug.h"
#include "../hes/sse.h"


/*
 * Parse chains for CLEAR commands
 */
#define ALTERNATE NONE
#include "exec_clear_novell_sse.h"
LINK_POINT(sse_clear_ipx_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chains for SHOW NOVELL commands
 */
#define ALTERNATE NONE
#include "exec_show_novell_sse.h"
LINK_POINT(sse_show_ipx_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chain registration array for SSE commands
 */

static parser_extension_request ipx_sse_chain_init_table[] = {
    { PARSE_ADD_CLEAR_IPX_CMD, &pname(sse_clear_ipx_commands) },
    { PARSE_ADD_SHOW_IPX_CMD, &pname(sse_show_ipx_commands) },
    { PARSE_LIST_END, NULL }
};

/*
 * ipx_sse_chain_init - Initialize parse chains for SSE commands
 * 
 * This routine is called at system startup to register the parse chains
 * for SSE commands.
 */

void ipx_sse_chain_init (void)
{
    parser_add_command_list(ipx_sse_chain_init_table, "ipx_sse");
}
