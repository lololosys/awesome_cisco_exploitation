/* $Id: gdb_chain.c,v 3.6.10.2 1996/05/21 09:58:11 thille Exp $
 * $Source: /release/112/cvs/Xsys/os/gdb_chain.c,v $
 *------------------------------------------------------------------
 * GDB parse chains
 *
 * May 1993, Robert Widmer
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: gdb_chain.c,v $
 * Revision 3.6.10.2  1996/05/21  09:58:11  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.6.10.1  1996/03/18  21:29:50  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.14.3  1996/03/16  07:20:38  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.3.14.2  1996/03/09  05:09:35  hampton
 * Slim down the kernel of the router.
 * Branch: DeadKingOnAThrone_branch
 *
 * Revision 3.3.14.1  1996/02/20  16:39:33  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.6  1996/02/22  14:36:40  widmer
 * CSCdi49281:  make all parser_extension_request static const
 * make parser_extension_request typedef const
 * make all parser_extension_request static
 *
 * Revision 3.5  1996/02/07  16:14:24  widmer
 * CSCdi48285:  Remove function name from bad_parser_subcommand
 *
 * Revision 3.4  1996/02/01  06:07:40  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.3  1995/12/17  18:34:13  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.2  1995/11/17  18:48:07  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:46:02  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/07/02  10:51:15  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize os, hes, les, if, and ibm.
 *
 * Revision 2.1  1995/06/07  21:55:27  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include "config.h"
#include "parser.h"
#include "ttysrv.h"
#include "subsys.h"
#include "sys_registry.h"
#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "parser_defs_gdb.h"
#include "../os/gdb.h"
#include "../os/sum.h"


/*
 * Parse chains for GDB exec commands
 */
#define	ALTERNATE	NONE
#include "exec_gdb.h"
LINK_POINT(gdb_exec_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chain registration array for GDB
 */
static parser_extension_request gdb_chain_init_table[] = {
    { PARSE_ADD_EXEC_CMD, &pname(gdb_exec_commands) },
    { PARSE_LIST_END, NULL }
};


/*
 * gdb_ui_init - Initialize GDB parser support
 */
static void gdb_ui_init (subsystype *subsys)
{
    parser_add_command_list(gdb_chain_init_table, "gdb");
}


/*
 * gdb_command - parse the GDB command
 */

void gdb_command (parseinfo *csb)
{
    boolean install = FALSE;

    /* Turn off paranoia checking--we might plant breakpoints. */

    defeat_sum();
    gdb_set_reloc_sem();

    switch (csb->which) {
      case GDB_DEBUG:
	debug_process(GETOBJ(int,1));
	install = TRUE;
	break;

      case GDB_EXAMINE:
	examine_process(GETOBJ(int,1));
	install = TRUE;
	break;

      case GDB_KERNEL:
	if (stdio->type == CTY_LINE) {
	    gdb_kernel_enable();
	    breakpoint();
	} else {
	    printf("\nKernel GDB allowed on console terminal only");
	}
	break;

      default:
	gdb_clear_reloc_sem();
	bad_parser_subcommand(csb, csb->which);
	break;
    }

    if (install) {
	reg_add_death(gdb_shutdown, "gdb_shutdown");
    }
}

/*
 * gdb_ui subsystem header
 */
 
#define GDB_UI_MAJVERSION    1
#define GDB_UI_MINVERSION    0
#define GDB_UI_EDITVERSION   1
 
SUBSYS_HEADER(gdb_ui, GDB_UI_MAJVERSION, GDB_UI_MINVERSION,
              GDB_UI_EDITVERSION,
              gdb_ui_init, SUBSYS_CLASS_MANAGEMENT,
              NULL, NULL);
