/* $Id: exec_clear_decnet.h,v 3.2.60.1 1996/08/12 16:01:05 widmer Exp $
 * $Source: /release/112/cvs/Xsys/decnet/exec_clear_decnet.h,v $
 *------------------------------------------------------------------
 * E X E C _ C L E A R _ D E C N E T . H
 *
 * July 1992 Arun Sastry
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Contains parser keyword info for the 'clear decnet command'
 *------------------------------------------------------------------
 * $Log: exec_clear_decnet.h,v $
 * Revision 3.2.60.1  1996/08/12  16:01:05  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.2  1995/11/17  08:59:17  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:16:39  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  01:44:29  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize xns, atalk, vines, decnet, xremote, x25, unix, wan, tn3270,
 * clns, cdp
 *
 * Revision 2.1  1995/06/07  22:15:40  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
/******************************************************************
 * clear decnet counter
 *
 */
EOLNS	(clear_decnet_eol, decnet_clear_counters);
KEYWORD_ID (clear_decnet_counter_kwd, clear_decnet_eol, no_alt, 
            OBJ(int,1), CLEAR_DECNET_COUNTERS, 
            "counter", "Clear DECnet counters", PRIV_OPR );
KEYWORD	(clear_decnet_kwd, clear_decnet_counter_kwd, ALTERNATE,
	 "decnet", "Reset DECnet information", PRIV_OPR );

#undef	ALTERNATE
#define	ALTERNATE	clear_decnet_kwd
