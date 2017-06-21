/* $Id: clsitest_chain.c,v 3.5.10.1 1996/05/21 09:44:13 thille Exp $
 * $Source: /release/112/cvs/Xsys/cls/clsitest_chain.c,v $
 *------------------------------------------------------------------
 * Provides chaining of parser macros for CLSI Test proogram.
 *
 * 09/94, Kushal Patel
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 * clsitest_chain.c provides chaining of parser macros used by CLSI 
 * CLSI Test program.
 *------------------------------------------------------------------
 * $Log: clsitest_chain.c,v $
 * Revision 3.5.10.1  1996/05/21  09:44:13  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.5  1996/02/22  14:32:23  widmer
 * CSCdi49281:  make all parser_extension_request static const
 * make parser_extension_request typedef const
 * make all parser_extension_request static
 *
 * Revision 3.4  1996/01/29  07:26:57  hampton
 * Remove unnecessary includes of parser definition files.  [CSCdi47717]
 *
 * Revision 3.3  1995/12/17  18:25:23  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.2  1995/11/17  08:55:25  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:11:27  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/07/04  01:52:20  tli
 * CSCdi36508: Rationalize parser file placement, part I
 * Rationalize cls dev dlsw dspu filesys irix lat les lex llc2 parser pgen
 * pt servers smf smrp snmp src-36-c1000 src-4k-c4000 src-68-c3000
 * src-68-c4000 src-c4000 src-rsp srt tcl ts ui
 *
 * Revision 2.1  1995/06/07  20:20:06  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include "interface_private.h"
#include "config.h"
#include "../ui/debug.h"
#include "parser.h"
#include "logger.h"
#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "clsi_msg.h"
#include "clsi_queue.h"
#include "clsitest.h"


/*
 * Parse chains for clsitest command
 */
#define ALTERNATE NONE
#include "exec_clsitest.h"
LINK_POINT(clsitest_exec_commands, ALTERNATE);
#undef ALTERNATE

/*
 * Parse chain registration array for clsitest
 */
static parser_extension_request clsitest_chain_init_table[] = {
    { PARSE_ADD_EXEC_CMD, &pname(clsitest_exec_commands) },
    { PARSE_LIST_END, NULL }
};


/*
 * clsitest_parser_init - Initialize CLS parser support
 */
void clsitest_parser_init (void)
{
    parser_add_command_list(clsitest_chain_init_table, "clsitest");
}

