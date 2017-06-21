/* $Id: cbus_chain.c,v 3.7.10.7 1996/09/14 00:05:13 sseal Exp $
 * $Source: /release/112/cvs/Xsys/hes/cbus_chain.c,v $
 *------------------------------------------------------------------
 * CBUS parse chains
 *
 * May 1993, Robert Widmer
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: cbus_chain.c,v $
 * Revision 3.7.10.7  1996/09/14  00:05:13  sseal
 * CSCdi61196:  ipx route-cache options need clean up
 * Branch: California_branch
 *
 * Revision 3.7.10.6  1996/07/25  20:07:57  avaz
 * CSCdi62098:  add show diagbus and show cont cbus to show tech-support
 * Branch: California_branch
 *
 * Revision 3.7.10.5  1996/07/23  23:23:29  avaz
 * CSCdi62098:  add show diagbus and show cont cbus to show tech-support
 * Branch: California_branch
 *
 * Revision 3.7.10.4  1996/07/09  23:51:08  etrehus
 * CSCdi45163:  transmitter-delay help message is confusing
 * Branch: California_branch
 *
 * Revision 3.7.10.3  1996/05/21  09:47:35  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.7.10.2  1996/04/25  23:11:55  ronnie
 * CSCdi55766:  Hey!  This ATM and Frame Relay is no different than in 11.1
 * Feature commit for ATM and Frame Relay into 11.2
 * Branch: California_branch
 *
 * Revision 3.7.10.1.2.1  1996/03/30  03:04:37  rzagst
 * ATM Modularity: Clean up and modularize by platform the ATM parser
 * commands
 * Branch: ATM_Cal_branch
 *
 * Revision 3.7.10.1  1996/03/18  19:39:41  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.4.14.3  1996/03/16  06:49:10  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.4.14.2  1996/03/13  01:21:02  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.4.14.1  1996/02/20  00:50:12  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.7  1996/02/22  14:33:09  widmer
 * CSCdi49281:  make all parser_extension_request static const
 * make parser_extension_request typedef const
 * make all parser_extension_request static
 *
 * Revision 3.6  1996/02/01  06:02:17  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.5  1996/01/29  07:27:34  hampton
 * Remove unnecessary includes of parser definition files.  [CSCdi47717]
 *
 * Revision 3.4  1995/12/17  18:27:16  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.3  1995/11/17  09:14:43  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:25:41  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:37:27  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/07/14  08:05:18  schaefer
 * CSCdi36459: hideous subsystem dependancy
 * use subsys_init_class service to do post-driver init
 *
 * Revision 2.3  1995/07/03  06:33:27  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Automation missed on a dual-homed file.
 *
 * Revision 2.2  1995/07/02  10:51:47  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize os, hes, les, if, and ibm.
 *
 * Revision 2.1  1995/06/07  20:37:59  hampton
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
#include "../ui/debug.h"
#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "parser_defs_fci.h"
#include "../if/parser_defs_fddi.h"
#include "../atm/parser_defs_atm.h"
#include "../hes/if_mci.h"
#include "../hes/if_fci.h"
#include "../if/atm.h"
#include "../if/atm_debug.h"
#include "../if/atm_actions.h"
#include "../hes/if_aip.h"
#include "cbus_debug.h"
#include "subsys.h"
#include "../ip/ip_registry.h"
#include "../xns/novell.h"
#include "media_registry.h"
#include "sys_registry.h"
#include "../os/techsupport.h"


/*
 * Parse chains for CBUS debug commands 
 */
#define ALTERNATE	NONE
#include "exec_debug_cbus.h"
LINK_POINT(cbus_debug_commands, ALTERNATE);
#undef  ALTERNATE


/*
 * Parse chains for CBUS show commands
 */
#define	ALTERNATE	NONE
#include "exec_show_controllers_cbus.h"
LINK_POINT(cbus_showcontrol_commands, ALTERNATE);
#undef	ALTERNATE

/*
 * Parse chains for CBUS common interface commands
 */
#define ALTERNATE       NONE
#include "../if/cfg_int_transmitter-delay.h"
LINK_POINT(cbus_common_interface_commands, ALTERNATE);
#undef  ALTERNATE

/*
 * Parse chains for CBUS FDDI interface commands
 */
#define	ALTERNATE	NONE
#include "cfg_int_fddi_cbus.h"
LINK_POINT(cbus_fddi_interface_commands, ALTERNATE);
#undef	ALTERNATE

/*
 * add show controllers to show techsupport registry
 */
static void show_controllers_cbus_techsupport (parseinfo *csb)
{
  show_techsupport_command(csb, "show controllers cbus");
}


/*
 * Parse chains for CBUS ip route-cache interface command
 */
static boolean ip_routec_cbus_cmd (parseinfo *csb)
{
    idbtype *idb = csb->interface;
    hwidbtype *hwidb = idb->hwptr;

    if (csb->nvgen) {
	if (hwidb->ip_routecache & IP_CBUS_ROUTECACHE) {
	    nv_write(TRUE, "%s", csb->nv_command);
	}
	return(TRUE);
    }

    if (cbus_switching_supported && !is_smds(hwidb) && !is_ddr(hwidb)) {
	if (hwidb->status & IDB_CBUS) {
	    if (csb->sense) {
		hwidb->ip_routecache |=
		    (IP_FAST_ROUTECACHE | IP_CBUS_ROUTECACHE);
	    } else {
		hwidb->ip_routecache &= ~IP_CBUS_ROUTECACHE;
	    }
	} else {
	    printf("%s not cBus interface", idb->namestring);
	    return(FALSE);
	}
    } else {
	if (csb->sense) {
	    printf("Autonomous switching not supported");
	} else {
	    hwidb->ip_routecache &= ~IP_CBUS_ROUTECACHE;
	}
	return(FALSE);
    }
    return(TRUE);
}

LINK_EXIT (ci_ip2_routecache_cbus_exit, no_alt);
KEYWORD_ID(ci_ip2_routecache_cbus, ci_ip2_routecache_cbus_exit, NONE,
	    OBJ(int,1), IP_CBUS_ROUTECACHE,
	    "cbus", "Enable cbus fast-switching cache", PRIV_CONF);
LINK_POINT(ci_ip2_routecache_cbus_enter, ci_ip2_routecache_cbus);
/*
 * For IPX fastswitching 
 */

LINK_EXIT (ci_ipx_routecache_cbus_exit, no_alt);
KEYWORD_ID(ci_ipx_routecache_cbus, ci_ipx_routecache_cbus_exit, NONE,
	    OBJ(int,1), NOVELL_RCACHE_CBUS,
	    "cbus", "Enable cbus fast-switching cache", PRIV_CONF);
LINK_POINT(ci_ipx_routecache_cbus_enter, ci_ipx_routecache_cbus);

/*
 * Parse chain registration array for CBUS
 */
static parser_extension_request cbus_chain_init_table[] = {
    { PARSE_ADD_DEBUG_CMD, &pname(cbus_debug_commands) },
    { PARSE_ADD_SHOW_CONTROLLER_CMD, &pname(cbus_showcontrol_commands) },
    { PARSE_ADD_CFG_INT_AS_CMD, &pname(cbus_common_interface_commands) },
    { PARSE_ADD_CFG_INT_FDDI_CMD, &pname(cbus_fddi_interface_commands) },
    { PARSE_LIST_END, NULL }
};

/*
 * Parse chain registration array for CBUS routecache
 */
static parser_extension_request cbus_routecache_chain_init_table[] = {
    { PARSE_ADD_CFG_INT_IPROUTEC_CMD, &pname(ci_ip2_routecache_cbus_enter) },
    { PARSE_ADD_CFG_INT_IPXROUTEC_CMD, &pname(ci_ipx_routecache_cbus_enter) },
    { PARSE_ADD_CFG_INT_IPROUTEC_EXIT,
	  (dynamic_transition *) &pname(ci_ip2_routecache_cbus_exit) },
    { PARSE_ADD_CFG_INT_IPXROUTEC_EXIT,
	  (dynamic_transition *) &pname(ci_ipx_routecache_cbus_exit) },
    { PARSE_LIST_END, NULL }
};



/*
 * cbus_parser_init - Initialize CBUS parser support
 *                  - Add show controllers cbus to show tech-support
 *                    registry
 */
static void cbus_parser_init (subsystype *subsys)
{
    static boolean cbus_parser_initialized = FALSE;

    if (ncBus == 0)
	return;
    if (!cbus_parser_initialized) {
	parser_add_command_list(cbus_chain_init_table, "cbus");
	reg_add_show_techsupport(show_controllers_cbus_techsupport,
				 "show_controllers_cbus_techsupport");
	cbus_parser_initialized = TRUE;
    }
}

/*
 * cbus_routcache_parser_init - Initialize CBUS route cache parser
 *                              support on platforms that support cbus
 *                              switching
 */
void cbus_routecache_parser_init (void)
{
  parser_add_command_list(cbus_routecache_chain_init_table, "cbus");
  reg_add_ip_route_cache_cmd(IP_CBUS_ROUTECACHE, ip_routec_cbus_cmd,
			     "ip_routec_cbus_cmd");
}


/*
 * cbus parser subsystem header
 */
#define	CBUS_UI_MAJVERSION	1
#define	CBUS_UI_MINVERSION	0
#define	CBUS_UI_EDITVERSION	1

SUBSYS_HEADER(cbus_ui, CBUS_UI_MAJVERSION,
	      CBUS_UI_MINVERSION, CBUS_UI_EDITVERSION,
	      cbus_parser_init, SUBSYS_CLASS_MANAGEMENT,
	      NULL,
	      NULL);

