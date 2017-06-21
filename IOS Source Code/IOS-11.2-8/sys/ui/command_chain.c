/* $Id: command_chain.c,v 3.5.10.2 1996/05/21 10:07:40 thille Exp $
 * $Source: /release/112/cvs/Xsys/ui/command_chain.c,v $
 *------------------------------------------------------------------
 * Command parse chains
 *
 * May 1993, Robert Widmer
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: command_chain.c,v $
 * Revision 3.5.10.2  1996/05/21  10:07:40  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.5.10.1  1996/03/18  22:28:51  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.14.3  1996/03/16  07:45:35  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.3.14.2  1996/03/14  01:35:56  gstovall
 * Branch: DeadKingOnAThrone_branch
 * The King is now ready for action.
 *
 * Revision 3.3.14.1  1996/02/20  21:25:44  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.5  1996/02/22  14:39:32  widmer
 * CSCdi49281:  make all parser_extension_request static const
 * make parser_extension_request typedef const
 * make all parser_extension_request static
 *
 * Revision 3.4  1996/01/30  19:16:03  foster
 * CSCdi47228:  HSA slave image needs config parsing disabled
 *
 * Revision 3.3  1995/12/17  18:40:12  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.2  1995/11/17  17:47:25  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:40:44  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/07/04  01:59:18  tli
 * CSCdi36508: Rationalize parser file placement, part I
 * Rationalize cls dev dlsw dspu filesys irix lat les lex llc2 parser pgen
 * pt servers smf smrp snmp src-36-c1000 src-4k-c4000 src-68-c3000
 * src-68-c4000 src-c4000 src-rsp srt tcl ts ui
 *
 * Revision 2.2  1995/06/15  15:19:41  widmer
 * CSCdi35889:  parser transition structures should be static
 *
 * Revision 2.1  1995/06/07  23:11:02  hampton
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
#include "../parser/parser_defs_exec.h"
#include "../parser/parser_defs_config.h"
#include "../parser/parser_defs_flash.h"
#include "login.h"
#include "../h/sys_registry.h"


/*
 * Parse chains for Command exec commands
 */
#define	ALTERNATE	NONE
#include "exec_write.h"
#include "exec_who.h"
#include "exec_trace.h"
#include "exec_systat.h"
#include "exec_ping.h"
#include "exec_enable.h"
#include "exec_disable.h"
#include "exec_configure.h"
LINK_POINT(command_exec_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chains for Command clear commands
 */
#define	ALTERNATE	NONE
#include "exec_clear_line.h"
LINK_POINT(command_clear_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chains for Command show commands
 */
#define	ALTERNATE	NONE
#include "exec_show_users.h"
LINK_POINT(command_show_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chains for Command line commands
 */
#define	ALTERNATE	NONE
#include "cfg_line_escape.h"
LINK_POINT(command_line_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chain registration array for Command
 */
static parser_extension_request command_chain_init_table[] = {
    { PARSE_ADD_EXEC_CMD, &pname(command_exec_commands) },
    { PARSE_ADD_CLEAR_CMD, &pname(command_clear_commands) },
    { PARSE_ADD_SHOW_CMD, &pname(command_show_commands) },
    { PARSE_ADD_LINE_TERM_CMD, &pname(command_line_commands) },
    { PARSE_LIST_END, NULL }
};

extern void (*enable_command_ptr)(parseinfo *);


/*
 * command_parser_init - Initialize Command parser support
 */
void command_parser_init (void)
{
    parser_add_link_point(PARSE_ADD_PING_CMD, "ping",
			  &pname(exec_ping_extend_here));
    parser_add_link_exit(PARSE_ADD_PING_EXIT, "ping",
			 &pname(exec_ping_return_here));
    parser_add_command_list(command_chain_init_table, "command");

    enable_command_ptr = &enable_command;
}
