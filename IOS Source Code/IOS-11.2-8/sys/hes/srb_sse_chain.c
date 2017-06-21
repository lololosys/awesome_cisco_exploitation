/* $Id: srb_sse_chain.c,v 3.5.10.1 1996/05/21 09:48:11 thille Exp $
 * $Source: /release/112/cvs/Xsys/hes/srb_sse_chain.c,v $
 *------------------------------------------------------------------
 * srb_sse_chain.c -- UI code for doing SRB on the SSE
 *
 * Feb 1994 Tony Li
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: srb_sse_chain.c,v $
 * Revision 3.5.10.1  1996/05/21  09:48:11  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.5  1996/02/22  14:33:24  widmer
 * CSCdi49281:  make all parser_extension_request static const
 * make parser_extension_request typedef const
 * make all parser_extension_request static
 *
 * Revision 3.4  1996/01/29  07:27:44  hampton
 * Remove unnecessary includes of parser definition files.  [CSCdi47717]
 *
 * Revision 3.3  1995/12/17  18:27:36  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.2  1995/11/17  09:20:39  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:42:48  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/07/02  10:52:01  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize os, hes, les, if, and ibm.
 *
 * Revision 2.1  1995/06/07  20:44:42  hampton
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


/*
 * Parse chains for SHOW SOURCE BRIDGE commands
 */
#define ALTERNATE NONE
#include "exec_show_srb_sse.h"
LINK_POINT(sse_show_srb_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chain registration array for SSE bridging commands
 */

static parser_extension_request srb_sse_chain_init_table[] = {
    { PARSE_ADD_SHOW_SRB_CMD, &pname(sse_show_srb_commands) },
    { PARSE_LIST_END, NULL }
};

/*
 * bridge_sse_chain_init - Initialize parse chains for SSE commands
 * 
 * This routine is called at system startup to register the parse chains
 * for SSE commands.
 */

void srb_sse_chain_init (void)
{
    parser_add_command_list(srb_sse_chain_init_table, "srb_sse");
}
