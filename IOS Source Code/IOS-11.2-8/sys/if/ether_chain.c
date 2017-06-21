/* $Id: ether_chain.c,v 3.5.10.1 1996/05/21 09:49:43 thille Exp $
 * $Source: /release/112/cvs/Xsys/if/ether_chain.c,v $
 *------------------------------------------------------------------
 * ether_chain.c - Parse chains for Ethernet support
 *
 * December 1994, Scott Mackie
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ether_chain.c,v $
 * Revision 3.5.10.1  1996/05/21  09:49:43  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.5  1996/02/22  14:34:00  widmer
 * CSCdi49281:  make all parser_extension_request static const
 * make parser_extension_request typedef const
 * make all parser_extension_request static
 *
 * Revision 3.4  1996/01/29  07:28:17  hampton
 * Remove unnecessary includes of parser definition files.  [CSCdi47717]
 *
 * Revision 3.3  1995/12/17  18:28:46  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.2  1995/11/17  09:26:56  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:48:53  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/07/02  10:53:17  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize os, hes, les, if, and ibm.
 *
 * Revision 2.1  1995/06/07  20:51:40  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include "interface_private.h"
#include "config.h"
#include "../ui/debug.h"
#include "parser.h"
#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "ether_debug.h"


/*
 * Parse chains for Ethernet debug commands
 */
#define	ALTERNATE	NONE
#include "exec_debug_ethernet.h"
LINK_POINT(ether_debug_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chain registration array for Ethernet
 */
static parser_extension_request ether_chain_init_table[] = {
    { PARSE_ADD_DEBUG_CMD, &pname(ether_debug_commands) },
    { PARSE_LIST_END, NULL }
};


/*
 * ether_parser_init - Initialize Ethernet parser support
 */
void ether_parser_init (void)
{
    parser_add_command_list(ether_chain_init_table, "ethernet");
}
