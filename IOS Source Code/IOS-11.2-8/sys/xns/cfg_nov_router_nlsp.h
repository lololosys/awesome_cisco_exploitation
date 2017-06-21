/* $Id: cfg_nov_router_nlsp.h,v 3.4.6.3 1996/08/23 04:08:33 hsmit Exp $
 * $Source: /release/112/cvs/Xsys/xns/cfg_nov_router_nlsp.h,v $
 *------------------------------------------------------------------
 * C F G _ N O V _ R O U T E R _ N L S P . H
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cfg_nov_router_nlsp.h,v $
 * Revision 3.4.6.3  1996/08/23  04:08:33  hsmit
 * CSCdi66788:  nlsp max-lsp-lifetime and lsp-refresh-int nvgened in wrong
 * order
 * Branch: California_branch
 *
 * Revision 3.4.6.2  1996/08/12  16:09:57  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.4.6.1  1996/05/30  23:47:54  hampton
 * All commands that take access list numbers should accept either numbers
 * in the current monolithic number space, or hidden numbers in the future
 * per-protocol access list spaces.  [CSCdi59067]
 * Branch: California_branch
 *
 * Revision 3.4  1996/03/01  07:34:17  mschaef
 * CSCdi50473:  NLSP route aggregation conformance (phase 1)
 *
 * Fix assorted nits and bugs
 *
 * Revision 3.3  1995/12/11  04:55:17  sluong
 * CSCdi45316:  Add route-aggregation command for nlsp
 * change to not automatically generate aggregate routes.
 * up the maximum number of nlsp instances to 28.
 *
 * Revision 3.2  1995/11/17  19:21:11  gstovall
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:57:32  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/11/08  21:38:10  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.3  1995/08/25  16:03:07  dkatz
 * CSCdi38487:  ISIS LSP MTU needs to be settable
 *
 * Revision 2.2  1995/08/25  05:40:23  dkatz
 * CSCdi38138:  ISIS needs adjacency state tracing
 *
 * Revision 2.1  1995/06/07  23:27:15  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
/************************************************************************
 * [no] distribute-list <list> in [<interface>]
 * [no] distribute-list <list> out [{ <routing-process> | <interface> }]
 *
 * OBJ(int,1) = <list>
 * OBJ(int,2) = IN or OUT
 * OBJ(idb,1) = <interface>
 * OBJ(string,2) = routing process
 * OBJ(int,3) = routing process AS number (if needed)
 *
 */

EOLNS	(nov_nlsp_rtr_distl_eol, novell_distributelist_command);

/* <interface-name><unit> */
INTERFACE (nov_nlsp_rtr_distl_if, nov_nlsp_rtr_distl_eol, 
	   nov_nlsp_rtr_distl_eol, OBJ(idb,1), IFTYPE_ANYSUB);

/* <routing-process> */
GENERAL_ROUTER(nov_nlsp_rtr_distl_rtrproc, nov_nlsp_rtr_distl_eol, 
	       nov_nlsp_rtr_distl_if, 
	       router_name, (PROC_IGRP2 | PROC_RIP | RMATCH_NOVELL | 
			     RMATCH_ARGS | PROC_NLSP));

/* in */
KEYWORD_ID (nov_nlsp_rtr_distl_in, nov_nlsp_rtr_distl_if, no_alt,
	    OBJ(int,2), IN,
	    "in", "Filter incoming routing updates", PRIV_CONF);

/* out */
KEYWORD_ID (nov_nlsp_rtr_distl_out, nov_nlsp_rtr_distl_rtrproc, 
	    nov_nlsp_rtr_distl_in,  OBJ(int,2), OUT,
	    "out", "Filter outgoing routing updates", PRIV_CONF);

/* <list>
 * Allow the range of 1200 lists.
 */
ACCESS_LIST(nov_nlsp_rtr_distl_list2, nov_nlsp_rtr_distl_out, no_alt,
	 OBJ(int,1), MINNOVSUM, MAXNOVSUM,
	 IPX_MIN_SUM_ACL, IPX_MAX_SUM_ACL, "An IPX access list number");
/* <list>
 * Allow the range of 800 lists.
 */
ACCESS_LIST(nov_nlsp_rtr_distl_list, nov_nlsp_rtr_distl_out, 
	 nov_nlsp_rtr_distl_list2,
	 OBJ(int,1), MINNOVACC, MAXNOVACC,
	 IPX_MIN_STD_ACL, IPX_MAX_STD_ACL, "An IPX access list number");
NVGENNS	(nov_nlsp_rtr_distl_nvgen, nov_nlsp_rtr_distl_list, 
	 novell_distributelist_command);

KEYWORD	(nov_nlsp_rtr_distribute, nov_nlsp_rtr_distl_nvgen, no_alt,
	 "distribute-list", "Filter networks in routing updates", PRIV_CONF);

/**********************************************************************
 * Format:
 * [no] redistribute [rip | eigrp <as>] | nlsp <tag>] [access-list] [<list>]
 *
 * router subcommand
 * OBJ(int,1) = list number
 * OBJ(string,1) = nlsp routing process tag
 */
pdecl   (nov_nlsp_redist_opts);

EOLNS    (nov_nlsp_redist_eol, novell_redist_command);
ACCESS_LIST(nov_nlsp_redist_list,  nov_nlsp_redist_eol, no_alt,
	 OBJ(int,1), MINNOVSUM, MAXNOVSUM,
	 IPX_MIN_SUM_ACL, IPX_MAX_SUM_ACL, "An IPX access list number");
STRING (nov_nlsp_redist_tag, nov_nlsp_redist_opts, NONE, 
	OBJ(string,1), "Routing process tag");
IFELSE (nov_nlsp_redist_ifelse, nov_nlsp_redist_tag, NONE, !*GETOBJ(string,1));
TEST_MULTIPLE_FUNCS(nov_nlsp_redist_test, nov_nlsp_redist_ifelse, 
		    nov_nlsp_redist_eol, NONE);
KEYWORD	(nov_nlsp_redist_acc, nov_nlsp_redist_list, nov_nlsp_redist_test,
	 "access-list", "Filter incoming routing updates", PRIV_CONF);
NOP (nov_nlsp_redist_opts, nov_nlsp_redist_acc, nov_nlsp_redist_eol);
SET (nov_nlsp_redist_set, nov_nlsp_redist_opts, router_name.protocol, 
     PROC_NLSP);
KEYWORD	(nov_nlsp_redist_nlsp, nov_nlsp_redist_set, no_alt, 
	 "nlsp", "IPX Netware Link Services Protocol", PRIV_CONF);

ACCESS_LIST(nov_nlsp_redist_list1, nov_nlsp_redist_eol, no_alt,
	 OBJ(int,1), MINNOVSUM, MAXNOVSUM,
	 IPX_MIN_SUM_ACL, IPX_MAX_SUM_ACL, "An IPX access list number");
KEYWORD	(nov_nlsp_redist_acc1, nov_nlsp_redist_list1, nov_nlsp_redist_eol,
	 "access-list", "Filter incoming routing updates", PRIV_CONF);

GENERAL_ROUTER (nov_nlsp_redist_rtr, nov_nlsp_redist_acc1, 
		nov_nlsp_redist_nlsp, router_name, (PROC_IGRP2 | PROC_RIP | 
                PROC_STATIC | RMATCH_NOVELL | RMATCH_ARGS));
NVGENNS  (nov_nlsp_redist_nvgen, nov_nlsp_redist_rtr, novell_redist_command);
KEYWORD (nov_nlsp_redist, nov_nlsp_redist_nvgen, nov_nlsp_rtr_distribute, 
	 "redistribute", "Enable routing protocol redistribution ", PRIV_CONF);

/**********************************************************************
 * [no] p-lsp-route-aggregation
 *
 */
EOLS       (nov_nlsp_rtr_plsp_eols,
	    novnlsp_command, NOV_NLSP_ENABLE_NP_LSP_SUMMARY);
KEYWORD_MM (nov_nlsp_plsp_kw, nov_nlsp_rtr_plsp_eols,
	    nov_nlsp_redist, "non-pseudonode-route-aggregation",
	    "Enable aggregated route generation in pseudonode LSPs",
	    PRIV_CONF|PRIV_HIDDEN, 16);

/**********************************************************************
 * [no] route-aggregation
 *
 */
EOLS (nov_nlsp_rtr_route_aggr_eols, novnlsp_command, NOV_NLSP_ENABLE_SUMMARY);
KEYWORD	(nov_nlsp_rtr_route_aggr_kw, nov_nlsp_rtr_route_aggr_eols,
	 nov_nlsp_plsp_kw, "route-aggregation", 
	 "Enable NLSP route aggregation", PRIV_CONF);

/**********************************************************************
 * [no] lsp-data-hashing [rt_frags] [svc_frags]
 *
 * OBJ(int,1) = Number of fragments to contain routes
 * OBJ(int,2) = Number of fragments to contain services
 */
EOLS    (nov_router_hash_eol, novnlsp_command, NOV_NLSP_LSP_HASH_ENABLE);
NUMBER  (nov_router_hash_get_svc, nov_router_hash_eol, nov_router_hash_eol,
 	 OBJ(int,2), 0, ISIS_MAX_LSP_FRAGMENT, "Service fragments");
NUMBER  (nov_router_hash_get_rt, nov_router_hash_get_svc, nov_router_hash_eol,
 	 OBJ(int,1), 0, ISIS_MAX_LSP_FRAGMENT, "Route fragments");
NOPREFIX(nov_router_hash_noprefix,
 	 nov_router_hash_get_rt, nov_router_hash_eol);
SET     (nov_router_hash_set_svc, nov_router_hash_noprefix,
 	 OBJ(int,2), ISIS_NLSP_SVC_FRAGS_DEFAULT);
SET     (nov_router_hash_set_rt, nov_router_hash_set_svc,
 	 OBJ(int,1), ISIS_NLSP_RT_FRAGS_DEFAULT);
NVGENS  (nov_router_hash_nvgen, nov_router_hash_set_rt,
 	 novnlsp_command, NOV_NLSP_LSP_HASH_ENABLE);
KEYWORD (nov_router_hash, nov_router_hash_nvgen, nov_nlsp_rtr_route_aggr_kw,
  	 "lsp-data-hashing", "Use hash algorithm to populate LSP's",
  	 PRIV_CONF);
/**********************************************************************
 * [no] log-adjacency-changes
 */
EOLS	(nov_router_log_adj_chg_eol, novnlsp_command, NOV_NLSP_LOG_ADJ_CHG);
KEYWORD  (nov_router_log_adj_chg, nov_router_log_adj_chg_eol, nov_router_hash,
	  "log-adjacency-changes",  "Log changes in adjacency state",
	  PRIV_CONF);

/**********************************************************************
 * [no] name-display-disable
 */
EOLS    (nov_router_sysid_display_eol, novnlsp_command,
	 NOV_NLSP_SYSID_DISPLAY);
KEYWORD (nov_router_sysid_display, nov_router_sysid_display_eol,
	 nov_router_log_adj_chg, "name-display-disable",
	  "Display system IDs rather than names", PRIV_CONF);

/**********************************************************************
 * [no] prc-interval n
 *
 * OBJ(int,1) = interval (in seconds)
 */
EOLS    (nov_router_prc_ivl_eol, novnlsp_command, NOV_NLSP_PRC_IVL);
NUMBER  (nov_router_prc_ivl_value, nov_router_prc_ivl_eol, no_alt,
	 OBJ(int,1), 0, ISIS_NLSP_PRC_IVL_MAX,
	 "Minimum LSP generation interval in seconds");
NOPREFIX(nov_router_prc_ivl_noprefix, nov_router_prc_ivl_value,
	 nov_router_prc_ivl_eol);
SET     (nov_router_prc_ivl_set, nov_router_prc_ivl_noprefix,
	 OBJ(int,1), ISIS_NLSP_PRC_IVL_DEFAULT);
NVGENS  (nov_router_prc_ivl_nvgen, nov_router_prc_ivl_set,
	 novnlsp_command, NOV_NLSP_PRC_IVL);
KEYWORD  (nov_router_prc_ivl, nov_router_prc_ivl_nvgen,
	  nov_router_sysid_display, "prc-interval", 
	  "Minimum interval between partial route calculation", PRIV_CONF);

/**********************************************************************
 * [no] spf-interval n
 *
 * OBJ(int,1) = interval (in seconds)
 */
EOLS    (nov_router_spf_ivl_eol, novnlsp_command, NOV_NLSP_SPF_IVL);
NUMBER  (nov_router_spf_ivl_value, nov_router_spf_ivl_eol, no_alt,
	 OBJ(int,1), 0, ISIS_NLSP_SPF_IVL_MAX,
	 "Minimum LSP generation interval in seconds");
NOPREFIX(nov_router_spf_ivl_noprefix, nov_router_spf_ivl_value,
	 nov_router_spf_ivl_eol);
SET     (nov_router_spf_ivl_set, nov_router_spf_ivl_noprefix,
	 OBJ(int,1), ISIS_NLSP_SPF_IVL_DEFAULT);
NVGENS  (nov_router_spf_ivl_nvgen, nov_router_spf_ivl_set,
	 novnlsp_command, NOV_NLSP_SPF_IVL);
KEYWORD  (nov_router_spf_ivl, nov_router_spf_ivl_nvgen,
	  nov_router_prc_ivl, "spf-interval", 
	  "Minimum interval between SPF calculations", PRIV_CONF);

/**********************************************************************
 * Format:
 * [no] lsp-refresh-interval n
 *
 * OBJ(int,1) = value
 *
 * router subcommand
 */
EOLS    (nov_router_maxlspgen_eol, novnlsp_command, 
	 NOV_NLSP_MAX_LSP_GEN);
NUMBER  (nov_router_maxlspgen_value, nov_router_maxlspgen_eol, 
	 no_alt, OBJ(int,1), 1, NOV_NLSP_MAX_LSP_GEN_MAX,
	 "Refresh time in seconds");
NOPREFIX(nov_router_maxlspgen_noprefix, nov_router_maxlspgen_value,
	 nov_router_maxlspgen_eol);
SET     (nov_router_maxlspgen_set, nov_router_maxlspgen_noprefix,
	 OBJ(int,1), NOV_NLSP_MAX_LSP_GEN_DEFAULT);
NVGENS  (nov_router_maxlspgen_nvgen, nov_router_maxlspgen_set,
	 novnlsp_command, NOV_NLSP_MAX_LSP_GEN);
KEYWORD (nov_router_maxlspgen, nov_router_maxlspgen_nvgen, 
	 nov_router_spf_ivl, "lsp-refresh-interval",
	 "Set LSP refresh interval", PRIV_CONF);

/**********************************************************************
 * Format:
 * [no] max-lsp-lifetime n
 *
 * OBJ(int,1) = value
 *
 * router subcommand
 */
EOLS    (nov_router_maxage_eol, novnlsp_command, 
	 NOV_NLSP_MAXAGE);
NUMBER  (nov_router_maxage_value, nov_router_maxage_eol, 
	 no_alt, OBJ(int,1), 1, NOV_NLSP_MAXAGE_MAX,
	 "Maximum lifetime in seconds");
NOPREFIX(nov_router_maxage_noprefix, nov_router_maxage_value,
	 nov_router_maxage_eol);
SET     (nov_router_maxage_set, nov_router_maxage_noprefix,
	 OBJ(int,1), NOV_NLSP_MAXAGE_DEFAULT);
NVGENS  (nov_router_maxage_nvgen, nov_router_maxage_set,
	 novnlsp_command, NOV_NLSP_MAXAGE);
KEYWORD (nov_router_maxage, nov_router_maxage_nvgen, nov_router_maxlspgen,
	 "max-lsp-lifetime", "Set maximum LSP lifetime", PRIV_CONF);

/**********************************************************************
 * Format:
 * [no] lsp-mtu n
 *
 * OBJ(int,1) = value
 *
 * router subcommand
 */
EOLS    (nov_router_lspmtu_eol, novnlsp_command, 
	 NOV_NLSP_LSP_MTU);
NUMBER  (nov_router_lspmtu_value, nov_router_lspmtu_eol, 
	 no_alt, OBJ(int,1), NOV_NLSP_LSP_MTU_MIN,
	 NOV_NLSP_LSP_MTU_MAX, "Max LSP size in bytes");
NOPREFIX(nov_router_lspmtu_noprefix, nov_router_lspmtu_value,
	 nov_router_lspmtu_eol);
NVGENS  (nov_router_lspmtu_nvgen, nov_router_lspmtu_noprefix,
	 novnlsp_command, NOV_NLSP_LSP_MTU);
KEYWORD (nov_router_lspmtu, nov_router_lspmtu_nvgen, 
	 nov_router_maxage, "lsp-mtu",
	 "Set maximum size of a link state packet", PRIV_CONF);

/**********************************************************************
 * Format:
 * [no] lsp-gen-interval n
 *
 * OBJ(int,1) = value
 *
 * router subcommand
 */
EOLS    (nov_router_minlspgen_eol, novnlsp_command, NOV_NLSP_LSP_GEN);
NUMBER  (nov_router_minlspgen_value, nov_router_minlspgen_eol, no_alt,
	 OBJ(int,1), 0, ISIS_NLSP_LSP_GEN_MAX,
	 "Minimum LSP generation interval in seconds");
NOPREFIX(nov_router_minlspgen_noprefix, nov_router_minlspgen_value,
	 nov_router_minlspgen_eol);
SET     (nov_router_minlspgen_set, nov_router_minlspgen_noprefix,
	 OBJ(int,1), ISIS_NLSP_LSP_GEN_DEFAULT);
NVGENS  (nov_router_minlspgen_nvgen, nov_router_minlspgen_set,
	 novnlsp_command, NOV_NLSP_LSP_GEN);
KEYWORD (nov_router_minlspgen, nov_router_minlspgen_nvgen, 
	 nov_router_lspmtu, "lsp-gen-interval",
	 "Set minimum LSP generation interval", PRIV_CONF);

/**********************************************************************
 * Format:
 * [no] area-address <net> <mask>
 *
 * OBJ(int,1) = net
 * OBJ(int,2) = mask
 *
 * router subcommand 
 */
EOLS	(nov_router_area_ipx_eol, novnlsp_command, NOV_NLSP_AREA_ADDR);
HEXDIGIT (nov_router_area_ipx_mask, nov_router_area_ipx_eol, no_alt,
	 OBJ(int,2), 0, 0xffffffff, "IPX network mask");
HEXDIGIT (nov_router_area_ipx_net, nov_router_area_ipx_mask, no_alt,
	 OBJ(int,1), 0, 0xffffffff, "IPX network number");
NVGENS  (nov_router_area_ipx_nv, nov_router_area_ipx_net,
	 novnlsp_command, NOV_NLSP_AREA_ADDR);
KEYWORD (nov_router_area_addr, nov_router_area_ipx_nv, nov_router_minlspgen,
	 "area-address", "Configure NLSP area address",
	 PRIV_CONF);

IFELSE (nov_router_area_addrs, nov_router_area_addr, ALTERNATE,
	(((novpdbtype *)csb->protocol)->proctype == NET_NLSP));

#undef  ALTERNATE
#define ALTERNATE 	nov_router_area_addrs
