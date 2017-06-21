/* $Id: c4000_fddi_chain.c,v 3.6.10.1 1996/05/21 10:04:14 thille Exp $
 * $Source: /release/112/cvs/Xsys/src-c4000/c4000_fddi_chain.c,v $
 *------------------------------------------------------------------
 * Parser chains for C4000 FDDI support
 *
 * July 1994, Greg Stovall
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: c4000_fddi_chain.c,v $
 * Revision 3.6.10.1  1996/05/21  10:04:14  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.6  1996/02/22  14:38:24  widmer
 * CSCdi49281:  make all parser_extension_request static const
 * make parser_extension_request typedef const
 * make all parser_extension_request static
 *
 * Revision 3.5  1996/02/01  06:09:25  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.4  1996/01/29  07:30:04  hampton
 * Remove unnecessary includes of parser definition files.  [CSCdi47717]
 *
 * Revision 3.3  1995/12/17  18:38:08  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.2  1995/11/17  18:45:35  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:26:35  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/07/04  01:57:32  tli
 * CSCdi36508: Rationalize parser file placement, part I
 * Rationalize cls dev dlsw dspu filesys irix lat les lex llc2 parser pgen
 * pt servers smf smrp snmp src-36-c1000 src-4k-c4000 src-68-c3000
 * src-68-c4000 src-c4000 src-rsp srt tcl ts ui
 *
 * Revision 2.1  1995/06/07  22:56:26  hampton
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
#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "../if/parser_defs_fddi.h"
#include "../src-c4000/c4000_bsi_public.h"
#include "../parser/parser_defs_parser.h"


/*
 * Parse chains for C4000 FDDI show controller commands
 */
#define	ALTERNATE	NONE
#include "exec_show_controllers_cam.h"
LINK_POINT(fddi_c4000_show_control_commands, ALTERNATE);
#undef  ALTERNATE


/*
 * Parse chain registration array for C4000 FDDI
 */
static parser_extension_request fddi_c4000_init_table[] = {
    { PARSE_ADD_SHOW_CONTROLLER_CMD,
	  &pname(fddi_c4000_show_control_commands) },
    { PARSE_LIST_END, NULL }
};


/*
 * fddi_c4000_parser_init - Initialize c4000 FDDI parser support
 */
void fddi_c4000_parser_init (void)
{
    static boolean fddi_c4000_initialized = FALSE;

    if (!fddi_c4000_initialized) {
	parser_add_command_list(fddi_c4000_init_table, "c4000 FDDI");
	fddi_c4000_initialized = TRUE;
    }
}
