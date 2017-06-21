/* $Id: arp_chain.c,v 3.5.10.1 1996/05/21 09:49:37 thille Exp $
 * $Source: /release/112/cvs/Xsys/if/arp_chain.c,v $
 *------------------------------------------------------------------
 * arp_chain.c - Parse chains for ARP support
 *
 * December 1994, Scott Mackie
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: arp_chain.c,v $
 * Revision 3.5.10.1  1996/05/21  09:49:37  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.5  1996/02/22  14:33:54  widmer
 * CSCdi49281:  make all parser_extension_request static const
 * make parser_extension_request typedef const
 * make all parser_extension_request static
 *
 * Revision 3.4  1996/01/29  07:28:08  hampton
 * Remove unnecessary includes of parser definition files.  [CSCdi47717]
 *
 * Revision 3.3  1995/12/17  18:28:35  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.2  1995/11/17  09:25:16  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:47:23  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/07/02  10:53:03  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize os, hes, les, if, and ibm.
 *
 * Revision 2.1  1995/06/07  20:49:55  hampton
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
#include "arp_debug.h"


/*
 * Parse chains for ARP clear command
 */
#define	ALTERNATE	NONE
#include "exec_clear_arp.h"
LINK_POINT(arp_clear_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chains for ARP debug commands
 */
#define	ALTERNATE	NONE
#include "exec_debug_arp.h"
LINK_POINT(arp_debug_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chain registration array for ARP
 */
static parser_extension_request arp_chain_init_table[] = {
    { PARSE_ADD_CLEAR_CMD, &pname(arp_clear_commands) },
    { PARSE_ADD_DEBUG_CMD, &pname(arp_debug_commands) },
    { PARSE_LIST_END, NULL }
};


/*
 * arp_parser_init - Initialize ARP parser support
 */
void arp_parser_init (void)
{
    parser_add_command_list(arp_chain_init_table, "arp");
}
