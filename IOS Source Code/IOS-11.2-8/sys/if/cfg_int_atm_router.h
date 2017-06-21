/* $Id: cfg_int_atm_router.h,v 3.1.4.1 1996/04/25 23:13:05 ronnie Exp $
 * $Source: /release/112/cvs/Xsys/if/cfg_int_atm_router.h,v $
 *------------------------------------------------------------------
 * cfg_int_atm_router.h: ATM parser commands generic to Cisco routers
 *
 * March 1996, Rob Zagst
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cfg_int_atm_router.h,v $
 * Revision 3.1.4.1  1996/04/25  23:13:05  ronnie
 * CSCdi55766:  Hey!  This ATM and Frame Relay is no different than in 11.1
 * Feature commit for ATM and Frame Relay into 11.2
 * Branch: California_branch
 *
 * Revision 3.1  1996/03/30  04:39:08  rzagst
 * Placeholder for cfg_int_atm_router.h
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/***************************************************************************
 * ATM mtu-reject-call
 *
 *  Reject calls if selected otherwise merely write warning message
 *
 */
EOLS    (ci_atm_mtu_reject_eol, atm_interface_commands, ATM_CMD_MTUREJECT);
KEYWORD (ci_atm_bad_mtu_reject_call, ci_atm_mtu_reject_eol,
	 ALTERNATE, "mtu-reject-call",
	 "Reject Call on MTU mismatch", PRIV_CONF|PRIV_HIDDEN);

/******************************************************************
 * ATM clock <INTERNAL>
 * OBJ(int,1) = (1 == INTERNAL)
 */
EOLS	(ci_atm_clock_eol, atm_interface_commands, ATM_CMD_CLOCKING);

KEYWORD_ID(ci_atm_clock_internal, ci_atm_clock_eol, no_alt, 
	OBJ(int,1), ATM_CLOCK_INTERNAL, "INTERNAL", 
	"Internally generated TX clock", PRIV_CONF);

KEYWORD	(ci_atm_clock, ci_atm_clock_internal, ci_atm_bad_mtu_reject_call,
	"clock", "ATM TX clock source", PRIV_CONF);

/******************************************************************
 * ATM multicast <E.164 64-bit SMDS address>
 *
 * OBJ(hwaddr,1) = ATM smds MULTICAST address
 */
EOLS (ci_atm_multic_eol, atm_interface_commands, ATM_CMD_MULTICAST);

SMDSADDR(ci_atm_multic_addr, ci_atm_multic_eol, no_alt,
	 OBJ(hwaddr,1), "An SMDS address");
NOPREFIX (ci_atm_multic_noprefix, ci_atm_multic_addr, ci_atm_multic_eol);
NVGENS	(ci_atm_multic_nvgen, ci_atm_multic_noprefix,
	atm_interface_commands, ATM_CMD_MULTICAST);
KEYWORD (ci_atm_multic, ci_atm_multic_nvgen, ci_atm_clock,
        "multicast", "E.164 ATM SMDS address", PRIV_CONF|PRIV_SUBIF); 

/******************************************************************
 * ATM smds-addr <E.164 64-bit SMDS address>
 *
 * OBJ(hwaddr,1) = ATM smds address
 */
EOLS (ci_atm_smds_eol, atm_interface_commands, ATM_CMD_SMDS);

SMDSADDR(ci_atm_addr_addr, ci_atm_smds_eol, no_alt,
	 OBJ(hwaddr,1), "An SMDS address");
NOPREFIX (ci_atm_smds_addr_noprefix, ci_atm_addr_addr, ci_atm_smds_eol);
NVGENS	(ci_atm_smds_nvgen, ci_atm_smds_addr_noprefix,
	atm_interface_commands, ATM_CMD_SMDS);
KEYWORD (ci_atm_smds, ci_atm_smds_nvgen, ci_atm_multic,
        "smds-address", "E.164 ATM SMDS address", PRIV_CONF|PRIV_SUBIF); 


#undef	ALTERNATE
#define	ALTERNATE	ci_atm_smds
