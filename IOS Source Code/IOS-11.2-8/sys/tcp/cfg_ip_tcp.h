/* $Id: cfg_ip_tcp.h,v 3.1.8.2 1996/04/30 20:55:42 jenny Exp $
 * $Source: /release/112/cvs/Xsys/tcp/cfg_ip_tcp.h,v $
 *------------------------------------------------------------------
 * TCP related config commands
 *
 * January 1996, Bruce Cole 
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cfg_ip_tcp.h,v $
 * Revision 3.1.8.2  1996/04/30  20:55:42  jenny
 * CSCdi52475:  TFTP: cannot specify source address
 * Branch: California_branch
 * Create interface for ftp, rcp, and telnet ource interface commands
 * if they are not there when the commands are parsed.
 *
 * Revision 3.1.8.1  1996/04/15  21:23:14  bcole
 * CSCdi54649:  IP Host Modularity, IP Automatic Stub Routing feature
 * commit
 * Branch: California_branch
 *
 * Revision 3.1  1996/03/13  00:58:37  jenny
 * CSCdi51402:  TCP Path-MTU Discovery needs to be dynamic
 * Implemented TCP PMTU dynamic discovery and PMTU timers (RFC 1191).
 * Moved ip tcp parser command into a seperated parser chain in TCP for
 * modularity.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************************
 * ip telnet source-interface <interface>
 *
 */
EOLS    (cip_telnet_eol, tcp_command, IP_TELNET_SOURCE);
FORWARD_INTERFACE(cip_telnet_source_name, cip_telnet_eol, NONE,
		  OBJ(idb,1), IFTYPE_ANYSUB);
NOPREFIX (cip_telnet_source_noprefix, cip_telnet_source_name,
	  cip_telnet_eol);
NVGENS	(cip_telnet_source_nvgen, cip_telnet_source_noprefix,
	 tcp_command, IP_TELNET_SOURCE);
KEYWORD	(cip_telnet_source, cip_telnet_source_nvgen, NONE,
         "source-interface", "Specify source interface", PRIV_CONF);
KEYWORD	(cip_telnet, cip_telnet_source, ALTERNATE,
         "telnet", "Specify telnet options", PRIV_CONF);

/******************************************************************************
 * ip tcp { chunk-size <size> |
 *          window-size <size> |
 *	    finwait-time <time> |
 *	    queuemax <size> |
 *	    synwait-time <time> |
 *	    path-mtu-discovery [<age-timer> [<time> | <infinite>]] |
 *          async-mobility server }
 *
 * no ip tcp { chunk-size [<size>] |
 *             window-size [<size>] |
 *	       finwait-time [<time>] |
 *	       queuemax [<size>] |
 *	       synwait-time [<time>] |
 *	       path-mtu-discovery [<age-timer> [<time> | <infinite>]] |
 *	       async-mobility server }
 */

EOLS    (cip_tcp_tunnel_eol, tcp_command, IP_TCP_TUNNEL);
KEYWORD (cip_tcp_tunnel_server, cip_tcp_tunnel_eol, no_alt,
	 "server", "Enable async-mobility server",
	 PRIV_CONF);
KEYWORD (cip_tcp_tunnel, cip_tcp_tunnel_server, no_alt,
	 "async-mobility", "Configure async-mobility",
	 PRIV_CONF);

EOLS    (cip_tcp_pathmtu_eol, tcp_command, IP_TCP_PATHMTU);
NUMBER 	(cip_tcp_pathmtu_settimer, cip_tcp_pathmtu_eol, no_alt,
	 OBJ(int,1), MIN_PATHMTU_AGER, MAX_PATHMTU_AGER, 
	 "Aging time");
KEYWORD_ID (cip_tcp_pathmtu_notimer, cip_tcp_pathmtu_eol, 
	    cip_tcp_pathmtu_settimer, 
	    OBJ(int,1), NO_PATHMTU_AGER,
	    "infinite", 
	    "Disable pathmtu aging timer",
	    PRIV_CONF);
KEYWORD	(cip_tcp_pathmtu_agetimer, cip_tcp_pathmtu_notimer, 
	 cip_tcp_pathmtu_eol,
	 "age-timer", 
	 "Set PMTU aging timer",
	 PRIV_CONF);
NOPREFIX (cip_tcp_pathmtu_noprefix, cip_tcp_pathmtu_agetimer,
	  cip_tcp_pathmtu_eol);
NVGENS	(cip_tcp_pathmtu_nvgen, cip_tcp_pathmtu_noprefix,
	 tcp_command, IP_TCP_PATHMTU);
KEYWORD_ID (cip_tcp_pathmtu, cip_tcp_pathmtu_nvgen, cip_tcp_tunnel,
	    OBJ(int,1), MIN_PATHMTU_AGER,
	    "path-mtu-discovery",
	    "Enable path-MTU discovery on new TCP connections",
	    PRIV_CONF);

PARAMS_KEYONLY(cip_tcp_synwait, cip_tcp_pathmtu, "synwait-time",
	       OBJ(int,1), 5, 300,
	       tcp_command, IP_TCP_SYNWAIT,
	       "Set time to wait on new TCP connections",
	       "Wait time", PRIV_CONF );

PARAMS_KEYONLY(cip_tcp_queuemax, cip_tcp_synwait, 
	       "queuemax", OBJ(int,1), 5, 50,
	       tcp_command, IP_TCP_QUEUEMAX,
	       "Maximum queue of outgoing TCP packets", "Number of packets",
	       PRIV_CONF );

PARAMS_KEYONLY(cip_tcp_finwwait, cip_tcp_queuemax, "finwait-time",
	       OBJ(int,1), 0, -1,
	       tcp_command, IP_TCP_FINWAITTM,
	       "Wait for a FIN for closed connections", "Seconds", PRIV_CONF );

PARAMS_KEYONLY(cip_tcp_window, cip_tcp_finwwait, "window-size",
	       OBJ(int,1), 0, 65535,
	       tcp_command, IP_TCP_WINDSZ,
	       "TCP window size", "Window size", PRIV_CONF );

PARAMS_KEYONLY(cip_tcp_chunk, cip_tcp_window, "chunk-size",
	       OBJ(int,1), 0, -1,
	       tcp_command, IP_TCP_CHUNKSZ,
	       "TCP chunk size", "Chunk size", PRIV_CONF );

KEYWORD (cip_tcp, cip_tcp_chunk, cip_telnet,
	 "tcp", "Global TCP parameters", PRIV_CONF);

#undef	ALTERNATE
#define	ALTERNATE	cip_tcp
