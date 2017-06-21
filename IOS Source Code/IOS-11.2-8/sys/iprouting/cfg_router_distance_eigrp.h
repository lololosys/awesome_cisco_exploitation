/* $Id: cfg_router_distance_eigrp.h,v 3.2 1995/11/17 17:32:07 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/iprouting/cfg_router_distance_eigrp.h,v $
 *------------------------------------------------------------------
 * C F G _ R O U T E R _ D I S T A N C E _ E I G R P . H
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cfg_router_distance_eigrp.h,v $
 * Revision 3.2  1995/11/17  17:32:07  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:05:33  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/01  12:15:52  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize iprouting
 *
 * Revision 2.1  1995/06/07  22:12:35  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/**********************************************************************
 * distance eigrp <dist1> <dist2>
 * no distance eigrp [<dist1> <dist2>]
 *
 * OBJ(int,1) = dist1
 * OBJ(int,2) = dist2
 */
EOLS (rtr_dist_eigrp_eol, distance_command, DISTANCE_IGRP2);
NUMBER  (rtr_dist_eigrp_dist2, rtr_dist_eigrp_eol, no_alt,
         OBJ(int,2), 1, MAXDISTANCE, "Distance for external routes");
NUMBER  (rtr_dist_eigrp_dist1, rtr_dist_eigrp_dist2, no_alt,
         OBJ(int,1), 1, MAXDISTANCE, "Distance for internal routes");
NOPREFIX (rtr_dist_eigrp_noprefix, rtr_dist_eigrp_dist1, rtr_dist_eigrp_eol);
NVGENS  (rtr_dist_eigrp_nvgen, rtr_dist_eigrp_noprefix,
         distance_command, DISTANCE_IGRP2);
KEYWORD (rtr_dist_eigrp_kw, rtr_dist_eigrp_nvgen, NONE,
         "eigrp", "IP-EIGRP distance", PRIV_CONF);
ASSERT (rtr_dist_eigrp, rtr_dist_eigrp_kw, ALTERNATE,
        ((pdbtype *) csb->protocol)->proctype & PROC_IGRP2);

#undef	ALTERNATE
#define	ALTERNATE	rtr_dist_eigrp
