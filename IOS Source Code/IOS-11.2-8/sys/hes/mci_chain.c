/* $Id: mci_chain.c,v 3.6.10.2 1996/08/12 16:01:55 widmer Exp $
 * $Source: /release/112/cvs/Xsys/hes/mci_chain.c,v $
 *------------------------------------------------------------------
 * MCI parse chains
 *
 * May 1993, Robert Widmer
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: mci_chain.c,v $
 * Revision 3.6.10.2  1996/08/12  16:01:55  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.6.10.1  1996/05/21  09:48:04  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.6  1996/02/22  14:33:18  widmer
 * CSCdi49281:  make all parser_extension_request static const
 * make parser_extension_request typedef const
 * make all parser_extension_request static
 *
 * Revision 3.5  1996/01/29  07:27:40  hampton
 * Remove unnecessary includes of parser definition files.  [CSCdi47717]
 *
 * Revision 3.4  1995/12/17  18:27:31  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.3  1995/11/17  09:19:15  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:29:05  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:41:26  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/07/02  10:52:00  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize os, hes, les, if, and ibm.
 *
 * Revision 2.1  1995/06/07  20:42:51  hampton
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
#include "interface_private.h"
#include "if_mci.h"


/*
 * Parse chains for MCI show commands
 */
#define	ALTERNATE	NONE
#include "exec_show_controllers_mci.h"
LINK_POINT(mci_showcontrol_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chain registration array for MCI
 */
static parser_extension_request mci_chain_init_table[] = {
    { PARSE_ADD_SHOW_CONTROLLER_CMD, &pname(mci_showcontrol_commands) },
    { PARSE_LIST_END, NULL }
};


/*
 * mci_parser_init - Initialize MCI parser support
 */
void mci_parser_init (void)
{
    static boolean mci_parser_initialized = FALSE;

    if (!mci_parser_initialized) {
	parser_add_command_list(mci_chain_init_table, "mci");
	mci_parser_initialized = TRUE;
    }
}




