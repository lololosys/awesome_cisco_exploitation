/* $Id: exec_debug_igrp2.h,v 3.2.60.1 1996/04/02 05:36:55 dkatz Exp $
 * $Source: /release/112/cvs/Xsys/iprouting/exec_debug_igrp2.h,v $
 *------------------------------------------------------------------
 * exec_debug_igrp2.h
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_debug_igrp2.h,v $
 * Revision 3.2.60.1  1996/04/02  05:36:55  dkatz
 * CSCdi36031:  Running out of memory running 2 or more EIGRP ASs
 * Branch: California_branch
 * EIGRP Jumbo Patch.
 *
 * Revision 3.2  1995/11/17  17:33:09  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:06:58  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/01  12:16:12  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize iprouting
 *
 * Revision 2.1  1995/06/07  22:18:36  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * debug eigrp packets [verbose | terse | update | request | query |
 *			reply | hello | ipxsap | probe | ack | retry]
 *
 * OBJ(int,1) = debug bits
 */
pdecl	(debug_igrp2_packets_options);
EOLS	(debug_igrp2_packets_eol, igrp2_debug_command, DEBUG_IGRP2_PKT);
EVAL	(debug_igrp2_packets_retry_set, debug_igrp2_packets_options,
 	 GETOBJ(int,1) |= IGRP2_DEBUG_RETRY);
KEYWORD (debug_igrp2_packets_retry, debug_igrp2_packets_retry_set,
 	 debug_igrp2_packets_eol, "retry",
 	 "EIGRP retransmissions", PRIV_OPR);
EVAL	(debug_igrp2_packets_ack_set, debug_igrp2_packets_options,
 	 GETOBJ(int,1) |= IGRP2_DEBUG_ACK);
KEYWORD (debug_igrp2_packets_ack, debug_igrp2_packets_ack_set,
 	 debug_igrp2_packets_retry, "ack",
 	 "EIGRP ack packets", PRIV_OPR);
EVAL	(debug_igrp2_packets_probe_set, debug_igrp2_packets_options,
 	 GETOBJ(int,1) |= IGRP2_DEBUG_PROBE);
KEYWORD (debug_igrp2_packets_probe, debug_igrp2_packets_probe_set,
 	 debug_igrp2_packets_ack, "probe",
 	 "EIGRP probe packets", PRIV_OPR);
EVAL	(debug_igrp2_packets_ipxsap_set, debug_igrp2_packets_options,
 	 GETOBJ(int,1) |= IGRP2_DEBUG_IPXSAP);
KEYWORD (debug_igrp2_packets_ipxsap, debug_igrp2_packets_ipxsap_set,
 	 debug_igrp2_packets_probe, "ipxsap",
 	 "EIGRP ipxsap packets", PRIV_OPR);
EVAL	(debug_igrp2_packets_hello_set, debug_igrp2_packets_options,
 	 GETOBJ(int,1) |= IGRP2_DEBUG_HELLO);
KEYWORD (debug_igrp2_packets_hello, debug_igrp2_packets_hello_set,
 	 debug_igrp2_packets_ipxsap, "hello",
 	 "EIGRP hello packets", PRIV_OPR);
EVAL	(debug_igrp2_packets_reply_set, debug_igrp2_packets_options,
 	 GETOBJ(int,1) |= IGRP2_DEBUG_REPLY);
KEYWORD (debug_igrp2_packets_reply, debug_igrp2_packets_reply_set,
 	 debug_igrp2_packets_hello, "reply",
 	 "EIGRP reply packets", PRIV_OPR);
EVAL	(debug_igrp2_packets_query_set, debug_igrp2_packets_options,
 	 GETOBJ(int,1) |= IGRP2_DEBUG_QUERY);
KEYWORD (debug_igrp2_packets_query, debug_igrp2_packets_query_set,
 	 debug_igrp2_packets_reply, "query",
 	 "EIGRP query packets", PRIV_OPR);
EVAL	(debug_igrp2_packets_request_set, debug_igrp2_packets_options,
 	 GETOBJ(int,1) |= IGRP2_DEBUG_REQUEST);
KEYWORD (debug_igrp2_packets_request, debug_igrp2_packets_request_set,
 	 debug_igrp2_packets_query, "request",
 	 "EIGRP request packets", PRIV_OPR);
EVAL	(debug_igrp2_packets_update_set, debug_igrp2_packets_options,
 	 GETOBJ(int,1) |= IGRP2_DEBUG_UPDATE);
KEYWORD (debug_igrp2_packets_update, debug_igrp2_packets_update_set,
 	 debug_igrp2_packets_request, "update",
 	 "EIGRP update packets", PRIV_OPR);
EVAL	(debug_igrp2_packets_terse_set, debug_igrp2_packets_options,
 	 GETOBJ(int,1) |= IGRP2_DEBUG_TERSE);
KEYWORD (debug_igrp2_packets_terse, debug_igrp2_packets_terse_set,
 	 debug_igrp2_packets_update, "terse",
 	 "Display all EIGRP packets except Hellos", PRIV_OPR);
EVAL	(debug_igrp2_packets_verbose_set, debug_igrp2_packets_options,
 	 GETOBJ(int,1) |= IGRP2_DEBUG_VERBOSE);
KEYWORD (debug_igrp2_packets_verbose, debug_igrp2_packets_verbose_set,
 	 debug_igrp2_packets_terse, "verbose",
 	 "Display all EIGRP packets", PRIV_OPR);
NOP	(debug_igrp2_packets_options, debug_igrp2_packets_verbose, NONE);
KEYWORD (debug_igrp2_packets, debug_igrp2_packets_verbose, no_alt,
 	 "packets", "EIGRP packets", PRIV_OPR);

/******************************************************************
 * debug eigrp transmit
 *
 * Flags are returned in OBJ(int,1)
 */
pdecl(debug_igrp2_xmit_options);
EOLS	(debug_igrp2_xmit_eol, igrp2_debug_command, DEBUG_IGRP2_XMIT);
EVAL	(debug_igrp2_xmit_detail_set, debug_igrp2_xmit_options,
 	 GETOBJ(int,1) |= XDBDETAIL);
KEYWORD (debug_igrp2_xmit_detail, debug_igrp2_xmit_detail_set,
 	 debug_igrp2_xmit_eol, "detail", "More detail", PRIV_OPR);
EVAL	(debug_igrp2_xmit_strange_set, debug_igrp2_xmit_options,
 	 GETOBJ(int,1) |= XDBSTRANGE);
KEYWORD (debug_igrp2_xmit_strange, debug_igrp2_xmit_strange_set,
 	 debug_igrp2_xmit_detail, "strange", "STRANGE events", PRIV_OPR);
EVAL	(debug_igrp2_xmit_build_set, debug_igrp2_xmit_options,
 	 GETOBJ(int,1) |= XDBBUILD);
KEYWORD (debug_igrp2_xmit_build, debug_igrp2_xmit_build_set,
 	 debug_igrp2_xmit_strange, "build", "BUILD events", PRIV_OPR);
EVAL	(debug_igrp2_xmit_link_set, debug_igrp2_xmit_options,
 	 GETOBJ(int,1) |= XDBLINK);
KEYWORD (debug_igrp2_xmit_link, debug_igrp2_xmit_link_set,
 	 debug_igrp2_xmit_build, "link", "LINK events", PRIV_OPR);
EVAL	(debug_igrp2_xmit_peerdown_set, debug_igrp2_xmit_options,
 	 GETOBJ(int,1) |= XDBPEERDOWN);
KEYWORD (debug_igrp2_xmit_peerdown, debug_igrp2_xmit_peerdown_set,
 	 debug_igrp2_xmit_link, "peerdown", "PEERDOWN events", PRIV_OPR);
EVAL	(debug_igrp2_xmit_startup_set, debug_igrp2_xmit_options,
 	 GETOBJ(int,1) |= XDBSTARTUP);
KEYWORD (debug_igrp2_xmit_startup, debug_igrp2_xmit_startup_set,
 	 debug_igrp2_xmit_peerdown, "startup", "STARTUP events", PRIV_OPR);
EVAL	(debug_igrp2_xmit_packetize_set, debug_igrp2_xmit_options,
 	 GETOBJ(int,1) |= XDBPACKETIZE);
KEYWORD (debug_igrp2_xmit_packetize, debug_igrp2_xmit_packetize_set,
 	 debug_igrp2_xmit_startup, "packetize", "PACKETIZE events", PRIV_OPR);
EVAL	(debug_igrp2_xmit_ack_set, debug_igrp2_xmit_options,
 	 GETOBJ(int,1) |= XDBACK);
KEYWORD (debug_igrp2_xmit_ack, debug_igrp2_xmit_ack_set,
 	 debug_igrp2_xmit_packetize, "ack", "ACK events", PRIV_OPR);
NOP	(debug_igrp2_xmit_options, debug_igrp2_xmit_ack, NONE);
KEYWORD (debug_igrp2_xmit, debug_igrp2_xmit_options, debug_igrp2_packets,
 	 "transmit", "EIGRP transmission events", PRIV_OPR);

/******************************************************************
 * debug eigrp neighbors
 *
 */
EOLS	(debug_igrp2_nbrs_eol, igrp2_debug_command, DEBUG_IGRP2_NBRS);
KEYWORD (debug_igrp2_nbrs, debug_igrp2_nbrs_eol, debug_igrp2_xmit,
 	 "neighbors", "EIGRP neighbors", PRIV_OPR);

/******************************************************************
 * debug eigrp fsm
 *
 */
EOLS	(debug_igrp2_fsm_eol, igrp2_debug_command, DEBUG_IGRP2_FSM);
KEYWORD (debug_igrp2_fsm, debug_igrp2_fsm_eol, debug_igrp2_nbrs,
	 "fsm", "EIGRP Dual Finite State Machine events/actions", PRIV_OPR);

KEYWORD (debug_igrp2, debug_igrp2_fsm, ALTERNATE, 
	 "eigrp", "EIGRP Protocol information", PRIV_OPR);

#undef	ALTERNATE
#define	ALTERNATE	debug_igrp2
