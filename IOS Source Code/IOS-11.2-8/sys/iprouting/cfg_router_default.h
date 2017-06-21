/* $Id: cfg_router_default.h,v 3.3.20.4 1996/08/12 16:03:42 widmer Exp $
 * $Source: /release/112/cvs/Xsys/iprouting/cfg_router_default.h,v $
 *------------------------------------------------------------------
 * C F G _ R O U T E R _ D E F A U L T S . H
 * 
 * Copyright (c) 1992-1997 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_router_default.h,v $
 * Revision 3.3.20.4  1996/08/12  16:03:42  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.3.20.3  1996/05/29  01:49:48  myeung
 * CSCdi58666:  OSPF default-information originate metric is limited to
 * value 65535.
 * Branch: California_branch
 * - Increase the limit to 16777214 (0xffffff - 1).
 *
 * Revision 3.3.20.2  1996/05/04  01:21:15  wilber
 * Branch: California_branch
 *
 * Constrained Multicast Flooding
 * - Define MOSPF hello option bit.
 *
 * Name Access List
 *
 * Revision 3.3.20.1  1996/04/02  07:29:12  dkatz
 * CSCdi52277:  Redistribution metric problems with IGRP/EIGRP
 * Branch: California_branch
 * Always store canonical metrics scaled for IGRP, and make EIGRP do
 * its own scaling internally.  Get the scaling right in all three
 * places.
 *
 * Revision 3.3  1996/02/01  23:33:56  widmer
 * CSCdi45325:  Change Keyword/Set to use Keyword_Id macro
 *
 * Revision 3.2  1995/11/17  17:32:04  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:05:28  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/01  12:15:49  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize iprouting
 *
 * Revision 2.1  1995/06/07  22:12:28  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/**********************************************************************
 *	default-metric <number>
 *	default-metric <bandwidth> <delay> <reliability> <loading> <mtu>
 *	no default-metric [ <metrics> ]
 *
 * OBJ(int,1) = <number> or <bandwidth>
 * OBJ(int,2) = <delay>
 * OBJ(int,3) = <reliability>
 * OBJ(int,4) = <loading>
 * OBJ(int,5) = <mtu>
 *
 * We check the current protocol to see if it is IGRP and we should
 * get additional parameters.
 */

EOLNS	(rtr_defmet_eol, defaultmetric_command);

/* <metric> */
NUMBER	(rtr_defmet_not_igrp_metric, rtr_defmet_eol, no_alt,
	 OBJ(int,1), 1, -1, "Default metric");

/* <mtu> */
NUMBER	(rtr_defmet_igrp_mtu, rtr_defmet_eol, no_alt,
	 OBJ(int,5), 1, -1,
	 "IGRP Maximum Transmission Unit metric of the path ");

/* <loading> */
NUMBER(rtr_defmet_igrp_loading, rtr_defmet_igrp_mtu, no_alt,
       OBJ(int,4), 1, 255,
       "IGRP Effective bandwidth metric (Loading) where 255 is 100% loaded");

/* <reliability> */
NUMBER	(rtr_defmet_igrp_reliability, rtr_defmet_igrp_loading, no_alt,
	 OBJ(int,3), 0, 255,
	 "IGRP reliability metric where 255 is 100% reliable");

/* igrp <delay> */
NUMBER	(rtr_defmet_igrp_delay, rtr_defmet_igrp_reliability, no_alt,
	 OBJ(int,2), 0, -1, "IGRP delay metric, in 10 microsecond units");

/* <bandwidth> for IGRP */
NUMBER	(rtr_defmet_igrp_bandwidth, rtr_defmet_igrp_delay, no_alt,
	 OBJ(int,1), 1, -1, "IGRP bandwidth metric in kilobits per second");

IFELSE (rtr_defmet_igrp, rtr_defmet_igrp_bandwidth, rtr_defmet_not_igrp_metric,
	(((pdbtype *)csb->protocol)->proctype & (PROC_IGRP2 | PROC_IGRP)));

NOPREFIX (rtr_defmet_noprefix, rtr_defmet_igrp, rtr_defmet_eol);
NVGENNS	(rtr_defmet_nvgen, rtr_defmet_noprefix, defaultmetric_command);

KEYWORD	(rtr_defmet, rtr_defmet_nvgen, NONE,
	 "default-metric", "Set metric of redistributed routes", PRIV_CONF);
ASSERT	(router_defmet, rtr_defmet, ALTERNATE,
	 ((pdbtype *) csb->protocol)->family == PDB_IP);

/**********************************************************************
 *  IGRP only:
 * default-information [allowed] { in | out }
 * no default-information [allowed] { in | out }
 *
 *  EGP & BGP only:
 * [no] default-information originate
 *
 *  OSPF only:
 * default-information originate [ metric <value> | metric-type { 1 | 2 } | 
 * route-map <map> ]
 * no default-information originate [ metric <value> | metric-type { 1 | 2 } |
 * route-map <map> ]
 *
 *  ISIS & RIP only:
 * default-information originate [ route-map <map> ]
 * [no] default-information originate [ route-map <map> ]
 *
 * The optional keyword [allowed] is there for backwards compatability.  It
 * should be visible to the user.
 *
 * The parse chain loops for the OSPF options because the original cisco code
 * did the same.
 *
 * OBJ(int,1) = DEFAULTINF_IN, DEFAULTINF_OUT, DEFAULTINF_ORIGINATE
 * OBJ(int,2) = TRUE if 'metric <value>'
 * OBJ(int,3) = <value>
 * OBJ(int,4) = TRUE if 'metric-type { 1 | 2 }'
 * OBJ(int,5) = 1 or 2
 * OBJ(int,6) = TRUE if 'always'
 * OBJ(string,1) = non-NULL if route-map specified
 * OBJ(string,2) = ACL name
 */

EOLNS	(rtr_definfo_eol, defaultinf_command);

pdecl(rtr_definfo_opts);  /* Forward reference for looping */

/* route-map */
STRING (rtr_definfo_routemap_str, rtr_definfo_opts, no_alt, 
	OBJ(string,1), "Route map name");
KEYWORD(rtr_definfo_routemap, rtr_definfo_routemap_str, rtr_definfo_eol,
	"route-map", "Route-map reference", PRIV_CONF);

/* always */
KEYWORD_ID(rtr_definfo_always, rtr_definfo_opts, rtr_definfo_routemap,
	   OBJ(int,6), TRUE, "always", "Always advertise default route", 
	   PRIV_CONF);

/* metric-type { 1 | 2 } */
NUMBER	(rtr_definfo_mtype_val, rtr_definfo_opts, no_alt,
	 OBJ(int,5), 1, 2, "OSPF Link State type");
KEYWORD_ID_MM(rtr_definfo_metrictype, rtr_definfo_mtype_val,
	      rtr_definfo_always,
	      OBJ(int,4), TRUE,
	      "metric-type", "OSPF metric type for default routes",
	      PRIV_CONF, 7);

/* metric <value> */
NUMBER	(rtr_definfo_metric_val, rtr_definfo_opts, no_alt,
	 OBJ(int,3), 0, (0xffffff - 1), "OSPF metric");
KEYWORD_ID (rtr_definfo_metric, rtr_definfo_metric_val, rtr_definfo_metrictype,
	    OBJ(int,2), TRUE,
	    "metric", "OSPF default metric", PRIV_CONF);

IFELSE (rtr_definfo_isis_rip, rtr_definfo_routemap, rtr_definfo_metric,
	 (((pdbtype *) csb->protocol)->family == PDB_OSI &&
	 ((clns_pdbtype *) csb->protocol)->typeflag & ISO_ISISMASK) || 
         (((pdbtype *) csb->protocol)->family == PDB_IP &&
	     ((pdbtype *) csb->protocol)->proctype & PROC_RIP));

NOP	(rtr_definfo_opts, rtr_definfo_isis_rip, NONE);

/* originate */

IFELSE (rtr_definfo_egp, rtr_definfo_eol, rtr_definfo_opts,
	(((pdbtype *) csb->protocol)->family == PDB_IP &&
	 (((pdbtype *) csb->protocol)->proctype & (PROC_EGP|PROC_BGP))));

NVGENNS	(rtr_definfo_orig_nvgen, rtr_definfo_egp, defaultinf_command);
KEYWORD_ID (rtr_definfo_orig_kw, rtr_definfo_orig_nvgen, no_alt,
	    OBJ(int,1), DEFAULTINF_ORIGINATE,
	    "originate", "Distribute a default route", PRIV_CONF);

IFELSE (rtr_definfo_orig, rtr_definfo_orig_kw, no_alt,
	(((pdbtype *) csb->protocol)->family == PDB_IP &&
	 (((pdbtype *) csb->protocol)->proctype &
	  (PROC_RIP|PROC_BGP|PROC_EGP|PROC_OSPF))) ||
	 (((clns_pdbtype *) csb->protocol)->family == PDB_OSI &&
	  (((clns_pdbtype *) csb->protocol)->typeflag & ISO_ISISMASK)));

/* { in | out } */
GENERAL_STRING (rtr_definfo_accname, rtr_definfo_eol, no_alt,
		OBJ(string,2), "Standard Access-list name",
		STRING_HELP_CHECK); 
TEST_MULTIPLE_FUNCS(rtr_definfo_stest, rtr_definfo_accname, NONE,
		    no_alt);
NUMBER	(rtr_definfo_inout_list, rtr_definfo_eol, rtr_definfo_stest,
	 OBJ(int,2), MINFASTACC, MAXFASTACC,
	 "A standard IP access list number"); 
NOPREFIX (rtr_definfo_inout_noprefix, rtr_definfo_inout_list,
	  rtr_definfo_eol);
NVGENNS	(rtr_definfo_inout_nvgen, rtr_definfo_inout_noprefix,
         defaultinf_command);
KEYWORD_ID (rtr_definfo_out, rtr_definfo_inout_nvgen, rtr_definfo_orig,
	    OBJ(int,1), DEFAULTINF_OUT,
	    "out", "Output default routing information", PRIV_CONF);

KEYWORD_ID (rtr_definfo_in_kw, rtr_definfo_inout_nvgen, rtr_definfo_out,
	    OBJ(int,1), DEFAULTINF_IN,
	    "in", "Accept default routing information", PRIV_CONF);

KEYWORD	(rtr_definfo_allowed, rtr_definfo_in_kw, rtr_definfo_in_kw,
	 "allowed", "Allow default information", PRIV_CONF);

TESTVAR	(rtr_definfo_allowed_test, rtr_definfo_in_kw, rtr_definfo_allowed,
	 NONE, NONE, NONE,
	 nvgen, TRUE);

IFELSE (rtr_definfo_test, rtr_definfo_allowed_test, rtr_definfo_orig,
	(((pdbtype *) csb->protocol)->family == PDB_IP &&
	 (((pdbtype *) csb->protocol)->proctype &
	  (PROC_IGRP|PROC_IGRP2))));

KEYWORD (rtr_defaults, rtr_definfo_test, router_defmet,
	 "default-information", "Control distribution of default information", 
	 PRIV_CONF);
IFELSE (router_defaults, rtr_defaults, router_defmet,
	 (((pdbtype *) csb->protocol)->family == PDB_IP &&
	   (((pdbtype *) csb->protocol)->proctype & 
	    (PROC_RIP|PROC_IGRP|PROC_IGRP2|PROC_BGP|PROC_EGP|PROC_OSPF))) ||
	 (((clns_pdbtype *) csb->protocol)->family == PDB_OSI &&
	  (((clns_pdbtype *) csb->protocol)->typeflag & ISO_ISISMASK)));

#undef	ALTERNATE
#define ALTERNATE	router_defaults
