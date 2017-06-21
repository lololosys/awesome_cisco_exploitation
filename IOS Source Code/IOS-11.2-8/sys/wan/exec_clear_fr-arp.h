/* $Id: exec_clear_fr-arp.h,v 3.1.22.1 1996/04/25 23:23:07 ronnie Exp $
 * $Source: /release/112/cvs/Xsys/wan/exec_clear_fr-arp.h,v $
 *------------------------------------------------------------------
 * Frame Relay ARP clear command
 *
 * January 1996, Charles Poe
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: exec_clear_fr-arp.h,v $
 * Revision 3.1.22.1  1996/04/25  23:23:07  ronnie
 * CSCdi55766:  Hey!  This ATM and Frame Relay is no different than in 11.1
 * Feature commit for ATM and Frame Relay into 11.2
 * Branch: California_branch
 *
 * Revision 3.1.30.1  1996/04/24  04:11:12  shankar
 * Branch: ATM_Cal_branch
 * Patch FR_SVC_branch changes
 *
 * Revision 3.1  1996/02/14  22:36:24  ccpoe
 * Frame Relay ARP
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/******************************************************************
 * clear frame-relay-inarp
 *
 */
EOLS	(clear_fr_inarp_eol, clear_fr_inarp, FR_CLEAR_INARP);
KEYWORD (clear_fr_inarp, clear_fr_inarp_eol, ALTERNATE,
	 "frame-relay-inarp", "Clear inverse ARP entries from the map table",
	 PRIV_OPR);

#undef	ALTERNATE
#define	ALTERNATE	clear_fr_inarp
