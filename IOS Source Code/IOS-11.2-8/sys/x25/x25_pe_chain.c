/* $Id: x25_pe_chain.c,v 3.1.2.2 1996/05/21 10:10:43 thille Exp $
 * $Source: /release/112/cvs/Xsys/x25/x25_pe_chain.c,v $
 *------------------------------------------------------------------
 * X.25 protocol engine parser chains
 *
 * April 1996, Gilbert Glick
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: x25_pe_chain.c,v $
 * Revision 3.1.2.2  1996/05/21  10:10:43  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.1.2.1  1996/04/11  23:47:42  gglick
 * CSCdi54514:  X.25 parser modularity files
 * Branch: California_branch
 *
 * Revision 3.1  1996/04/11  23:40:04  gglick
 * Initial commit of X.25 parser modularization files.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include "interface_private.h"
#include "config.h"
#include "parser.h"
#include "registry.h"
#include "media_registry.h"
#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "../parser/parser_defs_parser.h"
#include "parser_defs_x25.h"
#include "ll_ul.h"
#include "x25_library.h"
#include "x25_packet.h"
#include "x25_registry.h"
#include "x25_support.h"


/* 
 * Parse chains for X.25 interface config commands
 */
#define	ALTERNATE	NONE
#include "cfg_int_x25.h"
LINK_POINT(x25_interface_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chains for X.25 interface encapsulation commands
 */
#define	ALTERNATE	NONE
#include "cfg_int_encaps_x25.h"
LINK_POINT(x25_encaps_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chain registration array for X.25 protocol engine
 */
static parser_extension_request x25_chain_init_table[] = {
    { PARSE_ADD_CFG_INT_CMD, &pname(x25_interface_commands) },
    { PARSE_ADD_INT_ENCAPS_CMD, &pname(x25_encaps_commands) },
    { PARSE_ADD_INT_ENCAPS_EXIT,
	  (dynamic_transition *) &pname(cfg_encap_x25_exit) },
    { PARSE_LIST_END, NULL }
};


/*
 * x25_pe_parser_init
 * Install the X.25 protocol engine parser support
 */
void x25_pe_parser_init (void)
{
    /*
     *  add X.25 chains to the parser
     */
    parser_add_command_list(x25_chain_init_table, "x25");

    /*
     *  add the X.25 map/pad chain extension and exit points to the parser
     */
    parser_add_link_point(PARSE_ADD_CFG_INT_X25_MAPPVC_CMD,
			  "X.25 map/pad extend",
			  &pname(x25_mappvc_extend_here));
    parser_add_link_exit(PARSE_ADD_CFG_INT_X25_MAPPVC_EXIT,
			 "X.25 map/pad exit",
			 &pname(x25_mappvc_return_here));
}
