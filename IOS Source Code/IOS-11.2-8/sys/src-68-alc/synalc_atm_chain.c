/* $Id: synalc_atm_chain.c,v 3.1.66.6 1996/08/03 23:36:19 fox Exp $
 * $Source: /release/112/cvs/Xsys/src-68-alc/synalc_atm_chain.c,v $
 *------------------------------------------------------------------
 * synalc_atm_chain.c -- Parser chains for Synergy ATM line card
 *			 ATM interface support
 *
 * September 28, 1995, Sanjaya Kumar
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: synalc_atm_chain.c,v $
 * Revision 3.1.66.6  1996/08/03  23:36:19  fox
 * CSCdi63400:  high priority compatibility queue must die
 * Branch: California_branch
 * Remove high priority compatibility queue and activatehigh flag.
 * Convert high priority cfork'd processes to new-style processes with
 * watched queues, timers, and booleans.  Split Multilink PPP input
 * into separate high priority process.  Split load and busy calculations
 * into separate processes.  Speed up PPP input and scheduler processing.
 * Miscellaneous code changes.
 *
 * Revision 3.1.66.5  1996/07/02  23:44:40  sakumar
 * CSCdi60668:  PVC (RFC1483) support needs to be added to Cat5K ATM Module
 * Branch: California_branch
 *
 * Incorporated CLI change requests for PVC config and throttle config.
 *
 * Revision 3.1.66.4  1996/06/18  16:44:13  sakumar
 * CSCdi60668:  PVC (RFC1483) support needs to be added to Cat5K ATM Module
 * Branch: California_branch
 *
 * Revision 3.1.66.2  1996/05/21  10:03:45  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.1.66.1  1996/05/09  14:41:13  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.1.94.2  1996/04/29  22:21:48  sakumar
 * Changes to Cat5K ATM HW specific code for IOS to link after latest sync.
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.1.94.1  1996/04/27  07:25:27  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.1.84.1  1996/04/08  02:09:01  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.1.72.1  1996/03/22  09:43:46  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.1.56.2  1996/03/11  08:10:28  rlowe
 * More LANE merger fun and games.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 3.1.56.1  1996/03/03  21:24:58  rlowe
 * Apply SYNALC content to merge branch.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 3.1.54.2  1996/03/02  07:37:02  rlowe
 * Platform-specific hooks needed for sync to V111_1_0_3.
 * Branch: Synalc3_Cal_V111_1_0_3_branch
 *
 * Revision 3.1.54.1  1996/03/02  01:21:13  rlowe
 * Non-sync sync to mainline tag V111_1_0_3.
 * Branch: Synalc3_Cal_V111_1_0_3_branch
 *
 * Revision 3.1.38.1  1996/02/29  07:50:19  rlowe
 * First pass non-sync sync for Synergy ATM line card to V111_0_16.
 * Branch: Synalc3_Cal_V111_0_16_branch
 *
 * Revision 3.1  1995/11/09  13:24:55  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1.18.1  1996/02/03  07:08:36  rlowe
 * Non-sync sync of Synalc_catchup_branch to V111_0_5.
 * Branch: Synalc_catchup_pretal_branch
 *
 * Revision 2.1.2.1  1995/09/29  18:46:07  sakumar
 * Added support for vc-per-vp command for the Catalyst 5000 ATM module..
 * Branch: Synalc2_LE2_V110_1_0_3_branch
 *
 * Revision 2.1  1995/09/29  01:41:21  rlowe
 * Placeholders for Synergy ATM line card.
 * 
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
#include "../lane/parser_defs_lane.h"
#include "../lane/lane.h"

#include "../atm/atmsig_debug.h"
#include "ios_atmdriver.h"
#include "alc_lec.h"

static void ci_atm_pvc_avg_range (parseinfo *csb, uint *lower, uint *upper)
{
    if (csb && csb->interface && csb->interface->hwptr &&
        csb->interface->hwptr->atm_db) {
        *lower = 0;
        *upper = GETOBJ(int,5);
    }
}
 


/*
 * Parse chains for ATM debug commands
 */
LINK_TRANS(debug_atm_extend_here, no_alt);
#define ALTERNATE       debug_atm_extend_here
#include "../if/exec_debug_atm_common.h"
#include "../if/exec_debug_atm_global.h"
KEYWORD_DEBUG(debug_atm, ALTERNATE, NONE,
              OBJ(pdb,1), atm_debug_arr,
              "atm", "ATM interface packets", PRIV_OPR);
LINK_POINT(atm_debug_commands, debug_atm);
#undef  ALTERNATE
 
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

EOLS	(ci_show_atm_vlan_eol, cat5k_show_commands, SHOW_ATM_VLAN);
NUMBER  (ci_atm_vlan_num, ci_show_atm_vlan_eol, ci_show_atm_vlan_eol, 
	 OBJ(int,1), 1, 1023, "1-1023");

KEYWORD (ci_atm_show_vlan, ci_atm_vlan_num, ALTERNATE,
        "vlan", "VLAN to PVC bindings", PRIV_CONF);

#undef ALTERNATE
#define ALTERNATE ci_atm_show_vlan

KEYWORD (show_atm, ALTERNATE, NONE,
         "atm", "ATM information", PRIV_USER);
TEST_INT(show_atm_test, show_atm, NONE, NONE, natmnets); 

#undef ALTERNATE
#define ALTERNATE show_atm_test

LINK_POINT(atm_show_commands, ALTERNATE);
#undef  ALTERNATE

/*
 * Parse chains for platform-specific ATM interface configuration commands
 * Note: for atm pvc command to work properly the .h order must be
 * preserved. Specifically the vc-per-vp command must appear before
 * the atm pvc command
 */
LINK_TRANS(cfg_int_atm_extend_here, no_alt);
#define ALTERNATE       cfg_int_atm_extend_here
/*******************************************************************
 * atm bind pvc vlan
 * OBJ(int 1) = VLAN id
 * OBJ(int 2) = PVC
 */

EOLS    (ci_atm_bind_pvc_eol, cat5k_interface_commands, ATM_CMD_BIND_PVC_TO_VLAN);

NOPREFIX (ci_atm_bind_no_prefix, ci_atm_bind_pvc_eol, ci_atm_bind_pvc_eol);


NUMBER  (ci_atm_bind_vlan_num, ci_atm_bind_no_prefix, no_alt, OBJ(int,2),
	 1, 1023, "VLAN-ID(1-1023)");

NUMBER  (ci_atm_bind_pvc_num,  ci_atm_bind_vlan_num, no_alt, OBJ(int,1),
	 1, 1023, "PVC(1-1023)");

NVGENS  (ci_atm_bind_pvc_nvgen, ci_atm_bind_pvc_num,
	 cat5k_interface_commands, ATM_CMD_BIND_PVC_TO_VLAN);

KEYWORD (ci_atm_bind_pvc_vlan, ci_atm_bind_pvc_nvgen, NONE,
	 "vlan", "VLAN", PRIV_CONF|PRIV_SUBIF);

KEYWORD (ci_atm_bind_pvc, ci_atm_bind_pvc_vlan, NONE,
	 "pvc", "PVC", PRIV_CONF|PRIV_SUBIF);

KEYWORD (ci_atm_bind, ci_atm_bind_pvc, ALTERNATE,
	 "bind", "Bind PVC to VLAN", PRIV_CONF|PRIV_SUBIF);

#undef  ALTERNATE
#define ALTERNATE   ci_atm_bind


#include "../if/cfg_int_atm_global.h"
#include "../if/cfg_int_atm_common.h"
#include "../if/cfg_int_atm_router_switch.h"
#include "../if/cfg_int_atm_router.h"
#include "cfg_int_atm_synalc.h"
 
KEYWORD (ci_atm, ALTERNATE, NONE,
         "atm", "Modify ATM parameters", PRIV_CONF|PRIV_SUBIF);

ASSERT (ci_atm_test, ci_atm,NONE, is_atm(csb->interface->hwptr)); 

#undef ALTERNATE
#define ALTERNATE ci_atm_test
 
LINK_POINT(atm_interface_commands, ALTERNATE);
#undef  ALTERNATE


/*
 * Parse chain registration array for XX
 */
parser_extension_request synalc_atm_chain_init_table[] = {
    { PARSE_ADD_DEBUG_CMD, &pname(atm_debug_commands) },
    { PARSE_ADD_SHOW_CMD, &pname(atm_show_commands) },
    { PARSE_ADD_CFG_INT_CMD, &pname(atm_interface_commands) },
    { PARSE_ADD_CFG_STATIC_MAP, &pname(atm_maplist_commands) },
    { PARSE_ADD_CFG_MAPCLASS_CMD, &pname(atm_mapclass_commands) },
    { PARSE_LIST_END, NULL }
};



/*
 * a t m _ p a r s e r _ i n i t  ( )
 */
 
void
atm_parser_init (void)
{
    parser_add_command_list(synalc_atm_chain_init_table, "ATM");
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
}
