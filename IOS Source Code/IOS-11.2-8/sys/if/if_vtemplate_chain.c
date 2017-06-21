/* $Id: if_vtemplate_chain.c,v 3.1.34.3 1996/07/04 02:23:35 perryl Exp $
 * $Source: /release/112/cvs/Xsys/if/if_vtemplate_chain.c,v $
 *------------------------------------------------------------------
 * if_vtemplate_chain.c - Debug support for Virtual Template 
 * 
 * Febuary 1996, Perry Leung
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: if_vtemplate_chain.c,v $
 * Revision 3.1.34.3  1996/07/04  02:23:35  perryl
 * CSCdi61809:  Reconfiguring virtual template causes configuration errors
 * Branch: California_branch
 * Add "clear vtemplate" command to reset the vtemplate command history
 * buffer.
 *
 * Revision 3.1.34.2  1996/05/21  09:49:53  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.1.34.1  1996/04/27  05:49:45  syiu
 * Branch: California_branch
 * Commit Dial1_branch
 *
 * Revision 3.1.2.2  1996/04/17  01:59:32  syiu
 * Branch: Dial1_branch
 * Sync Dial1_branch to V111_1_3
 *
 * Revision 3.1.2.1  1996/02/10  02:19:44  perryl
 * Branch: Dial1_branch
 * Add debug support for Virtual Template
 *
 * Revision 3.1  1996/02/10  00:27:06  perryl
 * Placeholder for Dial1_branch
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include "interface_private.h"
#include "config.h"
#include "subsys.h"
#include "parser.h"
#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "../parser/parser_defs_exec.h"
#include "../parser/parser_defs_parser.h"
#include "../ui/debug.h"
#include "../if/if_vtemplate_debug.h"

extern void vtemplate_clear_cmdbuf(parseinfo *);


/*
 * Parse chains for VTEMPLATE clear commands
 */
#define ALTERNATE       NONE
#include "exec_clear_vtemplate.h"
LINK_POINT(vtemplate_clear_commands, ALTERNATE);
#undef	ALTERNATE

/*
 * Parse chains for VTEMPLATE debug commands
 */
#define	ALTERNATE	NONE
#include "exec_vtemplate_debug.h"
LINK_POINT(vtemplate_debug_commands, ALTERNATE);
#undef  ALTERNATE


/*
 * Parse chain registration array for VTEMPLATE commands
 */
static parser_extension_request vtemplate_chain_init_table[] = {
    { PARSE_ADD_CLEAR_CMD, &pname(vtemplate_clear_commands) },
    { PARSE_ADD_DEBUG_CMD, &pname(vtemplate_debug_commands) },
    { PARSE_LIST_END, NULL }
};


/*
 * vtemplate_parser_init
 */
void vtemplate_parser_init (void)
{
    parser_add_command_list(vtemplate_chain_init_table, "vtemplate");
}
