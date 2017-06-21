/* $Id: bsc_chain_tst.c,v 3.6.8.3 1996/09/10 08:47:55 jbalestr Exp $
 * $Source: /release/112/cvs/Xsys/bstun/bsc_chain_tst.c,v $
 *------------------------------------------------------------------
 * Test version of bsc_chain.c with parse chains for test commands
 *
 * Oct 95. Saritha Roach
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: bsc_chain_tst.c,v $
 * Revision 3.6.8.3  1996/09/10  08:47:55  jbalestr
 * CSCdi62395:  bsc/bstun debug for specific groups and addresses
 * Branch: California_branch
 *
 * Revision 3.6.8.2  1996/05/21  09:43:20  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.6.8.1  1996/05/17  10:40:03  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.5.2.2  1996/04/11  08:19:36  jbalestr
 * CSCdi51807:  need a test command to transmit raw bisync frames
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.5.2.1  1996/03/17  17:35:49  ppearce
 * Sync IbuMod_Calif_branch to V111_1_3
 *
 * Revision 3.6  1996/02/29  20:34:43  jbalestr
 * CSCdi48520:  BSTUN route command parses DIRECT
 * stops invalid configuration
 *
 * Revision 3.5  1996/02/22  14:32:11  widmer
 * CSCdi49281:  make all parser_extension_request static const
 * make parser_extension_request typedef const
 * make all parser_extension_request static
 *
 * Revision 3.4  1996/02/01  06:00:07  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.3  1996/01/29  07:26:31  hampton
 * Remove unnecessary includes of parser definition files.  [CSCdi47717]
 *
 * Revision 3.2  1995/12/17  18:24:53  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.1  1995/11/09  11:03:21  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/10/10  07:55:31  sroach
 * CSCdi41879:  Add BSC test command support
 *
 * Revision 2.1  1995/10/10  05:43:44  sroach
 * *** empty log message ***
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
#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "../bstun/bstun.h"
#include "../bstun/bsc.h"
#include "../bstun/bsc_debug.h"
#include "../bstun/bsc_debug_flags.h"
#include "../bstun/parser_defs_bsc.h"
#include "../bstun/parser_defs_bstun.h"
#include "../bstun/parser_defs_bsend_tst.h"
#include "../parser/parser_defs_parser.h"
#include "subsys.h"


/*
 * Parse chains for BSC show commands
 */
#define	ALTERNATE	NONE
#include "exec_show_bsc.h"
LINK_POINT(bsc_show_commands, ALTERNATE);
#undef	ALTERNATE

/*
 * Parse chains for BSC debug commands
 */
#define	ALTERNATE	NONE
#include "exec_debug_bsc.h"
LINK_POINT(bsc_debug_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chains for BSC interface commands
 */
#define	ALTERNATE	NONE
#include "cfg_int_bstun2_tst.h"
#include "cfg_int_bsc.h"
#include "cfg_int_bstun.h"
#include "cfg_int_bsend_tst.h"
LINK_POINT(bscif_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chain registration array for BSC
 */
static parser_extension_request bsc_chain_init_table[] = {
    { PARSE_ADD_DEBUG_CMD, &pname(bsc_debug_commands) },
    { PARSE_ADD_SHOW_CMD, &pname(bsc_show_commands) },
    { PARSE_ADD_CFG_INT_AS_CMD, &pname(bscif_commands) },
    { PARSE_LIST_END, NULL }
};


/*
 * bsc_parser_init - Initialize BSC parser support
 */
void bsc_parser_init (subsystype *subsys)
{
    parser_add_command_list(bsc_chain_init_table, "bstun/bsc/bsend");
}

/*
 * BSC UI (BSC_UI) subsystem header
 */

#define BSC_UI_MAJVERSION 1
#define BSC_UI_MINVERSION 0
#define BSC_UI_EDITVERSION  1

SUBSYS_HEADER(bsc_ui, BSC_UI_MAJVERSION, BSC_UI_MINVERSION, BSC_UI_EDITVERSION,
	      bsc_parser_init, SUBSYS_CLASS_MANAGEMENT,
	      "req: bsc", NULL);
