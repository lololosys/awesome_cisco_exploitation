/* $Id: smds_chain.c,v 3.8.10.1 1996/05/21 10:09:55 thille Exp $
 * $Source: /release/112/cvs/Xsys/wan/smds_chain.c,v $
 *------------------------------------------------------------------
 * SMDS parse chains
 *
 * May 1993, Robert Widmer
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: smds_chain.c,v $
 * Revision 3.8.10.1  1996/05/21  10:09:55  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.8  1996/02/22  14:40:06  widmer
 * CSCdi49281:  make all parser_extension_request static const
 * make parser_extension_request typedef const
 * make all parser_extension_request static
 *
 * Revision 3.7  1996/02/01  06:12:36  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.6  1996/01/29  07:31:28  hampton
 * Remove unnecessary includes of parser definition files.  [CSCdi47717]
 *
 * Revision 3.5  1995/12/17  18:41:27  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.4  1995/11/27  16:24:04  widmer
 * CSCdi33598:  WAN commands (eg X.25) in parser even when Subsystem not
 * included
 * Create parser link for interface encapsulation types
 *
 * Revision 3.3  1995/11/17  18:04:44  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:50:44  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:51:10  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/07/02  01:46:16  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize xns, atalk, vines, decnet, xremote, x25, unix, wan, tn3270,
 * clns, cdp
 *
 * Revision 2.1  1995/06/07  23:19:50  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include "config.h"
#include "parser.h"
#include "../h/interface_private.h"
#include "../h/interface.h"
#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "parser_defs_smds.h"
#include "../wan/smds_actions.h"
#include "../wan/smds.h"


/*
 * Parse chains for SMDS show commands
 */
#define	ALTERNATE	NONE
#include "exec_show_smds.h"
LINK_POINT(smds_show_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chains for SMDS interface commands
 */
#define ALTERNATE       NONE
#include "cfg_int_smds.h"
LINK_POINT(smds_interface_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chain for SMDS interface encapsulation command
 */
LINK_EXIT(cfg_encap_smds_exit, no_alt);
ENCAPS	(cfg_encap_smds, cfg_encap_smds_exit, NONE,
	 OBJ(int,1), ET_SMDS,
	 "smds", "Switched Megabit Data Service (SMDS)", IDB_SYNC);
LINK_POINT(cfg_encap_smds_cmd, cfg_encap_smds);



/*
 * Parse chain registration array for SMDS commands
 */
static parser_extension_request smds_chain_init_table[] = {
    { PARSE_ADD_SHOW_CMD, &pname(smds_show_commands) },
    { PARSE_ADD_CFG_INT_CMD, &pname(smds_interface_commands) },
    { PARSE_ADD_INT_ENCAPS_CMD, &pname( cfg_encap_smds_cmd) },
    { PARSE_ADD_INT_ENCAPS_EXIT,
	  (dynamic_transition *) &pname(cfg_encap_smds_exit) },
    { PARSE_LIST_END, NULL }
};


/*
 * smds_parser_init - Initialize parse structures for parsing SMDS commands
 */
void smds_parser_init (void)
{
    parser_add_command_list(smds_chain_init_table, "smds");
    parser_add_address(ADDR_PARSE(STATION_SMDS64), smds_addr_short_help,
		       match_smds_addr);
}
