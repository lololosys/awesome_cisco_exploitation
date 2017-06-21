/* $Id: cfg_hub_source-address.h,v 3.2 1995/11/17 17:36:49 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/les/cfg_hub_source-address.h,v $
 *------------------------------------------------------------------
 * C F G _ H U B _ S O U R C E - A D D R E S S . H 
 *
 * September 1994, Viet Nguyen
 *
 * Copyright (c) 1994-1995 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Gurus: Viet Nguyen
 *------------------------------------------------------------------
 * $Log: cfg_hub_source-address.h,v $
 * Revision 3.2  1995/11/17  17:36:49  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:29:42  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  10:52:30  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize os, hes, les, if, and ibm.
 *
 * Revision 2.1  1995/06/07  22:03:18  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/****************************************************************************
 * [no] source-address [<mac-addr>]
 *
 * OBJ(int,1) = TRUE iff a mac-addr was entered
 * OBJ(hwaddr,1) = <mac-addr>
 */

EOLS	(cfg_hub_source_address_eol, hub_config_command, HUB_SOURCE_ADDRESS);
SET     (cfg_hub_source_address_set, cfg_hub_source_address_eol, 
           OBJ(int,1), TRUE);
ENADDR  (cfg_hub_source_address_addr, cfg_hub_source_address_set, 
           cfg_hub_source_address_eol, OBJ(hwaddr,1), "MAC address");
NVGENS  (cfg_hub_source_address_nvgens, cfg_hub_source_address_addr,
	   hub_config_command, HUB_SOURCE_ADDRESS);
KEYWORD (cfg_hub_source_address, cfg_hub_source_address_nvgens, NONE, 
           "source-address", "Enable Source Address control for Hub port",
           PRIV_CONF);
ASSERT  (cfg_hub_source_address_test, cfg_hub_source_address, ALTERNATE,
	   (csb->hub->hub_type & HUB_ETHER));

#undef	ALTERNATE
#define	ALTERNATE	cfg_hub_source_address_test

