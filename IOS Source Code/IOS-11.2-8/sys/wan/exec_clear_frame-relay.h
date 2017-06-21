/* $Id: exec_clear_frame-relay.h,v 3.3.20.1 1996/08/12 16:09:37 widmer Exp $
 * $Source: /release/112/cvs/Xsys/wan/exec_clear_frame-relay.h,v $
 *------------------------------------------------------------------
 * E X E C _ C L E A R _ F R A M E - R E L A Y . H
 * February 1993, G. Fowler
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_clear_frame-relay.h,v $
 * Revision 3.3.20.1  1996/08/12  16:09:37  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.3  1996/01/29  07:10:16  hampton
 * Remove command number arguments passed to parse routines that only
 * perform a single function.  [CSCdi47717]
 *
 * Revision 3.2  1995/11/17  18:01:48  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:48:39  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  01:46:02  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize xns, atalk, vines, decnet, xremote, x25, unix, wan, tn3270,
 * clns, cdp
 *
 * Revision 2.1  1995/06/07  22:15:51  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * clear frame-relay-inarp
 *
 */
EOLNS	(clear_fr_inarp_eol, clear_fr_inarp);
KEYWORD (clear_fr_inarp, clear_fr_inarp_eol, ALTERNATE,
	 "frame-relay-inarp", "Clear inverse ARP entries from the map table",
	 PRIV_OPR);

#undef	ALTERNATE
#define	ALTERNATE	clear_fr_inarp
