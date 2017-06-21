/* $Id: cfg_int_atm_global.h,v 3.1.4.1 1996/04/25 23:13:04 ronnie Exp $
 * $Source: /release/112/cvs/Xsys/if/cfg_int_atm_global.h,v $
 *------------------------------------------------------------------
 * cfg_int_atm_global.h: ATM parser commands global to all platforms
 *
 * March 1996, Rob Zagst
 *
 * Copyright (c) 1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cfg_int_atm_global.h,v $
 * Revision 3.1.4.1  1996/04/25  23:13:04  ronnie
 * CSCdi55766:  Hey!  This ATM and Frame Relay is no different than in 11.1
 * Feature commit for ATM and Frame Relay into 11.2
 * Branch: California_branch
 *
 * Revision 3.1  1996/03/30  04:37:11  rzagst
 * Place holder for cfg_int_atm_global.h
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Below are the ATM Configuration options.  They MUST appear before the
 * VC config. options.
 */

/******************************************************************
 * ATM idle-timeout <seconds>
 *
 * OBJ(int,1) = seconds to timeout to
 */
PARAMS_KEYONLY(ci_atm_it, ALTERNATE, "idle-timeout",
	       OBJ(int,1), 0, ATM_MAX_TIMER,
	       atm_interface_commands, ATM_CMD_IDLETIMEOUT,
	       "Set idle time before disconnecting a SVC",
	       "Idle timeout in seconds", PRIV_CONF);


/******************************************************************
 * ATM multipoint-interval <seconds>
 *
 * OBJ(int,1) = seconds in interval
 */
PARAMS_KEYONLY(ci_atm_mpt_interval, ci_atm_it, "multipoint-interval",
	       OBJ(int,1), 0, ATM_MAX_TIMER,
	       atm_interface_commands, ATM_CMD_MTP_INTERVAL,
	       "Set minimum interval between multipoint party additions",
	       "Interval time in seconds", PRIV_CONF);


/******************************************************************
 * ATM [no] multipoint-signalling 
 *
 */
EOLS    (ci_atm_mpt_sig_eol, atm_interface_commands, ATM_CMD_MTP_SIG);
KEYWORD (ci_atm_mpt_sig, ci_atm_mpt_sig_eol, ci_atm_mpt_interval,
	 "multipoint-signalling", "Multipoint Signalling", 
	 PRIV_CONF | PRIV_SUBIF);


#undef	ALTERNATE
#define	ALTERNATE	ci_atm_mpt_sig
