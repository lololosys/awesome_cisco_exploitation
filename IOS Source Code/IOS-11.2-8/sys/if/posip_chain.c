/* $Id: posip_chain.c,v 3.1.2.1 1996/07/08 22:31:17 mwu Exp $
 * $Source: /release/112/cvs/Xsys/if/posip_chain.c,v $
 *------------------------------------------------------------------
 * posip_chain.c - POSIP Parser chain registration
 *
 * January 1996, Robert Broberg
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: posip_chain.c,v $
 * Revision 3.1.2.1  1996/07/08  22:31:17  mwu
 * CSCdi60922:  11.2 Core Features Commit
 * Branch: California_branch
 *
 * Revision 3.1  1996/06/07  05:31:31  rbroberg
 * Add placeholders for new project
 *
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#include "master.h"
#include "interface_private.h"
#include "config.h"
#include "parser.h"

#include "../wan/serial.h"

#include "../hes/cbus_registry.h"
#include "../hes/parser_defs_fci.h"
#include "../hes/if_hes_serial.h"

#include "../ui/debug.h"

#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "../parser/parser_defs_parser.h"


/*
 * Parse chains for packet over sonet interface commands
 */
#define	ALTERNATE	NONE
#include "cfg_int_posip.h"
LINK_POINT(posip_interface_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chain registration array for Packet over Sonet
 */
static parser_extension_request posip_chain_init_table[] = {
    { PARSE_ADD_CFG_INT_AS_CMD, &pname(posip_interface_commands) },
    { PARSE_LIST_END, NULL }
};


/*
 * posip__parser_init - Initialize POSIP parser support
 */
void posip_parser_init (void)
{
    static boolean posip_parser_initialized = FALSE;

    if (!posip_parser_initialized) {
	parser_add_command_list(posip_chain_init_table, 
				"Posi");
	posip_parser_initialized = TRUE;
    }
}


