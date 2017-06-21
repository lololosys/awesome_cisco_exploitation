/* $Id: exec_show_lanmgr.h,v 3.3.20.1 1996/05/21 09:48:47 thille Exp $
 * $Source: /release/112/cvs/Xsys/ibm/exec_show_lanmgr.h,v $
 *------------------------------------------------------------------
 * E X E C _ S H O W _ L A N M G R . H
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_show_lanmgr.h,v $
 * Revision 3.3.20.1  1996/05/21  09:48:47  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.3  1996/02/01  23:33:24  widmer
 * CSCdi45325:  Change Keyword/Set to use Keyword_Id macro
 *
 * Revision 3.2  1995/11/17  09:22:15  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:44:35  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  10:53:55  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize os, hes, les, if, and ibm.
 *
 * Revision 2.1  1995/06/07  22:24:53  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#ifdef IBMNM_DEBUG
/******************************************************************
 * show lanmgr zebra
 *
 */

EOLS	(show_lanmgr_zebra_eol, show_lanmgr, SHOW_LANMGR_ZEBRA);
KEYWORD (show_lanmgr_zebra, show_lanmgr_zebra_eol, no_alt,
	"zebra", "LAN Mgr testing", PRIV_USER);
#else

/* If debugging is off, don't show the zebra command */
NOP	(show_lanmgr_zebra, NONE, no_alt);
#endif

/******************************************************************
 * show lanmgr station [{ <mac-addr> | * }]
 *
 * OBJ(int,1) =	TRUE = A station address was specified
 * 		FALSE = No station address
 * OBJ(int,2) =	TRUE = A '*' was parsed
 *		FALSE = No '*' was parsed.
 * (*OBJ(hwaddr,1)) = <mac-addr>
 */

EOLS	(show_lanmgr_station_eol, show_lanmgr, SHOW_LANMGR_STATION);
KEYWORD_ID(show_lanmgr_station_all, show_lanmgr_station_eol,
	   show_lanmgr_station_eol,
	   OBJ(int,2), TRUE,
	   "*", "All stations", PRIV_USER);
SET	(show_lanmgr_station_addr_set, show_lanmgr_station_eol,
	 OBJ(int,1), TRUE);
ENADDR (show_lanmgr_station_addr, show_lanmgr_station_addr_set,
	show_lanmgr_station_all,
	OBJ(hwaddr,1), "Station MAC address");
KEYWORD (show_lanmgr_station, show_lanmgr_station_addr, show_lanmgr_zebra,
	"station", "LAN Mgr stations", PRIV_USER);

/******************************************************************
 * show lanmgr ring [<ring-number>]
 *
 * OBJ(int,1) = <ring-number>
 */
EOLS	(show_lanmgr_ring_eol, show_lanmgr, SHOW_LANMGR_RING);
NUMBER (show_lanmgr_ring_num, show_lanmgr_ring_eol, show_lanmgr_ring_eol,
	OBJ(int,1), 0, 4095, "Ring number");
KEYWORD (show_lanmgr_ring, show_lanmgr_ring_num, show_lanmgr_station,
	"ring", "LAN Mgr ring information", PRIV_USER);


/******************************************************************
 * show lanmgr interface <interface>
 *
 * OBJ(idb,1) = <interface>
 */
EOLS	(show_lanmgr_if_eol, show_lanmgr, SHOW_LANMGR_INTERFACE);
INTERFACE(show_lanmgr_if_name, show_lanmgr_if_eol, show_lanmgr_if_eol,
	  OBJ(idb,1), IFTYPE_TOKENRING);
KEYWORD (show_lanmgr_if, show_lanmgr_if_name, show_lanmgr_ring,
	common_str_interface, "Interface information", PRIV_USER);

/******************************************************************
 * show lanmgr config
 *
 */
EOLS	(show_lanmgr_config_eol, show_lanmgr, SHOW_LANMGR_CONFIG);
KEYWORD (show_lanmgr_config, show_lanmgr_config_eol, show_lanmgr_if,
	"config", "LAN Mgr configuration", PRIV_USER);

/******************************************************************
 * show lanmgr bridge <interface>
 *
 * OBJ(idb,1) = <interface>
 * 	<interface> must be a tokenring interface
 */

EOLS	(show_lanmgr_bridge_eol, show_lanmgr, SHOW_LANMGR_BRIDGE);
INTERFACE (show_lanmgr_bridge_if, show_lanmgr_bridge_eol,
	   show_lanmgr_bridge_eol,
	OBJ(idb,1), IFTYPE_TOKENRING);
KEYWORD (show_lanmgr_bridge, show_lanmgr_bridge_if, show_lanmgr_config,
	"bridge", "Bridging", PRIV_USER);

/******************************************************************/
KEYWORD (show_lanmgr_kwd, show_lanmgr_bridge, NONE,
	 "lnm", "IBM LAN manager", PRIV_USER|PRIV_USER_HIDDEN);

TEST_BOOLEAN(show_lanmgr_test, show_lanmgr_kwd, NONE,
	     ALTERNATE, bridge_enable);

#undef	ALTERNATE
#define	ALTERNATE	show_lanmgr_test
