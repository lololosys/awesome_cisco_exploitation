/* $Id: sdlc_chain.c,v 3.7.10.2 1996/05/21 09:48:54 thille Exp $
 * $Source: /release/112/cvs/Xsys/ibm/sdlc_chain.c,v $
 *------------------------------------------------------------------
 * SDLC parse chains
 *
 * May 1993, Robert Widmer
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: sdlc_chain.c,v $
 * Revision 3.7.10.2  1996/05/21  09:48:54  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.7.10.1  1996/05/17  11:21:19  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.7.2.1  1996/04/26  07:54:32  dstine
 * Branch: IbuMod_Calif_branch
 * - commit of IBU modularity work, phase 1.
 *
 * Revision 3.7  1996/02/22  14:33:44  widmer
 * CSCdi49281:  make all parser_extension_request static const
 * make parser_extension_request typedef const
 * make all parser_extension_request static
 *
 * Revision 3.6  1996/02/01  06:03:26  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.5  1996/01/18  15:46:42  anke
 * CSCdi46969:  Change empty req and/or seq strings in SUBSYS_HEADERs to
 *              NULL
 *              Five bytes saved is five bytes earned
 *
 * Revision 3.4  1995/12/17  18:28:08  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.3  1995/11/27  16:23:41  widmer
 * CSCdi33598:  WAN commands (eg X.25) in parser even when Subsystem not
 * included
 * Create parser link for interface encapsulation types
 *
 * Revision 3.2  1995/11/17  09:23:48  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:46:01  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/07/02  10:53:59  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize os, hes, les, if, and ibm.
 *
 * Revision 2.1  1995/06/07  20:48:02  hampton
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
#include "../ui/debug.h"
#include "parser.h"
#include "logger.h"
#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "../ibm/ibm.h"
#include "../ibm/sdlc.h"
#include "../ibm/sdlc_debug.h"
#include "../ibm/parser_defs_sdlc.h"
#include "../ibm/parser_defs_stun.h"
#include "../parser/parser_defs_parser.h"
#include "../parser/parser_defs_snmp.h"
#include "registry.h"
#include "../snmp/snmp_api.h"
#include "../snmp/snmp_registry.regh"

/*
 * Parse chains for SDLC exec commands
 */
#define ALTERNATE       NONE
#include "exec_sdlc.h"
LINK_POINT(sdlc_exec_commands, ALTERNATE);
#undef  ALTERNATE


/*
 * Parse chains for SDLC debug commands
 */
#define	ALTERNATE	NONE
#include "exec_debug_sdlc.h"
LINK_POINT(sdlc_debug_commands, ALTERNATE);
#undef	ALTERNATE


/* Since there is an order in which the stun and sdlc commands need
 * to be configured, the stun interface commands were moved to this
 * file from stun_chain.c. This file now contains all the interface
 * commands for stun and sdlc.
 */

/*
 * Parse chains for STUN and SDLC interface commands
 */
#define	ALTERNATE	NONE
#include "cfg_int_stun2.h"
#include "cfg_int_sdlc.h"
#include "cfg_int_stun.h"
LINK_POINT(stun_sdlc_interface_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chain for SDLC interface encapsulation command
 */
#define	ALTERNATE	NONE
#include "cfg_int_encaps_sdlc.h"
LINK_POINT(sdlc_encaps_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chain for SDLC snmp-server host command
 */
LINK_EXIT(cfg_snmp_host_sdlc_exit, no_alt);
KEYWORD_OR(cfg_snmp_host_sdlc, cfg_snmp_host_sdlc_exit, NONE,
	   OBJ(int,1), (1<<TRAP_SDLC),
	   "sdlc", "Allow sdlc event traps", PRIV_CONF);
LINK_POINT(cfg_snmp_host_sdlc_entry, cfg_snmp_host_sdlc);

static void sdlc_snmp_host_nvgen (ulong flags)
{
    nv_add(flags & (1 << TRAP_SDLC), " sdlc");
}


/*
 * Parse chain registration array for SDLC
 */
static parser_extension_request sdlc_chain_init_table[] = {
    { PARSE_ADD_EXEC_CMD, &pname(sdlc_exec_commands) },
    { PARSE_ADD_DEBUG_CMD, &pname(sdlc_debug_commands) },
    { PARSE_ADD_CFG_INT_AS_CMD, &pname(stun_sdlc_interface_commands) },
    { PARSE_ADD_CFG_SNMP_HOST_CMD, &pname(cfg_snmp_host_sdlc_entry) },
    { PARSE_ADD_CFG_SNMP_HOST_EXIT,
	  (dynamic_transition *) &pname(cfg_snmp_host_sdlc_exit) },
    { PARSE_ADD_INT_ENCAPS_CMD, &pname(sdlc_encaps_commands) },
    { PARSE_ADD_INT_ENCAPS_EXIT,
	  (dynamic_transition *) &pname(cfg_encap_sdlc_exit) },
    { PARSE_LIST_END, NULL }
};


/*
 * sdlc_parser_init - Initialize SDLC parser support
 */
void sdlc_parser_init (subsystype *subsys)
{
    parser_add_command_list(sdlc_chain_init_table, "stun/sdlc");
    reg_add_snmp_host_nvgen(sdlc_snmp_host_nvgen, "sdlc_snmp_host_nvgen");
}


/*
 * SDLC PRI UI (SDLC_UI) subsystem header
 */

#define SDLC_UI_MAJVERSION 1
#define SDLC_UI_MINVERSION 0
#define SDLC_UI_EDITVERSION  1

SUBSYS_HEADER(sdlc_ui, SDLC_UI_MAJVERSION, SDLC_UI_MINVERSION, SDLC_UI_EDITVERSION,
	      sdlc_parser_init, SUBSYS_CLASS_MANAGEMENT,
	      "seq: rsrb_ui",
	      NULL);
      
