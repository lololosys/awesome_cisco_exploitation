/* $Id: cfg_novigrp.h,v 3.1.2.1 1996/05/03 02:16:11 sluong Exp $
 * $Source: /release/112/cvs/Xsys/xns/cfg_novigrp.h,v $
 *------------------------------------------------------------------
 * IPX EIGRP global commands.
 *
 * April, 1996, Steven Luong
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cfg_novigrp.h,v $
 * Revision 3.1.2.1  1996/05/03  02:16:11  sluong
 * Branch: California_branch
 * IPX ACL Violation Logging, Plaining English Filters, Display SAP by
 * name,
 * NLSP MIBS, Scaleable RIP/SAP, Modularity, and SAP query by name.
 *
 * Revision 3.1  1996/04/13  01:24:45  sluong
 * Add placeholders
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*****************************************************************
 * [no] ipx eigrp-sap-split-horizon
 */
EOLS (conf_nov_eigrp_sap_split_horizon_eol, novigrp_command,
      NOVELL_EIGRP_SAP_SPLIT_HORIZON);
KEYWORD (conf_nov_eigrp_sap_split_horizon,
	 conf_nov_eigrp_sap_split_horizon_eol, ALTERNATE,
	 "eigrp-sap-split-horizon", "EIGRP SAP obeys split horizon",
	 PRIV_CONF);

#undef  ALTERNATE
#define ALTERNATE conf_nov_eigrp_sap_split_horizon
