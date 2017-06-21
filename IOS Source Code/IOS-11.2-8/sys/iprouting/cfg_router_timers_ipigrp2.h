/* $Id: cfg_router_timers_ipigrp2.h,v 3.3.20.1 1996/08/12 16:03:56 widmer Exp $
 * $Source: /release/112/cvs/Xsys/iprouting/cfg_router_timers_ipigrp2.h,v $
 * ------------------------------------------------------------------
 * C F G _ R O U T E R _ T I M E R S _ I P I G R P 2 . H
 * 
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_router_timers_ipigrp2.h,v $
 * Revision 3.3.20.1  1996/08/12  16:03:56  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.3  1996/02/01  23:34:02  widmer
 * CSCdi45325:  Change Keyword/Set to use Keyword_Id macro
 *
 * Revision 3.2  1995/11/17  17:32:32  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:06:17  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/01  12:16:06  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize iprouting
 *
 * Revision 2.1  1995/06/07  22:13:19  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/************************************************************************
 * [no] timers active-time [<max-active-time> | disabled ]
 *
 * OBJ(int,1) = <max-active-time>
 */
EOLNS    (router_timers_active_eol, ipigrp2_timers_command);
NUMBER  (router_timers_active_time, router_timers_active_eol,
	 router_timers_active_eol,
	 OBJ(int,1), 1, -1, "EIGRP active-state time limit in minutes");
KEYWORD	(router_timers_active_disabled, router_timers_active_eol,
	 router_timers_active_time,
	 "disabled", "disable EIGRP time limit for active state", PRIV_CONF);
NOPREFIX(router_timers_active_noprefix, router_timers_active_disabled,
	 router_timers_active_eol);
NVGENNS  (router_timers_active_nvgen, router_timers_active_noprefix,
	 ipigrp2_timers_command);
KEYWORD (router_timers_active, router_timers_active_nvgen, NONE,
	 "active-time", "EIGRP time limit for active state", PRIV_CONF);

IFELSE  (router_timers_ipigrp2, router_timers_active, ALTERNATE,
	 (((pdbtype *)csb->protocol)->proctype == PROC_IGRP2));

#undef	ALTERNATE
#define ALTERNATE	router_timers_ipigrp2
