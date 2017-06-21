/* $Id: dirresp_chain.c,v 3.1.18.3 1996/08/15 06:49:47 raj Exp $
 * $Source: /release/112/cvs/Xsys/servers/dirresp_chain.c,v $
 *------------------------------------------------------------------
 * Director-Responder parse chains
 *
 * Feburary 1996, Paul Traina
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: dirresp_chain.c,v $
 * Revision 3.1.18.3  1996/08/15  06:49:47  raj
 * CSCdi61197:  Part of  CSCdi49395 patches missing from 11.2
 * Branch: California_branch
 * Update Director Responder Agent to the latest version.
 *
 * Revision 3.1.18.2  1996/07/16  08:01:26  raj
 * CSCdi61197:  Part of  CSCdi49395 patches missing from 11.2
 * Branch: California_branch
 * Inserted patches which were missing and update dirresp code as well.
 *
 * Revision 3.1.18.1  1996/05/21  10:01:27  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.1  1996/02/20  22:46:00  pst
 * Initial incorporation of distributed directory agent support
 *
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "config.h"
#include "../ui/debug.h"
#include "parser.h"
#include "access.h"
#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "../parser/parser_defs_exec.h"
#include "../parser/parser_defs_tcpip.h"
#include "../parser/parser_defs_parser.h"

#include "dirresp_private.h"
#include "dirresp_debug.h"


/*
 * Parse chains for show commands
 */
#define ALTERNATE       NONE
#include "exec_show_ip_dirresp.h"
LINK_POINT(dirresp_show_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chains for configuration commands
 */
#define ALTERNATE       NONE
#include "cfg_ip_dirresp.h"
LINK_POINT(dirresp_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chains for debug commands
 */
#define ALTERNATE       NONE
#include "exec_debug_ip_dirresp.h"
LINK_POINT(dirresp_debug_commands, ALTERNATE);
#undef  ALTERNATE


/*
 * Parse chains for clear commands
 */
#define ALTERNATE       NONE
#include "exec_clear_ip_dirresp.h"
LINK_POINT(dirresp_clear_commands, ALTERNATE);
#undef  ALTERNATE


/*
 * Parse chain registration array
 */
parser_extension_request dirresp_chain_init_table[] = {
    { PARSE_ADD_SHOW_IP_CMD, &pname(dirresp_show_commands) },
    { PARSE_ADD_CFG_RTG_IP_CMD, &pname(dirresp_commands) },
    { PARSE_ADD_DEBUG_IP_CMD, &pname(dirresp_debug_commands) },
    { PARSE_ADD_CLEAR_IP_CMD, &pname(dirresp_clear_commands) },
    { PARSE_LIST_END, NULL }
};


/*
 * Initialize parser support
 */
void dirresp_parser_init (void)
{
    parser_add_command_list(dirresp_chain_init_table, "dirresp");
}
