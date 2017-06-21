/* $Id: exec_debug_atm_common.h,v 3.1.4.2 1996/05/21 09:49:44 thille Exp $
 * $Source: /release/112/cvs/Xsys/if/exec_debug_atm_common.h,v $
 *------------------------------------------------------------------
 * exec_debug_atm_common.h: DEBUG command parsing code
 *
 * March, 1996, Rob Zagst
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: exec_debug_atm_common.h,v $
 * Revision 3.1.4.2  1996/05/21  09:49:44  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.1.4.1  1996/04/25  23:13:21  ronnie
 * CSCdi55766:  Hey!  This ATM and Frame Relay is no different than in 11.1
 * Feature commit for ATM and Frame Relay into 11.2
 * Branch: California_branch
 *
 * Revision 3.1  1996/03/30  04:42:37  rzagst
 * Placeholder for exec_debug_atm_common.h.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * debug ATM OAM
 */
EOLS	(debug_atm_oam_eol, atm_debug_command, DEBUG_ATMOAM);
INTERFACE (debug_atm_oam_int, debug_atm_oam_eol, no_alt,
	   OBJ(idb,1), (IFTYPE_ATM | IFTYPE_SUBIF));
KEYWORD_ID (debug_atm_oam_interface,debug_atm_oam_int,debug_atm_oam_eol,
	    OBJ(int,1),DEBUG_ATM_INTERFACE,common_str_interface,"ATM Interface",
	    PRIV_OPR);
KEYWORD	(debug_atm_oam, debug_atm_oam_interface, ALTERNATE,
	"oam", "Dump OAM Cells", PRIV_OPR);

/******************************************************************
 * Debug ATM interface(events) only
 */
EOLS	(debug_atm_event_eol, atm_debug_command, DEBUG_ATMEVENTS);
KEYWORD (debug_atm_event, debug_atm_event_eol, debug_atm_oam,
	"events", "ATM Events", PRIV_OPR);

#undef	ALTERNATE
#define	ALTERNATE	debug_atm_event
