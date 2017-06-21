/* $Id: exec_show_bridge_sse.h,v 3.2.60.1 1996/08/12 16:01:30 widmer Exp $
 * $Source: /release/112/cvs/Xsys/hes/exec_show_bridge_sse.h,v $
 *------------------------------------------------------------------
 * exec_show_ip_sse.h
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_show_bridge_sse.h,v $
 * Revision 3.2.60.1  1996/08/12  16:01:30  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.2  1995/11/17  09:15:53  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:38:28  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  10:51:54  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize os, hes, les, if, and ibm.
 *
 * Revision 2.1  1995/06/07  22:22:26  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * show bridge [<group>] sse
 * OBJ(int,1) = <group>
 */

EOLNS (show_bridge_sse_eol, show_sse_bridge);
KEYWORD (show_bridge_sse, show_bridge_sse_eol, ALTERNATE,
	 "sse", NULL /*"SSE bridging table"*/, PRIV_USER|PRIV_HIDDEN);

#undef	ALTERNATE
#define	ALTERNATE	show_bridge_sse

