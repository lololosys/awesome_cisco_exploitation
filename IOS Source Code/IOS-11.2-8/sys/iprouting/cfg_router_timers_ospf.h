/* $Id: cfg_router_timers_ospf.h,v 3.2.60.1 1996/08/12 16:03:57 widmer Exp $
 * $Source: /release/112/cvs/Xsys/iprouting/cfg_router_timers_ospf.h,v $
 * ------------------------------------------------------------------
 * C F G _ R O U T E R _ T I M E R S _ O S P F . H
 * 
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_router_timers_ospf.h,v $
 * Revision 3.2.60.1  1996/08/12  16:03:57  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.2  1995/11/17  17:32:33  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:06:19  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/01  12:16:06  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize iprouting
 *
 * Revision 2.1  1995/06/07  22:13:20  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/************************************************************************
 *	timers spf <spf-delay> <spf-hold>
 *	no timers spf [<spf-delay> <spf-hold>]
 *
 * OBJ(int,1) = <spf-delay>
 * OBJ(int,2) = <spf-hold>
 *
 */
EOLNS	(router_timers_spf_eol, ospf_timers_command);
NUMBER	(router_timers_spf_hold, router_timers_spf_eol, no_alt,
	 OBJ(int,2), 0, MAXUINT,
	 "Hold time between consecutive SPF calculations");
NUMBER	(router_timers_spf_delay, router_timers_spf_hold, no_alt,
	 OBJ(int,1), 0, MAXUINT,
	 "Delay between receiving a change to SPF calculation");

NVGENNS	(router_timers_spf_nvgen, router_timers_spf_delay,
	 ospf_timers_command);
KEYWORD	(router_timers_spf, router_timers_spf_nvgen, no_alt,
	 "spf", "OSPF SPF timers", PRIV_CONF);

IFELSE (router_timers_ospf, router_timers_spf, ALTERNATE,
	(((pdbtype *)csb->protocol)->proctype == PROC_OSPF));

#undef	ALTERNATE
#define ALTERNATE	router_timers_ospf
