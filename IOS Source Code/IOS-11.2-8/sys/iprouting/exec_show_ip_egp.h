/* $Id: exec_show_ip_egp.h,v 3.2.60.1 1996/08/12 16:04:07 widmer Exp $
 * $Source: /release/112/cvs/Xsys/iprouting/exec_show_ip_egp.h,v $
 *------------------------------------------------------------------
 * EGP show commands
 *
 * May 1993, Robert Widmer
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: exec_show_ip_egp.h,v $
 * Revision 3.2.60.1  1996/08/12  16:04:07  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.2  1995/11/17  17:33:24  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:07:17  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/19  10:50:30  tli
 * CSCdi36065:  Rationalize EGP parser file placement
 * Move parser support to iprouting subdirectory.
 *
 * Revision 2.1  1995/06/07  22:24:25  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * show ip egp
 * 
 */
EOLNS	(show_ip_egp_eol, egp_show_neighbors);
KEYWORD (show_ip_egp, show_ip_egp_eol, ALTERNATE,
	 "egp", "EGP connections and statistics", PRIV_USER);

#undef	ALTERNATE
#define	ALTERNATE	show_ip_egp
