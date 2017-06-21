/* $Id: exec_which-route.h,v 3.3.20.1 1996/08/12 16:00:44 widmer Exp $
 * $Source: /release/112/cvs/Xsys/clns/exec_which-route.h,v $
 *------------------------------------------------------------------
 * exec_which-route.h
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_which-route.h,v $
 * Revision 3.3.20.1  1996/08/12  16:00:44  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.3  1996/01/29  07:08:23  hampton
 * Remove command number arguments passed to parse routines that only
 * perform a single function.  [CSCdi47717]
 *
 * Revision 3.2  1995/11/17  08:53:11  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:08:51  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  01:47:04  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize xns, atalk, vines, decnet, xremote, x25, unix, wan, tn3270,
 * clns, cdp
 *
 * Revision 2.1  1995/06/07  22:28:04  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/***************************************************************
 * which-route <NSAP address> | <CLNS name>
 *
 * (*OBJ(paddr,1)) = <addr>
 */

EOLNS	(exec_which_route_eol, which_route_command);
NSAPADDR(exec_which_route_addr, exec_which_route_eol, no_alt,
	 OBJ(paddr,1), "Lookup route to OSI destination address or host name");

KEYWORD	(exec_which_route, exec_which_route_addr, ALTERNATE,
	 "which-route", "Do OSI route table lookup and display results", 
	 PRIV_OPR);

#undef	ALTERNATE
#define	ALTERNATE	exec_which_route
