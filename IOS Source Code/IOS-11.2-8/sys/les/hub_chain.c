/* $Id: hub_chain.c,v 3.8.4.2 1996/06/17 23:34:51 hampton Exp $
 * $Source: /release/112/cvs/Xsys/les/hub_chain.c,v $
 *------------------------------------------------------------------
 * Hub parse chains
 *
 * March 1994, Robert Widmer
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Gurus: 
 *------------------------------------------------------------------
 * $Log: hub_chain.c,v $
 * Revision 3.8.4.2  1996/06/17  23:34:51  hampton
 * Implement the long sought parser "default" keyword.  [CSCdi60510]
 * Branch: California_branch
 *
 * Revision 3.8.4.1  1996/05/21  09:56:04  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.8  1996/03/09  22:56:16  widmer
 * CSCdi47890:  HTTP server doesnt work
 * Add HTTP server
 *
 * Revision 3.7  1996/02/22  14:35:52  widmer
 * CSCdi49281:  make all parser_extension_request static const
 * make parser_extension_request typedef const
 * make all parser_extension_request static
 *
 * Revision 3.6  1996/02/01  06:06:35  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.5  1995/12/17  18:32:42  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.4  1995/12/16  00:14:15  sdurham
 * CSCdi44645:  MAC illegal address trap needs snmp-server host and enable
 * cli support.
 *    moved all parser commands related to mib to appropriate mib
 *    subsystem.
 *
 * Revision 3.3  1995/12/01  15:56:36  widmer
 * CSCdi44724:  Remove nv_top argument to parser_add_mode()
 * Clean up "no" handling at beginning of modes
 * Fix appn modes
 * Removed unneded pdecl's
 *
 * Revision 3.2  1995/11/17  17:37:44  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:30:32  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/11/08  21:13:55  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.3  1995/07/02  10:52:38  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize os, hes, les, if, and ibm.
 *
 * Revision 2.2  1995/06/15  15:18:58  widmer
 * CSCdi35889:  parser transition structures should be static
 *
 * Revision 2.1  1995/06/07  21:31:49  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include "logger.h"
#include "config.h"
#include "parser.h"
#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "parser_defs_hub.h"
#include "../ui/debug.h"
#include "../les/hub.h"
#include "../les/hub_debug.h"


/*
 * Parse chains for HUB clear commands
 */
#define	ALTERNATE	NONE
#include "exec_clear_hub.h"
LINK_POINT(hub_clear_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chains for HUB show commands
 */
#define	ALTERNATE	NONE
#include "exec_show_hub.h"
LINK_POINT(hub_show_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chains for HUB debug commands
 */
#define	ALTERNATE	NONE
#include "exec_debug_hub.h"
LINK_POINT(hub_debug_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chains for HUB config commands
 */
#define	ALTERNATE	NONE
#include "cfg_hub.h"
LINK_POINT(hub_config_commands, ALTERNATE);
#undef	ALTERNATE

/*****************************************************************************/
LINK_EXIT(after_hub_exit, no_alt);
#define	ALTERNATE	after_hub_exit
#include "cfg_hub_exit.h"
LINK_TRANS	(cfg_hub_extend_here, ALTERNATE);
#undef	ALTERNATE

#define	ALTERNATE	cfg_hub_extend_here
#include "cfg_hub_link-test.h"
#include "cfg_hub_auto-polarity.h"
#include "cfg_hub_source-address.h"
#include "cfg_hub_shutdown.h"

NO_OR_DEFAULT(config_hub_no, ALTERNATE, PRIV_MIN | PRIV_NONVGEN);
HELP	(top_config_hub, config_hub_no,
	 "Hub configuration commands:\n");
/*****************************************************************************/


static parser_extension_request hub_chain_init_table[] = {
    { PARSE_ADD_CLEAR_CMD, &pname(hub_clear_commands) },
    { PARSE_ADD_SHOW_CMD, &pname(hub_show_commands) },
    { PARSE_ADD_DEBUG_CMD, &pname(hub_debug_commands) },
    { PARSE_ADD_CFG_HUB_CMD, &pname(hub_config_commands) },
    { PARSE_ADD_GLOBAL_EXIT,
	  (dynamic_transition *) &pname(after_hub_exit) },
    { PARSE_LIST_END, NULL }
};

static void *hub_save_var (parseinfo *csb)
{
    hub_t *hub = csb->hub;

    csb->hub = NULL;
    return(hub);
}

static void hub_reset_var (parseinfo *csb, void *var)
{
    csb->hub = var;
}

void hub_parser_init (void)
{
    parser_add_link_point(PARSE_ADD_CFG_HUB_PORT_CMD, "config hub",
			  &pname(cfg_hub_extend_here));
    parser_add_command_list(hub_chain_init_table, "hub");
    parser_add_mode("hub", "config-hub", "Hub configuration mode",
		    TRUE, TRUE, "configure",
		    hub_save_var, hub_reset_var,
		    &pname(top_config_hub),
		    NULL);
}
