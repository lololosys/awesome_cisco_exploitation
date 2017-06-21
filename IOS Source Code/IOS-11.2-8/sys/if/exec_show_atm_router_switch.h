/* $Id: exec_show_atm_router_switch.h,v 3.1.4.1 1996/04/25 23:13:27 ronnie Exp $
 * $Source: /release/112/cvs/Xsys/if/exec_show_atm_router_switch.h,v $
 *------------------------------------------------------------------
 * exec_show_atm_router_switch.h: SHOW command parsing
 *
 * March, 1996, Rob Zagst
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: exec_show_atm_router_switch.h,v $
 * Revision 3.1.4.1  1996/04/25  23:13:27  ronnie
 * CSCdi55766:  Hey!  This ATM and Frame Relay is no different than in 11.1
 * Feature commit for ATM and Frame Relay into 11.2
 * Branch: California_branch
 *
 * Revision 3.1  1996/03/30  04:47:02  rzagst
 * Placeholder for exec_show_atm_router_switch.h.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/******************************************************************
 * show ATM map
 */
EOLS	(show_atm_map_eol, atm_show_commands,   SHOW_ATM_MAP);
KEYWORD (show_atm_map, show_atm_map_eol, ALTERNATE,
            "map", "ATM static mapping", PRIV_USER);


#undef	ALTERNATE
#define	ALTERNATE	show_atm_map
