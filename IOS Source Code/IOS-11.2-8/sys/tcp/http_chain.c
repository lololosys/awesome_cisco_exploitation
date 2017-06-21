/* $Id: http_chain.c,v 3.2.4.2 1996/05/21 10:06:16 thille Exp $
 * $Source: /release/112/cvs/Xsys/tcp/http_chain.c,v $
 *------------------------------------------------------------------
 * http_chain.c -- HTTP commands
 *
 * July 1995 Tony Li
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: http_chain.c,v $
 * Revision 3.2.4.2  1996/05/21  10:06:16  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.2.4.1  1996/05/06  17:39:57  widmer
 * CSCdi56492:  Need to limit access to http server with access-list
 * Branch: California_branch
 * Add "ip http access-class" command
 *
 * Revision 3.2  1996/03/09  22:57:38  widmer
 * CSCdi47890:  HTTP server doesnt work
 * Add HTTP server
 *
 * Revision 3.1  1996/02/11  04:25:52  widmer
 * CSCdi47890:  HTTP server doesnt work
 * Add placeholder files
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include "interface_private.h"
#include "config.h"
#include "parser.h"
#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "../ui/debug.h"
#include "http_debug.h"
#include "http_parser_defs.h"
#include "access.h"

/*
 * Parse chains for HTTP debug commands 
 */
#define	ALTERNATE	NONE
#include "exec_debug_ip_http.h"
LINK_POINT(http_debug_commands, ALTERNATE);
#undef	ALTERNATE

/*
 * Parse chains for HTTP config commands
 */
#define	ALTERNATE	NONE
#include "cfg_http.h"
LINK_POINT(http_config_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chain registration array for HTTP
 */
static parser_extension_request http_chain_init_table[] = {
    { PARSE_ADD_DEBUG_IP_CMD, &pname(http_debug_commands) },
    { PARSE_ADD_CFG_RTG_IP_CMD, &pname(http_config_commands) },
    { PARSE_LIST_END, NULL }
};


/*
 * http_parser_init - Initialize HTTP parser support
 */
void http_parser_init (void)
{
    parser_add_command_list(http_chain_init_table, "http");
}
