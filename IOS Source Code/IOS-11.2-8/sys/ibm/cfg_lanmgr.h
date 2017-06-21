/* $Id: cfg_lanmgr.h,v 3.3 1996/02/08 17:57:14 ioakley Exp $
 * $Source: /release/111/cvs/Xsys/ibm/cfg_lanmgr.h,v $
 *------------------------------------------------------------------
 * C F G _ L A N M G R . H
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_lanmgr.h,v $
 * Revision 3.3  1996/02/08  17:57:14  ioakley
 * CSCdi47561: Resolve potential network congestion problems by adding
 *             a global configuration command to disable the automatic
 *             Report Path Trace function and also correct problems with
 *             Report Path Trace frames returned to originating stations.
 *
 * Revision 3.2  1995/11/17  09:21:59  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:44:17  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  10:53:50  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize os, hes, les, if, and ibm.
 *
 * Revision 2.1  1995/06/07  22:08:42  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 *
 */

/************************************************************************
 * lnm pathtrace disabled
 *
 */
EOLS	(conf_lanmgr_pathtrace_disable_eol, lanmgr_command,
	 LM_PATHTRACE_DISABLED);
KEYWORD_ID(conf_lanmgr_pathtrace_disable_origin,
	   conf_lanmgr_pathtrace_disable_eol,
	   conf_lanmgr_pathtrace_disable_eol, OBJ(int,1),
	   LM_PATHTRACE_DISABLE_ORIGIN, "origin",
	   "Disable Pathtrace Reporting to Originating Stations only",
	   PRIV_CONF);
KEYWORD_ID(conf_lanmgr_pathtrace_disable_all, conf_lanmgr_pathtrace_disable_eol,
	   conf_lanmgr_pathtrace_disable_origin, OBJ(int,1),
	   LM_PATHTRACE_DISABLE_ALL, "all",
	   "Disable Pathtrace Reporting to LAN Manager and Originating Stations",
	   PRIV_CONF);
NVGENS (conf_lanmgr_pathtrace_disable_nvgen, conf_lanmgr_pathtrace_disable_all,
	lanmgr_command, LM_PATHTRACE_DISABLED);
KEYWORD (conf_lanmgr_pathtrace_disable, conf_lanmgr_pathtrace_disable_nvgen,
	 no_alt,
	 "pathtrace-disabled",
	 "Disable Pathtrace Reporting to LANMGR stations",
	 PRIV_CONF);

/*************************************************************************
 * lnm disabled
 * no lnm disabled
 *
 */
EOLS	(conf_lanmgr_disable_eol, lanmgr_command, LM_DISABLED);
KEYWORD (conf_lanmgr_disable, conf_lanmgr_disable_eol,
	 conf_lanmgr_pathtrace_disable,
	 "disabled",
	 "Disable Lan Network Manager Functionality",
	 PRIV_CONF);

/*************************************************************************
 * lnm snmp-only
 * no lnm snmp-only
 *
 */
EOLS	(conf_lanmgr_snmp_eol, lanmgr_command, LM_SNMP_ONLY);
KEYWORD (conf_lanmgr_snmp, conf_lanmgr_snmp_eol, conf_lanmgr_disable,
	 "snmp-only",
	 "Prevent any LNM stations from modifying parameters in the router",
	 PRIV_CONF);

/*************************************************************************/
KEYWORD	(conf_lanmgr, conf_lanmgr_snmp, NONE,
	 "lnm", "IBM Lan Manager", PRIV_CONF);

TEST_BOOLEAN(conf_lanmgr_test, conf_lanmgr, NONE, ALTERNATE, bridge_enable);

#undef  ALTERNATE
#define ALTERNATE conf_lanmgr_test
