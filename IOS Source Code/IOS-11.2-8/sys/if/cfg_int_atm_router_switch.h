/* $Id: cfg_int_atm_router_switch.h,v 3.1.4.1 1996/04/25 23:13:06 ronnie Exp $
 * $Source: /release/112/cvs/Xsys/if/cfg_int_atm_router_switch.h,v $
 *------------------------------------------------------------------
 * cfg_int_atm_router_switch.h: ATM parser commands common to routers
 * and atm switch
 *
 * March, 1996, Rob Zagst
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cfg_int_atm_router_switch.h,v $
 * Revision 3.1.4.1  1996/04/25  23:13:06  ronnie
 * CSCdi55766:  Hey!  This ATM and Frame Relay is no different than in 11.1
 * Feature commit for ATM and Frame Relay into 11.2
 * Branch: California_branch
 *
 * Revision 3.1  1996/03/30  04:41:14  rzagst
 * Placeholder for cfg_int_atm_router_switch.h
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * ATM nsap-addr <20 byte nsap address>
 *
 * OBJ(hwaddr,1) = ATM nsap address
 */
EOLS (ci_atm_nsap_eol, atm_interface_commands, ATM_CMD_NSAP);
ATMNSAPADDR (ci_atm_nsap_addr, ci_atm_nsap_eol, no_alt, OBJ(hwaddr,1),
              "20-octet NSAP address");
NVGENS (ci_atm_nsap_nvgen, ci_atm_nsap_addr, atm_interface_commands,
        ATM_CMD_NSAP);
KEYWORD (ci_atm_nsap, ci_atm_nsap_nvgen, ALTERNATE,
        "nsap-address", "20-octet ATM NSAP address", PRIV_CONF|PRIV_SUBIF); 


#undef	ALTERNATE
#define	ALTERNATE	ci_atm_nsap
