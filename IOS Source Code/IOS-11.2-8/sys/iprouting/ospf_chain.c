/* $Id: ospf_chain.c,v 3.5.10.1 1996/05/21 09:53:26 thille Exp $
 * $Source: /release/112/cvs/Xsys/iprouting/ospf_chain.c,v $
 *------------------------------------------------------------------
 * OSPF parse chains
 *
 * May 1993, Robert Widmer
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: ospf_chain.c,v $
 * Revision 3.5.10.1  1996/05/21  09:53:26  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.5  1996/02/22  14:35:21  widmer
 * CSCdi49281:  make all parser_extension_request static const
 * make parser_extension_request typedef const
 * make all parser_extension_request static
 *
 * Revision 3.4  1996/02/01  06:05:50  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.3  1995/12/17  18:31:04  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.2  1995/11/17  17:35:47  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:09:31  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/09/05  22:08:59  myeung
 * CSCdi39807:  OSPF need access-list debugging
 * - Add access-list debugging for SPF calculation and flooding
 * - Add separate debugging for intra-area, inter-area and external SPF
 *
 * Revision 2.3  1995/07/01  12:16:31  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize iprouting
 *
 * Revision 2.2  1995/06/08  15:55:06  widmer
 * CSCdi35497:  Remove old style debugging
 *
 * Revision 2.1  1995/06/07  21:09:13  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include "config.h"
#include "login.h"
#include "../ui/debug.h"
#include "parser.h"
#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "parser_defs_ospf.h"
#include "../parser/parser_defs_parser.h"
#include "parser_defs_router.h"
#include "../util/tree.h"
#include "../iprouting/route.h"
#include "../iprouting/ospf_pak.h"
#include "../iprouting/ospf_dbase.h"
#include "../iprouting/ospf.h"
#include "../parser/parser_defs_tcpip.h"
#include "../h/access.h"
#include "../iprouting/iprouting_debug.h"
#include "access.h"


/*
 * Parse chains for OSPF clear commands
 */
#define	ALTERNATE	NONE
#include "exec_clear_ip_ospf.h"
LINK_POINT(ospf_clear_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chains for OSPF debug commands
 */
#define	ALTERNATE	NONE
#include "exec_debug_ip_ospf.h"
LINK_POINT(ospf_debug_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chains for OSPF show commands
 */
#define	ALTERNATE	NONE
#include "exec_show_ip_ospf.h"
LINK_POINT(ospf_show_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chains for OSPF interface commands
 */
#define ALTERNATE       NONE
#include "cfg_int_ip_ospf.h"
LINK_POINT(ospf_interface_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chains for OSPF routing commands
 */
#define	ALTERNATE	NONE
#include "cfg_ip_routing_ospf.h"
LINK_POINT(ospf_iprouting_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chains for OSPF specific router subcommands
 */
#define ALTERNATE NONE
#include "cfg_router_ospf.h"
#include "cfg_router_area.h"
LINK_POINT(ospf_router_commands, ALTERNATE);
#undef  ALTERNATE


/*
 * Parse chains for OSPF specific router neighbor subcommands
 */
#define ALTERNATE NONE
#include "cfg_router_neighbor_ospf.h"
LINK_POINT(ospf_router_neigh_commands, ALTERNATE);
#undef  ALTERNATE


/*
 * Parse chains for OSPF specific router timer commands
 */
#define	ALTERNATE NONE
#include "cfg_router_timers_ospf.h"
LINK_POINT(ospf_router_timer_commands, ALTERNATE);
#undef	ALTERNATE

/*
 * Parse chains for OSPF specific router distance subcommands
 */
#define	ALTERNATE	NONE
#include "cfg_router_distance_ospf.h"
LINK_POINT(ospf_router_distance_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chain registration array for OSPF
 */
static parser_extension_request ospf_chain_init_table[] = {
    { PARSE_ADD_CLEAR_IP_CMD, &pname(ospf_clear_commands) },
    { PARSE_ADD_DEBUG_IP_CMD, &pname(ospf_debug_commands) },
    { PARSE_ADD_SHOW_IP_CMD, &pname(ospf_show_commands) },
    { PARSE_ADD_CFG_INT_IP2_CMD, &pname(ospf_interface_commands) },
    { PARSE_ADD_CFG_RTG_IP_CMD, &pname(ospf_iprouting_commands) },
    { PARSE_ADD_CFG_ROUTER_CMD, &pname(ospf_router_commands) },
    { PARSE_ADD_CFG_ROUTER_NEIGHBOR_CMD, &pname(ospf_router_neigh_commands) },
    { PARSE_ADD_CFG_ROUTER_TIMER_CMD, &pname(ospf_router_timer_commands) },
    { PARSE_ADD_CFG_ROUTER_DISTANCE_CMD, 
      &pname(ospf_router_distance_commands) },
    { PARSE_LIST_END, NULL }
};


/*
 * ospf_parser_init - Initialize OSPF parser support
 */
void ospf_parser_init (void)
{
    parser_add_command_list(ospf_chain_init_table, "ospf");
}
