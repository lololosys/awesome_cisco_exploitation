/* $Id: fast_ethernet_chain.c,v 3.7.10.1 1996/05/21 09:49:47 thille Exp $
 * $Source: /release/112/cvs/Xsys/if/fast_ethernet_chain.c,v $
 *------------------------------------------------------------------
 * Fast Ethernet parse chain registration routines
 *
 * November 1994, Gary Kramling
 * Adapted from channel_chain.c by William H. Palmer
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Dynamically linked parse chains for fast ethernet interface. 
 *------------------------------------------------------------------
 * $Log: fast_ethernet_chain.c,v $
 * Revision 3.7.10.1  1996/05/21  09:49:47  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.7  1996/02/22  14:34:01  widmer
 * CSCdi49281:  make all parser_extension_request static const
 * make parser_extension_request typedef const
 * make all parser_extension_request static
 *
 * Revision 3.6  1996/02/13  21:25:44  rbatz
 * CSCdi30258:  Fixed the following:
 *                  Duplicate half-duplex commands.
 *                  Could not do half-duplex stun.
 *                  Could not tune half-duplex timers for bstun.
 *
 * Revision 3.5  1996/02/01  06:03:57  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.4  1996/01/29  07:28:18  hampton
 * Remove unnecessary includes of parser definition files.  [CSCdi47717]
 *
 * Revision 3.3  1995/12/17  18:28:46  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.2  1995/11/17  09:27:27  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:49:31  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/08/09  00:20:09  kramling
 * CSCdi38439:  FEIP product is now supported in 11.0.  Update support to
 * 10.3.
 *
 * Revision 2.2  1995/07/02  10:53:27  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize os, hes, les, if, and ibm.
 *
 * Revision 2.1  1995/06/07  20:51:53  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include "interface_private.h"
#include "config.h"
#include "parser.h"
#include "../ui/debug.h"
#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "parser_defs_fast_ethernet.h"
#include "fast_ethernet_debug.h"


/*
 * Parse chains for fast ethernet debug commands 
 */
#define ALTERNATE	NONE
#include "exec_debug_fast_ethernet.h"
LINK_POINT(fast_ethernet_debug_commands, ALTERNATE);
#undef  ALTERNATE


/*
 * Parse chain registration array for fast ethernet
 */
static parser_extension_request fast_ethernet_chain_init_table[] = {
    { PARSE_ADD_DEBUG_CMD, &pname(fast_ethernet_debug_commands) },
    { PARSE_LIST_END, NULL }
};


/*
 * fast_ethernet_parser_init - Initialize fast ethernet parser support
 */
void fast_ethernet_parser_init (void)
{
    static boolean fast_ethernet_parser_initialized = FALSE;

    if (!fast_ethernet_parser_initialized) {
	parser_add_command_list(fast_ethernet_chain_init_table, 
				"fastethernet");
	fast_ethernet_parser_initialized = TRUE;
    }
}
