/* $Id: diag_chain.c,v 3.5.10.1 1996/05/21 09:45:16 thille Exp $
 * $Source: /release/112/cvs/Xsys/dev/diag_chain.c,v $
 *------------------------------------------------------------------
 * EXEC Test parser chains
 *
 * May 1993, Robert Widmer
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: diag_chain.c,v $
 * Revision 3.5.10.1  1996/05/21  09:45:16  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.5  1996/02/22  14:32:41  widmer
 * CSCdi49281:  make all parser_extension_request static const
 * make parser_extension_request typedef const
 * make all parser_extension_request static
 *
 * Revision 3.4  1996/01/29  07:27:13  hampton
 * Remove unnecessary includes of parser definition files.  [CSCdi47717]
 *
 * Revision 3.3  1995/12/17  18:25:53  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.2  1995/11/17  09:00:10  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:18:37  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/07/04  01:52:37  tli
 * CSCdi36508: Rationalize parser file placement, part I
 * Rationalize cls dev dlsw dspu filesys irix lat les lex llc2 parser pgen
 * pt servers smf smrp snmp src-36-c1000 src-4k-c4000 src-68-c3000
 * src-68-c4000 src-c4000 src-rsp srt tcl ts ui
 *
 * Revision 2.1  1995/06/07  20:24:07  hampton
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
#include "../parser/parser_defs_parser.h"


/*
 * Parse chains for TEST exec commands
 */
#define	ALTERNATE	NONE
#include "exec_test_crash.h"
#include "exec_test_memory.h"
#include "exec_test_interfaces.h"
LINK_POINT(test_exec_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chain registration array for TEST
 */
static parser_extension_request diag_exec_chain_init_table[] = {
    { PARSE_ADD_TEST_CMD, &pname(test_exec_commands) },
    { PARSE_LIST_END, NULL }
};


/*
 * diag_exec_parser_init - Initialize TEST Server parser support
 */
void diag_exec_parser_init (void)
{
    parser_add_command_list(diag_exec_chain_init_table, "exec diag");
}
