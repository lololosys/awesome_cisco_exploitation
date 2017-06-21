/* $Id: bstun_chain.c,v 3.7.10.3 1996/09/10 08:48:36 jbalestr Exp $
 * $Source: /release/112/cvs/Xsys/bstun/bstun_chain.c,v $
 *------------------------------------------------------------------
 * bstun_chain.c - BSTUN parse chains
 *
 * August 1994, James Balestriere
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * Prepared by Metaplex.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: bstun_chain.c,v $
 * Revision 3.7.10.3  1996/09/10  08:48:36  jbalestr
 * CSCdi62395:  bsc/bstun debug for specific groups and addresses
 * Branch: California_branch
 *
 * Revision 3.7.10.2  1996/05/21  09:43:21  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.7.10.1  1996/05/17  10:41:01  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.7.2.2  1996/04/26  07:32:35  dstine
 * Branch: IbuMod_Calif_branch
 * - commit of IBU modularity work, phase 1.
 *
 * Revision 3.7.2.1  1996/04/11  08:19:47  jbalestr
 * CSCdi51807:  need a test command to transmit raw bisync frames
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.7  1996/02/22  14:32:12  widmer
 * CSCdi49281:  make all parser_extension_request static const
 * make parser_extension_request typedef const
 * make all parser_extension_request static
 *
 * Revision 3.6  1996/02/01  06:00:16  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.5  1996/01/29  07:26:34  hampton
 * Remove unnecessary includes of parser definition files.  [CSCdi47717]
 *
 * Revision 3.4  1995/12/17  18:24:54  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.3  1995/11/27  16:23:28  widmer
 * CSCdi33598:  WAN commands (eg X.25) in parser even when Subsystem not
 * included
 * Create parser link for interface encapsulation types
 *
 * Revision 3.2  1995/11/17  08:49:00  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:04:19  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/07/04  09:35:28  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize arap, atm, bstun
 *
 * Revision 2.3  1995/06/28  09:22:45  smackie
 * Repair widespread subsystem header braindamage. (CSCdi23568)
 *
 *   o Fix subsystem entry points to be foo(subsystype *) not foo(void)
 *   o Repair nonsensical and redundant subsystem sequences
 *   o Use NULL where no property is required
 *
 * Revision 2.2  1995/06/16  06:24:49  arothwel
 * CSCdi35957:  BSTUN/BSC cleanup.
 *
 * Revision 2.1  1995/06/07  20:14:23  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#include "master.h"
#include "../ui/common_strings.h"
#include "address.h"
#include "linktype.h"
#include "config.h"
#include "../ui/debug.h"
#include "parser.h"
#include "packet.h"
#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "bstun_actions.h"
#include "bstun.h"
#include "parser_defs_bstun.h"
#include "../parser/parser_defs_priority.h"
#include "bstun_debug.h"
#include "subsys.h"
#include "interface_generic.h"


/*
 * Parse chains for BSTUN debug commands
 */
#define	ALTERNATE	NONE
#include "exec_debug_bstun.h"
LINK_POINT(bstun_debug_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chains for BSTUN show commands
 */
#define	ALTERNATE	NONE
#include "exec_show_bstun.h"
LINK_POINT(bstun_show_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chains for BSTUN config commands
 */
#define ALTERNATE       NONE
#include "cfg_bstun.h"
LINK_POINT(bstun_config_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chains for BSTUN priority queueing commands
 */
#define ALTERNATE       NONE
#include "cfg_priority_bstun.h"
LINK_POINT(bstun_priority_commands, ALTERNATE);
#undef  ALTERNATE


/*
 * Parse chain for BSTUN interface encapsulation command
 */
LINK_EXIT(cfg_encaps_bstun_exit, no_alt);
ENCAPS	(cfg_encaps_bstun, cfg_encaps_bstun_exit, NONE,
	 OBJ(int,1), ET_BSTUN,
	 "bstun", "Block Serial tunneling (BSTUN)", IDB_SYNC);
LINK_POINT(cfg_encaps_bstun_cmd, cfg_encaps_bstun);


/*
 * Parse chain registration array for BSTUN
 */
static parser_extension_request bstun_chain_init_table[] = {
    { PARSE_ADD_DEBUG_CMD, &pname(bstun_debug_commands) },
    { PARSE_ADD_SHOW_CMD, &pname(bstun_show_commands) },
    { PARSE_ADD_CFG_TOP_CMD, &pname(bstun_config_commands) },
    { PARSE_ADD_PRIORITY_OPTIONS, &pname(bstun_priority_commands) },
    { PARSE_ADD_INT_ENCAPS_CMD, &pname(cfg_encaps_bstun_cmd) },
    { PARSE_ADD_INT_ENCAPS_EXIT,
	  (dynamic_transition *) &pname(cfg_encaps_bstun_exit) },
    { PARSE_LIST_END, NULL }
};


/*
 * bstun_parser_init - Initialize BSTUN parser support
 */
void bstun_parser_init (subsystype *subsys)
{
    parser_add_command_list(bstun_chain_init_table, "bstun");
    parser_add_protoaddr(LINK_BSTUN,		PMATCH_BSTUN,
			 "bstun",		"Block Serial Tunnel",
			 0, ADDR_ILLEGAL);
    parser_add_address(ADDR_BSTUN, bstun_addr_short_help, match_bstun_addr);
}

/*
 * BSTUN UI (BSTUN_UI) subsystem header
 */

#define BSTUN_UI_MAJVERSION 1
#define BSTUN_UI_MINVERSION 0
#define BSTUN_UI_EDITVERSION 1

SUBSYS_HEADER(bstun_ui, BSTUN_UI_MAJVERSION, BSTUN_UI_MINVERSION,
	      BSTUN_UI_EDITVERSION,
	      bstun_parser_init, SUBSYS_CLASS_MANAGEMENT,
	      "req: bstun", NULL);
