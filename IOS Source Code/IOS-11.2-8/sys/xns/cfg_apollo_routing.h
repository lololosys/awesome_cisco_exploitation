/* $Id: cfg_apollo_routing.h,v 3.2 1995/11/17 19:20:55 gstovall Exp $
 * $Source: /swtal/cherf.111/ios/sys/xns/cfg_apollo_routing.h,v $
 *------------------------------------------------------------------
 * C F G _ A P O L L O _ R O U T I N G . H
 *
 * Copyright (c) 1992-1995 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_apollo_routing.h,v $
 * Revision 3.2  1995/11/17  19:20:55  gstovall
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:57:15  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  01:42:56  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize xns, atalk, vines, decnet, xremote, x25, unix, wan, tn3270,
 * clns, cdp
 *
 * Revision 2.1  1995/06/07  22:01:13  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/**********************************************************************
 * apollo route <dstnet> <net.host>
 * no apollo route <dstnet> [<net.host>]
 *
 * OBJ(int,1) = <dstnet>
 * (*OBJ(paddr,1)) = <net.host>
 */
EOLS	(cr_apollo_rteol, apollo_command, APOLLO_ROUTE);
APOLLOADDR (cr_apollo_rtaddr, cr_apollo_rteol, no_alt,
	    OBJ(paddr,1), "Next hop address");
NOPREFIX (cr_apollo_noroute, cr_apollo_rtaddr, cr_apollo_rteol);
HEXNUM (cr_apollo_rtnet, cr_apollo_noroute, no_alt,
	     OBJ(int,1), "Destination network");
NVGENS	(cr_apollo_routenvgen, cr_apollo_rtnet, apollo_command, 
	 APOLLO_ROUTE);
KEYWORD	(cr_apollo_route, cr_apollo_routenvgen, no_alt,
	 "route", "Set static Apollo route", PRIV_CONF);

KEYWORD (cr_apollo, cr_apollo_route, ALTERNATE,
	 "apollo", " ", PRIV_CONF | PRIV_DUPLICATE);

#undef	ALTERNATE
#define	ALTERNATE	cr_apollo
