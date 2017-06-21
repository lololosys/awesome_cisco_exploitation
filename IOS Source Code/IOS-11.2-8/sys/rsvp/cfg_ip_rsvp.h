/* $Id: cfg_ip_rsvp.h,v 1.1.4.4 1996/08/20 10:24:20 fred Exp $
 * $Source: /release/112/cvs/Xsys/rsvp/cfg_ip_rsvp.h,v $
 *------------------------------------------------------------------
 * Resource Reservation Protocol
 *
 * April 1996, Fred Baker
 *
 * Copyright (c) 1996-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cfg_ip_rsvp.h,v $
 * Revision 1.1.4.4  1996/08/20  10:24:20  fred
 * CSCdi66538:  implement integrated services objects (RSVP)
 * Branch: California_branch
 *
 * Revision 1.1.4.3  1996/08/12  16:07:02  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 1.1.4.2  1996/05/13  16:38:57  fred
 * CSCdi57496:  ip rsvp reservation command parameter in wrong place
 * Branch: California_branch
 *
 * Revision 1.1.4.1  1996/04/17  13:57:14  fred
 * CSCdi54910:  commit rsvp, traffic shaping, and RED
 * Branch: California_branch
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/******************************************************************
 * [no] ip rsvp reservation
 *	<session ip> <sender ip> TCP/UDP/<ip protocol>
 *	<session dport> <sender sport>
 *	<next hop ip> <next hop interface>
 *	FF|SE|WF
 *	<average-kbps> <burst-size>
 *	<QoS Service> [<Controlled Delay Service Level>]
 *
 *	OBJ(paddr,1) = <session ip>
 *	OBJ(paddr,2) = <sender ip>
 *	OBJ(paddr,3) = <next hop ip>
 *	OBJ(idb,1) = <next hop interface>
 *	OBJ(int,1) = <session dport>
 *	OBJ(int,2) = <sender protocol>
 *	OBJ(int,3) = <sender sport>
 *	OBJ(int,4) = <style flags>
 *	OBJ(int,5) = <average kbps>
 *	OBJ(int,6) = <burst size kbytes>
 *	OBJ(int,7) = <QOS Service>
 *	OBJ(int,8) = <Service Level>
 */

EOLNS(cip_resv_eol, rsvp_ip_resv_command);

NUMBER(cirr_burst_size, cip_resv_eol, no_alt,
       OBJ(int, 6), 1, 0xffff, "Maximum Burst in Kilobytes");

NUMBER(cirr_average_kbps, cirr_burst_size, no_alt,
       OBJ(int, 5), 1, BANDWIDTH_SCALE, "Average Bit Rate to reserve (KBPS)");

NUMBER(cirr_service_level, cirr_average_kbps, no_alt,
       OBJ(int, 8), 1, 3, "Service Level; 1 = lowest delay");

SET(cirr_zero_service_level, cirr_average_kbps, OBJ (int, 8), 0);

KEYWORD_ID(cirr_rate_qos, cirr_zero_service_level, no_alt,
	   OBJ(int, 7), QOS_GUARANTEED, "rate",
	   "Guaranteed Bit Rate Service", PRIV_CONF);

KEYWORD_ID(cirr_load_qos, cirr_zero_service_level, cirr_rate_qos,
	   OBJ(int, 7), QOS_CONTROLLED_LOAD, "load",
	   "Controlled Load Service", PRIV_CONF);

KEYWORD_ID(cirr_sender_style_ff, cirr_load_qos, no_alt,
	   OBJ(int, 4), RSVP_FF_OPTION, "ff", "Single Reservation",
	   PRIV_CONF);

KEYWORD_ID(cirr_sender_style_se, cirr_load_qos, cirr_sender_style_ff,
	   OBJ(int, 4), RSVP_SE_OPTION, "se",
	   "Shared Reservation, Limited Scope", PRIV_CONF);

KEYWORD_ID(cirr_sender_style_wf, cirr_load_qos, cirr_sender_style_se,
	   OBJ(int, 4), RSVP_WF_OPTION, "wf",
	   "Shared Reservation, Unlimited Scope", PRIV_CONF);

INTERFACE(cirr_next_hop_interface, cirr_sender_style_wf, no_alt,
	  OBJ(idb, 1), IFTYPE_ANYSUB);
IPADDR_NAME(cirr_next_hop_ip, cirr_next_hop_interface, no_alt,
	    OBJ(paddr, 3), "Next Hop address");

NUMBER(cirr_sender_sport, cirr_next_hop_ip, no_alt,
       OBJ(int, 3), 0, 0xffff, "Source port");

NUMBER(cirr_session_dport, cirr_sender_sport, no_alt,
       OBJ(int, 1), 0, 0xffff, "Destination port");

KEYWORD_ID(cirr_protocol_tcp, cirr_session_dport, no_alt,
	   OBJ(int, 2), TCP_PROT, "tcp", "TCP Transport", PRIV_CONF);

KEYWORD_ID(cirr_protocol_udp, cirr_session_dport, cirr_protocol_tcp,
	   OBJ(int, 2), UDP_PROT, "udp", "UDP Transport", PRIV_CONF);

NUMBER(cirr_sender_protocol, cirr_session_dport, cirr_protocol_udp,
       OBJ(int, 2), 0, 255, "IP Protocol");

IPADDR_NAME(cirr_sender_ip, cirr_sender_protocol, no_alt,
	    OBJ(paddr, 2), "Source address");
IPADDR_NAME(cirr_session_ip, cirr_sender_ip, no_alt,
	    OBJ(paddr, 1), "Destination address");

NVGENNS(cip_resv_nvgen, cirr_session_ip, rsvp_ip_resv_command);
KEYWORD(cip_resv, cip_resv_nvgen, no_alt, "reservation",
	"Configure static RSVP Reservation Information", PRIV_CONF);

/******************************************************************
 * [no] ip rsvp sender
 *	<session ip> <sender ip> UDP/TCP/<ip protocol>
 *	<session dport> <sender sport>
 *	<next hop ip> <next hop interface>
 *	<average-kbps> <burst-size>
 *
 *	OBJ(paddr,1) = <session ip>
 *	OBJ(paddr,2) = <sender ip>
 *	OBJ(paddr,3) = <next hop ip>
 *	OBJ(idb,1) = <next hop interface>
 *	OBJ(int,1) = <session dport>
 *	OBJ(int,2) = <sender protocol>
 *	OBJ(int,3) = <sender sport>
 *	OBJ(int,6) = <burst size kbytes>
 *	OBJ(int,7) = <QOS Service>
 */

EOLNS(cip_path_eol, rsvp_ip_path_command);

NUMBER(cirp_burst_size, cip_path_eol, no_alt,
       OBJ(int, 6), 1, 0xffff, "Maximum Burst in Kilobytes");

NUMBER(cirp_average_kbps, cirp_burst_size, no_alt,
       OBJ(int, 5), 1, BANDWIDTH_SCALE, "Average Bit Rate to reserve (KBPS)");

INTERFACE(cirp_next_hop_interface, cirp_average_kbps, no_alt,
	  OBJ(idb, 1), IFTYPE_ANYSUB);
IPADDR_NAME(cirp_next_hop_ip, cirp_next_hop_interface, no_alt,
	    OBJ(paddr, 3), "Previous Hop address");

NUMBER(cirp_sender_sport, cirp_next_hop_ip, no_alt,
       OBJ(int, 3), 0, 0xffff, "Source port");

NUMBER(cirp_session_dport, cirp_sender_sport, no_alt,
       OBJ(int, 1), 0, 0xffff, "Destination port");

KEYWORD_ID(cirp_protocol_tcp, cirp_session_dport, no_alt,
	   OBJ(int, 2), TCP_PROT, "tcp", "TCP Transport", PRIV_CONF);

KEYWORD_ID(cirp_protocol_udp, cirp_session_dport, cirp_protocol_tcp,
	   OBJ(int, 2), UDP_PROT, "udp", "UDP Transport", PRIV_CONF);

NUMBER(cirp_sender_protocol, cirp_session_dport, cirp_protocol_udp,
       OBJ(int, 2), 0, 255, "IP Protocol");

IPADDR_NAME(cirp_sender_ip, cirp_sender_protocol, no_alt,
	    OBJ(paddr, 2), "Source address");
IPADDR_NAME(cirp_session_ip, cirp_sender_ip, no_alt,
	    OBJ(paddr, 1), "Destination address");

NVGENNS(cip_path_nvgen, cirp_session_ip, rsvp_ip_path_command);
KEYWORD(cip_path, cip_path_nvgen, cip_resv,
	"sender", "Configure static RSVP Path Information", PRIV_CONF);

KEYWORD(cip_rsvp, cip_path, ALTERNATE,
	"rsvp", "Configure static RSVP information", PRIV_CONF);

#undef ALTERNATE
#define ALTERNATE cip_rsvp

