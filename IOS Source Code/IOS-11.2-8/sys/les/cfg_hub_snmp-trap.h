/* $Id: cfg_hub_snmp-trap.h,v 3.2 1995/11/17 17:36:48 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/les/cfg_hub_snmp-trap.h,v $
 *------------------------------------------------------------------
 * cfg_hub_snmp-trap.h  HUB config/UI command for MAC Security feature
 *
 * March 1995, Wen-Zing Seifert
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cfg_hub_snmp-trap.h,v $
 * Revision 3.2  1995/11/17  17:36:48  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:29:41  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:13:42  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/10/05  21:51:35  wseifert
 * Adding place holder file for MAC Security feature.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
/****************************************************************************
 * [no] snmp trap illegal-address
 *
 */

EOLS	(cfg_hub_snmp_trap_eol, hub_config_command, HUB_SNMP_TRAP);

KEYWORD_ID (cfg_hub_trap_name, cfg_hub_snmp_trap_eol,no_alt,
	    OBJ(int,1),  HUB_SNMP_TRAP,
	    "illegal-address", "Send illegal MAC address traps to hosts", 
	    PRIV_CONF);

NVGENS  (cfg_hub_snmp_trap_nvgens, cfg_hub_trap_name,
	   hub_config_command, HUB_CONFIG);

KEYWORD	(cfg_hub_snmp_trap, cfg_hub_snmp_trap_nvgens, no_alt,
	 "trap", "Allow a specific SNMP trap", PRIV_CONF);

KEYWORD	(conf_hub_snmp, cfg_hub_snmp_trap, ALTERNATE,
	 "snmp", "Modify SNMP parameters", PRIV_CONF);

#undef	ALTERNATE
#define	ALTERNATE	conf_hub_snmp



