/* $Id: exec_show_lane.h,v 3.2.62.1 1996/05/21 09:54:15 thille Exp $
 * $Source: /release/112/cvs/Xsys/lane/exec_show_lane.h,v $
 *------------------------------------------------------------------
 * parser/exec_show_lane.h: SHOW LANE command parsing
 *
 * October, 1994, Chris Parrott
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: exec_show_lane.h,v $
 * Revision 3.2.62.1  1996/05/21  09:54:15  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.2  1995/11/17  17:49:56  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:22:32  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/07/18  01:09:46  cakyol
 * CSCdi37230:  lane default-atm-address command should be a show command
 * - fixed.  Here is the summary:
 *
 *         show lane default-atm-address
 *                 prints the possible lane components on ALL atm
 *                 interfaces.
 *
 *         show lane default-atm-address interface atm1/0
 *                 prints the possible lane components on the
 *                 specified main-interface.
 *
 *         show lane default-atm-address interface atm1/0.1
 *                 prints the possible lane components on the
 *                 specified sub-interface.
 *
 * Revision 2.1  1995/06/07  21:18:38  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
**
** show lane ENTITY [interface SUBINTERFACE | name ELAN-NAME] [brief]
**
** ENTITY = [null]|server|bus
**
** OBJ (idb, 1)    = Interface/Subinterface
** OBJ (string, 1) = Symbolic name of the emulated LAN.
** OBJ (int, 1)    = value for entity specified
** OBJ (int, 2)    = flags (interface, name, brief)
*/

EOLS	(show_lane_eol, lane_show_xxx_commands, SHOW_LANE_ENTITY);

KEYWORD_OR (show_lane_maybe_brief, show_lane_eol, show_lane_eol,
            OBJ (int, 2), SHOW_LANE_BRIEF,
            "brief", "skip per connection & per client info", PRIV_USER);

STRING	(show_lane_get_name, show_lane_maybe_brief, no_alt,
	 OBJ (string, 1), "Name of the emulated LAN");

KEYWORD_OR (show_lane_name, show_lane_get_name, show_lane_maybe_brief,
            OBJ (int, 2), SHOW_LANE_NAME,
            "name", "identified by ELAN name", PRIV_USER);

INTERFACE (show_lane_get_int, show_lane_maybe_brief, no_alt,
	   OBJ (idb, 1), IFTYPE_ATM | IFTYPE_SUBIF);

KEYWORD_OR (show_lane_int, show_lane_get_int, show_lane_name,
            OBJ (int, 2), SHOW_LANE_INT,
	    common_str_interface, "identified by interface", PRIV_USER);

SET (show_lane_all, show_lane_int, OBJ(int, 1), SHOW_LANE_ALL);

KEYWORD_ID (show_lane_bus, show_lane_int, show_lane_all,
            OBJ(int, 1), SHOW_LANE_BUS,
            "bus", "LAN Emulation Broadcast/Unknown Server", PRIV_USER);

KEYWORD_ID (show_lane_server, show_lane_int, show_lane_bus,
            OBJ(int, 1), SHOW_LANE_SERVER,
            "server", "LAN Emulation Server", PRIV_USER);

/******************************************************************
**
** show lane database [CONFIG-TABLE [brief]]
**
** OBJ (string, 1) = table name
** OBJ (int, 2) = flags (specific, brief)
*/

EOLS (show_lane_cfgtbl_eol, lecs_show_function, SHOW_LANE_CFGTBL);

KEYWORD_OR (show_lane_cfgtbl_brief, show_lane_cfgtbl_eol,
	    show_lane_cfgtbl_eol,
	    OBJ (int, 2), SHOW_LANE_BRIEF,
	    "brief", "display more detailed information", PRIV_USER);

SET (show_lane_cfgtbl_setname, show_lane_cfgtbl_brief,
     OBJ (int, 2), SHOW_LANE_SPECIFIC);

STRING (show_lane_cfgtbl_name, show_lane_cfgtbl_setname, 
	show_lane_cfgtbl_eol,
	OBJ (string, 1), "Name of the configuration table");

KEYWORD (show_lane_cfgtbl_kw, show_lane_cfgtbl_name, show_lane_server,
	    "database", "Lan Emulation LECS Configuration Table/s", 
	    PRIV_USER);

/******************************************************************
**
** show lane default-atm-addresses interface INTERFACE
**
** OBJ (idb, 1) = which interface
*/

EOLS (lane_def_addresses_eol, lane_show_auto_addresses, SHOW_LANE_ADDR_INT);

EOLS (lane_def_addresses_eol_noint, lane_show_auto_addresses, 
      SHOW_LANE_ADDRESSES);

INTERFACE (lane_def_addresses_interface, lane_def_addresses_eol, no_alt,
	   OBJ(idb, 1), IFTYPE_ATM | IFTYPE_SUBIF);

KEYWORD (lane_def_addresses_int, lane_def_addresses_interface, 
	 lane_def_addresses_eol_noint,
	 common_str_interface, "which interface the command pertains to",
	 PRIV_USER);

KEYWORD (lane_def_addresses, lane_def_addresses_int, show_lane_cfgtbl_kw, 
         "default-atm-addresses", "show lane components' auto atm addresses",
	 PRIV_USER);

/******************************************************************
**
** show lane config [interface INTERFACE] [brief]
**
** OBJ (int, 2) = flags (interface, brief)
** OBJ (idb, 1) = which interface
*/

EOLS (show_lane_lecs_eol, lecs_show_function, SHOW_LANE_LECS);

KEYWORD_OR (show_lane_lecs_maybe_brief, show_lane_lecs_eol, show_lane_lecs_eol,
            OBJ (int, 2), SHOW_LANE_BRIEF,
            "brief", "skip detailed information", PRIV_USER);

INTERFACE (show_lane_lecs_interface, show_lane_lecs_maybe_brief, no_alt,
	   OBJ (idb, 1), IFTYPE_ATM);

KEYWORD_OR (show_lane_lecs_int_kw, show_lane_lecs_interface, 
	    show_lane_lecs_maybe_brief, 
	    OBJ (int, 2), SHOW_LANE_INT,
	    common_str_interface, "Interface to display Lecs connections on", 
	    PRIV_USER);

KEYWORD (show_lane_lecs, show_lane_lecs_int_kw, lane_def_addresses, 
         "config", "Lan Emulation Configuration Server", PRIV_USER);

/******************************************************************
**
** show lane le-arp [interface SUBINTERFACE | name ELAN-NAME]
**
** OBJ (idb, 1) = Interface/Subinterface
** OBJ (string, 1) = Symbolic name of the emulated LAN.
** OBJ (int, 2) = flags (interface, name)
*/

EOLS	(show_lane_le_arp_eol, lec_show_commands, SHOW_LANE_LE_ARP);

STRING	(show_lane_le_arp_get_name, show_lane_le_arp_eol, no_alt,
	 OBJ (string, 1), "Name of the emulated LAN");

KEYWORD_OR (show_lane_le_arp_name, show_lane_le_arp_get_name,
            show_lane_le_arp_eol,
            OBJ(int, 2), SHOW_LANE_NAME,
            "name", "LE-ARP by LAN name", PRIV_USER);

INTERFACE (show_lane_le_arp_get_int, show_lane_le_arp_eol, no_alt,
	   OBJ(idb,1), (IFTYPE_ATM | IFTYPE_SUBIF));

KEYWORD_OR (show_lane_le_arp_int, show_lane_le_arp_get_int,
            show_lane_le_arp_name,
            OBJ(int, 2), SHOW_LANE_INT,
            common_str_interface, "LE-ARP by interface", PRIV_USER);

KEYWORD (show_lane_le_arp, show_lane_le_arp_int, show_lane_lecs,
	 "le-arp", "Lan Emulation ARP", PRIV_USER);

/******************************************************************
**
** show lane client [interface SUBINTERFACE | name ELAN-NAME] [brief]
**
** OBJ (idb, 1) = Interface/Subinterface
** OBJ (string, 1) = Symbolic name of the emulated LAN.
** OBJ (int, 2) = flags (interface, name, brief)
*
*  This could be folded into the "show lane ENTITY" chain, above.
*  It's kept separate because the client will exist by itself on
*  some platforms, so we want to keep client stuff separate.
*/

EOLS	(show_lane_lec_eol, lec_show_commands, SHOW_LANE_LEC);

KEYWORD_OR (show_lane_lec_maybe_brief, show_lane_lec_eol, show_lane_lec_eol,
            OBJ (int, 2), SHOW_LANE_BRIEF,
            "brief", "skip per connection information", PRIV_USER);

STRING	(show_lane_lec_get_name, show_lane_lec_maybe_brief, no_alt,
	 OBJ (string, 1), "Name of the emulated LAN");

KEYWORD_OR (show_lane_lec_name, show_lane_lec_get_name,
            show_lane_lec_maybe_brief,
            OBJ (int, 2), SHOW_LANE_NAME,
            "name", "LEC identified by LAN name", PRIV_USER);

INTERFACE (show_lane_lec_get_int, show_lane_lec_maybe_brief, no_alt,
	   OBJ (idb, 1), (IFTYPE_ATM | IFTYPE_SUBIF));

KEYWORD_OR (show_lane_lec_int, show_lane_lec_get_int, show_lane_lec_name,
            OBJ (int, 2), SHOW_LANE_INT,
            common_str_interface, "LEC identified by interface", PRIV_USER);

KEYWORD (show_lane_lec, show_lane_lec_int, show_lane_le_arp,
	 "client", "Lan Emulation Client", PRIV_USER);

/******************************************************************
**
** this is the starting point.
**
** show lane
*/

KEYWORD (show_lane, show_lane_lec, NONE,
	 "lane", "LAN Emulation information", PRIV_USER);

TEST_INT (show_lane_test, show_lane, NONE, ALTERNATE, natmnets);

#undef	ALTERNATE
#define	ALTERNATE	show_lane_test

