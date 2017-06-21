/* $Id: ipc_chain.h,v 3.4.20.1 1996/08/12 16:03:09 widmer Exp $
 * $Source: /release/112/cvs/Xsys/ipc/ipc_chain.h,v $
 *------------------------------------------------------------------
 * ipc_chain.h - Show, test and debug parser chains for IPC system
 *
 * March 1995, Jim Hayes
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ipc_chain.h,v $
 * Revision 3.4.20.1  1996/08/12  16:03:09  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.4  1996/02/01  23:33:46  widmer
 * CSCdi45325:  Change Keyword/Set to use Keyword_Id macro
 *
 * Revision 3.3  1995/11/28  03:08:49  widmer
 * CSCdi44514:  ipc commands break ambiguity detection
 *
 * Revision 3.2  1995/11/17  17:28:38  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:59:42  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:05:33  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * ------- SHOW COMMANDS ---------
 */

/******************************************************************
 * show ipc status
 * 
 */

EOLNS    (ipc_status_eol, ipc_show_status);

KEYWORD (ipc_status, ipc_status_eol, no_alt,
	 "status", "Show status of local IPC server",
	 PRIV_USER);

/******************************************************************
 * show ipc ports
 * 
 */

EOLNS    (ipc_ports_eol, ipc_show_ports);

KEYWORD (ipc_ports, ipc_ports_eol, ipc_status,
	 "ports", "Show local IPC ports",
	 PRIV_USER);

/******************************************************************
 * show ipc nodes
 * 
 */

EOLNS    (ipc_nodes_eol, ipc_show_seats);

KEYWORD (ipc_nodes, ipc_nodes_eol, ipc_ports,
	 "nodes", "Show participating nodes",
	 PRIV_USER);

/******************************************************************
 * show ipc queue
 * 
 */

EOLNS    (ipc_queue_eol, ipc_show_message_queue);

KEYWORD (ipc_queue, ipc_queue_eol, ipc_nodes,
	 "queue", "Show the IPC retransmission queue",
	 PRIV_USER);

/******************************************************************
 * show ipc
 * 
 */

KEYWORD_MM (show_ipc, ipc_queue, NONE,
	    "ipc", "Interprocess communications commands",
	    PRIV_USER, 3);

/*
 * ------- TEST COMMANDS ---------
 */

/******************************************************************
 * test ipc misc
 * 
 */

EOLNS    (ipc_misc_eol, ipc_test_misc);

KEYWORD (ipc_misc, ipc_misc_eol, no_alt,
	 "misc", "Misc. Software Tests Du Jour",
	 PRIV_ROOT);

/******************************************************************
 * test ipc rpc
 * 
 */

EOLNS    (ipc_rpc_eol, ipc_test_rpc);

KEYWORD_ID (ipc_rpc_forever, ipc_rpc_eol, ipc_rpc_eol,
	 OBJ(int,1), 1,
	 "forever", "Infinite loop",
	 PRIV_ROOT);

KEYWORD (ipc_rpc, ipc_rpc_forever, ipc_misc,
	 "rpc", "Remote procedure calls",
	 PRIV_ROOT);

/******************************************************************
 * test ipc callback
 * 
 */

EOLNS    (ipc_callback_eol, ipc_test_callback);

KEYWORD (ipc_callback, ipc_callback_eol, ipc_rpc,
	 "callback", "Message callback routines",
	 PRIV_ROOT);

/******************************************************************
 * test ipc queue
 * 
 */

EOLNS    (test_ipc_queue_eol, ipc_test_queue);

KEYWORD (test_ipc_queue, test_ipc_queue_eol, ipc_callback,
	 "queue", "Message queuing routines",
	 PRIV_ROOT);

/******************************************************************
 * test ipc slavelog
 * 
 */

EOLNS    (test_ipc_slavelog_eol, ipc_test_slavelog);

KEYWORD (test_ipc_slavelog, test_ipc_slavelog_eol, test_ipc_queue,
	 "slavelog", "Slave-corelog facility",
	 PRIV_ROOT);

/******************************************************************
 * test ipc cbus-tx <slot number>
 * 
 */

/*
EOLNS    (test_ipc_cbustx_eol, ipc_test_cbustx);

KEYWORD (test_ipc_cbustx, test_ipc_cbustx_eol, test_ipc_slavelog,
	 "cbus-tx", "Transmit IPC over CBUS",
	 PRIV_ROOT);
*/
PARAMS_KEYONLY  (test_ipc_cbustx, test_ipc_slavelog,
                 "cbus-tx", OBJ(int,1), 0, 12,
                 ipc_test_cbustx, 0,
                 "Transmit IPC over CBUS", "slot-number",
                 PRIV_ROOT);


/******************************************************************
 * test ipc slavecore
 * 
 */

EOLNS    (test_ipc_slavecore_eol, ipc_test_slavecore);

KEYWORD (test_ipc_slavecore, test_ipc_slavecore_eol, test_ipc_cbustx,
	 "slavecore", "Slave-coredump facility",
	 PRIV_ROOT);

/******************************************************************
 * test ipc api
 * 
 */

EOLNS    (ipc_api_eol, ipc_test_api);

KEYWORD (ipc_api, ipc_api_eol, test_ipc_slavecore,
	 "api", "Applications Programming Interface",
	 PRIV_ROOT);

/******************************************************************
 * test ipc
 * 
 */

KEYWORD_MM (test_ipc, ipc_api, NONE,
	    "ipc", "Interprocess communications test commands",
	    PRIV_ROOT, 3);

/*
 * ------- DEBUG COMMANDS ---------
 */

/******************************************************************
 * [no] debug ipc all
 * 
 */

EOLNS    (debug_ipc_all_eol, ipc_set_debug_all);

KEYWORD (debug_ipc_all, debug_ipc_all_eol, no_alt,
	 "all", "All IPC debugging messages",
	 PRIV_OPR);

/******************************************************************
 * [no] debug ipc blocking
 * 
 */

EOLS    (debug_ipc_blocking_eol, debug_command, DEBUG_IPC_BLOCKING);

KEYWORD (debug_ipc_blocking, debug_ipc_blocking_eol, debug_ipc_all,
	 "blocking", "IPC process blocking",
	 PRIV_OPR);

/******************************************************************
 * [no] debug ipc buffers
 * 
 */

EOLS    (debug_ipc_buffers_eol, debug_command, DEBUG_IPC_BUFFERS);

KEYWORD (debug_ipc_buffers, debug_ipc_buffers_eol, debug_ipc_blocking,
	 "buffers", "IPC buffer use",
	 PRIV_OPR);

/******************************************************************
 * [no] debug ipc ports
 * 
 */

EOLS    (debug_ipc_ports_eol, debug_command, DEBUG_IPC_PORTS);

KEYWORD (debug_ipc_ports, debug_ipc_ports_eol, debug_ipc_buffers,
	 "ports", "Creation and deletion of ports",
	 PRIV_OPR);

/******************************************************************
 * [no] debug ipc seats
 * 
 */

EOLS    (debug_ipc_seats_eol, debug_command, DEBUG_IPC_SEATS);

KEYWORD (debug_ipc_seats, debug_ipc_seats_eol, debug_ipc_ports,
	 "seats", "Creation and deletion of nodes",
	 PRIV_OPR);


/******************************************************************
 * [no] debug ipc events
 * 
 */

EOLS    (debug_ipc_events_eol, debug_command, DEBUG_IPC_EVENTS);

KEYWORD (debug_ipc_events, debug_ipc_events_eol, debug_ipc_seats,
	 "events", "Special events within IPC system",
	 PRIV_ROOT);

/******************************************************************
 * [no] debug ipc packets
 * 
 */

EOLS    (debug_ipc_packets_eol, debug_command, DEBUG_IPC_PACKETS);

KEYWORD (debug_ipc_packets, debug_ipc_packets_eol, debug_ipc_events,
	 "packets", "Packet data in and out of IPC system.",
	 PRIV_ROOT);


/******************************************************************
 * debug ipc
 * 
 */

SET	(debug_ipc_set, debug_ipc_packets, OBJ(pdb,1), ipc_debug_arr);
KEYWORD_MM (debug_ipc, debug_ipc_set, NONE,
	    "ipc", "Interprocess communications debugging",
	    PRIV_ROOT, 3);

LINK_POINT (show_ipc_commands, show_ipc);
LINK_POINT (test_ipc_commands, test_ipc);
LINK_POINT (debug_ipc_commands, debug_ipc);
