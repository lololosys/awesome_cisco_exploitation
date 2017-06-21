/* $Id: cfg_int_x25.h,v 3.3.20.3 1996/06/01 07:19:43 tylin Exp $
 * $Source: /release/112/cvs/Xsys/x25/cfg_int_x25.h,v $
 *------------------------------------------------------------------
 * C F G _ I N T _ X 2 5 . H
 *
 * X.25 Level 3 support
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_int_x25.h,v $
 * Revision 3.3.20.3  1996/06/01  07:19:43  tylin
 * CSCdi53715:  CDP is not working over Fr-Rel, X25 and SMDS interfaces
 *    Add new CUD (0xDA) for CDP
 * Branch: California_branch
 *
 * Revision 3.3.20.2  1996/05/21  10:10:31  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.3.20.1  1996/03/18  22:49:33  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.24.2  1996/03/13  02:13:03  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.2.24.1  1996/02/20  21:54:30  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.3  1996/02/01  23:35:03  widmer
 * CSCdi45325:  Change Keyword/Set to use Keyword_Id macro
 *
 * Revision 3.2  1995/11/17  18:05:42  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:51:58  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/08/08  19:10:08  gglick
 * CSCdi38404:  X.25 doesnt accept T1x timer configurations as a DCE
 *
 * Revision 2.1  1995/07/02  01:45:01  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize xns, atalk, vines, decnet, xremote, x25, unix, wan, tn3270,
 * clns, cdp
 *
 * Revision 2.1  1995/06/07  22:08:03  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/************************************************************************
 * ... {<protocol> <protocol-addr>} <X.121-addr> [options]
 *
 * Common parser structure for encapsulation commands; this tree is
 * reached from map configuration and from PVC configuration for the
 * syntax common to protocol encapsulation and PAD route/option
 * specification.
 *
 * The following element is set before entry into this tree:
 *  OBJ(int,1) VC number - zero for a map, non-zero for a PVC
 *
 * This tree sets the following elements when parsing protocol specifications:
 *  OBJ(paddr,1) X.121 address, and
 *
 *  OBJ(int,2):
 *    LINK_PAD - only allowed for OBJ(int,2), no other services allowed,
 *		 only X.121 address defined;
 *    LINK_BRIDGE - only allowed for OBJ(int,2), no other services allowed,
 *		    only X.121 address defined;
 *  -or-
 *  OBJ(int,N) and OBJ(paddr,N) (2 <= N <= 10) datagram protocol ID and address
 *  where a defined integer represents one of the service types:
 *    LINK_APOLLO
 *    LINK_APPLETALK
 *    LINK_CLNS - no address defined
 *    LINK_COMPRESSED_TCP - address must match an IP address
 *    LINK_DECNET
 *    LINK_IP - address must match a compressed TCP IP address
 *    LINK_NOVELL
 *    LINK_VINES
 *    LINK_XNS
 *
 * This tree sets the following elements when parsing protocol specifications:
 *  OBJ(int,11) = options specified, with corresponding value objects
 *    00000001 - broadcast
 *    00000002 - packetsize <in> OBJ(int,12) <out> OBJ(int,13)
 *    00000004 - windowsize <in> OBJ(int,14) <out> OBJ(int,15)
 *    00000008 - passive
 *    00000010 - method OBJ(int,16)
 *    00020000 - compress
 *	- Note that the above are allowed for a PVC, the below are not -
 *    00000020 - reverse
 *    00000040 - accept-reverse
 *    00000080 - cug OBJ(int,17)
 *    00000100 - nvc OBJ(int,18)
 *    00000200 - throughput <in> OBJ(int,19) <out> OBJ(int,20)
 *    00000400 - nuid <name> OBJ(string,1) <password> OBJ(string,2)
 *    00000800 - nudata <nuid> OBJ(string,1)
 *    00001000 - rpoa <rpoa-name> OBJ(string,3)
 *    00002000 - transit-delay OBJ(int,21)
 *    00004000 - no-incoming
 *    00008000 - no-outgoing
 *    00010000 - idle OBJ(int,22)
 */

EOLS	(x25_map_endline, x25_interface_commands, X25_MAP);

/* options */

pdecl(x25_map_options);

NUMBER	(x25_map_option18b, x25_map_options, NONE,
	 OBJ(int,22), 0, 255, "Idle time (minutes)");
KEYWORD_OR(x25_map_option18, x25_map_option18b, x25_map_endline,
	   OBJ(int,11), X25MAP_IDLE,
	   "idle", "Specify VC idle timer", PRIV_CONF | PRIV_SUBIF);

KEYWORD_OR(x25_map_option17, x25_map_options, x25_map_option18,
	   OBJ(int,11), X25MAP_NO_OUTGOING,
	   "no-outgoing", "Do not use map for outgoing Calls",
	   PRIV_CONF | PRIV_SUBIF);

KEYWORD_OR(x25_map_option16, x25_map_options, x25_map_option17,
	   OBJ(int,11), X25MAP_NO_INCOMING,
	   "no-incoming", "Do not use map for incoming Calls",
	   PRIV_CONF | PRIV_SUBIF);

NUMBER	(x25_map_option15b, x25_map_options, NONE,
	 OBJ(int,21), 0, 65534, "Transit delay");
KEYWORD_OR(x25_map_option15, x25_map_option15b, x25_map_option16,
	   OBJ(int,11), X25MAP_TDELAY,
	   "transit-delay", "Specify transit delay (msec)",
	   PRIV_CONF | PRIV_SUBIF);

STRING	(x25_map_option14b, x25_map_options, NONE,
	 OBJ(string,3), "RPOA Name");
KEYWORD_OR(x25_map_option14, x25_map_option14b, x25_map_option15,
	   OBJ(int,11), X25MAP_RPOA,
	   "rpoa", "Specify RPOA", PRIV_CONF | PRIV_SUBIF);

STRING  (x25_map_option13b, x25_map_options, NONE,
         OBJ(string,1), "User formatted network user ID");
KEYWORD_OR(x25_map_option13, x25_map_option13b, x25_map_option14,
	   OBJ(int,11), X25MAP_NUDATA,
	   "nudata", "Specify user formatted network user ID",
	   PRIV_CONF | PRIV_SUBIF);

STRING	(x25_map_option12c, x25_map_options, NONE,
	 OBJ(string,2), "Password");
STRING	(x25_map_option12b, x25_map_option12c, NONE,
	 OBJ(string,1), "Username");
KEYWORD_OR(x25_map_option12, x25_map_option12b, x25_map_option13,
	   OBJ(int,11), X25MAP_NUID,
	   "nuid", "Specify Cisco formatted network user ID",
	   PRIV_CONF | PRIV_SUBIF);

NUMBER	(x25_map_option11c, x25_map_options, NONE,
	 OBJ(int,20), 75, 64000, "Output baud rate");
NUMBER	(x25_map_option11b, x25_map_option11c, NONE,
	 OBJ(int,19), 75, 64000, "Input baud rate");
KEYWORD_OR(x25_map_option11, x25_map_option11b, x25_map_option12,
	   OBJ(int,11), X25MAP_THROUGHPUT,
	   "throughput", "Request bandwidth in X.25 network",
	   PRIV_CONF | PRIV_SUBIF);

NUMBER	(x25_map_option10d, x25_map_options, NONE,
	 OBJ(int,18), 1, 8, "Maximum VCs");
NUMBER	(x25_map_option10c, x25_map_options, NONE,
	 OBJ(int,18), 1, 1, "Maximum VCs (can not be > 1)");
IFELSE	(x25_map_option10b, x25_map_option10c, x25_map_option10d,
	 (GETOBJ(int,1) != 0) ||
	 ((GETOBJ(int,2) == LINK_COMPRESSED_TCP) && !ISOBJ(int,3)));
KEYWORD_OR(x25_map_option10, x25_map_option10b, x25_map_option11,
	   OBJ(int,11), X25MAP_NVC,
	   "nvc", "Set number of virtual circuits for this map",
	   PRIV_CONF | PRIV_SUBIF);

NUMBER	(x25_map_option9b, x25_map_options, NONE,
	 OBJ(int,17), 1, 99, "CUG number");
KEYWORD_OR(x25_map_option9, x25_map_option9b, x25_map_option10,
	   OBJ(int,11), X25MAP_CUG,
	   "cug", "Specify a Closed User Group number", PRIV_CONF | PRIV_SUBIF);

KEYWORD_OR(x25_map_option8, x25_map_options, x25_map_option9,
	   OBJ(int,11), X25MAP_ACCEPT_REVERSE,
	   "accept-reverse", "Accepting incoming reverse-charged calls",
	   PRIV_CONF | PRIV_SUBIF);

KEYWORD_OR(x25_map_option7, x25_map_options, x25_map_option8,
	   OBJ(int,11), X25MAP_REVERSE,
	   "reverse", "Use reverse charging on originated calls",
	   PRIV_CONF | PRIV_SUBIF);

TESTVAR	(x25_map_option_pvc_test, x25_map_option7, x25_map_endline,
	 NONE, NONE, NONE, OBJ(int,1), 0);

KEYWORD_OR(x25_map_option6, x25_map_options, x25_map_option_pvc_test,
	   OBJ(int,11), X25MAP_PBPCOMPRESS,
	   "compress", "Specify Packet By Packet Compression",
	   PRIV_CONF | PRIV_SUBIF);

KEYWORD_ID(x25_map_option5g, x25_map_options, no_alt,
	   OBJ(int,16), X25METHOD_MULTI,
	 "multi", "Specify IETF's RFC-1356 multiprotocol encapsulation",
	 PRIV_CONF | PRIV_SUBIF);
KEYWORD_ID(x25_map_option5f, x25_map_options, x25_map_option5g,
	   OBJ(int,16), X25METHOD_SNAP,
	   "snap", "Specify IETF's RFC-1356 SNAP encapsulation",
	   PRIV_CONF | PRIV_SUBIF);
KEYWORD_ID(x25_map_option5e, x25_map_options, x25_map_option5f,
	   OBJ(int,16), X25METHOD_IETF,
	   "ietf", "Specify standard IETF RFC-1356 encapsulation",
	   PRIV_CONF | PRIV_SUBIF);
KEYWORD_ID(x25_map_option5d, x25_map_options, x25_map_option5e,
	   OBJ(int,16), X25METHOD_CISCO,
	   "cisco", "Specify Cisco encapsulation", PRIV_CONF | PRIV_SUBIF);
TESTVAR	(x25_map_option5c, x25_map_option5d, x25_map_option5e,
	 NONE, NONE, NONE, OBJ(int,3), 0);
KEYWORD_OR(x25_map_option5a, x25_map_option5c, x25_map_option6,
	   OBJ(int,11), X25MAP_METHOD,
	   "method", "Specify encapsulation method", PRIV_CONF | PRIV_SUBIF);
IFELSE	(x25_map_option5, x25_map_option6, x25_map_option5a,
	 (GETOBJ(int,2) == LINK_PAD) || (GETOBJ(int,2) == LINK_BRIDGE));

KEYWORD_OR(x25_map_option4, x25_map_options, x25_map_option5,
	   OBJ(int,11), X25MAP_PASSIVE,
	   "passive", "Compress outgoing TCP packets only if incoming TCP"
	   " packets are compressed", PRIV_CONF | PRIV_SUBIF);

NUMBER	(x25_map_option3c, x25_map_options, NONE,
	 OBJ(int,15), 1, 127, "Output window size");
NUMBER	(x25_map_option3b, x25_map_option3c, NONE,
	 OBJ(int,14), 1, 127, "Input window size");
NOPREFIX (x25_map_option3_no, x25_map_option3b, x25_map_endline);
KEYWORD_OR(x25_map_option3, x25_map_option3_no, x25_map_option4,
	   OBJ(int,11), X25MAP_WINDOWSIZE,
	   "windowsize", "Request window sizes for originated calls",
	   PRIV_CONF | PRIV_SUBIF);

NUMBER	(x25_map_option2c, x25_map_options, NONE,
	 OBJ(int,13), 16, 4096, "Maximum output packet size (power of 2)");
NUMBER	(x25_map_option2b, x25_map_option2c, NONE,
	 OBJ(int,12), 16, 4096, "Maximum input packet size (power of 2)");
NOPREFIX (x25_map_option2_no, x25_map_option2b, x25_map_endline);
KEYWORD_OR(x25_map_option2, x25_map_option2_no, x25_map_option3,
	   OBJ(int,11), X25MAP_PACKETSIZE,
	   "packetsize", "Request maximum packet sizes for originated calls",
	   PRIV_CONF | PRIV_SUBIF);

KEYWORD_OR(x25_map_options, x25_map_options, x25_map_option2,
	   OBJ(int,11), X25MAP_BROADCAST,
	   "broadcast", "Send broadcasts to this host",
	   PRIV_CONF | PRIV_SUBIF);

X121ADDR(x25_map_x121_pvc, x25_map_options, x25_map_options,
	 OBJ(paddr,1), "Destination host address");
X121ADDR(x25_map_x121_svc, x25_map_options, NONE,
	 OBJ(paddr,1), "Destination host address");
TXT_LINE(x25_map_no_junk, x25_map_endline, x25_map_endline, OBJ(string,1));
X121ADDR(x25_map_no_x121, x25_map_no_junk, x25_map_endline, OBJ(paddr,1), "");
TESTVAR	(x25_map_svc_noprefix, x25_map_no_x121, x25_map_x121_svc,
	 NONE, NONE, NONE, sense, FALSE);
TESTVAR	(x25_map_x121, x25_map_svc_noprefix, x25_map_x121_pvc,
	 NONE, NONE, NONE, OBJ(int,1), 0);

PROTOADDR (x25_map_dgram9, x25_map_x121, x25_map_x121,
	   OBJ(int,10), OBJ(paddr,10), PMATCH_DGRAM PMATCH_COMPRESSED_TCP
	   PMATCH_EXCLUDE PMATCH_BRIDGE PMATCH_ARP PMATCH_AARP PMATCH_QLLC
	   PMATCH_SNAPSHOT PMATCH_CDP);

PROTOADDR (x25_map_dgram8, x25_map_dgram9, x25_map_x121,
	   OBJ(int,9), OBJ(paddr,9), PMATCH_DGRAM PMATCH_COMPRESSED_TCP
	   PMATCH_EXCLUDE PMATCH_BRIDGE PMATCH_ARP PMATCH_AARP PMATCH_QLLC
	   PMATCH_SNAPSHOT PMATCH_CDP);

PROTOADDR (x25_map_dgram7, x25_map_dgram8, x25_map_x121,
	   OBJ(int,8), OBJ(paddr,8), PMATCH_DGRAM PMATCH_COMPRESSED_TCP
	   PMATCH_EXCLUDE PMATCH_BRIDGE PMATCH_ARP PMATCH_AARP PMATCH_QLLC
	   PMATCH_SNAPSHOT PMATCH_CDP);

PROTOADDR (x25_map_dgram6, x25_map_dgram7, x25_map_x121,
	   OBJ(int,7), OBJ(paddr,7), PMATCH_DGRAM PMATCH_COMPRESSED_TCP
	   PMATCH_EXCLUDE PMATCH_BRIDGE PMATCH_ARP PMATCH_AARP PMATCH_QLLC
	   PMATCH_SNAPSHOT PMATCH_CDP);

PROTOADDR (x25_map_dgram5, x25_map_dgram6, x25_map_x121,
	   OBJ(int,6), OBJ(paddr,6), PMATCH_DGRAM PMATCH_COMPRESSED_TCP
	   PMATCH_EXCLUDE PMATCH_BRIDGE PMATCH_ARP PMATCH_AARP PMATCH_QLLC
	   PMATCH_SNAPSHOT PMATCH_CDP);

PROTOADDR (x25_map_dgram4, x25_map_dgram5, x25_map_x121,
	   OBJ(int,5), OBJ(paddr,5), PMATCH_DGRAM PMATCH_COMPRESSED_TCP
	   PMATCH_EXCLUDE PMATCH_BRIDGE PMATCH_ARP PMATCH_AARP PMATCH_QLLC
	   PMATCH_SNAPSHOT PMATCH_CDP);

PROTOADDR (x25_map_dgram3, x25_map_dgram4, x25_map_x121,
	   OBJ(int,4), OBJ(paddr,4), PMATCH_DGRAM PMATCH_COMPRESSED_TCP
	   PMATCH_EXCLUDE PMATCH_BRIDGE PMATCH_ARP PMATCH_AARP PMATCH_QLLC
	   PMATCH_SNAPSHOT PMATCH_CDP);

PROTOADDR (x25_map_dgram2, x25_map_dgram3, x25_map_x121,
	   OBJ(int,3), OBJ(paddr,3), PMATCH_DGRAM PMATCH_COMPRESSED_TCP
	   PMATCH_EXCLUDE PMATCH_BRIDGE PMATCH_ARP PMATCH_AARP
	   PMATCH_SNAPSHOT PMATCH_CDP);

TESTVAR	(x25_map_qllc_test, x25_map_x121, x25_map_dgram2,
	 NONE, NONE, NONE, OBJ(int,2), LINK_QLLC);

PROTOADDR (x25_map_dgram1, x25_map_qllc_test, NONE,
	   OBJ(int,2), OBJ(paddr,2), PMATCH_DGRAM PMATCH_COMPRESSED_TCP
	   PMATCH_EXCLUDE PMATCH_BRIDGE PMATCH_ARP PMATCH_AARP
	   PMATCH_SNAPSHOT PMATCH_CDP);

PROTONAME (x25_map_null_paddr, x25_map_x121, x25_map_dgram1,
	   OBJ(int,2), PMATCH_PAD PMATCH_BRIDGE PMATCH_QLLC PMATCH_CDP);

/************************************************************************
 * no x25 pvc <circuit> ...
 * int,1	= <circuit>
 *
 * x25 pvc <circuit> <protocol> ...
 * (encapsulation pvc preamble; terminates in the common encapsulation
 *  structure)
 * int,1	= <circuit>
 *
 * x25 pvc <circ> tunnel <ipaddr> interface serial <remote-num> ...SWITCH-SPEC
 * int,1	= <circ>
 * paddr,1	= <ipaddr>
 * int,2	= X25_ROUTE_IPADDR
 * string,1	= <remote-num> (may take form "#" or "#/#")
 *
 * x25 pvc <circ> interface <local-int> ...SWITCH-SPEC
 *
 * int,1	= <circ>
 * idb,1	= <local-int>
 * int,2	= X25_ROUTE_IF
 *
 * SWITCH-SPEC:
 * ... [pvc] <circ> ...FLOW-SPEC
 *
 * int,3	= <circ>
 *
 * FLOW-SPEC:
 * ... [packetsize <pin> <pout>] [windowsize <win> <wout>]
 *
 * int,4	= <pin>
 * int,5	= <pout>
 * int,6	= <win>
 * int,7	= <wout>
 *
 */

/* x25 pvc <circuit> tunnel <ipaddr> interface serial <remote-num>
 *   [pvc] <circuit> [packetsize <in> <out>] [windowsize <in> <out>]
 */

EOLS	(x25_pvc_eol, x25_interface_commands, X25_PVC);

NUMBER	(x25_pvc_wout, x25_pvc_eol, no_alt,
	 OBJ(int,7), 1, 127, "Output window (packets)");
NUMBER	(x25_pvc_win, x25_pvc_wout, no_alt,
	 OBJ(int,6), 1, 127, "Input window (packets)");
KEYWORD	(x25_pvc_wind, x25_pvc_win, x25_pvc_eol,
	 "windowsize", "Specify window sizes", PRIV_CONF);
NUMBER	(x25_pvc_pout, x25_pvc_wind, no_alt,
	 OBJ(int,5), 16, 4096, "Maximum output packet size (power of 2)");
NUMBER	(x25_pvc_pin, x25_pvc_pout, no_alt,
	 OBJ(int,4), 16, 4096, "Maximum input packet size (power of 2)");
KEYWORD	(x25_pvc_pack, x25_pvc_pin, x25_pvc_wind,
	 "packetsize", "Specify maximum packet sizes", PRIV_CONF);

NUMBER	(x25_pvc_targcir, x25_pvc_pack, no_alt,
	 OBJ(int,3), 1, 4095, "Target channel number");
EQUAL	(x25_pvc_targtest, x25_pvc_targcir, no_alt, in_help, FALSE);
KEYWORD	(x25_pvc_pvc, x25_pvc_targcir, x25_pvc_targtest,
	 "pvc", "Specify target PVC", PRIV_CONF);

INTERFACE(x25_pvc_locint, x25_pvc_pvc, no_alt, OBJ(idb,1), IFTYPE_SERIAL);

STRING	(x25_pvc_remintno, x25_pvc_pvc, no_alt,
	 OBJ(string,1), "Remote interface number");
KEYWORD	(x25_pvc_serial, x25_pvc_remintno, no_alt,
	 "Serial", "Remote interface type", PRIV_CONF);
KEYWORD	(x25_pvc_remint, x25_pvc_serial, no_alt,
	 common_str_interface, "Specify remote interface name", PRIV_CONF);
IPADDR	(x25_pvc_ipaddr, x25_pvc_remint, no_alt,
	 OBJ(paddr,1), "IP address of remote Cisco");

KEYWORD_ID(x25_pvc_interface, x25_pvc_locint, x25_map_null_paddr,
	   OBJ(int,2), X25_ROUTE_IF,
	   common_str_interface, "Switch PVC to a local interface", PRIV_CONF);
KEYWORD_ID(x25_pvc_tunnel, x25_pvc_ipaddr, x25_pvc_interface,
	   OBJ(int,2), X25_ROUTE_IPADDR,
	   "tunnel", "Switch PVC to a remote Cisco router", PRIV_CONF);

NOPREFIX(x25_pvc_nocheck, x25_pvc_tunnel, x25_pvc_eol);
NUMBER	(x25_pvc_circuit, x25_pvc_nocheck, no_alt,
	 OBJ(int,1), 1, 4095, "Circuit number");
NVGENS	(x25_pvc_nvgen, x25_pvc_circuit, x25_interface_commands, X25_PVC);
KEYWORD	(x25_pvc, x25_pvc_nvgen, no_alt,
	 "pvc", "Configure a Permanent Virtual Circuit",
	 PRIV_CONF | PRIV_SUBIF);

/************************************************************************
 * x25 map <protocol> ...
 * (encapsulation map command preamble)
 *
 * x25 map cmns <NSAP-addr> [<X.121-addr>]	for WAN (X.25) links
 * x25 map cmns <NSAP-addr> <MAC-addr>		for LAN (LLC2) links
 * (CMNS route map)
 *
 * For a standard map command preamble, this tree sets the object:
 *  OBJ(int,1) PVC number - set to zero
 *
 * For a CMNS route map, this tree sets the objects:
 *  OBJ(paddr,2) CMNS NSAP address
 *  OBJ(int,2) to LINK_CMNS
 *  OBJ(paddr,1) to any optional X.121 address on an X.25 interface
 *  OBJ(hwaddr,1) to the MAC address on a CMNS interface
 */

ENADDR	(x25_map_cmns_macaddr, x25_map_endline, NONE,
	 OBJ(hwaddr,1), "MAC address to map");
PROTOADDR (x25_map_cmns_if, x25_map_cmns_macaddr, NONE,
	   OBJ(int,2), OBJ(paddr,2), PMATCH_CMNS);

X121ADDR(x25_map_cmns_x121addr, x25_map_endline, x25_map_endline,
	 OBJ(paddr,1), "X.121 address to map");
PROTOADDR (x25_map_serial_if, x25_map_cmns_x121addr, x25_map_null_paddr,
	   OBJ(int,2), OBJ(paddr,2), PMATCH_CMNS);

IFELSE	(x25_map_cmns_test, x25_map_serial_if, x25_map_cmns_if,
	 is_x25(csb->interface->hwptr));

NVGENS	(x25_map_nvgen, x25_map_cmns_test, x25_interface_commands, X25_MAP);
KEYWORD (x25_map, x25_map_nvgen, x25_pvc,
	 "map", "Map protocol addresses to X.121 address",
	 PRIV_CONF | PRIV_SUBIF);

/************************************************************************
 *	x25 remote-red <host-ipaddr> remote-black <blacker-ipaddr>
 * paddr,1	= <host-ipaddr>
 * paddr,2	= <blacker-ipaddr>
 */

EOLS	(x25_rr_eol, x25_interface_commands, X25_BFE_REMOTE);
IPADDR	(x25_rr_blackip, x25_rr_eol, no_alt,
	 OBJ(paddr,2), "IP address of remote BFE");
KEYWORD	(x25_rr_rb, x25_rr_blackip, no_alt,
	 "remote-black", "Specify IP address of remote BFE", PRIV_CONF);
NOPREFIX(x25_rr_noprefix, x25_rr_rb, x25_rr_eol);
IPADDR	(x25_rr_hostip, x25_rr_noprefix, no_alt,
	 OBJ(paddr,1), "IP address of host");
NVGENS	(x25_rr_nvcheck, x25_rr_hostip,
	 x25_interface_commands, X25_BFE_REMOTE);
KEYWORD	(x25_rr, x25_rr_nvcheck, NONE,
	 "remote-red", "Establish BFE Emergency Mode mapping",
	 PRIV_CONF);

/************************************************************************
 *	x25 bfe-decision {yes | no | ask}
 */

EOLS	(x25_bfed_eol, x25_interface_commands, X25_BFE_DECISION);
KEYWORD_ID (x25_bfed_ask, x25_bfed_eol, no_alt,
	    OBJ(int,1), BFE_DECISION_ASK,
	    "ask", "Prompt administrator for emergency mode decision",
	    PRIV_CONF);
KEYWORD_ID (x25_bfed_no, x25_bfed_eol, x25_bfed_ask,
	    OBJ(int,1), BFE_DECISION_NO,
	    "no", "Router will not participate in emergency mode decision",
	    PRIV_CONF);
KEYWORD_ID (x25_bfed_yes, x25_bfed_eol, x25_bfed_no,
	    OBJ(int,1), BFE_DECISION_YES,
	    "yes", "Router will participate in emergency mode decision",
	    PRIV_CONF);

NOPREFIX (x25_bfed_nocheck, x25_bfed_yes, x25_bfed_eol);
KEYWORD	(x25_bfed, x25_bfed_nocheck, x25_rr,
	 "bfe-decision", "Specify BFE Emergency Mode participation",
	 PRIV_CONF);

/************************************************************************
 *	x25 bfe-emergency {never | always | decision}
 */
EOLS	(x25_bfee_eol, x25_interface_commands, X25_BFE_EMER);
KEYWORD_ID (x25_bfee_decision, x25_bfee_eol, no_alt,
	    OBJ(int,1), BFE_DECISION,
	    "decision",
      "Enter Emergency Mode after receiving Emergency mode window open packet",
	    PRIV_CONF);
KEYWORD_ID (x25_bfee_always, x25_bfee_eol, x25_bfee_decision,
	    OBJ(int,1), BFE_ALWAYS,
	    "always", "Enter BFE Emergency Mode when directed by BFE",
	    PRIV_CONF);
KEYWORD_ID (x25_bfee_never, x25_bfee_eol, x25_bfee_always,
	    OBJ(int,1), BFE_NEVER,
	    "never", "Never use BFE Emergency Mode",
	    PRIV_CONF);

NOPREFIX (x25_bfee_nocheck, x25_bfee_never, x25_bfee_eol);
KEYWORD	(x25_bfee, x25_bfee_nocheck, x25_bfed,
	 "bfe-emergency", "Set Blacker Front End Emergency Mode",
	 PRIV_CONF);
/* Don't allow BFE commands for non-BFE interfaces. */
ASSERT	(x25_bfe, x25_bfee, x25_map, ISBFE(csb->interface->hwptr));

/************************************************************************
 *	x25 facility <option>
 *
 * options:
 *	windowsize (OBJ(int,1) & OBJ(int,2))
 *	packetsize (OBJ(int,1) & OBJ(int,2))
 *	cug (OBJ(int,1))
 *	reverse
 *	rpoa (OBJ(string,1))
 *	throughput (OBJ(int,1) & OBJ(int,2))
 *	transmit-delay (OBJ(int,1))
 */
EOLS	(x25_facility_option12c, x25_interface_commands, X25_FACILITY);
NUMBER	(x25_facility_option12b, x25_facility_option12c, no_alt,
	 OBJ(int,1), 0, 65534, "Milliseconds");
NVGENS	(x25_facility_option12a, x25_facility_option12b,
	 x25_interface_commands, X25_FACILITY);
KEYWORD_ID(x25_facility_option12, x25_facility_option12a, no_alt,
	   OBJ(int,10), FACIL_TDELAY,
	   "transit-delay", "Specify maximum acceptable transit delay",
	   PRIV_CONF);

EOLS	(x25_facility_option8d, x25_interface_commands, X25_FACILITY);
NUMBER	(x25_facility_option8c, x25_facility_option8d, no_alt,
	 OBJ(int,2), 75, 64000, "Output throughput, a standard baud rate");
NUMBER	(x25_facility_option8b, x25_facility_option8c, no_alt,
	 OBJ(int,1), 75, 64000, "Input throughput, a standard baud rate");
NVGENS	(x25_facility_option8a, x25_facility_option8b,
	 x25_interface_commands, X25_FACILITY);
KEYWORD_ID(x25_facility_option8, x25_facility_option8a, x25_facility_option12,
	   OBJ(int,10), FACIL_THROUGHPUT,
	   "throughput", "Request bandwidth in X.25 network", PRIV_CONF);

EOLS	(x25_facility_option11c, x25_interface_commands, X25_FACILITY);
STRING	(x25_facility_option11b, x25_facility_option11c, no_alt,
	 OBJ(string,1), "RPOA list name specified in \"x25 rpoa\"");
NVGENS	(x25_facility_option11a, x25_facility_option11b,
	 x25_interface_commands, X25_FACILITY);
KEYWORD_ID(x25_facility_option11, x25_facility_option11a, x25_facility_option8,
	   OBJ(int,10), FACIL_RPOA,
	   "rpoa", "Specify transit RPOA list in Call Requests", PRIV_CONF);

EOLS	(x25_facility_option1a, x25_interface_commands, X25_FACILITY);
KEYWORD_ID(x25_facility_option1, x25_facility_option1a, x25_facility_option11,
	   OBJ(int,10), FACIL_REVERSE,
	   "reverse", "Use reverse charging on originated calls", PRIV_CONF);

EOLS	(x25_facility_option4c, x25_interface_commands, X25_FACILITY);
NUMBER	(x25_facility_option4b, x25_facility_option4c, no_alt,
	 OBJ(int,1), 1, 99, "CUG number");
NVGENS	(x25_facility_option4a, x25_facility_option4b,
	 x25_interface_commands, X25_FACILITY);
KEYWORD_ID(x25_facility_option4, x25_facility_option4a, x25_facility_option1,
	   OBJ(int,10), FACIL_CUG,
	   "cug", "Specify a Closed User Group number", PRIV_CONF);

EOLS	(x25_facility_option6d, x25_interface_commands, X25_FACILITY);
NUMBER	(x25_facility_option6c, x25_facility_option6d, no_alt,
	 OBJ(int,2), 16, 4096, "Maximum output packet size (power of 2)");
NUMBER	(x25_facility_option6b, x25_facility_option6c, no_alt,
	 OBJ(int,1), 16, 4096, "Maximum input packet size (power of 2)");
NVGENS	(x25_facility_option6a, x25_facility_option6b,
	 x25_interface_commands, X25_FACILITY);
KEYWORD_ID(x25_facility_option6, x25_facility_option6a, x25_facility_option4,
	   OBJ(int,10), FACIL_PACKET,
	   "packetsize", "Specify maximum packet sizes", PRIV_CONF);

EOLS	(x25_facility_option7d, x25_interface_commands, X25_FACILITY);
NUMBER	(x25_facility_option7c, x25_facility_option7d, no_alt,
	 OBJ(int,2), 1, 127, "Output window size (should be < modulo)");
NUMBER	(x25_facility_option7b, x25_facility_option7c, no_alt,
	 OBJ(int,1), 1, 127, "Input window size (should be < modulo)");
NVGENS	(x25_facility_option7a, x25_facility_option7b,
	 x25_interface_commands, X25_FACILITY);
KEYWORD_ID(x25_facility_option7, x25_facility_option7a, x25_facility_option6,
	   OBJ(int,10), FACIL_WINDOW,
	   "windowsize", "Specify window sizes", PRIV_CONF);

KEYWORD	(x25_facility, x25_facility_option7, x25_bfe,
	 "facility", "Set explicit facilities for originated calls", PRIV_CONF);

/************************************************************************
 *	x25 use-source-address
 */
EOLS	(x25_usesrcaddr_eol, x25_interface_commands, X25_SRCADDR);
KEYWORD	(x25_usesrcaddr, x25_usesrcaddr_eol, x25_facility,
	 "use-source-address", "Use local source address for forwarded calls",
	 PRIV_CONF);

/************************************************************************
 *	x25 th <packets>
 */
PARAMS_KEYONLY	(x25_th, x25_usesrcaddr, "th",
		 OBJ(int,1), 1, 127,
		 x25_interface_commands, X25_TH,
		 "Set packet count acknowledgement threshold", "Packets",
		 PRIV_CONF);

/************************************************************************
 *	x25 suppress-calling-address
 */
EOLS	(x25_sca_eol, x25_interface_commands, X25_SUPPRESS_SRC);
KEYWORD	(x25_sca, x25_sca_eol, x25_th,
	 "suppress-calling-address", "Omit source address in outgoing calls",
	 PRIV_CONF);

/************************************************************************
 *	x25 suppress-called-address
 */
EOLS	(x25_scd_eol, x25_interface_commands, X25_SUPPRESS_DST);
KEYWORD	(x25_scd, x25_scd_eol, x25_sca,
	 "suppress-called-address",
	 "Omit destination address in outgoing calls", PRIV_CONF);

/************************************************************************
 *	x25 pad-access
 */

EOLS	(x25_padaccess_eol, x25_interface_commands, X25_PADACCESS);
KEYWORD	(x25_padaccess, x25_padaccess_eol, x25_scd,
	 "pad-access",
	 "Accept only PAD connections from statically mapped X25 hosts",
	 PRIV_CONF);

/************************************************************************
 *	x25 nvc <count>
 */

PARAMS_KEYONLY	(x25_nvc, x25_padaccess, "nvc",
		 OBJ(int,1), 1, X25_MAXNVC,
		 x25_interface_commands, X25_NVC,
		 "Set maximum SVCs simultaneously open to one host "
		 "per protocol",
		 "SVCs", PRIV_CONF);

/************************************************************************
 *	x25 linkrestart
 */
EOLS	(x25_linkrestart_eol, x25_interface_commands, X25_LINKSTART);
KEYWORD	(x25_linkrestart, x25_linkrestart_eol, x25_nvc,
	 "linkrestart", "Restart when LAPB resets",
	 PRIV_CONF);

/************************************************************************
 *	x25 ip-precedence
 */

EOLS	(x25_ipprec_eol, x25_interface_commands, X25_IPTOS);
KEYWORD	(x25_ipprec, x25_ipprec_eol, x25_linkrestart,
	 "ip-precedence", "Open one virtual circuit for each IP TOS",
	 PRIV_CONF);

/************************************************************************
 *	x25 idle <minutes>
 */

PARAMS_KEYONLY	(x25_idle, x25_ipprec, "idle",
		 OBJ(int,1), 0, 255,
		 x25_interface_commands, X25_IDLE,
		 "Set inactivity time before clearing SVC",
		 "Minutes; 0 to never clear", PRIV_CONF);

/************************************************************************
 *	x25 hold-queue <value>
 */

PARAMS_KEYONLY	(x25_holdq, x25_idle, "hold-queue",
		 OBJ(int,1), 0, 9999,
		 x25_interface_commands, X25_HOLDQUEUE,
		 "Set VC hold-queue depth",
		 "Maximum packets to queue before dropping", PRIV_CONF);

/************************************************************************
 *	x25 hold-vc-timer <minutes>
 */

PARAMS_KEYONLY	(x25_hvctimer, x25_holdq, "hold-vc-timer",
		 OBJ(int,1), 0, 1000,
		 x25_interface_commands, X25_HOLDTIME,
		 "Set time to prevent calls to a failed destination",
		 "Minutes; 0 to always try calls", PRIV_CONF);

/************************************************************************
 *	x25 default { ip | pad }
 *
 * OBJ(int,1) = LINK_PAD or LINK_IP
 */

EOLS	(x25_default_eol, x25_interface_commands, X25_DEFAULT);
KEYWORD_ID(x25_default_pad, x25_default_eol, no_alt,
	 OBJ(int,1), LINK_PAD,
	 "pad", "Pad protocol", PRIV_CONF);
KEYWORD_ID(x25_default_ip, x25_default_eol, x25_default_pad,
	 OBJ(int,1), LINK_IP,
	 "ip", "IP protocol", PRIV_CONF);
NOPREFIX(x25_default_noprefix, x25_default_ip, x25_default_eol);
NVGENS	(x25_default_nvgen, x25_default_noprefix,
	 x25_interface_commands, X25_DEFAULT);
KEYWORD	(x25_default, x25_default_nvgen, x25_hvctimer,
	 "default", "Set protocol for calls with unknown Call User Data",
	 PRIV_CONF);

/************************************************************************
 *	x25 accept-reverse
 */

EOLS	(x25_accrev_eol, x25_interface_commands, X25_ACCEPTREV);
KEYWORD	(x25_accrev, x25_accrev_eol, x25_default,
	 "accept-reverse", "Accept all reverse charged calls", PRIV_CONF);

/************************************************************************
 *	x25 ops <bytes>
 */

PARAMS_KEYONLY	(x25_ops, x25_accrev, "ops",
		 OBJ(int,1), 16, 4096,
		 x25_interface_commands, X25_OPS,
		 "Set default maximum output packet size",
		 "Bytes (power of two)", PRIV_CONF);

/************************************************************************
 *	x25 ips <bytes>
 */

PARAMS_KEYONLY	(x25_ips, x25_ops, "ips",
		 OBJ(int,1), 16, 4096,
		 x25_interface_commands, X25_IPS,
		 "Set default maximum input packet size",
		 "Bytes (power of two)", PRIV_CONF);

/************************************************************************
 *	x25 wout <packets>
 */
PARAMS_KEYONLY	(x25_wout, x25_ips, "wout",
		 OBJ(int,1), 1, 127,
		 x25_interface_commands, X25_WOUT,
		 "Set default output window (maximum unacknowledged packets)",
		 "Packets", PRIV_CONF);

/************************************************************************
 *	x25 win <packets>
 */
PARAMS_KEYONLY	(x25_win, x25_wout, "win",
		 OBJ(int,1), 1, 127,
		 x25_interface_commands, X25_WIN,
		 "Set default input window (maximum unacknowledged packets)",
		 "Packets", PRIV_CONF);

/************************************************************************
 *	x25 t23 <seconds>
 * DTE only
 */

PARAMS_KEYONLY	(x25_t23, x25_win, "t23",
		 OBJ(int,1), 1, 255,
		 x25_interface_commands, X25_T23,
		 "Set DTE Clear Request retransmission timer", "Seconds",
		 PRIV_CONF);

/************************************************************************
 *	x25 t22 <seconds>
 * DTE only
 */

PARAMS_KEYONLY	(x25_t22, x25_t23, "t22",
		 OBJ(int,1), 1, 255,
		 x25_interface_commands, X25_T22,
		 "Set DTE Reset Request retransmission timer", "Seconds",
		 PRIV_CONF);

/************************************************************************
 *	x25 t21 <seconds>
 * DTE only
 */

PARAMS_KEYONLY	(x25_t21, x25_t22, "t21",
		 OBJ(int,1), 1, 255,
		 x25_interface_commands, X25_T21,
		 "Set DTE Call Request retransmission timer", "Seconds",
		 PRIV_CONF);

/************************************************************************
 *	x25 t20 <seconds>
 * DTE only
 */
PARAMS_KEYONLY	(x25_t20, x25_t21, "t20",
		 OBJ(int,1), 1, 255,
		 x25_interface_commands, X25_T20,
		 "Set DTE Restart Request retransmission timer", "Seconds",
		 PRIV_CONF);

/************************************************************************
 *	x25 t13 <seconds>
 * DCE only
 */
PARAMS_KEYONLY	(x25_t13, x25_win, "t13",
		 OBJ(int,1), 1, 255,
		 x25_interface_commands, X25_T13,
		 "Set DCE Clear Request retransmission timer", "Seconds",
		 PRIV_CONF);

/************************************************************************
 *	x25 t12 <seconds>
 * DCE only
 */

PARAMS_KEYONLY	(x25_t12, x25_t13, "t12",
		 OBJ(int,1), 1, 255,
		 x25_interface_commands, X25_T12,
		 "Set DCE Reset Request retransmission timer", "Seconds",
		 PRIV_CONF);

/************************************************************************
 *	x25 t11 <seconds>
 * DCE only
 */

PARAMS_KEYONLY	(x25_t11, x25_t12, "t11",
		 OBJ(int,1), 1, 255,
		 x25_interface_commands, X25_T11,
		 "Set DCE Call Request retransmission timer", "Seconds",
		 PRIV_CONF);

/************************************************************************
 *	x25 t10 <seconds>
 * DCE only
 */

PARAMS_KEYONLY	(x25_t10, x25_t11, "t10",
		 OBJ(int,1), 1, 255,
		 x25_interface_commands, X25_T10,
		 "Set DCE Restart Request retransmission timer", "Seconds",
		 PRIV_CONF);

/************************************************************
 * This IFELSE selects either the t10-t13 or the t20-t23
 * retransmission timers, depending on whether the interface
 * is a DCE or DTE.
 */
IFELSE	(x25_retrans_params, x25_t10, x25_t20,
	 IS_DCE(csb->interface->hwptr, x25_dce));


/************************************************************************
 *	x25 hoc <channel>
 */
PARAMS_KEYONLY	(x25_hoc, x25_retrans_params, "hoc",
		 OBJ(int,1), 0, 4095,
		 x25_interface_commands, X25_HOC,
		 "Set highest outgoing channel",
		 "Virtual Circuit number", PRIV_CONF);

/************************************************************************
 *	x25 loc <channel>
 */

PARAMS_KEYONLY	(x25_loc, x25_hoc, "loc",
		 OBJ(int,1), 0, 4095,
		 x25_interface_commands, X25_LOC,
		 "Set lowest outgoing channel",
		 "Virtual Circuit number", PRIV_CONF);

/************************************************************************
 *	x25 htc <channel>
 */

PARAMS_KEYONLY	(x25_htc, x25_loc, "htc",
		 OBJ(int,1), 0, 4095,
		 x25_interface_commands, X25_HTC,
		 "Set highest two-way channel",
		 "Virtual Circuit number", PRIV_CONF);

/************************************************************************
 *	x25 ltc <channel>
 */

PARAMS_KEYONLY	(x25_ltc, x25_htc, "ltc",
		 OBJ(int,1), 0, 4095,
		 x25_interface_commands, X25_LTC,
		 "Set lowest two-way channel",
		 "Virtual Circuit number", PRIV_CONF);

/************************************************************************
 *	x25 hic <channel>
 */
PARAMS_KEYONLY	(x25_hic, x25_ltc, "hic",
		 OBJ(int,1), 0, 4095,
		 x25_interface_commands, X25_HIC,
		 "Set highest incoming channel",
		 "Virtual Circuit number", PRIV_CONF);

/************************************************************************
 *	x25 lic <channel>
 */

PARAMS_KEYONLY	(x25_lic, x25_hic, "lic",
		 OBJ(int,1), 0, 4095,
		 x25_interface_commands, X25_LIC,
		 "Set lowest incoming channel",
		 "Virtual Circuit number", PRIV_CONF);

/************************************************************************
 *	x25 address <X.121-addr>
 */
EOLS	(x25_address_eol, x25_interface_commands, X25_ADDRESS);
X121ADDR(x25_address_addr, x25_address_eol, no_alt,
	 OBJ(paddr,1), "X.121 address");
NOPREFIX(x25_address_noprefix, x25_address_addr, x25_address_eol);
NVGENS	(x25_address_nvgen, x25_address_noprefix,
	 x25_interface_commands, X25_ADDRESS);
KEYWORD (x25_address_kwd, x25_address_nvgen, NONE,
	 "address", "Set interface X.121 address", PRIV_CONF);
/* Don't allow 'address' for IEEE interfaces when 'cmns enable' is set. */
ASSERT	(x25_address, x25_address_kwd, x25_lic,
	 (! ISCMNS_IEEE(csb->interface->hwptr)));

/************************************************************************
 *	x25 modulo <modulus>
 */

EOLS	(x25_modulo_eol, x25_interface_commands, X25_MODULO);
KEYWORD_ID(x25_modulo_128, x25_modulo_eol, no_alt,
	   OBJ(int,1), X25_MOD128,
	   "128", "Packet numbering modulus", PRIV_CONF);
KEYWORD_ID(x25_modulo_8, x25_modulo_eol, x25_modulo_128,
	   OBJ(int,1), X25_MOD8,
	   "8", "Packet numbering modulus", PRIV_CONF);
NOPREFIX(x25_modulo_noprefix, x25_modulo_8, x25_modulo_eol);
NVGENS	(x25_modulo_nvgen, x25_modulo_noprefix,
	 x25_interface_commands, X25_MODULO);
KEYWORD	(x25_modulo_kwd, x25_modulo_nvgen, NONE,
	 "modulo", "Set packet numbering modulus", PRIV_CONF);
/* Don't allow 'modulo' for BFE interfaces. */
ASSERT	(x25_modulo, x25_modulo_kwd, x25_address,
	 ! ISBFE(csb->interface->hwptr));

/************************************************************************/

KEYWORD (x25_keyword, x25_modulo, NONE,
	 "x25", "X.25 Level 3", PRIV_CONF | PRIV_SUBIF);

ASSERT (x25_keyword_test, x25_keyword, ALTERNATE,
	(is_x25(csb->interface->hwptr) || ISCMNS_IEEE(csb->interface->hwptr)));


#undef	ALTERNATE
#define	ALTERNATE	x25_keyword_test
