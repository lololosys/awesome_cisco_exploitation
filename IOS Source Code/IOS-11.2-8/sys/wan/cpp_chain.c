/* $Id: cpp_chain.c,v 3.1.2.2 1996/05/21 10:09:26 thille Exp $
 * $Source: /release/112/cvs/Xsys/wan/cpp_chain.c,v $
 *------------------------------------------------------------------
 * cpp parse chains
 *
 * October, 1995 - Bill May
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cpp_chain.c,v $
 * Revision 3.1.2.2  1996/05/21  10:09:26  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.1.2.1  1996/05/10  01:44:23  wmay
 * CPP and PPP half-bridge commit
 * Branch: California_branch
 *
 * Revision 3.1  1996/05/09  17:24:37  wmay
 * Placeholder for California branch
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include "config.h"
#include "../ui/debug.h"
#include "../h/interface_private.h"
#include "../h/interface.h"
#include "parser.h"
#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "../wan/cpp_debug.h"
#include "../h/packet.h"
#include "../wan/cpp.h"
#include "aaa.h"


/*
 * Parse chains for cpp debug commands
 */
#define	ALTERNATE	NONE
#include "exec_debug_cpp.h"
LINK_POINT(cpp_debug_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chains for cpp interface commands
 */
#define	ALTERNATE	NONE
#include "cfg_int_cpp.h"
LINK_POINT(cpp_interface_commands, ALTERNATE);
#undef	ALTERNATE

/*
 * Parse chain for CPP interface encapsulation command
 */
LINK_EXIT(cfg_encap_cpp_exit, no_alt);
ENCAPS	(cfg_encap_cpp, cfg_encap_cpp_exit, NONE,
	 OBJ(int,1), ET_CPP,
	 "cpp", "Combinet proprietary protocol",
	 (IDB_ISDN | IDB_DIALER));
LINK_POINT(cfg_encap_cpp_cmd, cfg_encap_cpp);

/*
 * Parse chain registration array for cpp commands
 */
static parser_extension_request cpp_chain_init_table[] = {
    { PARSE_ADD_DEBUG_CMD, &pname(cpp_debug_commands) },
    { PARSE_ADD_CFG_INT_AS_CMD, &pname(cpp_interface_commands) },
    { PARSE_ADD_INT_ENCAPS_CMD, &pname(cfg_encap_cpp_cmd) },
    { PARSE_ADD_INT_ENCAPS_EXIT,
	  (dynamic_transition *) &pname(cfg_encap_cpp_exit) },
    { PARSE_LIST_END, NULL }
};


/*
 * cpp_parser_init - Initialize parse structures for parsing
 * cpp commands
 */
void cpp_parser_init (void)
{
    parser_add_command_list(cpp_chain_init_table, "cpp");
}
