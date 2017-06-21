/* $Id: vines_chain.c,v 3.6.10.1 1996/05/21 10:08:29 thille Exp $
 * $Source: /release/112/cvs/Xsys/vines/vines_chain.c,v $
 *------------------------------------------------------------------
 * Parse chains for Vines
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: vines_chain.c,v $
 * Revision 3.6.10.1  1996/05/21  10:08:29  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.6  1996/02/22  14:39:41  widmer
 * CSCdi49281:  make all parser_extension_request static const
 * make parser_extension_request typedef const
 * make all parser_extension_request static
 *
 * Revision 3.5  1996/02/01  06:11:23  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.4  1996/01/29  07:30:54  hampton
 * Remove unnecessary includes of parser definition files.  [CSCdi47717]
 *
 * Revision 3.3  1995/12/17  18:40:45  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.2  1995/11/17  17:57:35  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:44:29  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/07/02  01:44:07  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize xns, atalk, vines, decnet, xremote, x25, unix, wan, tn3270,
 * clns, cdp
 *
 * Revision 2.2  1995/06/15  15:19:46  widmer
 * CSCdi35889:  parser transition structures should be static
 *
 * Revision 2.1  1995/06/07  23:14:09  hampton
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

#include "vines_private.h"
#include "vines_ipc.h"
#include "vines_debug.h"

#include "../parser/parser_actions.h"
#include "../vines/vines_actions.h"
#include "../parser/macros.h"
#include "parser_defs_vines.h"
#include "../parser/parser_defs_parser.h"
#include "../parser/parser_defs_priority.h"

/*
 * Vines config commands
 */
#define ALTERNATE NONE
#include        "cfg_vines.h"
LINK_POINT      (vines_config_commands, ALTERNATE);
#undef  ALTERNATE

/*
 * Vines config interface commands
 */
#define	ALTERNATE NONE
#include        "cfg_int_vines.h"
LINK_POINT(vines_interface_commands, ALTERNATE);
#undef  ALTERNATE

/*
 * Vines config routing commands
 */
#define ALTERNATE NONE
#include        "cfg_vines_routing.h"
LINK_POINT      (vines_config_routing_commands, ALTERNATE);
#undef	ALTERNATE

/*
 * Vines clear commands
 */
#define ALTERNATE NONE
#include        "exec_clear_vines.h"
LINK_POINT(vines_clear_commands, ALTERNATE);
#undef  ALTERNATE

/*
 * Vines debug commands
 */
#define ALTERNATE NONE
#include        "exec_debug_vines.h"
LINK_POINT(vines_debug_commands, ALTERNATE);
#undef  ALTERNATE

/*
 * Vines show commands
 */
#define ALTERNATE NONE
#include        "exec_show_vines.h"
LINK_POINT(vines_show_commands, ALTERNATE);
#undef  ALTERNATE

/*
 * Vines ping commands
 */
LINK_EXIT(exec_ping_vines_exit, no_alt);
KEYWORD_ID(exec_ping_vines, exec_ping_vines_exit, NONE,
	   OBJ(int,1), LINK_VINES, "vines", "Vines echo", PRIV_USER);
LINK_POINT(vines_ping_commands, exec_ping_vines);

/*
 * Vines priority queueing options
 */
#define	ALTERNATE	NONE
#include "cfg_priority_vines.h"
LINK_POINT(vines_priority_commands, ALTERNATE);
#undef	ALTERNATE


static parser_extension_request vinesparse_init_table[] = {
    { PARSE_ADD_CFG_TOP_CMD, &pname(vines_config_commands) },
    { PARSE_ADD_CFG_INT_CMD, &pname(vines_interface_commands) },
    { PARSE_ADD_CFG_ROUTING_CMD, &pname(vines_config_routing_commands) },
    { PARSE_ADD_CLEAR_CMD, &pname(vines_clear_commands) },
    { PARSE_ADD_DEBUG_CMD, &pname(vines_debug_commands) },
    { PARSE_ADD_SHOW_CMD, &pname(vines_show_commands) },
    { PARSE_ADD_PING_CMD, &pname(vines_ping_commands) },
    { PARSE_ADD_PRIORITY_OPTIONS, &pname(vines_priority_commands) },
    { PARSE_ADD_PING_EXIT,
	  (dynamic_transition *) &pname(exec_ping_vines_exit) },
    { PARSE_LIST_END, NULL },
};

void vines_parser_init (void)
{
    parser_add_protoaddr(LINK_VINES,		PMATCH_VINES PMATCH_DGRAM,
			 "vines",		"Banyan VINES",
			 0, ADDR_VINES);
    parser_add_address(ADDR_VINES, vines_addr_short_help, match_vines_addr);
    parser_add_command_list(vinesparse_init_table, "vines");
}
