/* $Id: cfg_int_physical_layer.h,v 3.1.24.1 1996/06/17 08:35:17 jhernand Exp $
 * $Source: /release/112/cvs/Xsys/parser/cfg_int_physical_layer.h,v $
 *-----------------------------------------------------------------------
 * cfg_int_physical_layer.h - Interface sub-command to set the physical
 *                            layer on sync/async serial interfaces
 *
 * February 1996,  Suresh Sangiah
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *-----------------------------------------------------------------------
 * $Log: cfg_int_physical_layer.h,v $
 * Revision 3.1.24.1  1996/06/17  08:35:17  jhernand
 * Commit Modular Cobra to California_branch
 *
 * Revision 3.1  1996/02/13  07:14:47  ssangiah
 * Placeholder files for porting new platform code from 11.0.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * The [no] physical-layer {sync | async} command
 */

EOLS	(config_physical_layer_sync_eol, 
         reg_invoke_media_physical_layer_command, 
         PHY_LAYER_SYNC);
KEYWORD	(config_physical_layer_sync, config_physical_layer_sync_eol, no_alt,
	 "sync", 
         "Configure synchronous physical layer on serial interface",
	 PRIV_CONF);

EOLS	(config_physical_layer_async_eol, 
         reg_invoke_media_physical_layer_command, 
         PHY_LAYER_ASYNC);
KEYWORD	(config_physical_layer_async, config_physical_layer_async_eol, 
         config_physical_layer_sync,
	 "async", 
         "Configure asynchronous physical layer on serial interface",
	 PRIV_CONF);

NOPREFIX (config_physical_layer_noprefix, config_physical_layer_async, 
          config_physical_layer_sync_eol);

NVGENS	(config_physical_layer_nvgen, config_physical_layer_noprefix, 
	 reg_invoke_media_physical_layer_command, 0);

KEYWORD	(config_physical_layer, config_physical_layer_nvgen, no_alt,
	 "physical-layer", 
         "Configure sync or async physical layer on serial interface",
	 PRIV_CONF);

ASSERT	(config_physical_layer_test, config_physical_layer, ALTERNATE,
	 ((csb->interface->hwptr->status & IDB_SERIAL) &&
	  (csb->interface->hwptr->serial_flags & SERIAL_FLAGS_SYNCASYNC)));

#undef	ALTERNATE
#define	ALTERNATE	config_physical_layer_test
