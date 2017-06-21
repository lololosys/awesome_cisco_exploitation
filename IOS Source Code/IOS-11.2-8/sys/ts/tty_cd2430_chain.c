/* $Id: tty_cd2430_chain.c,v 3.1.6.1 1996/06/12 19:57:21 jng Exp $
 * $Source: /release/112/cvs/Xsys/ts/tty_cd2430_chain.c,v $
 *------------------------------------------------------------------
 * CD2430 parser commands
 *
 * March 1996, Dave Coli
 *
 * Copyright (c) 1996-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: tty_cd2430_chain.c,v $
 * Revision 3.1.6.1  1996/06/12  19:57:21  jng
 * CSCdi60260:  Async catwoman commit
 * Branch: California_branch
 * Async Catwoman commit into California
 *
 * Revision 3.1  1996/03/20  01:29:53  jng
 * Initial Async Catwoman placeholder
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "config.h"
#include "parser.h"
#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "../parser/parser_defs_parser.h"


/*
 * Commserver/async show controllers command
 */

#define ALTERNATE       NONE
#include "../parser/macros.h"
#include "exec_show_controllers_async.h"
LINK_POINT(cd2430_showcontrollers_command, ALTERNATE);
#undef  ALTERNATE



/*
 * Wire the async controller show command into the parse chain.
 * We use the async_show_controllers LIST to actually invoke the
 * "show async controller" function for each type of controller.
 */
void tty_cd2430_parser_init (void)
{
    parser_add_commands(PARSE_ADD_SHOW_CONTROLLER_CMD,
                        &pname(cd2430_showcontrollers_command), "async");
}

