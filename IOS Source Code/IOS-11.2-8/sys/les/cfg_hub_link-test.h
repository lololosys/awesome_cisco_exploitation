/* $Id: cfg_hub_link-test.h,v 3.2 1995/11/17 17:36:46 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/les/cfg_hub_link-test.h,v $
 *------------------------------------------------------------------
 * HUB config link-test command
 *
 * September 1994, Viet Nguyen
 *
 * Copyright (c) 1994-1995 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Gurus: Viet Nguyen
 *------------------------------------------------------------------
 * $Log: cfg_hub_link-test.h,v $
 * Revision 3.2  1995/11/17  17:36:46  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:29:39  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  10:52:29  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize os, hes, les, if, and ibm.
 *
 * Revision 2.1  1995/06/07  22:03:15  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/****************************************************************************
 * [no] link-test
 */

EOLS	  (cfg_hub_link_test_eol, hub_config_command, HUB_LINK_TEST);
NVGENS  (cfg_hub_link_test_nvgens, cfg_hub_link_test_eol,
	   hub_config_command, HUB_LINK_TEST);
KEYWORD (cfg_hub_link_test, cfg_hub_link_test_nvgens, NONE,
	   "link-test", "Enable Link Test Function of Hub port", PRIV_CONF);
ASSERT  (cfg_hub_link_test_type, cfg_hub_link_test, ALTERNATE,
	   (csb->hub->hub_type & HUB_ETHER));

#undef	ALTERNATE
#define	ALTERNATE	cfg_hub_link_test_type

