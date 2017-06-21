/* $Id: exec_show_sysmgt_rpc.h,v 3.2.60.1 1996/08/12 16:02:38 widmer Exp $
 * $Source: /release/112/cvs/Xsys/if/exec_show_sysmgt_rpc.h,v $
 *------------------------------------------------------------------
 * E X E C _ S H O W _ S Y S M G T _ R P C . H
 *
 * April 1995, Mike Otto
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: exec_show_sysmgt_rpc.h,v $
 * Revision 3.2.60.1  1996/08/12  16:02:38  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.2  1995/11/17  09:27:26  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:49:30  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  10:53:26  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize os, hes, les, if, and ibm.
 *
 * Revision 2.1  1995/06/07  22:26:17  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * show sysmgt_rpc stats
 *
 */
EOLNS	(show_sysmgt_rpc_stats_eol, show_sysmgt_rpc);
KEYWORD (show_sysmgt_rpc_stats, show_sysmgt_rpc_stats_eol, no_alt,
         "statistics", NULL /*"Show statistics"*/,
         PRIV_HIDDEN|PRIV_NOHELP);
KEYWORD (show_sysmgt_rpc, show_sysmgt_rpc_stats, ALTERNATE,
         "sysmgt_rpc", NULL /*"System Management RPC"*/,
	 PRIV_HIDDEN|PRIV_NOHELP);

#undef ALTERNATE
#define ALTERNATE	show_sysmgt_rpc
