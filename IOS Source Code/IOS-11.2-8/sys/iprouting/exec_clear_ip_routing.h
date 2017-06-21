/* $Id: exec_clear_ip_routing.h,v 3.3 1996/01/26 03:37:15 widmer Exp $
 * $Source: /vws/aam/cherf.111/ios/sys/iprouting/exec_clear_ip_routing.h,v $
 *------------------------------------------------------------------
 * IP routing clear commands
 *
 * May 1993, Robert Widmer
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: exec_clear_ip_routing.h,v $
 * Revision 3.3  1996/01/26  03:37:15  widmer
 * CSCdi47606:  clear ip route * broken
 * Change '*' to use keyword macro
 *
 * Revision 3.2  1995/11/17  17:33:07  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:06:57  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:06:20  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/07/01  12:16:11  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize iprouting
 *
 * Revision 2.1  1995/06/07  22:16:13  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/******************************************************************
 * clear ip route { <network> <mask> | '*' }
 *
 * OBJ(int,1) = TRUE if <network> was given, FALSE if '*'.
 * (*OBJ(paddr,1)) = <network>
 * (*OBJ(paddr,2)) = <mask>
 */
EOLS	(clear_ip_route_eol, clear_routecommand, CLEAR_IP_ROUTE);
KEYWORD	(clear_ip_route_all, clear_ip_route_eol, no_alt,
	 "*", "Delete all routes", PRIV_OPR);

IPADDR	(clear_ip_route_mask, clear_ip_route_eol, clear_ip_route_eol,
	 OBJ(paddr,2), "Destination network mask" );
SET	(clear_ip_route_set, clear_ip_route_mask, OBJ(int,1), TRUE);
IPADDR	(clear_ip_route_addr, clear_ip_route_set, clear_ip_route_all,
	 OBJ(paddr,1), "Destination network route to delete");
KEYWORD_ID(clear_ip_route, clear_ip_route_addr, ALTERNATE,
	   OBJ(int,1), FALSE,
	   "route", "Delete route table entries", PRIV_OPR);

#undef	ALTERNATE
#define	ALTERNATE	clear_ip_route
