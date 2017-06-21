/* $Id: cfg_priority_ip.h,v 3.2 1995/11/17 09:32:24 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/ip/cfg_priority_ip.h,v $
 *------------------------------------------------------------------
 * cfg_priority_ip.h -- IP specific configuration options for priority
 * queueing 
 *
 * July 1994 Tony Li
 *
 * Copyright (c) 1994-1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cfg_priority_ip.h,v $
 * Revision 3.2  1995/11/17  09:32:24  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:54:15  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/07/13  21:38:59  ahh
 * CSCdi36962:  UDP parsing of tacacs-ds port name broken
 * Restore "tacacs-ds" as a keyword, but hide it and map it to TACACS_PORT
 * for backwards compatibility.
 *
 * Revision 2.1  1995/07/01  12:55:39  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize ip, ipmulticast, tcp
 *
 * Revision 2.2  1995/06/21  18:17:16  widmer
 * CSCdi36227:  Fix parse chain alternates
 *
 * Revision 2.1  1995/06/07  22:11:42  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

EOLS	(cfg_prio_ip_eol, prioritylist_command, FALSE);

/******************************************************************************
 * priority-list <list> protocol ip <priority> udp <port>
 *					       tcp <port>
 *					       list <listnum>
 *					       fragments
 *
 * OBJ(int,5) = PRIORITY_GT, PRIORITY_LT, PRIORITY_LIST,
 *		PRIORITY_TCP, PRIORITY_UDP, PRIORITY_FRAGS
 *
 * OBJ(int,6) = <access-list>,<port>
 */
/* Note "tacacs-ds" -> 49 is wrong, but needed for backwards compatibility */
KEYWORD_ID_MM (cfg_prio_ip_tacacs, cfg_prio_ip_eol, no_alt,
	       OBJ(int, 6), TACACS_PORT,
	       "tacacs-ds", NULL, PRIV_CONF|PRIV_NOHELP, 7);
UDP_PORT (cfg_prio_ip_udpport, cfg_prio_ip_eol, cfg_prio_ip_tacacs,
	  OBJ(int,6));
KEYWORD_ID (cfg_prio_ip_udp, cfg_prio_ip_udpport, NONE,
	    OBJ(int,5), PRIORITY_UDP,
	    "udp", "Prioritize UDP packets 'to' or 'from' the specified port",
	    PRIV_CONF);

/* tcp <port> */
TCP_PORT (cfg_prio_ip_tcpport, cfg_prio_ip_eol, cfg_prio_ip_tacacs,
	  OBJ(int,6));
KEYWORD_ID (cfg_prio_ip_tcpkwd, cfg_prio_ip_tcpport, cfg_prio_ip_udp,
	    OBJ(int,5), PRIORITY_TCP,
	    "tcp", "Prioritize TCP packets 'to' or 'from' the specified port",
	    PRIV_CONF);
KEYWORD_ID (cfg_prio_ip_fragments, cfg_prio_ip_eol, cfg_prio_ip_tcpkwd,
	    OBJ(int,5), PRIORITY_FRAGS,
	    "fragments", "Prioritize fragmented IP packets", PRIV_CONF);

NUMBER	(cfg_prio_ip_iplistno, cfg_prio_ip_eol, no_alt,
	 OBJ(int,6), MINFASTACC, MAXSLOWACC, "IP access list");

KEYWORD_ID (cfg_prio_ip_listkwd, cfg_prio_ip_iplistno, cfg_prio_ip_fragments,
	    OBJ(int,5), PRIORITY_LIST,
	    "list", "To specify an access list", PRIV_CONF);

TESTVAR	(cfg_priority_ip, cfg_prio_ip_listkwd, NONE, NONE, NONE, ALTERNATE,
	 OBJ(int,3), LINK_IP);

#undef	ALTERNATE
#define	ALTERNATE	cfg_priority_ip
