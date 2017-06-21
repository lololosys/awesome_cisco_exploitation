/* $Id: exec_debug_tbridge.h,v 3.2.62.2 1996/05/04 01:49:02 wilber Exp $
 * $Source: /release/112/cvs/Xsys/srt/exec_debug_tbridge.h,v $
 *------------------------------------------------------------------
 * E X E C _ D E B U G _ T B R I D G E . H
 *
 * May 1995, Tony Speakman
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_debug_tbridge.h,v $
 * Revision 3.2.62.2  1996/05/04  01:49:02  wilber
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
 * Revision 3.2.62.1  1996/03/27  09:02:59  speakman
 * CSCdi51444:  Blocked bridge ports do not respond to ARP broadcasts.
 * Branch: California_branch
 * Add a debug switch to display packets received in the tbridge path.
 *
 * Revision 3.2  1995/11/17  18:53:42  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:30:47  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/04  01:58:09  tli
 * CSCdi36508: Rationalize parser file placement, part I
 * Rationalize cls dev dlsw dspu filesys irix lat les lex llc2 parser pgen
 * pt servers smf smrp snmp src-36-c1000 src-4k-c4000 src-68-c3000
 * src-68-c4000 src-c4000 src-rsp srt tcl ts ui
 *
 * Revision 2.2  1995/06/19  14:49:13  widmer
 * CSCdi36074:  Clean up debug parse chain alternates
 * Add keyword_debug macro to assign debug array
 *
 * Revision 2.1  1995/06/07  22:20:20  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * debug tbridge
 *
 */

LINK_TRANS(debug_tbridge_cmf_extend_here, NONE);

EOLS	(debug_tbridge_cbus_fld_eol, debug_command, DEBUG_TBRIDGE_CBUS_FLOOD);
KEYWORD (debug_tbridge_cbus_fld, debug_tbridge_cbus_fld_eol,
	 debug_tbridge_cmf_extend_here,
	 "flood", "autonomously flooded packets", PRIV_OPR|PRIV_HIDDEN);
EOLS	(debug_tbridge_cbus_eol, debug_command, DEBUG_TBRIDGE_CBUS);
KEYWORD (debug_tbridge_cbus, debug_tbridge_cbus_eol, debug_tbridge_cbus_fld,
	 "autonomous", "autonomous bridging statistics", PRIV_OPR);
EOLS	(debug_tbridge_ipm_eol, tbridge_debug_command, DEBUG_TBRIDGE_IPM);
KEYWORD (debug_tbridge_ipm, debug_tbridge_ipm_eol, debug_tbridge_cbus,
	 "ipmulticast", "IP multicast flooding",
	 PRIV_OPR|PRIV_HIDDEN);
EOLS	(debug_tbridge_crb_eol, tbridge_debug_command, DEBUG_TBRIDGE_CRB);
KEYWORD (debug_tbridge_crb, debug_tbridge_crb_eol, debug_tbridge_ipm,
	 "crb", "concurrent routing and bridging events",
	 PRIV_OPR|PRIV_HIDDEN);
EOLS	(debug_tbridge_mcast_2_eol, tbridge_debug_command,
	 DEBUG_TBRIDGE_MCAST_2);
KEYWORD (debug_tbridge_mcast_2, debug_tbridge_mcast_2_eol, debug_tbridge_crb,
	 "multicast-2", "multicasts bridged in the fast path",
	 PRIV_OPR|PRIV_HIDDEN);
EOLS	(debug_tbridge_dis_1_eol, tbridge_debug_command, DEBUG_TBRIDGE_DIS_1);
KEYWORD (debug_tbridge_dis_1, debug_tbridge_dis_1_eol, debug_tbridge_mcast_2,
	 "discard-1", "packets discarded in the rx functions",
	 PRIV_OPR|PRIV_HIDDEN);
EOLS	(debug_tbridge_rcv_1_eol, tbridge_debug_command, DEBUG_TBRIDGE_RCV_1);
KEYWORD (debug_tbridge_rcv_1, debug_tbridge_rcv_1_eol, debug_tbridge_dis_1,
	 "receive-1", "packets received in the rx functions",
	 PRIV_OPR|PRIV_HIDDEN);
EOLS	(debug_tbridge_pak_3_eol, tbridge_debug_command, DEBUG_TBRIDGE_PAK_3);
KEYWORD (debug_tbridge_pak_3, debug_tbridge_pak_3_eol, debug_tbridge_rcv_1,
	 "packet-3", "packets bridged in the process path",
	 PRIV_OPR|PRIV_HIDDEN);
EOLS	(debug_tbridge_pak_2_eol, tbridge_debug_command, DEBUG_TBRIDGE_PAK_2);
KEYWORD (debug_tbridge_pak_2, debug_tbridge_pak_2_eol, debug_tbridge_pak_3,
	 "packet-2", "packets bridged in the fast path",
	 PRIV_OPR|PRIV_HIDDEN);
EOLS	(debug_tbridge_pak_1_eol, tbridge_debug_command, DEBUG_TBRIDGE_PAK_1);
KEYWORD (debug_tbridge_pak_1, debug_tbridge_pak_1_eol, debug_tbridge_pak_2,
	 "packet-1", "packets read in the rx functions",
	 PRIV_OPR|PRIV_HIDDEN);
EOLS	(debug_tbridge_pak_0_eol, tbridge_debug_command, DEBUG_TBRIDGE_PAK_0);
KEYWORD (debug_tbridge_pak_0, debug_tbridge_pak_0_eol, debug_tbridge_pak_1,
	 "packet-0", "packets received in the rx interrupt",
	 PRIV_OPR|PRIV_HIDDEN);

KEYWORD_DEBUG(debug_tbridge, debug_tbridge_pak_0, ALTERNATE,
	      OBJ(pdb,1), tbridge_debug_arr,
	      "tbridge", "Transparent Bridging", PRIV_OPR);

#undef	ALTERNATE
#define	ALTERNATE	debug_tbridge
