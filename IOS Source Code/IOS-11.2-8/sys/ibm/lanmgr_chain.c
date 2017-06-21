/* $Id: lanmgr_chain.c,v 3.7.10.3 1996/05/21 09:48:50 thille Exp $
 * $Source: /release/112/cvs/Xsys/ibm/lanmgr_chain.c,v $
 *------------------------------------------------------------------
 * Lan Manager parse chains
 *
 * May 1993, Robert Widmer
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: lanmgr_chain.c,v $
 * Revision 3.7.10.3  1996/05/21  09:48:50  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.7.10.2  1996/05/17  11:20:33  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.7.2.2  1996/04/26  07:54:13  dstine
 * Branch: IbuMod_Calif_branch
 * - commit of IBU modularity work, phase 1.
 *
 * Revision 3.7.2.1  1996/04/03  14:33:55  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.7.10.1  1996/03/18  20:11:35  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.4.14.3  1996/03/16  06:53:00  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.4.14.2  1996/03/07  09:41:57  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.4.14.1  1996/02/20  00:54:26  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.7  1996/02/22  14:33:36  widmer
 * CSCdi49281:  make all parser_extension_request static const
 * make parser_extension_request typedef const
 * make all parser_extension_request static
 *
 * Revision 3.6  1996/02/01  06:03:04  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.5  1996/01/29  07:27:59  hampton
 * Remove unnecessary includes of parser definition files.  [CSCdi47717]
 *
 * Revision 3.4  1995/12/17  18:28:05  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.3  1995/12/05  02:44:20  pleung
 * CSCdi41461:  lnm commands out of order
 *
 * Revision 3.2  1995/11/17  09:22:57  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:45:13  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/11/08  21:01:56  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.2  1995/07/02  10:53:56  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize os, hes, les, if, and ibm.
 *
 * Revision 2.1  1995/06/07  20:47:16  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include <string.h>
#include "config.h"
#include "subsys.h"
#include "../ui/debug.h"
#include "parser.h"
#include "packet.h"
#include "logger.h"
#include "interface_generic.h"
#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "../ibm/lanmgr.h"
#include "../ibm/ibmnm_debug.h"
#include "parser_defs_lanmgr.h"
#include "../parser/parser_defs_parser.h"


/*
 * Parse chains for LANMGR debug commands
 */
#define	ALTERNATE	NONE
#include "exec_debug_lanmgr.h"
LINK_POINT(lanmgr_debug_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chains for LANMGR show commands
 */
#define	ALTERNATE	NONE
#include "exec_show_lanmgr.h"
LINK_POINT(lanmgr_show_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chains for LANMGR config commands
 */
#define	ALTERNATE	NONE
#include "cfg_lanmgr.h"
LINK_POINT(lanmgr_config_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chains for LANMGR interface commands
 */
#define ALTERNATE       NONE
#include "cfg_int_lanmgr.h"
LINK_POINT(lanmgr_interface_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chain registration array for LANMGR
 */
static parser_extension_request lanmgr_chain_init_table[] = {
    { PARSE_ADD_DEBUG_CMD, &pname(lanmgr_debug_commands) },
    { PARSE_ADD_SHOW_CMD, &pname(lanmgr_show_commands) },
    { PARSE_ADD_CFG_TOP_CMD, &pname(lanmgr_config_commands) },
    { PARSE_ADD_CFG_INT_AS_CMD, &pname(lanmgr_interface_commands) },
    { PARSE_LIST_END, NULL }
};


/*
 * lanmgr_parser_init - Initialize LANMGR parser support
 */
void lanmgr_parser_init (subsystype *subsys)
{
    parser_add_command_list(lanmgr_chain_init_table, "lanmgr");
}


/*
 * LAN Manager UI (LANMGR_UI) subsystem header
 */

#define LANMGR_UI_MAJVERSION 1
#define LANMGR_UI_MINVERSION 0
#define LANMGR_UI_EDITVERSION  1

SUBSYS_HEADER(lanmgr_ui, LANMGR_UI_MAJVERSION, LANMGR_UI_MINVERSION, LANMGR_UI_EDITVERSION,
	      lanmgr_parser_init, SUBSYS_CLASS_MANAGEMENT,
              "seq: srb_ui",
              "req: srb_ui, lanmgr");

