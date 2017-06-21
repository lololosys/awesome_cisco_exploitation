/* $Id: cdp_chain.c,v 3.6.10.1 1996/05/21 09:43:33 thille Exp $:
 * $Source: /release/112/cvs/Xsys/cdp/cdp_chain.c,v $
 *------------------------------------------------------------------
 * Parse chains for CDP.
 *
 * October 1994, Arun Sastry
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cdp_chain.c,v $
 * Revision 3.6.10.1  1996/05/21  09:43:33  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.6  1996/02/22  14:32:15  widmer
 * CSCdi49281:  make all parser_extension_request static const
 * make parser_extension_request typedef const
 * make all parser_extension_request static
 *
 * Revision 3.5  1996/02/01  06:00:22  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.4  1996/01/29  07:26:36  hampton
 * Remove unnecessary includes of parser definition files.  [CSCdi47717]
 *
 * Revision 3.3  1995/12/17  18:24:58  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.2  1995/11/17  08:49:43  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:05:08  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/07/02  01:47:19  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize xns, atalk, vines, decnet, xremote, x25, unix, wan, tn3270,
 * clns, cdp
 *
 * Revision 2.1  1995/06/07  20:14:59  hampton
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
#include "../if/network.h"
#include "cdp.h"
#include "cdp_debug.h"

#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "parser_defs_cdp.h"
#include "../parser/parser_defs_parser.h"


/*
 * Parse chains for CLEAR commands
 */
#define ALTERNATE NONE
#include "exec_clear_cdp.h"
LINK_POINT(cdp_clear_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chains for CDP deubg commands
 */
#define	ALTERNATE	NONE
#include "exec_debug_cdp.h"
LINK_POINT(cdp_debug_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chains for CDP config commands
 */
#define ALTERNATE       NONE
#include "cfg_cdp.h"
LINK_POINT(cdp_config_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chains for top-level exec commands
 */
#define ALTERNATE NONE
#include "exec_show_cdp.h"
LINK_POINT(cdp_exec_show_commands, ALTERNATE);
#undef ALTERNATE


/*
 * Parse chains for CDP interface commands
 */
#define	ALTERNATE	NONE
#include "cfg_int_cdp.h"
LINK_POINT(cdp_interface_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chain registration array for CDP
 */
static parser_extension_request cdp_chain_init_table[] = {
    { PARSE_ADD_CLEAR_CMD, &pname(cdp_clear_commands) },
    { PARSE_ADD_DEBUG_CMD, &pname(cdp_debug_commands) },
    { PARSE_ADD_CFG_ROUTING_CMD, &pname(cdp_config_commands) },
    { PARSE_ADD_SHOW_CMD, &pname(cdp_exec_show_commands) },
    { PARSE_ADD_CFG_INT_AS_CMD, &pname(cdp_interface_commands) },
    { PARSE_LIST_END, NULL }
};


/*
 * cdp_parser_init - Initialize CDP parser support
 */
void cdp_parser_init (void)
{
    parser_add_link_point(PARSE_ADD_CFG_CDP, "cdp",
			  &pname(conf_cdp_extend_here));
    parser_add_protoaddr(LINK_CDP,	PMATCH_CDP,
 			 "cdp",	"Cisco Discovery Protocol",
 			 2, ADDR_ILLEGAL);
    parser_add_command_list(cdp_chain_init_table, "cdp");

}

