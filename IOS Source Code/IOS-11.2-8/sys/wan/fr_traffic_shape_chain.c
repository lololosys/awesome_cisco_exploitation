/* $Id: fr_traffic_shape_chain.c,v 3.1.2.3 1996/06/11 18:14:58 fowler Exp $
 * $Source: /release/112/cvs/Xsys/wan/fr_traffic_shape_chain.c,v $
 *------------------------------------------------------------------
 * Frame Relay Traffic Shaping Command Chain
 *
 * March 1996, Charles Poe
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: fr_traffic_shape_chain.c,v $
 * Revision 3.1.2.3  1996/06/11  18:14:58  fowler
 * CSCdi59356:  All frame relay subinterface tests are failing - could be
 *              a broadcast
 * Branch: California_branch
 *
 * Revision 3.1.2.2  1996/05/29  21:07:14  rzagst
 * CSCdi58657:  map-class command broken
 * Branch: California_branch
 *
 * Revision 3.1.2.1  1996/05/15  05:54:46  fowler
 * Frame relay VC queuing and rate enforcement
 * Branch: California_branch
 *
 * Revision 3.1  1996/04/26  05:48:11  ccpoe
 * placeholder
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
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
#include "registry.h"
#include "../snmp/snmp_api.h"
#include "../snmp/snmp_registry.regh"

/*
 * Parse chain for frame-relay vc-queue-enable command
 */
#define ALTERNATE       NONE
#include "cfg_int_fr-traffic-shape.h"
LINK_POINT(fr_vcq_enable_commands, ALTERNATE);
#undef ALTERNATE


/*
 * Parse chain for Frame-relay map class commands
 */
#define ALTERNATE       NONE
#include "cfg_mapclass_fr-traffic-shape.h"
LINK_POINT(fr_vcq_mapclass_commands, ALTERNATE);
#undef ALTERNATE



/*
 * Parse chains for Frame Relay class commands
 */
#define	ALTERNATE	NONE
#include "cfg_int_fr-class.h"
LINK_POINT(frame_class_commands, ALTERNATE);
#undef ALTERNATE


/*
 * Parse chain registration array for Frame Relay traffic shaping & VC queuing
 * commands
 */
static parser_extension_request fr_vcq_chain_init_table[] = {
    { PARSE_ADD_CFG_MAPCLASS_VCQ_CMD, &pname(fr_vcq_mapclass_commands) },
    { PARSE_ADD_CFG_INT_VCQ_CMD, &pname(fr_vcq_enable_commands) },
    { PARSE_ADD_CFG_FRCLASS_CMD, &pname(frame_class_commands) },
    { PARSE_LIST_END, NULL }
};


/*
 * fr_vcq_parser_init - Initialize parse structures for
 *                      parsing frame relay VC queueing commands
 */
void fr_vcq_parser_init (void)
{
    parser_add_command_list(fr_vcq_chain_init_table, "frame-relay-vcq");
}


      
