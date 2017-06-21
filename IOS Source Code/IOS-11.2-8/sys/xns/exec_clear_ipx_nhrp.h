/* $Id: exec_clear_ipx_nhrp.h,v 3.2.64.1 1996/08/12 16:10:00 widmer Exp $
 * $Source: /release/112/cvs/Xsys/xns/exec_clear_ipx_nhrp.h,v $
 *------------------------------------------------------------------
 * Next Hop Resolution Protocol.
 *
 * October 1995, Bruce Cole
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: exec_clear_ipx_nhrp.h,v $
 * Revision 3.2.64.1  1996/08/12  16:10:00  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.2  1995/11/17  19:21:21  gstovall
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:57:41  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:38:15  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/10/06  21:55:09  bcole
 * Placeholder files for 11.1 NHRP work.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * clear ipx nhrp [addr]
 *
 * OBJ(paddr,1) = network #
 */

EOLNS	(clear_novell_nhrp_eol, nhrp_ipx_clear);
NOVADDR (clear_novell_nhrp_addr, clear_novell_nhrp_eol, clear_novell_nhrp_eol,
	OBJ(paddr,1), "Destination network to delete");
KEYWORD(clear_novell_nhrp, clear_novell_nhrp_addr, ALTERNATE,
	"nhrp", "NHRP cache", PRIV_OPR);

#undef	ALTERNATE
#define	ALTERNATE	clear_novell_nhrp
