/* $Id: exec_clear_bridge.h,v 3.3.20.3 1996/08/12 16:07:47 widmer Exp $
 * $Source: /release/112/cvs/Xsys/srt/exec_clear_bridge.h,v $
 *------------------------------------------------------------------
 * E X E C _ C L E A R _ B R I D G E . H
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_clear_bridge.h,v $
 * Revision 3.3.20.3  1996/08/12  16:07:47  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.3.20.2  1996/06/18  15:41:46  widmer
 * CSCdi60634:  clear command help is wrong
 * Branch: California_branch
 *
 * Revision 3.3.20.1  1996/05/04  01:49:01  wilber
 * Branch: California_branch
 *
 * Integrated Routing and Bridging
 * - Bridge-group Virtual Interface configuration & bridging
 *   (process-level) support.
 * - BVI smf functions and registry.
 * - BVI settable mtu support & dynamically select mtu for the BVI.
 * - 'bridge irb' & 'bridge <x> route/bridge <protcocol>' support.
 *
 * Constrained Multicast Flooding
 * - Add the CMF parser link points.
 * - Add the CMF data structures to spantype.
 * - Add the creation and deletion of the tbridge monitor.
 * - Convert tbridge flood array to a linked list of tbifd's.
 * - Manage the CMF data structures when the spanning tree changes.
 *
 * Revision 3.3  1996/01/29  07:09:43  hampton
 * Remove command number arguments passed to parse routines that only
 * perform a single function.  [CSCdi47717]
 *
 * Revision 3.2  1995/11/17  18:53:24  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:30:40  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/04  01:58:01  tli
 * CSCdi36508: Rationalize parser file placement, part I
 * Rationalize cls dev dlsw dspu filesys irix lat les lex llc2 parser pgen
 * pt servers smf smrp snmp src-36-c1000 src-4k-c4000 src-68-c3000
 * src-68-c4000 src-c4000 src-rsp srt tcl ts ui
 *
 * Revision 2.1  1995/06/07  22:15:27  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * clear bridge
 *
 */
EOLNS	(clear_bridge_eol, clear_bridgecommand);

LINK_TRANS(clear_bridge_cmf_return_here, NONE);
LINK_TRANS(clear_bridge_cmf_extend_here, clear_bridge_eol);

NUMBER	(clear_bridge_group, clear_bridge_cmf_extend_here,
	 clear_bridge_cmf_extend_here,
	 OBJ(int,1), 1, SPAN_MAXINDEX - 1, "A bridge group number");

KEYWORD (clear_bridge_kwd, clear_bridge_group, NONE,
	 "bridge", "Reset bridge forwarding cache", PRIV_OPR);

TEST_BOOLEAN(clear_bridge, clear_bridge_kwd, NONE, ALTERNATE, bridge_enable);

#undef	ALTERNATE
#define	ALTERNATE	clear_bridge
