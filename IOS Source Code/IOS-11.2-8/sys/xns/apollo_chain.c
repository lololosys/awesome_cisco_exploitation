/* $Id: apollo_chain.c,v 3.6.10.1 1996/05/21 10:11:16 thille Exp $
 * $Source: /release/112/cvs/Xsys/xns/apollo_chain.c,v $
 *------------------------------------------------------------------
 * Parse chains for APOLLO
 *
 * January 5, 1993, Kevin Paul Herbert
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: apollo_chain.c,v $
 * Revision 3.6.10.1  1996/05/21  10:11:16  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.6  1996/02/22  14:40:29  widmer
 * CSCdi49281:  make all parser_extension_request static const
 * make parser_extension_request typedef const
 * make all parser_extension_request static
 *
 * Revision 3.5  1996/02/01  06:13:22  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.4  1996/01/29  07:31:42  hampton
 * Remove unnecessary includes of parser definition files.  [CSCdi47717]
 *
 * Revision 3.3  1995/12/17  18:42:21  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.2  1995/11/17  19:20:33  gstovall
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:56:58  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/11/08  21:37:55  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.3  1995/07/02  01:42:54  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize xns, atalk, vines, decnet, xremote, x25, unix, wan, tn3270,
 * clns, cdp
 *
 * Revision 2.2  1995/06/15  15:19:49  widmer
 * CSCdi35889:  parser transition structures should be static
 *
 * Revision 2.1  1995/06/07  23:26:41  hampton
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
#include "../ui/debug.h"
#include "parser.h"
#include "xns.h"
#include "apollo.h"
#include "apollo_debug.h"
#include "../ip/tunnel.h"

#include "../parser/parser_actions.h"
#include "../xns/apollo_actions.h"
#include "../parser/macros.h"
#include "parser_defs_apollo.h"
#include "../parser/parser_defs_parser.h"
#include "../if/network.h"


/*
 * Parse chains for DEBUG commands
 */
#define ALTERNATE NONE
#include "exec_debug_apollo.h"
LINK_POINT(apollo_debug_commands, ALTERNATE);
#undef ALTERNATE


/*
 * Parse chains for SHOW commands
 */
#define ALTERNATE NONE
#include "exec_show_apollo.h"
LINK_POINT(apollo_show_commands, ALTERNATE);
#undef ALTERNATE


/*
 * Parse chains for interface commands
 */
#define ALTERNATE NONE
#include "cfg_int_apollo.h"
LINK_POINT(apollo_int_commands, ALTERNATE);
#undef ALTERNATE


/*
 * Parse chains for APOLLO router commands
 */
#define ALTERNATE NONE
#include "cfg_apollo_routing.h"
LINK_POINT(apollo_routing_commands, ALTERNATE);
#undef ALTERNATE


/*
 * Parse chains for APOLLO top-level configuation commands
 */
#define ALTERNATE NONE
#include "cfg_apollo.h"
LINK_POINT(apollo_config_commands, ALTERNATE);
#undef ALTERNATE


/*
 * Parse chains for APOLLO ping command
 */
LINK_EXIT(exec_ping_apollo_exit, no_alt);
KEYWORD_ID(exec_ping_apollo, exec_ping_apollo_exit, NONE,
	   OBJ(int,1), LINK_APOLLO, "apollo", "Apollo echo", PRIV_USER);
LINK_POINT(apollo_ping_commands, exec_ping_apollo);


/*
 * Parse chain registration array for APOLLO
 */
static parser_extension_request apollo_chain_init_table[] = {
    { PARSE_ADD_DEBUG_CMD, &pname(apollo_debug_commands) },
    { PARSE_ADD_SHOW_CMD, &pname(apollo_show_commands) },
    { PARSE_ADD_CFG_INT_CMD, &pname(apollo_int_commands) },
    { PARSE_ADD_CFG_ROUTING_CMD, &pname(apollo_routing_commands) },
    { PARSE_ADD_CFG_TOP_CMD, &pname(apollo_config_commands) },
    { PARSE_ADD_PING_CMD, &pname(apollo_ping_commands) },
    { PARSE_ADD_PING_EXIT,
	  (dynamic_transition *) &pname(exec_ping_apollo_exit) },
    { PARSE_LIST_END, NULL }
};

/*
 * apollo_parser_init - Initialize parse chains for APOLLO
 * 
 * This routine is called at system startup to register the parse chains
 * for APOLLO.
 */

void apollo_parser_init (void)
{
    parser_add_protoaddr(LINK_APOLLO,		PMATCH_APOLLO PMATCH_DGRAM,
			 "apollo",		"Apollo Domain",
			 0, ADDR_APOLLO);
    parser_add_address(ADDR_APOLLO, apollo_addr_short_help, match_apollo_addr);
    parser_add_command_list(apollo_chain_init_table, "apollo");
}
