/* $Id: exec_chain.c,v 3.6.10.5 1996/07/23 23:23:07 avaz Exp $
 * $Source: /release/112/cvs/Xsys/ui/exec_chain.c,v $
 *------------------------------------------------------------------
 * Exec parse chains
 *
 * May 1993, Robert Widmer
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: exec_chain.c,v $
 * Revision 3.6.10.5  1996/07/23  23:23:07  avaz
 * CSCdi62098:  add show diagbus and show cont cbus to show tech-support
 * Branch: California_branch
 *
 * Revision 3.6.10.4  1996/07/19  01:01:20  avaz
 * CSCdi62098:  add show diagbus and show cont cbus to show tech-support
 * Branch: California_branch
 *
 * Revision 3.6.10.3  1996/05/21  10:07:45  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.6.10.2  1996/04/15  21:25:00  bcole
 * CSCdi54649:  IP Host Modularity, IP Automatic Stub Routing feature
 * commit
 * Branch: California_branch
 *
 * Revision 3.6.10.1  1996/04/15  15:00:40  widmer
 * CSCdi47180:  No Single Command to Collect General Router Data
 * Branch: California_branch
 * Add "show tech-support" command
 * Change references to encryption types to uint
 * Change tt_soc to use password_struct
 *
 * Revision 3.6  1996/02/22  14:39:34  widmer
 * CSCdi49281:  make all parser_extension_request static const
 * make parser_extension_request typedef const
 * make all parser_extension_request static
 *
 * Revision 3.5  1996/01/29  07:30:48  hampton
 * Remove unnecessary includes of parser definition files.  [CSCdi47717]
 *
 * Revision 3.4  1995/12/17  18:40:17  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.3  1995/11/17  17:47:45  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:46:03  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:40:58  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/09/24  00:55:00  dkatz
 * CSCdi40898:  Synchronized reload needed
 *
 * Revision 2.3  1995/07/04  01:59:21  tli
 * CSCdi36508: Rationalize parser file placement, part I
 * Rationalize cls dev dlsw dspu filesys irix lat les lex llc2 parser pgen
 * pt servers smf smrp snmp src-36-c1000 src-4k-c4000 src-68-c3000
 * src-68-c4000 src-c4000 src-rsp srt tcl ts ui
 *
 * Revision 2.2  1995/06/15  15:19:43  widmer
 * CSCdi35889:  parser transition structures should be static
 *
 * Revision 2.1  1995/06/07  23:11:20  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include "sys_registry.h"
#include "config.h"
#include "parser.h"
#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "../parser/parser_defs_exec.h"
#include "../ui/exec.h"
#include "../parser/parser_defs_parser.h"
#include "../parser/parser_defs_config.h"
#include "../parser/parser_defs_flash.h"
#include "exec_parser.h"
#include "../os/techsupport.h"


/*
 * Parse chains for Exec exec commands
 */
#define	ALTERNATE	NONE
#include "exec_setup.h"
#include "exec_send.h"
#include "exec_reload.h"
#include "exec_login.h"
#include "exec_hangup.h"
#include "exec_copy.h"
LINK_POINT(exec_exec_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chains for Exec show commands
 */
#define	ALTERNATE	NONE
#include "exec_show_version.h"
#include "exec_show_reload.h"
#include "exec_show_hardware.h"
#include "exec_show_debugging.h"
#include "exec_show_controllers.h"
LINK_POINT(exec_show_cmds, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chains for Exec clear commands
 */
#define	ALTERNATE	NONE
#include "exec_clear_interface.h"
LINK_POINT(exec_clear_cmds, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chains for Exec help commands
 */
#define	ALTERNATE	NONE
#include "exec_help.h"
LINK_POINT(global_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chain registration array for Exec
 */
static parser_extension_request exec_chain_init_table[] = {
    { PARSE_ADD_EXEC_CMD, &pname(exec_exec_commands) },
    { PARSE_ADD_SHOW_CMD, &pname(exec_show_cmds) },
    { PARSE_ADD_CLEAR_CMD, &pname(exec_clear_cmds) },
    { PARSE_ADD_GLOBAL_CMD, &pname(global_commands) },
    { PARSE_LIST_END, NULL }
};

static void show_controllers_techsupport (parseinfo *csb)
{
    show_techsupport_command(csb, "show controllers");
}

/*
 * exec_parser_init - Initialize Exec parser support
 */
void exec_parser_init (void)
{
  static int exec_parser_initialized = FALSE;
  
  if (!exec_parser_initialized) {
    /* copy ... */
    parser_add_link_point(PARSE_ADD_COPY_CMD, "copy",
			  &pname(exec_copy_extend_here));
    parser_add_link_point(PARSE_ADD_COPYFILE_CMD, "copy",
			  &pname(exec_copyfile_extend_here));
    /* copy run-config ... */
    parser_add_link_point(PARSE_ADD_COPY_RUNNING_CMD,
			  "copy running-config *",
			  &pname(exec_copy_running_extend_here));
    parser_add_link_point(PARSE_ADD_COPY_RUNNING_FILE_CMD,
			  "copy running-config *",
			  &pname(exec_copy_running_file_extend_here));
    /* copy start-config ... */
    parser_add_link_point(PARSE_ADD_COPY_STARTUP_CMD,
			  "copy startup-config *",
			  &pname(exec_copy_startup_extend_here));
    parser_add_link_point(PARSE_ADD_COPY_STARTUP_FILE_CMD,
			  "copy startup-config *",
			  &pname(exec_copy_startup_file_extend_here));
    parser_add_link_point(PARSE_ADD_SHOW_CONTROLLER_CMD,
			  "show controllers",
			  &pname(show_controllers_extend_here));
    parser_add_command_list(exec_chain_init_table, "exec");
    exec_parser_initialized = TRUE;
    
    reload_command_ptr = &reload_command;
    reg_add_parse_cli_cmd(PARSER_CLI_DEFAULT, parse_cmd, 
			  "parse_cmd");
    reg_add_parse_cli_prompt(PARSER_CLI_DEFAULT, default_parse_cli_prompt, 
			     "default_parse_cli_prompt");
    reg_add_emergency_message(reload_emergency_message,
			      "reload_emergency_message");
    reg_add_show_techsupport(show_controllers_techsupport,
			     "show_controllers_techsupport");
  }
}
