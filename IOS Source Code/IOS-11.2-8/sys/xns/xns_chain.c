/* $Id: xns_chain.c,v 3.5.10.2 1996/05/30 23:49:50 hampton Exp $
 * $Source: /release/112/cvs/Xsys/xns/xns_chain.c,v $
 *------------------------------------------------------------------
 * Parse chains for XNS
 *
 * January 5, 1993, Kevin Paul Herbert
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: xns_chain.c,v $
 * Revision 3.5.10.2  1996/05/30  23:49:50  hampton
 * All commands that take access list numbers should accept either numbers
 * in the current monolithic number space, or hidden numbers in the future
 * per-protocol access list spaces.  [CSCdi59067]
 * Branch: California_branch
 *
 * Revision 3.5.10.1  1996/05/21  10:11:52  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.5  1996/02/22  14:40:38  widmer
 * CSCdi49281:  make all parser_extension_request static const
 * make parser_extension_request typedef const
 * make all parser_extension_request static
 *
 * Revision 3.4  1996/01/29  07:32:05  hampton
 * Remove unnecessary includes of parser definition files.  [CSCdi47717]
 *
 * Revision 3.3  1995/12/17  18:42:42  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.2  1995/11/17  19:25:23  gstovall
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  14:00:40  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/11/08  21:40:08  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.3  1995/07/02  01:43:05  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize xns, atalk, vines, decnet, xremote, x25, unix, wan, tn3270,
 * clns, cdp
 *
 * Revision 2.2  1995/06/15  15:19:52  widmer
 * CSCdi35889:  parser transition structures should be static
 *
 * Revision 2.1  1995/06/07  23:30:36  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include "interface_private.h"
#include "packet.h"
#include "config.h"
#include "parser.h"
#include "../ui/debug.h"
#include "xns.h"
#include "xns_debug.h"
#include "../ip/tunnel.h"

#include "../parser/parser_actions.h"
#include "../xns/xns_actions.h"
#include "../parser/macros.h"
#include "parser_defs_xns_nov.h"
#include "../parser/parser_defs_parser.h"
#include "../parser/parser_defs_priority.h"

/*
 * Parse chains for CLEAR commands
 */
#define ALTERNATE NONE
#include "exec_clear_xns.h"
LINK_POINT(xns_clear_commands, ALTERNATE);
#undef ALTERNATE


/*
 * Parse chains for DEBUG commands
 */
#define ALTERNATE NONE
#include "exec_debug_xns.h"
LINK_POINT(xns_debug_commands, ALTERNATE);
#undef ALTERNATE


/*
 * Parse chains for SHOW commands
 */
#define ALTERNATE NONE
#include "exec_show_xns.h"
LINK_POINT(xns_show_commands, ALTERNATE);
#undef ALTERNATE


/*
 * Parse chains for interface commands
 */
#define ALTERNATE NONE
#include "cfg_int_xns.h"
LINK_POINT(xns_int_commands, ALTERNATE);
#undef ALTERNATE


/*
 * Parse chains for XNS router commands
 */
#define ALTERNATE NONE
#include "cfg_xns_routing.h"
LINK_POINT(xns_routing_commands, ALTERNATE);
#undef ALTERNATE


/*
 * Parse chains for XNS top-level configuation commands
 */
#define ALTERNATE NONE
#include "cfg_xns.h"
LINK_POINT(xns_config_commands, ALTERNATE);
#undef ALTERNATE


/*
 * Parse chains for XNS access-list commands
 */
#define	ALTERNATE	NONE
#define	OALTERNATE	NONE
#include "cfg_acclist_xns.h"
LINK_POINT(oxns_acclist_commands, OALTERNATE);
LINK_POINT(xns_acclist_commands, ALTERNATE);
#undef	OALTERNATE
#undef	ALTERNATE


/*
 * Parse chains for XNS priority queueing options
 */
#define	ALTERNATE	NONE
#include "cfg_priority_xns.h"
LINK_POINT(xns_priority_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chains for XNS ping commands
 */
LINK_EXIT(exec_ping_xns_exit, no_alt);
KEYWORD_ID(exec_ping_xns, exec_ping_xns_exit, NONE,
	   OBJ(int,1), LINK_XNS, "xns", "XNS echo", PRIV_USER);
LINK_POINT(xns_ping_commands, exec_ping_xns);


/*
 * Parse chain registration array for XNS
 */
static parser_extension_request xns_chain_init_table[] = {
    { PARSE_ADD_CLEAR_CMD, &pname(xns_clear_commands) },
    { PARSE_ADD_DEBUG_CMD, &pname(xns_debug_commands) },
    { PARSE_ADD_SHOW_CMD, &pname(xns_show_commands) },
    { PARSE_ADD_CFG_INT_CMD, &pname(xns_int_commands) },
    { PARSE_ADD_CFG_ROUTING_CMD, &pname(xns_routing_commands) },
    { PARSE_ADD_CFG_TOP_CMD, &pname(xns_config_commands) },
    { PARSE_ADD_CFG_XNS_ROUTING_CMD, &pname(xns_acclist_commands) },
    { PARSE_ADD_CFG_ACCLIST_CMD, &pname(oxns_acclist_commands) }, /* Old */
    { PARSE_ADD_PING_CMD, &pname(xns_ping_commands) },
    { PARSE_ADD_PRIORITY_OPTIONS, &pname(xns_priority_commands) },
    { PARSE_ADD_PING_EXIT,
	  (dynamic_transition *) &pname(exec_ping_xns_exit) },
    { PARSE_LIST_END, NULL }
};

/*
 * xns_parser_init - Initialize parse chains for XNS
 * 
 * This routine is called at system startup to register the parse chains
 * for XNS.
 */

void xns_parser_init (void)
{
    parser_add_protoaddr(LINK_XNS,		PMATCH_XNS PMATCH_DGRAM,
			 "xns",			"Xerox Network Services",
			 0, ADDR_XNS);
    parser_add_link_point(PARSE_ADD_CFG_XNS_ROUTING_CMD, "xns routing",
			  &pname(conf_xns_routing_extend_here));
    parser_add_address(ADDR_XNS, xns_addr_short_help, match_xns_addr);
    parser_add_command_list(xns_chain_init_table, "xns");
}
