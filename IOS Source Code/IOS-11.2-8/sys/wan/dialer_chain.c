/* $Id: dialer_chain.c,v 3.7.10.3 1996/05/30 23:46:37 hampton Exp $
 * $Source: /release/112/cvs/Xsys/wan/dialer_chain.c,v $
 *------------------------------------------------------------------
 * Dialer parse chains
 *
 * May 1993, Robert Widmer
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: dialer_chain.c,v $
 * Revision 3.7.10.3  1996/05/30  23:46:37  hampton
 * All commands that take access list numbers should accept either numbers
 * in the current monolithic number space, or hidden numbers in the future
 * per-protocol access list spaces.  [CSCdi59067]
 * Branch: California_branch
 *
 * Revision 3.7.10.2  1996/05/21  10:09:27  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.7.10.1  1996/04/27  06:36:07  syiu
 * Branch: California_branch
 * Commit Dial1_branch
 *
 * Revision 3.4.2.2  1996/04/17  00:00:14  syiu
 * Branch: Dial1_branch
 * Sync Dial1_branch to V111_1_3
 *
 * Revision 3.4.2.1  1996/02/23  21:31:21  lbustini
 * Branch: Dial1_branch
 * Move dialer fields out of the idb.
 *
 * Revision 3.7  1996/02/22  14:39:54  widmer
 * CSCdi49281:  make all parser_extension_request static const
 * make parser_extension_request typedef const
 * make all parser_extension_request static
 *
 * Revision 3.6  1996/02/01  06:11:57  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.5  1996/01/29  07:31:07  hampton
 * Remove unnecessary includes of parser definition files.  [CSCdi47717]
 *
 * Revision 3.4  1996/01/12  16:48:06  rbeach
 * CSCdi46101:  ISDN 4ESS Pri doesn't correctly handle outbound NSF.
 * The correct network specific facility values are now configurable.
 * We support outbound SDN, MEGACOMM and ACCUNET.
 *
 * Revision 3.3  1995/12/17  18:41:14  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.2  1995/11/17  18:01:37  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:48:30  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/09/30  05:06:18  dblair
 * CSCdi41333:  DDR Callback initial commit
 * Adding Callback feature for DDR interfaces.
 *
 * Revision 2.2  1995/07/02  01:46:01  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize xns, atalk, vines, decnet, xremote, x25, unix, wan, tn3270,
 * clns, cdp
 *
 * Revision 2.1  1995/06/07  23:17:20  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include "config.h"
#include "interface_private.h"
#include "parser.h"
#include "access.h"
#include "packet.h"
#include "../isdn/switch_def.h"
#include "../ui/debug.h"
#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "../parser/parser_defs_parser.h"
#include "parser_defs_dialer.h"
#include "../vines/parser_defs_vines.h"
#include "../wan/dialer.h"
#include "../wan/dialer_private.h"
#include "../wan/dialer_debug.h"
#include "../wan/dialer_registry.h"
#include "../xns/novell.h"
#include "../atalk/atalk.h"
#include "../decnet/decnet.h"


/*
 * Parse chains for dialer clear commands
 */
#define ALTERNATE       NONE
#include "exec_clear_dialer.h"
LINK_POINT(dialer_clear_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chains for dialer debug commands
 */
#define	ALTERNATE	NONE
#include "exec_debug_dialer.h"
LINK_POINT(dialer_debug_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chains for dialer show commands
 */
#define	ALTERNATE	NONE
#include "exec_show_dialer.h"
LINK_POINT(dialer_show_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chains for dialer config commands
 */
#define	ALTERNATE	NONE
#include "cfg_dialer.h"
LINK_POINT(dialer_config_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chains for dialer interface commands
 */
#define	ALTERNATE	NONE
#include "cfg_int_dialer.h"
LINK_POINT(dialer_interface_commands, ALTERNATE);
#undef	ALTERNATE

/*
 * Parse chains for dialer map class commands
 */
#define	ALTERNATE	NONE
#include "../wan/cfg_mapclass_dialer.h"
LINK_POINT(dialer_mapclass_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chain registration array for dialer commands
 */
static parser_extension_request dialer_chain_init_table[] = {
    { PARSE_ADD_CLEAR_CMD, &pname(dialer_clear_commands) },
    { PARSE_ADD_DEBUG_CMD, &pname(dialer_debug_commands) },
    { PARSE_ADD_SHOW_CMD, &pname(dialer_show_commands) },
    { PARSE_ADD_CFG_ROUTING_CMD, &pname(dialer_config_commands) },
    { PARSE_ADD_CFG_INT_CMD, &pname(dialer_interface_commands) },
    { PARSE_ADD_CFG_MAPCLASS_CMD, &pname(dialer_mapclass_commands) },
    { PARSE_LIST_END, NULL }
};


/*
 * dialer_parser_init - Initialize parse structures for
 *			parsing dialer commands
 */
void dialer_parser_init (void)
{
    parser_add_command_list(dialer_chain_init_table, "dialer");
}
