/* $Id: if_c4000_chain.c,v 3.6.10.1 1996/05/21 09:56:09 thille Exp $
 * $Source: /release/112/cvs/Xsys/les/if_c4000_chain.c,v $
 *------------------------------------------------------------------
 * C4000 parse chains
 *
 * May 1993, Robert Widmer
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: if_c4000_chain.c,v $
 * Revision 3.6.10.1  1996/05/21  09:56:09  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.6  1996/02/22  14:35:57  widmer
 * CSCdi49281:  make all parser_extension_request static const
 * make parser_extension_request typedef const
 * make all parser_extension_request static
 *
 * Revision 3.5  1996/02/13  21:26:09  rbatz
 * CSCdi30258:  Fixed the following:
 *                  Duplicate half-duplex commands.
 *                  Could not do half-duplex stun.
 *                  Could not tune half-duplex timers for bstun.
 *
 * Revision 3.4  1996/02/01  06:06:48  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.3  1995/12/17  18:32:47  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.2  1995/11/17  17:39:06  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:31:35  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/08/09  00:21:07  kramling
 * CSCdi38439:  FEIP product is now supported in 11.0.  Update support to
 * 10.3.
 *
 * Revision 2.3  1995/07/02  10:52:41  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize os, hes, les, if, and ibm.
 *
 * Revision 2.2  1995/06/28  09:28:49  smackie
 * Repair widespread subsystem header braindamage. (CSCdi23568)
 *
 *   o Fix subsystem entry points to be foo(subsystype *) not foo(void)
 *   o Repair nonsensical and redundant subsystem sequences
 *   o Use NULL where no property is required
 *
 * Revision 2.1  1995/06/07  21:34:04  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include "interface_private.h"
#include "config.h"
#include "parser.h"
#include "subsys.h"
#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "parser_defs_xx.h"


/*
 * Parse chains for C4000 interface commands
 */
#define	ALTERNATE	NONE
#include "cfg_int_squelch.h"
LINK_POINT(c4000_interface_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chain registration array for C4000
 */
static parser_extension_request c4000_chain_init_table[] = {
    { PARSE_ADD_CFG_INT_AS_CMD, &pname(c4000_interface_commands) },
    { PARSE_LIST_END, NULL }
};


/*
 * if_c4000_ui_init - Initialize C4000 interface parser support
 */
void if_c4000_ui_init (subsystype *subsys)
{
    parser_add_command_list(c4000_chain_init_table, "c4000");
}


/*
 * if_c4000_ui subsystem header
 */
 
#define IF_C4000_UI_MAJVERSION    1
#define IF_C4000_UI_MINVERSION    0
#define IF_C4000_UI_EDITVERSION   1
 
SUBSYS_HEADER(if_c4000_ui, IF_C4000_UI_MAJVERSION, IF_C4000_UI_MINVERSION,
              IF_C4000_UI_EDITVERSION,
              if_c4000_ui_init, SUBSYS_CLASS_MANAGEMENT,
              NULL, NULL);
