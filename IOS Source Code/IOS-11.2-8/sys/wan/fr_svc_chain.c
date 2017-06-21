/* $Id: fr_svc_chain.c,v 3.1.64.5 1996/07/03 01:57:19 chifei Exp $
 * $Source: /release/112/cvs/Xsys/wan/fr_svc_chain.c,v $
 *------------------------------------------------------------------
 * fr_svc_chain.c : Frame Relay SVC parse chains
 *
 * May 1995, Shankar Natarajan
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: fr_svc_chain.c,v $
 * Revision 3.1.64.5  1996/07/03  01:57:19  chifei
 * CSCdi58333:  Some frame relay SVC commands are available in
 * non-enterprise images
 * Branch: California_branch
 * Modularized "frame svc" and "frame lapf" commands to go with FR SVC
 * subsystem; fixed/clean-up LAPF command format; consolidated DL
 * primitives define's.
 *
 * Revision 3.1.64.4  1996/05/29  21:07:14  rzagst
 * CSCdi58657:  map-class command broken
 * Branch: California_branch
 *
 * Revision 3.1.64.3  1996/05/21  10:09:32  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.1.64.2  1996/05/15  05:54:42  fowler
 * Frame relay VC queuing and rate enforcement
 * Branch: California_branch
 *
 * Revision 3.1.64.1  1996/04/25  23:23:36  ronnie
 * CSCdi55766:  Hey!  This ATM and Frame Relay is no different than in 11.1
 * Feature commit for ATM and Frame Relay into 11.2
 * Branch: California_branch
 *
 * Revision 3.1.68.1  1996/04/24  04:11:27  shankar
 * Branch: ATM_Cal_branch
 * Patch FR_SVC_branch changes
 *
 * Revision 3.1  1995/11/09  13:49:07  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/09/01  19:43:44  shankar
 * Placeholder for arkansas feature
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
#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "packet.h"
#include "../wan/frame_relay.h"
#include "../wan/fr_debug.h"
#include "../parser/parser_defs_parser.h"
#include "../parser/parser_defs_exec.h"
#include "../wan/parser_defs_frame.h"
#include "../parser/parser_defs_snmp.h"
#include "../xns/novell.h"
#include "../vines/vines.h"
#include "../h/access.h"
#include "../ip/ip_actions.h"
#include "../ip/ip_macros.h"
#include "registry.h"
#include "../snmp/snmp_api.h"
#include "../snmp/snmp_registry.regh"

/*
 * Parse chain for Frame-relay map class commands
 */
#define ALTERNATE       NONE
#include "../wan/cfg_mapclass_fr-svc.h"
LINK_POINT(fr_mapclass_commands, ALTERNATE);
#undef ALTERNATE

/*
 * Parse chains for Frame Relay map-list command support
 */
#define	ALTERNATE	NONE
#include "../if/cfg_staticmap_fr.h"
LINK_POINT(fr_maplist_commands, ALTERNATE);
#undef	ALTERNATE

/*
 * Parse chains for "frame-relay lapf" commands
 */
#define ALTERNATE       NONE
#include "fr_l2.h"
#include "cfg_int_fr_l2.h"
LINK_POINT(fr_cfg_lapf_commands, ALTERNATE);
#undef  ALTERNATE

/*
 * Parse chains for "[no ]frame-relay svc" command
 */
#define	ALTERNATE	NONE
#include "cfg_int_fr_svc.h"
LINK_POINT(fr_cfg_svc_commands, ALTERNATE);
#undef	ALTERNATE

/*
 * Parse chain registration array for Frame Relay SVC commands
 */
static parser_extension_request frame_svc_chain_init_table[] = {
    { PARSE_ADD_CFG_INT_SVC_CMD, &pname(fr_cfg_svc_commands) },
    { PARSE_ADD_CFG_INT_LAPF_CMD, &pname(fr_cfg_lapf_commands) },
    { PARSE_ADD_CFG_STATIC_MAP, &pname(fr_maplist_commands) },
    { PARSE_ADD_CFG_MAPCLASS_SVC_CMD, &pname(fr_mapclass_commands) },
    { PARSE_LIST_END, NULL }
};

/*
 * fr_svc_parser_init - Initialize parse structures for
 *                           parsing frame relay SVC commands
 */
void fr_svc_parser_init (void)
{
    parser_add_command_list(frame_svc_chain_init_table, "frame-relay-svc");
}
      
