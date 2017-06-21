/* $Id: cfg_cfgtbl_lane.h,v 3.2 1995/11/17 17:49:45 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/lane/cfg_cfgtbl_lane.h,v $
 *------------------------------------------------------------------
 * lane lecs config table command.
 *
 * November 1994, Cihangir Akyol (Gee)
 *
 * Copyright (c) 1993-1995 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: cfg_cfgtbl_lane.h,v $
 * Revision 3.2  1995/11/17  17:49:45  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:22:24  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:18:22  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 *
 * create/select/[delete] an lecs config table for manipulation
 *
 * [no] lane database TABLENAME
 *
 * OBJ (string, 1) = TABLENAME
 */

EOLS (cfg_cfgtbl_eol, lecs_cfgtbl_function, LANE_LECS_CMD_CFGTBL);

NOPREFIX (cfg_cfgtbl_nopfx, cfg_cfgtbl_eol, cfg_cfgtbl_eol);

STRING (cfg_cfgtbl_name, cfg_cfgtbl_nopfx, no_alt, 
	 OBJ (string, 1), "Configuration table name");

NVGENS (cfg_cfgtbl_nvgens, cfg_cfgtbl_name, lecs_cfgtbl_function, 
	LANE_LECS_CMD_CFGTBL);

KEYWORD (cfg_cfgtbl1, cfg_cfgtbl_nvgens, no_alt,
         "database", "Configure the lecs config table",
	 PRIV_CONF);

KEYWORD (cfg_cfgtbl, cfg_cfgtbl1, ALTERNATE,
	 "lane", "Configure LAN Emulation",
	 PRIV_CONF);

#undef  ALTERNATE
#define ALTERNATE       cfg_cfgtbl

