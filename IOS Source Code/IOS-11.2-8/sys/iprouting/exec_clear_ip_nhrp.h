/* $Id: exec_clear_ip_nhrp.h,v 3.2.62.1 1996/08/12 16:04:02 widmer Exp $
 * $Source: /release/112/cvs/Xsys/iprouting/exec_clear_ip_nhrp.h,v $
 *------------------------------------------------------------------
 * Next Hop Resolution Protocol.
 *
 * October 1995, Bruce Cole
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: exec_clear_ip_nhrp.h,v $
 * Revision 3.2.62.1  1996/08/12  16:04:02  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.2  1995/11/17  17:33:04  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:06:55  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:06:19  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/10/06  21:54:55  bcole
 * Placeholder files for 11.1 NHRP work.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * clear ip nhrp [addr [mask]]
 *
 * OBJ(paddr,2) = network mask
 * OBJ(paddr,1) = network #
 */

EOLNS	(clear_ip_nhrp_eol, nhrp_ip_clear);
IPADDR	(clear_ip_nhrp_mask, clear_ip_nhrp_eol, clear_ip_nhrp_eol,
	 OBJ(paddr,2), "Destination network mask");
IPADDR (clear_ip_nhrp_addr, clear_ip_nhrp_mask, clear_ip_nhrp_eol,
	OBJ(paddr,1), "Destination network to delete");
KEYWORD(clear_ip_nhrp, clear_ip_nhrp_addr, ALTERNATE,
	"nhrp", "NHRP cache", PRIV_OPR);

#undef	ALTERNATE
#define	ALTERNATE	clear_ip_nhrp
