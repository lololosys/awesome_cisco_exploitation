/* $Id: decnet_chain.c,v 3.8.4.2 1996/05/30 23:42:49 hampton Exp $
 * $Source: /release/112/cvs/Xsys/decnet/decnet_chain.c,v $
 *------------------------------------------------------------------
 * Parse chains for DECnet
 *
 * January 5, 1993, Kevin Paul Herbert
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: decnet_chain.c,v $
 * Revision 3.8.4.2  1996/05/30  23:42:49  hampton
 * All commands that take access list numbers should accept either numbers
 * in the current monolithic number space, or hidden numbers in the future
 * per-protocol access list spaces.  [CSCdi59067]
 * Branch: California_branch
 *
 * Revision 3.8.4.1  1996/05/21  09:44:54  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.8  1996/03/09  22:54:31  widmer
 * CSCdi47890:  HTTP server doesnt work
 * Add HTTP server
 *
 * Revision 3.7  1996/02/22  14:32:33  widmer
 * CSCdi49281:  make all parser_extension_request static const
 * make parser_extension_request typedef const
 * make all parser_extension_request static
 *
 * Revision 3.6  1996/02/01  06:00:46  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.5  1996/01/29  07:27:10  hampton
 * Remove unnecessary includes of parser definition files.  [CSCdi47717]
 *
 * Revision 3.4  1995/12/17  18:25:41  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.3  1995/12/01  15:56:07  widmer
 * CSCdi44724:  Remove nv_top argument to parser_add_mode()
 * Clean up "no" handling at beginning of modes
 * Fix appn modes
 * Removed unneded pdecl's
 *
 * Revision 3.2  1995/11/17  08:58:43  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:15:46  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/07/02  01:44:27  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize xns, atalk, vines, decnet, xremote, x25, unix, wan, tn3270,
 * clns, cdp
 *
 * Revision 2.2  1995/06/15  15:18:12  widmer
 * CSCdi35889:  parser transition structures should be static
 *
 * Revision 2.1  1995/06/07  20:22:40  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include "interface_private.h"
#include "packet.h"
#include "address.h"
#include "../ui/debug.h"
#include "decnet.h"
#include "decnet_debug.h"
#include "config.h"
#include "parser.h"
#include "../if/network.h"

#include "../parser/parser_actions.h"
#include "../decnet/decnet_actions.h"
#include "../parser/macros.h"
#include "parser_defs_dec.h"
#include "../parser/parser_defs_parser.h"


/*
 * Parse chains for CLEAR commands
 */
#define ALTERNATE NONE
#include "exec_clear_decnet.h"
LINK_POINT(decnet_clear_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chains for DEBUG commands
 */
#define ALTERNATE NONE
#include "exec_debug_decnet.h"
LINK_POINT(decnet_debug_commands, ALTERNATE);
#undef ALTERNATE


/*
 * Parse chains for top-level exec commands
 */
#define ALTERNATE NONE
#include "exec_show_decnet.h"
LINK_POINT(decnet_exec_show_commands, ALTERNATE);
#undef ALTERNATE


/*
 * Parse chains for interface commands - after serial-specific parameters
 */
#define ALTERNATE NONE
#include "cfg_int_decnet.h"
LINK_POINT(decnet_int_commands, ALTERNATE);
#undef ALTERNATE


/*
 * Parse chains for DECnet top-level configuation commands
 */
#define ALTERNATE NONE
#include "cfg_decnet.h"
LINK_POINT(decnet_config_commands, ALTERNATE);
#undef ALTERNATE



/*
 * Parse chains for DECnet access-list commands
 */
#define	ALTERNATE	NONE
#define	OALTERNATE	NONE
#include "cfg_acclist_decnet.h"
LINK_POINT(decnet_acclist_commands, ALTERNATE);
LINK_POINT(odecnet_acclist_commands, OALTERNATE);	/* Old form */
#undef OALTERNATE
#undef ALTERNATE



/*
 * Parse chains for DECnet router configuration commands
 */

#define ALTERNATE NONE
#include "cfg_decnet_routing.h"
LINK_POINT(decnet_routing_commands, ALTERNATE);
#undef ALTERNATE


/*
 * Parse chains for ping command
 */
LINK_EXIT(exec_ping_decnet_exit, no_alt);
KEYWORD_ID(exec_ping_decnet, exec_ping_decnet_exit, NONE,
	   OBJ(int,1), LINK_DECNET, "decnet", "DECnet echo", PRIV_USER);
LINK_POINT(decnet_ping_command, exec_ping_decnet);


/*
 * Parse chain registration array for DECnet
 */

static parser_extension_request decnet_chain_init_table[] = {
    { PARSE_ADD_CLEAR_CMD, &pname(decnet_clear_commands) },
    { PARSE_ADD_DEBUG_CMD, &pname(decnet_debug_commands) },
    { PARSE_ADD_SHOW_CMD, &pname(decnet_exec_show_commands) },
    { PARSE_ADD_CFG_INT_CMD, &pname(decnet_int_commands) },
    { PARSE_ADD_CFG_TOP_CMD, &pname(decnet_config_commands) },
    { PARSE_ADD_CFG_DECNET_ROUTING, &pname(decnet_acclist_commands) },
    { PARSE_ADD_CFG_ACCLIST_CMD, &pname(odecnet_acclist_commands) }, /* Old */
    { PARSE_ADD_CFG_ROUTING_CMD, &pname(decnet_routing_commands) },
    { PARSE_ADD_PING_CMD, &pname(decnet_ping_command) },
    { PARSE_ADD_PING_EXIT,
	  (dynamic_transition *) &pname(exec_ping_decnet_exit) },
    { PARSE_LIST_END, NULL }
};

/*
 * decnet_parser_init - Initialize parse chains for DECnet
 * 
 * This routine is called at system startup to register the parse chains
 * for DECnet.
 */

void decnet_parser_init (void)
{
    parser_add_link_point(PARSE_ADD_CFG_DECNET, "decnet",
			  &pname(conf_decnet_extend_here));
    parser_add_link_point(PARSE_ADD_CFG_DECNET_ROUTING, "decnet routing",
			  &pname(conf_decnet_routing_extend_here));
    parser_add_protoaddr(LINK_DECNET,		PMATCH_DECNET PMATCH_DGRAM,
			 "decnet",		"DECnet",
			 0, ADDR_DECNET);
    parser_add_protoaddr(LINK_DECNET_NODE,	PMATCH_DECNET_NODE,
			 "decnet_node",		"DECnet Node",
			 7, ADDR_ILLEGAL);
    parser_add_protoaddr(LINK_DECNET_ROUTER_L1,	PMATCH_DECNET_ROUTER_L1,
 			 "decnet_router-l1",	"DECnet Router L1",
 			 7, ADDR_ILLEGAL);
    parser_add_protoaddr(LINK_DECNET_ROUTER_L2,	PMATCH_DECNET_ROUTER_L2,
 			 "decnet_router-l2",	"DECnet Router L2",
 			 7, ADDR_ILLEGAL);
    parser_add_protoaddr(LINK_DECNET_PRIME_ROUTER, PMATCH_DECNET_PRIME_ROUTER,
			 "decnet_prime_router",	"DECnet Prime Router",
			 7, ADDR_ILLEGAL);
    parser_add_address(ADDR_DECNET, decnet_addr_short_help, match_dn_addr);
    parser_add_command_list(decnet_chain_init_table, "decnet");
    parser_add_mode("decnet-map", "config-decmap",
		    "DECnet map configuration mode",
		    FALSE, FALSE, "configure", NULL, NULL,
		    &pname(conf_decnet_map_fromaddr),
		    NULL);
}
