/* $Id: cfg_router_traffic-share.h,v 3.3.20.1 1996/08/12 16:03:58 widmer Exp $
 * $Source: /release/112/cvs/Xsys/iprouting/cfg_router_traffic-share.h,v $
 *------------------------------------------------------------------
 * cfg_router_traffic-share.h - parsing of the traffic-share command
 *
 * Oct 1993 Tony Li
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cfg_router_traffic-share.h,v $
 * Revision 3.3.20.1  1996/08/12  16:03:58  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.3  1996/02/01  23:34:03  widmer
 * CSCdi45325:  Change Keyword/Set to use Keyword_Id macro
 *
 * Revision 3.2  1995/11/17  17:32:35  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:06:21  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/01  12:16:07  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize iprouting
 *
 * Revision 2.1  1995/06/07  22:13:22  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/************************************************************************
 * [no] traffic-share balanced
 * [no] traffic-share min
 *
 * OBJ(idb,1) = TRAFFIC_BALANCED, TRAFFIC_MIN
 */

EOLNS	(router_traffic_eol, traffic_share_command);

KEYWORD_ID(router_traffic_min, router_traffic_eol, no_alt,
	   OBJ(int,1), TRAFFIC_MIN,
	   "min", "All traffic shared among min metric paths", PRIV_CONF);

KEYWORD_ID(router_traffic_balanced, router_traffic_eol, router_traffic_min,
	   OBJ(int,1), TRAFFIC_BALANCED,
	   "balanced", "Share inversely proportional to metric", PRIV_CONF); 

NVGENNS	(router_traffic_nvgen, router_traffic_balanced,
	 traffic_share_command);

KEYWORD	(router_traffic_kw, router_traffic_nvgen, NONE,
	 "traffic-share",
	 "Algorithm for computing traffic share for alternate routes",
	 PRIV_CONF);

ASSERT	(router_traffic, router_traffic_kw, ALTERNATE,
	 (((pdbtype *) csb->protocol)->proctype == PROC_IGRP) ||
	 (((pdbtype *) csb->protocol)->proctype == PROC_IGRP2));

#undef	ALTERNATE
#define	ALTERNATE	router_traffic
