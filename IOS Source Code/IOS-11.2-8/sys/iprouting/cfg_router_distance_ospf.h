/* $Id$
 * $Source$
 *------------------------------------------------------------------
 * cfg_router_distance_ospf.h - OSPF distance command
 *
 * July 1997, Derek M. Yeung
 *
 * Copyright (c) 1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log$
 *------------------------------------------------------------------
 * $Endlog$
 */

/***********************************************************************
 * distance ospf {[intra-area <dist1>] [inter-area <dist2>] 
 *                [external <dist3>]}
 * no distance ospf
 *
 * OBJ(int,1) = dist1
 * OBJ(int,2) = dist2
 * OBJ(int,3) = dist3
 * OBJ(int,4) records which distance is set.
 */

pdecl(rtr_dist_ospf_intra);

EOLS	(rtr_dist_ospf_eol, ospf_distance_command, 0);

ASSERT  (rtr_dist_ospf_eol_check, rtr_dist_ospf_eol, NONE, GETOBJ(int,4));
NUMBER	(rtr_dist_ospf_dist3, rtr_dist_ospf_intra, no_alt,
	 OBJ(int,3), 1, MAXDISTANCE, 
	 "Distance for external type 5 and type 7 routes");
KEYWORD_ORTEST (rtr_dist_ospf_external, rtr_dist_ospf_dist3, 
		rtr_dist_ospf_eol_check, OBJ(int,4), OSPF_DIST_EXTERNAL, 
		"external", "External type 5 and type 7 routes", PRIV_CONF);
NUMBER	(rtr_dist_ospf_dist2, rtr_dist_ospf_intra, no_alt,
	 OBJ(int,2), 1, MAXDISTANCE, "Distance for inter-area routes");
KEYWORD_ORTEST (rtr_dist_ospf_inter, rtr_dist_ospf_dist2, 
		rtr_dist_ospf_external, OBJ(int,4), OSPF_DIST_INTER,
		"inter-area", "Inter-area routes", PRIV_CONF);
NUMBER	(rtr_dist_ospf_dist1, rtr_dist_ospf_intra, no_alt,
	 OBJ(int,1), 1, MAXDISTANCE, "Distance for intra-area routes");
KEYWORD_ORTEST (rtr_dist_ospf_intra, rtr_dist_ospf_dist1, rtr_dist_ospf_inter,
		OBJ(int,4), OSPF_DIST_INTRA, "intra-area", 
		"Intra-area routes", PRIV_CONF);
NOPREFIX (rtr_dist_ospf_noprefix, rtr_dist_ospf_intra, rtr_dist_ospf_eol);
NVGENS	(rtr_dist_ospf_nvgen, rtr_dist_ospf_noprefix, 
	 ospf_distance_command, 0);
KEYWORD	(rtr_dist_ospf_kw, rtr_dist_ospf_nvgen, NONE,
	 "ospf", "OSPF distance", PRIV_CONF);
ASSERT (rtr_dist_ospf, rtr_dist_ospf_kw, ALTERNATE,
	((pdbtype *) csb->protocol)->proctype & PROC_OSPF);

#undef	ALTERNATE
#define	ALTERNATE	rtr_dist_ospf
