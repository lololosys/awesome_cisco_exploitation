/* $Id: old_timer_callbacks_chain.c,v 3.4.10.1 1996/05/21 09:58:20 thille Exp $
 * $Source: /release/112/cvs/Xsys/os/old_timer_callbacks_chain.c,v $
 *------------------------------------------------------------------
 * Timers parser chains
 *
 * May 1993, Robert Widmer
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: old_timer_callbacks_chain.c,v $
 * Revision 3.4.10.1  1996/05/21  09:58:20  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.4  1996/02/22  14:36:46  widmer
 * CSCdi49281:  make all parser_extension_request static const
 * make parser_extension_request typedef const
 * make all parser_extension_request static
 *
 * Revision 3.3  1996/01/29  07:29:15  hampton
 * Remove unnecessary includes of parser definition files.  [CSCdi47717]
 *
 * Revision 3.2  1996/01/05  10:18:03  hampton
 * Move/rename the files containing the deprecated timer callback
 * routines.  [CSCdi46482]
 *
 * Revision 3.1  1996/01/05  09:39:41  hampton
 * New files for old timer code.
 *
 * Revision 3.4  1995/12/17  18:34:26  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.3  1995/11/17  18:52:26  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:19:12  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:48:36  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/07/02  10:51:20  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize os, hes, les, if, and ibm.
 *
 * Revision 2.1  1995/06/07  21:59:23  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include "config.h"
#include "parser.h"
#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "old_timer_callbacks.h"
#include "old_timer_callbacks_private.h"


/*
 * Parse chains for Timers exec commands
 */
#define	ALTERNATE	NONE
#include "exec_show_timers.h"
LINK_POINT(timers_show_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chain registration array for Timers
 */
static parser_extension_request timers_chain_init_table[] = {
    { PARSE_ADD_SHOW_CMD, &pname(timers_show_commands) },
    { PARSE_LIST_END, NULL }
};


/*
 * timers_parser_init - Initialize Timers parser support
 */
void timers_parser_init (void)
{
    parser_add_command_list(timers_chain_init_table, "timers");
}
