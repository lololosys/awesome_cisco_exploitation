/* $Id: cfg_router_timers_egp.h,v 3.2.60.1 1996/08/12 16:03:55 widmer Exp $
 * $Source: /release/112/cvs/Xsys/iprouting/cfg_router_timers_egp.h,v $
 * ------------------------------------------------------------------
 * C F G _ R O U T E R _ T I M E R S _ E G P . H
 * 
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_router_timers_egp.h,v $
 * Revision 3.2.60.1  1996/08/12  16:03:55  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.2  1995/11/17  17:32:31  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:06:15  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/19  10:50:28  tli
 * CSCdi36065:  Rationalize EGP parser file placement
 * Move parser support to iprouting subdirectory.
 *
 * Revision 2.1  1995/06/07  22:13:17  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/************************************************************************
 *	timers egp <hello> <poll>
 *	no timers egp [<hello> <poll>]
 *
 * OBJ(int,1) = <hello>
 * OBJ(int,2) = <poll>
 *
 */
EOLNS	(router_timers_egp_eol, egp_timers_command);
NUMBER	(router_timers_egp_poll, router_timers_egp_eol, no_alt,
	 OBJ(int,2), 1, MAXUINT, "Interval between polls");
NUMBER	(router_timers_egp_hello, router_timers_egp_poll, no_alt,
	 OBJ(int,1), 1, MAXUINT, "Interval between hellos");

/* egp */
NVGENNS	(router_timers_egp_nvgen, router_timers_egp_hello,
	 egp_timers_command);
KEYWORD	(router_timers_egp_kw, router_timers_egp_nvgen, NONE,
	 "egp", "EGP timers", PRIV_CONF);

IFELSE (router_timers_egp, router_timers_egp_kw, ALTERNATE,
	(((pdbtype *)csb->protocol)->proctype == PROC_EGP));

#undef	ALTERNATE
#define ALTERNATE	router_timers_egp
