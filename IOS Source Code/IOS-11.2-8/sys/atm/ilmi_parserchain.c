/* $Id: ilmi_parserchain.c,v 3.1.62.2 1996/05/21 09:43:03 thille Exp $
 * $Source: /release/112/cvs/Xsys/atm/ilmi_parserchain.c,v $
 *------------------------------------------------------------------
 * ILMI Parser commands
 *
 * October 1995, Kartik Chandrasekhar
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ilmi_parserchain.c,v $
 * Revision 3.1.62.2  1996/05/21  09:43:03  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.1.62.1  1996/04/25  23:02:47  ronnie
 * CSCdi55766:  Hey!  This ATM and Frame Relay is no different than in 11.1
 * Feature commit for ATM and Frame Relay into 11.2
 * Branch: California_branch
 *
 * Revision 3.1.66.1  1996/03/30  03:35:04  rzagst
 * ATM Modularity: Clean up and modularize by platform the ATM parser
 * commands.
 * Branch: ATM_Cal_branch
 *
 * Revision 3.1  1995/11/09  11:00:48  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/10/17  19:09:39  kartik
 * placeholder files for the Rhino Branch.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
#include "master.h"
#include "../ui/common_strings.h"
#include "sys_registry.h"
#include "media_registry.h"
#include "config.h"
#include "../ui/debug.h"
#include "parser.h"
#include "../h/interface.h"
#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "../parser/parser_defs_parser.h"
#include "../atm/parser_defs_ilmi.h"
#include "../if/atm_debug.h"
#include "ilmi_public.h"
#include "ilmi_private.h"

#define ALTERNATE       NONE
#include "exec_debug_ilmi.h"
LINK_POINT(ilmi_debug_commands, ALTERNATE);
#undef ALTERNATE

#define	ALTERNATE	NONE
#include "exec_show_ilmi.h"
LINK_POINT(ilmi_show_commands, ALTERNATE);
#undef  ALTERNATE

#define ALTERNATE NONE
#include "cfg_int_ilmi.h"
LINK_POINT(ilmi_int_cfg_commands,ALTERNATE );
#undef ALTERNATE

/*
 * Parse chain registration array for ATM commands
 */
static parser_extension_request ilmi_chain_init_table[] ={
    { PARSE_ADD_DEBUG_INT_ATM_CMD, &pname(ilmi_debug_commands) },
    { PARSE_ADD_SHOW_INT_ATM_CMD, &pname(ilmi_show_commands) },
    { PARSE_ADD_CFG_INT_ATM_CMD,&pname(ilmi_int_cfg_commands) },
    { PARSE_LIST_END, NULL }
};

/*
 * ilmi_parser_init - Initialize parse structures for parsing ATM commands
 */
void ilmi_parser_init (void)
{
    parser_add_command_list(ilmi_chain_init_table, "ATM");
}
