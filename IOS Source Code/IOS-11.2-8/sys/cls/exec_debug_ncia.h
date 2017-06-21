/* $Id: exec_debug_ncia.h,v 3.1.2.3 1996/07/03 06:18:33 jolee Exp $
 * $Source: /release/112/cvs/Xsys/cls/exec_debug_ncia.h,v $
 *------------------------------------------------------------------
 * exec_debug_ncia.h - Definitions for NCIA debug command
 *
 * March 1996, Steve Chiang
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: exec_debug_ncia.h,v $
 * Revision 3.1.2.3  1996/07/03  06:18:33  jolee
 * CSCdi61974:  Fix NCIA crash if unconfig a client in active open
 * processing
 * Branch: California_branch
 *
 * Revision 3.1.2.2  1996/06/12  06:51:38  schiang
 * CSCdi60121:  ncia keepalive cant be turned through configuration
 * clean up code so that no line exceeds 80 characters.
 * Branch: California_branch
 *
 * Revision 3.1.2.1  1996/05/17  10:43:12  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.1.4.2  1996/05/03  22:06:14  schiang
 * CSCdi56629:  ncia server code needs to clean up
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.1.4.1  1996/04/05  06:05:37  schiang
 * Branch: IbuMod_Calif_branch
 * merge ncia server code
 *
 * Revision 3.1  1996/03/29  07:11:40  schiang
 * CSCdi53012:  add placeholder for NCIA files
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/******************************************************************
 * debug ncia {circuits | client | circuit} (no options implies all)
 *
 * OBJ(int,1) = command type NCIA_DEBUG_{CIRCUIT,CLIENT,SERVER,ALL}  
 *
 * If command is "debug ncia clients"
 * ---------------------------------
 * options:
 * { msgs | state | event | error | all }
 * { <ipaddress> | <cr> for all}
 *
 * OBJ(int,2) = { msgs | event | error | all }
 * OBJ(paddr,1) = <ipaddress>
 * (no options implies debug all clients)
 *
 * If command type is "debug ncia circuit" 
 * --------------------------------------
 * options :
 *
 * { event | state | error | flow | all }
 *
 * OBJ(int,2) = { msgs | state | error | flow | all }
 *
 * command type is "debug ncia server"
 *
 * OBJ(int,2) = <server num>
 *
 * If command type is "debug ncia all"
 * --------------------------------------
 * -no additional parameters required
 *
 */

EOLS	(debug_ncia_eol, debug_ncia, DEBUG_NCIA);

KEYWORD_ID(debug_ncia_server, debug_ncia_eol, debug_ncia_eol,
           OBJ(int,1), NCIA_DEBUG_SERVER, "server",
           "Debug NCIA server events", PRIV_USER);


IPADDR  (debug_client_ip_addr, debug_ncia_eol, debug_ncia_eol,
         OBJ(paddr,1), "Remote client IP address");

KEYWORD_ID(debug_ncia_client_event, debug_client_ip_addr, debug_client_ip_addr,
           OBJ(int, 2), NCIA_DEBUG_CLIENT_EVENT, "event",
           "Debug NCIA client events", PRIV_USER);

KEYWORD_ID(debug_ncia_client_msgs, debug_client_ip_addr, debug_ncia_client_event,
           OBJ(int, 2), NCIA_DEBUG_CLIENT_MSGS, "message",
           "Debug NCIA client messages", PRIV_USER);

KEYWORD_ID(debug_ncia_client_error, debug_client_ip_addr, 
           debug_ncia_client_msgs,
           OBJ(int, 2), NCIA_DEBUG_CLIENT_ERROR, "error",
           "Debug NCIA client errors", PRIV_USER);


KEYWORD_ID(debug_ncia_client, debug_ncia_client_error, debug_ncia_server,
           OBJ(int,1), NCIA_DEBUG_CLIENT, "client",
           "Debug NCIA client", PRIV_USER);

SET     (debug_ncia_client_all, debug_ncia_client, OBJ(paddr, 1),
         NCIA_DEBUG_ALL_CLIENTS);
SET     (debug_ncia_client_def, debug_ncia_client_all, OBJ(int, 2),
         NCIA_DEBUG_CLIENT_ALL);
SET     (debug_ncia_clients, debug_ncia_client_def, OBJ(int,1),
         NCIA_DEBUG_ALL);



KEYWORD_ID(debug_ncia_circuit_flow, debug_ncia_eol, debug_ncia_eol,
	   OBJ(int, 2), NCIA_DEBUG_CIRCUIT_FLOW, "flow-control",
	   "Debug NCIA circuit flow-control", PRIV_USER);

KEYWORD_ID(debug_ncia_circuit_state, debug_ncia_eol, debug_ncia_circuit_flow,
	   OBJ(int, 2), NCIA_DEBUG_CIRCUIT_STATE, "state",
	   "Debug NCIA circuit fsm state transitions", PRIV_USER);

KEYWORD_ID(debug_ncia_circuit_event, debug_ncia_eol, debug_ncia_circuit_state,
	   OBJ(int, 2), NCIA_DEBUG_CIRCUIT_EVENT, "event",
	   "Debug NCIA circuit event", PRIV_USER);

KEYWORD_ID(debug_ncia_circuit_error, debug_ncia_eol, debug_ncia_circuit_event,
	   OBJ(int, 2), NCIA_DEBUG_CIRCUIT_ERROR, "error",
	   "Debug NCIA circuit errors", PRIV_USER);

KEYWORD_ID(debug_ncia_circuit, debug_ncia_circuit_error, debug_ncia_clients,
           OBJ(int,1), NCIA_DEBUG_CIRCUIT, "circuit", 
           "Debug NCIA circuit", PRIV_USER);

SET     (debug_ncia_circuit_def, debug_ncia_circuit, OBJ(int, 2),
         NCIA_DEBUG_CIRCUIT_ALL);

SET     (debug_ncia_def, debug_ncia_circuit_def, OBJ(int,1),
         NCIA_DEBUG_ALL);

/******************************************************************
 * debug ncia
 */
KEYWORD_DEBUG(debug_ncia, debug_ncia_def, NONE,
	      OBJ(pdb,1), ncia_debug_arr,
	      "ncia", "Native Client Interface Architecture (NCIA) events",
	      PRIV_USER);

TEST_BOOLEAN(debug_ncia_test, debug_ncia, NONE, ALTERNATE,
	     bridge_enable);

#undef	ALTERNATE
#define ALTERNATE       debug_ncia_test






