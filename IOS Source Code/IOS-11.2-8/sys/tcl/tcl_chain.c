/* $Id: tcl_chain.c,v 3.4.20.1 1996/05/21 10:05:59 thille Exp $
 * $Source: /release/112/cvs/Xsys/tcl/tcl_chain.c,v $
 *------------------------------------------------------------------
 * tcl_chain.c --
 * April 1994, Satish Gannu
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: tcl_chain.c,v $
 * Revision 3.4.20.1  1996/05/21  10:05:59  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.4  1996/01/29  07:30:18  hampton
 * Remove unnecessary includes of parser definition files.  [CSCdi47717]
 *
 * Revision 3.3  1995/12/17  18:39:05  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.2  1995/11/17  18:47:07  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:34:43  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/07/04  01:58:27  tli
 * CSCdi36508: Rationalize parser file placement, part I
 * Rationalize cls dev dlsw dspu filesys irix lat les lex llc2 parser pgen
 * pt servers smf smrp snmp src-36-c1000 src-4k-c4000 src-68-c3000
 * src-68-c4000 src-c4000 src-rsp srt tcl ts ui
 *
 * Revision 2.1  1995/06/07  23:04:40  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include "config.h"
#include "ttysrv.h"
#include "parser.h"
#include "interface_private.h"
#include "packet.h"
#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "../tcl/tcl_init.h"

/*
 * tcl exec command
 */
#define	ALTERNATE	NONE
#include "exec_tclsh.h"
LINK_POINT(tclsh_exec_command, ALTERNATE);
#undef	ALTERNATE

#define	ALTERNATE	NONE
#include "exec_tclquit.h"
LINK_POINT(tclquit_exec_command, ALTERNATE);
#undef	ALTERNATE

void tcl_parser_init (void)
{
    parser_add_commands(PARSE_ADD_EXEC_CMD, &pname(tclsh_exec_command),
			"tclsh");
    parser_add_commands(PARSE_ADD_EXEC_CMD,
			&pname(tclquit_exec_command), "tclquit");
}
