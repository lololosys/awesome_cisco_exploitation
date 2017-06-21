/* $Id: cfg_router_metric.h,v 3.2 1995/11/17 17:32:13 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/iprouting/cfg_router_metric.h,v $
 *------------------------------------------------------------------
 * C F G _ R O U T E R _ M E T R I C . H
 *
 * Copyright (c) 1992-1995 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_router_metric.h,v $
 * Revision 3.2  1995/11/17  17:32:13  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:05:41  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/01  12:15:54  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize iprouting
 *
 * Revision 2.1  1995/06/07  22:12:46  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/************************************************************************
 * metric weights <tos> <K1> <K2> <K3> <K4> <K5>
 *
 * OBJ(int,1) = K1
 * OBJ(int,2) = K2
 * OBJ(int,3) = K3
 * OBJ(int,4) = K4
 * OBJ(int,5) = K5
 * OBJ(int,6) = TOS
 */

EOLS	(router_metric_wt_eol, metric_command, P_METRIC_WEIGHTS);
NUMBER	(router_metric_wt_k5, router_metric_wt_eol, no_alt,
	 OBJ(int,5), 0, -1, "K5");
NUMBER	(router_metric_wt_k4, router_metric_wt_k5, no_alt,
	 OBJ(int,4), 0, -1, "K4");
NUMBER	(router_metric_wt_k3, router_metric_wt_k4, no_alt,
	 OBJ(int,3), 0, -1, "K3");
NUMBER	(router_metric_wt_k2, router_metric_wt_k3, no_alt,
	 OBJ(int,2), 0, -1, "K2");
NUMBER	(router_metric_wt_k1, router_metric_wt_k2, no_alt,
	 OBJ(int,1), 0, -1, "K1");
NUMBER	(router_metric_wt_tos, router_metric_wt_k1, no_alt,
	 OBJ(int,6), 0, 8, "Type Of Service (Only TOS 0 supported)");

NOPREFIX (router_metric_wt_noprefix, router_metric_wt_tos, 
	  router_metric_wt_eol);
NVGENS	(router_metric_wt_nvgens, router_metric_wt_noprefix, 
	 metric_command, P_METRIC_WEIGHTS);
KEYWORD	(router_metric_weights, router_metric_wt_nvgens, no_alt,
	 "weights", "Modify IGRP metric coefficients", PRIV_CONF);

/************************************************************************
 * metric maximum-hops <hops>
 * no metric maximum-hops [<hops>]
 *
 * OBJ(int,1) = hops
 */
PARAMS_KEYONLY(router_metric_maxhops, router_metric_weights,
	       "maximum-hops", OBJ(int,1), 1, 255, 
	       metric_command, P_METRIC_HOPCOUNT,
	       "Advertise IGRP routes greater than <hops> as unreachable",
	       "Hop count", PRIV_CONF);

/************************************************************************
 * metric holddown
 * no metric holddown
 *
 */
EOLS	(router_metric_holddown_eol, metric_command, P_METRIC_HOLDDOWN);
KEYWORD	(router_metric_holddown, router_metric_holddown_eol,
	 router_metric_maxhops,
	 "holddown", "Enable IGRP holddown", PRIV_CONF);

/***********************************************************************/
KEYWORD	(router_metric_kw, router_metric_holddown, NONE,
	 "metric", "Modify IGRP routing metrics and parameters", PRIV_CONF);

ASSERT	(router_metric, router_metric_kw, ALTERNATE,
	 (((pdbtype *) csb->protocol)->family == PDB_IP &&
	  ((pdbtype *) csb->protocol)->proctype & (PROC_IGRP | PROC_IGRP2)) ||
	 (((clns_pdbtype *) csb->protocol)->family == PDB_OSI &&
	  ((clns_pdbtype *) csb->protocol)->typeflag & ISO_IGRPMASK));

#undef	ALTERNATE
#define	ALTERNATE	router_metric
