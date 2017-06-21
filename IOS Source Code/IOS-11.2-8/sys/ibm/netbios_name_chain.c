/* $Id: netbios_name_chain.c,v 3.6.10.3 1996/07/09 05:58:15 ppearce Exp $
 * $Source: /release/112/cvs/Xsys/ibm/netbios_name_chain.c,v $
 *------------------------------------------------------------------
 * NetBios name parse chains
 *
 * September 1993, Robert Widmer
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: netbios_name_chain.c,v $
 * Revision 3.6.10.3  1996/07/09  05:58:15  ppearce
 * CSCdi59527:  inlines waste code space
 * Branch: California_branch
 *   Just say "no" to inline abuse - SRB subblock inlines removed
 *
 * Revision 3.6.10.2  1996/05/21  09:48:53  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.6.10.1  1996/05/17  11:21:03  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.6.2.2  1996/04/26  15:19:53  ppearce
 * IBU modularity - SRB subblock (part 1)
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.6.2.1  1996/04/26  07:54:23  dstine
 * Branch: IbuMod_Calif_branch
 * - commit of IBU modularity work, phase 1.
 *
 * Revision 3.6  1996/02/22  14:33:41  widmer
 * CSCdi49281:  make all parser_extension_request static const
 * make parser_extension_request typedef const
 * make all parser_extension_request static
 *
 * Revision 3.5  1996/02/01  06:03:18  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.4  1995/12/17  18:28:07  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.3  1995/11/17  09:23:35  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:35:42  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:45:50  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/07/02  10:53:58  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize os, hes, les, if, and ibm.
 *
 * Revision 2.1  1995/06/07  20:47:47  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include "interface_generic.h"
#include "config.h"
#include "subsys.h"
#include "parser.h"
#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "../parser/parser_defs_exec.h"
#include "../parser/parser_defs_parser.h"
#include "../ibm/parser_defs_netbios.h"
#include "../ui/debug.h"
#include "../ibm/netbios_debug.h"
#include "ieee.h"
#include "../if/rif.h"



/*
 * Parse chains for NETBIOS clear commands
 */
#define	ALTERNATE	NONE
#include "exec_clear_netbios.h"
LINK_POINT(netbios_clear_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chains for NETBIOS debug commands
 */
#define	ALTERNATE	NONE
#include "exec_debug_netbios.h"
LINK_POINT(netbios_debug_commands, ALTERNATE);
#undef  ALTERNATE


/*
 * Parse chains for NETBIOS show commands
 */
#define	ALTERNATE	NONE
#include "exec_show_netbios.h"
LINK_POINT(netbios_show_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chains for NETBIOS config commands
 */
#define	ALTERNATE	NONE
#include "cfg_netbios_name.h"
LINK_POINT(netbios_name_config_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chains for NETBIOS interface commands
 */
#define	ALTERNATE	NONE
#include "cfg_int_netbios_name.h"
LINK_POINT(netbios_name_interface_commands, ALTERNATE);
#undef	ALTERNATE


/*
 *
 */
static parser_extension_request netbios_name_chain_init_table[] = {
    { PARSE_ADD_CLEAR_CMD, &pname(netbios_clear_commands) },
    { PARSE_ADD_DEBUG_CMD, &pname(netbios_debug_commands) },
    { PARSE_ADD_SHOW_CMD, &pname(netbios_show_commands) },
    { PARSE_ADD_CFG_NBIOS_CMD, &pname(netbios_name_config_commands) },
    { PARSE_ADD_CFG_INT_NBIOS_CMD, &pname(netbios_name_interface_commands) },
    { PARSE_LIST_END, NULL }
};


/*
 * netbios_name_parser_init
 */
void netbios_name_parser_init (subsystype *subsys)
{
    parser_add_command_list(netbios_name_chain_init_table, "netbios name");
}

/*
 * Netbios UI (NETBIOS_NAME_UI) subsystem header
 */

#define NETBIOS_NAME_UI_MAJVERSION 1
#define NETBIOS_NAME_UI_MINVERSION 0
#define NETBIOS_NAME_UI_EDITVERSION  1

SUBSYS_HEADER(netbios_name_ui, NETBIOS_NAME_UI_MAJVERSION, NETBIOS_NAME_UI_MINVERSION, NETBIOS_NAME_UI_EDITVERSION,
	      netbios_name_parser_init, SUBSYS_CLASS_MANAGEMENT,
	      "NULL",
	      "NULL");
