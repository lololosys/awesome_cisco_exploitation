/* $Id: exec_clear_xns.h,v 3.2 1995/11/17 19:21:26 gstovall Exp $
 * $Source: /swtal/cherf.111/ios/sys/xns/exec_clear_xns.h,v $
 *------------------------------------------------------------------
 * E X E C _ C L E A R _ X N S . H
 *
 * Copyright (c) 1992-1995 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_clear_xns.h,v $
 * Revision 3.2  1995/11/17  19:21:26  gstovall
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:57:45  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  01:43:00  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize xns, atalk, vines, decnet, xremote, x25, unix, wan, tn3270,
 * clns, cdp
 *
 * Revision 2.1  1995/06/07  22:16:41  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * clear xns
 *
 */
EOLS	(clear_xns_eol, clear_xns_command, CLEAR_XNS);

KEYWORD_ID(clear_xns_route_all, clear_xns_eol, no_alt,
	   OBJ(int,2), TRUE,
	   "*", "Remove all XNS routing table entries", PRIV_OPR);
NUMBER	(clear_xns_route_net, clear_xns_eol, clear_xns_route_all,
	 OBJ(int,1), 1, -1, "Remove XNS routing table entries to a network");
XNSADDR	(clear_xns_route_addr, clear_xns_eol, clear_xns_route_net,
	 OBJ(paddr,1), "Remove XNS routing table entries to a network");
KEYWORD_ID(clear_xns_route, clear_xns_route_addr, no_alt,
	   OBJ(int,10), CLEAR_XNS_ROUTE,
	   "route", "Remove XNS routing table entry", PRIV_OPR);

KEYWORD_ID(clear_xns_cache, clear_xns_eol, clear_xns_route,
	   OBJ(int,10), CLEAR_XNS_CACHE,
	   "cache", "Clear XNS fast switching cache", PRIV_OPR);

KEYWORD	(clear_xns_kwd, clear_xns_cache, ALTERNATE,
	 "xns", "Reset XNS information", PRIV_OPR);

#undef	ALTERNATE
#define	ALTERNATE	clear_xns_kwd
