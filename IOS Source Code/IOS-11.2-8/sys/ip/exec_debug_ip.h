/* $Id: exec_debug_ip.h,v 3.4.20.2 1996/05/22 20:54:56 raj Exp $
 * $Source: /release/112/cvs/Xsys/ip/exec_debug_ip.h,v $
 *------------------------------------------------------------------
 * E X E C _ D E B U G _ I P . H
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_debug_ip.h,v $
 * Revision 3.4.20.2  1996/05/22  20:54:56  raj
 * CSCdi56721:  Debug ip mpacket doesnt show ttl and other important fields
 * Branch: California_branch
 * Added fields for "detail", and specifying acl as well as group.
 *
 * Revision 3.4.20.1  1996/04/15  21:15:27  bcole
 * CSCdi54649:  IP Host Modularity, IP Automatic Stub Routing feature
 * commit
 * Branch: California_branch
 *
 * Revision 3.4  1996/02/01  23:33:42  widmer
 * CSCdi45325:  Change Keyword/Set to use Keyword_Id macro
 *
 * Revision 3.3  1995/11/20  23:21:48  ahh
 * CSCdi41919:  PT: cannot specify source interface for TCP connections
 * Add source-interface option to outgoing TCP translation and telnet.
 * Also convert some parser defines to enums.
 *
 * Revision 3.2  1995/11/17  09:32:41  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:54:34  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/09/25  08:31:45  tli
 * CSCdi40960:  ISP jumbo patch 2
 *
 * Revision 2.2  1995/09/18  19:35:31  fox
 * CSCdi40523:  Add IP Address Pooling support for ISDN and other sync
 * interfaces
 * Modify IP address pooling to support all Point-to-Point interfaces
 * Remove Odd/Even Address feature
 * Add 'ppp negotiate' command
 *
 * Revision 2.1  1995/07/01  12:55:41  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize ip, ipmulticast, tcp
 *
 * Revision 2.1  1995/06/07  22:18:40  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

LINK_TRANS(debug_ip_extend_here, no_alt);

/******************************************************************
 * debug ip security
 *
 */
EOLS	(debug_ip_security_eol, ip_debug_command, DEBUG_IPSECURITY);
KEYWORD (debug_ip_security, debug_ip_security_eol, debug_ip_extend_here,
	 "security", "IP security options", PRIV_OPR);

/******************************************************************
 * debug peer
 *
 */
EOLS    (debug_peer_eol, debug_command, DEBUG_PEER);

KEYWORD_DEBUG(debug_peer_cmds, debug_peer_eol, debug_ip_security,
	      OBJ(pdb,1), ip_debug_arr,
	      "peer", "IP peer address activity", PRIV_OPR);


/******************************************************************
 * debug ip error [detail] [dump] [<list>]
 *
 * OBJ(int,1) = list
 * OBJ(int,2) = TRUE if "detail" specified
 * OBJ(int,3) = TRUE if "dump" specified
 */
pdecl   (debug_ip_error_test_det);
EOLS	(debug_ip_error_eol, ip_debug_command, DEBUG_IPERROR);
NUMBER	(debug_ip_error_list, debug_ip_error_test_det, debug_ip_error_eol,
	 OBJ(int,1), 1, MAXSLOWACC, "Access list");
SPLIT	(debug_ip_error_test_list, debug_ip_error_list, 
	 debug_ip_error_eol, OBJ(int,1), 0);
KEYWORD_ID(debug_ip_error_dump, debug_ip_error_test_det,
	   debug_ip_error_test_list, OBJ(int,3), TRUE, "dump",
	   "Dump packet data", PRIV_OPR|PRIV_HIDDEN);
SPLIT	(debug_ip_error_test_dump, debug_ip_error_dump, 
	 debug_ip_error_test_list, OBJ(int,3), FALSE);
KEYWORD_ID(debug_ip_error_detail, debug_ip_error_test_det,
	   debug_ip_error_test_dump, OBJ(int,2), TRUE, "detail",
	   "Print more debugging detail", PRIV_OPR);
SPLIT	(debug_ip_error_test_det, debug_ip_error_detail, 
	 debug_ip_error_test_dump, OBJ(int,2), FALSE);
KEYWORD (debug_ip_error, debug_ip_error_test_det, debug_peer_cmds,
	   "error", "IP error debugging", PRIV_OPR);
/******************************************************************
 * debug ip mpacket [detail] [<list>] [<group-name-or-address>]
 *
 * OBJ(int,1) - list
 * OBJ(int,2) - TRUE if "detail" specified
 * OBJ(paddr,3) - group name or address
 */
pdecl   (debug_ip_mpacket_test_det);
EOLS	(debug_ip_mpacket_eol, ip_debug_command, DEBUG_IPMPKT);
IPADDR_NAME (debug_ip_mpacket_addr, debug_ip_mpacket_test_det,
	     debug_ip_mpacket_eol,
	     OBJ(paddr,3), "IP group address");
SPLIT	(debug_ip_mpacket_test_addr, debug_ip_mpacket_addr,
	 debug_ip_mpacket_eol, OBJ(paddr, 3), 0);
TEST_MULTIPLE_FUNCS(debug_ip_mpacket_ambig, debug_ip_mpacket_test_addr,
		    NONE, no_alt);
NUMBER	(debug_ip_mpacket_list, debug_ip_mpacket_test_det,
	 debug_ip_mpacket_ambig,
	 OBJ(int,1), 1, MAXSLOWACC, "Access list");
SPLIT	(debug_ip_mpacket_test_list, debug_ip_mpacket_list,
	 debug_ip_mpacket_ambig, OBJ(int, 1), 0);
KEYWORD_ID(debug_ip_mpacket_detail, debug_ip_mpacket_test_det,
	   debug_ip_mpacket_test_list, OBJ(int,2), TRUE, "detail",
	   "Print IP header fields and MAC addresses", PRIV_OPR);
SPLIT	(debug_ip_mpacket_test_det, debug_ip_mpacket_detail,
	 debug_ip_mpacket_test_list, OBJ(int, 2), FALSE);
KEYWORD (debug_ip_mpacket, debug_ip_mpacket_test_det, debug_ip_error,
	 "mpacket", "IP multicast packet debugging", PRIV_OPR);

/******************************************************************
 * debug ip packet [detail] [dump] [<list>]
 *
 * OBJ(int,1) = list
 * OBJ(int,2) = TRUE if "detail" specified
 * OBJ(int,3) = TRUE if "dump" specified
 */
pdecl   (debug_ip_packet_test_det);
EOLS	(debug_ip_packet_eol, ip_debug_command, DEBUG_IPPKT);
NUMBER	(debug_ip_packet_list, debug_ip_packet_test_det, debug_ip_packet_eol,
	 OBJ(int,1), 1, MAXSLOWACC, "Access list");
SPLIT	(debug_ip_packet_test_list, debug_ip_packet_list, 
	 debug_ip_packet_eol, OBJ(int,1), 0);
KEYWORD_ID(debug_ip_packet_dump, debug_ip_packet_test_det,
	   debug_ip_packet_test_list, OBJ(int,3), TRUE, "dump",
	   "Dump packet data", PRIV_OPR|PRIV_HIDDEN);
SPLIT	(debug_ip_packet_test_dump, debug_ip_packet_dump, 
	 debug_ip_packet_test_list, OBJ(int,3), FALSE);
KEYWORD_ID(debug_ip_packet_detail, debug_ip_packet_test_det,
	   debug_ip_packet_test_dump, OBJ(int,2), TRUE, "detail",
	   "Print more debugging detail", PRIV_OPR);
SPLIT	(debug_ip_packet_test_det, debug_ip_packet_detail, 
	 debug_ip_packet_test_dump, OBJ(int,2), FALSE);
KEYWORD (debug_ip_packet, debug_ip_packet_test_det, debug_ip_mpacket,
	   "packet", "General IP debugging and IPSO security transactions",
	   PRIV_OPR);

/******************************************************************
 * debug ip icmp
 *
 */
EOLS	(debug_ip_icmp_eol, ip_debug_command, DEBUG_ICMPPKT);
KEYWORD (debug_ip_icmp, debug_ip_icmp_eol, debug_ip_packet,
	 "icmp", "ICMP transactions", PRIV_OPR);

/******************************************************************
 * debug ip
 *
 */
KEYWORD (debug_ip, debug_ip_icmp, ALTERNATE,
	 "ip", "IP information", PRIV_OPR);

#undef	ALTERNATE
#define	ALTERNATE	debug_ip
