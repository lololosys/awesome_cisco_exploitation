/* $Id: exec_clear_arp.h,v 3.3.20.1 1996/08/12 16:02:27 widmer Exp $
 * $Source: /release/112/cvs/Xsys/if/exec_clear_arp.h,v $
 *------------------------------------------------------------------
 * E X E C _ C L E A R _ A R P . H
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_clear_arp.h,v $
 * Revision 3.3.20.1  1996/08/12  16:02:27  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.3  1996/01/29  07:08:49  hampton
 * Remove command number arguments passed to parse routines that only
 * perform a single function.  [CSCdi47717]
 *
 * Revision 3.2  1995/11/17  09:27:04  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:49:01  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  10:53:18  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize os, hes, les, if, and ibm.
 *
 * Revision 2.1  1995/06/07  22:15:23  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * clear arp-cache
 *
 */
EOLNS	(clear_arp_eol, arp_clear_command);
KEYWORD (clear_arp, clear_arp_eol, ALTERNATE,
	 "arp-cache", "Clear the entire ARP cache", PRIV_OPR);


#undef	ALTERNATE
#define	ALTERNATE	clear_arp
