/* $Id: egp_chain.c,v 3.4.10.1 1996/05/21 09:53:13 thille Exp $
 * $Source: /release/112/cvs/Xsys/iprouting/egp_chain.c,v $
 *------------------------------------------------------------------
 * EGP parse chains
 *
 * May 1993, Robert Widmer
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: egp_chain.c,v $
 * Revision 3.4.10.1  1996/05/21  09:53:13  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.4  1996/02/22  14:35:03  widmer
 * CSCdi49281:  make all parser_extension_request static const
 * make parser_extension_request typedef const
 * make all parser_extension_request static
 *
 * Revision 3.3  1995/12/17  18:30:54  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.2  1995/11/17  17:33:00  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:06:51  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/06/19  10:50:29  tli
 * CSCdi36065:  Rationalize EGP parser file placement
 * Move parser support to iprouting subdirectory.
 *
 * Revision 2.1  1995/06/07  21:06:10  hampton
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
#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "../iprouting/route.h"
#include "../iprouting/iprouting_debug.h"


/*
 * Parse chains for EGP debug commands
 */
#define	ALTERNATE	NONE
#include "../iprouting/exec_debug_ip_egp.h"
LINK_POINT(egp_debug_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chains for EGP show commands
 */
#define	ALTERNATE	NONE
#include "../iprouting/exec_show_ip_egp.h"
LINK_POINT(egp_show_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chains for EGP config commands
 */
#define	ALTERNATE	NONE
#include "../iprouting/cfg_autonomous-system.h"
LINK_POINT(as_config_commands, ALTERNATE);
#undef	ALTERNATE

/*
 * Parse chains for EGP router timer subcommands
 */
#define ALTERNATE	NONE
#include "../iprouting/cfg_router_timers_egp.h"
LINK_POINT(egp_router_timer_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chain registration array for EGP
 */
static parser_extension_request egp_chain_init_table[] = {
    { PARSE_ADD_DEBUG_IP_CMD, &pname(egp_debug_commands) },
    { PARSE_ADD_SHOW_IP_CMD, &pname(egp_show_commands) },
    { PARSE_ADD_CFG_BEFORE_RTR_CMD, &pname(as_config_commands) },
    { PARSE_ADD_CFG_ROUTER_TIMER_CMD, &pname(egp_router_timer_commands) },
    { PARSE_LIST_END, NULL }
};


/*
 * egp_parser_init - Initialize EGP parser support
 */
void egp_parser_init (void)
{
    parser_add_command_list(egp_chain_init_table, "egp");
}
