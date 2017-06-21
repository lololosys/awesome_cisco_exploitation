/* $Id: cfg_arp.h,v 3.2.60.1 1996/08/12 16:02:43 widmer Exp $
 * $Source: /release/112/cvs/Xsys/ip/cfg_arp.h,v $
 *------------------------------------------------------------------
 * C F G _ A R P . H
 *
 * ARP global configuration command.
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cfg_arp.h,v $
 * Revision 3.2.60.1  1996/08/12  16:02:43  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.2  1995/11/17  09:32:08  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:53:55  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/01  12:55:33  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize ip, ipmulticast, tcp
 *
 * Revision 2.1  1995/06/07  22:01:39  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*************************************************************************
 * arp <ipaddr> <hardware-addr>
 *			{ arpa | sap | smds | snap } [{ alias | <interface> }]
 * no arp <ipaddr> [<hardware-addr> 
 *			{ arpa | sap | smds | snap } [{ alias | <interface> }]
 *
 * OBJ(int,1) = ARP_ARPA, ARP_SNAP, ARP_SAP or ARP_SMDS
 * OBJ(int,2) = ADDR_ULTRA, ADDR_ETHER
 * OBJ(int,4) = TRUE if "alias" is specified.
 * OBJ(idb,1) = interface - may only be specified when bridging IP
 * (*OBJ(paddr,1)) = IP address
 * (*OBJ(hwaddr,1)) = 48-bit hardware address
 */

EOLNS	(conf_arp_eol, ip_arp_command);

/* [interface] */
INTERFACE(conf_arp_interface, conf_arp_eol, conf_arp_eol, OBJ(idb,1), 
	  IFTYPE_ANYSUB);
/***
 This test requires BRIDGING_IF(idb, LINK_IP) and may have to
 be done in ip_arp_command rather than here in the parser.

IFELSE(conf_arp_interface_test, conf_arp_interface, conf_arp_eol,
       BRIDGING(LINK_IP));
 ***/

/* [alias] */
KEYWORD_ID (conf_arp_alias, conf_arp_eol, conf_arp_interface,
	 OBJ(int,4), TRUE,
	"alias", "Respond to ARP requests for the IP address", PRIV_CONF);

/* { arpa | sap | smds | snap } */
KEYWORD_ID (conf_arp_snap, conf_arp_alias, no_alt,
	    OBJ(int,1), ARP_SNAP,
	    "snap", "ARP type SNAP (FDDI and TokenRing)", PRIV_CONF);
KEYWORD_ID (conf_arp_smds, conf_arp_alias, conf_arp_snap,
		OBJ(int,1), ARP_SMDS,
		"smds", "ARP type SMDS", PRIV_CONF);
KEYWORD_ID (conf_arp_iso1, conf_arp_alias, conf_arp_smds,
	    OBJ(int,1), ARP_SAP,
	    "iso1", "", PRIV_CONF|PRIV_HIDDEN);
KEYWORD_ID (conf_arp_sap, conf_arp_alias, conf_arp_iso1,
	    OBJ(int,1), ARP_SAP,
	    "sap", "ARP type SAP (HP's ARP type)", PRIV_CONF);
KEYWORD_ID (conf_arp_arpa, conf_arp_alias, conf_arp_sap,
	    OBJ(int,1), ARP_ARPA,
	    "arpa", "ARP type ARPA", PRIV_CONF);

/* <ipaddr> { <ether-addr> } */

SET	(conf_arp_hwaddr_set, conf_arp_arpa, OBJ(int,2), ADDR_ETHER);
ENADDR	(conf_arp_hwaddr, conf_arp_hwaddr_set, no_alt,
	OBJ(hwaddr,1), "48-bit hardware address of ARP entry");
NOPREFIX (conf_arp_noprefix, conf_arp_hwaddr, conf_arp_eol);
IPADDR	(conf_arp_ipaddr, conf_arp_noprefix, no_alt,
	OBJ(paddr,1), "IP address of ARP entry");

/*************************************************************************/
NVGENNS	(conf_arp_nvgen, conf_arp_ipaddr, ip_arp_command);
KEYWORD	(configure_arp, conf_arp_nvgen, ALTERNATE,
	 "arp", "Set a static ARP entry", PRIV_CONF);

#undef  ALTERNATE
#define ALTERNATE configure_arp
