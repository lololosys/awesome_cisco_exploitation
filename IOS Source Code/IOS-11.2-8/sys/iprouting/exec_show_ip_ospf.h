/* $Id: exec_show_ip_ospf.h,v 3.2.60.3 1996/05/21 09:53:14 thille Exp $
 * $Source: /release/112/cvs/Xsys/iprouting/exec_show_ip_ospf.h,v $
 *------------------------------------------------------------------
 * OSPF show commands
 *
 * May 1993, Robert Widmer
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: exec_show_ip_ospf.h,v $
 * Revision 3.2.60.3  1996/05/21  09:53:14  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.2.60.2  1996/04/20  08:03:44  myeung
 * CSCdi54385:  Add show ip ospf maxage for debugging MAXAGE LSA problem
 * Branch: California_branch
 *
 * Revision 3.2.60.1  1996/04/16  19:01:30  rchandra
 * CSCdi54830:  IP features commit
 * Branch: California_branch
 *
 * Revision 3.2  1995/11/17  17:33:27  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:07:22  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/01  12:16:20  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize iprouting
 *
 * Revision 2.1  1995/06/07  22:24:33  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/******************************************************************
 * show ip ospf (default)
 */
EOLS	(show_ip_ospf_eol, ospf_show_command, OSPF_DEFAULT);
EQUAL	(show_ip_ospf_eoltest, show_ip_ospf_eol, no_alt, OBJ(int,5), FALSE);

/*
 * The commands below would need pdb and area id.
 */

/******************************************************************
 * ... database [{router | network | summary | asbr-summary |
 *                external | nssa-external} [IP-ADDR] [internal] |
 *               database-summary]
 *
 * OBJ(int,3)  = database type code (LSA_TYPE_...) or 0
 * OBJ(paddr,1) = optional LSA ID
 * OBJ(int,4) = TRUE if internal
 */
EOLS	(show_ip_ospf_database_eol, ospf_show_command, OSPF_DATABASE);

KEYWORD_ID (show_ip_ospf_database_internal, show_ip_ospf_database_eol,
	    show_ip_ospf_database_eol, OBJ(int,4), TRUE,
	    "internal", "Internal LSA information", PRIV_USER);

IPADDR	(show_ip_ospf_database_ipaddr, show_ip_ospf_database_internal,
	 show_ip_ospf_database_internal, OBJ(paddr,1),
	 "Link state ID (as an IP address)");

EOLS	(show_ip_ospf_database_summary_eol, ospf_show_command, OSPF_DATABASE_SUMMARY);
KEYWORD (show_ip_ospf_database_summary, show_ip_ospf_database_summary_eol,
	 show_ip_ospf_database_eol, "database-summary", "Summary of database", 
	 PRIV_USER);
KEYWORD_ID (show_ip_ospf_nssa_external, show_ip_ospf_database_ipaddr,
	    show_ip_ospf_database_summary, OBJ(int,3), LSA_TYPE_7_ASE,
	    "nssa-external", "NSSA External link states", PRIV_USER);
KEYWORD_ID (show_ip_ospf_external, show_ip_ospf_database_ipaddr,
	    show_ip_ospf_nssa_external, OBJ(int,3), LSA_TYPE_ASE,
	    "external", "External link states", PRIV_USER);
KEYWORD_ID (show_ip_ospf_asbrsum, show_ip_ospf_database_ipaddr,
	    show_ip_ospf_external, OBJ(int,3), LSA_TYPE_SUM_ASB,
	    "asbr-summary", "ASBR summary link states", PRIV_USER);
KEYWORD_ID (show_ip_ospf_netsum, show_ip_ospf_database_ipaddr,
	    show_ip_ospf_asbrsum, OBJ(int,3), LSA_TYPE_SUM_NET,
	    "summary", "Network summary link states", PRIV_USER);
KEYWORD_ID (show_ip_ospf_network, show_ip_ospf_database_ipaddr,
	    show_ip_ospf_netsum, OBJ(int,3), LSA_TYPE_NET,
	    "network", "Network link states", PRIV_USER);
KEYWORD_ID (show_ip_ospf_router, show_ip_ospf_database_ipaddr,
	    show_ip_ospf_network, OBJ(int,3), LSA_TYPE_RTR,
	    "router", "Router link states", PRIV_USER);
KEYWORD_ID (show_ip_ospf_database, show_ip_ospf_router,
	    show_ip_ospf_eoltest, OBJ(int,3), 0,
	    "database", "Database summary", PRIV_USER);

/*
 * The commands below would need pdb but not area id.
 */

/******************************************************************
 * ... maxage-list
 */
EOLS	(show_ip_ospf_maxage_eol, ospf_show_command, OSPF_MAXAGE_LIST);
KEYWORD (show_ip_ospf_maxage, show_ip_ospf_maxage_eol,
	 show_ip_ospf_database, "maxage-list", 
	 "Maxage List", PRIV_HIDDEN|PRIV_OPR);

/******************************************************************
 * ... delete-list
 */
EOLS	(show_ip_ospf_delete_eol, ospf_show_command, OSPF_DELETE_LIST);
KEYWORD (show_ip_ospf_delete, show_ip_ospf_delete_eol,
	 show_ip_ospf_maxage, "delete-list", 
	 "Delete List", PRIV_HIDDEN|PRIV_OPR);

/******************************************************************
 * ... retransmission-list <nbr-router-id> <interface>
 *
 * OBJ(paddr,1) = nbr-router-id
 * OBJ(idb,1) = interface
 */
EOLS	(show_ip_ospf_retrans_eol, show_ospf_internal_list, OSPF_RETX_LIST);
INTERFACE(show_ip_ospf_retrans_interface, show_ip_ospf_retrans_eol,
	  no_alt, OBJ(idb,1), IFTYPE_ANYSUB);
IPADDR	(show_ip_ospf_retrans_ipaddr, show_ip_ospf_retrans_interface,
	 no_alt, OBJ(paddr,1), "neighbor router ID");
KEYWORD (show_ip_ospf_retrans, show_ip_ospf_retrans_ipaddr,
	 show_ip_ospf_delete, "retransmission-list", 
	 "Link state retransmission list", PRIV_OPR);

/******************************************************************
 * ... request-list <nbr-router-id> <interface>
 *
 * OBJ(paddr,1) = nbr-router-id
 * OBJ(idb,1) = interface
 */
EOLS	(show_ip_ospf_request_eol, show_ospf_internal_list, OSPF_REQ_LIST);
INTERFACE(show_ip_ospf_request_interface, show_ip_ospf_request_eol,
	  no_alt, OBJ(idb,1), IFTYPE_ANYSUB);
IPADDR	(show_ip_ospf_request_ipaddr, show_ip_ospf_request_interface,
	 no_alt, OBJ(paddr,1), "neighbor router ID");
KEYWORD (show_ip_ospf_request, show_ip_ospf_request_ipaddr,
	 show_ip_ospf_retrans, "request-list", 
	 "Link state request list", PRIV_OPR);

/******************************************************************
 * ... statistic
 *
 */
EOLS	(show_ip_ospf_stat_eol, ospf_show_command, OSPF_SPF_STAT);
KEYWORD (show_ip_ospf_stat, show_ip_ospf_stat_eol,
	 show_ip_ospf_request, "statistic", 
	 "Various OSPF Statistic", PRIV_HIDDEN|PRIV_OPR);

/******************************************************************
 * ... bad-checksum
 */
EOLS	(show_ip_ospf_bad_cksum_eol, ospf_show_command, OSPF_BAD_CHECKSUM_QUEUE);
KEYWORD (show_ip_ospf_bad_cksum, show_ip_ospf_bad_cksum_eol,
	 show_ip_ospf_stat,
	 "bad-checksum", "Bad ospf checksum packets queue", PRIV_HIDDEN);

/******************************************************************
 * ... border-routers
 */
EOLS	(show_ip_ospf_border_eol, ospf_show_command, OSPF_BORDER_ROUTER);
KEYWORD (show_ip_ospf_border, show_ip_ospf_border_eol,
	 show_ip_ospf_bad_cksum, "border-routers", 
	 "Border and Boundary Router Information", PRIV_USER);

/******************************************************************
 * ... summary-address
 */
EOLS	(show_ip_ospf_summary_address_eol, ospf_show_command, OSPF_SUMMARY_ADDRESS);
KEYWORD (show_ip_ospf_summary_address, show_ip_ospf_summary_address_eol,
	 show_ip_ospf_border, "summary-address", 
	 "Summary-address redistribution Information", PRIV_USER);

/******************************************************************
 * ... neighbor [<interface>] [<nbr-router-id>]
 *
 * OBJ(idb,1) = interface
 * OBJ(paddr,1) = address  
 * OBJ(int,4) = TRUE for detail
 */
EOLS	(show_ip_ospf_neighbor_eol, ospf_show_command, OSPF_NEIGHBOR);
IPADDR_NAME (show_ip_ospf_neighbor_addr, show_ip_ospf_neighbor_eol,
	     NONE, OBJ(paddr,1), "Neighbor ID");
TEST_MULTIPLE_FUNCS(sipon_test, show_ip_ospf_neighbor_addr,
		    NONE, show_ip_ospf_neighbor_eol);
INTERFACE(show_ip_ospf_neighbor_int, sipon_test,
	  sipon_test, OBJ(idb,1), IFTYPE_ANYSUB);
EOLS	(show_ip_ospf_neighbor_detail_eol, ospf_show_command, OSPF_NEIGHBOR);
KEYWORD_ID (show_ip_ospf_neighbor_detail, show_ip_ospf_neighbor_detail_eol,
	    show_ip_ospf_neighbor_int, OBJ(int,4), TRUE,
	    "detail", "detail of all neighbors", PRIV_USER);
KEYWORD (show_ip_ospf_neighbor, show_ip_ospf_neighbor_detail,
	 show_ip_ospf_summary_address, "neighbor", "Neighbor list", PRIV_USER);

/******************************************************************
 * ... virtual-links
 */
EOLS	(show_ip_ospf_vlinks_eol, ospf_show_command, OSPF_VLINKS);
KEYWORD (show_ip_ospf_vlinks, show_ip_ospf_vlinks_eol,
	 show_ip_ospf_neighbor, "virtual-links", 
	 "Virtual link information", PRIV_USER);

/*
 * The commands below don't need pdb and area id.
 */

/******************************************************************
 * ... interface [<interface>]
 */
EOLS	(show_ip_ospf_interface_eol, ospf_show_command, OSPF_INTERFACE);
INTERFACE(show_ip_ospf_interface_int, show_ip_ospf_interface_eol,
	  show_ip_ospf_interface_eol, OBJ(idb,1), IFTYPE_ANYSUB);
KEYWORD (show_ip_ospf_interface, show_ip_ospf_interface_int,
	 show_ip_ospf_vlinks, common_str_interface, 
	 "Interface information", PRIV_USER);

/******************************************************************
 * ... events
 */
EOLS	(show_ip_ospf_events_eol, ospf_show_command, OSPF_EVENTS);
KEYWORD (show_ip_ospf_events, show_ip_ospf_events_eol,
	 show_ip_ospf_interface, "events", 
	 "OSPF event information", PRIV_USER | PRIV_HIDDEN);

/******************************************************************
 * show ip ospf [<as number>] [<area id>] ...
 *
 * It starts all ospf show commands.
 * OBJ(int,1) = process id, 0 if not set.
 * OBJ(int,2) and OBJ(paddr,2) = area id
 * OBJ(int,5) = TRUE if area id set
 *
 * show_ip_ospf_events starts the list of command which don't need pdb 
 * and area id
 * show_ip_ospf_vlinks starts the list of commands which would need 
 * pdb but not area id
 * show_ip_ospf_database starts the list of commands which would 
 * need pdb and area id.
 * The list of commands end with the default command - show ip ospf
 *
 */
SET (show_ip_ospf_areaid_set, show_ip_ospf_database, OBJ(int,5), TRUE);
NUMBER (show_ip_ospf_areaid_num, show_ip_ospf_areaid_set, 
	show_ip_ospf_vlinks, OBJ(int,2), 0, -1, 
	"OSPF area ID as a decimal number");
IPADDR (show_ip_ospf_areaid, show_ip_ospf_areaid_set, show_ip_ospf_areaid_num,
	OBJ(paddr,2), "OSPF area ID in IP address format");
NUMBER (show_ip_ospf_process_id, show_ip_ospf_areaid, show_ip_ospf_events,
	OBJ(int,1), 1, -1, "Process ID number");
KEYWORD (show_ip_ospf, show_ip_ospf_process_id, ALTERNATE,
	 "ospf", "OSPF information", PRIV_USER);

#undef	ALTERNATE
#define	ALTERNATE	show_ip_ospf
