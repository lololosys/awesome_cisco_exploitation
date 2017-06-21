/* $Id: ctrace_command.c,v 3.1.40.1 1996/05/21 10:06:37 thille Exp $
 * $Source: /release/112/cvs/Xsys/testtools/ctrace_command.c,v $
 *------------------------------------------------------------------
 * Implemetation of ctrace command
 *
 * January 1996, Lakshmankumar Mukkavilli
 *
 * Copyright (c) 1996-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ctrace_command.c,v $
 * Revision 3.1.40.1  1996/05/21  10:06:37  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.1  1996/01/31  18:24:13  lakshman
 * CSCdi47664:  Augment cflow with tracing capability
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include "config.h"
#include "interface_private.h"

#include "parser.h" 
#include "../parser/parser_actions.h"
#include "../parser/macros.h"

/*
 * Parse chain for
 * ctrace {on [nowrap] | off | status | fastcopy |
 *                  copy [{ <ipaddr> | <hostname> } [<filename>]}]
 */

#define ALTERNATE NONE
#include "../testtools/exec_ctrace.h"
LINK_POINT(ctrace_exec_command, ALTERNATE);
#undef ALTERNATE

/*
 * ctrace_parser_init - initialize the parser support for the ctrace
 */
void ctrace_parser_init (void)
{
  parser_add_commands(PARSE_ADD_EXEC_CMD,
		      &pname(ctrace_exec_command), "ctrace");
}
