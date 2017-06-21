/* $Id: cfg_hub.h,v 3.3 1996/02/21 17:27:43 widmer Exp $
 * $Source: /release/111/cvs/Xsys/les/cfg_hub.h,v $
 *------------------------------------------------------------------
 * HUB config commands
 *
 * March 1994, Robert Widmer
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Gurus: Steve Garcia, Viet Nguyen
 *------------------------------------------------------------------
 * $Log: cfg_hub.h,v $
 * Revision 3.3  1996/02/21  17:27:43  widmer
 * CSCdi49001:  privilege level configuration doesnt take hub ether
 * Make hub macro priv 0
 *
 * Revision 3.2  1995/11/17  17:36:43  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:29:36  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  10:52:28  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize os, hes, les, if, and ibm.
 *
 * Revision 2.1  1995/06/07  22:03:10  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/****************************************************************************
 * hub {ether} <hub-num> <first-port-num> [<second-port-num>]
 *
 * OBJ(int,1) = HUB_ETHER == Ether hub
 * OBJ(int,2) = <hub-num>
 * OBJ(int,3) = <first-port-num>
 * OBJ(int,4) = <second-port-num>
 *
 */

EOLS     (cfg_hub_eol, hub_config_command, HUB_CONFIG);
HUB_RANGE (cfg_hub_port, cfg_hub_eol, no_alt, 
            OBJ(int,3), OBJ(int,4), "Port number");
HUB      (cfg_hub_hub, cfg_hub_port, no_alt,
	       OBJ(int,1), OBJ(int,2), "Hub to configure");
NVGENS   (cfg_hub_nvgen, cfg_hub_hub, hub_config_command, HUB_CONFIG);
KEYWORD  (cfg_hub, cfg_hub_nvgen, ALTERNATE,
	       "hub", "Configure hub ports", PRIV_CONF);

#undef	ALTERNATE
#define	ALTERNATE	cfg_hub
