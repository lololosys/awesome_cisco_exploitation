/* $Id: bgp_chain.c,v 3.6.10.3 1996/05/21 09:53:10 thille Exp $
 * $Source: /release/112/cvs/Xsys/iprouting/bgp_chain.c,v $
 *------------------------------------------------------------------
 * BGP parse chains
 *
 * May 1993, Robert Widmer
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: bgp_chain.c,v $
 * Revision 3.6.10.3  1996/05/21  09:53:10  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.6.10.2  1996/05/04  01:21:03  wilber
 * Branch: California_branch
 *
 * Constrained Multicast Flooding
 * - Define MOSPF hello option bit.
 *
 * Name Access List
 *
 * Revision 3.6.10.1  1996/04/16  19:01:16  rchandra
 * CSCdi54830:  IP features commit
 * Branch: California_branch
 *
 * Revision 3.6  1996/02/22  14:35:01  widmer
 * CSCdi49281:  make all parser_extension_request static const
 * make parser_extension_request typedef const
 * make all parser_extension_request static
 *
 * Revision 3.5  1996/02/01  06:05:07  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.4  1996/01/29  07:28:42  hampton
 * Remove unnecessary includes of parser definition files.  [CSCdi47717]
 *
 * Revision 3.3  1995/12/17  18:30:53  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.2  1995/11/17  17:31:18  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:04:24  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.5  1995/09/25  08:30:49  tli
 * CSCdi40960:  ISP jumbo patch 2
 *
 * Revision 2.4  1995/07/01  12:15:31  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize iprouting
 *
 * Revision 2.3  1995/06/30  20:25:10  rchandra
 * CSCdi36620:  BGP requires IBGP neighbors to be fully meshed
 *
 * Revision 2.2  1995/06/15  15:18:42  widmer
 * CSCdi35889:  parser transition structures should be static
 *
 * Revision 2.1  1995/06/07  21:05:20  hampton
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
#include "access.h"
#include "interface_private.h"
#include "login.h"
#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "../parser/parser_defs_tcpip.h"
#include "parser_defs_router.h"
#include "parser_defs_community.h"
#include "../parser/parser_defs_parser.h"
#include "route.h"
#include "bgp.h"
#include "iprouting_debug.h"
#include "access.h"
#include "../ip/ipaccess.h"
#include "bgp_private.h"
#include "community.h"
#include "route_map.h"


/*
 * Parse chains for BGP clear commands
 */
#define	ALTERNATE	NONE
#include "exec_clear_ip_bgp.h"
LINK_POINT(bgp_clear_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chains for BGP debug commands
 */
#define	ALTERNATE	NONE
#include "exec_debug_ip_bgp.h"
LINK_POINT(bgp_debug_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chains for BGP show commands
 */
#define ALTERNATE       NONE
#include "exec_show_ip_bgp.h"
#include "exec_show_ip_as-path_access-list.h"
#include "exec_show_ip_community-list.h"
LINK_POINT(bgp_show_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chains for BGP configuration commands
 */
#define ALTERNATE       NONE
#include "cfg_ip_as-path_access-list.h"
#include "cfg_ip_community-list.h"
LINK_POINT(bgp_ip_config_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chains for BGP specific router subcommands
 */
#define ALTERNATE       NONE
#include "cfg_router_aggregate_addr.h"
#include "cfg_router_network_bgp.h"
#include "cfg_router_bgp.h"
#include "cfg_router_table-map.h"
#include "cfg_router_synchronization.h"
LINK_POINT(bgp_router_commands, ALTERNATE);
#undef	ALTERNATE

/*
 * Parse chains for BGP specific router neighbor subcommands
 */
#define	ALTERNATE	NONE
#include "cfg_router_neighbor_bgp.h"
LINK_POINT(bgp_router_neighbor_commands, ALTERNATE);
#undef	ALTERNATE

/*
 * Parse chains for BGP specific router timer subcommands
 */
#define	ALTERNATE	NONE
#include "cfg_router_timers_bgp.h"
LINK_POINT(bgp_router_timer_commands, ALTERNATE);
#undef	ALTERNATE

/*
 * Parse chains for BGP specific router distance subcommands
 */
#define	ALTERNATE	NONE
#include "cfg_router_distance_bgp.h"
LINK_POINT(bgp_router_distance_commands, ALTERNATE);
#undef	ALTERNATE

/*
 * Parse chains for BGP specific route map set commands
 */
#define	ALTERNATE	NONE
#include "../iprouting/cfg_routemap_set_bgp.h"
LINK_POINT(bgp_routemap_set_commands, ALTERNATE);
#undef	ALTERNATE

/*
 * Parse chains for BGP specific route map match commands
 */
#define	ALTERNATE	NONE
#include "../iprouting/cfg_routemap_match_bgp.h"
LINK_POINT(bgp_routemap_match_commands, ALTERNATE);
#undef	ALTERNATE

/*
 * Parse chain registration array for BGP
 */
static parser_extension_request bgp_chain_init_table[] = {
    { PARSE_ADD_CLEAR_IP_CMD, &pname(bgp_clear_commands) },
    { PARSE_ADD_DEBUG_IP_CMD, &pname(bgp_debug_commands) },
    { PARSE_ADD_SHOW_IP_CMD, &pname(bgp_show_commands) },
    { PARSE_ADD_CFG_RTG_IP_CMD, &pname(bgp_ip_config_commands) },
    { PARSE_ADD_CFG_ROUTER_INITIAL_CMD, &pname(bgp_router_commands) },
    { PARSE_ADD_CFG_ROUTER_NEIGHBOR_CMD,
	  &pname(bgp_router_neighbor_commands) }, 
    { PARSE_ADD_CFG_ROUTER_TIMER_CMD, &pname(bgp_router_timer_commands) },
    { PARSE_ADD_CFG_ROUTER_DISTANCE_CMD,
	  &pname(bgp_router_distance_commands) },
    { PARSE_ADD_CFG_ROUTEMAP_SET_CMD, &pname(bgp_routemap_set_commands) },
    { PARSE_ADD_CFG_ROUTEMAP_MATCH_CMD, &pname(bgp_routemap_match_commands) },
    { PARSE_ADD_CFG_RTMAP_SET_EXIT,
	  (dynamic_transition *) &pname(routemap_set_bgp_exit) },
    { PARSE_ADD_CFG_RTMAP_MATCH_EXIT,
	  (dynamic_transition *) &pname(routemap_match_bgp_exit) },
    { PARSE_LIST_END, NULL }
};


/*
 * bgp_parser_init - Initialize BGP parser support
 */
void bgp_parser_init (void)
{
    parser_add_command_list(bgp_chain_init_table, "bgp");
}
