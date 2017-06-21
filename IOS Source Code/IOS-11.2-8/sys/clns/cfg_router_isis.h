/* $Id: cfg_router_isis.h,v 3.2.60.4 1996/08/23 03:52:53 hsmit Exp $
 * $Source: /release/112/cvs/Xsys/clns/cfg_router_isis.h,v $
 *------------------------------------------------------------------
 * cfg_router_isis.h
 *
 * Supports IS-IS router subcommands.
 *
 * Copyright (c) 1988-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cfg_router_isis.h,v $
 * Revision 3.2.60.4  1996/08/23  03:52:53  hsmit
 * CSCdi66787:  isis max-lsp-lifetime and lsp-refresh-int nvgened in wrong
 * order
 * Branch: California_branch
 *
 * Revision 3.2.60.3  1996/08/08  14:50:03  dkatz
 * CSCdi65076:  ISIS/NLSP uses old scheduler primitives
 * Branch: California_branch
 * Use new scheduler primitives.  Get rid of gratuitous type coercion.
 *
 * Revision 3.2.60.2  1996/08/02  02:47:39  hsmit
 * CSCdi64256:  Add knob to ISIS to generate LSPs with longer lifetime
 * Branch: California_branch
 * Implemented lsp-refresh-interval and max-lsp-lifetime for ISIS. Changed
 * the
 * "show clns traffic" command to show the number of changed and refreshed
 * LSPs.
 *
 * Revision 3.2.60.1  1996/05/26  06:31:44  dkatz
 * CSCdi58433:  ISIS/NLSP can starve input queue
 * Branch: California_branch
 * Do a clear_if_input on incoming packets before requeueing them for the
 * Update process.  Depth-limit the secondary queue to avoid buffer
 * starvation.
 *
 * Revision 3.2  1995/11/17  08:51:30  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:07:04  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.5  1995/09/23  21:37:14  dkatz
 * CSCdi38306:  ISIS shouldnt run SPF on leaf route changes
 * Calculate routes from only changed LSPs if the inter-router topology
 * stays constant.  Don't gratuitously install IP routes that will
 * be immediately replaced, as this causes large-scale cache invalidation.
 * Eliminate multiple IP forwarding table traversals.
 *
 * Revision 2.4  1995/08/25  16:02:41  dkatz
 * CSCdi38487:  ISIS LSP MTU needs to be settable
 *
 * Revision 2.3  1995/08/25  05:39:49  dkatz
 * CSCdi38138:  ISIS needs adjacency state tracing
 *
 * Revision 2.2  1995/08/23  18:02:43  dkatz
 * CSCdi37692:  LSP checksum errors can melt network
 * Add a knob to discard incoming LSPs with checksum errors, rather
 * than purging them.
 *
 * Add additional debugging capabilities.
 *
 * Revision 2.1  1995/07/02  01:46:59  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize xns, atalk, vines, decnet, xremote, x25, unix, wan, tn3270,
 * clns, cdp
 *
 * Revision 2.1  1995/06/07  22:12:42  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifdef notdef	/* Commented out by dkatz until IPX-ISIS is supported */
/**********************************************************************
 * [no] ipx area-address net mask
 */
EOLS	(router_isis_area_ipx_eol, isis_rtr_commands, ISIS_IPX_AREA_CMD);
HEXDIGIT (router_isis_area_ipx_mask, router_isis_area_ipx_eol, no_alt,
	 OBJ(int,2), 0, 0xffffffff, "IPX network mask");
HEXDIGIT (router_isis_area_ipx_net, router_isis_area_ipx_mask, no_alt,
	 OBJ(int,1), 1, 0xfffffffe, "IPX network number");
NVGENS  (router_isis_area_ipx_nv, router_isis_area_ipx_net,
	 isis_rtr_commands, ISIS_IPX_AREA_CMD);
KEYWORD (router_isis_area_addr, router_isis_area_ipx_nv, 
	 no_alt,
	 "area-address", "Configure IS-IS area address",
	 PRIV_CONF);
KEYWORD  (router_isis_ipx, router_isis_area_addr, no_alt,
	  "ipx", "Configure IPX options", PRIV_CONF);
#endif

/**********************************************************************
 * update-queue-depth <nn>
 * no update-queue-depth
 */
EOLS	(router_isis_upq_depth_eol, isis_rtr_commands, ISIS_UPQ_DEPTH_CMD);
NUMBER  (router_isis_upq_depth_value, router_isis_upq_depth_eol, 
	 no_alt, OBJ(int,1), 1, MAXINT, "Max queue depth");
NOPREFIX(router_isis_upq_depth_noprefix, router_isis_upq_depth_value,
	 router_isis_upq_depth_eol);
NVGENS  (router_isis_upq_depth_nvgen, router_isis_upq_depth_noprefix,
	 isis_rtr_commands, ISIS_UPQ_DEPTH_CMD);
KEYWORD  (router_isis_upq_depth, router_isis_upq_depth_nvgen, no_alt,
	  "update-queue-depth", "Set Update process queue depth",
	  PRIV_CONF);

/**********************************************************************
 * lsp-mtu <nn>
 * no lsp-mtu
 */
EOLS	(router_isis_lsp_mtu_eol, isis_rtr_commands, ISIS_LSP_MTU_CMD);
NUMBER  (router_isis_lsp_mtu_value, router_isis_lsp_mtu_eol, 
	 no_alt, OBJ(int,1), ISIS_LSP_MTU_MIN,
	 ISIS_LSP_MTU_MAX, "Max LSP size in bytes");
NOPREFIX(router_isis_lsp_mtu_noprefix, router_isis_lsp_mtu_value,
	 router_isis_lsp_mtu_eol);
NVGENS  (router_isis_lsp_mtu_nvgen, router_isis_lsp_mtu_noprefix,
	 isis_rtr_commands, ISIS_LSP_MTU_CMD);
KEYWORD  (router_isis_lsp_mtu, router_isis_lsp_mtu_nvgen,
	  router_isis_upq_depth, "lsp-mtu", "Set maximum LSP size", PRIV_CONF);

/**********************************************************************
 * [no] display-route-detail
 */
EOLS	(router_isis_dsp_rt_det_eol, isis_rtr_commands, ISIS_DSP_RT_DET_CMD);
KEYWORD  (router_isis_dsp_rt_det, router_isis_dsp_rt_det_eol,
	  router_isis_lsp_mtu, "display-route-detail",
	  "Display detailed IP route information", PRIV_CONF);

/**********************************************************************
 * [no] log-adjacency-changes
 */
EOLS	(router_isis_log_adj_chg_eol, isis_rtr_commands, ISIS_LOG_ADJ_CHG_CMD);
KEYWORD  (router_isis_log_adj_chg, router_isis_log_adj_chg_eol,
	  router_isis_dsp_rt_det, "log-adjacency-changes",
	  "Log changes in adjacency state", PRIV_CONF);

/**********************************************************************
 * [no] ignore-lsp-errors
 */
EOLS	(router_isis_ign_lsp_err_eol, isis_rtr_commands, ISIS_IGN_LSP_ERR_CMD);
KEYWORD  (router_isis_ign_lsp_err, router_isis_ign_lsp_err_eol,
	  router_isis_log_adj_chg, "ignore-lsp-errors",
	  "Ignore LSPs with bad checksums", PRIV_CONF);

/**********************************************************************
 * [no] lsp-gen-interval [level-1 | level-2] <n>
 *
 * OBJ(int,1) = interval (in seconds)
 * OBJ(int,2) = 1 if level-1, 2 if level-2, 0 if unspecified
 */
EOLS	(router_isis_lsp_gen_ivl_eol, isis_rtr_commands, ISIS_LSP_GEN_IVL_CMD);
NUMBER  (router_isis_lsp_gen_ivl_val, router_isis_lsp_gen_ivl_eol,
	 no_alt, OBJ(int,1), 0, ISIS_NLSP_LSP_GEN_MAX, "Interval in seconds");
NOPREFIX(router_isis_lsp_gen_ivl_no, router_isis_lsp_gen_ivl_val, 
	 router_isis_lsp_gen_ivl_eol);
KEYWORD_ID(router_isis_lsp_gen_ivl_l2, router_isis_lsp_gen_ivl_no, 
	   router_isis_lsp_gen_ivl_no, OBJ(int,2), 2, "level-2",
	   "Set interval for level 2 only", PRIV_CONF);
KEYWORD_ID(router_isis_lsp_gen_ivl_l1, router_isis_lsp_gen_ivl_no, 
	   router_isis_lsp_gen_ivl_l2, OBJ(int,2), 1, "level-1",
	   "Set interval for level 1 only", PRIV_CONF);
SET     (router_isis_lsp_gen_ivl_set, router_isis_lsp_gen_ivl_l1,
	 OBJ(int,1), ISIS_NLSP_LSP_GEN_DEFAULT);
NVGENS  (router_isis_lsp_gen_ivl_nv, router_isis_lsp_gen_ivl_set,
	 isis_rtr_commands, ISIS_LSP_GEN_IVL_CMD);
KEYWORD  (router_isis_lsp_gen_ivl, router_isis_lsp_gen_ivl_nv,
	  router_isis_ign_lsp_err, "lsp-gen-interval", 
	  "Minimum interval between SPF calculations", PRIV_CONF);

/**********************************************************************
 * [no] spf-interval [level-1 | level-2] <n>
 *
 * OBJ(int,1) = interval (in seconds)
 * OBJ(int,2) = 1 if level-1, 2 if level-2, 0 if unspecified
 */
EOLS	(router_isis_spf_ivl_eol, isis_rtr_commands, ISIS_SPF_IVL_CMD);
NUMBER  (router_isis_spf_ivl_val, router_isis_spf_ivl_eol,
	 no_alt, OBJ(int,1), 0, ISIS_NLSP_SPF_IVL_MAX, 
	 "Interval in seconds");
NOPREFIX(router_isis_spf_ivl_no, router_isis_spf_ivl_val, 
	 router_isis_spf_ivl_eol);
KEYWORD_ID(router_isis_spf_ivl_l2, router_isis_spf_ivl_no, 
	   router_isis_spf_ivl_no, OBJ(int,2), 2, "level-2",
	   "Set interval for level 2 only", PRIV_CONF);
KEYWORD_ID(router_isis_spf_ivl_l1, router_isis_spf_ivl_no, 
	   router_isis_spf_ivl_l2, OBJ(int,2), 1, "level-1",
	   "Set interval for level 1 only", PRIV_CONF);
SET     (router_isis_spf_ivl_set, router_isis_spf_ivl_l1,
	 OBJ(int,1), ISIS_NLSP_SPF_IVL_DEFAULT);
NVGENS  (router_isis_spf_ivl_nv, router_isis_spf_ivl_set,
	 isis_rtr_commands, ISIS_SPF_IVL_CMD);
KEYWORD (router_isis_spf_ivl, router_isis_spf_ivl_nv, router_isis_lsp_gen_ivl,
	 "spf-interval", 
	 "Minimum interval between SPF calculations", PRIV_CONF);

/**********************************************************************
 * Format:
 * [no] lsp-refresh-interval n
 *
 * OBJ(int,1) = value
 */
EOLS    (router_isis_maxlspgen_eol, isis_rtr_commands,
	 ISIS_MAX_LSP_GEN_CMD);
NUMBER  (router_isis_maxlspgen_value, router_isis_maxlspgen_eol,
	 no_alt, OBJ(int,1), ISIS_MAX_LSP_GEN_MIN, ISIS_MAX_LSP_GEN_MAX,
	 "LSP refresh time in seconds");
NOPREFIX(router_isis_maxlspgen_no_prefix, router_isis_maxlspgen_value,
	 router_isis_maxlspgen_eol);
SET     (router_isis_maxlspgen_set, router_isis_maxlspgen_no_prefix,
	 OBJ(int,1), ISIS_MAX_LSP_GEN_DEFAULT);
NVGENS  (router_isis_maxlspgen_nvgen, router_isis_maxlspgen_set,
	 isis_rtr_commands, ISIS_MAX_LSP_GEN_CMD);
KEYWORD (router_isis_maxlspgen, router_isis_maxlspgen_nvgen,
	 router_isis_spf_ivl, "lsp-refresh-interval",
	 "Set LSP refresh interval", PRIV_CONF);

/**********************************************************************
 * Format:
 * [no] max-lsp-lifetime n
 * OBJ(int,1) = value
 */
EOLS    (router_isis_maxage_eol, isis_rtr_commands,
	 ISIS_MAXAGE_CMD);
NUMBER  (router_isis_maxage_value, router_isis_maxage_eol,
	 no_alt, OBJ(int,1), ISIS_MAXAGE_MIN, ISIS_MAXAGE_MAX,
	 "Maximum LSP lifetime in seconds");
NOPREFIX(router_isis_maxage_noprefix, router_isis_maxage_value,
	 router_isis_maxage_eol);
SET     (router_isis_maxage_set, router_isis_maxage_noprefix,
	 OBJ(int,1), ISIS_MAXAGE_DEFAULT);
NVGENS  (router_isis_maxage_nvgen, router_isis_maxage_set,
	 isis_rtr_commands, ISIS_MAXAGE_CMD);
KEYWORD (router_isis_maxage, router_isis_maxage_nvgen, router_isis_maxlspgen,
	 "max-lsp-lifetime", "Set maximum LSP lifetime", PRIV_CONF);

/**********************************************************************
 * [no] set-overload-bit
 */
EOLS	(router_isis_hippity_eols, isis_rtr_commands, ISIS_FORCE_HIPPITY_CMD);
KEYWORD	(router_isis_hippity, router_isis_hippity_eols,
	 router_isis_maxage,
	 "set-overload-bit", "Signal other routers not to use us in SPF",
	 PRIV_CONF);

/**********************************************************************
 * [no] ignore-attached-bit [ip | clns]
 *
 * OBJ(int,1) = 1 if ip, 2 if clns, 0 if unspecified
 */
EOLS	(router_isis_ignore_att_eols, isis_rtr_commands, ISIS_IGN_ATT_BIT_CMD);
KEYWORD_ID(router_isis_ignore_att_clns, router_isis_ignore_att_eols,
	   router_isis_ignore_att_eols, OBJ(int,1), 2, "clns",
	   "Never install a CLNS default route to closest L2 IS", PRIV_CONF);
KEYWORD_ID(router_isis_ignore_att_ip, router_isis_ignore_att_eols,
	   router_isis_ignore_att_clns, OBJ(int,1), 1, "ip",
	   "Never install an IP default route to closest L2 IS", PRIV_CONF);
NOPREFIX(router_isis_ignore_att_no, router_isis_ignore_att_ip,
	 router_isis_ignore_att_eols);
NVGENS	(router_isis_ignore_att_nv, router_isis_ignore_att_ip,
	 isis_rtr_commands, ISIS_IGN_ATT_BIT_CMD);
KEYWORD	(router_isis_ignore_att, router_isis_ignore_att_nv,
	 router_isis_hippity, "ignore-attached-bit",
	 "Never install default route to closest L2 router",
	 PRIV_CONF|PRIV_HIDDEN);

/**********************************************************************
 * [no] never-set-attached-bit
 */
EOLS	(router_isis_never_set_att_eols, isis_rtr_commands,
	 ISIS_NEVER_SET_ATT_CMD);
KEYWORD (router_isis_never_set_att, router_isis_never_set_att_eols,
	 router_isis_ignore_att,
	 "never-set-attached-bit", "Never advertise us as attached to L2",
	 PRIV_CONF|PRIV_HIDDEN);

/**********************************************************************
 * [no] partition-avoidance
 */
EOLS	(router_isis_part_avoid_eols, isis_rtr_commands, ISIS_PART_AVOID_CMD);
KEYWORD (router_isis_part_avoid, router_isis_part_avoid_eols, 
	 router_isis_never_set_att,
	 "partition-avoidance", "Enable partition avoidance",
	 PRIV_CONF|PRIV_HIDDEN);

/**********************************************************************
 * area-password <string>
 * no area-password
 */
EOLS (router_isis_area_eols, isis_rtr_commands, ISIS_AREA_PW_CMD);
STRING (router_isis_area_string, router_isis_area_eols, no_alt, 
	OBJ(string,1), "Area password");
TESTVAR	(router_isis_area_noprefix, router_isis_area_string, 
	 router_isis_area_eols, NONE, NONE, NONE, sense, TRUE);
NVGENS (router_isis_area_nvgen, router_isis_area_noprefix, isis_rtr_commands,
	ISIS_AREA_PW_CMD);
KEYWORD (router_isis_area_pass, router_isis_area_nvgen, router_isis_part_avoid,
	 "area-password", "Configure the authentication password for an area",
	 PRIV_CONF);

/**********************************************************************
 * domain-password <string>
 * no domain-password
 */
EOLS (router_isis_domain_eols, isis_rtr_commands, ISIS_DOMAIN_PW_CMD);
STRING (router_isis_domain_string, router_isis_domain_eols, no_alt,
	OBJ(string,1), "Routing domain password");
TESTVAR	(router_isis_domain_noprefix, router_isis_domain_string, 
	 router_isis_domain_eols, NONE, NONE, NONE, sense, TRUE);
NVGENS (router_isis_domain_nvgen, router_isis_domain_noprefix,
	isis_rtr_commands, ISIS_DOMAIN_PW_CMD);
KEYWORD (router_isis_domain_pass, router_isis_domain_nvgen, 
	 router_isis_area_pass, "domain-password", 
	 "Configure the authentication password for a routing domain", 
	 PRIV_CONF);

ASSERT	(router_isis, router_isis_domain_pass, ALTERNATE,
	 ((clns_pdbtype *) csb->protocol)->family == PDB_OSI &&
	 ((clns_pdbtype *) csb->protocol)->typeflag & ISO_ISISMASK);

#undef	ALTERNATE
#define	ALTERNATE	router_isis
