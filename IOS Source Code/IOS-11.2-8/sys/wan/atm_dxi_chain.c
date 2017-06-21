/* $Id: atm_dxi_chain.c,v 3.7.10.1 1996/05/21 10:09:24 thille Exp $
 * $Source: /release/112/cvs/Xsys/wan/atm_dxi_chain.c,v $
 *------------------------------------------------------------------
 * Support for ATM-DXI encapsulation and RFC1483 support
 *
 * October 1994, Shankar Natarajan
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: atm_dxi_chain.c,v $
 * Revision 3.7.10.1  1996/05/21  10:09:24  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.7  1996/02/22  14:39:52  widmer
 * CSCdi49281:  make all parser_extension_request static const
 * make parser_extension_request typedef const
 * make all parser_extension_request static
 *
 * Revision 3.6  1996/02/01  06:11:49  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.5  1996/01/29  07:31:05  hampton
 * Remove unnecessary includes of parser definition files.  [CSCdi47717]
 *
 * Revision 3.4  1995/12/17  18:41:13  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.3  1995/11/27  16:23:58  widmer
 * CSCdi33598:  WAN commands (eg X.25) in parser even when Subsystem not
 * included
 * Create parser link for interface encapsulation types
 *
 * Revision 3.2  1995/11/17  18:00:33  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:47:38  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/07/02  01:45:44  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize xns, atalk, vines, decnet, xremote, x25, unix, wan, tn3270,
 * clns, cdp
 *
 * Revision 2.1  1995/06/07  23:16:48  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
#include "master.h"
#include "../ui/common_strings.h"
#include "config.h"
#include "../ui/debug.h"
#include "parser.h"
#include "../h/interface_private.h"
#include "../h/interface.h"
#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "packet.h"
#include "../wan/atm_dxi.h"
#include "../wan/atm_dxi_debug.h"
#include "../parser/parser_defs_parser.h"
#include "parser_defs_atm_dxi.h"
#include "../h/access.h"

/*
 * Parse chains for atm-dxi debug commands
 */
#define	ALTERNATE	NONE
#include "exec_debug_atm_dxi.h"
LINK_POINT(atm_dxi_debug_commands, ALTERNATE);
#undef	ALTERNATE

/*
 * Parse chains for atm-dxi show commands
 */
#define	ALTERNATE	NONE
#include "exec_show_atm_dxi.h"
LINK_POINT(atm_dxi_show_commands, ALTERNATE);
#undef	ALTERNATE

/*
 * Parse chains for atm-dxi interface commands
 */
#define	ALTERNATE	NONE
#include "cfg_int_atm_dxi.h"
LINK_POINT(atm_dxi_interface_commands, ALTERNATE);
#undef	ALTERNATE

/*
 * Parse chains for atm-dxi interface encapsulation command
 */
LINK_EXIT(cfg_encap_atmdxi_exit, no_alt);
ENCAPS	(cfg_encap_atmdxi, cfg_encap_atmdxi_exit, NONE,
	 OBJ(int,1), ET_ATM_DXI,
	 "atm-dxi", "ATM-DXI encapsulation", IDB_SYNC);
LINK_POINT(cfg_encap_atmdxi_cmd, cfg_encap_atmdxi);

/*
 * Parse chain registration array for atm-dxi commands
 */
static parser_extension_request atm_dxi_chain_init_table[] = {
    { PARSE_ADD_DEBUG_CMD, &pname(atm_dxi_debug_commands) },
    { PARSE_ADD_SHOW_CMD, &pname(atm_dxi_show_commands) },
    { PARSE_ADD_CFG_INT_AS_CMD, &pname(atm_dxi_interface_commands) },
    { PARSE_ADD_INT_ENCAPS_CMD, &pname(cfg_encap_atmdxi_cmd) },
    { PARSE_ADD_INT_ENCAPS_EXIT,
	  (dynamic_transition *) &pname(cfg_encap_atmdxi_exit) },
    { PARSE_LIST_END, NULL }
};

/*
 * atm_dxi_parser_init - Initialize parse structures for
 *                           parsing atm-dxi commands
 */
void atm_dxi_parser_init (void)
{
    parser_add_command_list(atm_dxi_chain_init_table, "atm_dxi");
}
