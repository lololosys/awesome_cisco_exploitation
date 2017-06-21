/* $Id: exec_show_dlsw.h,v 3.4.20.4 1996/08/12 16:01:14 widmer Exp $
 * $Source: /release/112/cvs/Xsys/dlsw/exec_show_dlsw.h,v $
 *------------------------------------------------------------------
 * E X E C _ S H O W _ D L S W . H
 *
 * July 1994, Frank Bordonaro 
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: exec_show_dlsw.h,v $
 * Revision 3.4.20.4  1996/08/12  16:01:14  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.4.20.3  1996/07/19  03:55:38  athippes
 * CSCdi60758:  dlsw : duplicate circuit identifier
 * Branch: California_branch
 *
 * Revision 3.4.20.2  1996/05/30  14:43:45  sbales
 * CSCdi59064:  DLSw+: show commands need to be more usable
 *              modify options for "sh dls reach", "sh dls cir", "sh dls
 *              loc", modify formatting for "show dls peer"
 * Branch: California_branch
 *
 * Revision 3.4.20.1  1996/05/21  09:45:41  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.4  1996/01/29  07:08:35  hampton
 * Remove command number arguments passed to parse routines that only
 * perform a single function.  [CSCdi47717]
 *
 * Revision 3.3  1996/01/18  02:12:33  tli
 * CSCdi47063:  show ip cache cannot be restricted to subinterfaces
 * Change IFTYPE_ANY to IFTYPE_HWIDB
 *
 * Revision 3.2  1995/11/17  09:04:08  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:25:22  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  20:56:08  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/07/04  01:52:53  tli
 * CSCdi36508: Rationalize parser file placement, part I
 * Rationalize cls dev dlsw dspu filesys irix lat les lex llc2 parser pgen
 * pt servers smf smrp snmp src-36-c1000 src-4k-c4000 src-68-c3000
 * src-68-c4000 src-c4000 src-rsp srt tcl ts ui
 *
 * Revision 2.1  1995/06/07  22:23:39  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/******************************************************************
 * show dlsw {circuits | peers | reachability |
 *            fastcache | local-circuits}
 *
 * OBJ(int,1) = command type DLSW_SHOW_{CORE,PEER,CAP,CSM}  
 *
 * If command type "show dlsw reachability" 
 * ---------------------------------
 * options:
 * {mac {<macaddress>} | netbios {<name>} | <cr> for all}
 *
 * OBJ(int,2) = show reachability for mac addresses, netbios names, or both
 * OBJ(hwaddr,1) = mac address for which to show reachability
 * OBJ(string,1) = netbios name for which to show reachability
 *
 * If command is "show dlsw peers"
 * ---------------------------------
 * options:
 * {ip-address <ipaddress> | interface <name> | <cr> for all}
 *
 * OBJ(int,2) = show peer by ip-address, interface, all peers
 * OBJ(paddr,1) = ip address of remote peer to display
 * OBJ(idb,1) = pointer to hwidbtype for interface  
 *
 * If command is "show dlsw capabilities"
 * ---------------------------------
 * options:
 * {local | ip-address <ipaddress> | interface <name> | <cr> for all}
 *
 * OBJ(int,2) = show local peer cap, or 
 *              remote peer by ip-address, interface, all peers
 * OBJ(paddr,1) = ip address of remote peer to display
 * OBJ(idb,1) = pointer to hwidbtype for interface
 *
 * If command type is "show dlsw circuit" 
 * --------------------------------------
 * options:
 * {detail}
 * { <Ckt ID> | mac <mac-addr> | sap <sap-addr> | <cr> for all}
 *
 * OBJ(int,5) = {SHOW_DLSW_CIRCUIT_ALL, SHOW_DLSW_CIRCUIT_KEY, 
 *               SHOW_DLSW_CIRCUIT_MAC, SHOW_DLSW_CIRCUIT_SAP}
 * OBJ(int,6) = {0,1} - 1 means show detail
 * If OBJ(int,5) = SHOW_DLSW_CIRCUIT_KEY
 *     OBJ(int,3) = <Ckt ID>
 * If OBJ(int,5) = SHOW_DLSW_CIRCUIT_MAC
 *     OBJ(hwaddr,3) = <mac-address>
 * If OBJ(int,5) = SHOW_DLSW_CIRCUIT_SAP
 *     OBJ(int,3) = <sap-address>
 *
 * If command type is "show dlsw local-circuit"
 * --------------------------------------
 * options:
 * { <Ckt ID> | mac <mac-addr> | sap <sap-addr> | <cr> for all}
 *
 * OBJ(int,5) = {SHOW_DLSW_CIRCUIT_ALL, SHOW_DLSW_CIRCUIT_KEY, 
 *               SHOW_DLSW_CIRCUIT_MAC, SHOW_DLSW_CIRCUIT_SAP}
 * If OBJ(int,5) = SHOW_DLSW_CIRCUIT_KEY
 *     OBJ(int,3) = <Ckt ID>
 * If OBJ(int,5) = SHOW_DLSW_CIRCUIT_MAC
 *     OBJ(hwaddr,3) = <mac-address>
 * If OBJ(int,5) = SHOW_DLSW_CIRCUIT_SAP
 *     OBJ(int,3) = <sap-address>
 */

EOLNS	(show_dlsw_eol, show_dlsw);

HEXDIGIT(show_local_get_sap, show_dlsw_eol, no_alt, OBJ(int,3),
         0, 0xFF, "SAP value for which to show local circuits");

KEYWORD_ID(show_local_sap, show_local_get_sap, show_dlsw_eol,
           OBJ(int,5), DLSW_SHOW_CIRCUIT_SAP, "sap-value",
           "Display all local circuits using a specific SAP", PRIV_USER);

ENADDR  (show_local_get_mac, show_dlsw_eol, no_alt,
         OBJ(hwaddr,3), "MAC address for which to show local circuits");

KEYWORD_ID(show_local_mac, show_local_get_mac, show_local_sap,
           OBJ(int,5), DLSW_SHOW_CIRCUIT_MAC, "mac-address",
           "Display all local circuits using a specific MAC", PRIV_USER);

SET     (show_local_specific, show_dlsw_eol, OBJ(int,5), 
         DLSW_SHOW_CIRCUIT_KEY);

NUMBER(show_local_major, show_local_specific, show_local_mac,
       OBJ(int,3), 0, 0xFFFFFFFF,
       "Circuit ID for a specific local circuit");

SET     (show_local_init, show_local_major, OBJ(int,5), DLSW_SHOW_CIRCUIT_ALL);

KEYWORD_ID(show_local_ckt, show_local_init, no_alt,
           OBJ(int,1), DLSW_SHOW_LOC_CKT, "local-circuit",
           "Display DLSw local circuits", PRIV_USER);

KEYWORD_ID(show_dlsw_fast, show_dlsw_eol, show_local_ckt,
           OBJ(int,1), DLSW_SHOW_FAST, "fastcache",
           "Display DLSw fast cache for FST and Direct", PRIV_USER);

INTERFACE(show_int_name, show_dlsw_eol, no_alt, 
          OBJ(idb,1), IFTYPE_HWIDB     );

KEYWORD_ID(show_by_int, show_int_name, show_dlsw_eol,
           OBJ(int,2), DLSW_SHOW_BY_INT, common_str_interface,
           "Specify a remote peer by a direct interface", 
           PRIV_USER);

IPADDR  (show_ip_addr, show_dlsw_eol, no_alt,
         OBJ(paddr,1), "Remote peer's ip address");

KEYWORD_ID(show_by_ip, show_ip_addr, show_by_int,
           OBJ(int,2), DLSW_SHOW_BY_IP, "ip-address",
           "Specify a remote peer by it's ip address", PRIV_USER);

KEYWORD_ID(show_local, show_dlsw_eol, show_by_ip,
           OBJ(int,2), DLSW_SHOW_LOCAL, "local",
           "Specify the local DLSw peer", PRIV_USER);

SET     (show_cap_def, show_local, OBJ(int,2), DLSW_SHOW_ALL);

KEYWORD_ID(show_dlsw_cap, show_cap_def, show_dlsw_fast,
           OBJ(int,1), DLSW_SHOW_CAP, "capabilities",
           "Display DLSw capabilities information", PRIV_USER);

SET     (show_peer_def, show_by_ip, OBJ(int,2), DLSW_SHOW_ALL);

KEYWORD_ID(show_dlsw_peers, show_peer_def, show_dlsw_cap,
           OBJ(int,1), DLSW_SHOW_PEER, "peers",
           "Display DLSw peer information", PRIV_USER);

SET     (show_reach_name_valid, show_dlsw_eol, OBJ(int,4), 1);

STRING    (show_reach_name, show_reach_name_valid, show_dlsw_eol,
           OBJ(string,1), "Netbios name for which to check reachability");

SET     (show_reach_name_set, show_reach_name, OBJ(int,4), 0);

KEYWORD_ID(show_reach_netb, show_reach_name_set, show_dlsw_eol,
           OBJ(int,2), DLSW_SHOW_CSM_NETB, "netbios-names",
           "Display DLSw NetBIOS name reachability",
           PRIV_USER);

SET     (show_reach_address_valid, show_dlsw_eol, OBJ(int,4), 1);

ENADDR    (show_reach_address, show_reach_address_valid, show_dlsw_eol,
           OBJ(hwaddr,1), "MAC address for which to check reachability");

SET     (show_reach_address_set, show_reach_address, OBJ(int,4), 0);

KEYWORD_ID(show_reach_mac, show_reach_address_set, show_reach_netb,
           OBJ(int,2), DLSW_SHOW_CSM_MAC, "mac-address",
           "Display DLSw mac address reachability",
           PRIV_USER);

KEYWORD_ID(show_reach_remote, show_reach_mac, show_reach_mac,
           OBJ(int,3), DLSW_SHOW_CSM_REMOTE, "remote",
           "Display DLSw remote reachability", PRIV_USER);

KEYWORD_ID(show_reach_local, show_reach_mac, show_reach_remote,
           OBJ(int,3), DLSW_SHOW_CSM_LOCAL, "local",
           "Display DLSw local reachability", PRIV_USER);

SET     (show_reach_def2, show_reach_local, OBJ(int,3), DLSW_SHOW_CSM_ALL);
SET     (show_reach_def1, show_reach_def2, OBJ(int,2), DLSW_SHOW_CSM_ALL);

KEYWORD_ID(show_dlsw_path, show_reach_def1, show_dlsw_peers,
           OBJ(int,1), DLSW_SHOW_CSM, "reachability",
           "Display DLSw reachability information",
           PRIV_USER);

HEXDIGIT(show_ckt_get_sap, show_dlsw_eol, no_alt, OBJ(int,3),
         0, 0xFF, "SAP value for which to show remote circuits");

KEYWORD_ID(show_ckt_sap, show_ckt_get_sap, show_dlsw_eol,
           OBJ(int,5), DLSW_SHOW_CIRCUIT_SAP, "sap-value",
           "Display all remote circuits using a specific SAP", PRIV_USER);

ENADDR  (show_ckt_get_mac, show_dlsw_eol, no_alt,
         OBJ(hwaddr,3), "MAC address for which to show remote circuits");

KEYWORD_ID(show_ckt_mac, show_ckt_get_mac, show_ckt_sap,
           OBJ(int,5), DLSW_SHOW_CIRCUIT_MAC, "mac-address",
           "Display all remote circuits using a specific MAC", PRIV_USER);

SET     (show_ckt_specific, show_dlsw_eol, OBJ(int,5), DLSW_SHOW_CIRCUIT_KEY);

NUMBER(show_ckt_major, show_ckt_specific, show_ckt_mac,
       OBJ(int,3), 0, 0xFFFFFFFF,
       "Circuit ID for a specific remote circuit");
 
SET     (show_ckt_init, show_ckt_major, OBJ(int,5), DLSW_SHOW_CIRCUIT_ALL);

KEYWORD_ID(show_ckt_detail, show_ckt_init, show_ckt_init,
           OBJ(int,6), 1, "detail", "Display full remote circuit details",
           PRIV_USER);

SET     (show_ckt_detail_init, show_ckt_detail, OBJ(int,6), 0);

KEYWORD_ID(show_dlsw_circuits, show_ckt_detail_init, show_dlsw_path,
           OBJ(int,1), DLSW_SHOW_CORE, "circuits", 
           "Display DLSw circuit information", PRIV_USER);

KEYWORD (show_dlsw, show_dlsw_circuits, NONE,
	 "dlsw", "Data Link Switching information",
	 PRIV_USER|PRIV_USER_HIDDEN);

TEST_BOOLEAN(show_dlsw_test, show_dlsw, NONE, ALTERNATE, bridge_enable);

#undef	ALTERNATE
#define	ALTERNATE	show_dlsw_test

