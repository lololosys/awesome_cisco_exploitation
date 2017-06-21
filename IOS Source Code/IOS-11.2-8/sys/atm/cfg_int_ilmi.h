/* $Id: cfg_int_ilmi.h,v 3.1.62.1 1996/04/25 23:02:36 ronnie Exp $
 * $Source: /release/112/cvs/Xsys/atm/cfg_int_ilmi.h,v $
 *------------------------------------------------------------------
 * Interface Configutration for ILMI.
 *
 * October 1995, Kartik Chandrasekhar
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cfg_int_ilmi.h,v $
 * Revision 3.1.62.1  1996/04/25  23:02:36  ronnie
 * CSCdi55766:  Hey!  This ATM and Frame Relay is no different than in 11.1
 * Feature commit for ATM and Frame Relay into 11.2
 * Branch: California_branch
 *
 * Revision 3.1.66.1  1996/03/30  03:34:45  rzagst
 * ATM Modularity: Clean up and modularize by platform the ATM parser
 * commands.
 * Branch: ATM_Cal_branch
 *
 * Revision 3.1  1995/11/09  11:00:24  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/10/17  19:09:36  kartik
 * placeholder files for the Rhino Branch.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
/******************************************************************
 * ATM esi-addr <7 byte end station id portion of nsap address>
 *
 * OBJ(hwaddr,1) = ATM esi portion of nsap address
 */
EOLS (ci_atm_esi_eol, ilmi_intf_config_commands, ILMI_IF_CONFIG_ESI);

ATMESIADDR (ci_atm_esi_addr, ci_atm_esi_eol, no_alt, OBJ(hwaddr,1),
              "7-octet End Station ID of NSAP address");

NVGENS (ci_atm_esi_nvgen, ci_atm_esi_addr, ilmi_intf_config_commands,
        ILMI_IF_CONFIG_ESI);
KEYWORD (ci_atm_esi, ci_atm_esi_nvgen, ALTERNATE,
        "esi-address", "7-octet ATM ESI address", PRIV_CONF|PRIV_SUBIF); 


/******************************************************************
 * Hidden command to disable ILMI enterprise traps in case some broken
 * implementation cannot handle it.
 */
EOLS    (ci_atm_disable_ilmitraps_eol, ilmi_intf_config_commands,
	 ILMI_IF_DISABLE_ENTERPRISE_TRAPS);
KEYWORD (ci_atm_disable_ilmitraps, ci_atm_disable_ilmitraps_eol, ci_atm_esi,
	 "disable-ilmi-enterprise-traps", "Disables ILMI enterprise traps",
	 PRIV_CONF | PRIV_HIDDEN);


/******************************************************************
 * ATM [no] address-registration
 *
 */

EOLS (config_ilmiaddreg_eol,ilmi_intf_config_commands,ILMI_IF_CONFIG_ADDRESS_REG);
KEYWORD (config_ilmiaddreg, config_ilmiaddreg_eol, ci_atm_disable_ilmitraps,
	 "address-registration", "Address Registration",PRIV_CONF);

/******************************************************************
 * ATM ilmi-keepalive interval <seconds>
 *
 * OBJ(int,1) = seconds in interval
 */
PARAMS_KEYONLY(config_ilmipoll_interval,config_ilmiaddreg,
		   "ilmi-keepalive", OBJ(int,1), 0, ILMI_MAX_TIMER,
    	   ilmi_intf_config_commands, ILMI_IF_CONFIG_KEEPALIVE,
		   "Enable ILMI keepalive", "Interval time in seconds", PRIV_CONF);

#undef	ALTERNATE
#define	ALTERNATE	config_ilmipoll_interval


