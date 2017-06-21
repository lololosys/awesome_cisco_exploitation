/* $Id: cfg_hub_shutdown.h,v 3.2 1995/11/17 17:36:47 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/les/cfg_hub_shutdown.h,v $
 *------------------------------------------------------------------
 * HUB config shutdown command
 *
 * March 1994, Robert Widmer
 *
 * Copyright (c) 1994-1995 by cisco Systems, Inc.
 * All rights reserved.
 * 
 * Gurus: Steve Garcia, Viet Nguyen
 *------------------------------------------------------------------
 * $Log: cfg_hub_shutdown.h,v $
 * Revision 3.2  1995/11/17  17:36:47  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:29:40  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  10:52:30  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize os, hes, les, if, and ibm.
 *
 * Revision 2.1  1995/06/07  22:03:17  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/****************************************************************************
 * [no] shutdown
 */

EOLS	(cfg_hub_shutdown_eol, hub_config_command, HUB_SHUTDOWN);
NVGENS	(cfg_hub_shutdown_nvgens, cfg_hub_shutdown_eol,
	 hub_config_command, HUB_SHUTDOWN);
KEYWORD	(cfg_hub_shutdown, cfg_hub_shutdown_nvgens, ALTERNATE,
	 "shutdown", "Shutdown the selected port", PRIV_CONF);

#undef	ALTERNATE
#define	ALTERNATE	cfg_hub_shutdown
