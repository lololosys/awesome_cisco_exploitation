/* $Id$
 * $Source$
 *------------------------------------------------------------------
 * RSVP clear commands.
 *
 * March, 1997 Chandrashekhar Appanna
 *
 * Copyright (c) 1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log$
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * clear ip rsvp reservation
 *	{ <session ip> <sender ip> TCP/UDP/<ip protocol>
 *	<session dport> <sender sport> | '*' }
 *
 *	OBJ(paddr,1) = <session ip>
 *	OBJ(paddr,2) = <sender ip>
 *	OBJ(int,1) = <session dport>
 *	OBJ(int,2) = <sender protocol>
 *	OBJ(int,3) = <sender sport>
 *	OBJ(int,4) = TRUE if <session ip> is given; FALSE if '*'
 */

EOLNS(cl_iprr_eol, rsvp_clear_ip_resv_command);
KEYWORD(cl_iprr_all, cl_iprr_eol, no_alt,
	"*", "Clear all reservations", PRIV_OPR);
NUMBER(cl_iprr_sender_sport, cl_iprr_eol, no_alt,
       OBJ(int, 3), 0, 0xffff, "Source port");
NUMBER(cl_iprr_session_dport, cl_iprr_sender_sport, no_alt,
       OBJ(int, 1), 0, 0xffff, "Destination port");
KEYWORD_ID(cl_iprr_protocol_tcp, cl_iprr_all, no_alt,
	   OBJ(int, 2), TCP_PROT, "tcp", "TCP Transport", PRIV_OPR);
KEYWORD_ID(cl_iprr_protocol_udp, cl_iprr_session_dport, 
	   cl_iprr_protocol_tcp, OBJ(int, 2), UDP_PROT, "udp",
	   "UDP Transport", PRIV_OPR);
NUMBER(cl_iprr_sender_protocol, cl_iprr_session_dport, 
       cl_iprr_protocol_udp, OBJ(int, 2), 0, 255, "IP Protocol");
IPADDR_NAME(cl_iprr_sender_ip, cl_iprr_sender_protocol, no_alt,
	    OBJ(paddr, 2), "Source address");
SET(cl_iprr_sender_set, cl_iprr_sender_ip, OBJ(int,4), TRUE);
IPADDR_NAME(cl_iprr_session_ip, cl_iprr_sender_set, cl_iprr_all,
	    OBJ(paddr, 1), "Destination address");
KEYWORD(clear_ip_rsvp_resv, cl_iprr_session_ip, no_alt, "reservation",
	"Clear RSVP reservations", PRIV_OPR);

/******************************************************************
 * clear ip rsvp sender
 *	{ <session ip> <sender ip> UDP/TCP/<ip protocol>
 *	<session dport> <sender sport> | '*' }
 *
 *	OBJ(paddr,1) = <session ip>
 *	OBJ(paddr,2) = <sender ip>
 *	OBJ(int,1) = <session dport>
 *	OBJ(int,2) = <sender protocol>
 *	OBJ(int,3) = <sender sport>
 *	OBJ(int,4) = TRUE if <session ip> is given, FALSE if '*'
 */
EOLNS(cl_iprp_eol, rsvp_clear_ip_sender_command);
KEYWORD(cl_iprp_all, cl_iprp_eol, no_alt,
	"*", "Clear all reservations", PRIV_OPR);
NUMBER(cl_iprp_sender_sport, cl_iprp_eol, no_alt,
       OBJ(int, 3), 0, 0xffff, "Source port");
NUMBER(cl_iprp_session_dport, cl_iprp_sender_sport, no_alt,
       OBJ(int, 1), 0, 0xffff, "Destination port");
KEYWORD_ID(cl_iprp_protocol_tcp, cl_iprp_all, no_alt,
	   OBJ(int, 2), TCP_PROT, "tcp", "TCP Transport", PRIV_OPR);
KEYWORD_ID(cl_iprp_protocol_udp, cl_iprp_session_dport, 
	   cl_iprp_protocol_tcp, OBJ(int, 2), UDP_PROT, "udp",
	   "UDP Transport", PRIV_OPR);
NUMBER(cl_iprp_sender_protocol, cl_iprp_session_dport, 
       cl_iprp_protocol_udp, OBJ(int, 2), 0, 255, "IP Protocol");
IPADDR_NAME(cl_iprp_sender_ip, cl_iprp_sender_protocol, no_alt,
	    OBJ(paddr, 2), "Source address");
SET(cl_iprp_sender_set, cl_iprp_sender_ip, OBJ(int,4), TRUE);
IPADDR_NAME(cl_iprp_session_ip, cl_iprp_sender_set, cl_iprp_all,
	    OBJ(paddr, 1), "Destination address");
KEYWORD(clear_ip_rsvp_sender, cl_iprp_session_ip, clear_ip_rsvp_resv,
	"sender", "Clear RSVP path state information", PRIV_OPR);

KEYWORD(clear_ip_rsvp, clear_ip_rsvp_sender, ALTERNATE,
	"rsvp", "RSVP", PRIV_OPR);

#undef ALTERNATE
#define ALTERNATE clear_ip_rsvp
