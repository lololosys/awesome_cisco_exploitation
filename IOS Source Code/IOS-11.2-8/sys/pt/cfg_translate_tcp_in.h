/* $Id: cfg_translate_tcp_in.h,v 3.2 1995/11/17 18:58:56 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/pt/cfg_translate_tcp_in.h,v $
 *------------------------------------------------------------------
 * TCP translate in command
 *
 * December 1993, Robert Widmer
 *
 * Copyright (c) 1993-1995 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: cfg_translate_tcp_in.h,v $
 * Revision 3.2  1995/11/17  18:58:56  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:00:42  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/04  01:56:27  tli
 * CSCdi36508: Rationalize parser file placement, part I
 * Rationalize cls dev dlsw dspu filesys irix lat les lex llc2 parser pgen
 * pt servers smf smrp snmp src-36-c1000 src-4k-c4000 src-68-c3000
 * src-68-c4000 src-c4000 src-rsp srt tcl ts ui
 *
 * Revision 2.1  1995/06/07  22:14:27  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

LINK_EXIT(trans_in_tcp_return, no_alt);

/****************************************************************
 *  tcp and options
 *
 * translate tcp <ipaddr> [ binary | port <num> | printer | stream ]
 *
 * tcp <ipaddr> = OBJ(paddr,1)
 * binary = OBJ(int,2)
 * port <num> = OBJ(int,3)
 * stream = OBJ(int,4)
 * printer = OBJ(int,5)
 */
pdecl(trans_in_tcp_opts);
KEYWORD_ID(trans_in_tcp_printer, trans_in_tcp_opts, NONE,
	   OBJ(int,5), TRUE,
	   "printer", "Enable non-interactive (implies global quiet)",
	   PRIV_CONF);
NUMBER	(trans_in_tcp_port_num, trans_in_tcp_opts, no_alt,
	 OBJ(int,3), 1, (2<<15)-1, "Port Number");
KEYWORD	(trans_in_tcp_port, trans_in_tcp_port_num, trans_in_tcp_printer,
	 "port", "Port Number", PRIV_CONF);
KEYWORD_ID(trans_in_tcp_stream, trans_in_tcp_opts, trans_in_tcp_port,
	   OBJ(int,4), TRUE,
	   "stream", "Enable stream processing", PRIV_CONF);
KEYWORD_ID(trans_in_tcp_binary, trans_in_tcp_opts, trans_in_tcp_stream,
	   OBJ(int,2), TRUE,
	   "binary", "Negotiate Telnet binary mode on the connection",
	   PRIV_CONF);
NOP	(trans_in_tcp_opts, trans_in_tcp_binary, trans_in_tcp_return);

/*************************************************************/
IPADDR_NAME(trans_in_tcp_addr, trans_in_tcp_opts, no_alt,
	    OBJ(paddr,1), "IP address");
KEYWORD_ID(trans_in_tcp, trans_in_tcp_addr, ALTERNATE,
	   OBJ(int,1), TRANS_PROT_TCP,
	   "tcp", "TCP/IP Telnet", PRIV_CONF);


SET	(trans_in_ipaddr_set, trans_in_tcp_opts, OBJ(int,1), TRANS_PROT_OLD);
IPADDR	(trans_in_ipaddr, trans_in_ipaddr_set, trans_in_tcp,
	 OBJ(paddr,1), "IP address");

#undef	ALTERNATE
#define	ALTERNATE	trans_in_ipaddr
