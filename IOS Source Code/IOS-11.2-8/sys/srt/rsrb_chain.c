/* $Id: rsrb_chain.c,v 3.6.10.3 1996/07/09 06:05:37 ppearce Exp $
 * $Source: /release/112/cvs/Xsys/srt/rsrb_chain.c,v $
 *------------------------------------------------------------------
 * Non-pure SRB parse chains
 *
 * Nov  1994, Jacob Teplitsky
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: rsrb_chain.c,v $
 * Revision 3.6.10.3  1996/07/09  06:05:37  ppearce
 * CSCdi59527: inlines waste code space
 * Branch: California_branch
 *   Just say "no" to inline abuse - SRB subblock inlines removed
 *
 * Revision 3.6.10.2  1996/05/21  10:05:25  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.6.10.1  1996/05/17  12:13:43  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.6.2.2  1996/04/26  15:23:20  ppearce
 * IBU modularity - SRB subblock (part 1)
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.6.2.1  1996/04/26  07:58:35  dstine
 * Branch: IbuMod_Calif_branch
 * - commit of IBU modularity work, phase 1.
 *
 * Revision 3.6  1996/02/22  14:38:45  widmer
 * CSCdi49281:  make all parser_extension_request static const
 * make parser_extension_request typedef const
 * make all parser_extension_request static
 *
 * Revision 3.5  1996/02/01  06:10:23  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.4  1996/01/29  07:30:10  hampton
 * Remove unnecessary includes of parser definition files.  [CSCdi47717]
 *
 * Revision 3.3  1995/12/17  18:38:51  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.2  1995/11/17  18:55:48  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:31:52  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/07/04  01:58:15  tli
 * CSCdi36508: Rationalize parser file placement, part I
 * Rationalize cls dev dlsw dspu filesys irix lat les lex llc2 parser pgen
 * pt servers smf smrp snmp src-36-c1000 src-4k-c4000 src-68-c3000
 * src-68-c4000 src-c4000 src-rsp srt tcl ts ui
 *
 * Revision 2.2  1995/06/28  09:31:26  smackie
 * Repair widespread subsystem header braindamage. (CSCdi23568)
 *
 *   o Fix subsystem entry points to be foo(subsystype *) not foo(void)
 *   o Repair nonsensical and redundant subsystem sequences
 *   o Use NULL where no property is required
 *
 * Revision 2.1  1995/06/07  23:01:34  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
#include "master.h"
#include "../ui/common_strings.h"
#include "config.h"
#include "interface_generic.h"
#include "subsys.h"
#include "parser.h"
#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "access.h"
#include "../if/tring.h"
#include "../srt/rsrb.h"
#include "../tcp/tcp.h"
#include "../ibm/sdlc.h"
#include "packet.h"
#include "../srt/span.h"
#include "../ui/debug.h"
#include "../srt/srt_debug.h"


#include "../parser/parser_defs_parser.h"
#include "../ibm/parser_defs_sdlc.h"
#include "../srt/parser_defs_srb.h"
#include "../parser/parser_defs_sse.h"
#include "../parser/parser_defs_snmp.h"
#include "registry.h"
#include "../snmp/snmp_api.h"
#include "../snmp/snmp_registry.regh"

/*
 * Parse chains for SRB debug commands
 */
#define	ALTERNATE	NONE
#include "exec_debug_localack.h"
LINK_POINT(rsrb_debug_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chains for RSRB show commands
 */
#define	ALTERNATE	NONE
#include "exec_show_sdllc.h"
#include "exec_show_lack.h"
LINK_POINT(rsrb_show_commands, ALTERNATE);
#undef	ALTERNATE

/*
 * Parse chains for SRB config commands
 */
#define	ALTERNATE	NONE
#include "cfg_rsrb_filter.h"
LINK_POINT(rsrb_config_filter_commands, ALTERNATE);
#undef	ALTERNATE

#define	ALTERNATE	NONE
#include "cfg_rsrb.h"
LINK_POINT(rsrb_config_commands, ALTERNATE);
#undef	ALTERNATE

/*
 * Parse chains for interface commands
 */
#define	ALTERNATE	NONE
#include "cfg_int_sdllc.h"
LINK_POINT(sdllc_interface_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chain for RSRB snmp-server host command
 */
LINK_EXIT(cfg_snmp_host_rsrb_exit, no_alt);
KEYWORD_OR(cfg_snmp_host_rsrb, cfg_snmp_host_rsrb_exit, NONE,
	   OBJ(int,1), (1<<TRAP_RSRB),
	   "rsrb", "Allow rsrb event traps", PRIV_CONF);
LINK_POINT(cfg_snmp_host_rsrb_entry, cfg_snmp_host_rsrb);

static void rsrb_snmp_host_nvgen (ulong flags)
{
    nv_add(flags & (1 << TRAP_RSRB), " rsrb");
}


/*
 * Parse chain for SDLLC snmp-server host command
 */
LINK_EXIT(cfg_snmp_host_sdllc_exit, no_alt);
KEYWORD_OR(cfg_snmp_host_sdllc, cfg_snmp_host_sdllc_exit, NONE,
	   OBJ(int,1), (1<<TRAP_SDLLC),
	   "sdllc", "Allow sdllc event traps", PRIV_CONF);
LINK_POINT(cfg_snmp_host_sdllc_entry, cfg_snmp_host_sdllc);

static void sdllc_snmp_host_nvgen (ulong flags)
{
    nv_add(flags & (1 << TRAP_SDLLC), " sdllc");
}


/*
 * Parse chain registration array for RSRB
 */
static parser_extension_request rsrb_chain_init_table[] = {
    { PARSE_ADD_DEBUG_CMD, &pname(rsrb_debug_commands) },
    { PARSE_ADD_CFG_TOP_SRB_CMD, &pname(rsrb_config_commands) },
    { PARSE_ADD_CFG_INT_AS_CMD, &pname(sdllc_interface_commands) },
    { PARSE_ADD_SHOW_CMD, &pname(rsrb_show_commands) },
    { PARSE_ADD_CFG_TOP_AC_CMD, &pname(rsrb_config_filter_commands) },
    { PARSE_ADD_CFG_SNMP_HOST_CMD, &pname(cfg_snmp_host_rsrb_entry) },
    { PARSE_ADD_CFG_SNMP_HOST_EXIT,
	  (dynamic_transition *) &pname(cfg_snmp_host_rsrb_exit) },
    { PARSE_ADD_CFG_SNMP_HOST_CMD, &pname(cfg_snmp_host_sdllc_entry) },
    { PARSE_ADD_CFG_SNMP_HOST_EXIT,
	  (dynamic_transition *) &pname(cfg_snmp_host_sdllc_exit) },
    { PARSE_LIST_END, NULL }
};


/*
 * rsrb_parser_init - Initialize RSRB parser support
 */
static void rsrb_parser_init (subsystype *subsys)
{
    parser_add_command_list(rsrb_chain_init_table, "rsrb");
    reg_add_snmp_host_nvgen(rsrb_snmp_host_nvgen, "rsrb_snmp_host_nvgen");
    reg_add_snmp_host_nvgen(sdllc_snmp_host_nvgen, "sdllc_snmp_host_nvgen");
}


/*
 * Source Route Bridging user interface (SRB_UI) subsystem header
 */

#define RSRB_UI_MAJVERSION 1
#define RSRB_UI_MINVERSION 0
#define RSRB_UI_EDITVERSION  1

SUBSYS_HEADER(rsrb_ui, RSRB_UI_MAJVERSION, RSRB_UI_MINVERSION,
	      RSRB_UI_EDITVERSION,
	      rsrb_parser_init, SUBSYS_CLASS_MANAGEMENT,
	      "req: srb", NULL);
      
