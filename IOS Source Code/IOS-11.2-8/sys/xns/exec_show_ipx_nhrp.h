/* $Id: exec_show_ipx_nhrp.h,v 3.2 1995/11/17 19:21:37 gstovall Exp $
 * $Source: /swtal/cherf.111/ios/sys/xns/exec_show_ipx_nhrp.h,v $
 *------------------------------------------------------------------
 * Next Hop Resolution Protocol.
 *
 * October 1995, Bruce Cole
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: exec_show_ipx_nhrp.h,v $
 * Revision 3.2  1995/11/17  19:21:37  gstovall
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:57:57  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:38:24  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/10/06  21:55:08  bcole
 * Placeholder files for 11.1 NHRP work.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * show ipx nhrp [dynamic | static | incomplete ] [<interface>] [detail]
 *
 */
EOLS	(show_nov_nhrp_eols, show_ipx_nhrp, NHRP_SHOW_CACHE);
KEYWORD_ID (show_nov_nhrp_det, show_nov_nhrp_eols, show_nov_nhrp_eols,
	    OBJ(int,2), 1,
	    "detail", "NHRP detailed cache information", PRIV_USER);
INTERFACE (show_nov_nhrp_if, show_nov_nhrp_det, show_nov_nhrp_det,
	   OBJ(idb,1), IFTYPE_ANYSUB);
KEYWORD_ID (show_nov_nhrp_inc, show_nov_nhrp_if, show_nov_nhrp_if,
	    OBJ(int,1), NHRP_SHOW_INC,
	    "incomplete", "NHRP incomplete cache information", PRIV_USER);
KEYWORD_ID (show_nov_nhrp_stat, show_nov_nhrp_if, show_nov_nhrp_inc,
	    OBJ(int,1), NHRP_SHOW_STAT,
	    "static", "NHRP static cache information", PRIV_USER);
KEYWORD_ID (show_nov_nhrp_dyn, show_nov_nhrp_if, show_nov_nhrp_stat,
	    OBJ(int,1), NHRP_SHOW_DYN,
	    "dynamic", "NHRP dynamic cache information", PRIV_USER);

/******************************************************************
 * show ipx nhrp purge
 *
 */

EOLS	(show_nov_nhrp_purge_eol, show_ipx_nhrp, NHRP_SHOW_PURGE);
KEYWORD	(show_nov_nhrp_purge, show_nov_nhrp_purge_eol, show_nov_nhrp_dyn,
	 "purge", "NHRP purge information", PRIV_USER);

/******************************************************************
 * show ipx nhrp traffic
 *
 */

EOLS	(show_nov_nhrp_traffic_eol, show_ipx_nhrp, NHRP_SHOW_TRAFFIC);
KEYWORD	(show_nov_nhrp_traffic, show_nov_nhrp_traffic_eol, show_nov_nhrp_purge,
	 "traffic", "NHRP traffic information", PRIV_USER);
KEYWORD	(show_nov_nhrp, show_nov_nhrp_traffic, ALTERNATE, "nhrp",
	 "NHRP information", PRIV_USER);

#undef	ALTERNATE
#define	ALTERNATE	show_nov_nhrp
