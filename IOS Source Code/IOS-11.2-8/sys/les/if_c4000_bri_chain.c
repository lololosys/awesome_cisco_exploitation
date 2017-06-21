/* $Id: if_c4000_bri_chain.c,v 3.3.10.1 1996/05/21 09:56:08 thille Exp $
 * $Source: /release/112/cvs/Xsys/les/if_c4000_bri_chain.c,v $
 *------------------------------------------------------------------
 * if_c4000_bri_chain.c -- Parser chains for C4000 BRI support.
 *
 * March 1994, Manoj Leelanivas.
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * 
$Endlog$
 */
#include "master.h"
#include "../ui/common_strings.h"
#include "interface_private.h"
#include "../ui/debug.h"
#include "ttysrv.h"
#include "logger.h"
#include "subsys.h"
#include "config.h"
#include "parser.h"
#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "if_c4000_mcn.h"
#include "if_c4000_m32.h"
#include "if_c4000_bri.h"


/*
 * Parse chains for C4000 BRI interface commands
 */
#define	ALTERNATE	NONE
#include "cfg_int_bri_c4000.h"
LINK_POINT(bri_c4000_interface_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chain registration array for C4000 BRI
 */
static parser_extension_request bri_c4000_init_table[] = {
    { PARSE_ADD_CFG_INT_AS_CMD, &pname(bri_c4000_interface_commands) },
    { PARSE_LIST_END, NULL }
};


/*
 * bri_c4000_parser_init - Initialize c4000 bri parser support
 */
void bri_c4000_parser_init (void)
{
    static boolean bri_c4000_parser_initialized = FALSE;

    if (!bri_c4000_parser_initialized) {
	parser_add_command_list(bri_c4000_init_table, "c4000 BRI");
    }

    bri_c4000_parser_initialized = TRUE;
}


