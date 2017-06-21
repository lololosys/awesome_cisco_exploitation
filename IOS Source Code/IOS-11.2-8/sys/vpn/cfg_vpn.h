/* $Id: cfg_vpn.h,v 1.2.40.3 1996/08/07 22:51:55 tkolar Exp $
 * $Source: /release/112/cvs/Xsys/vpn/cfg_vpn.h,v $
 *------------------------------------------------------------------
 * cfg_vpn.h  ---  parse tree for VPN
 *
 * January 1996, Tim Kolar
 *
 * Copyright (c) 1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cfg_vpn.h,v $
 * Revision 1.2.40.3  1996/08/07  22:51:55  tkolar
 * CSCdi62172:  VPDN dual CHAP challenge from Home-Gateway
 * Branch: California_branch
 * Merge two disparate VPDN authentication schemes.
 *
 * Revision 1.2.40.2  1996/05/12  19:22:15  tkolar
 * CSCdi57472:  vpn is an overloaded acronym
 * Branch: California_branch
 * Change "vpn" to "vpdn", and make a few other user interface tweaks
 * while we're at it.
 *
 * Revision 1.2.40.1  1996/04/27  06:34:08  syiu
 * Branch: California_branch
 * Commit Dial1_branch
 *
 * Revision 1.2.2.2  1996/04/20  21:07:54  tkolar
 * Branch: Dial1_branch
 * Hide the X.25 and Frame-relay portions of L2F.
 *
 * Revision 1.2.2.1  1996/02/17  22:59:00  tkolar
 * Branch: Dial1_branch
 * Checkpoint a pingable VPN.
 *
 * Revision 1.2  1996/01/31  18:38:27  tkolar
 * Branch: Dial1_branch
 * Get all of the VPN code into the repository, with the right names
 * and in the right places.
 *
 * Revision 1.1  1996/01/29  18:09:05  tkolar
 * Populate the VPN directory.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*************************************************************
 *
 *  [no] vpdn outgoing []
 *
 */

EOLS (cfg_vpdn_outgoing_eol, l2f_command, L2F_OUTGOING);


IPADDR  (cfg_vpdn_fr_address, cfg_vpdn_outgoing_eol, no_alt, 
	OBJ(paddr,1), "DLCI");

KEYWORD (cfg_vpdn_address_fr, cfg_vpdn_fr_address, no_alt,
	 "frame-relay", "Use Frame Relay ",
	 PRIV_CONF | PRIV_HIDDEN);

IPADDR  (cfg_vpdn_x25_address, cfg_vpdn_outgoing_eol, no_alt, 
	OBJ(paddr,1), "x25 destination");

KEYWORD (cfg_vpdn_address_x25, cfg_vpdn_x25_address, cfg_vpdn_address_fr,
	 "x25", "Use X.25",
	 PRIV_CONF | PRIV_HIDDEN);

IPADDR  (cfg_vpdn_ip_address, cfg_vpdn_outgoing_eol, no_alt, 
	OBJ(paddr,1), "IP destination");

KEYWORD (cfg_vpdn_address_ip, cfg_vpdn_ip_address, cfg_vpdn_address_x25,
	 "ip", "Use IP",
	 PRIV_CONF);

STRING (cfg_vpdn_out_aname, cfg_vpdn_address_ip, no_alt,
	 OBJ(string, 2), "Name to use when authenticating to gateway");

STRING (cfg_vpdn_out_rname, cfg_vpdn_out_aname, no_alt,
	 OBJ(string, 1), "Name of GW domain");


/*************************************************************
 *
 *  [no] vpdn incoming []
 *
 */
EOLS (cfg_vpdn_incoming_eol, l2f_command, L2F_INCOMING);

IPADDR  (cfg_vpdn_in_srcipaddress, cfg_vpdn_incoming_eol, no_alt, 
	OBJ(paddr,2), "IP source");

KEYWORD (cfg_vpdn_in_ipsrc, cfg_vpdn_in_srcipaddress, cfg_vpdn_incoming_eol,
	 "source-ip", "Set Source IP address",
	 PRIV_CONF);

NUMBER  (cfg_vpdn_in_virtemp_num, cfg_vpdn_in_ipsrc, no_alt, 
		OBJ(int,1), 1, 10, "Virtual Template interface to use");

KEYWORD (cfg_vpdn_in_virtemp, cfg_vpdn_in_virtemp_num, no_alt,
	 "virtual-template", "Virtual template interface to use",
	 PRIV_CONF);

STRING (cfg_vpdn_in_aname, cfg_vpdn_in_virtemp, no_alt,
	 OBJ(string, 2), "Name to use when authenticating to NAS");

STRING (cfg_vpdn_in_rname, cfg_vpdn_in_aname, no_alt,
	 OBJ(string, 1), "Name NAS will use to identify itself");



/*************************************************************
 *
 *  [no] vpdn [incoming|outgoing] ...
 *
 */
EOLS (cfg_vpdn_ip_address_srcip_eol, l2f_command, L2F_SOURCE_IP_ADDRESS);

IPADDR  (cfg_vpdn_ip_address_srcip, cfg_vpdn_ip_address_srcip_eol, no_alt, 
	OBJ(paddr,1), "IP source");

KEYWORD (cfg_vpdn_ip_address_src, cfg_vpdn_ip_address_srcip, cfg_vpdn_outgoing_eol,
	 "source-ip", "Set Source IP address",
	 PRIV_CONF);

EOLS (cfg_vpdn_ims_eol, l2f_command, L2F_IGNORE_MID_SEQUENCE);

KEYWORD (cfg_vpdn_ims, cfg_vpdn_ims_eol, cfg_vpdn_ip_address_src,
	 "ignore-mid-sequence", "Ignore MID sequencing (backward compatible)",
	 PRIV_CONF);

EOLS (cfg_vpdn_flc_eol, l2f_command, L2F_FORCE_LOCAL_CHAP);

KEYWORD (cfg_vpdn_flc, cfg_vpdn_flc_eol, cfg_vpdn_ims,
	 "force-local-chap", "Force a CHAP challenge to be instigated locally",
	 PRIV_CONF);

KEYWORD (cfg_vpdn_outgoing, cfg_vpdn_out_rname, cfg_vpdn_flc,
	 "outgoing", "Configure an outgoing tunnel",
	 PRIV_CONF);

KEYWORD (cfg_vpdn_incoming, cfg_vpdn_in_rname, cfg_vpdn_outgoing,
	 "incoming", "Configure an incoming tunnel",
	 PRIV_CONF);

EOLS (cfg_vpdn_enable_eol, l2f_command, L2F_ENABLE);

KEYWORD (cfg_vpdn_enable, cfg_vpdn_enable_eol, cfg_vpdn_incoming,
	 "enable", "Enable VPDN",
	 PRIV_CONF);

NVGENS  (cfg_vpdn_nvgen, cfg_vpdn_enable, l2f_command,
	 0);


KEYWORD (cfg_vpdn, cfg_vpdn_nvgen, ALTERNATE,
	 "vpdn", "Virtual Private Dialup Network",
	 PRIV_CONF);

#undef  ALTERNATE
#define ALTERNATE	cfg_vpdn
