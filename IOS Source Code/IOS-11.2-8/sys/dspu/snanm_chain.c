/* $Id: snanm_chain.c,v 3.5.10.1 1996/05/21 09:46:11 thille Exp $
 * $Source: /release/112/cvs/Xsys/dspu/snanm_chain.c,v $
 *------------------------------------------------------------------
 * SNA Network Management Parser chain
 *
 * Jan 95, Barry Chan
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: snanm_chain.c,v $
 * Revision 3.5.10.1  1996/05/21  09:46:11  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.5  1996/02/22  14:32:52  widmer
 * CSCdi49281:  make all parser_extension_request static const
 * make parser_extension_request typedef const
 * make all parser_extension_request static
 *
 * Revision 3.4  1996/02/01  06:02:03  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.3  1995/12/17  18:26:22  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.2  1995/11/17  09:06:12  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:29:58  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/07/04  01:53:09  tli
 * CSCdi36508: Rationalize parser file placement, part I
 * Rationalize cls dev dlsw dspu filesys irix lat les lex llc2 parser pgen
 * pt servers smf smrp snmp src-36-c1000 src-4k-c4000 src-68-c3000
 * src-68-c4000 src-c4000 src-rsp srt tcl ts ui
 *
 * Revision 2.1  1995/06/07  20:30:39  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include "config.h"
#include "parser.h"
#include "packet.h"
#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "parser_defs_dspu.h"
#include "../parser/parser_defs_exec.h"
#include "../parser/parser_defs_parser.h"


/*
 * Parse chains for SNA show commands
 */
#define	ALTERNATE	NONE
#include "exec_show_snanm.h"
LINK_POINT(snanm_show_command, ALTERNATE);
#undef	ALTERNATE

/*
 * Parse chains for SNA show commands
 */
#define	ALTERNATE	NONE
#include "cfg_snanm.h"
LINK_POINT(sna_command, ALTERNATE);
#undef	ALTERNATE


#define ALTERNATE	NONE
#include "cfg_int_snanm.h"
LINK_POINT(snaif_command, ALTERNATE);
#undef ALTERNATE

/*
 * Parse chains for SNA debug commands
 */
#define	ALTERNATE	NONE
#include "exec_debug_snanm.h"
LINK_POINT(sna_debug_command, ALTERNATE);
#undef	ALTERNATE

/*
 * Parse chain registration array for SNA
 */
static parser_extension_request snanm_chain_init_table[] = {
    { PARSE_ADD_DEBUG_CMD, &pname(sna_debug_command) },
    { PARSE_ADD_SHOW_CMD, &pname(snanm_show_command) },
    { PARSE_ADD_CFG_AFTER_SRB_CMD, &pname(sna_command) },
    { PARSE_ADD_CFG_INT_AS2_CMD, &pname(snaif_command) },
    { PARSE_LIST_END, NULL }
};


/*
 * sna_parser_init - Initialize SNA parser support
 */
void snanm_parser_init (void)
{
    parser_add_command_list(snanm_chain_init_table, "sna");
}
