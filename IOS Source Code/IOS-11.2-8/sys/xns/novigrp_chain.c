/* $Id: novigrp_chain.c,v 3.1.2.3 1996/06/03 20:23:52 john Exp $
 * $Source: /release/112/cvs/Xsys/xns/novigrp_chain.c,v $
 *------------------------------------------------------------------
 * Parse chains for IPX EIGRP.
 *
 * April 1996, Steven Luong
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: novigrp_chain.c,v $
 * Revision 3.1.2.3  1996/06/03  20:23:52  john
 * CSCdi58748:  ipxwan and nlsp command nvgen order wrong and subsys issues
 * Branch: California_branch
 *
 * Revision 3.1.2.2  1996/05/21  10:11:49  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.1.2.1  1996/05/03  02:19:44  sluong
 * Branch: California_branch
 * IPX ACL Violation Logging, Plaining English Filters, Display SAP by
 * name,
 * NLSP MIBS, Scaleable RIP/SAP, Modularity, and SAP query by name.
 *
 * Revision 3.1  1996/04/13  01:24:52  sluong
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
#include "../parser/parser_defs_sse.h"
#include "../iprouting/parser_defs_igrp2.h"
#include "../clns/parser_defs_isis.h"
#include "../clns/clns_route.h"
#include "../clns/isis.h"

/*
 * Parse chains for DEBUG commands
 */
#define ALTERNATE NONE
#include "exec_debug_novigrp.h"
LINK_POINT(novigrp_debug_commands, ALTERNATE);
#undef ALTERNATE


/*
 * Parse chains for SHOW commands
 */
#define ALTERNATE NONE
#include "exec_show_novigrp.h"
LINK_POINT(novigrp_show_commands, ALTERNATE);
#undef ALTERNATE


#define ALTERNATE NONE
#include "cfg_novigrp.h"
LINK_POINT(novigrp_top_commands, ALTERNATE);
#undef ALTERNATE

/*
 * Parse chains for TEST commands
 */
#define ALTERNATE NONE
#include "exec_test_novigrp.h"
LINK_POINT(novigrp_test_commands, ALTERNATE);
#undef ALTERNATE

/*
 * Parse chains for interface commands
 */
#define ALTERNATE NONE
#include "cfg_int_novigrp.h"
LINK_POINT(novigrp_int_commands, ALTERNATE);
#undef ALTERNATE

/*
 * Parse chains for router commands
 */
#define ALTERNATE NONE
#include "cfg_nov_router_igrp.h"
LINK_POINT(novigrp_router_commands, ALTERNATE);
#undef ALTERNATE

/*
 * Parse chains for clear commands
 */
#define ALTERNATE NONE
#include "exec_clear_novigrp.h"
LINK_POINT(novigrp_clear_commands, ALTERNATE);
#undef ALTERNATE

/*
 * Parse chain registration array for IPX/EIGRP
 */
static parser_extension_request novigrp_chain_init_table[] = {
    { PARSE_ADD_CFG_TOP_IPX_CMD, &pname(novigrp_top_commands) },
    { PARSE_ADD_SHOW_IPX_CMD,&pname(novigrp_show_commands) },
    { PARSE_ADD_TEST_CMD, &pname(novigrp_test_commands) },
    { PARSE_ADD_CLEAR_IPX_CMD, &pname(novigrp_clear_commands) },
    { PARSE_ADD_DEBUG_IPX_CMD,&pname(novigrp_debug_commands) },
    { PARSE_ADD_CFG_INT_IPX_CMD,&pname(novigrp_int_commands) },
    { PARSE_ADD_ROUTER_IPX_CMD,&pname(novigrp_router_commands) }, 
    { PARSE_LIST_END, NULL }
};

/*
 * novigrp_parser_init - Initialize parse chains for NOVELL
 * 
 * This routine is called at system startup to register the parse chains
 * for NOVELL.
 */

void novigrp_parser_init (void)
{
   parser_add_command_list(novigrp_chain_init_table, "ipxeigrp");
}
