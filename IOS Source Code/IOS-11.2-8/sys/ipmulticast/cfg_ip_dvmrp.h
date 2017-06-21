/* $Id: cfg_ip_dvmrp.h,v 3.2.58.1 1996/06/28 07:48:15 dino Exp $
 * $Source: /release/112/cvs/Xsys/ipmulticast/cfg_ip_dvmrp.h,v $
 *------------------------------------------------------------------
 * cfg_ip_dvmrp.h - DVMRP global config commands
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_ip_dvmrp.h,v $
 * Revision 3.2.58.1  1996/06/28  07:48:15  dino
 * CSCdi61612:  Put in DVMRP route-limit, have it default to 7000, avoid
 *              MBONE meltdown.
 * Branch: California_branch
 *
 * Revision 3.2  1995/11/21  23:11:56  dino
 * CSCdi43427:  cisco does not process DVMRP prunes.
 *
 * Revision 3.1  1995/11/09  12:00:30  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/10/12  19:43:51  dino
 * Placeholder commit.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * ip dvmrp distance <admin-distance>
 * no ip dvmrp distance
 *
 * OBJ(int,1) = <admin-distance>
 */
EOLS (ip_dvmrp_dist_eols, igmp_commands, IGMP_DVMRP_DISTANCE);
NUMBER (ip_dvmrp_dist_num, ip_dvmrp_dist_eols, no_alt,
	OBJ(int,1), 0, 255, "Administrative distance");
NOPREFIX (ip_dvmrp_dist_no, ip_dvmrp_dist_num, ip_dvmrp_dist_eols);
NVGENS (ip_dvmrp_dist_nvgen, ip_dvmrp_dist_no, igmp_commands, 
	IGMP_DVMRP_DISTANCE);
KEYWORD (ip_dvmrp_dist, ip_dvmrp_dist_nvgen, ALTERNATE,
	 "distance", "Set default distance for DVMRP routes", PRIV_CONF);

/*
 * ip dvmrp route-limit <num-routes>
 * no ip dvmrp route-limit
 *
 * OBJ(int,1) = <num_routes>
 */
EOLS (ip_dvmrp_routelim_eols, igmp_commands, IGMP_DVMRP_ROUTE_LIMIT);
NUMBER (ip_dvmrp_routelim_num, ip_dvmrp_routelim_eols, no_alt,
	OBJ(int,1), 0, 0xffffffff, "Number of DVMRP routes");
NOPREFIX (ip_dvmrp_routelim_no, ip_dvmrp_routelim_num, 
	  ip_dvmrp_routelim_eols);
NVGENS (ip_dvmrp_routelim_nvgen, ip_dvmrp_routelim_no, igmp_commands, 
	IGMP_DVMRP_ROUTE_LIMIT);
KEYWORD (ip_dvmrp_routelim, ip_dvmrp_routelim_nvgen, ip_dvmrp_dist,
	 "route-limit", "Maximum number of DVMRP routes reported", PRIV_CONF);

/*
 * ip dvmrp routehog-notification <num-routes>
 * no ip dvmrp routehog-notification
 *
 * OBJ(int,1) = <num_routes>
 */
EOLS (ip_dvmrp_routehog_eols, igmp_commands, IGMP_DVMRP_ROUTEHOG);
NUMBER (ip_dvmrp_routehog_num, ip_dvmrp_routehog_eols, no_alt,
	OBJ(int,1), 1, 0xffffffff, "Number of DVMRP routes");
NOPREFIX (ip_dvmrp_routehog_no, ip_dvmrp_routehog_num, 
	  ip_dvmrp_routehog_eols);
NVGENS (ip_dvmrp_routehog_nvgen, ip_dvmrp_routehog_no, igmp_commands, 
	IGMP_DVMRP_ROUTEHOG);
KEYWORD (ip_dvmrp_routehog, ip_dvmrp_routehog_nvgen, ip_dvmrp_routelim,
	 "routehog-notification", "Log DVMRP ROUTEHOG messages", PRIV_CONF);

KEYWORD (ip_dvmrp, ip_dvmrp_routehog, ALTERNATE,
	 "dvmrp", "DVMRP global commands", PRIV_CONF);

#undef	ALTERNATE
#define	ALTERNATE	ip_dvmrp







