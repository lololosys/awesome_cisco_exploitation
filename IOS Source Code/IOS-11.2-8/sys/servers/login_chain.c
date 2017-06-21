/* $Id: login_chain.c,v 3.4.10.1 1996/05/21 10:01:30 thille Exp $
 * $Source: /release/112/cvs/Xsys/servers/login_chain.c,v $
 *------------------------------------------------------------------
 * Login parse chains
 *
 * May 1993, Robert Widmer
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: login_chain.c,v $
 * Revision 3.4.10.1  1996/05/21  10:01:30  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.4  1996/02/22  14:37:38  widmer
 * CSCdi49281:  make all parser_extension_request static const
 * make parser_extension_request typedef const
 * make all parser_extension_request static
 *
 * Revision 3.3  1995/12/17  18:36:43  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.2  1995/11/17  18:51:27  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:11:51  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/07/04  01:56:47  tli
 * CSCdi36508: Rationalize parser file placement, part I
 * Rationalize cls dev dlsw dspu filesys irix lat les lex llc2 parser pgen
 * pt servers smf smrp snmp src-36-c1000 src-4k-c4000 src-68-c3000
 * src-68-c4000 src-c4000 src-rsp srt tcl ts ui
 *
 * Revision 2.1  1995/06/07  22:44:11  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include "config.h"
#include "parser.h"
#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "login.h"
#include "aaa.h"
#include "access.h"
#include "../parser/parser_defs_config.h"


/*
 * Parse chains for Login config commands
 */
#define	ALTERNATE	NONE
#include "cfg_username.h"
#include "cfg_enable.h"
LINK_POINT(login_config_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chains for Login line commands
 */
#define	ALTERNATE	NONE
#include "cfg_line_password.h"
LINK_POINT(login_line_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chain registration array for Login
 */
static parser_extension_request login_chain_init_table[] = {
    { PARSE_ADD_CFG_TOP_CMD, &pname(login_config_commands) },
    { PARSE_ADD_CFG_LINE_CMD, &pname(login_line_commands) },
    { PARSE_LIST_END, NULL }
};


/*
 * login_parser_init - Initialize Login parser support
 */
void login_parser_init (void)
{
    parser_add_command_list(login_chain_init_table, "login");
}
