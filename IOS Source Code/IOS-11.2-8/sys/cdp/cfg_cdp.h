/* $Id: cfg_cdp.h,v 3.2 1995/11/17 08:49:54 hampton Exp $:
 * $Source: /swtal/cherf.111/ios/sys/cdp/cfg_cdp.h,v $
 *------------------------------------------------------------------
 * Global CDP configuration commands
 *
 * October 1994, Arun Sastry
 *
 * Copyright (c) 1994-1995 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_cdp.h,v $
 * Revision 3.2  1995/11/17  08:49:54  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:05:20  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  01:47:20  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize xns, atalk, vines, decnet, xremote, x25, unix, wan, tn3270,
 * clns, cdp
 *
 * Revision 2.1  1995/06/07  22:02:06  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

LINK_TRANS(conf_cdp_extend_here, no_alt);

/******************************************************************************
 * cdp running
 * no cdp running
 *
 */
EOLS	(conf_cdp_running_eol, cdp_command, CDP_RUNNING);
NVGENS	(conf_cdp_nvgen, conf_cdp_running_eol, cdp_command, CDP_RUNNING);
KEYWORD	(conf_cdp_running, conf_cdp_nvgen, no_alt,
	 "run", "", PRIV_CONF);

/******************************************************************************
 * cdp holdtime <time>
 * no cdp configuration-time
 *
 */
PARAMS_KEYONLY( conf_cdp_holdtime, conf_cdp_running, "holdtime",
		OBJ(int,1), CDP_MIN_TTL, CDP_MAX_TTL,
		cdp_command, CDP_CONFIG_HOLDTIME,
	        "Specify the holdtime (in sec) to be sent in packets",
	        "Length of time (in sec) that receiver must keep this packet ",
	        PRIV_CONF);

/******************************************************************************
 * cdp configuration-time <interval>
 * no cdp configuration-time
 *
 */
PARAMS_KEYONLY( conf_cdp_time, conf_cdp_holdtime, "timer",
		OBJ(int,1), CDP_MIN_XMIT_INTERVAL, CDP_MAX_XMIT_INTERVAL,
		cdp_command, CDP_CONFIG_TIMER,
	        "Specify the rate at which CDP packets are sent (in sec)",
	        "Rate at which CDP packets are sent (in sec)", PRIV_CONF);

KEYWORD	(configure_cdp, conf_cdp_time, ALTERNATE, "cdp",
	 "Global CDP configuration subcommands", PRIV_CONF);

#undef  ALTERNATE
#define ALTERNATE configure_cdp


