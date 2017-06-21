/* $Id: pgen_chain.c,v 3.6.4.1 1996/05/21 10:00:29 thille Exp $
 * $Source: /release/112/cvs/Xsys/pgen/pgen_chain.c,v $
 *------------------------------------------------------------------
 * P G E N _ C H A I N . C
 *
 * Parse chains for packet generator. This is a separate file from
 * the system-wide "chain.c" file because we need to keep the rest
 * of the system as free as possible of packet-generator code.
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: pgen_chain.c,v $
 * Revision 3.6.4.1  1996/05/21  10:00:29  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.6  1996/03/09  22:57:17  widmer
 * CSCdi47890:  HTTP server doesnt work
 * Add HTTP server
 *
 * Revision 3.5  1996/01/29  07:29:26  hampton
 * Remove unnecessary includes of parser definition files.  [CSCdi47717]
 *
 * Revision 3.4  1995/12/17  18:36:12  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.3  1995/12/01  15:57:11  widmer
 * CSCdi44724:  Remove nv_top argument to parser_add_mode()
 * Clean up "no" handling at beginning of modes
 * Fix appn modes
 * Removed unneded pdecl's
 *
 * Revision 3.2  1995/11/17  18:56:27  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:59:09  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/09/14  01:52:05  biren
 * CSCdi40312:  Move pakgen and pgen (Pagent 2.2) to seperate subsystems
 *
 * Revision 2.2  1995/07/04  01:56:18  tli
 * CSCdi36508: Rationalize parser file placement, part I
 * Rationalize cls dev dlsw dspu filesys irix lat les lex llc2 parser pgen
 * pt servers smf smrp snmp src-36-c1000 src-4k-c4000 src-68-c3000
 * src-68-c4000 src-c4000 src-rsp srt tcl ts ui
 *
 * Revision 2.1  1995/06/07  22:34:10  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include "config.h"
#include "parser.h"
#include "interface_private.h"
#include "packet.h"
#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "../parser/parser_defs_parser.h"
#include "pgen_private.h"
#include "sys_registry.h"


/*
 * Parse chain for main command mode...
 */
#define	ALTERNATE no_alt
#include "exec_pgen_mode.h"
HELP (top_pgen, ALTERNATE, "Packet generator commands:\n");
#undef	ALTERNATE


/*
 * Parse chain for hex data input mode...
 */
#define ALTERNATE no_alt
#include "pgen_hex_mode.h"
HELP (top_pgen_hex, ALTERNATE, "Hex input mode:\n");
#undef	ALTERNATE


/*
 * Pgen exec command
 */
#define	ALTERNATE	NONE
#include "exec_pgen.h"
LINK_POINT(pgen_exec_command, ALTERNATE);
#undef	ALTERNATE


/*
 *  Initialize pgen parser support
 */
void pgen_parser_init (void)
{
    parser_add_commands(PARSE_ADD_EXEC_CMD, &pname(pgen_exec_command), "pgen");
    parser_add_mode("pgen", "PaGenT", "PaGenT mode",
		    TRUE, TRUE, NULL, NULL, NULL,
		    &pname(top_pgen),
		    NULL);
    parser_add_mode("pgen-hex", "PaGenT-hex", "PaGenT patch mode",
		    TRUE, TRUE, NULL, NULL, NULL,
		    &pname(top_pgen_hex),
		    NULL);
}
