/* $Id: cfg_int_ipx_nlsp.h,v 3.2 1995/11/17 19:21:00 gstovall Exp $
 * $Source: /swtal/cherf.111/ios/sys/xns/cfg_int_ipx_nlsp.h,v $
 *------------------------------------------------------------------
 * C F G _ I N T _ I P X _ N L S P . H
 * 
 * NLSP specific interface commands.
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_int_ipx_nlsp.h,v $
 * Revision 3.2  1995/11/17  19:21:00  gstovall
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:57:19  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.5  1995/11/08  21:38:02  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.4  1995/09/23  22:09:27  dkatz
 * CSCdi38152:  NLSP parameters cant be set on subinterfaces
 *
 * Revision 2.3  1995/08/25  15:00:05  dkatz
 * CSCdi38139:  ISIS needs better control of adjacency parameters
 *
 * Revision 2.2  1995/06/21  18:18:28  widmer
 * CSCdi36227:  Fix parse chain alternates
 *
 * Revision 2.1  1995/06/07  23:27:04  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************************
 * ipx nlsp [<tag-id>] metric <num>
 * no ipx nlsp metric
 *
 * OBJ(int,1) = value
 * OBJ(string,1) = tag-id
 */
pdecl  (ci_ipx_nlsp_opts);

STRING (ci_ipx_nlsp_tag, ci_ipx_nlsp_opts, NONE, OBJ(string,1),
	"Routing process tag");
IFELSE (ci_ipx_nlsp_ifelse, ci_ipx_nlsp_tag, NONE, !*GETOBJ(string,1));
TEST_MULTIPLE_FUNCS(ci_ipx_nlsp_test, ci_ipx_nlsp_ifelse, no_alt, NONE);

EOLS    (ci_ipx_nlsp_metric_eol, nlsp_if_command, 
	 IPX_NLSP_METRIC);
NUMBER  (ci_ipx_nlsp_metric_value, ci_ipx_nlsp_metric_eol, 
	 no_alt, OBJ(int,1), 0, ISIS_MAX_METRIC,
	 "Metric value");
NOPREFIX(ci_ipx_nlsp_metric_noprefix, ci_ipx_nlsp_metric_value,
	 ci_ipx_nlsp_metric_eol);
NVGENS  (ci_ipx_nlsp_metric_nvgen, ci_ipx_nlsp_metric_noprefix,
	 nlsp_if_command, IPX_NLSP_METRIC);
KEYWORD (ci_ipx_nlsp_metric, ci_ipx_nlsp_metric_nvgen, 
	 ci_ipx_nlsp_test, "metric",
	 "Set link metric", PRIV_CONF|PRIV_SUBIF);

/******************************************************************************
 * ipx nlsp [<tag>] priority <num>
 * no ipx nlsp priority
 *
 * OBJ(int,1) = value (or default value)
 * OBJ(string,1) = tag-id
 */
EOLS    (ci_ipx_nlsp_prio_eol, nlsp_if_command, 
	 IPX_NLSP_PRIO);
NUMBER  (ci_ipx_nlsp_prio_value, ci_ipx_nlsp_prio_eol, 
	 no_alt, OBJ(int,1), 0, ISIS_MAX_PRIO,
	 "Priority value");
NOPREFIX(ci_ipx_nlsp_prio_noprefix, ci_ipx_nlsp_prio_value,
	 ci_ipx_nlsp_prio_eol);
SET     (ci_ipx_nlsp_prio_set, ci_ipx_nlsp_prio_noprefix,
	 OBJ(int,1), IPX_NLSP_PRIO_DEFAULT);
NVGENS  (ci_ipx_nlsp_prio_nvgen, ci_ipx_nlsp_prio_set,
	 nlsp_if_command, IPX_NLSP_PRIO);
KEYWORD (ci_ipx_nlsp_prio, ci_ipx_nlsp_prio_nvgen, 
	 ci_ipx_nlsp_metric, "priority",
	 "Set designated router election priority", PRIV_CONF|PRIV_SUBIF);

/******************************************************************************
 * ipx nlsp [<tag-id>] hello-multiplier <num>
 * no ipx nlsp hello-multiplier
 *
 * OBJ(int,1) = value
 * OBJ(string,1) = tag-id
 */
EOLS    (ci_ipx_nlsp_hello_mult_eol, nlsp_if_command, 
	 IPX_NLSP_HELLO_MULT);
NUMBER  (ci_ipx_nlsp_hello_mult_value, ci_ipx_nlsp_hello_mult_eol, 
	 no_alt, OBJ(int,1), 3, 1000,
	 "HELLO holding time multiplier");
NOPREFIX(ci_ipx_nlsp_hello_mult_noprefix, ci_ipx_nlsp_hello_mult_value,
	 ci_ipx_nlsp_hello_mult_eol);
NVGENS  (ci_ipx_nlsp_hello_mult_nvgen, ci_ipx_nlsp_hello_mult_noprefix,
	 nlsp_if_command, IPX_NLSP_HELLO_MULT);
KEYWORD (ci_ipx_nlsp_hello_mult, ci_ipx_nlsp_hello_mult_nvgen, 
	 ci_ipx_nlsp_prio, "hello-multiplier",
	 "Set hello holding time multiplier", PRIV_CONF|PRIV_SUBIF);

/******************************************************************************
 * ipx nlsp [<tag-id>] hello-interval <num>
 * no ipx nlsp hello-interval
 *
 * OBJ(int,1) = value (or default value)
 * OBJ(string,1) = tag-id
 */
EOLS    (ci_ipx_nlsp_hello_ivl_eol, nlsp_if_command, 
	 IPX_NLSP_HELLO_IVL);
NUMBER  (ci_ipx_nlsp_hello_ivl_value, ci_ipx_nlsp_hello_ivl_eol, 
	 no_alt, OBJ(int,1), 1, IPX_NLSP_HELLO_IVL_MAX,
	 "HELLO interval in seconds");
NOPREFIX(ci_ipx_nlsp_hello_ivl_noprefix, ci_ipx_nlsp_hello_ivl_value,
	 ci_ipx_nlsp_hello_ivl_eol);
SET     (ci_ipx_nlsp_hello_ivl_set, ci_ipx_nlsp_hello_ivl_noprefix,
	 OBJ(int,1), IPX_NLSP_HELLO_IVL_DEFAULT);
NVGENS  (ci_ipx_nlsp_hello_ivl_nvgen, ci_ipx_nlsp_hello_ivl_set,
	 nlsp_if_command, IPX_NLSP_HELLO_IVL);
KEYWORD (ci_ipx_nlsp_hello_ivl, ci_ipx_nlsp_hello_ivl_nvgen, 
	 ci_ipx_nlsp_hello_mult, "hello-interval",
	 "Set hello transmission interval", PRIV_CONF|PRIV_SUBIF);

/******************************************************************************
 * ipx nlsp [<tag-id>] lsp-interval <num>
 * no ipx nlsp lsp-interval
 *
 * OBJ(int,1) = value (0 = set per bandwidth/throughput or use default)
 * OBJ(string,1) = tag-id
 */
EOLS    (ci_ipx_nlsp_lsp_ivl_eol, nlsp_if_command, 
	 IPX_NLSP_LSPIVL_CMD);
NUMBER  (ci_ipx_nlsp_lsp_ivl_value, ci_ipx_nlsp_lsp_ivl_eol, 
	 no_alt, OBJ(int,1), IPX_NLSP_LSPIVL_MIN, IPX_NLSP_LSPIVL_MAX,
	 "mSec");
NOPREFIX(ci_ipx_nlsp_lsp_ivl_noprefix,
	 ci_ipx_nlsp_lsp_ivl_value, ci_ipx_nlsp_lsp_ivl_eol);
SET     (ci_ipx_nlsp_lsp_ivl_set, ci_ipx_nlsp_lsp_ivl_noprefix,
	 OBJ(int,1), IPX_NLSP_LSPIVL_MIN);
NVGENS  (ci_ipx_nlsp_lsp_ivl_nvgen, ci_ipx_nlsp_lsp_ivl_set,
	 nlsp_if_command, IPX_NLSP_LSPIVL_CMD);
KEYWORD (ci_ipx_nlsp_lsp_ivl,
	 ci_ipx_nlsp_lsp_ivl_nvgen, ci_ipx_nlsp_hello_ivl,
	 "lsp-interval", "Set LSP transmission (pacing) interval",
	 PRIV_CONF|PRIV_SUBIF);

/******************************************************************************
 * ipx nlsp [<tag-id>] csnp-interval <num>
 * no ipx nlsp csnp-interval
 *
 * OBJ(int,1) = value (or default value)
 * OBJ(string,1) = tag-id
 */
EOLS    (ci_ipx_nlsp_csnp_ivl_eol, nlsp_if_command, 
	 IPX_NLSP_CSNP_IVL);
NUMBER  (ci_ipx_nlsp_csnp_ivl_value, ci_ipx_nlsp_csnp_ivl_eol, 
	 no_alt, OBJ(int,1), 1, IPX_NLSP_CSNP_IVL_MAX,
	 "CSNP interval in seconds");
NOPREFIX(ci_ipx_nlsp_csnp_ivl_noprefix, ci_ipx_nlsp_csnp_ivl_value,
	 ci_ipx_nlsp_csnp_ivl_eol);
SET     (ci_ipx_nlsp_csnp_ivl_set, ci_ipx_nlsp_csnp_ivl_noprefix,
	 OBJ(int,1), IPX_NLSP_CSNP_IVL_DEFAULT);
NVGENS  (ci_ipx_nlsp_csnp_ivl_nvgen, ci_ipx_nlsp_csnp_ivl_set,
	 nlsp_if_command, IPX_NLSP_CSNP_IVL);
KEYWORD (ci_ipx_nlsp_csnp_ivl, ci_ipx_nlsp_csnp_ivl_nvgen, 
	 ci_ipx_nlsp_lsp_ivl, "csnp-interval",
	 "Set CSNP transmission interval", PRIV_CONF|PRIV_SUBIF);

/******************************************************************************
 * ipx nlsp [<tag-id>] retransmit-interval <num>
 * no ipx nlsp retransmit-interval
 *
 * OBJ(int,1) = value
 * OBJ(string,1) = tag-id
 */
EOLS    (ci_ipx_nlsp_retrans_ivl_eol, nlsp_if_command, 
	 IPX_NLSP_RETRANS_IVL);
NUMBER  (ci_ipx_nlsp_retrans_ivl_value, ci_ipx_nlsp_retrans_ivl_eol, 
	 no_alt, OBJ(int,1), 1, IPX_NLSP_RETRANS_IVL_MAX,
	 "Retransmission interval in seconds");
NOPREFIX(ci_ipx_nlsp_retrans_ivl_noprefix, ci_ipx_nlsp_retrans_ivl_value,
	 ci_ipx_nlsp_retrans_ivl_eol);
NVGENS  (ci_ipx_nlsp_retrans_ivl_nvgen, ci_ipx_nlsp_retrans_ivl_noprefix,
	 nlsp_if_command, IPX_NLSP_RETRANS_IVL);
KEYWORD (ci_ipx_nlsp_retrans_ivl, ci_ipx_nlsp_retrans_ivl_nvgen, 
	 ci_ipx_nlsp_csnp_ivl, "retransmit-interval",
	 "Set LSP retransmission interval", PRIV_CONF|PRIV_SUBIF);

/************************************************************************
 *	[no] ipx nlsp [<tag-id>] sap [on | off | auto]
 *
 * OBJ(int,1) = mode (IPX_RIPSAP_{ON|OFF|AUTO}) 
 * OBJ(string,1) = tag-id
 */
EOLS	(ci_ipx_nlsp_sap_eol, nlsp_if_command, IPX_NLSP_SAP_MODE);
KEYWORD_ID (ci_ipx_nlsp_sap_auto, ci_ipx_nlsp_sap_eol, no_alt,
	    OBJ(int,1), IPX_RIPSAP_AUTO,
	    "auto", "Enable SAP only if a SAP router is detected",
	    PRIV_CONF|PRIV_SUBIF);
KEYWORD_ID (ci_ipx_nlsp_sap_off, ci_ipx_nlsp_sap_eol, ci_ipx_nlsp_sap_auto,
	    OBJ(int,1), IPX_RIPSAP_OFF, "off", "Disable SAP unconditionally",
	    PRIV_CONF|PRIV_SUBIF);
KEYWORD_ID (ci_ipx_nlsp_sap_on, ci_ipx_nlsp_sap_eol, ci_ipx_nlsp_sap_off,
	    OBJ(int,1), IPX_RIPSAP_ON, "on", "Enable SAP unconditionally",
	    PRIV_CONF|PRIV_SUBIF);
KEYWORD	(ci_ipx_nlsp_sap_key, ci_ipx_nlsp_sap_on, ci_ipx_nlsp_retrans_ivl,
	 "sap", "SAP compatibility mode",
	 PRIV_CONF|PRIV_SUBIF);

/************************************************************************
 *	[no] ipx nlsp [<tag-id>] rip [on | off | auto]
 *
 * OBJ(int,1) = mode (IPX_RIPSAP_{ON|OFF|AUTO})
 * OBJ(string,1) = tag-id
 */
EOLS	(ci_ipx_nlsp_rip_eol, nlsp_if_command, IPX_NLSP_RIP_MODE);
KEYWORD_ID (ci_ipx_nlsp_rip_auto, ci_ipx_nlsp_rip_eol, no_alt,
	    OBJ(int,1), IPX_RIPSAP_AUTO,
	    "auto", "Enable RIP only if a RIP router is detected",
	    PRIV_CONF|PRIV_SUBIF);
KEYWORD_ID (ci_ipx_nlsp_rip_off, ci_ipx_nlsp_rip_eol, ci_ipx_nlsp_rip_auto,
	    OBJ(int,1), IPX_RIPSAP_OFF, "off", "Disable RIP unconditionally",
	    PRIV_CONF|PRIV_SUBIF);
KEYWORD_ID (ci_ipx_nlsp_rip_on, ci_ipx_nlsp_rip_eol, ci_ipx_nlsp_rip_off,
	    OBJ(int,1), IPX_RIPSAP_ON, "on", "Enable RIP unconditionally",
	    PRIV_CONF|PRIV_SUBIF);
KEYWORD	(ci_ipx_nlsp_rip_key, ci_ipx_nlsp_rip_on, ci_ipx_nlsp_sap_key,
	 "rip", "RIP compatibility mode",
	 PRIV_CONF|PRIV_SUBIF);


/************************************************************************
 *	[no] ipx nlsp [<tag-id>] enable 
 *
 * OBJ(string,1) = tag-id
 */
EOLS	(ci_ipx_nlsp_enable_eol, nlsp_if_command, IPX_NLSP_ENABLE);
KEYWORD (ci_ipx_nlsp_enable, ci_ipx_nlsp_enable_eol, ci_ipx_nlsp_rip_key,
	 "enable", "Enable NLSP on this interface",
	 PRIV_CONF|PRIV_SUBIF);

NOP (ci_ipx_nlsp_opts, ci_ipx_nlsp_enable, NONE);
NVGENS  (ci_ipx_nlsp_enable_nvgen, ci_ipx_nlsp_opts,
	 nlsp_if_command, IPX_NLSP_ENABLE);

KEYWORD	(ci_ipx_nlsp_key, ci_ipx_nlsp_enable_nvgen, no_alt,
	 "nlsp", "Select NLSP commands",
	 PRIV_CONF|PRIV_SUBIF);

#undef  ALTERNATE
#define ALTERNATE ci_ipx_nlsp_key
