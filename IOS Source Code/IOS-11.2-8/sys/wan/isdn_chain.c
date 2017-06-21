/* $Id: isdn_chain.c,v 3.8.10.1 1996/05/21 10:09:37 thille Exp $
 * $Source: /release/112/cvs/Xsys/wan/isdn_chain.c,v $
 *------------------------------------------------------------------
 * ISDN Parse chains
 *
 * May 1993, Robert Widmer
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: isdn_chain.c,v $
 * Revision 3.8.10.1  1996/05/21  10:09:37  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.8  1996/02/22  14:39:59  widmer
 * CSCdi49281:  make all parser_extension_request static const
 * make parser_extension_request typedef const
 * make all parser_extension_request static
 *
 * Revision 3.7  1996/02/01  06:12:18  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.6  1996/01/29  07:31:21  hampton
 * Remove unnecessary includes of parser definition files.  [CSCdi47717]
 *
 * Revision 3.5  1996/01/25  11:23:32  smackie
 * Registry code gets some much-needed hot-oil massaging, free liposuction
 * and a makeover.  (CSCdi47101)
 *
 *    o Compress registry structures. Saves over 120K for all platforms
 *    o Add registry subsystems and remove feature registry initializion
 *      from registry.c to fix woeful initialization scaling problems
 *    o Remove unused and seemingly unloved registry debugging code
 *    o Add registry memory statistics to shame people into action
 *
 * Revision 3.4  1995/12/17  18:41:16  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.3  1995/11/18  00:22:52  jjohnson
 * CSCdi43548:  need mechanism to enable multiple traps for a subsystem
 *
 * Revision 3.2  1995/11/17  18:03:03  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:49:45  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/11/08  21:35:47  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.2  1995/07/02  01:46:13  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize xns, atalk, vines, decnet, xremote, x25, unix, wan, tn3270,
 * clns, cdp
 *
 * Revision 2.1  1995/06/07  23:18:27  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include "interface_private.h"
#include "config.h"
#include "../ui/debug.h"
#include "parser.h"
#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "parser_defs_isdn.h"
#include "../parser/parser_defs_parser.h"
#include "../parser/parser_defs_snmp.h"
#include "../isdn/switch_def.h"
#include "../les/isdn_bri.h"
#include "../wan/isdn_debug.h"
#include "registry.h"
#include "../snmp/snmp_api.h"
#include "../snmp/snmp_registry.regh"


/*
 * Parse chains for ISDN show commands
 */
#define ALTERNATE   NONE
#include "exec_show_isdn.h"
LINK_POINT(isdn_show_commands, ALTERNATE);
#undef  ALTERNATE


/*
 * Parse chains for ISDN debug commands
 */
#define	ALTERNATE	NONE
#include "exec_debug_isdn.h"
LINK_POINT(isdn_debug_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chains for ISDN config commands
 */
#define	ALTERNATE	NONE
#include "cfg_isdn.h"
LINK_POINT(isdn_config_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chains for ISDN interface commands
 */
#define	ALTERNATE	NONE
#include "cfg_int_isdn.h"
LINK_POINT(isdn_interface_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chain for ISDN snmp-server host command
 */
LINK_EXIT(cfg_snmp_host_isdn_exit, no_alt);
KEYWORD_OR(cfg_snmp_host_isdn, cfg_snmp_host_isdn_exit, NONE,
	   OBJ(int,1), (1<<TRAP_ISDN),
	   "isdn", "Allow SNMP ISDN traps", PRIV_CONF);
LINK_POINT(cfg_snmp_host_isdn_entry, cfg_snmp_host_isdn);

/******************************************************************
 * Parse for isdn snmp-server enable command
 *
 * [no] snmp-server enable {trap | inform} [snmp | isdn | ...]
 *      [trap-number | trap-name_string | ...]
 ******************************************************************/

LINK_EXIT(cfg_snmp_enable_isdn_exit, no_alt);

KEYWORD_OR(conf_snmp_enable_isdn, cfg_snmp_enable_isdn_exit, NONE,
           OBJ(int,1), (1<<TRAP_ISDN),
           "isdn", "Enable SNMP isdn traps", PRIV_CONF);
LINK_POINT(cfg_snmp_enable_isdn_entry, conf_snmp_enable_isdn);

static void isdn_snmp_host_nvgen (ulong flags)
{
    nv_add(flags & (1 << TRAP_ISDN), " isdn");
}


/*
 * Parse chain registration array for ISDN commands
 */
static parser_extension_request isdn_chain_init_table[] = {
    { PARSE_ADD_CFG_TOP_CMD, &pname(isdn_config_commands) },
    { PARSE_ADD_CFG_INT_CMD, &pname(isdn_interface_commands) },
    { PARSE_ADD_DEBUG_CMD, &pname(isdn_debug_commands) },
    { PARSE_ADD_SHOW_CMD, &pname(isdn_show_commands) },
    { PARSE_ADD_CFG_SNMP_HOST_CMD, &pname(cfg_snmp_host_isdn_entry) },
    { PARSE_ADD_CFG_SNMP_HOST_EXIT,
	  (dynamic_transition *) &pname(cfg_snmp_host_isdn_exit) },
    { PARSE_ADD_CFG_SNMP_ENABLE_CMD, &pname(cfg_snmp_enable_isdn_entry) },
    { PARSE_ADD_CFG_SNMP_ENABLE_EXIT,
	      (dynamic_transition *) &pname(cfg_snmp_enable_isdn_exit) },
    { PARSE_LIST_END, NULL }
};


/*
 * isdn_parser_init - Initialize parse structures for parsing ISDN commands
 */
void isdn_parser_init (void)
{
    static boolean isdn_parser_initialized = FALSE;

    if (!isdn_parser_initialized) {
	parser_add_command_list(isdn_chain_init_table, "isdn");
	parser_add_link_point(PARSE_ADD_CFG_INT_ISDN_CMD, "isdn_interface",
			      &pname(ci_isdn_extend_here));
	isdn_parser_initialized = TRUE;

	if (registry_populated(REG_SNMP))
	    reg_add_snmp_host_nvgen(isdn_snmp_host_nvgen,
				    "isdn_snmp_host_nvgen");
    }
}
      
