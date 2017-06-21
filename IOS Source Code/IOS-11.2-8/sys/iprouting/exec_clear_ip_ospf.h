/* $Id: exec_clear_ip_ospf.h,v 3.2.60.1 1996/08/12 16:04:03 widmer Exp $ 
 * $Source: /release/112/cvs/Xsys/iprouting/exec_clear_ip_ospf.h,v $
 *------------------------------------------------------------------
 * OSPF clear commands
 *
 * June 1994, Arpakorn Boonkongchuen
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: exec_clear_ip_ospf.h,v $
 * Revision 3.2.60.1  1996/08/12  16:04:03  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.2  1995/11/17  17:33:06  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:06:56  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/01  12:16:11  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize iprouting
 *
 * Revision 2.1  1995/06/07  22:16:11  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * clear ip ospf redistribution
 *
 */
EOLNS (clear_ip_ospf_redist_eol, ospf_clear_redistribute);
KEYWORD (clear_ip_ospf_redist, clear_ip_ospf_redist_eol, no_alt,
	 "redistribution", "Clear OSPF route redistribution", PRIV_OPR);
KEYWORD (clear_ip_ospf, clear_ip_ospf_redist, ALTERNATE,
	 "ospf", "Clear OSPF", PRIV_OPR);

#undef	ALTERNATE
#define	ALTERNATE	clear_ip_ospf
