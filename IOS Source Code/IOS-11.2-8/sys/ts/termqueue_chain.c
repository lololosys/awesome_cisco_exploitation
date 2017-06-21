/* $Id: termqueue_chain.c,v 3.5.10.1 1996/05/21 10:07:17 thille Exp $
 * $Source: /release/112/cvs/Xsys/ts/termqueue_chain.c,v $
 *------------------------------------------------------------------
 * Termqueue parse chains
 *
 * May 1993, Robert Widmer
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: termqueue_chain.c,v $
 * Revision 3.5.10.1  1996/05/21  10:07:17  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.5  1996/02/22  14:39:25  widmer
 * CSCdi49281:  make all parser_extension_request static const
 * make parser_extension_request typedef const
 * make all parser_extension_request static
 *
 * Revision 3.4  1996/01/29  07:30:44  hampton
 * Remove unnecessary includes of parser definition files.  [CSCdi47717]
 *
 * Revision 3.3  1995/12/17  18:39:56  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.2  1995/11/17  18:56:19  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:40:09  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/07/04  01:59:07  tli
 * CSCdi36508: Rationalize parser file placement, part I
 * Rationalize cls dev dlsw dspu filesys irix lat les lex llc2 parser pgen
 * pt servers smf smrp snmp src-36-c1000 src-4k-c4000 src-68-c3000
 * src-68-c4000 src-c4000 src-rsp srt tcl ts ui
 *
 * Revision 2.1  1995/06/07  23:10:20  hampton
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
#include "termqueue.h"
#include "../ts/ts_debug.h"


/*
 * Parse chains for Termqueue clear commands
 */
#define	ALTERNATE	NONE
#include "exec_clear_entry.h"
LINK_POINT(termqueue_clear_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chains for Termqueue debug commands
 */
#define	ALTERNATE	NONE
#include "exec_debug_entry.h"
LINK_POINT(termqueue_debug_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chains for Termqueue interface commands
 */
#define	ALTERNATE	NONE
#include "exec_show_entry.h"
LINK_POINT(termqueue_show_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chains for Termqueue config commands
 */
#define	ALTERNATE	NONE
#include "cfg_termqueue.h"
LINK_POINT(termqueue_config_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chain registration array for Termqueue
 */
static parser_extension_request termqueue_chain_init_table[] = {
    { PARSE_ADD_CLEAR_CMD, &pname(termqueue_clear_commands) },
    { PARSE_ADD_DEBUG_CMD, &pname(termqueue_debug_commands) },
    { PARSE_ADD_SHOW_CMD, &pname(termqueue_show_commands) },
    { PARSE_ADD_CFG_ROUTING_CMD, &pname(termqueue_config_commands) },
    { PARSE_LIST_END, NULL }
};


/*
 * termqueue_parser_init - Initialize Termqueue parser support
 */
void termqueue_parser_init (void)
{
    parser_add_command_list(termqueue_chain_init_table, "termqueue");
}
