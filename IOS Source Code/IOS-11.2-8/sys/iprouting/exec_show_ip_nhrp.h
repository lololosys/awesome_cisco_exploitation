/* $Id: exec_show_ip_nhrp.h,v 3.2 1995/11/17 17:33:26 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/iprouting/exec_show_ip_nhrp.h,v $
 *------------------------------------------------------------------
 * Next Hop Resolution Protocol.
 *
 * October 1995, Bruce Cole
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: exec_show_ip_nhrp.h,v $
 * Revision 3.2  1995/11/17  17:33:26  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:07:21  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:06:23  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/10/06  21:54:54  bcole
 * Placeholder files for 11.1 NHRP work.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * show ip nhrp [dynamic | static | incomplete ] [<interface>] [detail]
 *
 */
EOLS	(show_ip_nhrp_eols, show_ip_nhrp, NHRP_SHOW_CACHE);
KEYWORD_ID (show_ip_nhrp_det, show_ip_nhrp_eols, show_ip_nhrp_eols,
	    OBJ(int,2), 1,
	    "detail", "NHRP detailed cache information", PRIV_USER);
INTERFACE (show_ip_nhrp_if, show_ip_nhrp_det, show_ip_nhrp_det,
	   OBJ(idb,1), IFTYPE_ANYSUB);
KEYWORD_ID (show_ip_nhrp_inc, show_ip_nhrp_if, show_ip_nhrp_if,
	    OBJ(int,1), NHRP_SHOW_INC,
	    "incomplete", "NHRP incomplete cache information", PRIV_USER);
KEYWORD_ID (show_ip_nhrp_stat, show_ip_nhrp_if, show_ip_nhrp_inc,
	    OBJ(int,1), NHRP_SHOW_STAT,
	    "static", "NHRP static cache information", PRIV_USER);
KEYWORD_ID (show_ip_nhrp_dyn, show_ip_nhrp_if, show_ip_nhrp_stat,
	    OBJ(int,1), NHRP_SHOW_DYN,
	    "dynamic", "NHRP dynamic cache information", PRIV_USER);

/******************************************************************
 * show ip nhrp purge
 *
 */

EOLS	(show_ip_nhrp_purge_eol, show_ip_nhrp, NHRP_SHOW_PURGE);
KEYWORD	(show_ip_nhrp_purge, show_ip_nhrp_purge_eol, show_ip_nhrp_dyn,
	 "purge", "NHRP purge information", PRIV_USER);

/******************************************************************
 * show ip nhrp traffic
 *
 */

EOLS	(show_ip_nhrp_traffic_eol, show_ip_nhrp, NHRP_SHOW_TRAFFIC);
KEYWORD	(show_ip_nhrp_traffic, show_ip_nhrp_traffic_eol, show_ip_nhrp_purge,
	 "traffic", "NHRP traffic information", PRIV_USER);
KEYWORD	(show_ip_nhrp, show_ip_nhrp_traffic, ALTERNATE, "nhrp",
	 "NHRP information", PRIV_USER);

#undef	ALTERNATE
#define	ALTERNATE	show_ip_nhrp
