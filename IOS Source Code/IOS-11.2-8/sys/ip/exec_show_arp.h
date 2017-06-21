/* $Id: exec_show_arp.h,v 3.3.20.1 1996/08/12 16:02:55 widmer Exp $
 * $Source: /release/112/cvs/Xsys/ip/exec_show_arp.h,v $
 *------------------------------------------------------------------
 * E X E C _ S H O W _ A R P . H
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_show_arp.h,v $
 * Revision 3.3.20.1  1996/08/12  16:02:55  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.3  1996/01/29  07:08:54  hampton
 * Remove command number arguments passed to parse routines that only
 * perform a single function.  [CSCdi47717]
 *
 * Revision 3.2  1995/11/17  09:32:45  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:54:38  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/01  12:55:43  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize ip, ipmulticast, tcp
 *
 * Revision 2.1  1995/06/07  22:22:11  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/******************************************************************
 *  show arp
 *
 */
EOLNS	(show_arp_eol, ip_arp_show_command);
KEYWORD (ip_arp_show_command, show_arp_eol, ALTERNATE,
	 "arp", "ARP table", PRIV_USER|PRIV_USER_HIDDEN);


#undef	ALTERNATE
#define	ALTERNATE	ip_arp_show_command
