/* $Id: cfg_router_timers.h,v 3.2.60.1 1996/04/15 21:17:46 bcole Exp $
 * $Source: /release/112/cvs/Xsys/iprouting/cfg_router_timers.h,v $
 * ------------------------------------------------------------------
 * C F G _ R O U T E R _ T I M E R S . H
 * 
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_router_timers.h,v $
 * Revision 3.2.60.1  1996/04/15  21:17:46  bcole
 * CSCdi54649:  IP Host Modularity, IP Automatic Stub Routing feature
 * commit
 * Branch: California_branch
 *
 * Revision 3.2  1995/11/17  17:32:28  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:06:09  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/01  12:16:04  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize iprouting
 *
 * Revision 2.1  1995/06/07  22:13:13  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

LINK_TRANS(router_timers_extend_here, no_alt);

/************************************************************************
 *	timers basic <update> <invalid> <holddown> <flush> <sleeptime>
 *	no timers basic [<update> <invalid> <holddown> <flush> <sleeptime>]
 *
 * Valid for: IGRP, RIP, EGP, ODR, and ISO-IGRP only.
 *
 * OBJ(int,1) = <update>
 * OBJ(int,2) = <invalid>
 * OBJ(int,3) = <holddown>
 * OBJ(int,4) = <flush>     (not used by ISO-IGRP)
 * OBJ(int,5) = <sleeptime> (not used by ISO-IGRP)
 */
EOLS	(router_timers_basic_eol, timers_command, P_TIMERS_BASIC);
NUMBER	(router_timers_basic_igrp, router_timers_basic_eol,
	 router_timers_basic_eol,
	 OBJ(int,5), 1, -1, "Sleep time, in milliseconds");
NUMBER	(router_timers_basic_flush, router_timers_basic_igrp, no_alt,
	 OBJ(int,4), 1, -1, "Flush");
IFELSE (router_timers_basic_test, router_timers_basic_eol, 
	router_timers_basic_flush, 
	((pdbtype *) csb->protocol)->family == PDB_OSI);
NUMBER	(router_timers_basic_holddown, router_timers_basic_test, no_alt,
	 OBJ(int,3), 0, -1, "Holddown");
NUMBER	(router_timers_basic_invalid, router_timers_basic_holddown, no_alt,
	 OBJ(int,2), 1, -1, "Invalid");
NUMBER	(router_timers_basic_update, router_timers_basic_invalid, no_alt,
	 OBJ(int,1), 0, -1, "Interval between updates");

NOPREFIX (router_timers_basic_noprefix, router_timers_basic_update,
	  router_timers_basic_eol);
NVGENS	(router_timers_basic_nvgen, router_timers_basic_noprefix,
	 timers_command, P_TIMERS_BASIC);
KEYWORD	(router_timers_basic_kw, router_timers_basic_nvgen, no_alt,
	 "basic", "Basic routing protocol update timers", PRIV_CONF);

ASSERT	(router_timers_basic, router_timers_basic_kw,
	 router_timers_extend_here,
	 (((((pdbtype *)csb->protocol)->family == PDB_IP) &&
	   (((pdbtype *)csb->protocol)->proctype &
	    (PROC_IGRP|PROC_RIP|PROC_EGP|PROC_ODR))) ||
	  ((((pdbtype *)csb->protocol)->family == PDB_OSI) &&
	   (((clns_pdbtype *)csb->protocol)->typeflag & ISO_IGRPMASK))));

/************************************************************************/

KEYWORD	(router_timers_kw, router_timers_basic, NONE,
	 "timers", "Adjust routing timers", PRIV_CONF);

ASSERT	(router_timers, router_timers_kw, ALTERNATE,
	 (((pdbtype *) csb->protocol)->family == PDB_IP) ||
	  (((clns_pdbtype *) csb->protocol)->typeflag & ISO_IGRPMASK));

#undef	ALTERNATE
#define ALTERNATE	router_timers
