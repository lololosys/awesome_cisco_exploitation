/* $Id: cfg_router_distance_bgp.h,v 3.2.60.1 1996/08/12 16:03:43 widmer Exp $
 * $Source: /release/112/cvs/Xsys/iprouting/cfg_router_distance_bgp.h,v $
 *------------------------------------------------------------------
 * C F G _ R O U T E R _ D I S T A N C E _ B G P . H
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: cfg_router_distance_bgp.h,v $
 * Revision 3.2.60.1  1996/08/12  16:03:43  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.2  1995/11/17  17:32:06  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:05:31  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/01  12:15:51  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize iprouting
 *
 * Revision 2.1  1995/06/07  22:12:33  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/**********************************************************************
 * distance bgp <dist1> <dist2> <dist3>
 * no distance bgp [<dist1> <dist2> <dist3>]
 *
 * OBJ(int,1) = dist1
 * OBJ(int,2) = dist2
 * OBJ(int,3) = dist3
 */

EOLNS	(rtr_dist_bgp_eol, bgp_distance_command);

NUMBER	(rtr_dist_bgp_dist3, rtr_dist_bgp_eol, no_alt,
	 OBJ(int,3), 1, MAXDISTANCE, "Distance for local routes");
NUMBER	(rtr_dist_bgp_dist2, rtr_dist_bgp_dist3, no_alt,
	 OBJ(int,2), 1, MAXDISTANCE, "Distance for routes internal to the AS");
NUMBER	(rtr_dist_bgp_dist1, rtr_dist_bgp_dist2, no_alt,
	 OBJ(int,1), 1, MAXDISTANCE, "Distance for routes external to the AS");
NOPREFIX (rtr_dist_bgp_noprefix, rtr_dist_bgp_dist1, rtr_dist_bgp_eol);
NVGENNS	(rtr_dist_bgp_nvgen, rtr_dist_bgp_noprefix, bgp_distance_command);
KEYWORD	(rtr_dist_bgp_kw, rtr_dist_bgp_nvgen, NONE,
	 "bgp", "BGP distance", PRIV_CONF);
ASSERT (rtr_dist_bgp, rtr_dist_bgp_kw, ALTERNATE,
	((pdbtype *) csb->protocol)->proctype & PROC_BGP);

#undef	ALTERNATE
#define	ALTERNATE	rtr_dist_bgp
