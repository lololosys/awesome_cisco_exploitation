/* $Id: rlogin_chain.c,v 3.4.20.1 1996/05/21 10:07:15 thille Exp $
 * $Source: /release/112/cvs/Xsys/ts/rlogin_chain.c,v $
 *------------------------------------------------------------------
 * rlogin_chain.c -- Unix remote login service, parser support
 *
 * August 1994, Andy Heffernan
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: rlogin_chain.c,v $
 * Revision 3.4.20.1  1996/05/21  10:07:15  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.4  1996/01/29  07:30:41  hampton
 * Remove unnecessary includes of parser definition files.  [CSCdi47717]
 *
 * Revision 3.3  1995/12/17  18:39:54  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.2  1995/11/17  18:55:58  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:39:57  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/08/01  23:09:58  irfan
 * CSCdi37754:  rlogin should allow various sources of remote username
 * Allow a /user switch for rlogin command. Also let the current username
 * be passed as remote username, if rlogin trusted-remoteuser-source is
 * local. Besides, send the local username or "ciscoTS" if the current
 * username was authenticated via one of trusted methods.
 *
 * Revision 2.3  1995/07/04  01:59:05  tli
 * CSCdi36508: Rationalize parser file placement, part I
 * Rationalize cls dev dlsw dspu filesys irix lat les lex llc2 parser pgen
 * pt servers smf smrp snmp src-36-c1000 src-4k-c4000 src-68-c3000
 * src-68-c4000 src-c4000 src-rsp srt tcl ts ui
 *
 * Revision 2.2  1995/06/15  15:19:38  widmer
 * CSCdi35889:  parser transition structures should be static
 *
 * Revision 2.1  1995/06/07  23:10:04  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include "config.h"
#include "parser.h"
#include "logger.h"
#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "../ui/debug.h"
#include "rlogin.h"

#include "packet.h"
#include "../ip/ip.h"
#include "../parser/parser_defs_exec.h"


/*
 * Parse chains for RLOGIN command
 */
#define ALTERNATE NONE
#include "exec_rlogin.h"
LINK_POINT(exec_rlogin_command, ALTERNATE);
#undef	ALTERNATE

/*
 * Parse chains for rlogin "config" command
 */
#define	ALTERNATE	NONE
#include "cfg_rlogin.h"
LINK_POINT(cfg_rlogin_commands, ALTERNATE);
#undef	ALTERNATE





/*
 * Parse chain registration array for rlogin command
 */

static parser_extension_request rlogin_chain_init_table[] = {
    { PARSE_ADD_EXEC_CMD, &pname(exec_rlogin_command) },
    { PARSE_ADD_TELNET_PORT_EXIT,
	  (dynamic_transition *) &pname(exec_rlogin_port_exit) },
    { PARSE_ADD_RLOGIN_EOL_EXIT,
	  (dynamic_transition *) &pname(exec_rlogin_eol_exit) },
    { PARSE_ADD_CFG_TOP_CMD, &pname(cfg_rlogin_commands) },
    { PARSE_LIST_END, NULL }
};


/*
 * rlogin_chain_init - Initialize parse chains for rlogin command
 * 
 * This routine is called at system startup to register the parse chains
 * for the rlogin command.
 */

void rlogin_chain_init (void)
{
    parser_add_command_list(rlogin_chain_init_table, "rlogin");
}
