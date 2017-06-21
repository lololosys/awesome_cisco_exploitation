/* $Id: exec_debug_atm_arp.h,v 3.1.4.1 1996/04/25 23:02:38 ronnie Exp $
 * $Source: /release/112/cvs/Xsys/atm/exec_debug_atm_arp.h,v $
 *------------------------------------------------------------------
 * exec_debug_atm_arp.h: DEBUG command parsing code
 *
 * March, 1996, Rob Zagst
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: exec_debug_atm_arp.h,v $
 * Revision 3.1.4.1  1996/04/25  23:02:38  ronnie
 * CSCdi55766:  Hey!  This ATM and Frame Relay is no different than in 11.1
 * Feature commit for ATM and Frame Relay into 11.2
 * Branch: California_branch
 *
 * Revision 3.1  1996/03/30  05:53:39  rzagst
 * Place holder for atm modularity files.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/******************************************************************
 * debug ATM arp-server
 */
EOLS	(debug_atm_arpserv_eol, atm_debug_command, DEBUG_ATMARP);
KEYWORD	(debug_atm_arpserv, debug_atm_arpserv_eol, ALTERNATE,
	"arp", "Show ATM ARP events", PRIV_OPR);

#undef	ALTERNATE
#define	ALTERNATE	debug_atm_arpserv

