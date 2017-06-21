/* $Id: exec_show_smds.h,v 3.2 1995/11/17 18:02:04 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/wan/exec_show_smds.h,v $
 *------------------------------------------------------------------
 * E X E C _ S H O W _ S M D S . H
 *
 * Copyright (c) 1992-1995 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_show_smds.h,v $
 * Revision 3.2  1995/11/17  18:02:04  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:48:53  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  01:46:10  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize xns, atalk, vines, decnet, xremote, x25, unix, wan, tn3270,
 * clns, cdp
 *
 * Revision 2.1  1995/06/07  22:25:48  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/******************************************************************
 * show smds traffic
 */
EOLS	(show_smds_traffic_eol, show_smds,        SHOW_SMDS_TRAFFIC);
KEYWORD (show_smds_traffic, show_smds_traffic_eol, no_alt,
            "traffic", "SMDS protocol statistics", PRIV_USER);

/******************************************************************
 * show smds map
 */
EOLS	(show_smds_map_eol, show_smds,        SHOW_SMDS_MAP);
KEYWORD (show_smds_map, show_smds_map_eol, show_smds_traffic,
            "map", "SMDS map table", PRIV_USER);

/******************************************************************
 * show smds addresses
 */
EOLS	(show_smds_addr_eol, show_smds,        SHOW_SMDS_ADDRESSES);
KEYWORD (show_smds_addr, show_smds_addr_eol, show_smds_map,
            "addresses", "Interfaces and their SMDS addresses", PRIV_USER);

/******************************************************************
 * show smds
 */
KEYWORD (show_smds, show_smds_addr, NONE,
	 "smds", "SMDS information", PRIV_USER|PRIV_USER_HIDDEN);


ASSERT	(show_smds_test, show_smds, ALTERNATE,
	 nhssinets || nserialnets);

#undef	ALTERNATE
#define	ALTERNATE	show_smds_test
