/* $Id: exec_debug_ip_ospf.h,v 3.3 1996/02/01 23:34:06 widmer Exp $
 * $Source: /release/111/cvs/Xsys/iprouting/exec_debug_ip_ospf.h,v $
 *------------------------------------------------------------------
 * IP OSPF debug commands
 *
 * May 1993, Robert Widmer
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: exec_debug_ip_ospf.h,v $
 * Revision 3.3  1996/02/01  23:34:06  widmer
 * CSCdi45325:  Change Keyword/Set to use Keyword_Id macro
 *
 * Revision 3.2  1995/11/17  17:33:15  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:07:04  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/09/05  22:08:50  myeung
 * CSCdi39807:  OSPF need access-list debugging
 * - Add access-list debugging for SPF calculation and flooding
 * - Add separate debugging for intra-area, inter-area and external SPF
 *
 * Revision 2.1  1995/07/01  12:16:14  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize iprouting
 *
 * Revision 2.1  1995/06/07  22:18:52  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * debug ip ospf lsa-generation
 *
 * OBJ(int,1) = list
 */
EOLS	(debug_ip_ospf_lsa_gen_eol, iprouting_debug_command,
	 DEBUG_OSPF_LSA_GEN);
NUMBER	(debug_ip_ospf_lsa_gen_list, debug_ip_ospf_lsa_gen_eol,
	 debug_ip_ospf_lsa_gen_eol, OBJ(int,1), 1, MAXSLOWACC, "Access list");
KEYWORD	(debug_ip_ospf_lsa_gen, debug_ip_ospf_lsa_gen_list, no_alt,
	 "lsa-generation", "OSPF lsa generation", PRIV_OPR);

/******************************************************************
 * debug ip ospf tree
 *
 */
EOLS	(debug_ip_ospf_tree_eol, iprouting_debug_command, DEBUG_OSPF_TREE);
KEYWORD	(debug_ip_ospf_tree, debug_ip_ospf_tree_eol, debug_ip_ospf_lsa_gen,
	 "tree", "OSPF database tree", PRIV_OPR);

/******************************************************************
 * debug ip ospf spf [intra [list] | inter [list] | ext [list]]
 *
 */
EOLS	(debug_ip_ospf_spf_eol, iprouting_debug_command, DEBUG_OSPF_SPF);
EOLS	(debug_ip_ospf_spf_ext_eol, iprouting_debug_command,
	 DEBUG_OSPF_SPF_EXT);
EOLS	(debug_ip_ospf_spf_inter_eol, iprouting_debug_command,
	 DEBUG_OSPF_SPF_INTER);
EOLS	(debug_ip_ospf_spf_intra_eol, iprouting_debug_command,
	 DEBUG_OSPF_SPF_INTRA);
NUMBER	(debug_ip_ospf_spf_ext_acl, debug_ip_ospf_spf_ext_eol,
	 debug_ip_ospf_spf_ext_eol, OBJ(int,1), 1, MAXFASTACC,
	 "Access list");
KEYWORD	(debug_ip_ospf_spf_ext, debug_ip_ospf_spf_ext_acl,
	 debug_ip_ospf_spf_eol, "external", "OSPF spf external-route",
	 PRIV_OPR);
NUMBER	(debug_ip_ospf_spf_inter_acl, debug_ip_ospf_spf_inter_eol,
	 debug_ip_ospf_spf_inter_eol, OBJ(int,1), 1, MAXFASTACC,
	 "Access list");
KEYWORD	(debug_ip_ospf_spf_inter, debug_ip_ospf_spf_inter_acl,
	 debug_ip_ospf_spf_ext, "inter", "OSPF spf inter-route", PRIV_OPR);
NUMBER	(debug_ip_ospf_spf_intra_acl, debug_ip_ospf_spf_intra_eol,
	 debug_ip_ospf_spf_intra_eol, OBJ(int,1), 1, MAXFASTACC,
	 "Access list");
KEYWORD	(debug_ip_ospf_spf_intra, debug_ip_ospf_spf_intra_acl,
	 debug_ip_ospf_spf_inter, "intra", "OSPF spf inter-route", PRIV_OPR);
KEYWORD	(debug_ip_ospf_spf, debug_ip_ospf_spf_intra, debug_ip_ospf_tree,
	 "spf", "OSPF spf", PRIV_OPR);

/******************************************************************
 * debug ip ospf packet
 *
 */
EOLS	(debug_ip_ospf_packet_eol, iprouting_debug_command, DEBUG_OSPF_PACKET);
KEYWORD	(debug_ip_ospf_packet, debug_ip_ospf_packet_eol, debug_ip_ospf_spf,
	 "packet", "OSPF packets", PRIV_OPR);

/******************************************************************
 * debug ip ospf monitor
 *
 */
EOLS	(debug_ip_ospf_monitor_eol, iprouting_debug_command,
	 DEBUG_OSPF_MONITOR);
KEYWORD (debug_ip_ospf_monitor, debug_ip_ospf_monitor_eol,
	 debug_ip_ospf_packet,
	 "monitor", "OSPF spf monitoring", PRIV_OPR|PRIV_HIDDEN);

/******************************************************************
 * debug ip ospf flood
 *
 */
EOLS	(debug_ip_ospf_flood_eol, iprouting_debug_command,
	 DEBUG_OSPF_FLOOD);
NUMBER	(debug_ip_ospf_flood_acl, debug_ip_ospf_flood_eol,
	 debug_ip_ospf_flood_eol, OBJ(int,1), 1, MAXFASTACC,
	 "Access list");
KEYWORD	(debug_ip_ospf_flood, debug_ip_ospf_flood_acl, debug_ip_ospf_monitor,
	 "flood", "OSPF flooding", PRIV_OPR);

/******************************************************************
 * debug ip ospf events
 *
 */
EOLS	(debug_ip_ospf_events_eol, iprouting_debug_command, DEBUG_OSPF_EVENTS);
KEYWORD	(debug_ip_ospf_events, debug_ip_ospf_events_eol, debug_ip_ospf_flood,
	 "events", "OSPF events", PRIV_OPR);

/******************************************************************
 * debug ip ospf adj
 *
 */
EOLS	(debug_ip_ospf_adj_eol, iprouting_debug_command, DEBUG_OSPF_ADJ);
KEYWORD	(debug_ip_ospf_adj, debug_ip_ospf_adj_eol, debug_ip_ospf_events,
	 "adj", "OSPF adjacency events", PRIV_OPR);

/******************************************************************
 * debug ip ospf retransmission
 *
 */
EOLS	(debug_ip_ospf_retrans_eol, iprouting_debug_command,
	DEBUG_OSPF_RETRANS);
KEYWORD	(debug_ip_ospf_retrans, debug_ip_ospf_retrans_eol, debug_ip_ospf_adj,
	 "retransmission", "OSPF retransmission events", PRIV_OPR);

/******************************************************************
 * debug ip ospf
 *
 */
KEYWORD	(debug_ip_ospf, debug_ip_ospf_retrans, ALTERNATE,
	 "ospf", "OSPF information", PRIV_OPR);

#undef	ALTERNATE
#define	ALTERNATE	debug_ip_ospf
