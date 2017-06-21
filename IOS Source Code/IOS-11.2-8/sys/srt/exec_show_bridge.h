/* $Id: exec_show_bridge.h,v 3.4.20.3 1996/09/13 21:40:30 perryl Exp $
 * $Source: /release/112/cvs/Xsys/srt/exec_show_bridge.h,v $
 *------------------------------------------------------------------
 * E X E C _ S H O W _ B R I D G E . H	
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_show_bridge.h,v $
 * Revision 3.4.20.3  1996/09/13  21:40:30  perryl
 * CSCdi64131:  Show bridge not showing Virtual Access interface
 * Branch: California_branch
 *
 * Revision 3.4.20.2  1996/08/12  16:07:50  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.4.20.1  1996/05/04  01:49:03  wilber
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
 * Revision 3.4  1996/01/29  07:09:45  hampton
 * Remove command number arguments passed to parse routines that only
 * perform a single function.  [CSCdi47717]
 *
 * Revision 3.3  1996/01/23  08:54:07  mmcneali
 * CSCdi47323:  vLAN encapsulated IP SSE switched.
 *
 * Faster Baby, Faster!
 *
 * Revision 3.2  1995/11/17  18:53:47  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:30:50  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/04  01:58:10  tli
 * CSCdi36508: Rationalize parser file placement, part I
 * Rationalize cls dev dlsw dspu filesys irix lat les lex llc2 parser pgen
 * pt servers smf smrp snmp src-36-c1000 src-4k-c4000 src-68-c3000
 * src-68-c4000 src-c4000 src-rsp srt tcl ts ui
 *
 * Revision 2.1  1995/06/07  22:22:24  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * show bridge [<group>] [<interface>]
 * show bridge [<group>] [<address> [<mask>]]
 *
 * OBJ(idb,1) = <interface>
 * (*OBJ(hwaddr,1)) = <address>
 * (*OBJ(hwaddr,2)) = <mask>
 */

EOLNS	(show_bridge_eol, show_bridge_table);

LINK_TRANS(show_bridge_extend_here, show_bridge_eol);

ENADDR	(show_bridge_mask, show_bridge_eol, show_bridge_eol,
	OBJ(hwaddr,2), "Ethernet address mask");
ENADDR	(show_bridge_address, show_bridge_mask, show_bridge_extend_here,
	OBJ(hwaddr,1), "Ethernet address");
INTERFACE (show_bridge_interface, show_bridge_eol, show_bridge_address,
	   OBJ(idb,1), IFTYPE_ANYSUB | IFTYPE_VACCESS);

LINK_TRANS(show_bridge_cmf_return_here, NONE);
LINK_TRANS(show_bridge_cmf_extend_here, show_bridge_interface);

/******************************************************************
 *
 * show bridge vlans
 *
 */

EOLNS       (show_tb_vlan_eol, show_transparently_bridged_vlans);
KEYWORD    (show_tbridged_vlans, show_tb_vlan_eol, show_bridge_cmf_extend_here,
            "vlans", "Display Transparently Bridged vLAN configuration",
            PRIV_USER | PRIV_USER_HIDDEN);


/******************************************************************
 * show bridge [<group>] circuit-group [<circgroup>] 
 *			[<srcmacaddr>] [<dstmacaddr>]
 *
 * OBJ(int,2) = <circgroup>
 * (*OBJ(hwaddr,1)) = <srcmacaddr>
 * (*OBJ(hwaddr,2)) = <dstmacaddr>
 */
EOLNS    (show_circuit_group_eol, show_bridge_circuit_group);

ENADDR  (show_circuit_group_address1, show_circuit_group_eol, 
	 show_circuit_group_eol, OBJ(hwaddr,2), "Dst mac address");

ENADDR  (show_circuit_group_address, show_circuit_group_address1,
         show_circuit_group_eol, OBJ(hwaddr,1), "Src mac address");

NUMBER  (show_circuit_group1, show_circuit_group_address,
         show_circuit_group_eol, OBJ(int,2), 1, 9, "Circuit Group number");

KEYWORD (show_circuit_group_kwd, show_circuit_group1, show_tbridged_vlans,
         "circuit-group", "Circuit Group", PRIV_USER|PRIV_USER_HIDDEN);


/******************************************************************
 *
 * show bridge [<group>] <"verbose">
 *
 * OBJ(int,2) = TRUE, if "verbose" entered
 */

KEYWORD_ID (show_btable_verbose, show_bridge_eol, show_circuit_group_kwd,
            OBJ(int,2), TRUE, "verbose",
            "Display the extended Bridge Table", PRIV_USER | PRIV_USER_HIDDEN);


/******************************************************************
 *
 * show bridge [<group>] [<"group">] <"verbose">
 *
 * OBJ(int,1) = <group>
 */

EOLNS       (show_btable_eol, show_bridge_group);

KEYWORD_ID (show_bgroup_extended, show_btable_eol, show_btable_eol,
            OBJ(int,2), TRUE, "verbose",
	    "Display additional Bridge Table information",
	    PRIV_USER | PRIV_HIDDEN);

KEYWORD    (show_bgroup_kwd, show_bgroup_extended, show_btable_verbose,
           "group", "Bridge Group [verbose]", PRIV_USER | PRIV_USER_HIDDEN);


/******************************************************************
 *
 * show bridge [<group>] <interface | mac address <mac mask>>
 *
 * OBJ(int,1) = <group>
 */

NUMBER   (show_btable, show_bgroup_kwd, show_bgroup_kwd,
         OBJ(int,1), 1, SPAN_MAXINDEX - 1, "Bridge Group Number");

KEYWORD  (show_bridge_kwd, show_btable, NONE, "bridge",
	 "Bridge Forwarding/Filtering Database [verbose]",
	 PRIV_USER | PRIV_USER_HIDDEN);

TEST_BOOLEAN(show_bridge_test, show_bridge_kwd, NONE, ALTERNATE,
	     bridge_enable);

#undef  ALTERNATE
#define ALTERNATE       show_bridge_test
