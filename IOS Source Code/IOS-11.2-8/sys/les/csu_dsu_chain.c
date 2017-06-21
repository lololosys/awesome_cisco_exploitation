/* $Id: csu_dsu_chain.c,v 3.2 1996/03/29 03:49:33 sagarwal Exp $
 * $Source: /release/111/cvs/Xsys/les/csu_dsu_chain.c,v $
 *------------------------------------------------------------------
 * Service Module (CSU/DSU)  parse chains
 *
 * March 1996, Johnny Chan
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: csu_dsu_chain.c,v $
 * Revision 3.2  1996/03/29  03:49:33  sagarwal
 * CSCdi46172:  Add Boa support to 11.1 mainline
 *
 * Revision 3.1  1996/03/28  20:06:11  sagarwal
 * Placeholder files for 2500 Derivative with integrated CSU/DSU
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "config.h"
#include "sys_registry.h"
#include "media_registry.h"
#include "logger.h"
#include "../ui/debug.h"
#include "../h/interface_private.h"
#include "../h/interface.h"
#include "parser.h"
#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "../parser/parser_defs_parser.h"
#include "parser_defs_csu_dsu.h"
#include "if_les.h"
#include "init_les.h"
#include "if_les_hd64570.h"
#include "if_c3000_csu_dsu.h"
#include "csu_dsu_debug.h"


/*
 * Parse chains for Service Module interface commands
 */
#define	ALTERNATE	NONE
#include "cfg_int_csu_dsu.h"
LINK_POINT(csu_dsu_interface_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chains for Service Module debug commands
 */
#define ALTERNATE       NONE
#include "exec_debug_csu_dsu.h"
LINK_POINT(csu_dsu_debug_commands, ALTERNATE);
#undef  ALTERNATE


/*
 * Parse chains for Service Module test commands
 */
#define ALTERNATE       NONE
#include "exec_test_csu_dsu.h"
LINK_POINT(csu_dsu_test_commands, ALTERNATE);
#undef  ALTERNATE


/*
 * Parse chains for Service Module show commands
 */
#define ALTERNATE       NONE
#include "exec_show_csu_dsu.h"
LINK_POINT(csu_dsu_show_commands, ALTERNATE);
#undef  ALTERNATE


/*
 * Parse chains for Service Module clear commands
 */
#define ALTERNATE       NONE
#include "exec_clear_csu_dsu.h"
LINK_POINT(csu_dsu_clear_commands, ALTERNATE);
#undef  ALTERNATE


/*
 * Parse chain registration array for Service Module commands
 */
static parser_extension_request csu_dsu_chain_init_table[] = {
    { PARSE_ADD_CFG_INT_AS_CMD, &pname(csu_dsu_interface_commands) },
    { PARSE_ADD_DEBUG_CMD, &pname(csu_dsu_debug_commands) },
    { PARSE_ADD_TEST_CMD, &pname(csu_dsu_test_commands) },
    { PARSE_ADD_SHOW_CMD, &pname(csu_dsu_show_commands) },
    { PARSE_ADD_CLEAR_CMD, &pname(csu_dsu_clear_commands) },
    { PARSE_LIST_END, NULL }
};


/*
 * csu_dsu_parser_init - Initialize parse structures for parsing Service Module commands
 */
void csu_dsu_parser_init (void)
{
    parser_add_command_list(csu_dsu_chain_init_table, "service-module");
}
