/* $Id: cfg_int_ipxwan.h,v 3.1.2.2 1996/08/16 00:14:04 hampton Exp $
 * $Source: /release/112/cvs/Xsys/xns/cfg_int_ipxwan.h,v $
 *------------------------------------------------------------------
 * IPX WAN interface subcommands.
 *
 * April, 1996, Steven Luong
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cfg_int_ipxwan.h,v $
 * Revision 3.1.2.2  1996/08/16  00:14:04  hampton
 * Change parser comments to reflect ipx command usage.  [CSCdi66208]
 * Branch: California_branch
 *
 * Revision 3.1.2.1  1996/05/03  02:15:44  sluong
 * Branch: California_branch
 * IPX ACL Violation Logging, Plaining English Filters, Display SAP by
 * name,
 * NLSP MIBS, Scaleable RIP/SAP, Modularity, and SAP query by name.
 *
 * Revision 3.1  1996/04/13  01:24:50  sluong
 * Add placeholders
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*****************************************************************
 * ipx ipxwan [node-id external-net server-name [retry-int [retry-limit]]]
 * no ipx ipxwan [node-id external-net server-name]
 *
 * ipx ipxwan error [down | reset | nothing]
 * no ipx ipxwan error [down | reset | nothing]
 *
 *  what to do on an ipxwan fail to negotiate error.
 *
 * ipx ipxwan client
 *
 *  enable negotiation of end client connection, ipxwan must already be
 *  configured on this interface.
 *
 * ipx ipxwan static
 * 
 *  enable negotiation of static negotiation, for this to work right
 *  RIP and NLSP muct already be off and static routes and SAP defined.
 *  ipxwan must already be configured on this interface.
 *
 */
EOLS    (ci_nov_ipxwan_enable_eol, nov_ipxwan_commands, NOVELL_IPXWAN_ENABLE);

NUMBER  (ci_nov_ipxwan_enable_retry_limit, ci_nov_ipxwan_enable_eol,
	 ci_nov_ipxwan_enable_eol, OBJ(int,5), 1, 100, "Retry attempt limit");

NUMBER  (ci_nov_ipxwan_enable_retry_int, ci_nov_ipxwan_enable_retry_limit,
	 ci_nov_ipxwan_enable_eol, OBJ(int,4), 1, 600,
	 "Retry interval, in seconds");

/* This will ensure decent known values for the above two values */
SET	(ci_nov_ipxwan_enable_set2, ci_nov_ipxwan_enable_retry_int,
	 OBJ(int,5), 0);
SET	(ci_nov_ipxwan_enable_set1, ci_nov_ipxwan_enable_set2, OBJ(int,4), 0);

STRING  (ci_nov_ipxwan_enable_sname, ci_nov_ipxwan_enable_set1, no_alt,
	 OBJ(string,1), "Local Server Name");

HEXDIGIT (ci_nov_ipxwan_enable_external_net, ci_nov_ipxwan_enable_sname,
	 no_alt, OBJ(int, 3), 0, NOV_MAX_NET, "External net number");

KEYWORD_ID (ci_nov_ipxwan_enable_extnetorun, ci_nov_ipxwan_enable_sname,
	    ci_nov_ipxwan_enable_external_net, OBJ(int,2), 0, "unnumbered",
	    "unnumbered interface", PRIV_CONF|PRIV_SUBIF);

HEXDIGIT (ci_nov_ipxwan_enable_nodeid,
	  ci_nov_ipxwan_enable_extnetorun, ci_nov_ipxwan_enable_eol,
	  OBJ(int, 2), 0, NOV_MAX_NET, "Local Node ID, 0 Uses Global ID");

KEYWORD_ID (ci_nov_ipxwan_enable_errresume, ci_nov_ipxwan_enable_eol,
            no_alt, OBJ(int,2), IPX_IPXWAN_ERROR_CONFIG_NOOP,
	    "resume", "on error, resume/retry (ignore error)",
	    PRIV_CONF|PRIV_SUBIF);

KEYWORD_ID (ci_nov_ipxwan_enable_errreset, ci_nov_ipxwan_enable_eol,
            ci_nov_ipxwan_enable_errresume, OBJ(int,2),
	    IPX_IPXWAN_ERROR_CONFIG_RESET,
	    "reset", "on error, reset interface", PRIV_CONF|PRIV_SUBIF);

KEYWORD_ID (ci_nov_ipxwan_enable_errdown, ci_nov_ipxwan_enable_eol,
            ci_nov_ipxwan_enable_errreset, OBJ(int,2),
	    IPX_IPXWAN_ERROR_CONFIG_DOWN,
	    "shutdown", "on error, shutdown interface", PRIV_CONF|PRIV_SUBIF);

KEYWORD_ID (ci_nov_ipxwan_enable_error, ci_nov_ipxwan_enable_errdown,
            ci_nov_ipxwan_enable_nodeid, OBJ(int,1), IPX_IPXWAN_ERROR,
	    "error", "set IPXWAN error action", PRIV_CONF|PRIV_SUBIF);

KEYWORD_ID (ci_nov_ipxwan_enable_client, ci_nov_ipxwan_enable_eol,
            ci_nov_ipxwan_enable_error, OBJ(int,1), IPX_IPXWAN_CLIENT,
	    "client", "IPXWAN Client mode", PRIV_CONF|PRIV_SUBIF|PRIV_HIDDEN);

KEYWORD_ID (ci_nov_ipxwan_enable_static, ci_nov_ipxwan_enable_eol,
            ci_nov_ipxwan_enable_client, OBJ(int,1), IPX_IPXWAN_STATIC,
	    "static", "IPXWAN Static mode", PRIV_CONF|PRIV_SUBIF);

/* OBJ(int,5) holds which mode of IPXWAN command we are doing */
SET	(ci_nov_ipxwan_enable_set3, ci_nov_ipxwan_enable_static, 
	 OBJ(int,1), IPX_IPXWAN_DEFAULT);

NVGENS  (ci_nov_ipxwan_enable_nvgen, ci_nov_ipxwan_enable_set3,
	 nov_ipxwan_commands, NOVELL_IPXWAN_ENABLE);

KEYWORD (ci_nov_ipxwan, ci_nov_ipxwan_enable_nvgen, no_alt,
	 "ipxwan", "Configure IPXWAN on this interface", PRIV_CONF|PRIV_SUBIF);

#undef  ALTERNATE
#define ALTERNATE ci_nov_ipxwan
