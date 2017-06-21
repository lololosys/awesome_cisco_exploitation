/* $Id: exec_clear_novell_sse.h,v 3.2.62.1 1996/08/16 00:14:16 hampton Exp $
 * $Source: /release/112/cvs/Xsys/xns/exec_clear_novell_sse.h,v $
 *------------------------------------------------------------------
 * exec_clear_novell_sse.h
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_clear_novell_sse.h,v $
 * Revision 3.2.62.1  1996/08/16  00:14:16  hampton
 * Change parser comments to reflect ipx command usage.  [CSCdi66208]
 * Branch: California_branch
 *
 * Revision 3.2  1995/11/17  19:21:25  gstovall
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:57:44  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  01:43:00  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize xns, atalk, vines, decnet, xremote, x25, unix, wan, tn3270,
 * clns, cdp
 *
 * Revision 2.1  1995/06/07  22:16:20  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * clear ipx sse
 *
 */
EOLS	(clear_ipx_sse_eol, clear_sse, SSE_PROTOCOL_IPX);
KEYWORD_ID (clear_novell_sse, clear_ipx_sse_eol, ALTERNATE,
	    OBJ(int,1), 0,
	    "sse", "Delete SSE cache table entries", PRIV_OPR);

#undef  ALTERNATE
#define ALTERNATE 	clear_novell_sse
