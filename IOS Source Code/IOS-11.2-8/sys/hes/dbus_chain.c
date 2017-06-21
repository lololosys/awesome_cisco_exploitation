/* $Id: dbus_chain.c,v 3.3.44.3 1996/07/25 20:08:02 avaz Exp $
 * $Source: /release/112/cvs/Xsys/hes/dbus_chain.c,v $
 *------------------------------------------------------------------
 * Diag Bus parse chains
 *
 * May 1993, Robert Widmer
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: dbus_chain.c,v $
 * Revision 3.3.44.3  1996/07/25  20:08:02  avaz
 * CSCdi62098:  add show diagbus and show cont cbus to show tech-support
 * Branch: California_branch
 *
 * Revision 3.3.44.2  1996/07/23  23:23:31  avaz
 * CSCdi62098:  add show diagbus and show cont cbus to show tech-support
 * Branch: California_branch
 *
 * Revision 3.3.44.1  1996/05/21  09:47:38  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.3  1995/12/17  18:27:18  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.2  1995/11/17  09:15:26  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:38:03  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/07/02  10:51:51  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize os, hes, les, if, and ibm.
 *
 * Revision 2.1  1995/06/07  20:38:53  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include "config.h"
#include "parser.h"
#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "../hes/dbus.h"
#include "sys_registry.h"
#include "../os/techsupport.h"


/*
 * Parse chains for Diag Bus SHOW commands
 */
#define	ALTERNATE	NONE
#include "exec_show_diag.h"
LINK_POINT(dbus_show_commands, ALTERNATE);
#undef	ALTERNATE

/*
 * Add show diagbus to show tech-support registry
 */
static void show_diagbus_techsupport (parseinfo *csb)
{
  show_techsupport_command(csb, "show diagbus");
}


/*
 * dbus_parser_init - Initialize parser routines for the Diag Bus
 *                  - Add show diagbus to show tech-support registry
 */
void dbus_parser_init (void)
{
    static int dbus_parser_initialized = FALSE;

    if (!dbus_parser_initialized) {
	parser_add_commands(PARSE_ADD_SHOW_CMD, &pname(dbus_show_commands),
			    "dbus");
	reg_add_show_techsupport(show_diagbus_techsupport,
				 "show_diagbus_techsupport");
	dbus_parser_initialized = TRUE;
    }
}
