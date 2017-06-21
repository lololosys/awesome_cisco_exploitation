/* $Id: cfg_int_smds.h,v 3.2 1995/11/17 18:01:13 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/wan/cfg_int_smds.h,v $
 *------------------------------------------------------------------
 * C F G _ I N T _ S M D S . H
 *
 * Copyright (c) 1993-1995 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_int_smds.h,v $
 * Revision 3.2  1995/11/17  18:01:13  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:48:10  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/09/15  19:22:41  lmercili
 * CSCdi34816:  add dynamic mapping for ipx/smds
 *
 * Revision 2.1  1995/07/02  01:45:57  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize xns, atalk, vines, decnet, xremote, x25, unix, wan, tn3270,
 * clns, cdp
 *
 * Revision 2.2  1995/06/21  03:12:47  gstovall
 * CSCdi35731:  Cleanup unused cruft in idb
 * Nuke some unused vectors and variables from the hwidb.
 *
 * Revision 2.1  1995/06/07  22:07:16  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * [no] smds enable-arp
 */
EOLS	(ci_smds_arp_eol, smds_command, SMDS_ARP_ON);
KEYWORD (ci_smds_arp, ci_smds_arp_eol, no_alt,
	"enable-arp", "Enable ARP processing.", PRIV_CONF|PRIV_SUBIF);

/******************************************************************
 * smds multicast <protocol> <smds-group-address> <ipaddr> <ip-subnet-mask>
 * no smds multicast <protocol> <smds-group-address>
 *
 * OBJ(int,1) = <protocol> 
 * OBJ(string,1) = <smds-group-address>
 */
EOLS	(ci_smds_mcastip_eol, smds_command, SMDS_BA);

IPADDR(ci_smds_mcastipmask_addr, ci_smds_mcastip_eol, no_alt,
	OBJ(paddr,2), "IP address mask");

IPADDR(ci_smds_mcastipnet_addr, ci_smds_mcastipmask_addr, ci_smds_mcastip_eol,
	OBJ(paddr, 1), "IP address net");

SMDSADDR(ci_smds_mcastip_addr, ci_smds_mcastipnet_addr, no_alt,
	 OBJ(hwaddr,1), "SMDS address");

EOLS	(ci_smds_mcast_eol, smds_command, SMDS_BA);

SMDSADDR(ci_smds_mcast_addr, ci_smds_mcast_eol, no_alt,
	 OBJ(hwaddr,1), "SMDS address");

PROTONAME(ci_smds_mcast_ipproto, ci_smds_mcastip_addr, no_alt,
	  OBJ(int,10), PMATCH_ARP PMATCH_IP);

PROTONAME(ci_smds_mcast_proto, ci_smds_mcast_addr, ci_smds_mcast_ipproto,
	  OBJ(int,10),
	  PMATCH_AARP PMATCH_APOLLO PMATCH_APPLETALK PMATCH_BRIDGE PMATCH_CDP
	  PMATCH_CLNS PMATCH_CLNS_ALL_ES PMATCH_CLNS_ALL_IS PMATCH_DECNET
	  PMATCH_DECNET_NODE PMATCH_DECNET_ROUTER_L1 PMATCH_DECNET_ROUTER_L2
	  PMATCH_DECNET_PRIME_ROUTER PMATCH_NOVELL PMATCH_VINES PMATCH_XNS);

NVGENS	(ci_smds_mcast_nvgen, ci_smds_mcast_proto, 
	 smds_command, SMDS_BA);

KEYWORD (ci_smds_mcast, ci_smds_mcast_nvgen, ci_smds_arp,
	 "multicast", "Set an SMDS multicast (or broadcast) address",
	 PRIV_CONF|PRIV_SUBIF);

/******************************************************************
 * [no] smds static-map <protocol> <protocoladdr> <smds-address> [broadcast]
 *
 * OBJ(int,1) = TRUE if BROADCAST flag present, else FALSE
 */

EOLS	(ci_smds_smap_eol, smds_command, SMDS_MAP);

/* secondary - GETOBJ(int,1) is initialized to FALSE (0) */
KEYWORD_ID(ci_smds_smap_broadcast, ci_smds_smap_eol, ci_smds_smap_eol, 
	   OBJ(int,1), TRUE,
	   "broadcast", "Make this SMDS address a broadcast address",
	   PRIV_CONF|PRIV_SUBIF);

/* <smds-address> */
SMDSADDR(ci_smds_smap_smdsaddr, ci_smds_smap_broadcast, no_alt,
	 OBJ(hwaddr,1), "SMDS address");

/*
 * This is where the protocols that are loaded dynamically load
 * their parse chain in to parse this command.
 */
LINK_TRANS(ci_smds_smap_extend_here, no_alt);

/* <protocol> <protocoladdr> */
NSAPADDR(ci_smds_smap_clns_addr, ci_smds_smap_smdsaddr, no_alt,
         OBJ(paddr,1), "CLNS address to map");
SPLIT	(ci_smds_smap_proto_test, ci_smds_smap_clns_addr,ci_smds_smap_smdsaddr,
	 OBJ(int,10), LINK_CLNS);
PROTOADDR(ci_smds_smap_proto, ci_smds_smap_proto_test, no_alt,
	  OBJ(int,10), OBJ(paddr,1),
	  PMATCH_APPLETALK PMATCH_APOLLO PMATCH_CLNS PMATCH_DECNET
	  PMATCH_IP PMATCH_NOVELL PMATCH_PAD PMATCH_VINES PMATCH_XNS);

NVGENS	(ci_smds_smap_nvgen, ci_smds_smap_proto, 
	 smds_command, SMDS_MAP);
KEYWORD	(ci_smds_smap, ci_smds_smap_nvgen, ci_smds_mcast,
	 "static-map", "Map high level protocol address to SMDS address",
	 PRIV_CONF|PRIV_SUBIF);

/******************************************************************
 * smds nec-mode
 * no smds nec-mode
 */

EOLS	(ci_smds_nec_eol, smds_command, SMDS_NEC);
KEYWORD (ci_smds_nec, ci_smds_nec_eol, ci_smds_smap,
	 "nec-mode", "SMDS NEC mode", PRIV_CONF);

/******************************************************************
 * smds glean-mode <protocol> [timeout] [broadcast]
 * no smds glean-mode 
 */

EOLS	 (ci_smds_glean_eol, smds_command, SMDS_GLEAN);

KEYWORD_ID (ci_smds_glean_bcast, ci_smds_glean_eol, ci_smds_glean_eol, 
	   OBJ(int,2), TRUE,
	   "broadcast", "Make SMDS address in dynamic maps a broadcast address",
	   PRIV_CONF|PRIV_SUBIF);

NUMBER   (ci_smds_glean_timeout, ci_smds_glean_bcast, ci_smds_glean_bcast,
	  OBJ(int,1), 1,  0xffff, "TIMEOUT value (minutes)");

NOPREFIX (ci_smds_glean_noprefix, ci_smds_glean_timeout, ci_smds_glean_eol); 

PROTONAME (ci_smds_glean_proto, ci_smds_glean_noprefix, no_alt,
	  OBJ(int,3), PMATCH_NOVELL);

NVGENS  (ci_smds_glean_nvgen, ci_smds_glean_proto, smds_command, SMDS_GLEAN);

KEYWORD  (ci_smds_glean, ci_smds_glean_nvgen, ci_smds_nec,
	 "glean-mode", "SMDS GLEAN mode", PRIV_CONF|PRIV_SUBIF);

/******************************************************************
 * [no] smds dxi-mode
 */
EOLS    (ci_smds_dxi_eol, smds_command, SMDS_DXI);
KEYWORD (ci_smds_dxi, ci_smds_dxi_eol, ci_smds_glean,
	 "dxi-mode", "SMDS DXI3.2 mode", PRIV_CONF);

/******************************************************************
 * smds address <smds-address>
 * no smds address [<smds-address>]
 *
 * OBJ(string,1) = smds address
 *
 * The smds address should really be parsed here.  The format
 * is really bizzare as explained in smds_parse_addr():
 *   * Brain dead SMDS address are one byte of hex
 *   * followed be 5 bytes of bcd, followed
 *   * by 2 bytes of 0xff, bleah!
 *   *
 *   * This routine will need to change if SMDS ever uses
 *   * all 60 bits of thier adddress.
 *   *
 *
 */
EOLS	(ci_smds_addr_eol, smds_command, SMDS_HARDWARE);
SMDSADDR(ci_smds_addr_addr, ci_smds_addr_eol, no_alt,
	 OBJ(hwaddr,1), "An SMDS address");
NOPREFIX (ci_smds_addr_noprefix, ci_smds_addr_addr, ci_smds_addr_eol);
NVGENS	(ci_smds_addr_nvgen, ci_smds_addr_noprefix,
	smds_command, SMDS_HARDWARE);
KEYWORD	(ci_smds_address, ci_smds_addr_nvgen, ci_smds_dxi,
	"address", "Set an interface SMDS address", PRIV_CONF|PRIV_SUBIF);

/******************************************************************/
KEYWORD	(ci_smds, ci_smds_address, NONE,
	 "smds", "Modify SMDS parameters", PRIV_CONF|PRIV_SUBIF);

TEST_IDBSTATUS(ci_smds_int, ci_smds, NONE, ALTERNATE, IDB_SERIAL);

#undef	ALTERNATE
#define	ALTERNATE	ci_smds_int
