/* $Id: exec_debug_dlsw.h,v 3.3.26.2 1996/07/19 03:55:36 athippes Exp $
 * $Source: /release/112/cvs/Xsys/dlsw/exec_debug_dlsw.h,v $
 *------------------------------------------------------------------
 * E X E C _ D E B U G _ D L S W . H
 *
 * July 1994, Frank Bordonaro 
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: exec_debug_dlsw.h,v $
 * Revision 3.3.26.2  1996/07/19  03:55:36  athippes
 * CSCdi60758:  dlsw : duplicate circuit identifier
 * Branch: California_branch
 *
 * Revision 3.3.26.1  1996/05/21  09:45:41  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.3  1996/01/18  02:12:33  tli
 * CSCdi47063:  show ip cache cannot be restricted to subinterfaces
 * Change IFTYPE_ANY to IFTYPE_HWIDB
 *
 * Revision 3.2  1995/11/17  09:04:07  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:25:21  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  20:56:07  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/07/04  01:52:52  tli
 * CSCdi36508: Rationalize parser file placement, part I
 * Rationalize cls dev dlsw dspu filesys irix lat les lex llc2 parser pgen
 * pt servers smf smrp snmp src-36-c1000 src-4k-c4000 src-68-c3000
 * src-68-c4000 src-c4000 src-rsp srt tcl ts ui
 *
 * Revision 2.2  1995/06/19  14:47:50  widmer
 * CSCdi36074:  Clean up debug parse chain alternates
 * Add keyword_debug macro to assign debug array
 *
 * Revision 2.1  1995/06/07  22:18:16  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/******************************************************************
 * debug dlsw {circuits | peers |
 *             reachability | local-circuits | all} 
 *
 * OBJ(int,1) = command type DLSW_DEBUG_{FSM,PEER,CSM,LOCAL,ALL}  
 *
 * If command type "debug dlsw reachability" 
 * -----------------------------------------
 * options:
 * {netbios | sna} {error | verbose}
 * (no options implies debug all reachability, events)
 *
 * OBJ(int,2) = debug reachability for netbios, sna, all
 * OBJ(int,3) = debug csm severity level error, verbose, event
 *
 * If command is "debug dlsw peers"
 * ---------------------------------
 * options:
 * {ip-address <ipaddress> | interface <name>}
 * (no options implies debug all peers)
 *
 * OBJ(int,5) = debug errors for fast switched packets 
 * OBJ(int,4) = debug fast switched packets 
 * OBJ(int,3) = debug fast switched FST sequence numbers packets 
 * OBJ(int,2) = debug peer by ip-address, interface, all peers
 * OBJ(paddr,1) = ip address of remote peer to debug 
 * OBJ(idb,1) = pointer to hwidbtype for direct peer interface  
 *
 * If command type is "debug dlsw circuit" 
 * --------------------------------------
 * options :
 *
 * { msgs | state | error | xid | flow | all }
 * { <Ckt ID> | <cr> for all}
 *
 * OBJ(int,2) = { msgs | state | error | xid | flow | all }
 * OBJ(int,3) = <Ckt ID>
 *
 * If command type is "debug dlsw local-circuit"
 * --------------------------------------
 * options:
 * { <Ckt ID> | <cr> for all}
 *
 * OBJ(int,3) = <Ckt ID>
 *
 * If command type is "debug dlsw all"
 * --------------------------------------
 * -no additional parameters required
 *
 */

EOLS	(debug_dlsw_eol, debug_dlsw, DEBUG_DLSW);

NUMBER(debug_local_major, debug_dlsw_eol, debug_dlsw_eol,
       OBJ(int,3), 0, 0xFFFFFFFF,
       "Circuit ID for a specific local circuit");

SET     (debug_local_init1, debug_local_major, OBJ(int,3), -1);

KEYWORD_ID(debug_dlsw_local, debug_local_init1, debug_dlsw_eol,
           OBJ(int,1), DLSW_DEBUG_LOCAL, "local-circuit",
           "Debug DLSw local circuit events", PRIV_OPR);

pdecl(csm_opts);

KEYWORD_ID(debug_csm_verbose, csm_opts, no_alt,
           OBJ(int,3), DLSW_DEBUG_CSM_VERBOSE, "verbose",
           "Show reachability event detail", PRIV_OPR);

KEYWORD_ID(debug_csm_error, csm_opts, debug_csm_verbose,
           OBJ(int,3), DLSW_DEBUG_CSM_ERRORS, "error",
           "Show only reachability errors", PRIV_OPR);

KEYWORD_ID(debug_csm_netb, csm_opts, debug_csm_error,
           OBJ(int,2), DLSW_DEBUG_NETB_REACH, "netbios",
           "Show only reachability events for NetBIOS", PRIV_OPR);
 
KEYWORD_ID(debug_csm_sna, csm_opts, debug_csm_netb,
           OBJ(int,2), DLSW_DEBUG_SNA_REACH, "sna",
           "Show only reachability events for SNA", PRIV_OPR); 

NOP     (csm_opts, debug_csm_sna, debug_dlsw_eol);

SET (debug_csm_def2, csm_opts, OBJ(int,3), DLSW_DEBUG_CSM_EVENTS);
SET (debug_csm_def1, debug_csm_def2, OBJ(int,2), DLSW_DEBUG_ALL_REACH);

KEYWORD_ID(debug_dlsw_path, debug_csm_def1, debug_dlsw_local,
           OBJ(int,1), DLSW_DEBUG_CSM, "reachability", 
           "Debug DLSw reachability (explorer traffic)", 
           PRIV_OPR);

KEYWORD_ID(debug_peer_fast_err, debug_dlsw_eol, debug_dlsw_eol,
           OBJ(int,5), TRUE, "fast-errors",
           "Debug errors for fast switched packets", PRIV_OPR);

KEYWORD_ID(debug_peer_fast_pak, debug_peer_fast_err, debug_peer_fast_err,
           OBJ(int,4), TRUE, "fast-paks",
           "Debug fast switched packets", PRIV_OPR);

KEYWORD_ID(debug_peer_fast_seq, debug_peer_fast_pak, debug_peer_fast_pak,
           OBJ(int,3), TRUE, "fst-seq",
           "Debug FST sequence numbers on fast switched packets", PRIV_OPR);

INTERFACE(debug_peer_int_name, debug_peer_fast_pak, no_alt, 
          OBJ(idb,1), IFTYPE_HWIDB     );

KEYWORD_ID(debug_peer_int, debug_peer_int_name, debug_dlsw_eol,
           OBJ(int,2), DLSW_DEBUG_INT_PEER, common_str_interface,
           "Specify a remote peer to debug by a direct interface", 
           PRIV_OPR);

IPADDR  (debug_peer_ip_addr, debug_peer_fast_seq, no_alt,
         OBJ(paddr,1), "Remote peer's ip address");

KEYWORD_ID(debug_peer_ip, debug_peer_ip_addr, debug_peer_int,
           OBJ(int,2), DLSW_DEBUG_IP_PEER, "ip-address",
           "Specify a remote peer to debug by it's ip address", PRIV_OPR);

SET     (debug_peer_def3, debug_peer_ip, OBJ(int,5), FALSE);
SET     (debug_peer_def2, debug_peer_def3, OBJ(int,4), FALSE);
SET     (debug_peer_def1, debug_peer_def2, OBJ(int,3), FALSE);
SET     (debug_peer_def, debug_peer_def1, OBJ(int,2), DLSW_DEBUG_ALL_PEERS);

KEYWORD_ID(debug_dlsw_peers, debug_peer_def, debug_dlsw_path,
           OBJ(int,1), DLSW_DEBUG_PEER, "peers",
           "Debug DLSw peer events", PRIV_OPR);

NUMBER(debug_core_major, debug_dlsw_eol, debug_dlsw_eol,
       OBJ(int,3), 0, 0xFFFFFFFF,
       "Circuit ID for a specific remote circuit");
 
KEYWORD_ID(debug_dlsw_core_xid, debug_core_major, debug_core_major,
	   OBJ(int, 2), DEBUG_DLSW_CORE_XID, "xid",
	   "Debug DLSw core XID C/R bit tracking", PRIV_OPR);

KEYWORD_ID(debug_dlsw_core_flow, debug_core_major, debug_dlsw_core_xid, 
	   OBJ(int, 2), DEBUG_DLSW_CORE_FLOW, "flow-control",
	   "Debug DLSw core flow-control", PRIV_OPR);

KEYWORD_ID(debug_dlsw_core_state, debug_core_major, debug_dlsw_core_flow, 
	   OBJ(int, 2), DEBUG_DLSW_CORE_STATE, "state",
	   "Debug DLSw core fsm state transitions", PRIV_OPR);

KEYWORD_ID(debug_dlsw_core_msgs, debug_core_major, debug_dlsw_core_state,
	   OBJ(int, 2), DEBUG_DLSW_CORE_MSGS, "messages",
	   "Debug DLSw core messages", PRIV_OPR);

KEYWORD_ID(debug_dlsw_core, debug_dlsw_core_msgs, debug_dlsw_peers,
           OBJ(int,1), DEBUG_DLSW_CORE, "core", 
           "Debug DLSw core", PRIV_OPR);

SET     (debug_core_def2, debug_dlsw_core, OBJ(int,4), -1);
SET     (debug_core_def1, debug_core_def2, OBJ(int,3), -1);
SET     (debug_dlsw_core_def, debug_core_def1, OBJ(int, 2),
         DEBUG_DLSW_CORE_SUM);
SET     (debug_dlsw_def, debug_dlsw_core_def, OBJ(int,1),
         DLSW_DEBUG_ALL);

/******************************************************************
 * debug dlsw
 */
KEYWORD_DEBUG(debug_dlsw, debug_dlsw_def, NONE,
	      OBJ(pdb,1), dlsw_debug_arr,
	      "dlsw", "Data Link Switching (DLSw) events",
	      PRIV_OPR);

TEST_BOOLEAN(debug_dlsw_test, debug_dlsw, NONE, ALTERNATE,
	     bridge_enable);

#undef	ALTERNATE
#define	ALTERNATE	debug_dlsw_test

