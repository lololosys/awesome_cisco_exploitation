/* $Id: exec_show_ip_nat.h,v 3.1.6.3 1996/08/07 22:47:36 ahh Exp $
 * $Source: /release/112/cvs/Xsys/ip/exec_show_ip_nat.h,v $
 *------------------------------------------------------------------
 * IP show nat commands
 *
 * October 1995, Andy Heffernan
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: exec_show_ip_nat.h,v $
 * Revision 3.1.6.3  1996/08/07  22:47:36  ahh
 * CSCdi65435:  IP: NAT statistics command does not need to be privilegedI
 * Branch: California_branch
 *
 * Revision 3.1.6.2  1996/06/03  23:23:09  ahh
 * CSCdi59119:  IP: NAT should support named access-lists
 * And extended access-lists and non-contiguous address pools.
 * Branch: California_branch
 *
 * Revision 3.1.6.1  1996/04/16  18:53:02  rchandra
 * CSCdi54830:  IP features commit
 * Branch: California_branch
 *
 * Revision 3.1  1996/03/07  05:59:00  ahh
 * Placeholder...
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * show ip nat { statistics | translations [ verbose ] }
 * 
 * OBJ(paddr,1) -- optional global IP address
 * OBJ(int,1) -- TRUE if verbose
 */

EOLS	(show_ip_nat_stats_eol, ipnat_show, IPNAT_STATS);
KEYWORD (show_ip_nat_stats, show_ip_nat_stats_eol, no_alt,
	 "statistics", "Translation statistics", PRIV_USER);

EOLS	(show_ip_nat_entries_eol, ipnat_show, IPNAT_DYNAMIC);

KEYWORD_ID(show_ip_nat_entries_verbose, show_ip_nat_entries_eol,
	 show_ip_nat_entries_eol,
	 OBJ(int,1), TRUE,
	 "verbose", "Show extra information", PRIV_USER);
KEYWORD (show_ip_nat_entries, show_ip_nat_entries_verbose, show_ip_nat_stats,
	 "translations", "Translation entries", PRIV_USER);

KEYWORD (show_ip_nat, show_ip_nat_entries, ALTERNATE,
	 "nat", "IP NAT information", PRIV_USER | PRIV_USER_HIDDEN);

#undef	ALTERNATE
#define	ALTERNATE	show_ip_nat
