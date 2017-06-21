/* $Id: if_les_async_chain.c,v 3.1.6.1 1996/06/12 19:56:10 jng Exp $
 * $Source: /release/112/cvs/Xsys/les/if_les_async_chain.c,v $
 *------------------------------------------------------------------
 * if_les_async_chain.c  - Async parser chain for low-end systems.
 *
 * March 1996, Dave Coli
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: if_les_async_chain.c,v $
 * Revision 3.1.6.1  1996/06/12  19:56:10  jng
 * CSCdi60260:  Async catwoman commit
 * Branch: California_branch
 * Async Catwoman commit into California
 *
 * Revision 3.1  1996/03/20  01:28:57  jng
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
#include "interface_private.h"


/*
 * Async show controllers command
 */
#define	ALTERNATE	NONE
#include "exec_show_controllers_async.h"
LINK_POINT(async_showcontrollers_command, ALTERNATE);
#undef	ALTERNATE


void async_show_controller_parser_init (void)
{
    parser_add_commands(PARSE_ADD_SHOW_CONTROLLER_CMD,
                        &pname(async_showcontrollers_command), "async");
}
