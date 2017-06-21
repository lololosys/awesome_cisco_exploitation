/* $Id: pakgen_chain.c,v 3.3.20.1 1996/05/21 10:00:29 thille Exp $
 * $Source: /release/112/cvs/Xsys/pgen/pakgen_chain.c,v $
 *------------------------------------------------------------------
 * P A K G E N _ C H A I N . C
 *
 * Parse chains for pakgen. This is a separate file from
 * the system-wide "chain.c" file because we need to keep the rest
 * of the system as free as possible of packet-generator code.
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: pakgen_chain.c,v $
 * Revision 3.3.20.1  1996/05/21  10:00:29  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.3  1996/01/29  07:29:24  hampton
 * Remove unnecessary includes of parser definition files.  [CSCdi47717]
 *
 * Revision 3.2  1995/12/17  18:36:11  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.1  1995/11/09  12:59:06  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/09/14  01:52:03  biren
 * CSCdi40312:  Move pakgen and pgen (Pagent 2.2) to seperate subsystems
 *
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
#include "sys_registry.h"
#include "pakgen.h"


/*
 * Parse chain TEST pgen command
 */
#define	ALTERNATE	no_alt
#include "exec_test_pakgen.h"
LINK_POINT(pgen_test_command, ALTERNATE);
#undef	ALTERNATE


/*
 *  Initialize Pakgen parser support
 */
void pakgen_parser_init (void)
{
    parser_add_commands(PARSE_ADD_TEST_CMD, &pname(pgen_test_command), "pgen");
}
