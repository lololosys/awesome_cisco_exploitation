/* $Id: cfg_int_tunnel.h,v 3.2.60.1 1996/03/23 01:33:23 slin Exp $
 * $Source: /release/111/cvs/Xsys/ip/cfg_int_tunnel.h,v $
 *------------------------------------------------------------------
 * C F G _ I N T _ T U N N E L . H
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_int_tunnel.h,v $
 * Revision 3.2.60.1  1996/03/23  01:33:23  slin
 * Branch: California_branch
 *
 * AppleTalk makes its exodus from the idb.
 *
 * Revision 3.2  1995/11/17  09:32:16  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:54:03  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/01  12:55:36  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize ip, ipmulticast, tcp
 *
 * Revision 2.1  1995/06/07  22:07:50  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/***************************************************************
 * [no] tunnel checksum 
 */
EOLS	(ci_tunnel_checksum_eol, tunnel_cmd, TUNNEL_CMD_CHECKSUM);
KEYWORD	(ci_tunnel_checksum, ci_tunnel_checksum_eol, no_alt,
	 "checksum", "enable end to end checksumming of packets",
	 PRIV_CONF);

/***************************************************************
 * [no] tunnel carry-security
 */
EOLS	(ci_tunnel_carry_eol, tunnel_cmd, TUNNEL_CMD_CARRY_SEC);
KEYWORD	(ci_tunnel_carry, ci_tunnel_carry_eol, ci_tunnel_checksum,
	 "carry-security", "carry security options of client packet",
	 PRIV_CONF|PRIV_HIDDEN);

/***************************************************************
 * [no] tunnel sequence-datagrams
 */
EOLS	(ci_tunnel_sequencing_eol, tunnel_cmd, TUNNEL_CMD_SEQUENCING);
KEYWORD	(ci_tunnel_sequencing, ci_tunnel_sequencing_eol, ci_tunnel_carry,
	 "sequence-datagrams", "drop datagrams arriving out of order",
	 PRIV_CONF);

/***************************************************************
 * tunnel key <num>
 * no tunnel key
 */
PARAMS_KEYONLY(ci_tunnel_key, ci_tunnel_sequencing, "key",
	       OBJ(int,1), 0, -1, tunnel_cmd, TUNNEL_CMD_KEY,
	       "security or selector key", "key", PRIV_CONF);

/***************************************************************
 * tunnel mode { gre { ip | multi }  | nos | cayman | eon | aurp | ipip |
 *               iptalk }
 * no tunnel mode
 */
EOLS	(ci_tunnel_mode_eol, tunnel_cmd, TUNNEL_CMD_MODE);

KEYWORD_ID(ci_tunnel_mode_iptalk, ci_tunnel_mode_eol, no_alt,
	   OBJ(int, 1), TUN_MODE_IPTALK_UDP,
	   "iptalk", "Apple IPTalk encapsulation", PRIV_CONF);

KEYWORD_ID(ci_tunnel_mode_ipip, ci_tunnel_mode_eol, ci_tunnel_mode_iptalk,
	  OBJ(int,1), TUN_MODE_IP_IP,
	  "ipip", "IP over IP encapsulation", PRIV_CONF);

KEYWORD_ID(ci_tunnel_mode_dvmrp, ci_tunnel_mode_eol, ci_tunnel_mode_ipip,
	  OBJ(int,1), TUN_MODE_DVMRP_IP,
	  "dvmrp", "DVMRP multicast tunnel", PRIV_CONF);

KEYWORD_ID(ci_tunnel_mode_aurp, ci_tunnel_mode_eol, ci_tunnel_mode_dvmrp,
	  OBJ(int,1), TUN_MODE_AURP_UDP,
	  "aurp", "AURP TunnelTalk AppleTalk encapsulation", PRIV_CONF);

KEYWORD_ID(ci_tunnel_mode_eon, ci_tunnel_mode_eol, ci_tunnel_mode_aurp,
	  OBJ(int,1), TUN_MODE_EON_IP,
	  "eon", "EON compatible CLNS tunnel", PRIV_CONF);

KEYWORD_ID(ci_tunnel_mode_cayman, ci_tunnel_mode_eol, ci_tunnel_mode_eon,
	  OBJ(int,1), TUN_MODE_CAYMAN_UDP,
	  "cayman", "Cayman TunnelTalk AppleTalk encapsulation", PRIV_CONF);

KEYWORD_ID(ci_tunnel_mode_nos, ci_tunnel_mode_eol, ci_tunnel_mode_cayman,
	   OBJ(int,1), TUN_MODE_NOS_IP,
	   "nos", "IP over IP encapsulation (KA9Q/NOS compatible)", PRIV_CONF);

KEYWORD_ID(ci_tunnel_mode_mgreip, ci_tunnel_mode_eol, no_alt,
           OBJ(int,1), TUN_MODE_MGRE_IP,
           "multipoint", "over IP (multipoint)", PRIV_CONF);

KEYWORD_ID(ci_tunnel_mode_greip, ci_tunnel_mode_eol, ci_tunnel_mode_mgreip,
           OBJ(int,1), TUN_MODE_GRE_IP,
           "ip", "over IP", PRIV_CONF);
KEYWORD	(ci_tunnel_mode_gre, ci_tunnel_mode_greip, ci_tunnel_mode_nos,
	 "gre", "generic route encapsulation protocol", PRIV_CONF);

NOPREFIX(ci_tunnel_mode_noprefix, ci_tunnel_mode_gre, ci_tunnel_mode_eol );
KEYWORD	(ci_tunnel_mode, ci_tunnel_mode_noprefix, ci_tunnel_key,
	 "mode", "tunnel encapsulation method", PRIV_CONF);

/***************************************************************
 * [no] tunnel destination <ipaddr>
 */
EOLS	(ci_tunnel_dest_eol, tunnel_cmd, TUNNEL_CMD_DESTINATION);
IPADDR_NAME(ci_tunnel_dest_addr, ci_tunnel_dest_eol, no_alt,
	    OBJ(paddr,1), "ip address or host name");
NOPREFIX(ci_tunnel_dest_noprefix, ci_tunnel_dest_addr, ci_tunnel_dest_eol);
NVGENS	(ci_tunnel_dest_nvgen, ci_tunnel_dest_noprefix,
	 tunnel_cmd, TUNNEL_CMD_DESTINATION);
KEYWORD	(ci_tunnel_dest, ci_tunnel_dest_nvgen, ci_tunnel_mode,
	 "destination", "destination of tunnel", PRIV_CONF);

/***************************************************************
 * tunnel source { <interface> | <ipaddr> }
 * no tunnel source
 */
EOLS	(ci_tunnel_source_eol, tunnel_cmd, TUNNEL_CMD_SOURCE);
IPADDR	(ci_tunnel_source_ipaddr, ci_tunnel_source_eol, no_alt,
	 OBJ(paddr,1), "ip address");
FORWARD_INTERFACE(ci_tunnel_source_interface, ci_tunnel_source_eol,
	  ci_tunnel_source_ipaddr, OBJ(idb,1), IFTYPE_ANYSUB);
NOPREFIX(ci_tunnel_noprefix, ci_tunnel_source_interface, ci_tunnel_source_eol);
NVGENS	(ci_tunnel_source_nvgen, ci_tunnel_noprefix,
	 tunnel_cmd, TUNNEL_CMD_SOURCE);
KEYWORD	(ci_tunnel_source, ci_tunnel_source_nvgen, ci_tunnel_dest,
	 "source", "source of tunnel packets", PRIV_CONF);


KEYWORD	(ci_tunnel, ci_tunnel_source, NONE,
	 "tunnel", "protocol-over-protocol tunneling", PRIV_CONF);

TEST_IDBSTATUS(ci_tunnel_test, ci_tunnel, NONE, ALTERNATE, IDB_TUNNEL);

#undef	ALTERNATE
#define	ALTERNATE	ci_tunnel_test
