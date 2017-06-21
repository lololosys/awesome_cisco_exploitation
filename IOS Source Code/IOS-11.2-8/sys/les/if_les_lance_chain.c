/* $Id: if_les_lance_chain.c,v 3.3.44.1 1996/05/21 09:56:14 thille Exp $
 * $Source: /release/112/cvs/Xsys/les/if_les_lance_chain.c,v $
 *------------------------------------------------------------------
 * LANCE parse chains
 *
 * November 1993, Robert Widmer
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: if_les_lance_chain.c,v $
 * Revision 3.3.44.1  1996/05/21  09:56:14  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.3  1995/12/17  18:32:48  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.2  1995/11/17  17:40:42  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:32:54  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/07/02  10:52:43  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize os, hes, les, if, and ibm.
 *
 * Revision 2.1  1995/06/07  21:36:39  hampton
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
#include "logger.h"
#include "../les/if_les.h"
#include "../les/if_les_lance.h"
#include "../parser/parser_defs_parser.h"
#include "interface_private.h"


/*
 * LANCE show controllers commands
 */
#define	ALTERNATE	NONE
#include "exec_show_controllers_ether.h"
LINK_POINT(lance_showcontroller_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * lance_parser_init
 */
void lance_parser_init (void)
{
    parser_add_commands(PARSE_ADD_SHOW_CONTROLLER_CMD,
			&pname(lance_showcontroller_commands), "lance");
}
