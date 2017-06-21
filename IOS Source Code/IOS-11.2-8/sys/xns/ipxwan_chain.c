/* $Id: ipxwan_chain.c,v 3.1.2.3 1996/06/03 20:23:48 john Exp $
 * $Source: /release/112/cvs/Xsys/xns/ipxwan_chain.c,v $
 *------------------------------------------------------------------
 * Parse chains for IPX WAN.
 *
 * April 1996, Steven Luong
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ipxwan_chain.c,v $
 * Revision 3.1.2.3  1996/06/03  20:23:48  john
 * CSCdi58748:  ipxwan and nlsp command nvgen order wrong and subsys issues
 * Branch: California_branch
 *
 * Revision 3.1.2.2  1996/05/21  10:11:22  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.1.2.1  1996/05/03  02:17:04  sluong
 * Branch: California_branch
 * IPX ACL Violation Logging, Plaining English Filters, Display SAP by
 * name,
 * NLSP MIBS, Scaleable RIP/SAP, Modularity, and SAP query by name.
 *
 * Revision 3.1  1996/04/13  01:24:53  sluong
 * Add placeholders
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include "interface_private.h"
#include "packet.h"
#include "../if/network.h"
#include "config.h"
#include "parser.h"
#include "../iprouting/route.h"
#include "../xns/novell.h"
#include "../xns/novroute.h"
#include "../ui/debug.h"
#include "xns.h"
#include "novell.h"
#include "novell_debug.h"

#include "../parser/parser_actions.h"
#include "../xns/novell_actions.h"
#include "../parser/macros.h"
#include "../parser/parser_defs_exec.h"
#include "../xns/parser_defs_xns_nov.h"
#include "../parser/parser_defs_exec.h"
#include "../parser/parser_defs_parser.h"

/*
 * Parse chains for DEBUG commands
 */
#define ALTERNATE NONE
#include "exec_debug_ipxwan.h"
LINK_POINT(ipxwan_debug_commands, ALTERNATE);
#undef ALTERNATE


/*
 * Parse chains for interface commands
 */
#define ALTERNATE NONE
#include "cfg_int_ipxwan.h"
LINK_POINT(ipxwan_int_commands, ALTERNATE);
#undef ALTERNATE

/*
 * Parse chain registration array for IPX/EIGRP
 */
static parser_extension_request ipxwan_chain_init_table[] = {
    { PARSE_ADD_DEBUG_IPX_CMD,&pname(ipxwan_debug_commands) },
    { PARSE_ADD_CFG_INT_IPX_CMD,&pname(ipxwan_int_commands) },
    { PARSE_LIST_END, NULL }
};

/*
 * ipxwan_parser_init - Initialize parse chains for NOVELL
 * 
 * This routine is called at system startup to register the parse chains
 * for NOVELL.
 */

void ipxwan_parser_init (void)
{
   parser_add_command_list(ipxwan_chain_init_table, "ipxwan");
}
