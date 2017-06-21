/* $Id: igrp2_chain.c,v 3.5.10.1 1996/05/21 09:53:17 thille Exp $
 * $Source: /release/112/cvs/Xsys/iprouting/igrp2_chain.c,v $
 *------------------------------------------------------------------
 * IGRP2 parse chains
 *
 * November 1993, Robert Widmer
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: igrp2_chain.c,v $
 * Revision 3.5.10.1  1996/05/21  09:53:17  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.5  1996/02/22  14:35:09  widmer
 * CSCdi49281:  make all parser_extension_request static const
 * make parser_extension_request typedef const
 * make all parser_extension_request static
 *
 * Revision 3.4  1996/02/01  06:05:27  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.3  1995/12/17  18:30:57  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.2  1995/11/17  17:34:00  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:07:53  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/08/04  03:07:47  dstine
 * CSCdi37681:  Need event logging for EIGRP
 *
 * Revision 2.2  1995/07/01  12:16:26  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize iprouting
 *
 * Revision 2.1  1995/06/07  21:06:45  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include "config.h"
#include "parser.h"
#include "interface_private.h"
#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "../ui/debug.h"
#include "route.h"
#include "ipigrp2.h"
#include "../iprouting/igrp2_debug.h"
#include "../iprouting/dual.h"
#include "parser_defs_igrp2.h"
#include "parser_defs_ipigrp2.h"


/*
 * Parse chains for IGRP2 debug commands
 */
#define	ALTERNATE	NONE
#include "exec_debug_igrp2.h"
LINK_POINT(igrp2_debug_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chain registration array for IGRP2
 */
static parser_extension_request igrp2_chain_init_table[] = {
    { PARSE_ADD_DEBUG_CMD, &pname(igrp2_debug_commands) },
    { PARSE_LIST_END, NULL }
};


/*
 * igrp2_parser_init - Initialize IGRP2 parser support
 */
void igrp2_parser_init (void)
{
    parser_add_command_list(igrp2_chain_init_table, "igrp2");
}
