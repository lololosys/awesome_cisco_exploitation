/* $Id: cflow_command.c,v 3.5.20.1 1996/05/21 10:06:36 thille Exp $
 * $Source: /release/112/cvs/Xsys/testtools/cflow_command.c,v $
 * ------------------------------------------------------------------
 * Implemetation of cflow commands
 * 
 * June-9 1994, Lakshmankumar Mukkavilli
 * 
 * Copyright (c) 1993-1997 by cisco Systems, Inc. All rights reserved.
 * 
 * 
 * ------------------------------------------------------------------
 * $Log: cflow_command.c,v $
 * Revision 3.5.20.1  1996/05/21  10:06:36  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.5  1996/01/31  18:24:06  lakshman
 * CSCdi47664:  Augment cflow with tracing capability
 *
 * Revision 3.4  1996/01/29  07:30:29  hampton
 * Remove unnecessary includes of parser definition files.  [CSCdi47717]
 *
 * Revision 3.3  1995/12/17  18:39:25  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.2  1995/11/17  18:50:27  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:36:59  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:07:20  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 * ------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include "config.h"
#include "interface_private.h"

#include "parser.h" 
#include "../parser/parser_actions.h"
#include "../parser/macros.h"

/*
 *Parse chain for 
 * cflow {save| clear | killsaved | listfiles [all] | status |
 *                  fastcopy [saved] |
 *                  copy [{ <ipaddr> | <hostname> } [<filename>]}] |
 *                  movesaved [{ <ipaddr> | <hostname> } [<filename>]}] |
 *                  [ <ipaddr> [<filename>] ] 
 */

#define ALTERNATE NONE
#include "../testtools/exec_cflow.h"
LINK_POINT(cflow_exec_command, ALTERNATE);
#undef ALTERNATE

/*
 *	cflow_parser_init:
 *	initialize the parser support for cflow
 */
void cflow_parser_init (void)
{
    parser_add_commands(PARSE_ADD_EXEC_CMD,
                            &pname(cflow_exec_command), "cflow");
}

