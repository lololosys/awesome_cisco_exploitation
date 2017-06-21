/* $Id: x25_chain.c,v 3.8.10.1 1996/05/21 10:10:42 thille Exp $
 * $Source: /release/112/cvs/Xsys/x25/x25_chain.c,v $
 *------------------------------------------------------------------
 * X25 parse chains
 *
 * May 1993, Robert Widmer
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: x25_chain.c,v $
 * Revision 3.8.10.1  1996/05/21  10:10:42  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.8  1996/02/22  14:40:24  widmer
 * CSCdi49281:  make all parser_extension_request static const
 * make parser_extension_request typedef const
 * make all parser_extension_request static
 *
 * Revision 3.7  1996/02/01  06:13:07  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.6  1996/01/29  07:31:34  hampton
 * Remove unnecessary includes of parser definition files.  [CSCdi47717]
 *
 * Revision 3.5  1996/01/25  11:23:54  smackie
 * Registry code gets some much-needed hot-oil massaging, free liposuction
 * and a makeover.  (CSCdi47101)
 *
 *    o Compress registry structures. Saves over 120K for all platforms
 *    o Add registry subsystems and remove feature registry initializion
 *      from registry.c to fix woeful initialization scaling problems
 *    o Remove unused and seemingly unloved registry debugging code
 *    o Add registry memory statistics to shame people into action
 *
 * Revision 3.4  1995/12/17  18:41:57  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.3  1995/11/27  16:24:11  widmer
 * CSCdi33598:  WAN commands (eg X.25) in parser even when Subsystem not
 * included
 * Create parser link for interface encapsulation types
 *
 * Revision 3.2  1995/11/17  18:07:25  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:53:31  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/07/02  01:45:14  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize xns, atalk, vines, decnet, xremote, x25, unix, wan, tn3270,
 * clns, cdp
 *
 * Revision 2.1  1995/06/07  23:22:09  hampton
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
#include "../x25/x25.h" /* X25_ENABLE || DDN_ENABLE */
#include "../x25/x25_address.h" /* X25_ENABLE || DDN_ENABLE */
#include "../x25/x25_switch.h" /* X25_ENABLE || DDN_ENABLE */
#include "../x25/x25_proto.h" /* X25_ENABLE || DDN_ENABLE */
#include "../x25/x25_bfe.h" /* X25_ENABLE || DDN_ENABLE */
#include "../x25/x25_debug.h"
#include "../x25/lapb.h"
#include "../parser/parser_defs_parser.h"
#include "parser_defs_x25.h"
#include "../parser/parser_defs_snmp.h"
#include "../x25/x25_actions.h"
#include "registry.h"
#include "../snmp/snmp_api.h"
#include "../snmp/snmp_registry.regh"


/*
 * Parse chains for X25 clear commands
 */
#define	ALTERNATE	NONE
#include "exec_clear_x25.h"
LINK_POINT(x25_clear_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chains for X25 debug commands
 */
#define	ALTERNATE	NONE
#include "exec_debug_x25.h"
LINK_POINT(x25_debug_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chains for X25 show commands
 */
#define	ALTERNATE	NONE
#include "exec_show_x25.h"
LINK_POINT(x25_show_commands, ALTERNATE);
#undef	ALTERNATE


/* 
 * Parse chains for X25 interface commands
 */
#define	ALTERNATE	NONE
#include "cfg_int_x25.h"
LINK_POINT(x25_interface_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chains for X25 routing commands
 */
#define	ALTERNATE	NONE
#include "cfg_x25_routing.h"
LINK_POINT(x25_rtng_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chains for X25 config commands
 */
#define	ALTERNATE	NONE
#include "cfg_x25.h"
LINK_POINT(x25_config_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chains for X25 exec commands
 */
#define	ALTERNATE	NONE
#include "exec_bfe.h"
LINK_POINT(bfe_exec_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chains for X25 interface encapsulation commands
 */
#define	ALTERNATE	NONE
#include "cfg_int_encaps_x25.h"
LINK_POINT(x25_encaps_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chains for X25 snmp-server host commands
 */
LINK_EXIT(cfg_snmp_host_x25_exit, no_alt);
KEYWORD_OR(cfg_snmp_host_x25, cfg_snmp_host_x25_exit, NONE,
	   OBJ(int,1), (1<<TRAP_X25),
	   "x25", "Allow x25 event traps", PRIV_CONF);
LINK_POINT(cfg_snmp_host_x25_entry, cfg_snmp_host_x25);

static void x25_snmp_host_nvgen (ulong flags)
{
    nv_add(flags & (1 << TRAP_X25), " x25");
}


/*
 * Parse chain registration array for X25
 */
static parser_extension_request x25_chain_init_table[] = {
    { PARSE_ADD_CLEAR_CMD, &pname(x25_clear_commands) },
    { PARSE_ADD_DEBUG_CMD, &pname(x25_debug_commands) },
    { PARSE_ADD_SHOW_CMD, &pname(x25_show_commands) },
    { PARSE_ADD_CFG_INT_CMD, &pname(x25_interface_commands) },
    { PARSE_ADD_CFG_ROUTING_CMD, &pname(x25_rtng_commands) },
    { PARSE_ADD_CFG_TOP_CMD, &pname(x25_config_commands) },
    { PARSE_ADD_EXEC_CMD, &pname(bfe_exec_commands) },
    { PARSE_ADD_CFG_SNMP_HOST_CMD, &pname(cfg_snmp_host_x25_entry) },
    { PARSE_ADD_CFG_SNMP_HOST_EXIT,
	  (dynamic_transition *) &pname(cfg_snmp_host_x25_exit) },
    { PARSE_ADD_INT_ENCAPS_CMD, &pname(x25_encaps_commands) },
    { PARSE_ADD_INT_ENCAPS_EXIT,
	  (dynamic_transition *) &pname(cfg_encap_x25_exit) },
    { PARSE_LIST_END, NULL }
};


/*
 * x25_parser_init - Initialize X25 parser support
 */
void x25_parser_init (void)
{
    parser_add_protoaddr(LINK_PAD,              PMATCH_PAD,
			 "pad",                 "PAD links",
			 0, ADDR_X121);
    parser_add_address(ADDR_X121, x121_addr_short_help, match_x121_addr);
    parser_add_address(ADDR_PARSE(STATION_X121), x121_hwaddr_short_help,
		       match_x121_hwaddr);

    cons_parser_init();
    parser_add_command_list(x25_chain_init_table, "x25");

    if (registry_populated(REG_SNMP))
	reg_add_snmp_host_nvgen(x25_snmp_host_nvgen, "x25_snmp_host_nvgen");
}
      
