/* $Id: pim_chain.c,v 3.8.10.2 1996/09/10 05:32:03 lwei Exp $
 * $Source: /release/112/cvs/Xsys/ipmulticast/pim_chain.c,v $
 *------------------------------------------------------------------
 * pim_chain.c - PIM parse chains.
 *
 * November 1993, Dino Farinacci.
 *
 * Copyright (c) 1993-1996, 1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: pim_chain.c,v $
 * Revision 3.8.10.2  1996/09/10  05:32:03  lwei
 * CSCdi68465:  RP information is too difficult to change in last-hop
 *              routers
 * Branch: California_branch
 *
 * Revision 3.8.10.1  1996/05/21  09:52:21  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.8  1996/02/22  14:34:55  widmer
 * CSCdi49281:  make all parser_extension_request static const
 * make parser_extension_request typedef const
 * make all parser_extension_request static
 *
 * Revision 3.7  1996/02/22  09:03:20  dino
 * CSCdi49566:  Allow control over which RPs are used in transit routers.
 *
 * Revision 3.6  1996/02/01  06:04:50  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.5  1995/12/17  19:24:29  dino
 * CSCdi44742:  PIM should only switch to the SPT when traffic threshold
 *              is reached
 *
 * Revision 3.4  1995/12/17  18:30:18  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.3  1995/11/21  23:12:16  dino
 * CSCdi43427:  cisco does not process DVMRP prunes.
 *
 * Revision 3.2  1995/11/17  17:30:21  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:01:50  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/08/03  01:13:03  chwhite
 * CSCdi38104:  It would be nice if the router supported mtrace/mrinfo.
 *              Added support for mbone traceroute and mrinfo
 *
 * Revision 2.2  1995/07/01  12:56:53  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize ip, ipmulticast, tcp
 *
 * Revision 2.1  1995/06/07  21:04:28  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <ciscolib.h>
#include "../ui/common_strings.h"
#include "logger.h"
#include "packet.h"
#include "interface_private.h"
#include "config.h"
#include "parser.h"
#include "access.h"
#include "../os/techsupport.h"
#include "../ui/debug.h"
#include "../parser/macros.h"
#include "../parser/parser_actions.h"
#include "../parser/parser_defs_parser.h"
#include "parser_defs_pim.h"
#include "parser_defs_igmp.h"
#include "../ip/ip.h"
#include "../ip/ipaccess.h"
#include "../iprouting/route.h"
#include "../ipmulticast/ipmulticast_debug.h"
#include "../ipmulticast/igmp.h"
#include "../ipmulticast/mbone_mtrace.h"
#include "../ipmulticast/mroute.h"
#include "../ipmulticast/pim.h"
#include "sd.h"

/*
 * Parse chains for PIM and general multicast debug commands.
 */
#define	ALTERNATE	NONE
#include "exec_debug_ip_pim.h"
LINK_POINT(pim_debug_commands, ALTERNATE);
#undef	ALTERNATE

/*
 * Parse chains for general IP multicast clear commands
 */
#define	ALTERNATE	NONE
#include "exec_clear_ip_mrouting.h"
LINK_POINT(ipmulticast_clear_commands, ALTERNATE);
#undef	ALTERNATE

/*
 * cisco multicast trace EXEC commands.
 */
#define ALTERNATE       NONE
#include "exec_mbone_mtrace.h"
LINK_POINT(mbone_mtrace_exec_command, ALTERNATE);
#undef ALTERNATE


/*
 * Parse chains for PIM show commands.
 */
#define	ALTERNATE	NONE
#include "exec_show_ip_pim.h"
LINK_POINT(pim_show_commands, ALTERNATE);
#undef	ALTERNATE

/*
 * Parse chains for IP multicast top-level commands.
 */
#define ALTERNATE       NONE
#include "cfg_ip_mrouting.h"
LINK_POINT(ipmrouting_global_commands, ALTERNATE);
#undef	ALTERNATE

/*
 * Parse chains for PIM top-level commands.
 */
#define ALTERNATE       NONE
#include "cfg_ip_pim.h"
LINK_POINT(pim_global_commands, ALTERNATE);
#undef	ALTERNATE

/*
 * Parse chains for DVMRP multicast top-level commands.
 */
#define ALTERNATE       NONE
#include "cfg_ip_dvmrp.h"
LINK_POINT(dvmrp_global_commands, ALTERNATE);
#undef	ALTERNATE

/*
 * Parse chains for PIM interface commands.
 */
#define ALTERNATE       NONE
#include "cfg_int_ip_pim.h"
LINK_POINT(pim_interface_commands, ALTERNATE);
#undef	ALTERNATE

/*
 * Parse chains for IP multicast interface commands.
 */
#define ALTERNATE       NONE
#include "cfg_int_ip_multicast.h"
LINK_POINT(ipmulticast_interface_commands, ALTERNATE);
#undef	ALTERNATE

/*
 * Parse chains for show techsupport ipmulticast command
 */
LINK_EXIT(show_ipmulticast_techsupport_exit, no_alt);
KEYWORD_ID(show_ipmulticast_techsupport, show_ipmulticast_techsupport_exit,
	   NONE, OBJ(int,2), TECHSUPPORT_IPMULTICAST, "ipmulticast",
	   "IP multicast related information", PRIV_USER);
LINK_POINT(ipmulticast_show_techsupport_commands, show_ipmulticast_techsupport);

/*
 * Parse chain registration array for PIM.
 */
static parser_extension_request pim_chain_init_table[] = {
    { PARSE_ADD_EXEC_CMD, &pname(mbone_mtrace_exec_command) },
    { PARSE_ADD_CLEAR_IP_CMD, &pname(ipmulticast_clear_commands) },
    { PARSE_ADD_DEBUG_IP_CMD, &pname(pim_debug_commands) },
    { PARSE_ADD_SHOW_IP_CMD, &pname(pim_show_commands) },
    { PARSE_ADD_CFG_TOP_IP_CMD, &pname(ipmrouting_global_commands) },
    { PARSE_ADD_CFG_TOP_IP_CMD, &pname(dvmrp_global_commands) },
    { PARSE_ADD_CFG_RTG_IP_CMD, &pname(pim_global_commands) },
    { PARSE_ADD_CFG_INT_IP_CMD, &pname(pim_interface_commands) },
    { PARSE_ADD_CFG_INT_IP_CMD, &pname(ipmulticast_interface_commands) },
    { PARSE_ADD_SHOW_TECHSUPPORT_CMD,
	&pname(ipmulticast_show_techsupport_commands) },
    { PARSE_ADD_SHOW_TECHSUPPORT_EXIT,
	(dynamic_transition *) &pname(show_ipmulticast_techsupport_exit) },
    { PARSE_LIST_END, NULL }
};


/*
 * pim_parser_init - Initialize PIM parser support.
 */
void pim_parser_init (void)
{
    parser_add_command_list(pim_chain_init_table, "pim");
}

