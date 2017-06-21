/* $Id: exec_debug_atm_global.h,v 3.1.4.2 1996/05/21 09:49:44 thille Exp $
 * $Source: /release/112/cvs/Xsys/if/exec_debug_atm_global.h,v $
 *------------------------------------------------------------------
 * exec_debug_atm_global.h: DEBUG command parsing code
 *
 * March, 1996, Rob Zagst
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: exec_debug_atm_global.h,v $
 * Revision 3.1.4.2  1996/05/21  09:49:44  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.1.4.1  1996/04/25  23:13:23  ronnie
 * CSCdi55766:  Hey!  This ATM and Frame Relay is no different than in 11.1
 * Feature commit for ATM and Frame Relay into 11.2
 * Branch: California_branch
 *
 * Revision 3.1  1996/03/30  04:44:01  rzagst
 * Placeholder for exec_debug_atm_global.h.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/******************************************************************
 * Debug ATM errors
 */
EOLS	(debug_atm_errors_eol, atm_debug_command, DEBUG_ATMERRORS);
KEYWORD	(debug_atm_errors, debug_atm_errors_eol, ALTERNATE,
	"errors", "ATM errors", PRIV_OPR);

/******************************************************************
 * Debug ATM packets
 */
EOLS	(debug_atm_packet_eol, atm_debug_command, DEBUG_ATMPACKET);
NUMBER  (debug_atm_packet_vcno, debug_atm_packet_eol, no_alt,
        OBJ(int,3),1, ATM_MAX_VCNUM, "ATM VCD number");
KEYWORD_ID (debug_atm_packet_vc_prompt,debug_atm_packet_vcno,
	    debug_atm_packet_eol,OBJ(int,2),DEBUG_ATM_VC,"vcd",
	    "ATM VCD Number",PRIV_OPR);
INTERFACE (debug_atm_packet_int, debug_atm_packet_vc_prompt, 
	  no_alt ,OBJ(idb,1), (IFTYPE_ATM | IFTYPE_SUBIF));
KEYWORD_ID (debug_atm_packet_interface,debug_atm_packet_int,
	    debug_atm_packet_eol,OBJ(int,1),DEBUG_ATM_INTERFACE,common_str_interface,
	    "ATM Interface",PRIV_OPR);
KEYWORD	(debug_atm_packet, debug_atm_packet_interface, debug_atm_errors,
	"packet", "ATM packets", PRIV_OPR);


#undef	ALTERNATE
#define	ALTERNATE	debug_atm_packet
