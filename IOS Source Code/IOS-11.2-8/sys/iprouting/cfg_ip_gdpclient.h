/* $Id: cfg_ip_gdpclient.h,v 3.2 1995/11/17 17:31:36 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/iprouting/cfg_ip_gdpclient.h,v $
 *------------------------------------------------------------------
 * GDP Client config commands
 *
 * May 1993, Robert Widmer
 *
 * Copyright (c) 1993-1995 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: cfg_ip_gdpclient.h,v $
 * Revision 3.2  1995/11/17  17:31:36  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:04:53  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/01  12:15:39  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize iprouting
 *
 * Revision 2.2  1995/06/21  18:17:01  widmer
 * CSCdi36227:  Fix parse chain alternates
 *
 * Revision 2.1  1995/06/07  22:08:22  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************************
 * ip gdp [{ rip | igrp |  irdp }]
 * no ip gdp [{ rip | igrp | irdp }]
 *
 * csb->which = GDP_CMD_RIP	= gdp for rip
 *		GDP_CMD_IGRP	= gdp for igrp
 *		GDP_CMD_IRDP	= gdp for irdp
 *		GDP_CMD_GDP	= gdp for gdp (Oh God, why did we name it this?)
 */

/******************************************************************************
 * [no] ip gdp irdp [multicast]
 *
 */
EOLS	(cip_gdp_irdp_eol, ip_gdpclient_command, GDP_CMD_IRDP);
EOLS	(cip_gdp_irdp_multi_eol, ip_gdpclient_command, GDP_CMD_IRDP_MULTICAST);
KEYWORD (cip_gdp_irdp_multi, cip_gdp_irdp_multi_eol, cip_gdp_irdp_eol,
	"multicast", "IRDP solicitations are multicast", PRIV_CONF);
KEYWORD (cip_gdp_irdp, cip_gdp_irdp_multi, no_alt,
	 "irdp", "Discover routers transmitting IRDP router updates",
	 PRIV_CONF);

/******************************************************************************
 * [no] ip gdp gdp
 *
 */
EOLS	(cip_gdp_gdp_eol, ip_gdpclient_command, GDP_CMD_GDP);
KEYWORD (cip_gdp_gdp, cip_gdp_gdp_eol, cip_gdp_irdp,
	 "gdp", "Discover routers transmitting GDP router updates",
	 PRIV_CONF);

/******************************************************************************
 * [no] ip gdp rip
 *
 */
EOLS	(cip_gdp_rip_eol, ip_gdpclient_command, GDP_CMD_RIP);
KEYWORD (cip_gdp_rip, cip_gdp_rip_eol, cip_gdp_gdp,
	 "rip", "Discover routers transmitting RIP router updates", PRIV_CONF);

/******************************************************************************
 * [no] ip gdp igrp
 *
 */
EOLS	(cip_gdp_igrp_eol, ip_gdpclient_command, GDP_CMD_IGRP);
KEYWORD (cip_gdp_igrp, cip_gdp_igrp_eol, cip_gdp_rip,
	 "igrp", "Discover routers transmitting IGRP router updates",
	 PRIV_CONF);

KEYWORD (cip_gdp, cip_gdp_igrp, NONE,
	 "gdp", "Router discovery mechanism",
	 PRIV_CONF);

TEST_BOOLEAN(cip_gdp_client, NONE, cip_gdp, ALTERNATE, ip_router_running);

#undef	ALTERNATE
#define	ALTERNATE	cip_gdp_client
