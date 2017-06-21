/* $Id: exec_show_x25.h,v 3.2 1995/11/17 18:06:12 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/x25/exec_show_x25.h,v $
 *------------------------------------------------------------------
 * E X E C _ S H O W _ X 2 5 . H
 *
 * Copyright (c) 1992-1995 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_show_x25.h,v $
 * Revision 3.2  1995/11/17  18:06:12  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:52:26  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  01:45:11  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize xns, atalk, vines, decnet, xremote, x25, unix, wan, tn3270,
 * clns, cdp
 *
 * Revision 2.1  1995/06/07  22:26:36  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/******************************************************************
 * show x25 vc [<line>]
 *
 * OBJ(int,1) = <line>
 *		-1 if no line
 */
EOLS	(show_x25_vc_eol, show_x25,        SHOW_X25_VC);
SET	(show_x25_vc_noline, show_x25_vc_eol, OBJ(int,1), -1);
NUMBER	(show_x25_vc_line, show_x25_vc_eol, show_x25_vc_noline,
	OBJ(int,1), 1, 4095, "Virtual circuit number");
KEYWORD (show_x25_vc, show_x25_vc_line, no_alt,
	 "vc", "Show x25 virtual circuit parameters and statistics",
	 PRIV_USER);

/******************************************************************
 * show x25 route
 */
EOLS	(show_x25_route_eol, show_x25,        SHOW_X25_ROUTE);
KEYWORD (show_x25_route, show_x25_route_eol, show_x25_vc,
	 "route", "Show x25 routing table", PRIV_USER);

/******************************************************************
 * show x25 remote-red
 */
EOLS	(show_x25_remoter_eol, show_x25, SHOW_X25_BFE);
KEYWORD	(show_x25_remoter, show_x25_remoter_eol, show_x25_route,
	 "remote-red", "X25 REMOTE-RED table", PRIV_USER);

/******************************************************************
 * show x25 pad [<line>]
 *
 * OBJ(int,1) = <line>
 *		-1 if no line
 */
EOLS	(show_x25_pad_eol, show_x25,        SHOW_X25_PAD);
SET	(show_x25_pad_noline, show_x25_pad_eol, OBJ(int,1), -1);
NUMBER	(show_x25_pad_line, show_x25_pad_eol, show_x25_pad_noline,
	OBJ(int,1), 0, 256, "Line number");
KEYWORD (show_x25_pad, show_x25_pad_line, show_x25_remoter,
            "pad", "X25 pad connection status", PRIV_USER);

/******************************************************************
 * show x25 map
 */
EOLS	(show_x25_map_eol, show_x25, SHOW_X25_MAP);
KEYWORD (show_x25_map, show_x25_map_eol, show_x25_pad,
            "map", "Show x25 map table", PRIV_USER);

/******************************************************************
 * show x25
 *
 */
KEYWORD (show_x25_kwd, show_x25_map, NONE,
	 "x25", "X.25 information", PRIV_USER|PRIV_USER_HIDDEN);

TEST_BOOLEAN(show_x25_test, show_x25_kwd, NONE, ALTERNATE, x25_enable);

#undef	ALTERNATE
#define	ALTERNATE	show_x25_test
