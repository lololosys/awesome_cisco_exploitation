/* $Id: ip_policy_chain.c,v 3.4.10.1 1996/05/21 09:53:19 thille Exp $
 * $Source: /release/112/cvs/Xsys/iprouting/ip_policy_chain.c,v $
 *------------------------------------------------------------------
 * ip_policy_chain.c -- Parser support for IP policy routing
 *
 * Feb 1995 Tony Li
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ip_policy_chain.c,v $
 * Revision 3.4.10.1  1996/05/21  09:53:19  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.4  1996/02/22  14:35:10  widmer
 * CSCdi49281:  make all parser_extension_request static const
 * make parser_extension_request typedef const
 * make all parser_extension_request static
 *
 * Revision 3.3  1995/12/17  18:30:58  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.2  1995/11/17  17:34:10  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:08:03  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.5  1995/10/19  08:05:58  tli
 * CSCdi41042:  IP policy: local policy fails, debugging obtuse
 * Local policy packets weren't being checksummed correctly.
 * Give debugging its own command.
 *
 * Revision 2.4  1995/08/19  22:22:03  tli
 * CSCdi36983:  Need policy routing for locally generated packets
 *
 * Revision 2.3  1995/07/12  07:19:11  tli
 * CSCdi36426:  Policy routing will not disable on an interface
 * - Policy routed packets were creating fastswitching cache entries.
 *   That's not till _next_ release.
 * - Add counters to record packet and byte count on a per route map entry
 *   basis.
 * - Marketing identified a catastrophic deficiency.  Implemented "set ip
 *   tos" and "set ip precedence" to cover this deficiency.
 *
 * Revision 2.2  1995/06/15  15:18:48  widmer
 * CSCdi35889:  parser transition structures should be static
 *
 * Revision 2.1  1995/06/07  21:07:01  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include "config.h"
#include "parser.h"
#include "../ui/debug.h"
#include "iprouting_debug.h"
#include "ip_policy.h"

#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "../parser/parser_defs_parser.h"
#include "../parser/parser_defs_access.h"

#include "../ip/ip.h"



/*
 * Parse chains for IP policy interface commands.
 */
#define	ALTERNATE	NONE
#include "cfg_int_ip_policy.h"
LINK_POINT(ip_policy_interface_commands, ALTERNATE);
#undef	ALTERNATE

/*
 * Parse chains for IP local policy commands.
 */
#define	ALTERNATE	NONE
#include "cfg_ip_local_policy.h"
LINK_POINT(ip_local_policy_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chains for IP policy specific route map commands.
 */
#define	ALTERNATE	NONE
#include "cfg_routemap_set_policy.h"
LINK_POINT(ip_policy_set_commands, ALTERNATE);
#undef	ALTERNATE

#define	ALTERNATE	NONE
#include "cfg_routemap_set_ip_policy.h"
LINK_POINT(ip_policy_set_ip_commands, ALTERNATE);
#undef	ALTERNATE

#define	ALTERNATE	NONE
#include "cfg_routemap_match_policy.h"
LINK_POINT(ip_policy_match_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chains for IP policy show commands.
 */
#define	ALTERNATE	NONE
#include "exec_show_ip_local_policy.h"
LINK_POINT(show_ip_local_policy_commands, ALTERNATE);
#undef	ALTERNATE

#define	ALTERNATE	NONE
#include "exec_show_ip_policy.h"
LINK_POINT(show_ip_policy_commands, ALTERNATE);
#undef	ALTERNATE

/*
 * Parse chains for policy routing debugging.
 */
#define	ALTERNATE	NONE
#include "exec_debug_ip_policy.h"
LINK_POINT(debug_ip_policy_commands, ALTERNATE);
#undef	ALTERNATE

/*
 * Parse chain registration array for IP policy routing
 */
static parser_extension_request ip_policy_chain_init_table[] = {
    { PARSE_ADD_CFG_INT_IP2_CMD, &pname(ip_policy_interface_commands) },
    { PARSE_ADD_CFG_ROUTEMAP_MATCH_CMD, &pname(ip_policy_match_commands) },
    { PARSE_ADD_CFG_ROUTEMAP_SET_CMD, &pname(ip_policy_set_commands) },
    { PARSE_ADD_CFG_ROUTEMAP_SET_IP_CMD, &pname(ip_policy_set_ip_commands) },
    { PARSE_ADD_CFG_RTMAP_SET_EXIT,
	  (dynamic_transition *) &pname(routemap_set_ippolicy_return) },
    { PARSE_ADD_CFG_RTMAP_SET_EXIT,
	  (dynamic_transition *) &pname(routemap_set_policy_return) },
    { PARSE_ADD_CFG_RTMAP_MATCH_EXIT,
	  (dynamic_transition *) &pname(routemap_match_policy_exit) },
    { PARSE_ADD_CFG_IP_LOCAL_CMD, &pname(ip_local_policy_commands) },
    { PARSE_ADD_SHOW_IP_LOCAL_CMD, &pname(show_ip_local_policy_commands) },
    { PARSE_ADD_SHOW_IP_CMD, &pname(show_ip_policy_commands) },
    { PARSE_ADD_DEBUG_IP_CMD, &pname(debug_ip_policy_commands) },
    { PARSE_LIST_END, NULL }
};


/*
 * ip_policy_parser_init - Initialize IP Routing parser support
 */
void ip_policy_parser_init (void)
{
    parser_add_command_list(ip_policy_chain_init_table, "ip_policy");
}
