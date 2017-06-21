/* $Id: cfg_int_novigrp.h,v 3.1.2.2 1996/06/21 16:49:58 sluong Exp $
 * $Source: /release/112/cvs/Xsys/xns/cfg_int_novigrp.h,v $
 *------------------------------------------------------------------
 * IPX EIGRP interface subcommands.
 *
 * April, 1996, Steven Luong
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cfg_int_novigrp.h,v $
 * Revision 3.1.2.2  1996/06/21  16:49:58  sluong
 * CSCdi57042:  ipx floating static route kicks in after 4 min with EIGRP
 * Branch: California_branch
 *
 * Revision 3.1.2.1  1996/05/03  02:15:53  sluong
 * Branch: California_branch
 * IPX ACL Violation Logging, Plaining English Filters, Display SAP by
 * name,
 * NLSP MIBS, Scaleable RIP/SAP, Modularity, and SAP query by name.
 *
 * Revision 3.1  1996/04/13  01:24:43  sluong
 * Add placeholders
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*****************************************************************
 * [no] ipx sap-incremental <as-number> [rsup-only]
 */
EOLS	(ci_ipx_sap_inc_eol, novigrp_if_commands, IPXIGRP_SAPUPDATE);
KEYWORD_ID (ci_ipx_sap_inc_rsuponly, ci_ipx_sap_inc_eol, ci_ipx_sap_inc_eol,
	    OBJ(int,3), 1, "rsup-only",
	    "Enable EIGRP to only carry SAP traffic on the link",
	    PRIV_CONF|PRIV_SUBIF);
GENERAL_ROUTER(ci_ipx_sap_inc_router, ci_ipx_sap_inc_rsuponly,
               no_alt, router_name,
	       (PROC_IGRP2 | RMATCH_NOVELL | RMATCH_ARGS));
NVGENS (ci_ipx_sap_inc_nvgen, ci_ipx_sap_inc_router,
        novigrp_if_commands, IPXIGRP_SAPUPDATE);
KEYWORD (ci_ipx_sap_inc, ci_ipx_sap_inc_nvgen, no_alt,
	"sap-incremental",
	"Send incremental SAP updates - for IPX EIGRP networks only",
	PRIV_CONF|PRIV_SUBIF);

/************************************************************************
 * [no] ipx split-horizon eigrp <as-number>
 *
 * OBJ(pdb,1) = pdb for <as-number> 
 */
EOLS (ci_ipx_igrp_sh_eols, novigrp_if_commands, IPXIGRP_SPLIT);
GENERAL_ROUTER(ci_ipx_igrp_sh_router, ci_ipx_igrp_sh_eols, no_alt, router_name,
               (PROC_IGRP2 | RMATCH_NOVELL | RMATCH_ARGS));
NVGENS (ci_ipx_igrp_sh_nvgen, ci_ipx_igrp_sh_router, novigrp_if_commands,
        IPXIGRP_SPLIT);
KEYWORD (ci_ipx_igrp_sh, ci_ipx_igrp_sh_nvgen, ci_ipx_sap_inc, "split-horizon",
         "Perform split horizon", PRIV_CONF|PRIV_SUBIF);

/*****************************************************************
 * [no] ipx hold-down eigrp <as-number> <value>
 *
 * OBJ(int,1) = <value>
 * OBJ(pdb,1) = IGRP1 pdb for <as-number>
 */
EOLS (ci_ipx_igrp_holddown_eols, novigrp_if_commands, IPXIGRP_HOLDDOWN);
NUMBER (ci_ipx_igrp_holddown_number, ci_ipx_igrp_holddown_eols, no_alt,
        OBJ(int,1), 1, 0xffff, "Seconds that IPX EIGRP routes in hold down");
NOPREFIX(ci_ipx_igrp_holddown_noprefix, ci_ipx_igrp_holddown_number,
         ci_ipx_igrp_holddown_eols);
GENERAL_ROUTER(ci_ipx_igrp_holddown_router, ci_ipx_igrp_holddown_noprefix,
               no_alt, router_name,
	       (PROC_IGRP2 | RMATCH_NOVELL | RMATCH_ARGS));
NVGENS (ci_ipx_igrp_holddown_nvgen, ci_ipx_igrp_holddown_router,
        novigrp_if_commands, IPXIGRP_HOLDDOWN);
KEYWORD (ci_ipx_igrp_holddown, ci_ipx_igrp_holddown_nvgen, ci_ipx_igrp_sh,
         "hold-down",
         "Configures IPX EIGRP routes hold down time", 
	 PRIV_CONF|PRIV_SUBIF);

/*****************************************************************
 * [no] ipx hold-time eigrp <as-number> <value>
 *
 * OBJ(int,1) = <value>
 * OBJ(pdb,1) = IGRP1 pdb for <as-number>
 */
EOLS (ci_ipx_igrp_holdtime_eols, novigrp_if_commands, IPXIGRP_HOLDTIME);
NUMBER (ci_ipx_igrp_holdtime_number, ci_ipx_igrp_holdtime_eols, no_alt,
        OBJ(int,1), 1, 0xffff, "Seconds before neighbor is considered down");
NOPREFIX(ci_ipx_igrp_holdtime_noprefix, ci_ipx_igrp_holdtime_number,
         ci_ipx_igrp_holdtime_eols);
GENERAL_ROUTER(ci_ipx_igrp_holdtime_router, ci_ipx_igrp_holdtime_noprefix,
               no_alt, router_name,
	       (PROC_IGRP2 | RMATCH_NOVELL | RMATCH_ARGS));
NVGENS (ci_ipx_igrp_holdtime_nvgen, ci_ipx_igrp_holdtime_router,
        novigrp_if_commands, IPXIGRP_HOLDTIME);
KEYWORD (ci_ipx_igrp_holdtime, ci_ipx_igrp_holdtime_nvgen, 
         ci_ipx_igrp_holddown, "hold-time",
         "Configures IPX EIGRP hold time", PRIV_CONF|PRIV_SUBIF);

/****************************************************************
 * [no] ipx bandwidth-percent eigrp <as-number> <value>
 *
 *
 * OBJ(int,1) = <value>
 * OBJ(pdb,1) = IGRP1 pdb for <as-number>
 */
EOLS (ci_ipx_igrp_bw_pct_eols, novigrp_if_commands, IPXIGRP_BW_PCT);
NUMBER (ci_ipx_igrp_bw_pct_number, ci_ipx_igrp_bw_pct_eols, no_alt,
        OBJ(int,1), 1, 999999,
	"Maximum percentage of bandwidth that EIGRP may use");
NOPREFIX(ci_ipx_igrp_bw_pct_noprefix, ci_ipx_igrp_bw_pct_number, 
	ci_ipx_igrp_bw_pct_eols);
GENERAL_ROUTER(ci_ipx_igrp_bw_pct_router, ci_ipx_igrp_bw_pct_noprefix, no_alt,
	router_name, (PROC_IGRP2 | RMATCH_NOVELL | RMATCH_ARGS));
NVGENS (ci_ipx_igrp_bw_pct_nvgen, ci_ipx_igrp_bw_pct_router,
        novigrp_if_commands, IPXIGRP_BW_PCT);
KEYWORD (ci_ipx_igrp_bw_pct, ci_ipx_igrp_bw_pct_nvgen, ci_ipx_igrp_holdtime,
         "bandwidth-percent",
         "Set EIGRP bandwidth limit", PRIV_CONF|PRIV_SUBIF);

/****************************************************************
 * [no] ipx hello-interval eigrp <as-number> <value>
 *
 *
 * OBJ(int,1) = <value>
 * OBJ(pdb,1) = IGRP1 pdb for <as-number>
 */
EOLS (ci_ipx_igrp_hello_eols, novigrp_if_commands, IPXIGRP_HELLO);
NUMBER (ci_ipx_igrp_hello_number, ci_ipx_igrp_hello_eols, no_alt,
        OBJ(int,1), 1, 0xffff, "Seconds between hello transmissions");
NOPREFIX(ci_ipx_igrp_hello_noprefix, ci_ipx_igrp_hello_number, 
	ci_ipx_igrp_hello_eols);
GENERAL_ROUTER(ci_ipx_igrp_hello_router, ci_ipx_igrp_hello_noprefix, no_alt,
	router_name, (PROC_IGRP2 | RMATCH_NOVELL | RMATCH_ARGS));
NVGENS (ci_ipx_igrp_hello_nvgen, ci_ipx_igrp_hello_router,
        novigrp_if_commands, IPXIGRP_HELLO);
KEYWORD (ci_ipx_igrp_hello, ci_ipx_igrp_hello_nvgen, ci_ipx_igrp_bw_pct,
         "hello-interval",
         "Configures IPX EIGRP hello interval", PRIV_CONF|PRIV_SUBIF);

#undef  ALTERNATE
#define ALTERNATE ci_ipx_igrp_hello
