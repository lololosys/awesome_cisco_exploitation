/* $Id: at_chain.c,v 3.6.10.5 1996/07/10 22:17:47 dstine Exp $
 * $Source: /release/112/cvs/Xsys/atalk/at_chain.c,v $
 *------------------------------------------------------------------
 * Parse chains for Appletalk
 *
 * January 5, 1993, Kevin Paul Herbert
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: at_chain.c,v $
 * Revision 3.6.10.5  1996/07/10  22:17:47  dstine
 * CSCdi62471:  Superfluous #include files in atalk, atm, os files
 * Branch: California_branch
 *
 * Revision 3.6.10.4  1996/05/30  23:42:02  hampton
 * All commands that take access list numbers should accept either numbers
 * in the current monolithic number space, or hidden numbers in the future
 * per-protocol access list spaces.  [CSCdi59067]
 * Branch: California_branch
 *
 * Revision 3.6.10.3  1996/05/21  09:42:24  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.6.10.2  1996/04/02  05:35:35  dkatz
 * CSCdi36031:  Running out of memory running 2 or more EIGRP ASs
 * Branch: California_branch
 * EIGRP Jumbo Patch.
 *
 * Revision 3.6.10.1  1996/03/23  01:25:53  slin
 * Branch: California_branch
 *
 * AppleTalk makes its exodus from the idb.
 *
 * Revision 3.6  1996/02/22  14:31:57  widmer
 * CSCdi49281:  make all parser_extension_request static const
 * make parser_extension_request typedef const
 * make all parser_extension_request static
 *
 * Revision 3.5  1996/02/01  05:58:52  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.4  1996/01/29  07:25:59  hampton
 * Remove unnecessary includes of parser definition files.  [CSCdi47717]
 *
 * Revision 3.3  1995/12/17  18:24:18  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.2  1995/11/17  08:41:53  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  10:56:20  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/11/08  20:49:31  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.3  1995/07/02  01:43:33  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize xns, atalk, vines, decnet, xremote, x25, unix, wan, tn3270,
 * clns, cdp
 *
 * Revision 2.2  1995/06/15  15:18:05  widmer
 * CSCdi35889:  parser transition structures should be static
 *
 * Revision 2.1  1995/06/07  20:07:44  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#include "master.h"
#include "../ui/common_strings.h"
#include <interface_private.h>
#include "packet.h"
#include "config.h"
#include "parser.h"
#include "access.h"
#include "atalk_private.h"

#include "../parser/parser_actions.h"
#include "../atalk/at_actions.h"
#include "../parser/macros.h"
#include "parser_defs_atalk.h"
#include "../parser/parser_defs_parser.h"

/*
 * Parse chains for CLEAR commands
 */
#define ALTERNATE NONE
#include "exec_clear_apple.h"
LINK_POINT(apple_clear_commands, ALTERNATE);
#undef ALTERNATE


/*
 * Parse chains for DEBUG commands
 */
#define ALTERNATE NONE
#include "exec_debug_apple.h"
LINK_POINT(apple_debug_commands, ALTERNATE);
#undef ALTERNATE


/*
 * Parse chains for SHOW commands
 */
#define ALTERNATE NONE
#include "exec_show_apple.h"
LINK_POINT(apple_show_commands, ALTERNATE);
#undef ALTERNATE


/*
 * Parse chains for interface commands
 */
#define ALTERNATE NONE
#include "cfg_int_apple.h"
LINK_POINT(apple_int_commands, ALTERNATE);
#undef ALTERNATE


/*
 * Parse chains for APPLE router commands
 */
#define ALTERNATE NONE
#include "cfg_appletalk_routing.h"
LINK_POINT(apple_routing_commands, ALTERNATE);
#undef ALTERNATE

/*
 * Parse chains for APPLE top-level configuation commands
 */
#define ALTERNATE NONE
#include "cfg_apple.h"
LINK_POINT(apple_config_commands, ALTERNATE);
#undef ALTERNATE

/*
 * Parse chains for APPLE access list commands
 */
#define	ALTERNATE	NONE
#define	OALTERNATE	NONE
#include "cfg_acclist_atalk.h"
LINK_POINT(apple_acclist_commands, ALTERNATE);
LINK_POINT(oapple_acclist_commands, OALTERNATE);	/* Old form */
#undef	OALTERNATE
#undef	ALTERNATE

/*
 * Parse chains for APPLE ping command
 */
LINK_EXIT(exec_ping_atalk_exit, no_alt);
KEYWORD_ID(exec_ping_atalk, exec_ping_atalk_exit, NONE,
	   OBJ(int,1), LINK_APPLETALK, "appletalk", "Appletalk echo",
	   PRIV_USER);
LINK_POINT(apple_ping_commands, exec_ping_atalk);


/*
 * Parse chains for APPLE priority queueing options
 */
#define	ALTERNATE	NONE
#include "cfg_priority_apple.h"
LINK_POINT(apple_priority_commands, ALTERNATE);
#undef	ALTERNATE

/*
 * Parse chain registration array for APPLE
 */
static parser_extension_request apple_chain_init_table[] = {
    { PARSE_ADD_CLEAR_CMD, &pname(apple_clear_commands) },
    { PARSE_ADD_DEBUG_CMD, &pname(apple_debug_commands) },
    { PARSE_ADD_SHOW_CMD, &pname(apple_show_commands) },
    { PARSE_ADD_CFG_TOP_CMD, &pname(apple_config_commands) },
    { PARSE_ADD_CFG_INT_CMD, &pname(apple_int_commands) },
    { PARSE_ADD_CFG_ROUTING_CMD, &pname(apple_routing_commands) },
    { PARSE_ADD_CFG_ATALK_ROUTING_CMD, &pname(apple_acclist_commands) },
    { PARSE_ADD_CFG_ACCLIST_CMD, &pname(oapple_acclist_commands) }, /* Old */
    { PARSE_ADD_PING_CMD, &pname(apple_ping_commands) },
    { PARSE_ADD_PRIORITY_OPTIONS, &pname(apple_priority_commands) },
    { PARSE_ADD_PING_EXIT, 
	  (dynamic_transition *) &pname(exec_ping_atalk_exit) },
    { PARSE_LIST_END, NULL }
};

/*
 * apple_parser_init - Initialize parse chains for APPLE
 * 
 * This routine is called at system startup to register the parse chains
 * for APPLE.
 */

void atalk_parser_init (void)
{
    parser_add_link_point(PARSE_ADD_CLEAR_ATALK_CMD, "clear atalk",
			  &pname(clear_apple_extend_here));
    parser_add_link_point(PARSE_ADD_DEBUG_ATALK_CMD, "debug atalk",
			  &pname(debug_atalk_extend_here));
    parser_add_link_point(PARSE_ADD_SHOW_ATALK_CMD, "show atalk",
			  &pname(show_atalk_extend_here));
    parser_add_link_point(PARSE_ADD_CFG_TOP_ATALK_CMD, "config atalk",
			  &pname(cfg_atalk_extend_here));
    parser_add_link_point(PARSE_ADD_CFG_TOP_ATALK_ROUTING_CMD,
			  "config apple routing",
			  &pname(cfg_apple_routing_extend_here));
    parser_add_link_point(PARSE_ADD_CFG_INT_ATALK_CMD,
			  "interface config atalk",
			  &pname(ci_atalk_extend_here));
    parser_add_link_point(PARSE_ADD_ATALK_PROTOCOL_CMD, "config atalk protocol",
			  &pname(ci_at_rp_extend));
    parser_add_link_point(PARSE_ADD_CFG_ATALK_ROUTING_CMD,
			  "config atalk routing",
			  &pname(cfg_atalk_routing_extend_here));
    parser_add_protoaddr(LINK_AARP,		PMATCH_AARP PMATCH_DGRAM,
			 "aarp",		"AppleTalk ARP",
			 0, ADDR_ILLEGAL);
    parser_add_protoaddr(LINK_APPLETALK,	PMATCH_APPLETALK PMATCH_DGRAM,
			 "appletalk",		"AppleTalk",
			 0, ADDR_APPLE);
    parser_add_address(ADDR_APPLE, apple_addr_short_help, match_apple_addr);
    parser_add_command_list(apple_chain_init_table, "apple");
}
