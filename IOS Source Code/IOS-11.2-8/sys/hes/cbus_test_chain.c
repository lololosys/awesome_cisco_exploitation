/* $Id: cbus_test_chain.c,v 3.5.20.1 1996/05/21 09:47:36 thille Exp $
 * $Source: /release/112/cvs/Xsys/hes/cbus_test_chain.c,v $
 *------------------------------------------------------------------
 * CBUS test commands
 *
 * October 1993, Robert Widmer
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: cbus_test_chain.c,v $
 * Revision 3.5.20.1  1996/05/21  09:47:36  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.5  1996/01/29  07:27:36  hampton
 * Remove unnecessary includes of parser definition files.  [CSCdi47717]
 *
 * Revision 3.4  1995/12/17  18:27:17  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.3  1995/11/17  09:15:03  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:25:51  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:37:42  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/07/02  10:51:48  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize os, hes, les, if, and ibm.
 *
 * Revision 2.1  1995/06/07  20:38:21  hampton
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

/*
 * Parse chains for TEST fci commands
 */
#define ALTERNATE       NONE
#include "exec_test_cbus.h"
LINK_POINT(test_fci_commands, ALTERNATE);
#undef  ALTERNATE

void cbus_parser_test_init (void)
{
    parser_add_commands(PARSE_ADD_TEST_CMD, &pname(test_fci_commands),
			"fci");
}
