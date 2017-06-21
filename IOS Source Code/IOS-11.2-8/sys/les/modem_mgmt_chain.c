/* $Id: modem_mgmt_chain.c,v 3.1.68.1 1996/06/16 21:16:25 vnguyen Exp $
 * $Source: /release/112/cvs/Xsys/les/modem_mgmt_chain.c,v $
 *------------------------------------------------------------------
 * A dummy file for brasil branch.
 * modem_mgmt_chain.c -- Parse chains for Modem Management
 *
 * November 1995, Jonathan Chiung-Hsi Lin
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: modem_mgmt_chain.c,v $
 * Revision 3.1.68.1  1996/06/16  21:16:25  vnguyen
 * Finally committing Brasil to California_branch
 *
 * Revision 3.1  1995/12/06  19:36:33  jchlin
 * Placeholders for new development.
 *
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
#include "master.h"
#include "sys_registry.h"
#include "interface_private.h"
#include "packet.h"
#include "../if/network.h"
#include "config.h"
#include "parser.h"
#include "../ui/debug.h"

extern int csm_make_bit_field(int, int);

#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "../parser/parser_defs_exec.h"
#include "../les/parser_defs_modem_mgmt.h"
#include "../parser/parser_defs_parser.h"


/*
 * Parse chains for SHOW commands
 */
#define ALTERNATE NONE
#include "exec_show_modem_mgmt.h"
LINK_POINT(modem_mgmt_show_commands, ALTERNATE);
#undef ALTERNATE


/*
 * Parse chains for EXEC top level modem-mgmt commands
 */
#define ALTERNATE NONE
#include "exec_modem_mgmt.h"
LINK_POINT(modem_mgmt_exec_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chain registration array for IPX/NLSP
 */
static parser_extension_request modem_mgmt_chain_init_table[] = {
    { PARSE_ADD_EXEC_CMD, &pname(modem_mgmt_exec_commands) },
    { PARSE_ADD_SHOW_CMD,&pname(modem_mgmt_show_commands) },
    { PARSE_LIST_END, NULL }
};

/*
 * modem_mgmt_parser_init - Initialize parse chains for MODEM MGMT
 * 
 * This routine is called at system startup to register the parse chains
 * for MODEM MGMT.
 */

void modem_mgmt_parser_init (void)
{
   parser_add_command_list(modem_mgmt_chain_init_table, "modem-mgmt");
}
