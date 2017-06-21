/* $Id: exec_clear_novell.h,v 3.2.62.4 1996/08/16 00:14:15 hampton Exp $
 * $Source: /release/112/cvs/Xsys/xns/exec_clear_novell.h,v $
 *------------------------------------------------------------------
 * E X E C _ C L E A R _ N O V E L L . H
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_clear_novell.h,v $
 * Revision 3.2.62.4  1996/08/16  00:14:15  hampton
 * Change parser comments to reflect ipx command usage.  [CSCdi66208]
 * Branch: California_branch
 *
 * Revision 3.2.62.3  1996/07/26  00:08:31  hampton
 * Various minor IPX cleanups.  [CSCdi64087]
 * Branch: California_branch
 *
 * Revision 3.2.62.2  1996/05/03  02:16:14  sluong
 * Branch: California_branch
 * IPX ACL Violation Logging, Plaining English Filters, Display SAP by
 * name,
 * NLSP MIBS, Scaleable RIP/SAP, Modularity, and SAP query by name.
 *
 * Revision 3.2.62.1  1996/04/02  05:37:58  dkatz
 * CSCdi36031:  Running out of memory running 2 or more EIGRP ASs
 * Branch: California_branch
 * EIGRP Jumbo Patch.
 *
 * Revision 3.2  1995/11/17  19:21:23  gstovall
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:57:43  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:38:17  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/06/07  23:27:25  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

LINK_TRANS(clear_novell_extend_here, no_alt);

/******************************************************************
 * clear ipx
 *
 */


EOLS	(clear_novell_eol, clear_novell_command, CLEAR_NOVELL);

KEYWORD_ID(clear_novell_route_all, clear_novell_eol, no_alt,
	   OBJ(int,2), TRUE,
	   "*", "Delete routes to all IPX networks", PRIV_OPR );

KEYWORD_ID(clear_novell_route_def, clear_novell_eol, clear_novell_route_all,
	   OBJ(int,1), NOV_RIP_DEFAULT_ROUTE,
	   "default", "Delete default route", PRIV_OPR );

HEXDIGIT(clear_novell_route_mask, clear_novell_eol, clear_novell_eol,
	 OBJ(int,3), 0x80000000, 0xffffffff,
	 "Delete routes to a specific IPX network and mask" );

HEXDIGIT(clear_novell_route_addr, clear_novell_route_mask, 
	 clear_novell_route_def,
	 OBJ(int,1), NOV_MIN_NET, NOV_MAX_NET,
	 "Delete routes to a specific IPX network" );

KEYWORD_ID(clear_novell_route_kwd, clear_novell_route_addr,
	   clear_novell_extend_here,
	   OBJ(int,10), CLEAR_NOVELL_ROUTE,
	   "route", "Remove IPX routing table entry", PRIV_OPR );

KEYWORD_ID(clear_novell_spx_spoof_kwd, clear_novell_eol, 
	   clear_novell_route_kwd,
	   OBJ(int,10), CLEAR_NOVELL_SPX_SPOOF,
	   "spx-spoof", "Clear IPX SPX Spoof table", PRIV_OPR );

KEYWORD_ID(clear_novell_cache_kwd, clear_novell_eol,
	   clear_novell_spx_spoof_kwd,
	   OBJ(int,10), CLEAR_NOVELL_CACHE,
	   "cache", "Clear IPX fast switching cache", PRIV_OPR );

EOLS(clear_novell_acctg_eol, clear_ipx_accounting, CLEAR_NOVELL_ACCOUNTING);

KEYWORD_ID(clear_novell_acctg_ckpoint, clear_novell_acctg_eol,
	   clear_novell_acctg_eol, OBJ(int,1), TRUE, "checkpoint",
	   "Clear IPX accounting checkpoint database", PRIV_OPR);

KEYWORD_ID(clear_novell_acctg_kwd, clear_novell_acctg_ckpoint,
	   clear_novell_cache_kwd, OBJ(int,1), FALSE, "accounting",
	   "Clear IPX accounting database", PRIV_OPR);

NOVELL_IPX_KEYWORD (clear_novell_kwd, clear_novell_acctg_kwd, ALTERNATE,
		    "Reset Novell/IPX information", PRIV_OPR );

#undef	ALTERNATE
#define	ALTERNATE	clear_novell_kwd
