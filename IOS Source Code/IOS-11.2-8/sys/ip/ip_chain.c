/* $Id: ip_chain.c,v 3.7.4.3 1996/05/30 23:43:58 hampton Exp $
 * $Source: /release/112/cvs/Xsys/ip/ip_chain.c,v $
 *------------------------------------------------------------------
 * IP parse chains
 *
 * July 1993, Robert Widmer
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: ip_chain.c,v $
 * Revision 3.7.4.3  1996/05/30  23:43:58  hampton
 * All commands that take access list numbers should accept either numbers
 * in the current monolithic number space, or hidden numbers in the future
 * per-protocol access list spaces.  [CSCdi59067]
 * Branch: California_branch
 *
 * Revision 3.7.4.2  1996/05/21  09:51:05  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.7.4.1  1996/04/15  21:15:49  bcole
 * CSCdi54649:  IP Host Modularity, IP Automatic Stub Routing feature
 * commit
 * Branch: California_branch
 *
 * Revision 3.7  1996/03/09  22:55:35  widmer
 * CSCdi47890:  HTTP server doesnt work
 * Add HTTP server
 *
 * Revision 3.6  1996/02/22  14:34:27  widmer
 * CSCdi49281:  make all parser_extension_request static const
 * make parser_extension_request typedef const
 * make all parser_extension_request static
 *
 * Revision 3.5  1996/02/01  06:04:19  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.4  1995/12/17  18:29:27  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.3  1995/12/01  15:56:26  widmer
 * CSCdi44724:  Remove nv_top argument to parser_add_mode()
 * Clean up "no" handling at beginning of modes
 * Fix appn modes
 * Removed unneded pdecl's
 *
 * Revision 3.2  1995/11/17  09:33:21  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:55:17  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.7  1995/11/08  21:04:09  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.6  1995/09/25  08:31:56  tli
 * CSCdi40960:  ISP jumbo patch 2
 *
 * Revision 2.5  1995/08/17  01:43:18  tli
 * CSCdi38878:  DHCP: change ip local-pool to ip local pool
 *
 * Revision 2.4  1995/07/01  12:55:48  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize ip, ipmulticast, tcp
 *
 * Revision 2.3  1995/06/15  15:18:35  widmer
 * CSCdi35889:  parser transition structures should be static
 *
 * Revision 2.2  1995/06/08  15:54:26  widmer
 * CSCdi35497:  Remove old style debugging
 *
 * Revision 2.1  1995/06/07  20:56:59  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include "interface_private.h"
#include "config.h"
#include "parser.h"
#include "registry.h"
#include "sys_registry.h"
#include "../if/network.h"
#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "../ui/debug.h"
#include "../ip/ip_debug.h"
#include "access.h"
#include "../parser/parser_defs_parser.h"
#include "../parser/parser_defs_exec.h"
#include "../parser/parser_defs_tcpip.h"
#include "parser_defs_arp.h"
#include "../parser/parser_defs_config.h"
#include "../parser/parser_defs_priority.h"
#include "../wan/parser_defs_frame.h"
#include "packet.h"
#include "../ip/ip.h"
#include "../ip/ipoptions.h"
#include "../parser/parser_defs_snmp.h"
#include "ip_actions.h"
#include "ip_macros.h"
#include "media_registry.h"
#include "../tcp/rcp.h"
#include "ipaccess_private.h"
#include "ipaccess.h"


/*
 * Parse chains for IP clear commands
 */
#define	ALTERNATE	NONE
#include "exec_clear_ip.h"
LINK_POINT(ip_clear_commands, ALTERNATE);
#undef	ALTERNATE

#define	ALTERNATE	NONE
#define	OALTERNATE	NONE
#include "exec_clear_access.h"
LINK_POINT(ip_clear_acl_commands, ALTERNATE);
LINK_POINT(oip_clear_acl_commands, OALTERNATE);
#undef	OALTERNATE
#undef	ALTERNATE


/*
 * Parse chains for IP debug commands 
 */
#define	ALTERNATE	NONE
#include "exec_debug_ip.h"
#include "exec_debug_domain.h"
LINK_POINT(ip_debug_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chains for IP test commands
 */
#define ALTERNATE       NONE
#include "exec_test_ip.h"
LINK_POINT(ip_test_commands, ALTERNATE);
#undef ALTERNATE


/*
 * Parse chains for IP show commands
 */
#define	ALTERNATE	NONE
#include "exec_show_ip.h"
#include "exec_show_arp.h"
LINK_POINT(ip_show_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chains for IP global config commands
 */
#define	ALTERNATE	NONE
#include "cfg_ip.h"
LINK_POINT(ip_config_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chains for IP related routing config commands
 */
#define	ALTERNATE	NONE
#include "cfg_arp.h"
LINK_POINT(ip_related_routing_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chains for IP routing config commands
 */
#define	ALTERNATE	NONE
#include "cfg_ip_routing.h"
LINK_POINT(ip_routing_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chains for IP interface1 commands
 */
#define	ALTERNATE	NONE
#include "cfg_int_ip.h"
LINK_POINT(ip_interface1_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chains for IP interface2 commands
 */
#define	ALTERNATE	NONE
#include "cfg_int_arp.h"
#include "cfg_int_ip2.h"
LINK_POINT(ip_interface2_commands, ALTERNATE);
#undef	ALTERNATE

/*
 * Parse chains for exec commands
 */
#define	ALTERNATE	NONE
#include "exec_tunnel.h"
#include "exec_tempacl.h"
LINK_POINT(ip_connect_exec_commands, ALTERNATE);
#undef	ALTERNATE

/*
 * Parse chain for the NULL interface
 */
#define	ALTERNATE	NONE
#include "cfg_int_null.h"
LINK_POINT(ip_null_int_commands, ALTERNATE);
#undef	ALTERNATE

/*
 * Parse chain for IP terminal commands
 */
#define	ALTERNATE	NONE
#include "cfg_line_ip.h"
LINK_POINT(ip_line_config_commands, ALTERNATE);
#undef	ALTERNATE

/*
 * Parse chain to extend priority queueing
 */
#define	ALTERNATE	NONE
#include "cfg_priority_ip.h"
LINK_POINT(ip_priority_commands, ALTERNATE);
#undef	ALTERNATE

/*
 * Parse chain to extend frame relay
 */
#define	ALTERNATE	NONE
#include "cfg_frame_ip.h"
LINK_POINT(ip_frame_relay_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chain registration array for IP
 */
static parser_extension_request ip_chain_init_table[] = {
    { PARSE_ADD_EXEC_CMD, &pname(ip_connect_exec_commands) },
    { PARSE_ADD_CLEAR_CMD, &pname(ip_clear_commands) },
    { PARSE_ADD_CLEAR_CMD, &pname(oip_clear_acl_commands) }, /* Old */
    { PARSE_ADD_CLEAR_IP_CMD, &pname(ip_clear_acl_commands) },
    { PARSE_ADD_DEBUG_CMD, &pname(ip_debug_commands) },
    { PARSE_ADD_TEST_CMD, &pname(ip_test_commands) },
    { PARSE_ADD_SHOW_CMD, &pname(ip_show_commands) },
    { PARSE_ADD_CFG_TOP_CMD, &pname(ip_config_commands) },
    { PARSE_ADD_CFG_ROUTING_CMD, &pname(ip_related_routing_commands) },
    { PARSE_ADD_ROUTING_IP_CMD, &pname(ip_routing_commands) },
    { PARSE_ADD_INTERFACE_IP1_CMD, &pname(ip_interface1_commands) },
    { PARSE_ADD_INTERFACE_IP2_CMD, &pname(ip_interface2_commands) },
    { PARSE_ADD_NULLINT_CMD, &pname(ip_null_int_commands) },
    { PARSE_ADD_LINE_TERM_CMD, &pname(ip_line_config_commands) },
    { PARSE_ADD_PRIORITY_OPTIONS, &pname(ip_priority_commands) },
    { PARSE_ADD_FR_DELIST_OPTIONS, &pname(ip_frame_relay_commands) },
    { PARSE_LIST_END, NULL }
};


/*
 * ip_parser_init - Initialize IP parser support
 */
void ip_parser_init (void)
{
    parser_add_protoaddr(LINK_ARP,		PMATCH_ARP PMATCH_DGRAM,
			 "arp",		"IP ARP",
			 0, ADDR_ILLEGAL);
    parser_add_link_point(PARSE_ADD_CLEAR_IP_CMD, "clear ip",
			  &pname(clear_ip_extend_here));
    parser_add_link_point(PARSE_ADD_DEBUG_IP_CMD, "debug ip",
			  &pname(debug_ip_extend_here));
    parser_add_link_point(PARSE_ADD_TEST_IP_CMD, "test ip",
			  &pname(test_ip_extend_here));
    parser_add_link_point(PARSE_ADD_SHOW_IP_CMD, "show ip",
			  &pname(show_ip_extend_here));
    parser_add_link_point(PARSE_ADD_CFG_TOP_IP_CMD, "config ip",
			  &pname(cfg_ip_extend_here));
    parser_add_link_point(PARSE_ADD_CFG_INT_IP_CMD, "interface config ip",
			  &pname(ci_ip_extend_here));
    parser_add_link_point(PARSE_ADD_CFG_INT_IP2_CMD, "interface config ip2",
			  &pname(ci_ip2_extend_here));
    parser_add_link_point(PARSE_ADD_CFG_RTG_IP_CMD, "routing config ip",
			  &pname(cr_ip_extend_here));
    parser_add_link_point(PARSE_ADD_CFG_INT_IPROUTEC_CMD,
			  "int ip route-cache",
			  &pname(ci_ip2_routecache_extend_here));
    parser_add_link_point(PARSE_ADD_CFG_IP_LOCAL_CMD,
			  "ip local",
			  &pname(cfg_ip_local_extend_here));
    parser_add_link_point(PARSE_ADD_SHOW_IP_LOCAL_CMD,
			  "ip local",
			  &pname(show_ip_local_extend_here));
    parser_add_link_exit(PARSE_ADD_CFG_INT_IPROUTEC_EXIT,
			 "int ip route-cache return",
			 &pname(ci_ip2_routecache_return_here));
    parser_add_command_list(ip_chain_init_table, "ip");
}
