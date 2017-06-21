/* $Id: cfg_int_isis.h,v 3.3.20.4 1996/08/11 15:12:49 dkatz Exp $
 * $Source: /release/112/cvs/Xsys/clns/cfg_int_isis.h,v $
 *------------------------------------------------------------------
 * C F G _ I N T _ I S I S . H
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_int_isis.h,v $
 * Revision 3.3.20.4  1996/08/11  15:12:49  dkatz
 * CSCdi58739:  ISIS needs to be able to limit flooding
 * Branch: California_branch
 * Add phase I of mesh group support.  Allow an interface to be
 * configured to be part of a mesh group, or to be blocked for
 * flooding.  Allow CSNP transmission to be configured on point-to-
 * point interfaces as a minimal robustness mechanism.  Redefine
 * the SRM, etc., bits as an array of ulongs rather than bytes (to
 * speed up masking operations).  Change the interface number field
 * from a short to a long in order to shrink the code size by a few
 * hundred bytes.
 *
 * Revision 3.3.20.3  1996/07/10  10:08:40  dawnli
 * CSCdi54313:  ISIS: Negation of interface commands are not consistant
 * Branch: California_branch
 *
 * Revision 3.3.20.2  1996/06/18  16:50:32  dkatz
 * CSCdi59965:  ISIS retransmits too quickly
 * Branch: California_branch
 * Add a retransmission throttling mechanism.
 *
 * Revision 3.3.20.1  1996/05/07  17:15:42  dkatz
 * CSCdi54576:  ISIS LSP pacing rate needs to be configurable
 * Branch: California_branch
 * Provide a knob to alter the LSP pacing timer;  the timer is already
 * there.
 *
 * Revision 3.3  1996/02/01  23:33:01  widmer
 * CSCdi45325:  Change Keyword/Set to use Keyword_Id macro
 *
 * Revision 3.2  1995/11/17  08:51:25  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:06:57  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/08/25  14:59:31  dkatz
 * CSCdi38139:  ISIS needs better control of adjacency parameters
 *
 * Revision 2.1  1995/07/02  01:46:57  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize xns, atalk, vines, decnet, xremote, x25, unix, wan, tn3270,
 * clns, cdp
 *
 * Revision 2.1  1995/06/07  22:05:55  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************************
 * isis adjacency-filter <name> [match-all]
 * no isis adjacency-filter
 *
 * OBJ(string,1) = filter name
 * OBJ(int,1) = non-zero if match-all selected
 */
EOLS    (conf_isis_adjfil_eol, isis_intf_commands, ISIS_ADJFILTER_CMD);
KEYWORD_ID(conf_isis_adjfil_matchall, conf_isis_adjfil_eol,
	   conf_isis_adjfil_eol, OBJ(int,1), TRUE, "match-all",
	   "Match all area addresses", PRIV_CONF|PRIV_SUBIF);
STRING  (conf_isis_adjfil_name, conf_isis_adjfil_matchall, no_alt,
	 OBJ(string,1), "Name of filter expression or set");
NOPREFIX(conf_isis_adjfil_noprefix, conf_isis_adjfil_name,
	 conf_isis_adjfil_eol);
NVGENS  (conf_isis_adjfil_nvgen, conf_isis_adjfil_noprefix,
	 isis_intf_commands, ISIS_ADJFILTER_CMD);
KEYWORD (conf_isis_adj_filter, conf_isis_adjfil_nvgen, no_alt,
	 "adjacency-filter", "Filter IS-IS adjacencies", PRIV_CONF|PRIV_SUBIF);

/******************************************************************************
 * isis mesh-group {<num> | blocked}
 * no isis mesh-group
 *
 * OBJ(int,1) = mesh group number, or 0 if blocked
 */
EOLS    (conf_isis_meshgrp_eol, isis_intf_commands, ISIS_MESHGRP_CMD);
KEYWORD (conf_isis_meshgrp_blocked, conf_isis_meshgrp_eol, no_alt,
	 "blocked", "Block LSPs on this interface", PRIV_CONF|PRIV_SUBIF);
NUMBER  (conf_isis_meshgrp_number, conf_isis_meshgrp_eol,
	 conf_isis_meshgrp_blocked, OBJ(int,1), 1, MAXUINT,
	 "Mesh group number");
NOPREFIX(conf_isis_meshgrp_noprefix, conf_isis_meshgrp_number,
	 conf_isis_meshgrp_eol);
NVGENS  (conf_isis_meshgrp_nv, conf_isis_meshgrp_noprefix, isis_intf_commands,
	 ISIS_MESHGRP_CMD);
KEYWORD (conf_isis_meshgrp, conf_isis_meshgrp_nv, conf_isis_adj_filter,
	 "mesh-group", "Set ISIS mesh group", PRIV_CONF|PRIV_SUBIF);

/******************************************************************************
 * isis lsp-interval <num>
 * no isis lsp-interval
 */
PARAMS_KEYONLY(conf_isis_lspivl, conf_isis_meshgrp, "lsp-interval",
	       OBJ(int,1), 1, MAXUINT,
	       isis_intf_commands, ISIS_LSPIVL_CMD,
	       "Set LSP transmission interval",
	       "LSP transmission interval (milliseconds)",
	       PRIV_CONF|PRIV_SUBIF);

/******************************************************************************
 * isis retransmit-interval <num>
 * no isis retransmit-interval
 */
PARAMS_KEYONLY(conf_isis_retrans, conf_isis_lspivl, "retransmit-interval",
	       OBJ(int,1), 0, 0xffff,
	       isis_intf_commands, ISIS_RETRANS_CMD,
	       "Set per-LSP retransmission interval",
	       "Interval between retransmissions of the same LSP (seconds)", 
	       PRIV_CONF|PRIV_SUBIF);

/******************************************************************************
 * isis retransmit-throttle-interval <num>
 * no isis retransmit-throttle-interval
 */
PARAMS_KEYONLY(conf_isis_minrexmit, conf_isis_retrans,
	       "retransmit-throttle-interval", OBJ(int,1), 0, 0xffff,
	       isis_intf_commands, ISIS_MIN_REXMIT_CMD,
	       "Set interface LSP retransmission interval",
	       "Delay between retransmitted LSPs (milliseconds)", 
	       PRIV_CONF|PRIV_SUBIF);

/******************************************************************************
 * isis csnp-interval <num> { level-1 | level-2 }
 * no isis csnp-interval { level-1 | level-2 }
 */
EOLS (conf_isis_csnp_eol, isis_intf_commands, ISIS_CSNP_CMD);
KEYWORD_ID(conf_isis_csnp_level2, conf_isis_csnp_eol, conf_isis_csnp_eol,
	   OBJ(int,2), ISIS_CIRCUIT_L2, "level-2",
	   "Specify interval for level-2 CSNPs", PRIV_CONF|PRIV_SUBIF);
KEYWORD_ID(conf_isis_csnp_level1, conf_isis_csnp_eol, conf_isis_csnp_level2,
	   OBJ(int,2), ISIS_CIRCUIT_L1,
	   "level-1", "Specify interval for level-1 CSNPs",
	   PRIV_CONF|PRIV_SUBIF);
SET	(conf_isis_csnp_level, conf_isis_csnp_level1,
	 OBJ(int,2), ISIS_CIRCUIT_L1);
SPLIT   (conf_isis_csnp_noprefix, conf_isis_csnp_level,
         no_alt, sense, FALSE);
NUMBER	(conf_isis_csnp_num, conf_isis_csnp_level, conf_isis_csnp_noprefix,
	 OBJ(int,1), 0, 0xffff, "CSNP interval value");
NVGENS	(conf_isis_csnp_nvgen, conf_isis_csnp_num,
	 isis_intf_commands, ISIS_CSNP_CMD);
KEYWORD	(conf_isis_csnp, conf_isis_csnp_nvgen, conf_isis_minrexmit,
	 "csnp-interval", "Set CSNP interval in seconds",
	 PRIV_CONF|PRIV_SUBIF);

/******************************************************************************
 * isis hello-interval <num> { level-1 | level-2 }
 * no isis hello-interval { level-1 | level-2 }
 */
EOLS (conf_isis_hello_eol, isis_intf_commands, ISIS_HELLO_CMD);
KEYWORD_ID(conf_isis_hello_level2, conf_isis_hello_eol, conf_isis_hello_eol,
	   OBJ(int,2), ISIS_CIRCUIT_L2,
	   "level-2", "Specify hello-interval for level-2 IIHs",
	   PRIV_CONF|PRIV_SUBIF);
KEYWORD_ID(conf_isis_hello_level1, conf_isis_hello_eol, conf_isis_hello_level2,
	   OBJ(int,2), ISIS_CIRCUIT_L1,
	   "level-1", "Specify hello-interval for level-1 IIHs",
	   PRIV_CONF|PRIV_SUBIF);
SET	(conf_isis_hello_level, conf_isis_hello_level1,
	 OBJ(int,2), ISIS_CIRCUIT_L1);
SPLIT   (conf_isis_hello_noprefix, conf_isis_hello_level,
         no_alt, sense, FALSE);
NUMBER	(conf_isis_hello_num, conf_isis_hello_level, conf_isis_hello_noprefix,
         OBJ(int,1), 0, 0xffff, "Hello interval value");
NVGENS	(conf_isis_hello_nvgen, conf_isis_hello_num,
	 isis_intf_commands, ISIS_HELLO_CMD);
KEYWORD	(conf_isis_hello, conf_isis_hello_nvgen, conf_isis_csnp,
	 "hello-interval", "Set Hello interval in seconds",
	 PRIV_CONF|PRIV_SUBIF);

/******************************************************************************
 * isis hello-multiplier <num> { level-1 | level-2 }
 * no isis hello-multiplier { level-1 | level-2 }
 */
EOLS (conf_isis_hello_mult_eol, isis_intf_commands, ISIS_HELLO_MULT_CMD);
KEYWORD_ID (conf_isis_hello_mult_level2, conf_isis_hello_mult_eol,
	    conf_isis_hello_mult_eol, OBJ(int,2), ISIS_CIRCUIT_L2, "level-2",
	    "Specify hello multiplier for level-2 IIHs", PRIV_CONF|PRIV_SUBIF);
KEYWORD_ID (conf_isis_hello_mult_level1, conf_isis_hello_mult_eol,
	    conf_isis_hello_mult_level2, OBJ(int,2), ISIS_CIRCUIT_L1,
	    "level-1", "Specify hello multiplier for level-1 IIHs",
	    PRIV_CONF|PRIV_SUBIF);
SET	(conf_isis_hello_mult_level, conf_isis_hello_mult_level1,
	 OBJ(int,2), ISIS_CIRCUIT_L1);
SPLIT   (conf_isis_hello_mult_noprefix, conf_isis_hello_mult_level,
         no_alt, sense, FALSE);
NUMBER	(conf_isis_hello_mult_num, conf_isis_hello_mult_level,
         conf_isis_hello_mult_noprefix,
	 OBJ(int,1), 3, 1000, "Hello multiplier value");
NVGENS	(conf_isis_hello_mult_nvgen, conf_isis_hello_mult_num,
	 isis_intf_commands, ISIS_HELLO_MULT_CMD);
KEYWORD (conf_isis_hello_mult, conf_isis_hello_mult_nvgen, conf_isis_hello,
	 "hello-multiplier", "Set multiplier for Hello holding time",
	 PRIV_CONF|PRIV_SUBIF);

/******************************************************************************
 * isis priority <num> { level-1 | level-2 }
 * no isis priority { level-1 | level-2 }
 */
EOLS	(conf_isis_pri_eol, isis_intf_commands, ISIS_PRIO_CMD);
KEYWORD_ID(conf_isis_pri_level2, conf_isis_pri_eol, conf_isis_pri_eol,
	   OBJ(int,2), ISIS_CIRCUIT_L2,
	   "level-2", "Specify priority for level-2 routing",
	   PRIV_CONF|PRIV_SUBIF);
KEYWORD_ID(conf_isis_pri_level1, conf_isis_pri_eol, conf_isis_pri_level2,
	   OBJ(int,2), ISIS_CIRCUIT_L1,
	   "level-1", "Specify priority for level-1 routing",
	   PRIV_CONF|PRIV_SUBIF);
SET(conf_isis_pri_level, conf_isis_pri_level1, OBJ(int,2), ISIS_CIRCUIT_L1);
SPLIT	(conf_isis_pri_noprefix, conf_isis_pri_level, no_alt, sense, FALSE);
NUMBER	(conf_isis_pri_num, conf_isis_pri_level, conf_isis_pri_noprefix,
	 OBJ(int,1), 0, ISIS_MAX_PRIO, "Priority value");
NVGENS	(conf_isis_pri_nvgen, conf_isis_pri_num,
	 isis_intf_commands, ISIS_PRIO_CMD);
KEYWORD	(conf_isis_priority, conf_isis_pri_nvgen, conf_isis_hello_mult,
	 "priority", "Set priority for Designated Router election",
	 PRIV_CONF|PRIV_SUBIF);

/******************************************************************************
 * isis password <password> { level-1 | level-2 }
 * no isis password { level-1 | level-2 }
 */
EOLS	(conf_isis_pw_eol, isis_intf_commands, ISIS_PW_CMD);
KEYWORD_ID(conf_isis_pw_level2, conf_isis_pw_eol, conf_isis_pw_eol,
	   OBJ(int,1), ISIS_CIRCUIT_L2,
	   "level-2", "Specify password for level-2 PDUs",
	   PRIV_CONF|PRIV_SUBIF);
KEYWORD_ID(conf_isis_pw_level1, conf_isis_pw_eol, conf_isis_pw_level2,
	   OBJ(int,1), ISIS_CIRCUIT_L1,
	   "level-1", "Specify password for level-1 PDUs",
	   PRIV_CONF|PRIV_SUBIF);
SET(conf_isis_pw_level, conf_isis_pw_level1, OBJ(int,1), ISIS_CIRCUIT_L1);
SPLIT	(conf_isis_pw_noprefix, conf_isis_pw_level, no_alt, sense, FALSE );
STRING	(conf_isis_pw_password, conf_isis_pw_level, conf_isis_pw_noprefix,
	 OBJ(string,1), "Password");
NVGENS	(conf_isis_pw_nvgen, conf_isis_pw_password,
	 isis_intf_commands, ISIS_PW_CMD);
KEYWORD	(conf_isis_password, conf_isis_pw_nvgen, conf_isis_priority,
	 "password",
	 "Configure the authentication password for interface",
	 PRIV_CONF|PRIV_SUBIF);

/******************************************************************************
 * isis metric <default> <delay> <expense> <error> { level-1 | level-2 }
 * no isis metric { level-1 | level-2 }
 *
 */
EOLS	(conf_isis_metric_eol, isis_intf_commands, ISIS_METRIC_CMD);
KEYWORD_ID(conf_isis_metric_level2, conf_isis_metric_eol, conf_isis_metric_eol,
	   OBJ(int,5), ISIS_CIRCUIT_L2, "level-2",
	   "Apply metric to level-2 links", PRIV_CONF|PRIV_SUBIF);
KEYWORD_ID(conf_isis_metric_level1, conf_isis_metric_eol,
	   conf_isis_metric_level2,
	   OBJ(int,5), ISIS_CIRCUIT_L1,
	   "level-1", "Apply metric to level-1 links", PRIV_CONF|PRIV_SUBIF);
SET	(conf_isis_metric_level, conf_isis_metric_level1,
	 OBJ(int,5), ISIS_CIRCUIT_L1);
NUMBER	(conf_isis_metric_error, conf_isis_metric_level,
	 conf_isis_metric_level,
	 OBJ(int,4), 0, ISIS_MAX_METRIC, "Error metric");
NUMBER	(conf_isis_metric_expense, conf_isis_metric_error,
	 conf_isis_metric_level,
	 OBJ(int,3), 0, ISIS_MAX_METRIC, "Expense metric");
NUMBER	(conf_isis_metric_delay, conf_isis_metric_expense,
	 conf_isis_metric_level,
	 OBJ(int,2), 0, ISIS_MAX_METRIC, "Delay metric");
SPLIT	(conf_isis_metric_noprefix, conf_isis_metric_level,
         no_alt, sense, FALSE );
NUMBER	(conf_isis_metric_default, conf_isis_metric_delay,
         conf_isis_metric_noprefix,
	 OBJ(int,1), 0, ISIS_MAX_METRIC, "Default metric");
NVGENS	(conf_isis_metric_nvgen, conf_isis_metric_default,
	 isis_intf_commands, ISIS_METRIC_CMD);
KEYWORD	(conf_isis_metric, conf_isis_metric_nvgen, conf_isis_password,
	 "metric", "Configure the metric for interface", PRIV_CONF|PRIV_SUBIF);

/******************************************************************************
 * isis circuit-type { level-1-2 | level-1 | level-2-only }
 * no isis circuit-type
 */
EOLS	(conf_isis_circ_eol, isis_intf_commands, ISIS_CIRTYPE_CMD);
KEYWORD_ID(conf_isis_circ_level2only, conf_isis_circ_eol, conf_isis_circ_eol,
	   OBJ(int,1), ISIS_CIRCUIT_L2,
	   "level-2-only", "Level-2 only adjacencies are formed",
	   PRIV_CONF|PRIV_SUBIF);
KEYWORD_ID_MM(conf_isis_circ_level12, conf_isis_circ_eol,
	      conf_isis_circ_level2only,
	      OBJ(int,1), ISIS_CIRCUIT_L1L2,
	      "level-1-2",
	      "Level-1-2 adjacencies are formed", PRIV_CONF|PRIV_SUBIF, 9);
KEYWORD_ID(conf_isis_circ_level1, conf_isis_circ_eol, conf_isis_circ_level12,
	   OBJ(int,1), ISIS_CIRCUIT_L1,
	   "level-1", "Level-1 only adjacencies are formed",
	   PRIV_CONF|PRIV_SUBIF);
NOPREFIX(conf_isis_circ_noprefix, conf_isis_circ_level1, conf_isis_circ_eol);
KEYWORD_ID(conf_isis_circuit, conf_isis_circ_noprefix, conf_isis_metric,
	   OBJ(int,1), ISIS_CIRCUIT_L1L2,
	   "circuit-type", "Configure circuit type for interface",
	   PRIV_CONF|PRIV_SUBIF);


KEYWORD	(conf_isis, conf_isis_circuit, NONE,
	 "isis", "IS-IS commands", PRIV_CONF|PRIV_SUBIF);

ASSERT  (conf_isis_subif, conf_isis, ALTERNATE, main_if_or_vc(csb->interface));

#undef	ALTERNATE
#define	ALTERNATE	conf_isis_subif
