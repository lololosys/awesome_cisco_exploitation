/* $Id: ts_chain.c,v 3.7.10.4 1996/08/12 16:08:39 widmer Exp $
 * $Source: /release/112/cvs/Xsys/ts/ts_chain.c,v $
 *------------------------------------------------------------------
 * Terminal Server parse chains
 *
 * May 1993, Robert Widmer
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: ts_chain.c,v $
 * Revision 3.7.10.4  1996/08/12  16:08:39  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.7.10.3  1996/07/04  00:29:07  irfan
 * CSCdi61001:  Autoselect throws away characters on startup
 * Branch: California_branch
 * Add "flush-at-activation" line subcommand to the ts system.
 *
 * Revision 3.7.10.2  1996/06/12  14:43:02  bjthomas
 * CSCdi38891:  need warning when line session-timeout nears
 * Branch: California_branch
 *
 * Revision 3.7.10.1  1996/05/21  10:07:18  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.7  1996/02/23  04:12:07  tkolar
 * CSCdi49648:  Modem autoconfig break modularity rules
 * Move confmodem_debug out of the commserver subsystem.
 *
 * Revision 3.6  1996/02/22  14:39:27  widmer
 * CSCdi49281:  make all parser_extension_request static const
 * make parser_extension_request typedef const
 * make all parser_extension_request static
 *
 * Revision 3.5  1996/02/01  06:11:14  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.4  1996/01/29  07:30:45  hampton
 * Remove unnecessary includes of parser definition files.  [CSCdi47717]
 *
 * Revision 3.3  1995/12/17  18:39:57  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.2  1995/11/17  18:56:22  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:40:10  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/11/08  21:33:31  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.2  1995/07/04  01:59:08  tli
 * CSCdi36508: Rationalize parser file placement, part I
 * Rationalize cls dev dlsw dspu filesys irix lat les lex llc2 parser pgen
 * pt servers smf smrp snmp src-36-c1000 src-4k-c4000 src-68-c3000
 * src-68-c4000 src-c4000 src-rsp srt tcl ts ui
 *
 * Revision 2.1  1995/06/07  23:10:22  hampton
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
#include "../ui/debug.h"
#include "../parser/parser_defs_parser.h"
#include "parser_defs_ts.h"
#include "../parser/parser_defs_config.h"
#include "../parser/parser_defs_autoselect.h"
#include "ttysrv.h"
#include "../ts/ts_debug.h"
#include "ttystatem.h"


/*
 * Parse chains for TS config commands
 */
#define	ALTERNATE	NONE
#include "cfg_state-mach.h"
LINK_POINT(ts_config_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chains for TS config routing commands
 */
#define	ALTERNATE	NONE
#include "cfg_busy_string.h"
#include "cfg_login-string.h"
#include "cfg_resume-string.h"
LINK_POINT(ts_cfg_rtng_commands, ALTERNATE);
#undef	ALTERNATE

/*
 * Parse chains for TS line terminal commands
 */
#define	ALTERNATE	NONE
#include "cfg_line_holdchar.h"
#include "cfg_line_dispatchchar.h"
#include "cfg_line_dispatchtimeout.h"
#include "cfg_line_autohangup.h"
#include "cfg_line_telnet.h"
LINK_POINT(ts_line_terminal_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chains for TS line commands
 */
#define	ALTERNATE	NONE
#include "cfg_line_insecure.h"
#include "cfg_line_private.h"
#include "cfg_line_session-limit.h"
#include "cfg_line_sessiontimeout_warning.h"
#include "cfg_line_autohost.h"
#include "cfg_line_absolute_timeout.h"
#include "cfg_line_autoselect.h"
#include "cfg_line_logout_warning.h"
#include "cfg_line_lockable.h"
#include "cfg_line_autobaud.h"
#include "cfg_line_disconnectchar.h"
#include "cfg_line_activatechar.h"
#include "cfg_line_disp-mach.h"
#include "cfg_line_flush_activate.h"
LINK_POINT(ts_line_commands, ALTERNATE);
#undef	ALTERNATE




/*
 * Parse chain registration array for TS
 */
static parser_extension_request ts_chain_init_table[] = {
    { PARSE_ADD_CFG_TOP_CMD, &pname(ts_config_commands) },
    { PARSE_ADD_CFG_ROUTING_CMD, &pname(ts_cfg_rtng_commands) },
    { PARSE_ADD_LINE_TERM_CMD, &pname(ts_line_terminal_commands) },
    { PARSE_ADD_CFG_LINE_CMD, &pname(ts_line_commands) },
    { PARSE_LIST_END, NULL }
};


/*
 * ts_parser_init - Initialize TS parser support
 */
void ts_parser_init (void)
{
    parser_add_command_list(ts_chain_init_table, "ts");
}
