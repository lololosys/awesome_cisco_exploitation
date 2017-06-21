/* $Id: iprouting_chain.c,v 3.8.4.4 1996/06/17 23:33:58 hampton Exp $
 * $Source: /release/112/cvs/Xsys/iprouting/iprouting_chain.c,v $
 *------------------------------------------------------------------
 * IP routing parse chains
 *
 * May 1993, Robert Widmer
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: iprouting_chain.c,v $
 * Revision 3.8.4.4  1996/06/17  23:33:58  hampton
 * Implement the long sought parser "default" keyword.  [CSCdi60510]
 * Branch: California_branch
 *
 * Revision 3.8.4.3  1996/05/21  09:53:22  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.8.4.2  1996/04/16  19:01:33  rchandra
 * CSCdi54830:  IP features commit
 * Branch: California_branch
 *
 * Revision 3.8.4.1  1996/03/18  20:28:12  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.4.14.2  1996/03/16  06:57:03  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.4.14.1  1996/02/20  01:03:22  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.8  1996/03/09  22:56:02  widmer
 * CSCdi47890:  HTTP server doesnt work
 * Add HTTP server
 *
 * Revision 3.7  1996/02/22  14:35:17  widmer
 * CSCdi49281:  make all parser_extension_request static const
 * make parser_extension_request typedef const
 * make all parser_extension_request static
 *
 * Revision 3.6  1996/02/01  06:05:41  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.5  1996/01/29  07:28:48  hampton
 * Remove unnecessary includes of parser definition files.  [CSCdi47717]
 *
 * Revision 3.4  1995/12/17  18:31:00  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.3  1995/12/01  15:56:30  widmer
 * CSCdi44724:  Remove nv_top argument to parser_add_mode()
 * Clean up "no" handling at beginning of modes
 * Fix appn modes
 * Removed unneded pdecl's
 *
 * Revision 3.2  1995/11/17  17:34:42  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:08:32  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.8  1995/11/08  21:06:27  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.7  1995/08/25  18:44:57  dkatz
 * CSCdi38487:  ISIS LSP MTU needs to be settable
 * Missed one file on the commit.
 *
 * Revision 2.6  1995/07/13  21:39:06  ahh
 * CSCdi36962:  UDP parsing of tacacs-ds port name broken
 * Restore "tacacs-ds" as a keyword, but hide it and map it to TACACS_PORT
 * for backwards compatibility.
 *
 * Revision 2.5  1995/07/01  12:16:28  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize iprouting
 *
 * Revision 2.4  1995/06/30  20:25:13  rchandra
 * CSCdi36620:  BGP requires IBGP neighbors to be fully meshed
 *
 * Revision 2.3  1995/06/15  15:18:50  widmer
 * CSCdi35889:  parser transition structures should be static
 *
 * Revision 2.2  1995/06/08  15:55:03  widmer
 * CSCdi35497:  Remove old style debugging
 *
 * Revision 2.1  1995/06/07  21:07:38  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include <stdlib.h>		/* for atoi */
#include <ciscolib.h>
#include "interface_private.h"
#include "config.h"
#include "parser.h"
#include "access.h"
#include "login.h"
#include "../ui/debug.h"
#include "route.h"
#include "bgp.h"
#include "iprouting_debug.h"
#include "route_map.h"
#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "../parser/parser_defs_parser.h"
#include "../parser/parser_defs_tcpip.h"
#include "parser_defs_iprouting.h"
#include "parser_defs_ospf.h"
#include "parser_defs_route_map.h"
#include "parser_defs_standby.h"
#include "../clns/parser_defs_isis.h"
#include "parser_defs_router.h"
#include "../clns/clns_route.h"
#include "../clns/isis.h"
#include "../ip/ip_actions.h"
#include "../ip/ip_macros.h"
#include "../ip/ip.h"
#include "../ip/ip_registry.h"


/*********************************************************
 *      start of route-map subcommands
 */

LINK_EXIT(after_routemap_exit, no_alt);
NVCOMMENT(after_routemap_comment, after_routemap_exit);
#undef	ALTERNATE
#define	ALTERNATE	after_routemap_comment
#include "cfg_routemap_exit.h"
NOP	(cfg_routemap_extend_here, ALTERNATE, NONE);
#undef	ALTERNATE
#define	ALTERNATE	cfg_routemap_extend_here

#include "cfg_routemap_set.h"
#include "cfg_routemap_match.h"

NO_OR_DEFAULT(configroutemap_no, ALTERNATE, PRIV_MIN | PRIV_NONVGEN);
HELP	(top_config_routemap, configroutemap_no,
	 "Route Map configuration commands:\n");
#undef	ALTERNATE


/*
 * Parse chains for IP Routing clear commands
 */
#define	ALTERNATE	NONE
#include "exec_clear_ip_routing.h"
LINK_POINT(iprouting_clear_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chains for IP Routing show commands
 */
#define	ALTERNATE	NONE
#include "exec_show_ip_routing.h"
LINK_POINT(iprouting_show_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chains for IP Routing IP debug commands
 */
#define	ALTERNATE	NONE
#include "exec_debug_ip_routing.h"
LINK_POINT(iprouting_ipdebug_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chains for Protocols show commands
 */
#define	ALTERNATE	NONE
#include "exec_show_route-map.h"
#include "exec_show_protocols.h"
LINK_POINT(ipprotocols_show_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chains for IP Routing config commands
 */
#define ALTERNATE       NONE
#include "cfg_ip_routing_routing.h"
LINK_POINT(iprouting_routing_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chains for Route Map config command
 */
#define	ALTERNATE	NONE
#include "cfg_route_map.h"
LINK_POINT(routemap_routing_command, ALTERNATE);
#undef	ALTERNATE

/*
 * Parse chains for IP specific route map clauses.
 */
#define	ALTERNATE	NONE
#include "../iprouting/cfg_routemap_match_ip.h"
LINK_POINT(routemap_match_ip_command, ALTERNATE);
#undef	ALTERNATE

#define	ALTERNATE	NONE
#include "../iprouting/cfg_routemap_set_ip.h"
LINK_POINT(routemap_set_ip_command, ALTERNATE);
#undef	ALTERNATE

/*
 * Parse chains for IP Routing interface commands
 */
#define	ALTERNATE	NONE
#include "cfg_int_ip2_routing.h"
LINK_POINT(iprouting_interface2_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chains for hot standby protocol interface commands
 */
#define	ALTERNATE	NONE
#include "cfg_int_standby.h"
LINK_POINT(standby_interface_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chains for hot standby protocol debug commands
 */
#define	ALTERNATE	NONE
#include "exec_debug_standby.h"
LINK_POINT(standby_debug_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chains for hot standby show commands
 */
#define	ALTERNATE	NONE
#include "exec_show_standby.h"
LINK_POINT(standby_show_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chains for IP Routing router command
 */
#define	ALTERNATE	NONE
#include "cfg_router.h"
LINK_POINT(ip_router_command, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chains for IP Routing router commands
 */

/*********************************************************
 *	start of configure router commands
 */

LINK_EXIT(after_router_exit, no_alt);
NVCOMMENT(after_router_comment, after_router_exit);
#undef	ALTERNATE
#define	ALTERNATE	after_router_comment
#include "../parser/cfg_router_exit.h"
LINK_TRANS	(cfg_router_extend_here, ALTERNATE);
#undef	ALTERNATE
#define	ALTERNATE	cfg_router_extend_here

#include "cfg_router_auto_summary.h"
#include "cfg_router_distance.h"
#include "cfg_router_distribute.h"
#include "cfg_router_default.h"
#include "cfg_router_maximum-paths.h"
#include "cfg_router_metric.h"
#include "cfg_router_neighbor.h"
#include "cfg_router_network.h"
#include "cfg_router_offset.h"
#include "cfg_router_output-delay.h"
#include "cfg_router_passive-interface.h"
#include "cfg_router_redistribute.h"
#include "cfg_router_summary_addr.h"
#include "cfg_router_timers.h"
#include "cfg_router_traffic-share.h"
#include "cfg_router_validate.h"
#include "cfg_router_variance.h"
#include "cfg_router_version.h"
LINK_TRANS(ip_router_initial_commands, ALTERNATE);

NO_OR_DEFAULT(configrouter_no, ip_router_initial_commands,
	   PRIV_MIN | PRIV_NONVGEN);
HELP	(top_config_router, configrouter_no,
	 "Router configuration commands:\n");
#undef	ALTERNATE



/*
 * Parse chain registration array for IP Routing
 */
static parser_extension_request iprouting_chain_init_table[] = {
    { PARSE_ADD_CLEAR_IP_CMD, &pname(iprouting_clear_commands) },
    { PARSE_ADD_SHOW_IP_CMD, &pname(iprouting_show_commands) },
    { PARSE_ADD_SHOW_CMD, &pname(ipprotocols_show_commands) },
    { PARSE_ADD_CFG_RTG_IP_CMD, &pname(iprouting_routing_commands) },
    { PARSE_ADD_CFG_ROUTING_CMD, &pname(routemap_routing_command) },
    { PARSE_ADD_CFG_INT_IP2_CMD, &pname(iprouting_interface2_commands) },
    { PARSE_ADD_CFG_ROUTER_COMMAND, &pname(ip_router_command) },
    { PARSE_ADD_DEBUG_IP_CMD, &pname(iprouting_ipdebug_commands) },
    { PARSE_ADD_CFG_INT_AS_CMD, &pname(standby_interface_commands) },
    { PARSE_ADD_DEBUG_CMD, &pname(standby_debug_commands) },
    { PARSE_ADD_SHOW_CMD, &pname(standby_show_commands) },
    { PARSE_ADD_CFG_ROUTEMAP_MATCH_CMD, &pname(routemap_match_ip_command) },
    { PARSE_ADD_CFG_ROUTEMAP_SET_CMD, &pname(routemap_set_ip_command) },
    { PARSE_ADD_GLOBAL_EXIT,
	  (dynamic_transition *) &pname(after_routemap_exit) },
    { PARSE_ADD_GLOBAL_EXIT,
	  (dynamic_transition *) &pname(after_router_exit) },
    { PARSE_ADD_CFG_RTMAP_SET_EXIT,
	  (dynamic_transition *) &pname(routemap_set_ip_return) },
    { PARSE_ADD_CFG_RTMAP_MATCH_EXIT,
	  (dynamic_transition *) &pname(routemap_match_ip_exit) },
    { PARSE_LIST_END, NULL }
};

static void *router_save_var (parseinfo *csb)
{
    void *protocol = csb->protocol;

    csb->protocol = NULL;
    return(protocol);
}

static void router_reset_var (parseinfo *csb, void *var)
{
    csb->protocol = var;
}

static boolean router_http_init (parseinfo *csb)
{
    char *proto = reg_invoke_http_get_uri_token(csb->http_state);
    routerprotocolinfo *pi;
    int pdb_type = PDB_UNKNOWN;
    pdbtype *pdb;

    if (proto) {
	csb->router_name.myname = proto;
	for (pi = routerprotocols; pi->str; pi++) {
	    if (parser_http_debug) {
		buginf("\nComparing '%s' to '%s'", proto, pi->str);
	    }
	    if (strcasecmp(pi->str, proto) == 0) {
		csb->router_name.protocol = pi->protocol;
		break;
	    }
	}

	if (pi->str) {
	    csb->router_name.as = 0;
	    csb->router_name.tag[0] = '\0';

	    switch (pi->type) {
	      case ROARG_PID:
	      case ROARG_AS:
		csb->router_name.as =
		    atoi(reg_invoke_http_get_uri_token(csb->http_state));
		csb->router_name.arg_specified = TRUE;
		break;
	      case ROARG_NONE:
		csb->router_name.arg_specified = FALSE;
		break;
	      case ROARG_TAG:
		sstrncpy(csb->router_name.tag,
			 reg_invoke_http_get_uri_token(csb->http_state),
			 MAXTOKLEN);
		break;
	      default:
		/* Uh, what */
		return(FALSE);
	    }

	    pdb = router_parse(csb, FALSE, &pdb_type);
	    if (pdb) {
		csb->protocol = pdb;
		return(TRUE);
	    }
	}
    }

    reg_invoke_http_send_not_found(csb->http_state,
				   "\nInvalid routing process");
    return(FALSE);
}

static void *routemap_save_var (parseinfo *csb)
{
    route_maptype *routemap = csb->route_map;

    csb->route_map = NULL;
    return(routemap);
}

static void routemap_reset_var (parseinfo *csb, void *var)
{
    csb->route_map = var;
}


/*
 * iprouting_parser_init - Initialize IP Routing parser support
 */
void iprouting_parser_init (void)
{
    parser_add_command_list(iprouting_chain_init_table, "iprouting");
    router_mode = parser_add_mode("router", "config-router",
				  "Router configuration mode",
				  TRUE, TRUE, "configure",
				  router_save_var, router_reset_var,
				  &pname(top_config_router),
				  router_http_init);
    parser_add_mode("route-map", "config-route-map",
		    "Route map config mode",
		    TRUE, TRUE, "configure",
		    routemap_save_var, routemap_reset_var,
		    &pname(top_config_routemap),
		    NULL);
    parser_add_link_point(PARSE_ADD_CFG_ROUTER_CMD, "router configuration",
			  &pname(cfg_router_extend_here));
    parser_add_link_point(PARSE_ADD_CFG_ROUTER_NEIGHBOR_CMD,
			  "neighbor", &pname(rtr_neigh_extend_here));
    parser_add_link_point(PARSE_ADD_CFG_ROUTER_TIMER_CMD,
			  "timers", &pname(router_timers_extend_here));
    parser_add_link_point(PARSE_ADD_CFG_ROUTER_DISTANCE_CMD,
			  "distance", &pname(router_distance_extend_here));
    parser_add_link_point(PARSE_ADD_CFG_ROUTER_INITIAL_CMD,
 			  "initial router-commands", 
 			  &pname(ip_router_initial_commands));
    parser_add_link_point(PARSE_ADD_CFG_ROUTEMAP_SET_CMD,
			  "set", &pname(routemap_set_extend_here));
    parser_add_link_point(PARSE_ADD_CFG_ROUTEMAP_MATCH_CMD,
			  "match", &pname(routemap_match_extend_here));
    parser_add_link_point(PARSE_ADD_CFG_ROUTEMAP_SET_IP_CMD,
			  "set ip", &pname(routemap_set_ip_extend_here));
    parser_add_link_exit(PARSE_ADD_CFG_RTMAP_SET_EXIT, "routemap set exit",
			 &pname(routemap_set_return_here));

    parser_add_link_exit(PARSE_ADD_CFG_RTMAP_MATCH_EXIT, "routemap match exit",
			 &pname(routemap_match_return));
    parser_add_link_point(PARSE_ADD_CFG_INT_IP_SPLIT_CMD, "ip split horizon",
			  &pname(ci_ip_split_link));
}
