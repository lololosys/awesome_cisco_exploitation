/* $Id: mop_chain.c,v 3.5.10.1 1996/05/21 09:44:59 thille Exp $
 * $Source: /release/112/cvs/Xsys/decnet/mop_chain.c,v $
 *------------------------------------------------------------------
 * MOP parse chains
 *
 * May 1993, Robert Widmer
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: mop_chain.c,v $
 * Revision 3.5.10.1  1996/05/21  09:44:59  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.5  1996/02/22  14:32:37  widmer
 * CSCdi49281:  make all parser_extension_request static const
 * make parser_extension_request typedef const
 * make all parser_extension_request static
 *
 * Revision 3.4  1996/02/01  06:00:55  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.3  1995/12/17  18:25:43  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.2  1995/11/17  08:59:27  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:16:56  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/07/02  01:44:32  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize xns, atalk, vines, decnet, xremote, x25, unix, wan, tn3270,
 * clns, cdp
 *
 * Revision 2.1  1995/06/07  20:23:26  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include "config.h"
#include "parser.h"
#include "packet.h"
#include "interface_private.h"
#include "../ui/debug.h"
#include "../if/ether.h"
#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "parser_defs_dec.h"
#include "../decnet/mop.h"
#include "../decnet/mop_debug.h"


/*
 * Parse chains for MOP deubg commands
 */
#define	ALTERNATE	NONE
#include "exec_debug_mop.h"
LINK_POINT(mop_debug_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chains for MOP config commands
 */
#define ALTERNATE       NONE
#include "cfg_mop.h"
LINK_POINT(mop_config_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chains for MOP interface commands
 */
#define	ALTERNATE	NONE
#include "cfg_int_mop.h"
LINK_POINT(mop_interface_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chain registration array for MOP
 */
static parser_extension_request mop_chain_init_table[] = {
    { PARSE_ADD_DEBUG_CMD, &pname(mop_debug_commands) },
    { PARSE_ADD_CFG_ROUTING_CMD, &pname(mop_config_commands) },
    { PARSE_ADD_CFG_INT_AS_CMD, &pname(mop_interface_commands) },
    { PARSE_LIST_END, NULL }
};


/*
 * mop_parser_init - Initialize MOP parser support
 */
void mop_parser_init (void)
{
    parser_add_command_list(mop_chain_init_table, "mop");
}
