/* $Id: exec_clear_atm_arp.h,v 3.1.2.2 1996/08/12 16:00:30 widmer Exp $
 * $Source: /release/112/cvs/Xsys/atm/exec_clear_atm_arp.h,v $
 *------------------------------------------------------------------
 * exec_clear_atm_arp.h: clear atm arp command parsing.
 *
 * May 1996, Aditi Chopra
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: exec_clear_atm_arp.h,v $
 * Revision 3.1.2.2  1996/08/12  16:00:30  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.1.2.1  1996/05/14  06:28:54  achopra
 * CSCdi47186:  Implement clear atm arp command.
 * Branch: California_branch
 *
 * Revision 3.1  1996/05/14  06:08:02  achopra
 * Placeholder for atm clear arp command.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/******************************************************************
 * clear atm arp <interface> { '*' | <ip-addr> }
 *
 * OBJ(idb,1) = <atm interface>
 * OBJ(int,1) = TRUE if <ip-addr> is specified else FALSE if '*'
 * OBJ(paddr,1) = <ip-addr>
 */
EOLNS    (clear_atm_arp_eol, atm_arp_clear);
KEYWORD (clear_atm_arp_all, clear_atm_arp_eol, no_alt,
         "*", "Delete all ARP entries", PRIV_OPR);

SET     (clear_atm_arp_set, clear_atm_arp_eol, OBJ(int,1), TRUE);
IPADDR  (clear_atm_arp_addr, clear_atm_arp_set, clear_atm_arp_all,
	 OBJ(paddr,1), "IP address of the entry to delete");
INTERFACE (clear_atm_arp_if, clear_atm_arp_addr, no_alt, OBJ(idb,1),
	   IFTYPE_ATM | IFTYPE_SUBIF);
KEYWORD_ID (clear_atm_arp, clear_atm_arp_if, ALTERNATE,
	    OBJ(int,1), FALSE,
	    "arp", "Clear ATM ARP entries on an interface", PRIV_OPR);

#undef  ALTERNATE
#define ALTERNATE       clear_atm_arp
