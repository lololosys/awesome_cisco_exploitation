/* $Id: stun_chain.c,v 3.7.10.2 1996/05/21 09:49:00 thille Exp $
 * $Source: /release/112/cvs/Xsys/ibm/stun_chain.c,v $
 *------------------------------------------------------------------
 * STUN parse chains
 *
 * May 1993, Robert Widmer
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: stun_chain.c,v $
 * Revision 3.7.10.2  1996/05/21  09:49:00  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.7.10.1  1996/05/17  11:21:48  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.7.2.1  1996/04/26  07:54:45  dstine
 * Branch: IbuMod_Calif_branch
 * - commit of IBU modularity work, phase 1.
 *
 * Revision 3.7  1996/02/22  14:33:48  widmer
 * CSCdi49281:  make all parser_extension_request static const
 * make parser_extension_request typedef const
 * make all parser_extension_request static
 *
 * Revision 3.6  1996/02/01  06:03:33  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.5  1996/01/29  07:28:05  hampton
 * Remove unnecessary includes of parser definition files.  [CSCdi47717]
 *
 * Revision 3.4  1995/12/17  18:28:11  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.3  1995/11/27  16:23:43  widmer
 * CSCdi33598:  WAN commands (eg X.25) in parser even when Subsystem not
 * included
 * Create parser link for interface encapsulation types
 *
 * Revision 3.2  1995/11/17  09:24:36  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:46:44  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/07/02  10:54:01  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize os, hes, les, if, and ibm.
 *
 * Revision 2.2  1995/06/28  09:26:06  smackie
 * Repair widespread subsystem header braindamage. (CSCdi23568)
 *
 *   o Fix subsystem entry points to be foo(subsystype *) not foo(void)
 *   o Repair nonsensical and redundant subsystem sequences
 *   o Use NULL where no property is required
 *
 * Revision 2.1  1995/06/07  20:49:11  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include "config.h"
#include "subsys.h"
#include "../ui/debug.h"
#include "parser.h"
#include "packet.h"
#include "logger.h"
#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "../ibm/stun_actions.h"
#include "../ibm/ibm.h"
#include "../ibm/stun.h"
#include "../ibm/sdlc_debug.h"
#include "../ibm/parser_defs_stun.h"
#include "../parser/parser_defs_priority.h"
#include "../parser/parser_defs_snmp.h"
#include "registry.h"
#include "../snmp/snmp_api.h"
#include "../snmp/snmp_registry.regh"
#include "interface_generic.h"


/*
 * Parse chains for STUN debug commands
 */
#define	ALTERNATE	NONE
#include "exec_debug_stun.h"
LINK_POINT(stun_debug_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chains for STUN show commands
 */
#define	ALTERNATE	NONE
#include "exec_show_stun.h"
LINK_POINT(stun_show_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chains for STUN config commands
 */
#define ALTERNATE       NONE
#include "cfg_stun.h"
LINK_POINT(stun_config_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chains for STUN priority queueing commands
 */
#define ALTERNATE       NONE
#include "cfg_priority_stun.h"
LINK_POINT(stun_priority_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chain for STUN interface encapsulation command
 */
LINK_EXIT(cfg_encaps_stun_exit, no_alt);
ENCAPS	(cfg_encaps_stun, cfg_encaps_stun_exit, NONE,
	 OBJ(int,1), ET_STUN,
	 "stun", "Serial tunneling (STUN)", IDB_SYNC);
LINK_POINT(cfg_encaps_stun_cmd, cfg_encaps_stun);


/*
 * Parse chain for STUN snmp-server host command
 */
LINK_EXIT(cfg_snmp_host_stun_exit, no_alt);
KEYWORD_OR(cfg_snmp_host_stun, cfg_snmp_host_stun_exit, NONE,
	   OBJ(int,1), (1<<TRAP_STUN),
	   "stun", "Allow stun event traps", PRIV_CONF);
LINK_POINT(cfg_snmp_host_stun_entry, cfg_snmp_host_stun);

static void stun_snmp_host_nvgen (ulong flags)
{
    nv_add(flags & (1 << TRAP_STUN), " stun");
}


/*
 * Parse chain registration array for STUN
 */
static parser_extension_request stun_chain_init_table[] = {
    { PARSE_ADD_DEBUG_CMD, &pname(stun_debug_commands) },
    { PARSE_ADD_SHOW_CMD, &pname(stun_show_commands) },
    { PARSE_ADD_CFG_TOP_CMD, &pname(stun_config_commands) },
    { PARSE_ADD_PRIORITY_OPTIONS, &pname(stun_priority_commands) },
    { PARSE_ADD_CFG_SNMP_HOST_CMD, &pname(cfg_snmp_host_stun_entry) },
    { PARSE_ADD_CFG_SNMP_HOST_EXIT,
	  (dynamic_transition *) &pname(cfg_snmp_host_stun_exit) },
    { PARSE_ADD_INT_ENCAPS_CMD, &pname(cfg_encaps_stun_cmd) },
    { PARSE_ADD_INT_ENCAPS_EXIT,
	  (dynamic_transition *) &pname(cfg_encaps_stun_exit) },
    { PARSE_LIST_END, NULL }
};


/*



 * stun_parser_init - Initialize STUN parser support
 */
void stun_parser_init (subsystype *subsys)
{
    parser_add_command_list(stun_chain_init_table, "stun");
    parser_add_protoaddr(LINK_STUN,		PMATCH_STUN,
			 "stun",		"Serial Tunnel",
			 0, ADDR_ILLEGAL);
    parser_add_address(ADDR_STUN, stun_addr_short_help, match_stun_addr);
    reg_add_snmp_host_nvgen(stun_snmp_host_nvgen, "stun_snmp_host_nvgen");
}

/*
 * STUN UI (STUN_UI) subsystem header
 */

#define STUN_UI_MAJVERSION 1
#define STUN_UI_MINVERSION 0
#define STUN_UI_EDITVERSION  1

SUBSYS_HEADER(stun_ui, STUN_UI_MAJVERSION, STUN_UI_MINVERSION,
	      STUN_UI_EDITVERSION,
	      stun_parser_init, SUBSYS_CLASS_MANAGEMENT,
	      "req: sdlc", NULL);
      
