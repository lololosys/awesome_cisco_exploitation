/* $Id: cfg_router_timers_bgp.h,v 3.2.60.1 1996/08/12 16:03:54 widmer Exp $
 * $Source: /release/112/cvs/Xsys/iprouting/cfg_router_timers_bgp.h,v $
 * ------------------------------------------------------------------
 * C F G _ R O U T E R _ T I M E R S _ B G P . H
 * 
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_router_timers_bgp.h,v $
 * Revision 3.2.60.1  1996/08/12  16:03:54  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.2  1995/11/17  17:32:29  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:06:13  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/01  12:16:05  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize iprouting
 *
 * Revision 2.1  1995/06/07  22:13:15  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/************************************************************************
 *	timers bgp <keepalive> <holdtime>
 *	no timers bgp [<keepalive> <holdtime>]
 *
 * OBJ(int,1) = <keepalive>
 * OBJ(int,2) = <holdtime>
 */
EOLNS	(router_timers_bgp_eol, bgp_timers_command);

NUMBER	(router_timers_bgp_holdtime, router_timers_bgp_eol, no_alt,
	 OBJ(int,2), 1, -1, "Holdtime");

NUMBER	(router_timers_bgp_keepalive, router_timers_bgp_holdtime, no_alt,
	 OBJ(int,1), 1, -1, "Keepalive interval");

NVGENNS	(router_timers_bgp_nvgen, router_timers_bgp_keepalive,
	 bgp_timers_command);
KEYWORD	(router_timers_bgp_kw, router_timers_bgp_nvgen, NONE,
	 "bgp", "BGP timers", PRIV_CONF);

IFELSE (router_timers_bgp, router_timers_bgp_kw, ALTERNATE,
	(((pdbtype *)csb->protocol)->proctype == PROC_BGP));

#undef	ALTERNATE
#define ALTERNATE	router_timers_bgp
