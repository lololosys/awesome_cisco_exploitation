/* $Id: atm_aip_chain.c,v 3.1.4.7 1996/08/03 23:24:01 fox Exp $
 * $Source: /release/112/cvs/Xsys/hes/atm_aip_chain.c,v $
 *------------------------------------------------------------------
 *  hes/atm_aip_chain.c: AIP parse chains.
 *
 *  March 1996, Rob Zagst
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: atm_aip_chain.c,v $
 * Revision 3.1.4.7  1996/08/03  23:24:01  fox
 * CSCdi63400:  high priority compatibility queue must die
 * Branch: California_branch
 * Remove high priority compatibility queue and activatehigh flag.
 * Convert high priority cfork'd processes to new-style processes with
 * watched queues, timers, and booleans.  Split Multilink PPP input
 * into separate high priority process.  Split load and busy calculations
 * into separate processes.  Speed up PPP input and scheduler processing.
 * Miscellaneous code changes.
 *
 * Revision 3.1.4.6  1996/05/31  15:30:45  rzagst
 * CSCdi54940:  ATM driver creates low-bit-rate SVCs
 * Branch: California_branch
 *
 * Revision 3.1.4.5  1996/05/21  09:47:30  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.1.4.4  1996/05/14  06:29:07  achopra
 * CSCdi47186:  Implement clear atm arp command.
 * Branch: California_branch
 *
 * Revision 3.1.4.3  1996/05/07  16:00:00  rzagst
 * CSCdi45984:  CBUS-3-OUTHUNG: ATM3/0: tx0 output hung (800E = queue full)
 * Branch: California_branch
 *
 * Revision 3.1.4.2  1996/05/07  00:17:47  rzagst
 * CSCdi56619:  arp-server showing up in show commands instead of show atm
 * commands
 * Branch: California_branch
 *
 * Revision 3.1.4.1  1996/04/25  23:11:53  ronnie
 * CSCdi55766:  Hey!  This ATM and Frame Relay is no different than in 11.1
 * Feature commit for ATM and Frame Relay into 11.2
 * Branch: California_branch
 *
 * Revision 3.1.2.1  1996/04/14  00:26:27  rzagst
 * ATM Modularity: Move atm arp to separate sub-system, remove inarp from
 * pvc command.
 * Branch: ATM_Cal_branch
 *
 * Revision 3.1  1996/03/30  03:50:39  rzagst
 * *** empty log message ***
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
#include "../parser/parser_defs_parser.h"
#include "../atm/parser_defs_atm.h"
#include "../lane/parser_defs_lane.h"
#include "../if/atm.h"
#include "../if/atm_debug.h"
#include "../if/atm_actions.h"
#include "../lane/lane.h"
#include "../atm/atm_arp.h"
#include "../hes/if_aip.h"
#include "../atm/atmsig_debug.h"

extern int natmnets;
/*
 * Average pvc range is chipset dependent for aip or nim. Note magic
 * number 64 is AIP_MAX_TIQ + 1 for fred chipset (See chip manual)
 * May want to put most of this in registry call for better modularity,
 * at cost of some more overhead in other files.
 */

static void ci_atm_pvc_avg_range (parseinfo *csb, uint *lower, uint *upper)
{
    if (csb && csb->interface && csb->interface->hwptr &&
        csb->interface->hwptr->atm_db) {
	/* round up to guarantee valid value due to int arithmatic
	 * truncation
	 */
	if ((GETOBJ(int,5) % 64) == 0) {
	    *lower = GETOBJ(int,5)/64;
	} else {
	    *lower = (GETOBJ(int,5)/64) + 1;
	}
	
	*upper = GETOBJ(int,5);
    }
}

/*
 * Parse chains for ATM debug commands
 */
LINK_TRANS(debug_atm_extend_here,no_alt);
#define	ALTERNATE debug_atm_extend_here
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
 * Parse chains for ATM interface commands
 * Note: for the command to work properly the order of the .h files
 * is significant. cfg_int_atm_cbus.h must be before cfg_int_atm_common.h
 * Specifically the aal command must be before the atm pvc command
 */
LINK_TRANS(cfg_int_atm_extend_here, no_alt);
#define ALTERNATE       cfg_int_atm_extend_here
/*
 * This #define USING_32_CELL_BURSTS is used to separate the
 * pvc burst command of platforms like the AIP that take a 32
 * cell burst parameter vs. the normal platforms that use a
 * parameter of number of cells.
 */
#define USING_32_CELL_BURSTS
#include "../if/cfg_int_atm_global.h"
#include "../if/cfg_int_atm_common.h"
#include "../if/cfg_int_atm_router_switch.h"
#include "../if/cfg_int_atm_router.h"
#include "../hes/cfg_int_atm_cbus.h"
#undef USING_32_CELL_BURSTS

KEYWORD	(ci_atm, ALTERNATE, NONE,
	 "atm", "Modify ATM parameters", PRIV_CONF|PRIV_SUBIF);

ASSERT (ci_atm_test, ci_atm,NONE, is_atm(csb->interface->hwptr));

#undef ALTERNATE
#define ALTERNATE ci_atm_test

LINK_POINT(atm_interface_commands, ALTERNATE);
#undef	ALTERNATE



/*
 * Parse chain registration array for ATM commands
 */
static parser_extension_request atm_aip_chain_init_table[] = {
    { PARSE_ADD_DEBUG_CMD, &pname(atm_debug_commands) },
    { PARSE_ADD_SHOW_CMD, &pname(atm_show_commands) },
    { PARSE_ADD_CLEAR_CMD, &pname(atm_clear_commands) },
    { PARSE_ADD_CFG_INT_CMD, &pname(atm_interface_commands) },
    { PARSE_ADD_CFG_STATIC_MAP, &pname(atm_maplist_commands) },
    { PARSE_ADD_CFG_MAPCLASS_CMD, &pname(atm_mapclass_commands) },
    { PARSE_LIST_END, NULL }
};


/*
 * atm_parser_init - Initialize parse structures for parsing ATM commands
 */
void atm_parser_init (void)
{
    parser_add_command_list(atm_aip_chain_init_table, "ATM");
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
    parser_add_link_point(PARSE_ADD_CLEAR_INT_ATM_CMD,
			  "clear atm", &pname(clear_int_atm_extend_here));
}
