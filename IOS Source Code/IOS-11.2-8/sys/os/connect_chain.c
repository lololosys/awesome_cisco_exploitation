/* $Id: connect_chain.c,v 3.5.10.1 1996/05/21 09:58:08 thille Exp $
 * $Source: /release/112/cvs/Xsys/os/connect_chain.c,v $
 *------------------------------------------------------------------
 * Connect parse chains
 *
 * May 1993, Robert Widmer
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: connect_chain.c,v $
 * Revision 3.5.10.1  1996/05/21  09:58:08  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.5  1996/02/22  14:36:35  widmer
 * CSCdi49281:  make all parser_extension_request static const
 * make parser_extension_request typedef const
 * make all parser_extension_request static
 *
 * Revision 3.4  1995/12/17  18:34:12  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.3  1995/11/17  18:46:16  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:17:30  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:44:57  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/07/02  10:51:05  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize os, hes, les, if, and ibm.
 *
 * Revision 2.1  1995/06/07  21:54:27  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include "ttysrv.h"
#include "config.h"
#include "parser.h"
#include "packet.h"
#include "../ip/ip.h"
#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "../parser/parser_defs_parser.h"
#include "../parser/parser_defs_exec.h"
#include "../parser/parser_defs_config.h"
#include "../parser/parser_defs_misc.h"


/*
 * Parse chains for Connect exec commands
 */
#define	ALTERNATE	NONE
#include "exec_where.h"
#include "exec_resume.h"
#include "exec_name-connection.h"
#include "exec_implicit.h"
#include "exec_disconnect.h"
LINK_POINT(connect_exec_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chains for Connect show commands
 */
#define	ALTERNATE	NONE
#include "exec_show_sessions.h"
LINK_POINT(connect_show_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chains for Connect line commands
 */
#define	ALTERNATE	NONE
#include "cfg_line_transport.h"
LINK_POINT(connect_line_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chain registration array for Connect
 */
static parser_extension_request connect_chain_init_table[] = {
    { PARSE_ADD_EXEC_CMD, &pname(connect_exec_commands) },
    { PARSE_ADD_SHOW_CMD, &pname(connect_show_commands) },
    { PARSE_ADD_LINE_TERM_CMD, &pname(connect_line_commands) },
    { PARSE_LIST_END, NULL }
};


/*
 * connect_parser_init - Initialize Connect parser support
 */
void connect_parser_init (void)
{
    parser_add_command_list(connect_chain_init_table, "connect");
    /*
     * We finish setting up the "connect" commands when IP telnet is
     * present in the box.  See ip/ip_init.c
     *
     * connect_parse_start = &pname(exec_connect_name);
     */
}
