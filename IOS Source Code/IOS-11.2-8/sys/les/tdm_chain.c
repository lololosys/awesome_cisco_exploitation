/* $Id: tdm_chain.c,v 3.1.40.1 1996/06/16 21:16:54 vnguyen Exp $
 * $Source: /release/112/cvs/Xsys/les/tdm_chain.c,v $
 *------------------------------------------------------------------
 * tdm_chain.c -- TDM parse chains.
 *
 * February 1996, Joe Turner
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: tdm_chain.c,v $
 * Revision 3.1.40.1  1996/06/16  21:16:54  vnguyen
 * Finally committing Brasil to California_branch
 *
 * Revision 3.1  1996/02/02  22:50:12  jturner
 * Placeholder for new development.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "logger.h"
#include "config.h"
#include "parser.h"
#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "../parser/parser_defs_exec.h"
#include "parser_defs_tdm.h"
#include "../ui/debug.h"
#include "if_as5200_tdm.h"
#include "tdm_debug.h"
#include "tdm_parser.h"

/*
 * Parse chains for TDM show commands
 */
#define	ALTERNATE	NONE
#include "exec_show_tdm.h"
LINK_POINT(tdm_show_commands, ALTERNATE);
#undef	ALTERNATE

/*
 * Parse chains for TDM debug commands
 */
#define	ALTERNATE	NONE
#include "exec_debug_tdm.h"
LINK_POINT(tdm_debug_commands, ALTERNATE);
#undef	ALTERNATE

static parser_extension_request tdm_chain_init_table[] = {
    { PARSE_ADD_SHOW_CMD, &pname(tdm_show_commands) },
    { PARSE_ADD_DEBUG_CMD, &pname(tdm_debug_commands) },
    { PARSE_LIST_END, NULL }
};

void tdm_parser_init (void)
{
    parser_add_command_list(tdm_chain_init_table, "tdm");
}
