/* $Id: exec_debug_tcp.h,v 3.1.2.1 1996/04/15 21:23:16 bcole Exp $
 * $Source: /release/112/cvs/Xsys/tcp/exec_debug_tcp.h,v $
 *------------------------------------------------------------------
 * TCP debugging commands
 *
 * January 1996, Bruce Cole
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: exec_debug_tcp.h,v $
 * Revision 3.1.2.1  1996/04/15  21:23:16  bcole
 * CSCdi54649:  IP Host Modularity, IP Automatic Stub Routing feature
 * commit
 * Branch: California_branch
 *
 * Revision 3.1  1996/04/15  01:40:28  bcole
 * Placeholders for 11.2 features.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

LINK_TRANS (debug_ip_tcp_extend, no_alt);

/******************************************************************
 * debug ip tcp packet [<line> | port <num> | addr <ipaddr> | in | out]
 *
 * OBJ(int,1) = <line>
 * OBJ(int,2) = <num>
 * OBJ(paddr,1) = <ipaddr>
 * OBJ(int,3) = TCPPKT_DBG_INOUT (default), TCPPKT_DBG_IN, or TCPPKT_DBG_OUT
 *
 */
pdecl(debug_ip_tcp_packet_node);

EOLS	(debug_ip_tcp_packet_eol, tcp_debug_command, DEBUG_TCPPKT);
KEYWORD_ID(debug_ip_tcp_packet_out, debug_ip_tcp_packet_node, no_alt,
	   OBJ(int,3), TCPPKT_DBG_OUT,
	   "out", "Outgoing segments", PRIV_OPR|PRIV_HIDDEN);
KEYWORD_ID(debug_ip_tcp_packet_in, debug_ip_tcp_packet_node,
	   debug_ip_tcp_packet_out,
	   OBJ(int,3), TCPPKT_DBG_IN,
	   "in", "Incoming segments", PRIV_OPR|PRIV_HIDDEN);
SET	(debug_ip_tcp_packet_inout, debug_ip_tcp_packet_in,
	 OBJ(int,3), TCPPKT_DBG_INOUT);
IPADDR	(debug_ip_tcp_packet_addr_val, debug_ip_tcp_packet_node, no_alt,
	 OBJ(paddr,1), "IP address");
KEYWORD	(debug_ip_tcp_packet_addr, debug_ip_tcp_packet_addr_val,
	 debug_ip_tcp_packet_inout,
	 "address", "IP address (source or destination)", PRIV_OPR|PRIV_HIDDEN);
NUMBER	(debug_ip_tcp_packet_port_val, debug_ip_tcp_packet_node, no_alt,
	 OBJ(int,2), 1, (1 << 16) - 1, "Port number");
KEYWORD	(debug_ip_tcp_packet_port, debug_ip_tcp_packet_port_val,
	 debug_ip_tcp_packet_addr,
	 "port", "Port number (source or destination)", PRIV_OPR|PRIV_HIDDEN);
TTYLINE	(debug_ip_tcp_packet_line, debug_ip_tcp_packet_node,
	 debug_ip_tcp_packet_port,
	 OBJ(int,1), "Line number");
NOP	(debug_ip_tcp_packet_node, debug_ip_tcp_packet_line,
	 debug_ip_tcp_packet_eol);
KEYWORD (debug_ip_tcp_packet, debug_ip_tcp_packet_node, debug_ip_tcp_extend,
	 "packet", "TCP packets", PRIV_OPR);

/******************************************************************
 * debug ip tcp header-compression
 *
 */
EOLS	(debug_ip_tcp_hcomp_eol, tcp_debug_command, DEBUG_THC);
KEYWORD (debug_ip_tcp_hcomp, debug_ip_tcp_hcomp_eol, debug_ip_tcp_packet,
	 "header-compression", "Header compression statistics", PRIV_OPR);

/******************************************************************
 * debug ip tcp transactions
 *
 */
EOLS	(debug_ip_tcp_trans_eol, tcp_debug_command, DEBUG_TCP);
KEYWORD (debug_ip_tcp_trans, debug_ip_tcp_trans_eol, debug_ip_tcp_hcomp,
	 "transactions", "Significant TCP events", PRIV_OPR);

/******************************************************************
 * debug ip tcp driver-pak
 *
 */
EOLS	(debug_ip_tcp_driverp_eol, tcp_debug_command, DEBUG_TCPDP);
KEYWORD_MM (debug_ip_tcp_driverp, debug_ip_tcp_driverp_eol, debug_ip_tcp_trans,
	 "driver-pak", "TCP driver verbose", PRIV_OPR, 7);

/******************************************************************
 * debug ip tcp driver
 *
 */
EOLS	(debug_ip_tcp_driver_eol, tcp_debug_command, DEBUG_TCPD);
KEYWORD (debug_ip_tcp_driver, debug_ip_tcp_driver_eol, debug_ip_tcp_driverp,
	 "driver", "TCP driver events", PRIV_OPR);

/******************************************************************
 *
 */
KEYWORD (debug_ip_tcp, debug_ip_tcp_driver, ALTERNATE,
	 "tcp", "TCP information", PRIV_OPR);

#undef	ALTERNATE
#define	ALTERNATE	debug_ip_tcp
