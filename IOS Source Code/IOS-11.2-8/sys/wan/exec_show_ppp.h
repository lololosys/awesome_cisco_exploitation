/* $Id: exec_show_ppp.h,v 3.3.34.1 1996/04/27 06:36:26 syiu Exp $
 * $Source: /release/112/cvs/Xsys/wan/exec_show_ppp.h,v $
 *------------------------------------------------------------------
 * E X E C _ S H O W _ P P P. H - show commands for ppp
 *
 * Jan-96, Dana Blair
 * 
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: exec_show_ppp.h,v $
 * Revision 3.3.34.1  1996/04/27  06:36:26  syiu
 * Branch: California_branch
 * Commit Dial1_branch
 *
 * Revision 3.1.2.1  1996/01/16  18:10:51  dblair
 * CSCdi42375:  Move Multilink PPP into PPP subsystem
 * Branch: Dial1_branch
 *
 * Revision 3.1  1996/01/16  16:46:35  dblair
 * Place holder for show ppp commands
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/******************************************************************
 * show ppp multilink
 * 
 */

EOLS	(show_ppp_mlp_eol, show_ppp_mlp, SHOW_PPP_MULTILINK);
KEYWORD (show_ppp_mlp, show_ppp_mlp_eol, no_alt,
	 "multilink", "Multilink PPP bundle information",
	 PRIV_USER);
KEYWORD (show_ppp, show_ppp_mlp, ALTERNATE,
	 "ppp", "PPP parameters and statistics",
	 PRIV_USER);

#undef	ALTERNATE
#define	ALTERNATE	show_ppp




