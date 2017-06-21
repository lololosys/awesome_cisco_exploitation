/* $Id: lane_chain.c,v 3.9.4.4 1996/06/17 23:34:36 hampton Exp $
 * $Source: /release/112/cvs/Xsys/lane/lane_chain.c,v $
 *------------------------------------------------------------------
 * lane_chain.c -- Parse chains for LAN Emulation
 *
 * 06-Sep-94,  Brad Benson
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: lane_chain.c,v $
 * Revision 3.9.4.4  1996/06/17  23:34:36  hampton
 * Implement the long sought parser "default" keyword.  [CSCdi60510]
 * Branch: California_branch
 *
 * Revision 3.9.4.3  1996/05/21  09:54:16  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.9.4.2  1996/05/09  14:32:20  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.9.4.1.2.1  1996/04/27  07:06:15  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.9.4.1  1996/04/25  23:14:17  ronnie
 * CSCdi55766:  Hey!  This ATM and Frame Relay is no different than in 11.1
 * Feature commit for ATM and Frame Relay into 11.2
 * Branch: California_branch
 *
 * Revision 3.9.18.3  1996/04/22  17:34:49  rbadri
 * Branch: LE_Cal_V112_0_2_branch
 * cfg_int_lane.h is prettied up. Observe indentation from now on!
 * Make TokenRing parse commands visible only if TRLANE is in.
 *
 * Revision 3.9.18.2  1996/04/08  14:51:35  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 3.9.18.1  1996/04/08  01:56:42  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.9  1996/03/09  22:56:13  widmer
 * CSCdi47890:  HTTP server doesnt work
 * Add HTTP server
 *
 * Revision 3.8  1996/02/22  14:35:37  widmer
 * CSCdi49281:  make all parser_extension_request static const
 * make parser_extension_request typedef const
 * make all parser_extension_request static
 *
 * Revision 3.7  1996/02/13  19:50:39  hampton
 * Remove unnecessary includes.  [CSCdi48844]
 *
 * Revision 3.6  1996/01/29  07:28:52  hampton
 * Remove unnecessary includes of parser definition files.  [CSCdi47717]
 *
 * Revision 3.5  1996/01/19  02:46:08  cakyol
 * CSCdi47155:  Removing a LANE database while editing it can crash router
 * - fixed.  a simple locking mechanism now takes care of it.
 *
 * Revision 3.4  1995/12/17  18:31:34  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.3  1995/12/01  15:56:33  widmer
 * CSCdi44724:  Remove nv_top argument to parser_add_mode()
 * Clean up "no" handling at beginning of modes
 * Fix appn modes
 * Removed unneded pdecl's
 *
 * Revision 3.2  1995/11/17  17:50:16  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:22:49  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/06/15  15:18:55  widmer
 * CSCdi35889:  parser transition structures should be static
 *
 * Revision 2.1  1995/06/07  21:19:08  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#include "master.h"
#include "../ui/common_strings.h"
#include "../../boot/cpu.h"
#include "config.h"
#include "../ui/debug.h"
#include "parser.h"
#include "../h/interface_private.h"
#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "../parser/parser_defs_parser.h"
#include "../if/atm.h"
#include "../if/atm_actions.h"
#include "../if/rif.h"
#include "parser_defs_lane.h"

#include "lane.h"
#include "lane_debug.h"

extern boolean tr_lane_present;

/*
 * Parse chains for LANE clear commands
 */
#define ALTERNATE	NONE
#include "exec_clear_lane.h"
LINK_POINT (lane_clear_commands, ALTERNATE);
#undef	ALTERNATE

/*
 * Parse chains for LANE debug commands
 */
#define ALTERNATE       NONE
#include "exec_debug_lane.h"
LINK_POINT (lane_debug_commands, ALTERNATE);
#undef ALTERNATE

/*
 * Parse chains for LANE show commands
 */
#define	ALTERNATE	NONE
#include "exec_show_lane.h"
LINK_POINT (lane_show_commands, ALTERNATE);
#undef ALTERNATE

/*
 * Parse chains for LANE config interface commands 
 */
#define ALTERNATE       NONE
#include "cfg_int_lane.h"
LINK_POINT (lane_interface_commands, ALTERNATE);
#undef ALTERNATE

/*
 * Parse chains for LECS config-table mode commands.
 */
#define ALTERNATE       NONE
#include "cfg_cfgtbl_lane.h"
LINK_POINT (lane_cfgtbl_commands, ALTERNATE);
#undef ALTERNATE

LINK_EXIT(cfg_cfgtbl_lane_exit,  no_alt);
#define ALTERNATE	cfg_cfgtbl_lane_exit
#include "cfg_cfgtblexit_lane.h"
LINK_TRANS (cfg_cfgtbl_lane_extend_here, ALTERNATE);
#undef  ALTERNATE

#define ALTERNATE	cfg_cfgtbl_lane_extend_here
#include "cfg_cfgtbl_subcommands.h"

NO_OR_DEFAULT(config_lane_cfgtbl_no, ALTERNATE, PRIV_MIN | PRIV_NONVGEN);

HELP	(top_config_lane_cfgtbl, config_lane_cfgtbl_no,
	 "Lane LECS configuration table manipulation commands:\n");


/*
 * Parse chain registration array for LANE.
 */
static parser_extension_request lane_chain_init_table [] =
{
    { PARSE_ADD_CLEAR_CMD, &pname (lane_clear_commands) },
    { PARSE_ADD_DEBUG_CMD, &pname (lane_debug_commands) },
    { PARSE_ADD_SHOW_CMD, &pname (lane_show_commands) },
    { PARSE_ADD_CFG_TOP_CMD, &pname (lane_cfgtbl_commands) },
    { PARSE_ADD_CFG_INT_CMD, &pname (lane_interface_commands) },
    { PARSE_ADD_GLOBAL_EXIT,
	  (dynamic_transition *) &pname(cfg_cfgtbl_lane_exit) },
    { PARSE_LIST_END, NULL }
};

/*
 * lane_parser_init - Initialize LANE parser support
 */
void lane_parser_init (void)
{
    parser_add_link_point(PARSE_ADD_CFG_CFGTBL_CMD, 
			  "config lane lecs cfgtbl commands",
			  &pname (cfg_cfgtbl_lane_extend_here));

    parser_add_command_list(lane_chain_init_table, "lane");

    parser_add_mode("lane", "lane-config-database",
		    "ATM Lan Emulation Lecs Configuration Table",
		    TRUE, TRUE,
		    "configure",
		    cfgtable_context_exit, cfgtable_context_enter,
		    &pname(top_config_lane_cfgtbl),
		    NULL);
    parser_add_address(ADDR_PARSE(STATION_ATMNSAP),
		       atm_nsap_short_help, match_atm_nsap);
    parser_add_address(ADDR_PARSE(STATION_MASKED_ATMNSAP),
		       atm_masked_nsap_short_help, match_masked_atm_nsap);
}
