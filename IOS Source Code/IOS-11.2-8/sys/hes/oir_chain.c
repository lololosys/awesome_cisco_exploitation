/* $Id: oir_chain.c,v 3.3.44.1 1996/05/21 09:48:06 thille Exp $
 * $Source: /release/112/cvs/Xsys/hes/oir_chain.c,v $
 *------------------------------------------------------------------
 * OIR parse chains
 *
 * November 1994, William H. Palmer
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: oir_chain.c,v $
 * Revision 3.3.44.1  1996/05/21  09:48:06  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.3  1995/12/17  18:27:33  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.2  1995/11/17  09:19:39  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:41:50  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/07/02  10:52:01  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize os, hes, les, if, and ibm.
 *
 * Revision 2.1  1995/06/07  20:43:24  hampton
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
#include "oir_debug.h"
#include "oir_debug_flags.h"


/*
 * Parse chains for OIR debug commands
 */
#define	ALTERNATE	NONE
#include "exec_debug_oir.h"
LINK_POINT(oir_debug_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * oir_parser_init - initialize parser routines for OIR
 *
 */
void oir_parser_init (void)
{
    static int oir_parser_initialized = FALSE;

    if (!oir_parser_initialized) {
	parser_add_commands(PARSE_ADD_DEBUG_CMD, &pname(oir_debug_commands),
			    "oir");
	oir_parser_initialized = TRUE;
    }
}
