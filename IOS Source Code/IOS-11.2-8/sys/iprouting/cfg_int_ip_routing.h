/* $Id: cfg_int_ip_routing.h,v 3.2.60.1 1996/04/02 05:36:46 dkatz Exp $
 * $Source: /release/112/cvs/Xsys/iprouting/cfg_int_ip_routing.h,v $
 *------------------------------------------------------------------
 * IP routing interface commands
 *
 * May 1993, Robert Widmer
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: cfg_int_ip_routing.h,v $
 * Revision 3.2.60.1  1996/04/02  05:36:46  dkatz
 * CSCdi36031:  Running out of memory running 2 or more EIGRP ASs
 * Branch: California_branch
 * EIGRP Jumbo Patch.
 *
 * Revision 3.2  1995/11/17  17:31:32  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:04:47  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/11/08  21:06:13  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.2  1995/08/04  03:07:46  dstine
 * CSCdi37681:  Need event logging for EIGRP
 *
 * Revision 2.1  1995/07/01  12:15:36  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize iprouting
 *
 * Revision 2.1  1995/06/07  22:05:46  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/************************************************************************
 * [no] ip summary-address eigrp <as-number> <address> <mask>
 *
 * OBJ(pdb,1) = IGRP1 pdb for <as-number> for the second form
 * OBJ(paddr,1) = <address>
 * OBJ(paddr,2) = <mask>
 */
EOLS (ci_ip_igrp2_summary_eols, ipigrp2_if_commands, IPIGRP2_SUMMARY);
IPADDR	(ci_ip_igrp2_summary_mask, ci_ip_igrp2_summary_eols, no_alt,
	 OBJ(paddr,2), "IP network mask");
IPADDR	(ci_ip_igrp2_summary_ipaddr, ci_ip_igrp2_summary_mask, no_alt,
	 OBJ(paddr,1), "IP address");
GENERAL_ROUTER(ci_ip_igrp2_summary_router, ci_ip_igrp2_summary_ipaddr,
	       no_alt, router_name, (PROC_IGRP2 | RMATCH_IP | RMATCH_ARGS));
NVGENS (ci_ip_igrp2_summary_nvgen, ci_ip_igrp2_summary_router, 
	ipigrp2_if_commands, IPIGRP2_SUMMARY);
KEYWORD (ci_ip_igrp2_summary, ci_ip_igrp2_summary_nvgen, ALTERNATE,
	 "summary-address", "Perform address summarization", 
	 PRIV_CONF|PRIV_SUBIF);

/************************************************************************
 * [no] ip hold-time eigrp <as-number> <value>
 *
 * OBJ(int,1) = <value>
 * OBJ(pdb,1) = IGRP2 pdb for <as-number>
 */
EOLS (ci_ip_igrp2_holdtime_eols, ipigrp2_if_commands, IPIGRP2_HOLDTIME);
NUMBER (ci_ip_igrp2_holdtime_number, ci_ip_igrp2_holdtime_eols, no_alt,
	OBJ(int,1), 1, 0xffff, "Seconds before neighbor is considered down");
NOPREFIX(ci_ip_igrp2_holdtime_noprefix, ci_ip_igrp2_holdtime_number, 
	 ci_ip_igrp2_holdtime_eols);
GENERAL_ROUTER(ci_ip_igrp2_holdtime_router, ci_ip_igrp2_holdtime_noprefix, 
	       no_alt, router_name, (PROC_IGRP2 | RMATCH_IP | RMATCH_ARGS));
NVGENS (ci_ip_igrp2_holdtime_nvgen, ci_ip_igrp2_holdtime_router, 
	ipigrp2_if_commands, IPIGRP2_HOLDTIME);
KEYWORD (ci_ip_igrp2_holdtime, ci_ip_igrp2_holdtime_nvgen, ci_ip_igrp2_summary,
	 "hold-time", 
	 "Configures IP-EIGRP hold time", PRIV_CONF|PRIV_SUBIF);

/************************************************************************
 * [no] ip hello-interval eigrp <as-number> <value>
 *
 * OBJ(int,1) = <value>
 * OBJ(pdb,1) = IGRP2 pdb for <as-number>
 */
EOLS (ci_ip_igrp2_hello_eols, ipigrp2_if_commands, IPIGRP2_HELLO);
NUMBER (ci_ip_igrp2_hello_number, ci_ip_igrp2_hello_eols, no_alt,
	OBJ(int,1), 1, 0xffff, "Seconds between hello transmissions");
NOPREFIX(ci_ip_igrp2_hello_noprefix, ci_ip_igrp2_hello_number, 
	 ci_ip_igrp2_hello_eols);
GENERAL_ROUTER(ci_ip_igrp2_hello_router, ci_ip_igrp2_hello_noprefix, no_alt,
	       router_name, (PROC_IGRP2 | RMATCH_IP | RMATCH_ARGS));
NVGENS (ci_ip_igrp2_hello_nvgen, ci_ip_igrp2_hello_router, ipigrp2_if_commands,
	IPIGRP2_HELLO);
KEYWORD (ci_ip_igrp2_hello, ci_ip_igrp2_hello_nvgen, ci_ip_igrp2_holdtime,
	 "hello-interval",
	 "Configures IP-EIGRP hello interval", PRIV_CONF|PRIV_SUBIF);

/************************************************************************
 * [no] ip bandwidth-percent eigrp <as-number> <value>
 *
 * OBJ(int,1) = <value>
 * OBJ(pdb,1) = IGRP2 pdb for <as-number>
 */
EOLS (ci_ip_igrp2_bw_pct_eols, ipigrp2_if_commands, IPIGRP2_BW_PCT);
NUMBER (ci_ip_igrp2_bw_pct_number, ci_ip_igrp2_bw_pct_eols, no_alt,
	OBJ(int,1), 1, 999999,
	"Maximum bandwidth percentage that EIGRP may use");
NOPREFIX(ci_ip_igrp2_bw_pct_noprefix, ci_ip_igrp2_bw_pct_number, 
	 ci_ip_igrp2_bw_pct_eols);
GENERAL_ROUTER(ci_ip_igrp2_bw_pct_router, ci_ip_igrp2_bw_pct_noprefix, no_alt,
	       router_name, (PROC_IGRP2 | RMATCH_IP | RMATCH_ARGS));
NVGENS (ci_ip_igrp2_bw_pct_nvgen, ci_ip_igrp2_bw_pct_router,
	ipigrp2_if_commands, IPIGRP2_BW_PCT);
KEYWORD (ci_ip_igrp2_bw_pct, ci_ip_igrp2_bw_pct_nvgen, ci_ip_igrp2_hello,
	 "bandwidth-percent", "Set EIGRP bandwidth limit",
	 PRIV_CONF|PRIV_SUBIF);

#undef	ALTERNATE
#define	ALTERNATE	ci_ip_igrp2_bw_pct
