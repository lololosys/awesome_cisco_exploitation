/* $Id: clns_chain.c,v 3.6.10.1 1996/05/21 09:43:53 thille Exp $
 * $Source: /release/112/cvs/Xsys/clns/clns_chain.c,v $
 *------------------------------------------------------------------
 * Parse chains for CLNS
 *
 * January 5, 1993, Kevin Paul Herbert
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: clns_chain.c,v $
 * Revision 3.6.10.1  1996/05/21  09:43:53  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.6  1996/02/22  14:32:18  widmer
 * CSCdi49281:  make all parser_extension_request static const
 * make parser_extension_request typedef const
 * make all parser_extension_request static
 *
 * Revision 3.5  1996/02/01  06:00:26  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.4  1996/01/29  07:26:41  hampton
 * Remove unnecessary includes of parser definition files.  [CSCdi47717]
 *
 * Revision 3.3  1995/12/17  18:25:12  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.2  1995/11/17  08:51:52  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:07:24  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.5  1995/10/21  19:02:48  dkatz
 * CSCdi42558:  ISIS needs more debugging hooks
 * Provide a means to look at LSP database internal information.
 * Provide a means to flush the LSP database.
 *
 * Revision 2.4  1995/08/25  14:59:32  dkatz
 * CSCdi38139:  ISIS needs better control of adjacency parameters
 *
 * Revision 2.3  1995/07/02  01:47:00  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize xns, atalk, vines, decnet, xremote, x25, unix, wan, tn3270,
 * clns, cdp
 *
 * Revision 2.2  1995/06/15  15:18:10  widmer
 * CSCdi35889:  parser transition structures should be static
 *
 * Revision 2.1  1995/06/07  20:16:56  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include "clns.h"
#include "interface_private.h"
#include "clns_route.h"
#include "isis.h"
#include "clns_externs.h"
#include "../os/filter.h"
#include "clns_filter.h"
#include "config.h"
#include "parser.h"
#include "../x25/x25.h"
#include "../if/network.h"
#include "../ui/debug.h"
#include "../clns/clns_debug.h"
#include "../iprouting/route_map.h"

#include "../parser/parser_actions.h"
#include "../clns/clns_actions.h"
#include "../parser/macros.h"
#include "../clns/parser_defs_clns.h"
#include "../clns/parser_defs_isis.h"
#include "../parser/parser_defs_parser.h"
#include "../iprouting/parser_defs_route_map.h"


/*
 * Parse chains for CLNS exec commands
 */
#define	ALTERNATE	NONE
#include "exec_which-route.h"
LINK_POINT(clns_exec_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chains for CLEAR commands
 */
#define ALTERNATE NONE
#include "exec_clear_clns.h"
#include "exec_clear_isis.h"
LINK_POINT(clns_clear_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chains for DEBUG commands
 */
#define ALTERNATE NONE
#include "exec_debug_isis.h"
#include "exec_debug_clns.h"
LINK_POINT(clns_debug_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chains for top-level exec commands
 */
#define ALTERNATE NONE
#include "exec_show_isis.h"
#include "exec_show_clns.h"
LINK_POINT(clns_exec_show_commands, ALTERNATE);
#undef ALTERNATE


/*
 * Parse chains for interface commands - after serial-specific parameters
 */
#define ALTERNATE NONE
#include "cfg_int_iso_igrp.h"
#include "cfg_int_isis.h"
#include "cfg_int_clns.h"
LINK_POINT(clns_int_after_serial_commands, ALTERNATE);
#undef ALTERNATE


/*
 * Parse chains for CLNS router configuration commands
 */
#define ALTERNATE NONE
#include "cfg_clns_routing.h"
LINK_POINT(clns_routing_commands, ALTERNATE);
#undef ALTERNATE


/*
 * Parse chains for CLNS router commands
 */
#define ALTERNATE NONE
#include "cfg_router_isis.h"
#include "cfg_router_is-type.h"
#include "cfg_router_net.h"
LINK_POINT(clns_router_commands, ALTERNATE);
#undef ALTERNATE


/*
 * Parse chains for CLNS top-level configuation commands
 */
#define ALTERNATE NONE
#include "cfg_clns.h"
LINK_POINT(clns_config_commands, ALTERNATE);
#undef ALTERNATE


/*
 * Parse chains for CLNS ip interface commands
 */
#define	ALTERNATE	NONE
#include "cfg_int_ip_clns.h"
LINK_POINT(clns_ipinterface_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chains for CLNS ping command
 */
LINK_EXIT(exec_ping_clns_exit, no_alt);
KEYWORD_ID(exec_ping_clns, exec_ping_clns_exit, NONE,
	   OBJ(int,1), LINK_CLNS, "clns", "CLNS echo", PRIV_USER);
LINK_POINT(clns_ping_commands, exec_ping_clns);


/*
 * Parse chains for CLNS specific route map match commands
 */
#define	ALTERNATE	NONE
#include "../clns/cfg_routemap_match_clns.h"
LINK_POINT(clns_routemap_match_commands, ALTERNATE);
#undef	ALTERNATE

#define	ALTERNATE	NONE
#include "../clns/cfg_routemap_set_clns.h"
LINK_POINT(clns_routemap_set_commands, ALTERNATE);
#undef	ALTERNATE

/*
 * Parse chain registration array for CLNS
 */

static parser_extension_request clns_chain_init_table[] = {
    { PARSE_ADD_EXEC_CMD, &pname(clns_exec_commands) },
    { PARSE_ADD_CLEAR_CMD, &pname(clns_clear_commands) },
    { PARSE_ADD_DEBUG_CMD, &pname(clns_debug_commands) },
    { PARSE_ADD_SHOW_CMD, &pname(clns_exec_show_commands) },
    { PARSE_ADD_CFG_INT_AS_CMD, &pname(clns_int_after_serial_commands) },
    { PARSE_ADD_CFG_ROUTING_CMD, &pname(clns_routing_commands) },
    { PARSE_ADD_CFG_ROUTER_CMD, &pname(clns_router_commands) },
    { PARSE_ADD_CFG_TOP_CMD, &pname(clns_config_commands) },
    { PARSE_ADD_CFG_INT_IP_CMD, &pname(clns_ipinterface_commands) },
    { PARSE_ADD_PING_CMD, &pname(clns_ping_commands) },
    { PARSE_ADD_PING_EXIT, 
	  (dynamic_transition *) &pname(exec_ping_clns_exit) },
    { PARSE_ADD_CFG_RTMAP_SET_EXIT,
	  (dynamic_transition *) &pname(routemap_clnp_exit) },
    { PARSE_ADD_CFG_RTMAP_MATCH_EXIT,
	  (dynamic_transition *) &pname(routemap_match_clnp_exit) },
    { PARSE_LIST_END, NULL }
};

/*
 * clns_parser_init - Initialize parse chains and address parsing for CLNS
 * 
 * This routine is called at system startup to register the parse chains
 * for CLNS.
 */

void clns_parser_init (void)
{
    parser_add_protoaddr(LINK_CLNS,		PMATCH_CLNS PMATCH_DGRAM,
			 "clns",		"ISO CLNS",
			 0, ADDR_ILLEGAL);
    parser_add_protoaddr(LINK_CLNS_ALL_ES,	PMATCH_CLNS_ALL_ES,
			 "clns_es",		"ISO CLNS End System",
			 5, ADDR_ILLEGAL);
    parser_add_protoaddr(LINK_CLNS_ALL_IS,	PMATCH_CLNS_ALL_IS,
			 "clns_is",	       "ISO CLNS Intermediate System",
			 5, ADDR_ILLEGAL);
    parser_add_protoaddr(LINK_CMNS,		PMATCH_CMNS,
			 "cmns",		"ISO CMNS",
			 0, ADDR_CLNS);
    parser_add_address(ADDR_CLNS, clns_addr_short_help, match_clns_addr);
    parser_add_link_point(PARSE_ADD_SHOW_CLNS_CMD, "show clns",
			  &pname(show_clns_extend_here));
    parser_add_command_list(clns_chain_init_table, "clns");
    parser_add_router(RMATCH_OSI, parser_clns_router);
    if (parser_link_point_exists(PARSE_ADD_CFG_ROUTEMAP_MATCH_CMD)) {
	parser_add_commands(PARSE_ADD_CFG_ROUTEMAP_MATCH_CMD,
			    &pname(clns_routemap_match_commands), "clns");
    }
    if (parser_link_point_exists(PARSE_ADD_CFG_ROUTEMAP_SET_CMD)) {
	parser_add_commands(PARSE_ADD_CFG_ROUTEMAP_SET_CMD,
			    &pname(clns_routemap_set_commands), "clns");
    }
}
