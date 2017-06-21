/* $Id: exec_show_rif.h,v 3.3.20.1 1996/08/12 16:02:37 widmer Exp $
 * $Source: /release/112/cvs/Xsys/if/exec_show_rif.h,v $
 *------------------------------------------------------------------
 * E X E C _ S H O W _ R I F . H
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_show_rif.h,v $
 * Revision 3.3.20.1  1996/08/12  16:02:37  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.3  1996/01/29  07:08:51  hampton
 * Remove command number arguments passed to parse routines that only
 * perform a single function.  [CSCdi47717]
 *
 * Revision 3.2  1995/11/17  09:27:25  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:49:28  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  10:53:26  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize os, hes, les, if, and ibm.
 *
 * Revision 2.1  1995/06/07  22:25:34  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/******************************************************************
 * show rif
 */
EOLNS	(show_rif_eol, display_tr_rif);
KEYWORD (show_rif, show_rif_eol, NONE,
	 "rif", "RIF cache entries", PRIV_USER|PRIV_USER_HIDDEN);

ASSERT(show_rif_test, show_rif, ALTERNATE, (bridge_enable || nTRs));

#undef	ALTERNATE
#define	ALTERNATE	show_rif_test
