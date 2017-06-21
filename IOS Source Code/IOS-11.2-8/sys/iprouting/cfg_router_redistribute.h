/* $Id: cfg_router_redistribute.h,v 3.4.20.3 1996/08/12 16:03:50 widmer Exp $
 * $Source: /release/112/cvs/Xsys/iprouting/cfg_router_redistribute.h,v $
 *------------------------------------------------------------------
 * C F G _ R O U T E R _ R E D I S T R I B U T E . H
 *
 * Copyright (c) 1992-1997 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_router_redistribute.h,v $
 * Revision 3.4.20.3  1996/08/12  16:03:50  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.4.20.2  1996/04/15  21:17:44  bcole
 * CSCdi54649:  IP Host Modularity, IP Automatic Stub Routing feature
 * commit
 * Branch: California_branch
 *
 * Revision 3.4.20.1  1996/04/02  07:29:13  dkatz
 * CSCdi52277:  Redistribution metric problems with IGRP/EIGRP
 * Branch: California_branch
 * Always store canonical metrics scaled for IGRP, and make EIGRP do
 * its own scaling internally.  Get the scaling right in all three
 * places.
 *
 * Revision 3.4  1996/02/01  23:34:00  widmer
 * CSCdi45325:  Change Keyword/Set to use Keyword_Id macro
 *
 * Revision 3.3  1995/12/02  09:05:56  rchandra
 * CSCdi43948:  ISIS-IP LSP contain non-zero metric with I/E bit set
 * - prevent the parser from accepting more than 63 as metric for
 *   redistributed routes
 * - while adding the L1 learned routes into L2, do not allow the metric
 *   to be more than 63
 *
 * Revision 3.2  1995/11/17  17:32:23  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:05:59  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/01  12:16:01  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize iprouting
 *
 * Revision 2.1  1995/06/07  22:13:03  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/************************************************************************
 * [no] redistribute <protocol> [{
 *		     match { internal | external <extern-type> }
 *	  	     metric <metric-value> |
 *		     weight <weight-modifier> |
 *		     metric-type <metric-type> [internal|external]
 *	 	     tag <tag-value> |
 *		     subnets |
 *		     [level-1 | level-1-2 | level-2] }]
 *                   route-map <map value>
 *
 * For IS-IS:
 * [no] redistribute static [clns | ip]
 *
 * csb->OBJ(pdb,1) = <protocol>
 * OBJ(int,1) = REDIST_METRIC | REDIST_METRICTYPE | REDIST_TAG | REDIST_SUBNET
 *		REDIST_MATCH | REDIST_DEFAULT |
 *		REDIST_WEIGHT | REDIST_ROUTEMAP
 * OBJ(int,2) = RTYPE_INTRA | RTYPE_INTER | RTYPE_EXTERN1 | RTYPE_EXTERN2
 * OBJ(int,3) = <tag-value>
 * OBJ(int,4) = <metric-type>
 *
 * The metric is single valued for all protocols but IGRP, 
 * which takes a 5 valued metric.
 * OBJ(int,5) = <metric-value> or igrp <bandwidth>
 * OBJ(int,6) = <delay>
 * OBJ(int,7) = <reliability>
 * OBJ(int,8) = <loading>
 * OBJ(int,9) = <mtu>
 * OBJ(int,10) = level
 * OBJ(int,11) = 0 for "clns" or 1 for "ip" when redistributing static
 *		 into IS-IS.
 * OBJ(int,12) = <weight>
 * OBJ(string,1) = <map value>
 *
 * NOTE: This parse chain loops on all the options.  If no option matches the
 * input, then we transition to rtr_redist_eol, where an EOL is expected.
 *
 * Several parse chains have rtr_redist_*_noprefix as an alternate from their
 * values so that when entering a command prefixed by 'no', it will accept
 * with or without the value.  For the 'metric' option, only the first IGRP
 * metric is treated this way.
 */
pdecl (rtr_redist_opts);

EOLNS (rtr_redist_eol, redistribute_command);

/*
 * route-map <map value>
 */
pdecl (rtr_redist_routemap);
IFELSE (rtr_redist_routemap_clns_test, rtr_redist_routemap, rtr_redist_opts,
	 !strcmp(csb->router_name.myname, RTRNAME_STATIC) &&
	 !strcmp(((pdbtype *) csb->protocol)->name, RTRNAME_ISIS) &&
	 !GETOBJ(int,11));
STRING (rtr_redist_routemap_string, rtr_redist_routemap_clns_test, no_alt,
	OBJ(string,1), "Pointer to route-map entries");
KEYWORD_OR(rtr_redist_routemap, rtr_redist_routemap_string, rtr_redist_eol,
	   OBJ(int,1), REDIST_ROUTEMAP,
	   "route-map", "Route map reference", PRIV_CONF);

/* 
 * level-1 | level-2 | level-1-2 
 *
 * Accepted when redistributing IS-IS routes into any other protocol. Also
 * accepted when redistributing into IS-IS.
 */
EVAL (rtr_redist_level_setflag, rtr_redist_opts, 
      GETOBJ(int,1) |= REDIST_LEVEL);
KEYWORD_ID_MM(rtr_redist_level12, rtr_redist_level_setflag, NONE,
	      OBJ(int,10), ISIS_CIRCUIT_L1L2,
	      "level-1-2", "IS-IS level-1 and level-2 routes", PRIV_CONF, 8);
KEYWORD_ID (rtr_redist_level2, rtr_redist_level_setflag, rtr_redist_level12, 
	    OBJ(int,10), ISIS_CIRCUIT_L2, "level-2", 
	    "IS-IS level-2 routes only", PRIV_CONF);
KEYWORD_ID (rtr_redist_level1, rtr_redist_level_setflag, rtr_redist_level2, 
	    OBJ(int,10), ISIS_CIRCUIT_L1, "level-1", 
	    "IS-IS level-1 routes only", PRIV_CONF);
SET (rtr_redist_level_default, rtr_redist_level1, OBJ(int,10), 
     ISIS_CIRCUIT_L2);
IFELSE (rtr_redist_level_check, NONE, rtr_redist_level_default,
	(GETOBJ(int,1) & REDIST_LEVEL));
ASSERT (rtr_redist_level, rtr_redist_level_check, rtr_redist_routemap,
	(!strcmp(((pdbtype *) csb->protocol)->name, RTRNAME_ISIS) ||
	!strcmp(csb->router_name.myname, RTRNAME_ISIS)));

/* 
 * subnets 
 *
 * Accepted when redistributing any protocol into OSPF.
 */
KEYWORD_OR(rtr_redist_subnet_kw, rtr_redist_opts, NONE,
	   OBJ(int,1), REDIST_SUBNET,
	   "subnets", "Consider subnets for redistribution into OSPF", 
	   PRIV_CONF);
ASSERT (rtr_redist_subnet, rtr_redist_subnet_kw, rtr_redist_level,
	!strcmp(((pdbtype *) csb->protocol)->name, RTRNAME_OSPF));

/* 
 * tag <tag-value> 
 *
 * Accepted when redistributing any protocol into OSPF.
 */
TESTVAR	(rtr_redist_tag_noprefix, no_alt, rtr_redist_opts, NONE, NONE, NONE, 
	 sense, TRUE);
NUMBER (rtr_redist_tag_num, rtr_redist_opts, rtr_redist_tag_noprefix,
	 OBJ(int,3), 0, -1, "32-bit tag value");
KEYWORD_OR(rtr_redist_tag_kw, rtr_redist_tag_num, NONE,
	   OBJ(int,1), REDIST_TAG,
	   "tag", "Set tag for routes redistributed into OSPF", PRIV_CONF);
ASSERT	(rtr_redist_tag, rtr_redist_tag_kw, rtr_redist_subnet,
	 !strcmp(((pdbtype *) csb->protocol)->name, RTRNAME_OSPF));

/* 
 * metric-type <metric-type> 
 *
 * Accepted when redistributing any protocol into OSPF or IS-IS.
 */
TESTVAR	(rtr_redist_mtype_noprefix, no_alt, rtr_redist_opts,
	 NONE, NONE, NONE, sense, TRUE);
KEYWORD_ID (rtr_redist_mtype_isis_external, rtr_redist_opts, 
	    rtr_redist_mtype_noprefix, OBJ(int,4), 4,
	    "external", "Set IS-IS External metric type", PRIV_CONF);
KEYWORD_ID (rtr_redist_mtype_isis, rtr_redist_opts, 
	    rtr_redist_mtype_isis_external, OBJ(int,4), 3,
	    "internal", "Set IS-IS Internal metric type", PRIV_CONF);
KEYWORD_ID (rtr_redist_mtype_ext_type2, rtr_redist_opts, 
	    rtr_redist_mtype_noprefix, OBJ(int,4), 2, "2", 
	    "Set OSPF External Type 2 metrics", PRIV_CONF);
KEYWORD_ID (rtr_redist_mtype_ospf, rtr_redist_opts, 
	    rtr_redist_mtype_ext_type2, OBJ(int,4), 1,
	    "1", "Set OSPF External Type 1 metrics", PRIV_CONF);
IFELSE (rtr_redist_mtype_ifelse, rtr_redist_mtype_ospf, rtr_redist_mtype_isis,
	!strcmp(((pdbtype *) csb->protocol)->name, RTRNAME_OSPF));
KEYWORD_OR(rtr_redist_mtype_kwd, rtr_redist_mtype_ifelse, NONE,
	   OBJ(int,1), REDIST_METRICTYPE,
	   "metric-type", 
	   "OSPF/IS-IS exterior metric type for redistributed routes",
	   PRIV_CONF);
TEST_MULTIPLE_FUNCS(rtr_redist_mtype_test, rtr_redist_mtype_kwd, NONE, NONE);
ASSERT	(rtr_redist_mtype, rtr_redist_mtype_test, rtr_redist_tag,
	 !strcmp(((pdbtype *) csb->protocol)->name, RTRNAME_OSPF) ||
	 !strcmp(((pdbtype *) csb->protocol)->name, RTRNAME_ISIS));

/* 
 * weight <weight-modifer> [BGP only]
 * 
 * Accepted for all protocols but BGP.
 */
TESTVAR	(rtr_redist_weight_noprefix, no_alt, rtr_redist_opts, NONE, NONE,
	 NONE, sense, TRUE);
NUMBER(rtr_redist_weight_number, rtr_redist_opts,
	 rtr_redist_weight_noprefix, OBJ(int,12),
	 BGP_MIN_WEIGHT, BGP_MAX_WEIGHT, "Absolute weight");
KEYWORD_OR(rtr_redist_weight_kw, rtr_redist_weight_number, NONE,
	   OBJ(int,1), REDIST_WEIGHT,
	   "weight", "Set network weight when redistributing into BGP",
	   PRIV_CONF);
ASSERT	(rtr_redist_weight, rtr_redist_weight_kw, rtr_redist_mtype,
	 strcmp(csb->router_name.myname, RTRNAME_BGP) &&
	 !strcmp(((pdbtype *) csb->protocol)->name, RTRNAME_BGP));

/* 
 * metric <metric-value> 
 * 
 * Accepted for all protocols.
 */
TESTVAR	(rtr_redist_met_noprefix, no_alt, rtr_redist_opts, NONE, NONE, NONE,
	 sense, TRUE);
NUMBER	(rtr_redist_met_not_isis_metric, rtr_redist_opts, 
	 rtr_redist_met_noprefix, OBJ(int,5), 0, -1, "Default metric");

NUMBER (rtr_redist_met_isis_metric, rtr_redist_opts,
	rtr_redist_met_noprefix, OBJ(int,5), 0, ISIS_MAX_LINK_METRIC,
	"ISIS default metric");
IFELSE (rtr_redist_met_not_igrp_metric, rtr_redist_met_isis_metric,
	rtr_redist_met_not_isis_metric,
	!strcmp(((pdbtype *) csb->protocol)->name, RTRNAME_ISIS));

/* <mtu> */
NUMBER (rtr_redist_met_igrp_mtu, rtr_redist_opts, no_alt,
	OBJ(int,9), 1, -1, "IGRP MTU of the path ");

/* <loading> */
NUMBER (rtr_redist_met_igrp_loading, rtr_redist_met_igrp_mtu, no_alt,
	OBJ(int,8), 1, 255, 
	"IGRP Effective bandwidth metric (Loading) where 255 is 100% loaded");

/* <reliability> */
NUMBER (rtr_redist_met_igrp_reliability, rtr_redist_met_igrp_loading, no_alt,
	OBJ(int,7), 0, 255, 
	"IGRP reliability metric where 255 is 100% reliable");

/* <delay> */
NUMBER (rtr_redist_met_igrp_delay, rtr_redist_met_igrp_reliability, no_alt,
	OBJ(int,6), 0, -1, "IGRP delay metric, in 10 microsecond units");

/* <bandwidth> for IGRP */
NUMBER (rtr_redist_met_igrp_bandwidth, rtr_redist_met_igrp_delay, 
	rtr_redist_met_noprefix,
	OBJ(int,5), 1, -1, "IGRP bandwidth metric in kilobits per second");
IFELSE (rtr_redist_met_assert, rtr_redist_met_igrp_bandwidth, 
	rtr_redist_met_not_igrp_metric,
	!strcmp(((pdbtype * )csb->protocol)->name, RTRNAME_IGRP) ||
	!strcmp(((pdbtype * )csb->protocol)->name, RTRNAME_IGRP2));
KEYWORD_OR(rtr_redist_metric, rtr_redist_met_assert, rtr_redist_weight,
	   OBJ(int,1), REDIST_METRIC,
	   "metric", "Metric for redistributed routes", PRIV_CONF);

/* 
 * match { internal | external <extern-type> } 
 *
 * Accepted only when redistributing OSPF into any other protocol.
 */
pdecl(rtr_redist_match_opts);
NOP	(rtr_redist_match_accept, rtr_redist_match_opts, rtr_redist_opts);
EVAL	(rtr_redist_match_ext_set, rtr_redist_match_accept,
	 GETOBJ(int,2) |= RTYPE_EXTERN1|RTYPE_EXTERN2);
KEYWORD_OR(rtr_redist_match_ext_type2, rtr_redist_match_accept,
	   rtr_redist_match_ext_set,
	   OBJ(int,2), RTYPE_EXTERN2,
	   "2", "Redistribute OSPF external type 2 routes", PRIV_CONF);
KEYWORD_OR(rtr_redist_match_ext_type1, rtr_redist_match_accept,
	   rtr_redist_match_ext_type2,
	   OBJ(int,2), RTYPE_EXTERN1,
	   "1", "Redistribute external type 1 routes", PRIV_CONF);
KEYWORD (rtr_redist_match_ext, rtr_redist_match_ext_type1, no_alt,
	 "external", "Redistribute OSPF external routes", PRIV_CONF);
KEYWORD_OR(rtr_redist_match_int, rtr_redist_match_accept, rtr_redist_match_ext,
	   OBJ(int,2), RTYPE_INTRA|RTYPE_INTER,
	   "internal", "Redistribute OSPF internal routes", PRIV_CONF);
NOP	(rtr_redist_match_opts, rtr_redist_match_int, NONE);

KEYWORD_OR(rtr_redist_match_kw, rtr_redist_match_opts, NONE,
	   OBJ(int,1), REDIST_MATCH,
	   "match", "Redistribution of OSPF routes", PRIV_CONF);
ASSERT	(rtr_redist_match, rtr_redist_match_kw, rtr_redist_metric,
	 !strcmp(csb->router_name.myname, RTRNAME_OSPF));

NOP	(rtr_redist_opts, rtr_redist_match, NONE);

/*
 * Redistributing IP or CLNS static routes into IS-IS.
 */
KEYWORD_ID (rtr_redist_static_isis_ip, rtr_redist_opts, rtr_redist_routemap,
	    OBJ(int,11), 1,
	    "ip", "Redistribution of IP static routes", PRIV_CONF);
KEYWORD (rtr_redist_static_isis, rtr_redist_routemap, rtr_redist_static_isis_ip,
	 "clns", "Redistribution of OSI static routes", PRIV_CONF);
IFELSE	(rtr_redist_static, rtr_redist_static_isis, rtr_redist_opts,
	 !strcmp(csb->router_name.myname, RTRNAME_STATIC) &&
	 !strcmp(((pdbtype *) csb->protocol)->name, RTRNAME_ISIS));

/*
 * If redistributing to/from ISO-IGRP, go get route-map parameter only.
 */
IFELSE (rtr_redist_isoigrp, rtr_redist_routemap, rtr_redist_static,
	!strcmp(((pdbtype *) csb->protocol)->name, RTRNAME_ISOIGRP) ||
	!strcmp(csb->router_name.myname, RTRNAME_ISOIGRP));

/*
 * Get router name and [<AS number>, <tag>]
 */
TEST_MULTIPLE_FUNCS(rtr_redist_router_test, rtr_redist_isoigrp, NONE, NONE);
GENERAL_ROUTER(rtr_redist_router2, rtr_redist_router_test, NONE, router_name,
	       (PROC_ISIS | PROC_ISOIGRP | RMATCH_IP | RMATCH_OSI |
		RMATCH_ARGS | RMATCH_TAG_REQUIRED | RMATCH_NO_KEY_HELP));
GENERAL_ROUTER(rtr_redist_router, rtr_redist_isoigrp,
	       rtr_redist_router2, router_name,
	       (PROC_BGP | PROC_CONNECTED | PROC_EGP | PROC_IGRP | 
		PROC_IGRP2 | PROC_RIP | PROC_ISIS | PROC_ISOIGRP |
		PROC_OSPF | PROC_STATIC | RMATCH_IP | RMATCH_OSI |
		RMATCH_ARGS | RMATCH_NO_TAG | PROC_MOBILE | PROC_ODR));
NVGENNS	(rtr_redist_nvgen, rtr_redist_router, redistribute_command);
KEYWORD	(router_redistribute, rtr_redist_nvgen, ALTERNATE,
	"redistribute", 
	 "Redistribute information from another routing protocol", PRIV_CONF);

#undef	ALTERNATE
#define	ALTERNATE	router_redistribute
