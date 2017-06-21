/* $Id: exec_clear_lane.h,v 3.2.60.4 1996/08/12 16:04:20 widmer Exp $
 * $Source: /release/112/cvs/Xsys/lane/exec_clear_lane.h,v $
 *------------------------------------------------------------------
 * exec_clear_lane.h -- 
 *
 * January 1995,  Brad Benson
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: exec_clear_lane.h,v $
 * Revision 3.2.60.4  1996/08/12  16:04:20  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.2.60.3  1996/05/23  14:36:53  kjadams
 * CSCdi55067:  Cant clear LEC from LESs registered list by the route
 * descriptor
 * Branch: California_branch
 * parser command was missing for clearing RD's from registered list.
 *
 * Revision 3.2.60.2  1996/05/21  09:54:14  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.2.60.1  1996/05/09  14:32:04  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.2.84.1  1996/04/27  07:05:59  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.2.74.2  1996/04/26  00:32:27  michellw
 * Branch: LE_Cal_V112_0_2_branch
 * Add a new exec command to clear the LEC's join-attempt-cnt
 * globally or per sub-interface.
 * "clear lane client join-cnt [interface SUBINTERFACE]"
 *
 * Revision 3.2.74.1  1996/04/08  01:56:25  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.2.64.1  1996/03/22  22:53:38  rlowe
 * Non-sync of Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.2.50.1  1996/03/05  06:42:26  rlowe
 * Apply LANE content to merge branch.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 3.2.38.1  1996/02/27  21:09:48  cakyol
 * non sync sync of LE_Cal_V111_0_16_branch to V111_1_0_3 yielding
 * LE_Cal_V111_1_0_3_branch
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 3.2.14.1  1996/01/17  20:26:16  cakyol
 * - Index_add now returns the inserted element or the one already
 *   present.  More convenient for handling duplicates.
 * - Tidy up the parser help strings that involve atm address
 *   template.
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 3.2  1995/11/17  17:49:52  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:22:30  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/07/10  21:46:15  nfinn
 * CSCdi36880:  clear lane uses lec-nsap instead of client-atm-address
 * keyword
 *
 * Revision 2.3  1995/06/23  22:17:54  cakyol
 * CSCdi35865:  No error or warning messages displayed for clear atm
 * server...
 * fixed.  Most of the error messages were already there but they were
 * protected buginf's.  I changed them to direct printf's for
 * immediate response.
 *
 * Revision 2.2  1995/06/21  18:16:05  widmer
 * CSCdi36227:  Fix parse chain alternates
 *
 * Revision 2.1  1995/06/07  21:18:33  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

LINK_TRANS(clear_lane_extend_here, no_alt);


/**********************************************************
**
** clear lane client join-cnt [interface SUBINTERFACE]
**
** OBJ (idb, 1)	= Interface/Subinterface
*/

EOLS	(clear_lec_join_eol, lec_exec_clear_join_cnt, 
	 CLEAR_LANE_LEC_JOIN_CNT_ALL);

EOLS	(clear_lec_join_int_eol, lec_exec_clear_join_cnt, 
	 CLEAR_LANE_LEC_JOIN_CNT_INT);

INTERFACE (clear_lec_join_cnt_get_int, clear_lec_join_int_eol, no_alt,
	   OBJ(idb,1), (IFTYPE_ATM | IFTYPE_SUBIF));

KEYWORD (clear_lec_join_cnt_test_int, clear_lec_join_cnt_get_int,
	 clear_lec_join_eol, common_str_interface,
	 "select lec by interface", PRIV_OPR);

KEYWORD (clear_lane_lec_join, clear_lec_join_cnt_test_int, no_alt,
	 "join-cnt", "Join Attempt Counter", 
	 PRIV_OPR | PRIV_NONVGEN);

KEYWORD (clear_lane_lec, clear_lane_lec_join, no_alt,
	 "client", "Client", PRIV_OPR | PRIV_NONVGEN);


/*
 * clear lane server {interface SUB-INTERFACE | name ELAN-NAME}
 *       [mac-address MAC-ADDRESS | client-atm-address NSAP | lecid LECID 
 *        route-desc SEGMENT bridge BRIDGE]
 *  OBJ (idb, 1) sub-interface   |   obj (string, 1) elan-name
 *  OBJ (hwaddr, 2) mac-address | OBJ (hwaddr, 2) nsap | OBJ (int, 2) lecid
 *  OBJ (int, 1) segment and OBJ(int, 2) bridge 
 */
 
EOLS	(clear_lane_les_name_eol, lsv_clear_name_commands, CLEAR_LANE_LES_ALL);
 
EOLS	(clear_lane_les_name_rd_eol, lsv_clear_name_commands, CLEAR_LANE_LES_RD);
 
NUMBER	(clear_lane_les_name_get_br, clear_lane_les_name_rd_eol, no_alt,
	 OBJ(int,2), 1, LANE_MAX_BRIDGENUM, "bridge number");
 
KEYWORD	(clear_lane_les_name_br_kw,
	 clear_lane_les_name_get_br, no_alt, 
	 "bridge", "bridge number of route descriptor", PRIV_OPR);
 
NUMBER	(clear_lane_les_name_get_seg, clear_lane_les_name_br_kw, no_alt,
	 OBJ(int,1), 1, LANE_MAX_SEGMENTID, "segment number");
 
KEYWORD	(clear_lane_les_name_rd_kw,
	 clear_lane_les_name_get_seg, clear_lane_les_name_eol,
	 "route-desc", "choose client by route descriptor", PRIV_OPR);
 
EOLS	(clear_lane_les_name_lecid_eol, lsv_clear_name_commands,
	 CLEAR_LANE_LES_LECID);
 
NUMBER	(clear_lane_les_name_get_lecid, clear_lane_les_name_lecid_eol, no_alt,
	 OBJ(int,2), 1, LANE_MAX_LECID, "LEC ID");
 
KEYWORD	(clear_lane_les_name_lecid,
	 clear_lane_les_name_get_lecid, clear_lane_les_name_rd_kw, 
	 "lecid", "choose client by LECID", PRIV_OPR);
 
EOLS	(clear_lane_les_name_nsap_eol, lsv_clear_name_commands,
	 CLEAR_LANE_LES_NSAP);
 
ATMNSAPADDR (clear_lane_les_name_get_nsap,
	     clear_lane_les_name_nsap_eol, no_alt,
	     OBJ(hwaddr,2), "Client (LEC) ATM address");
 
KEYWORD	(clear_lane_les_name_nsap,
	 clear_lane_les_name_get_nsap, clear_lane_les_name_lecid,
	 "client-atm-address", "choose client by ATM address", PRIV_OPR);
 
EOLS	(clear_lane_les_name_mac_eol, lsv_clear_name_commands,
	 CLEAR_LANE_LES_MAC);
 
ENADDR	(clear_lane_les_name_get_mac, clear_lane_les_name_mac_eol, no_alt,
	 OBJ(hwaddr,2), "6-octet LEC mac address");
 
KEYWORD	(clear_lane_les_name_mac,
	 clear_lane_les_name_get_mac, clear_lane_les_name_nsap,
	 "mac-address", "choose client by MAC address", PRIV_OPR);
 
STRING	(clear_lane_les_get_name, clear_lane_les_name_mac, no_alt,
         OBJ(string,1), "Name of the emulated LAN");
 
KEYWORD	(clear_lane_les_name, clear_lane_les_get_name, no_alt,
         "name", "choose Server (LES) by LAN name", PRIV_OPR);
 
EOLS	(clear_lane_les_int_eol, lsv_clear_int_commands, CLEAR_LANE_LES_ALL);
 
EOLS	(clear_lane_les_int_rd_eol, lsv_clear_int_commands, CLEAR_LANE_LES_RD);
 
NUMBER	(clear_lane_les_int_get_br, clear_lane_les_int_rd_eol, no_alt,
	 OBJ(int,2), 1, LANE_MAX_BRIDGENUM, "bridge");
 
KEYWORD	(clear_lane_les_int_br_kw,
	 clear_lane_les_int_get_br, no_alt, 
	 "bridge", "bridge number", PRIV_OPR);
 
NUMBER	(clear_lane_les_int_get_seg, clear_lane_les_int_br_kw, no_alt,
	 OBJ(int,1), 1, LANE_MAX_SEGMENTID, "segment");
 
KEYWORD	(clear_lane_les_int_rd_kw,
	 clear_lane_les_int_get_seg, clear_lane_les_int_eol,
	 "route-desc", "choose client by route descriptor", PRIV_OPR);
 
EOLS	(clear_lane_les_int_lecid_eol, lsv_clear_int_commands,
	 CLEAR_LANE_LES_LECID);
 
NUMBER	(clear_lane_les_int_get_lecid, clear_lane_les_int_lecid_eol, no_alt,
	 OBJ(int,2), 1, LANE_MAX_LECID, "LEC ID");
 
KEYWORD	(clear_lane_les_int_lecid,
	 clear_lane_les_int_get_lecid, clear_lane_les_int_rd_kw,
	 "lecid", "choose client by LECID", PRIV_OPR);
 
EOLS	(clear_lane_les_int_nsap_eol, lsv_clear_int_commands,
	 CLEAR_LANE_LES_NSAP);
 
ATMNSAPADDR (clear_lane_les_int_get_nsap,
	     clear_lane_les_int_nsap_eol, no_alt,
	     OBJ(hwaddr,2), "Client (LEC) ATM address");
 
KEYWORD	(clear_lane_les_int_nsap,
	 clear_lane_les_int_get_nsap, clear_lane_les_int_lecid,
	 "client-atm-address", "choose client by ATM address", PRIV_OPR);
 
EOLS	(clear_lane_les_int_mac_eol, lsv_clear_int_commands,
	 CLEAR_LANE_LES_MAC);
 
ENADDR	(clear_lane_les_int_get_mac, clear_lane_les_int_mac_eol, no_alt,
	 OBJ(hwaddr,2), "6-octet LEC mac address");
 
KEYWORD	(clear_lane_les_int_mac,
	 clear_lane_les_int_get_mac, clear_lane_les_int_nsap,
	 "mac-address", "choose client by MAC address", PRIV_OPR);
 
INTERFACE (clear_lane_les_get_int, clear_lane_les_int_mac, no_alt,
           OBJ(idb,1), (IFTYPE_ATM | IFTYPE_SUBIF));
 
KEYWORD	(clear_lane_les_int, clear_lane_les_get_int, clear_lane_les_name,
         common_str_interface, "choose Server (LES) by interface", PRIV_OPR);
 
KEYWORD	(clear_lane_les, clear_lane_les_int, clear_lane_lec,
         "server", "Lan Emulation server (LES)", PRIV_OPR);


/**************************************************************************
**
** clear lane le-arp [interface SUBINTERFACE [MAC-ADDRESS]] |
**                   [name ELAN-NAME [MAC-ADDRESS]]
**
** OBJ (idb, 1)    = Interface/Subinterface
** OBJ (string, 1) = ELAN name
** OBJ (hwaddr, 1) = MAC address
** OBJ (int, 1)    = CLEAR_LE_ARP_{ALL | NAME | INT}
** OBJ (int, 2)    = {CLEAR_LE_ARP_NO_MAC | CLEAR_LE_ARP_MAC}
*/

EOLNS (clear_le_arp_eols, lec_exec_clear_le_arp);

SET (clear_le_arp_mac, clear_le_arp_eols, OBJ(int,2), CLEAR_LE_ARP_MAC);

ENADDR (clear_le_arp_test_mac, clear_le_arp_mac, clear_le_arp_eols,
        OBJ(hwaddr,1), "specific MAC address to clear");

STRING (clear_le_arp_get_name, clear_le_arp_test_mac, no_alt,
        OBJ(string,1), "Name of the Emulated LAN");

KEYWORD_ID (clear_le_arp_test_name, clear_le_arp_get_name,
            clear_le_arp_eols, OBJ(int,1), CLEAR_LE_ARP_BY_NAME,
	    "name", "select entries by ELAN name", PRIV_OPR);

INTERFACE (clear_le_arp_get_int, clear_le_arp_test_mac, no_alt,
           OBJ(idb,1), (IFTYPE_ATM | IFTYPE_SUBIF));

KEYWORD_ID (clear_le_arp_test_int, clear_le_arp_get_int,
            clear_le_arp_test_name, OBJ(int,1), CLEAR_LE_ARP_BY_INT,
            common_str_interface, "select entries by interface", PRIV_OPR);

KEYWORD (clear_lane_le_arp, clear_le_arp_test_int, clear_lane_les,
	 "le-arp", "Clear LE-ARP cache entry or entries", PRIV_OPR);

/**************************************************************************
**
** clear lane 
**
*/

KEYWORD (clear_lane, clear_lane_le_arp, ALTERNATE,
	 "lane", "lane", PRIV_OPR);

#undef	ALTERNATE
#define	ALTERNATE	clear_lane
