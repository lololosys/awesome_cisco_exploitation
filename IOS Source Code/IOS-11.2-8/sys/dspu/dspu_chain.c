/* $Id: dspu_chain.c,v 3.5.10.1 1996/05/21 09:46:06 thille Exp $
 * $Source: /release/112/cvs/Xsys/dspu/dspu_chain.c,v $
 *------------------------------------------------------------------
 * Down Stream PU parse chains
 * 
 * Mar. 1994, Vivian Tseng
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: dspu_chain.c,v $
 * Revision 3.5.10.1  1996/05/21  09:46:06  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.5  1996/02/22  14:32:49  widmer
 * CSCdi49281:  make all parser_extension_request static const
 * make parser_extension_request typedef const
 * make all parser_extension_request static
 *
 * Revision 3.4  1996/02/01  06:01:55  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.3  1995/12/17  18:26:19  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.2  1995/11/17  09:04:44  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:28:01  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.5  1995/11/08  20:56:22  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.4  1995/08/09  13:16:15  ppearce
 * CSCdi38432:  dspu subsystem causing confusion, rename it
 *   Change dspu subsystem name in subsytems that require dspu
 *
 * Revision 2.3  1995/07/04  01:53:05  tli
 * CSCdi36508: Rationalize parser file placement, part I
 * Rationalize cls dev dlsw dspu filesys irix lat les lex llc2 parser pgen
 * pt servers smf smrp snmp src-36-c1000 src-4k-c4000 src-68-c3000
 * src-68-c4000 src-c4000 src-rsp srt tcl ts ui
 *
 * Revision 2.2  1995/06/28  10:18:58  smackie
 * Subsystem header cleanup. Round up the last few stragglers and rope 'em
 * in with the rest of the herd. (CSCdi23568)
 *
 * Revision 2.1  1995/06/07  20:28:29  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include "config.h"
#include "../ui/debug.h"
#include "parser.h"
#include "packet.h"
#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "dspu_notification.h"
#include "dspu_debug.h"
#include "dspu_debug_flags.h"
#include "parser_defs_dspu.h"
#include "../parser/parser_defs_exec.h"
#include "../parser/parser_defs_parser.h"
#include "../parser/parser_defs_snmp.h"
#include "subsys.h"
#include "registry.h"
#include "../snmp/snmp_api.h"
#include "../snmp/snmp_registry.regh"


/*
 * Parse chains for DSPU show commands
 */
#define	ALTERNATE	NONE
#include "exec_show_dspu.h"
LINK_POINT(dspu_show_command, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chains for DSPU config commands
 */
#define	ALTERNATE	NONE
#include "cfg_dspu.h"
LINK_POINT(dspu_command, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chains for DSPU interface commands
 */
#define ALTERNATE       NONE
#include "cfg_int_dspu.h"
LINK_POINT(dspuif_command, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chains for DSPU debug commands
 */
#define	ALTERNATE	NONE
#include "exec_debug_dspu.h"
LINK_POINT(dspu_debug_command, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chain for DSPU snmp-server host command
 */
LINK_EXIT(cfg_snmp_host_dspu_exit, no_alt);
KEYWORD_OR(cfg_snmp_host_dspu, cfg_snmp_host_dspu_exit, NONE,
	   OBJ(int,1), (1<<TRAP_DSPU),
	   "dspu", "Allow dspu event traps", PRIV_CONF);
LINK_POINT(cfg_snmp_host_dspu_entry, cfg_snmp_host_dspu);

static void dspu_snmp_host_nvgen (ulong flags) {
    nv_add(flags & (1 << TRAP_DSPU), " dspu");
}

/*
 * Parse chain registration array for DSPU
 */
static parser_extension_request dspu_chain_init_table[] = {
    { PARSE_ADD_DEBUG_CMD, &pname(dspu_debug_command) },
    { PARSE_ADD_SHOW_CMD, &pname(dspu_show_command) },
    { PARSE_ADD_CFG_AFTER_SRB_CMD, &pname(dspu_command) },
    { PARSE_ADD_CFG_INT_AS2_CMD, &pname(dspuif_command) },
    { PARSE_ADD_CFG_SNMP_HOST_CMD, &pname(cfg_snmp_host_dspu_entry) },
    { PARSE_ADD_CFG_SNMP_HOST_EXIT,
	  (dynamic_transition *) &pname(cfg_snmp_host_dspu_exit) },
    { PARSE_LIST_END, NULL }
};


/*
 * dspu_ui_init - Initialize DSPU parser support
 */
static void dspu_ui_init (subsystype *subsys)
{
    parser_add_command_list(dspu_chain_init_table, "dspu");
    reg_add_snmp_host_nvgen(dspu_snmp_host_nvgen, "dspu_snmp_host_nvgen");
    dspu_debug_init();
}

/*
 * DSPU user interface (DSPU_UI) subsystem header
 */
#define DSPU_UI_MAJVERSION 1
#define DSPU_UI_MINVERSION 0
#define DSPU_UI_EDITVERSION  1
 
SUBSYS_HEADER(dspu_ui,
              DSPU_UI_MAJVERSION, DSPU_UI_MINVERSION, DSPU_UI_EDITVERSION,
              dspu_ui_init, SUBSYS_CLASS_MANAGEMENT,
              "req: sna_pu", NULL);
      
