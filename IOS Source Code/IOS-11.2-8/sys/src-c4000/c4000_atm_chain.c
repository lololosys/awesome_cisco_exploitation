/* $Id: c4000_atm_chain.c,v 3.6.10.7 1996/08/03 23:36:27 fox Exp $
 * $Source: /release/112/cvs/Xsys/src-c4000/c4000_atm_chain.c,v $
 *------------------------------------------------------------------
 * Parser chains for C4000 ATM support
 *
 * March 1995, Walter Friedrich
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: c4000_atm_chain.c,v $
 * Revision 3.6.10.7  1996/08/03  23:36:27  fox
 * CSCdi63400:  high priority compatibility queue must die
 * Branch: California_branch
 * Remove high priority compatibility queue and activatehigh flag.
 * Convert high priority cfork'd processes to new-style processes with
 * watched queues, timers, and booleans.  Split Multilink PPP input
 * into separate high priority process.  Split load and busy calculations
 * into separate processes.  Speed up PPP input and scheduler processing.
 * Miscellaneous code changes.
 *
 * Revision 3.6.10.6  1996/05/31  15:30:57  rzagst
 * CSCdi54940:  ATM driver creates low-bit-rate SVCs
 * Branch: California_branch
 *
 * Revision 3.6.10.5  1996/05/21  10:04:13  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.6.10.4  1996/05/14  17:13:35  achopra
 * CSCdi57607:  wrong argument in parser_add_link_point
 * Branch: California_branch
 *
 * Revision 3.6.10.3  1996/05/14  06:29:12  achopra
 * CSCdi47186:  Implement clear atm arp command.
 * Branch: California_branch
 *
 * Revision 3.6.10.2  1996/05/07  00:17:38  rzagst
 * CSCdi56619:  arp-server showing up in show commands instead of show atm
 * commands
 * Branch: California_branch
 *
 * Revision 3.6.10.1  1996/04/25  23:22:17  ronnie
 * CSCdi55766:  Hey!  This ATM and Frame Relay is no different than in 11.1
 * Feature commit for ATM and Frame Relay into 11.2
 * Branch: California_branch
 *
 * Revision 3.6.12.2  1996/04/14  00:26:35  rzagst
 * ATM Modularity: Move atm arp to separate sub-system, remove inarp from
 * pvc command.
 * Branch: ATM_Cal_branch
 *
 * Revision 3.6.12.1  1996/03/30  02:19:36  rzagst
 * ATM Modularity: Clean up and modularize by platform ATM Commands.
 * Branch: ATM_Cal_branch
 *
 * Revision 3.6  1996/02/22  14:38:23  widmer
 * CSCdi49281:  make all parser_extension_request static const
 * make parser_extension_request typedef const
 * make all parser_extension_request static
 *
 * Revision 3.5  1996/02/01  06:09:11  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.4  1996/01/29  07:29:59  hampton
 * Remove unnecessary includes of parser definition files.  [CSCdi47717]
 *
 * Revision 3.3  1995/12/17  18:38:07  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.2  1995/11/17  18:44:42  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:26:09  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/07/04  01:57:31  tli
 * CSCdi36508: Rationalize parser file placement, part I
 * Rationalize cls dev dlsw dspu filesys irix lat les lex llc2 parser pgen
 * pt servers smf smrp snmp src-36-c1000 src-4k-c4000 src-68-c3000
 * src-68-c4000 src-c4000 src-rsp srt tcl ts ui
 *
 * Revision 2.2  1995/06/27  00:30:51  wfried
 * CSCdi36069:  Performance tuning and changes on controller screen for
 * new interfac
 * Included more interface status in the show controller output,
 * forced ATM receiving packets to start at 16 byte boundaries for
 * better DBUS utilization, and made some fastswitching optimizations.
 * Created new ATM interface loopback command options and renamed E3
 * framing modes to reflect current terminology
 *
 * Revision 2.1  1995/06/07  22:55:54  hampton
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
#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "../parser/parser_defs_parser.h"
#include "../atm/parser_defs_atm.h"
#include "../if/atm.h"
#include "../if/atm_actions.h"
#include "../if/atm_debug.h"
#include "../src-c4000/c4000_atmzr.h"
#include "../lane/parser_defs_lane.h"
#include "../lane/lane.h"
#include "../atm/atmsig_debug.h"

static void ci_atm_pvc_avg_range (parseinfo *csb, uint *lower, uint *upper)
{
    if (csb && csb->interface && csb->interface->hwptr &&
        csb->interface->hwptr->atm_db) {
	*lower = ATMZR_MIN_AVG_RATE;
	*upper = GETOBJ(int,5);
    }
}


/*
 * Parse chains for ATM debug commands
 */
LINK_TRANS(debug_atm_extend_here, no_alt);
#define	ALTERNATE	debug_atm_extend_here
#include "../if/exec_debug_atm_common.h"
#include "../if/exec_debug_atm_global.h"
KEYWORD_DEBUG(debug_atm, ALTERNATE, NONE,
	      OBJ(pdb,1), atm_debug_arr,
	      "atm", "ATM interface packets", PRIV_OPR);
LINK_POINT(atm_debug_commands, debug_atm);
#undef	ALTERNATE

/*
 * Parse chain for ATM map class commands
 */
#define ALTERNATE       NONE
#include "../if/cfg_mapclass_atm.h"
LINK_POINT(atm_mapclass_commands, ALTERNATE);
#undef ALTERNATE

/*
 * Parse chains for ATM show controllers command
 */
#define ALTERNATE       NONE
#include "exec_show_controllers_atm_c4000.h"
LINK_POINT(c4000_atm_showcontrol_commands, ALTERNATE);
#undef  ALTERNATE


/*
 * Parse chain for ATM static map commands
 */
#define ALTERNATE       NONE
#include "../if/cfg_staticmap_atm.h"
LINK_POINT(atm_maplist_commands, ALTERNATE);
#undef ALTERNATE

/*
 * Parse chains for ATM show commands
 */
LINK_TRANS(show_int_atm_extend_here,no_alt);
#define ALTERNATE    show_int_atm_extend_here
#include "../if/exec_show_atm_router_switch.h"
#include "../if/exec_show_atm_common.h"
KEYWORD (show_atm, ALTERNATE, NONE,
	 "atm", "ATM information", PRIV_USER);

TEST_INT(show_atm_test, show_atm, NONE, NONE, natmnets);

#undef ALTERNATE
#define ALTERNATE show_atm_test



LINK_POINT(atm_show_commands, ALTERNATE);
#undef	ALTERNATE

/*
 * Parse chains for ATM clear commands
 */
LINK_TRANS(clear_int_atm_extend_here,no_alt);

KEYWORD (clear_atm, clear_int_atm_extend_here, NONE,
         "atm", "ATM information", PRIV_OPR);

TEST_INT(clear_atm_test, clear_atm, NONE, NONE, natmnets);
 
#undef ALTERNATE
#define ALTERNATE clear_atm_test

LINK_POINT(atm_clear_commands, ALTERNATE);
#undef  ALTERNATE

/*
 * Parse chains for platform-specific ATM interface configuration commands
 * Note: for atm pvc command to work properly the .h order must be
 * preserved. Specifically the vc-per-vp command must appear before
 * the atm pvc command
 */
LINK_TRANS(cfg_int_atm_extend_here, no_alt);
#define ALTERNATE       cfg_int_atm_extend_here
#include "../if/cfg_int_atm_global.h"
#include "../if/cfg_int_atm_common.h"
#include "../if/cfg_int_atm_router_switch.h"
#include "../if/cfg_int_atm_router.h"
#include "cfg_int_atm_c4000.h"

KEYWORD	(ci_atm, ALTERNATE, NONE,
	 "atm", "Modify ATM parameters", PRIV_CONF|PRIV_SUBIF);

ASSERT (ci_atm_test, ci_atm,NONE, is_atm(csb->interface->hwptr));

#undef ALTERNATE
#define ALTERNATE ci_atm_test

LINK_POINT(atm_interface_commands, ALTERNATE);
#undef  ALTERNATE


/*
 * Parse chain registration array for XX
 */
static parser_extension_request c4000_atm_chain_init_table[] = {
    { PARSE_ADD_DEBUG_CMD, &pname(atm_debug_commands) },
    { PARSE_ADD_SHOW_CMD, &pname(atm_show_commands) },
    { PARSE_ADD_CLEAR_CMD, &pname(atm_clear_commands) },
    { PARSE_ADD_CFG_INT_CMD, &pname(atm_interface_commands) },
    { PARSE_ADD_CFG_STATIC_MAP, &pname(atm_maplist_commands) },
    { PARSE_ADD_CFG_MAPCLASS_CMD, &pname(atm_mapclass_commands) },
    { PARSE_ADD_SHOW_CONTROLLER_CMD, &pname(c4000_atm_showcontrol_commands) },
    { PARSE_LIST_END, NULL }
};


/*
 * a t m _ p a r s e r _ i n i t  ( )
 */

void
atm_parser_init (void)
{
    parser_add_command_list(c4000_atm_chain_init_table, "ATM");
    parser_add_link_point(PARSE_ADD_CFG_INT_ATM_CMD, "atm config int",
                          &pname(cfg_int_atm_extend_here));
    parser_add_link_point(PARSE_ADD_DEBUG_INT_ATM_CMD, "atm debug int",
                          &pname(debug_atm_extend_here));
    parser_add_link_point(PARSE_ADD_SHOW_INT_ATM_CMD, "show atm int",
			  &pname(show_int_atm_extend_here)); 
    parser_add_address(ADDR_PARSE(STATION_ATMNSAP), atm_nsap_short_help,
                       match_atm_nsap);
    parser_add_address(ADDR_PARSE(STATION_ATMESI), atm_esi_short_help,
                       match_atm_esi);
    parser_add_link_point(PARSE_ADD_CFG_INT_PVC_INARP_CMD,
			  "config inarp pvc",
			  &pname(ci_atm_pvc_inarp_extend_here));
    parser_add_link_exit(PARSE_ADD_CFG_INT_PVC_INARP_EXIT,
			 "config inarp pvc exit",
			 &pname(ci_atm_pvc_inarp_return_here));
    parser_add_link_point(PARSE_ADD_CLEAR_INT_ATM_CMD, "clear atm",
                            &pname(clear_int_atm_extend_here));
}


