/* $Id: exec_show_atm_arp.h,v 3.1.4.1 1996/04/25 23:02:42 ronnie Exp $
 * $Source: /release/112/cvs/Xsys/atm/exec_show_atm_arp.h,v $
 *------------------------------------------------------------------
 * exec_show_atm_arp.h: show atm arp commands parsing
 *
 * March 1996, Rob Zagst
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: exec_show_atm_arp.h,v $
 * Revision 3.1.4.1  1996/04/25  23:02:42  ronnie
 * CSCdi55766:  Hey!  This ATM and Frame Relay is no different than in 11.1
 * Feature commit for ATM and Frame Relay into 11.2
 * Branch: California_branch
 *
 * Revision 3.1  1996/03/30  05:53:40  rzagst
 * Place holder for atm modularity files.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/******************************************************************
 * show ATM arp-server <interface>
 * OBJ(idb,1) = <interface>
 *	if no interface is specified, show all
 */
EOLS	(show_atm_arpserver_eol, atm_show_arp_commands, SHOW_ATM_ARPSERVER);
INTERFACE (show_atm_arpserver_int, show_atm_arpserver_eol,
	   show_atm_arpserver_eol,
	   OBJ(idb,1), (IFTYPE_ATM | IFTYPE_SUBIF));
KEYWORD	(show_atm_arpserver, show_atm_arpserver_int, ALTERNATE,
	 "arp-server", "ATM ARP Server Table", PRIV_USER);

#undef	ALTERNATE
#define	ALTERNATE	show_atm_arpserver
