/* $Id: x25_enc_chain.c,v 3.1.2.2 1996/05/21 10:10:43 thille Exp $
 * $Source: /release/112/cvs/Xsys/x25/x25_enc_chain.c,v $
 *------------------------------------------------------------------
 * X.25 encapsulation client parser chains
 *
 * April 1996, Gilbert Glick
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: x25_enc_chain.c,v $
 * Revision 3.1.2.2  1996/05/21  10:10:43  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.1.2.1  1996/04/11  23:47:42  gglick
 * CSCdi54514:  X.25 parser modularity files
 * Branch: California_branch
 *
 * Revision 3.1  1996/04/11  23:40:03  gglick
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


/*
 * Parse chain additions to X.25 map and PVC configuration commands
 */
#define	ALTERNATE	NONE
#include "cfg_int_x25_enc.h"
LINK_POINT(x25enc_cfg_int_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chain additions for X.25 Blacker Front End exec commands
 */
#define ALTERNATE       NONE
#include "exec_bfe.h"
LINK_POINT(bfe_exec_commands, ALTERNATE);
#undef  ALTERNATE


/*
 * Parse chain registration array for X25
 */
static parser_extension_request x25enc_chain_init_table[] = {
    { PARSE_ADD_CFG_INT_CMD, &pname(x25enc_cfg_int_commands) },
    { PARSE_ADD_EXEC_CMD, &pname(bfe_exec_commands) },
    { PARSE_ADD_CFG_INT_X25_MAPPVC_CMD,
	  (dynamic_transition *) &pname(x25enc_mappvc_enter) },
    { PARSE_ADD_CFG_INT_X25_MAPPVC_EXIT,
	  (dynamic_transition *) &pname(ci_x25enc_mappvc_exit) },
    { PARSE_LIST_END, NULL }
};


/*
 * x25enc_parser_init
 * Install X.25 encapsulation client parse chains
 */
void x25enc_parser_init (void)
{
    /*
     *  add the X.25 encapsulation client chains to the parser
     */
    parser_add_command_list(x25enc_chain_init_table, "x25_enc");
}
