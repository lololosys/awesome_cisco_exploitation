/* $Id: exec_show_ip_irdp.h,v 3.2.60.1 1996/08/12 16:04:08 widmer Exp $
 * $Source: /release/112/cvs/Xsys/iprouting/exec_show_ip_irdp.h,v $
 *------------------------------------------------------------------
 * IRDP show commands
 *
 * May 1993, Robert Widmer
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: exec_show_ip_irdp.h,v $
 * Revision 3.2.60.1  1996/08/12  16:04:08  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.2  1995/11/17  17:33:25  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:07:19  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/01  12:16:19  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize iprouting
 *
 * Revision 2.1  1995/06/07  22:24:30  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * show ip irdp
 */
EOLNS	(show_ip_irdp_eol, show_ipirdp);
INTERFACE(show_ip_irdp_interface, show_ip_irdp_eol, show_ip_irdp_eol,
	  OBJ(idb,1), IFTYPE_ANYSUB);
KEYWORD	(show_ip_irdp_kwd, show_ip_irdp_interface, ALTERNATE,
	 "irdp", "ICMP Router Discovery Protocol", PRIV_USER);

#undef	ALTERNATE
#define	ALTERNATE	show_ip_irdp_kwd
