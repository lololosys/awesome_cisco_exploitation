/* $Id: ataurp_chain.c,v 3.1.8.3 1996/07/10 22:19:15 dstine Exp $
 * $Source: /release/112/cvs/Xsys/atalk/ataurp_chain.c,v $
 *------------------------------------------------------------------
 * Parse chains for Appletalk AURP-service
 *
 * March 1996, Dean Wong
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ataurp_chain.c,v $
 * Revision 3.1.8.3  1996/07/10  22:19:15  dstine
 * CSCdi62471:  Superfluous #include files in atalk, atm, os files
 * Branch: California_branch
 *
 * Revision 3.1.8.2  1996/05/21  09:42:31  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.1.8.1  1996/05/02  08:40:57  dwong
 * Branch: California_branch
 * Commit SMRP-7kFastSwitch, Load Balancing, and AURP Modularity
 *
 * Revision 3.1  1996/03/14  00:37:41  dwong
 * Placeholder for California.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#include "master.h"
#include "interface_private.h"
#include "packet.h"
#include "config.h"
#include "parser.h"

#include "../ui/common_strings.h"
#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "../parser/parser_defs_parser.h"
#include "parser_defs_atalk.h"
#include "atalk_private.h"
#include "ataurp_debug.h"

/*
 * Parse chains for DEBUG commands
 */
#define ALTERNATE NONE

EOLS	(debug_apple_aurp_connection_eol, ataurp_debug_command,
	 DEBUG_AURP_CONNECTION);
INTERFACE (debug_apple_aurp_connection_int_name,
	   debug_apple_aurp_connection_eol,
	   no_alt, OBJ(idb, 1), IFTYPE_HWIDB);
KEYWORD_ID (debug_apple_aurp_connection_int,
	    debug_apple_aurp_connection_int_name,
	    debug_apple_aurp_connection_eol,
	    OBJ(int, 1), ATALK_DEBUG_INTERFACE,
	    common_str_interface, "Appletalk interface-specific debugging", PRIV_OPR);
KEYWORD (debug_apple_aurp_connection, debug_apple_aurp_connection_int,
	 ALTERNATE,
	 "aurp-connection", "AURP connection", PRIV_OPR);

EOLS	(debug_apple_aurp_packet_eol, ataurp_debug_command,
	 DEBUG_AURP_PACKET);
INTERFACE (debug_apple_aurp_packet_int_name, debug_apple_aurp_packet_eol,
	   no_alt, OBJ(idb, 1), IFTYPE_HWIDB);
KEYWORD_ID (debug_apple_aurp_packet_int, debug_apple_aurp_packet_int_name,
	    debug_apple_aurp_packet_eol, OBJ(int, 1), ATALK_DEBUG_INTERFACE,
	    common_str_interface, "Appletalk interface-specific debugging", PRIV_OPR);
KEYWORD (debug_apple_aurp_packet, debug_apple_aurp_packet_int,
	 debug_apple_aurp_connection,
	 "aurp-packet", "AURP packets", PRIV_OPR);

EOLS	(debug_apple_aurp_redist_eol, ataurp_debug_command,
	 DEBUG_AURP_REDIST);
INTERFACE (debug_apple_aurp_redist_int_name, debug_apple_aurp_redist_eol,
	   no_alt, OBJ(idb, 1), IFTYPE_HWIDB);
KEYWORD_ID (debug_apple_aurp_redist_int, debug_apple_aurp_redist_int_name,
	    debug_apple_aurp_redist_eol, OBJ(int, 1), ATALK_DEBUG_INTERFACE,
	    common_str_interface, "Appletalk interface-specific debugging", PRIV_OPR);
KEYWORD (debug_apple_aurp_redist, debug_apple_aurp_redist_int,
	 debug_apple_aurp_packet,
	 "aurp-redist", "AURP route redistribution", PRIV_OPR|PRIV_HIDDEN);

EOLS	(debug_apple_aurp_update_eol, ataurp_debug_command,
	 DEBUG_AURP_UPDATE);
INTERFACE (debug_apple_aurp_update_int_name, debug_apple_aurp_update_eol,
	   no_alt, OBJ(idb, 1), IFTYPE_HWIDB);
KEYWORD_ID (debug_apple_aurp_update_int, debug_apple_aurp_update_int_name,
	    debug_apple_aurp_update_eol, OBJ(int, 1), ATALK_DEBUG_INTERFACE,
	    common_str_interface, "Appletalk interface-specific debugging", PRIV_OPR);
KEYWORD (debug_apple_aurp_update, debug_apple_aurp_update_int,
	 debug_apple_aurp_redist,
	 "aurp-update", "AURP routing updates", PRIV_OPR);

#undef ALTERNATE
#define ALTERNATE	debug_apple_aurp_update
LINK_POINT(ataurp_debug_commands, ALTERNATE);
#undef ALTERNATE

/*
 * Parse chains for AURP interface commands
 */
#define ALTERNATE NONE
/*******************************************************************
 * appletalk aurp tickle-time <seconds>
 * no apple aurp tickle-time
 *
 * The nominal time between tickle packets.
 */
PARAMS_KEYONLY (ci_aurp_tickle, no_alt,
	"tickle-time", OBJ(int, 1), 30, -1,
	ataurpif_command, AURP_TICKLE,
	"AURP Tickle Interval", "Seconds", PRIV_CONF|PRIV_SUBIF);

KEYWORD (ci_aurp, ci_aurp_tickle, NONE,
	 "aurp", "Set AURP parameters", PRIV_CONF|PRIV_SUBIF);

ASSERT	(ci_aurp_assert, ci_aurp, ALTERNATE,
	 is_tunnel(csb->interface->hwptr));

/*******************************************************************
 * appletalk protocol aurp
 */
EOLS (ci_appletalk_protocol_eol, ataurpif_command, AURP_RP);

KEYWORD (ci_appletalk_protocol_aurp, ci_appletalk_protocol_eol, no_alt,
	"aurp", "Enable AURP processing on this interface", 
	PRIV_CONF|PRIV_SUBIF);

TEST_IDBSTATUS(ci_appletalk_protocol_assert, ci_appletalk_protocol_aurp,
	       NONE, no_alt, IDB_TUNNEL);

KEYWORD (ci_appletalk_protocol, ci_appletalk_protocol_assert, ci_aurp_assert,
	"protocol", "Select AppleTalk routing protocol",
	PRIV_CONF|PRIV_SUBIF|PRIV_DUPLICATE);

#undef ALTERNATE
#define ALTERNATE ci_appletalk_protocol
LINK_POINT(ataurp_interface_commands, ALTERNATE);
#undef  ALTERNATE

/*
 * Parse chains for AURP top-level configuration commands
 */
#define ALTERNATE NONE
PARAMS_KEYONLY (configure_aurp_update, no_alt,
	       "update-interval", OBJ(int, 1), 10, -1,
	       appleaurp_command, AURP_UPDATE,
	       "AURP Update Interval", "Seconds", PRIV_CONF);

KEYWORD (configure_aurp, configure_aurp_update, no_alt,
	 "aurp", "Set AURP parameters", PRIV_CONF);

KEYWORD (cr_ataurp, configure_aurp, ALTERNATE, 
	 "appletalk", "AppleTalk global configuration commands",
	 PRIV_CONF | PRIV_DUPLICATE);

#undef  ALTERNATE
#define ALTERNATE	cr_ataurp
LINK_POINT(ataurp_config_commands, ALTERNATE);
#undef ALTERNATE

/*
 * Parse chain for SHOW commands.
 */
#define ALTERNATE NONE
EOLS	(show_aurp_events_eol, show_appleaurp, SHOW_APPLE_AURP_EVENTS);
NUMBER 	(show_aurp_events_network, show_aurp_events_eol, show_aurp_events_eol,
	OBJ(int, 1), 1, 65279, "Network number");
KEYWORD (show_aurp_events, show_aurp_events_network, no_alt,
	 "events", "AURP pending events", PRIV_USER);

EOLS	(show_aurp_topology_eol, show_appleaurp, SHOW_APPLE_AURP_TOPOLOGY);
NUMBER 	(show_aurp_topology_network, show_aurp_topology_eol,
	 show_aurp_topology_eol,
	 OBJ(int, 1), 1, 65279, "Network number");
KEYWORD (show_aurp_topology, show_aurp_topology_network, show_aurp_events,
	 "topology", "AURP private topology database", PRIV_USER);

KEYWORD (show_apple_aurp, show_aurp_topology, ALTERNATE,
	 "aurp", "AURP information", PRIV_USER);

#undef  ALTERNATE
#define ALTERNATE	show_apple_aurp
LINK_POINT(ataurp_show_commands, ALTERNATE);
#undef ALTERNATE


/*
 * Parse chain registration array for AURP
 */
static parser_extension_request ataurp_chain_init_table[] = {
    { PARSE_ADD_DEBUG_ATALK_CMD, &pname(ataurp_debug_commands) },
    { PARSE_ADD_CFG_INT_ATALK_CMD, &pname(ataurp_interface_commands) },
    { PARSE_ADD_CFG_ROUTING_CMD, &pname(ataurp_config_commands) },
    { PARSE_ADD_SHOW_ATALK_CMD, &pname(ataurp_show_commands) },
    { PARSE_LIST_END, NULL }
};


/*
 * ataurp_parser_init - Initialize ATAURP parser support
 */
void
ataurp_parser_init (void)
{
    parser_add_command_list(ataurp_chain_init_table, "ataurp");
}

