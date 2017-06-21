/* $Id: exec_clear_ip_bgp.h,v 3.2.60.2 1996/08/12 16:04:00 widmer Exp $
 * $Source: /release/112/cvs/Xsys/iprouting/exec_clear_ip_bgp.h,v $
 *------------------------------------------------------------------
 * BGP clear commands
 *
 * May 1993, Robert Widmer
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: exec_clear_ip_bgp.h,v $
 * Revision 3.2.60.2  1996/08/12  16:04:00  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.2.60.1  1996/04/16  19:01:28  rchandra
 * CSCdi54830:  IP features commit
 * Branch: California_branch
 *
 * Revision 3.2  1995/11/17  17:33:02  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:06:53  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/09/25  08:30:53  tli
 * CSCdi40960:  ISP jumbo patch 2
 *
 * Revision 2.1  1995/07/01  12:16:09  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize iprouting
 *
 * Revision 2.1  1995/06/07  22:16:03  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * clear ip bgp flap-statistics filter-list <list>
 * clear ip bgp flag-statistics
 *
 */

EOLS	(clear_ip_bgp_fstat_eol, clear_ip_bgp, BGP_CLEAR_FSTAT);

EOLS	(clear_ip_bgp_fstat_flist_eol, clear_ip_bgp, BGP_CLEAR_FSTAT_FLIST);
NUMBER	(clear_ip_bgp_fstat_flist_n, clear_ip_bgp_fstat_flist_eol, no_alt,
	 OBJ(int, 1), MINREGEXPACC+1, MAXREGEXPACC,
	 "Regular expression access list number");
KEYWORD	(clear_ip_bgp_fstat_flist, clear_ip_bgp_fstat_flist_n,
	 clear_ip_bgp_fstat_eol,
	 "filter-list", "Clear flap statistics for routes conforming to "
	 "the filter-list", PRIV_USER);


/******************************************************************
 * clear ip bgp flap-statistics regexp <regexp>
 *
 */

EOLS	(clear_ip_bgp_fstat_regexp_eol, clear_ip_bgp, BGP_CLEAR_FSTAT_REGEXP);
TEXT	(clear_ip_bgp_fstat_regexp_s, clear_ip_bgp_fstat_regexp_eol, no_alt,
	 OBJ(string,1), "A regular expression to match the BGP AS paths");
KEYWORD	(clear_ip_bgp_fstat_regexp, clear_ip_bgp_fstat_regexp_s,
	 clear_ip_bgp_fstat_flist, 
	 "regexp", "Clear flap statistics for routes matching the "
	 "regular expression", PRIV_USER);


/******************************************************************
 * clear ip bgp flap-statistics <addr> <mask>
 *
 */

EOLS	(clear_ip_bgp_fstat_addr_eol, clear_ip_bgp, BGP_CLEAR_FSTAT_ADDR);
IPADDR	(clear_ip_bgp_fstat_mask, clear_ip_bgp_fstat_addr_eol, 
	 clear_ip_bgp_fstat_addr_eol, OBJ(paddr,2), "Network mask");
IPADDR	(clear_ip_bgp_fstat_addr, clear_ip_bgp_fstat_mask, 
	 clear_ip_bgp_fstat_regexp, OBJ(paddr,1),
	 "Network to clear flap statistics");
KEYWORD	(clear_ip_bgp_fstat_kw, clear_ip_bgp_fstat_addr, no_alt,
	 "flap-statistics", "Clear route flap statistics", PRIV_USER);


/******************************************************************
 * clear ip bgp dampening [<addr> <mask>]
 *
 */

EOLNS	(clear_ip_bgp_damp_eol, clear_ip_bgp_dampinfo);
IPADDR	(clear_ip_bgp_damp_mask, clear_ip_bgp_damp_eol, clear_ip_bgp_damp_eol,
	 OBJ(paddr,2), "Network mask");
IPADDR	(clear_ip_bgp_damp_addr, clear_ip_bgp_damp_mask, clear_ip_bgp_damp_eol,
	 OBJ(paddr,1), "Network to clear damping information");
KEYWORD	(clear_ip_bgp_damp_kw, clear_ip_bgp_damp_addr, clear_ip_bgp_fstat_kw,
	 "dampening", "Clear route flap dampening information", PRIV_OPR);


/******************************************************************
 * clear ip bgp * [soft [in | out]]
 *
 * OBJ(int,2) = TRUE for inbound  soft reconfig
 * OBJ(int,3) = TRUE for outbound soft reconfig
 */

EOLS	(clear_ip_bgp_eol, clear_ip_bgp, BGP_CLEAR_NEIGH);

EOLS	(clear_ip_bgp_all_soft_eol, clear_ip_bgp_soft, BGP_CLEAR_SOFT_ALL);
KEYWORD_ID (clear_ip_bgp_all_soft_out, clear_ip_bgp_all_soft_eol,
	    clear_ip_bgp_all_soft_eol, OBJ(int,3), TRUE,
	    "out", "soft reconfigure outbound update", PRIV_OPR);
KEYWORD_ID (clear_ip_bgp_all_soft_in, clear_ip_bgp_all_soft_out,
	    clear_ip_bgp_all_soft_out, OBJ(int,2), TRUE,
	    "in", "soft reconfigure inbound update", PRIV_OPR);
KEYWORD	(clear_ip_bgp_all_soft, clear_ip_bgp_all_soft_in, clear_ip_bgp_eol,
	 "soft", "Soft reconfiguration", PRIV_OPR);

KEYWORD	(clear_ip_bgp_all, clear_ip_bgp_all_soft, clear_ip_bgp_damp_kw,
	 "*", "Clear all connections", PRIV_OPR);

/******************************************************************
 * clear ip bgp <AS number> [soft [in | out]]
 *
 * OBJ(int,2) = TRUE for inbound  soft reconfig
 * OBJ(int,3) = TRUE for outbound soft reconfig
 * OBJ(int,1) = AS number
 */

EOLS  (clear_ip_bgp_as_eol, clear_ip_bgp, BGP_CLEAR_NEIGH_AS);

EOLS  (clear_ip_bgp_as_soft_eol, clear_ip_bgp_soft,
       BGP_CLEAR_SOFT_AS);
KEYWORD_ID    (clear_ip_bgp_as_soft_out, clear_ip_bgp_as_soft_eol,
               clear_ip_bgp_as_soft_eol, OBJ(int,3), TRUE,
               "out", "soft reconfigure outbound update", PRIV_OPR);
KEYWORD_ID    (clear_ip_bgp_as_soft_in, clear_ip_bgp_as_soft_out,
               clear_ip_bgp_as_soft_out, OBJ(int,2), TRUE,
               "in", "soft reconfigure inbound update", PRIV_OPR);
KEYWORD       (clear_ip_bgp_as_soft, clear_ip_bgp_as_soft_in, clear_ip_bgp_as_eol,
       "soft", "Soft reconfiguration", PRIV_OPR);
NUMBER        (clear_ip_bgp_as, clear_ip_bgp_as_soft, clear_ip_bgp_all,
       OBJ(int,1), 1, MAXUSHORT, "AS number of the peers");

/******************************************************************
 * clear ip bgp <peer-group> [soft [in | out]]
 *
 * OBJ(int,2) = TRUE for inbound  soft reconfig
 * OBJ(int,3) = TRUE for outbound soft reconfig
 */

EOLNS	(clear_ip_bgp_pgrp_eol, clear_ip_bgp_pgrp);

EOLS	(clear_ip_bgp_pgrp_soft_eol, clear_ip_bgp_soft, BGP_CLEAR_SOFT_PGRP);
KEYWORD_ID (clear_ip_bgp_pgrp_soft_out, clear_ip_bgp_pgrp_soft_eol,
	    clear_ip_bgp_pgrp_soft_eol, OBJ(int,3), TRUE,
	    "out", "soft reconfigure outbound update", PRIV_OPR);
KEYWORD_ID (clear_ip_bgp_pgrp_soft_in, clear_ip_bgp_pgrp_soft_out,
	    clear_ip_bgp_pgrp_soft_out, OBJ(int,2), TRUE,
	    "in", "soft reconfigure inbound update", PRIV_OPR);
KEYWORD	(clear_ip_bgp_pgrp_soft, clear_ip_bgp_pgrp_soft_in,
	 clear_ip_bgp_pgrp_eol,
	 "soft", "Soft reconfiguration", PRIV_OPR);

STRING	(clear_ip_bgp_pgrp_tag, clear_ip_bgp_pgrp_soft, no_alt,
	 OBJ(string, 1), "BGP peer-group name to clear connection");
KEYWORD (clear_ip_bgp_pgrp_kw, clear_ip_bgp_pgrp_tag, clear_ip_bgp_as,
	 "peer-group", "Clear BGP connections of peer-group", PRIV_OPR);


/******************************************************************
 * clear ip bgp <ipaddr> [flap-statistics | soft [in | out]]
 *
 * OBJ(int,1) = TRUE if an <ipaddr> of the bgp peer was given, 
 *              FALSE otherwise
 * OBJ(int,2) = TRUE for inbound  soft reconfig
 * OBJ(int,3) = TRUE for outbound soft reconfig
 */

SET	(clear_ip_bgp_set, clear_ip_bgp_eol, OBJ(int,1), TRUE);

EOLS	(clear_ip_bgp_neigh_soft_eol, clear_ip_bgp_soft, BGP_CLEAR_SOFT_NEIGH);
KEYWORD_ID (clear_ip_bgp_neigh_soft_out, clear_ip_bgp_neigh_soft_eol,
	    clear_ip_bgp_neigh_soft_eol, OBJ(int,3), TRUE,
	    "out", "soft reconfigure outbound update", PRIV_OPR);
KEYWORD_ID (clear_ip_bgp_neigh_soft_in, clear_ip_bgp_neigh_soft_out,
	    clear_ip_bgp_neigh_soft_out, OBJ(int,2), TRUE,
	    "in", "soft reconfigure inbound update", PRIV_OPR);
KEYWORD	(clear_ip_bgp_neigh_soft, clear_ip_bgp_neigh_soft_in,
	 clear_ip_bgp_set,
	 "soft", "Soft reconfiguration", PRIV_OPR);


EOLS	(clear_ip_bgp_neigh_fstat_eol, clear_ip_bgp, BGP_CLEAR_FSTAT_NEIGH);
KEYWORD	(clear_ip_bgp_neigh_fstat, clear_ip_bgp_neigh_fstat_eol,
	 clear_ip_bgp_neigh_soft, 
	 "flap-statistics", "Clear flap statistic", PRIV_USER);


IPADDR	(clear_ip_bgp_addr, clear_ip_bgp_neigh_fstat, clear_ip_bgp_pgrp_kw,
	OBJ(paddr,1), "BGP neighbor address to clear");
KEYWORD_ID (clear_ip_bgp, clear_ip_bgp_addr, ALTERNATE,
	 OBJ(int,1), FALSE,
	 "bgp", "Clear BGP connections", PRIV_OPR);

#undef	ALTERNATE
#define	ALTERNATE	clear_ip_bgp
