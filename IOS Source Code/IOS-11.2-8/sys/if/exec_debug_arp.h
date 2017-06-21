/* $Id: exec_debug_arp.h,v 3.2 1995/11/17 09:27:07 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/if/exec_debug_arp.h,v $
 *------------------------------------------------------------------
 * E X E C _ D E B U G _ A R P . H
 *
 * Copyright (c) 1992-1995 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_debug_arp.h,v $
 * Revision 3.2  1995/11/17  09:27:07  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:49:06  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  10:53:19  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize os, hes, les, if, and ibm.
 *
 * Revision 2.2  1995/06/19  14:47:12  widmer
 * CSCdi36074:  Clean up debug parse chain alternates
 * Add keyword_debug macro to assign debug array
 *
 * Revision 2.1  1995/06/07  22:17:29  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * debug arp
 *
 */
EOLS	(debug_arp_eol, debug_command, DEBUG_ARP);
KEYWORD_DEBUG(debug_arp, debug_arp_eol, ALTERNATE,
	      OBJ(pdb,1), arp_debug_arr,
	      "arp", "IP ARP and HP Probe transactions", PRIV_OPR);

#undef	ALTERNATE
#define	ALTERNATE	debug_arp
