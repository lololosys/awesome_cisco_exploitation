/* $Id: modem_chain.c,v 3.1.64.2 1996/07/11 01:11:33 styang Exp $
 * $Source: /release/112/cvs/Xsys/les/modem_chain.c,v $
 *------------------------------------------------------------------
 * modem_chain.c -- Parse chains for Modem Management
 *
 * November 1995, Viet Nguyen
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: modem_chain.c,v $
 * Revision 3.1.64.2  1996/07/11  01:11:33  styang
 * CSCdi62212:  code review clean up
 * Branch: California_branch
 * Code review clean up.
 *
 * Revision 3.1.64.1  1996/06/16  21:16:14  vnguyen
 * Finally committing Brasil to California_branch
 *
 * Revision 3.1  1995/12/21  23:39:27  vnguyen
 * Placeholders for new development.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "sys_registry.h"
#include "../h/cs_registry.regh"
#include "interface_private.h"
#include "packet.h"
#include "../if/network.h"
#include "config.h"
#include "parser.h"
#include "../ui/debug.h"

#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "../parser/parser_defs_exec.h"
#include "../parser/parser_defs_parser.h"
#include "parser_defs_modem.h"
#include "modem_parser.h"
#include "modem_debug.h"


/*
 * Parse chains for EXEC top level modem commands
 */
#define ALTERNATE NONE
#include "exec_modem.h"
LINK_POINT(modem_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chains for test modem back-to-back commands
 */
#define ALTERNATE NONE
#include "exec_test_modem.h"
LINK_POINT(test_modem_commands, ALTERNATE);
#undef	ALTERNATE

 
/*
 * Parse chains for EXEC top level show commands
 */
#define ALTERNATE NONE
#include "exec_show_modem.h"
LINK_POINT(show_modem_commands, ALTERNATE);
#undef  ALTERNATE
 

/*
 * Parse chains for EXEC top level clear commands
 */
#define ALTERNATE NONE
#include "exec_clear_modem.h"
LINK_POINT(clear_modem_commands, ALTERNATE);
#undef ALTERNATE


/*
 * Parse chains for DEBUG commands
 */
#define ALTERNATE NONE
#include "exec_debug_modem_csm.h"
LINK_POINT(debug_modem_commands, ALTERNATE);
#undef ALTERNATE


/*
 * Parse chains for modem global config commands
 */
#define ALTERNATE NONE
#include "cfg_modem.h"
LINK_POINT(modem_top_commands, ALTERNATE);
#undef ALTERNATE


/*
 * Parse chains for line modem commands
 */
#define ALTERNATE NONE
#include "cfg_line_modem_mgmt.h"
LINK_POINT(line_modem_commands, ALTERNATE);
#undef ALTERNATE


/*
 * Parse chains for copy tftp modem command
 */
#define ALTERNATE NONE
#include "exec_copy_tftp_modem.h"
LINK_POINT(copy_tftp_modem_command, ALTERNATE);
#undef ALTERNATE


/*
 * Parse chains for copy flash modem command
 */
#define ALTERNATE NONE
#include "exec_copy_flash_modem.h"
LINK_POINT(copy_flash_modem_command, ALTERNATE);
#undef ALTERNATE


/*
 * Parse chains for copy rcp modem command
 */
#define ALTERNATE NONE
#include "exec_copy_rcp_modem.h"
LINK_POINT(copy_rcp_modem_command, ALTERNATE);
#undef ALTERNATE

/*
 * Parse chain registration array for Modem Management
 */
static parser_extension_request modem_chain_init_table[] = {
    { PARSE_ADD_EXEC_CMD, &pname(modem_commands) },
    { PARSE_ADD_SHOW_CMD, &pname(show_modem_commands) },
    { PARSE_ADD_CLEAR_CMD, &pname(clear_modem_commands) },
    { PARSE_ADD_DEBUG_MODEM_CMD,&pname(debug_modem_commands) },
    { PARSE_ADD_CFG_TOP_CMD, &pname(modem_top_commands) },
    { PARSE_ADD_CFG_LINE_MODEM_CMD, &pname(line_modem_commands) },
    { PARSE_ADD_TEST_CMD, &pname(test_modem_commands) },
    { PARSE_ADD_COPY_CMD, &pname(copy_flash_modem_command) },
    { PARSE_ADD_COPY_CMD, &pname(copy_tftp_modem_command) },
    { PARSE_ADD_COPY_CMD, &pname(copy_rcp_modem_command) },
    { PARSE_LIST_END, NULL }
};

/*
 * modem_parser_init - Initialize parse chains for MODEM MGMT
 * 
 * This routine is called at system startup to register the parse chains
 * for MODEM MGMT.
 */

void modem_parser_init (void)
{
   parser_add_command_list(modem_chain_init_table, "modem");
}
