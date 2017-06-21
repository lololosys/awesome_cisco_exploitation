/* $Id: cfg_frame_ip.h,v 3.2 1995/11/17 09:32:10 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/ip/cfg_frame_ip.h,v $
 *------------------------------------------------------------------
 * cfg_frame_ip.h -- IP options for frame relay.
 *
 * July 1994 Tony Li
 *
 * Copyright (c) 1994-1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cfg_frame_ip.h,v $
 * Revision 3.2  1995/11/17  09:32:10  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:53:57  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/08/22  02:59:59  ccpoe
 * CSCdi38917:  parser does not accept frame-relay de-list command with ip
 * list
 *
 * Revision 2.2  1995/07/13  21:38:58  ahh
 * CSCdi36962:  UDP parsing of tacacs-ds port name broken
 * Restore "tacacs-ds" as a keyword, but hide it and map it to TACACS_PORT
 * for backwards compatibility.
 *
 * Revision 2.1  1995/07/01  12:55:33  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize ip, ipmulticast, tcp
 *
 * Revision 2.2  1995/06/21  18:16:29  widmer
 * CSCdi36227:  Fix parse chain alternates
 *
 * Revision 2.1  1995/06/07  22:03:06  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

EOLS	(cfg_fr_ip_eol, reg_invoke_media_frame_relay_parse, FR_LISTNUM);

/******************************************************************************
 * frame-relay de-list <list> protocol ip [<args>]
 *
 * Valid arguments are:
 *
 *	fragments
 *	list <ip-access-list>
 *	tcp <port>
 *	udp <port>
 *
 * OBJ(int,5) = FR_PRIOR_LIST, FR_PR_TCP, FR_PR_UDP, FR_PR_FRAGS
 * OBJ(int,6) = <ip-access-list>, <port>
 *
 * First we check if we're parsing the 'de-list <list> protocol' 
 * command.  Then we check for IP, for the 'list'
 * argument, and for IP for the 'tcp' and 'udp' argument.  
 *
 */

/******************************************************************************
 * frame-relay de-list <list> protocol ip udp <port>
 *
 */
/* Note "tacacs-ds" -> 49 is wrong, but needed for backwards compatibility */
KEYWORD_ID_MM (cfg_fr_ip_tacacs, cfg_fr_ip_eol, no_alt,
	       OBJ(int, 6), TACACS_PORT,
	       "tacacs-ds", NULL, PRIV_CONF|PRIV_NOHELP, 7);
UDP_PORT (cfg_fr_ip_udpport, cfg_fr_ip_eol, cfg_fr_ip_tacacs,
	  OBJ(int,6));
KEYWORD_ID (cfg_fr_ip_udp, cfg_fr_ip_udpport, NONE,
	    OBJ(int,5), FR_PR_UDP,
	    "udp", "Classify UDP packets 'to' or 'from' the specified port",
	    PRIV_CONF);

/* tcp <port> */
TCP_PORT (cfg_fr_ip_tcpport, cfg_fr_ip_eol, cfg_fr_ip_tacacs,
	 OBJ(int,6));
KEYWORD_ID (cfg_fr_ip_tcpkwd, cfg_fr_ip_tcpport, cfg_fr_ip_udp,
	    OBJ(int,5), FR_PR_TCP,
	    "tcp", "Classify TCP packets 'to' or 'from' the specified port",
	    PRIV_CONF);
KEYWORD_ID (cfg_fr_ip_fragments, cfg_fr_ip_eol, cfg_fr_ip_tcpkwd,
	    OBJ(int,5), FR_PR_FRAGS,
	    "fragments", "Classify fragmented IP packets", PRIV_CONF);

NUMBER	(cfg_fr_ip_iplistno, cfg_fr_ip_eol, no_alt,
	 OBJ(int,6), MINFASTACC, MAXSLOWACC, "IP access list");

KEYWORD_ID (cfg_fr_ip_listkwd, cfg_fr_ip_iplistno, cfg_fr_ip_fragments,
	    OBJ(int,5), FR_PR_LIST,
	    "list", "To specify an access list", PRIV_CONF);

TESTVAR	(cfg_fr_ip, cfg_fr_ip_listkwd, NONE, NONE, NONE, ALTERNATE,
	 OBJ(int,3), LINK_IP);

#undef	ALTERNATE
#define	ALTERNATE	cfg_fr_ip
