/* $Id: cfg_hub_auto-polarity.h,v 3.2 1995/11/17 17:36:44 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/les/cfg_hub_auto-polarity.h,v $
 *------------------------------------------------------------------
 * C F G _ H U B _ A U T O - P O L A R I T Y . h
 *
 * September 1994, Viet Nguyen
 *
 * Copyright (c) 1994-1995 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Gurus: Viet Nguyen
 *------------------------------------------------------------------
 * $Log: cfg_hub_auto-polarity.h,v $
 * Revision 3.2  1995/11/17  17:36:44  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:29:37  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  10:52:28  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize os, hes, les, if, and ibm.
 *
 * Revision 2.1  1995/06/07  22:03:11  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/****************************************************************************
 * [no] automatic-polarity
 */

EOLS(cfg_hub_auto_polarity_eol,hub_config_command,HUB_AUTOMATIC_POLARITY);
NVGENS(cfg_hub_auto_polarity_nvgens, cfg_hub_auto_polarity_eol,
	 hub_config_command, HUB_AUTOMATIC_POLARITY);
KEYWORD(cfg_hub_auto_polarity, cfg_hub_auto_polarity_nvgens, NONE, 
        "auto-polarity", "Enable automatic receiver polarity reversal", 
         PRIV_CONF);
ASSERT  (cfg_hub_auto_polarity_test, cfg_hub_auto_polarity, ALTERNATE,
	   (csb->hub->hub_type & HUB_ETHER));

#undef	ALTERNATE
#define	ALTERNATE	cfg_hub_auto_polarity_test

