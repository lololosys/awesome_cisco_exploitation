/* $Id: atm_arp_chain.c,v 3.1.4.3 1996/05/21 09:42:58 thille Exp $
 * $Source: /release/112/cvs/Xsys/atm/atm_arp_chain.c,v $
 *------------------------------------------------------------------
 * atm_arp_chain.c: Parser specific routines
 *
 * March, 1996, Rob Zagst
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: atm_arp_chain.c,v $
 * Revision 3.1.4.3  1996/05/21  09:42:58  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.1.4.2  1996/05/14  06:28:49  achopra
 * CSCdi47186:  Implement clear atm arp command.
 * Branch: California_branch
 *
 * Revision 3.1.4.1  1996/04/25  23:01:52  ronnie
 * CSCdi55766:  Hey!  This ATM and Frame Relay is no different than in 11.1
 * Feature commit for ATM and Frame Relay into 11.2
 * Branch: California_branch
 *
 * Revision 3.1  1996/03/30  05:53:42  rzagst
 * Place holder for atm modularity files.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include "config.h"
#include "parser.h"
#include "../h/interface_private.h"
#include "../h/interface.h"
#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "../parser/parser_defs_parser.h"
#include "../atm/parser_defs_atm.h"
#include "../if/atm.h"
#include "../if/atm_debug.h"
#include "../atm/atm_arp.h"



#define ALTERNATE       NONE
#include "exec_debug_atm_arp.h"
LINK_POINT(atm_arp_debug_commands, ALTERNATE);
#undef ALTERNATE

#define ALTERNATE NONE
#include "exec_show_atm_arp.h"
LINK_POINT(atm_arp_show_commands,ALTERNATE );
#undef ALTERNATE

#define ALTERNATE NONE
#include "exec_clear_atm_arp.h"
LINK_POINT(atm_clear_extend_here,ALTERNATE);
#undef ALTERNATE

#define ALTERNATE NONE
#include "cfg_int_atm_arp.h"
LINK_POINT(cfg_int_atm_arp,ALTERNATE );
#undef ALTERNATE

LINK_EXIT(ci_atm_pvc_inarp_return_here,no_alt);
NUMBER	(ci_atm_pvc_inarpfreq, ci_atm_pvc_inarp_return_here,
	 ci_atm_pvc_inarp_return_here,
	 OBJ(int,11),1, 60, "Inverse ARP frequency(minutes)");

KEYWORD_ID(ci_atm_pvc_inarp, ci_atm_pvc_inarpfreq,
	   NONE, 
	OBJ(int,11), ATM_VC_INARP, "inarp",
	"Inverse ARP enable", PRIV_CONF|PRIV_SUBIF);

LINK_POINT(ci_atm_pvc_inarp_extend_here,ci_atm_pvc_inarp);
/*
 * Parse chain registration array for ATM commands
 */
static parser_extension_request atm_arp_chain_init_table[] ={
    { PARSE_ADD_DEBUG_INT_ATM_CMD, &pname(atm_arp_debug_commands) },
    { PARSE_ADD_SHOW_INT_ATM_CMD,&pname(atm_arp_show_commands) },
    { PARSE_ADD_CLEAR_INT_ATM_CMD,&pname(atm_clear_extend_here) },
    { PARSE_ADD_CFG_INT_ATM_CMD,&pname(cfg_int_atm_arp) },
    { PARSE_ADD_CFG_INT_PVC_INARP_CMD,&pname(ci_atm_pvc_inarp_extend_here) },
    { PARSE_ADD_CFG_INT_PVC_INARP_EXIT,
      (dynamic_transition *) &pname(ci_atm_pvc_inarp_return_here) },
    { PARSE_LIST_END, NULL }
};


/*
 * atm_arp_parser_init - Initialize parse structures for parsing ATM ARP
 * commands
 */
void
atm_arp_parser_init(void)
{
    parser_add_command_list(atm_arp_chain_init_table, "atm arp");
}
