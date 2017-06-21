/* $Id: cfg_smrp.h,v 3.2 1995/11/17 18:52:49 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/smrp/cfg_smrp.h,v $
 *------------------------------------------------------------------
 * Top-level configuration commands for SMRP.
 *
 * April 1995, Steven Lin
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cfg_smrp.h,v $
 * Revision 3.2  1995/11/17  18:52:49  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:12:43  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:44:55  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
EOLS (conf_smrp_routing_eol, smrp_command, SMRP_ROUTING);
NVGENS (conf_smrp_routing_nvgen, conf_smrp_routing_eol,
	smrp_command, SMRP_ROUTING);
KEYWORD (conf_smrp_routing, conf_smrp_routing_nvgen, no_alt,
	 "routing", "Enable SMRP routing", PRIV_CONF);

KEYWORD (configure_smrp, conf_smrp_routing, ALTERNATE,
	 "smrp", "Simple Multicast Routing Protocol configuration commands",
	 PRIV_CONF);

#undef  ALTERNATE
#define ALTERNATE configure_smrp
