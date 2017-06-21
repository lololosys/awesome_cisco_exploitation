/* $Id: cfg_translate_tcp_out.h,v 3.4 1996/01/23 23:45:17 vandys Exp $
 * $Source: /vws/aam/cherf.111/ios/sys/pt/cfg_translate_tcp_out.h,v $
 *------------------------------------------------------------------
 * TCP translate out command
 *
 * December 1993, Robert Widmer
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: cfg_translate_tcp_out.h,v $
 * Revision 3.4  1996/01/23  23:45:17  vandys
 * CSCdi45547:  Desire rotary behavior on PT TCP destinations
 * Add rotor option to randomize destination for TCP host lists
 *
 * Revision 3.3  1995/11/20  23:22:01  ahh
 * CSCdi41919:  PT: cannot specify source interface for TCP connections
 * Add source-interface option to outgoing TCP translation and telnet.
 * Also convert some parser defines to enums.
 *
 * Revision 3.2  1995/11/17  18:58:58  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:00:43  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/10/13  21:48:02  eschaffe
 * CSCdi18102:  Require one translate statment to multiple hosts
 *         Add "host-name" Telnet/TCP outgoing option.
 *
 * Revision 2.1  1995/07/04  01:56:28  tli
 * CSCdi36508: Rationalize parser file placement, part I
 * Rationalize cls dev dlsw dspu filesys irix lat les lex llc2 parser pgen
 * pt servers smf smrp snmp src-36-c1000 src-4k-c4000 src-68-c3000
 * src-68-c4000 src-c4000 src-rsp srt tcl ts ui
 *
 * Revision 2.1  1995/06/07  22:14:30  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

LINK_EXIT(trans_out_tcp_return, no_alt);

/****************************************************************
 *  tcp and options
 *
 * translate tcp <ipaddr> [ binary | port <num> | stream | multibyte_IAC |
 *			    host-name | source-interface <interface> ]
 *
 * tcp <ipaddr> = OBJ(paddr,2)
 * binary = OBJ(int,7)
 * port <num> = OBJ(int,8)
 * stream = OBJ(int,9)
 * multibyte-IAC = OBJ(int,10)
 * host-name = OBJ(int,11)
 * interface = OBJ(idb,1)
 * rotor = OBJ(int,12)
 */
pdecl(trans_out_tcp_opts);

INTERFACE(trans_out_tcp_source_name, trans_out_tcp_opts, NONE,
	   OBJ(idb,1), IFTYPE_ANYSUB);
KEYWORD	(trans_out_tcp_source, trans_out_tcp_source_name, NONE,
         "source-interface", "Specify source interface", PRIV_CONF);
KEYWORD_ID(trans_out_tcp_rotor, trans_out_tcp_opts, trans_out_tcp_opts,
	OBJ(int,12), TRUE, "rotor",
	"Specify rotor among host-name addresses", PRIV_CONF);
KEYWORD_ID(trans_out_tcp_host_name, trans_out_tcp_rotor, trans_out_tcp_source,
	   OBJ(int,11), TRUE, "host-name", 
	   "Store the host name rather than its IP address", PRIV_CONF);
KEYWORD_ID(trans_out_tcp_iachack, trans_out_tcp_opts, 
	   trans_out_tcp_host_name, OBJ(int,10), TRUE, "multibyte-IAC", 
	   "Always treat multiple IACs as telnet command", PRIV_CONF);
KEYWORD_ID(trans_out_tcp_stream, trans_out_tcp_opts, trans_out_tcp_iachack,
	   OBJ(int,9), TRUE,
	   "stream", "Treat telnet escape characters as data", PRIV_CONF);
NUMBER	(trans_out_tcp_port_num, trans_out_tcp_opts, no_alt,
	 OBJ(int,8), 1, (2<<15)-1, "Port Number");
KEYWORD	(trans_out_tcp_port, trans_out_tcp_port_num, trans_out_tcp_stream,
	 "port", "Port Number", PRIV_CONF);
KEYWORD_ID(trans_out_tcp_binary, trans_out_tcp_opts, trans_out_tcp_port,
	   OBJ(int,7), TRUE,
	   "binary",
	   "Negotiate Telnet binary mode on the connection",
	   PRIV_CONF);
NOP	(trans_out_tcp_opts, trans_out_tcp_binary, trans_out_tcp_return);

/*************************************************************/
IPADDR_NAME(trans_out_tcp_addr, trans_out_tcp_opts, no_alt,
	    OBJ(paddr,2), "IP address");
KEYWORD_ID(trans_out_tcp, trans_out_tcp_addr, ALTERNATE,
	   OBJ(int,6), TRANS_PROT_TCP,
	   "tcp", "TCP/IP Telnet", PRIV_CONF);

#undef	ALTERNATE
#define	ALTERNATE	trans_out_tcp
