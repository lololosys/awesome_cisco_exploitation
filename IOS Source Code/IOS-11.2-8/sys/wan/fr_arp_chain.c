/* $Id: fr_arp_chain.c,v 3.1.32.2 1996/05/21 10:09:32 thille Exp $
 * $Source: /release/112/cvs/Xsys/wan/fr_arp_chain.c,v $
 *------------------------------------------------------------------
 * Frame Relay parse chains
 *
 * Feburary 1996, Charles Poe
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: fr_arp_chain.c,v $
 * Revision 3.1.32.2  1996/05/21  10:09:32  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.1.32.1  1996/04/25  23:23:14  ronnie
 * CSCdi55766:  Hey!  This ATM and Frame Relay is no different than in 11.1
 * Feature commit for ATM and Frame Relay into 11.2
 * Branch: California_branch
 *
 * Revision 3.1.38.1  1996/04/24  04:11:16  shankar
 * Branch: ATM_Cal_branch
 * Patch FR_SVC_branch changes
 *
 * Revision 3.1.2.2  1996/03/05  06:37:38  shankar
 * Branch: FR_SVC_branch
 * Sync between FR_SVC_baseline_020396 and FR_SVC_bseline_030396
 *
 * Revision 3.1.2.1  1996/02/19  08:16:50  ccpoe
 * Frame Relay ARP subsystem
 * Branch: FR_SVC_branch
 *
 * Revision 3.1  1996/02/10  00:42:27  ccpoe
 * Frame Relay ARP subsystem
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include "config.h"
#include "../ui/debug.h"
#include "parser.h"
#include "../h/interface_private.h"
#include "../wan/frame_relay.h"
#include "../wan/fr_arp.h"
#include "../wan/fr_debug.h"
#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "../parser/parser_defs_parser.h"
#include "../parser/parser_defs_exec.h"
#include "../wan/parser_defs_frame.h"





/*
 * Parse chains for Frame Relay clear commands
 */
#define	ALTERNATE	NONE
#include "exec_clear_fr-arp.h"
LINK_POINT(fr_inarp_clear_commands, ALTERNATE);
#undef	ALTERNATE



/*
 * Parse chains for Frame Relay ARP commands
 */
#define	ALTERNATE	NONE
#include "cfg_int_fr-arp.h"
LINK_POINT(fr_arp_int_commands, ALTERNATE);
#undef	ALTERNATE





/*
 * Parse chain registration array for Frame Relay commands
 */
static parser_extension_request fr_arp_chain_init_table[] = {
    { PARSE_ADD_CLEAR_CMD, &pname(fr_inarp_clear_commands) },
    { PARSE_ADD_CFG_INT_INARP_CMD, &pname(fr_arp_int_commands) },
    { PARSE_LIST_END, NULL }
};


/*
 * fr_arp_parser_init - Initialize parse structures for
 *                           parsing frame relay commands
 */
void fr_arp_parser_init (void)
{
    parser_add_command_list(fr_arp_chain_init_table, "frame-relay-arp");
}














