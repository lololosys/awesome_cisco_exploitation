/* $Id: cfg_bridge.h,v 3.2.62.2 1996/05/09 14:46:32 rbadri Exp $
 * $Source: /release/112/cvs/Xsys/srt/cfg_bridge.h,v $
 *------------------------------------------------------------------
 * C F G _ B R I D G E . H
 *
 * Transparent bridging support
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_bridge.h,v $
 * Revision 3.2.62.2  1996/05/09  14:46:32  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.2.62.1  1996/05/04  01:49:00  wilber
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
 * Revision 3.2.90.1  1996/04/27  07:26:15  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.2.80.2  1996/04/16  18:53:38  cyoung
 * Make the bridge management-domain command unsupported/hidden.
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 3.2.80.1  1996/04/08  02:09:42  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.2.68.1  1996/03/25  02:21:11  cakyol
 * sync LE_Syn_Cal_V111_1_3_branch to LE_Cal_V111_1_0_3_branch
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.2.40.2  1996/03/07  02:30:41  cyoung
 * nv_gen supported added for managent-domain keyword.
 * action routines fleshed out.
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 3.2.40.1  1996/03/05  01:52:23  cyoung
 * Add management-domain and vlan-id to bridge command.
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 3.2  1995/11/17  18:53:00  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:30:28  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.5  1995/10/16  23:01:12  pitargue
 * CSCdi42250:  fix CSCdi40658, tbridge access lists
 * code review fix
 *
 * Revision 2.4  1995/10/04  22:46:55  pitargue
 * CSCdi40658:  Forward mac addr. filter failed on dec & ieee bridge
 * (eip-aip-eip)
 * atm needs a map between a mac addr and a vc number.
 *
 * Revision 2.3  1995/08/18  23:57:05  speakman
 * CSCdi39005:  CRB commands collide
 * Convert 'bridge <group> route <protocol>' to use PROTONAME macro.
 *
 * Revision 2.2  1995/07/09  21:07:19  speakman
 * CSCdi36848:  Configuration of CRB proves unpopular
 * Introduce explicit, bridge-group specific route/bridge indicators
 * for CRB, and the commands to configure them.  Provide some smarts
 * to generate the appropriate 'bridge route' commands when CRB is
 * enabled in the presence of existing bridge groups.
 *
 * Revision 2.1  1995/07/04  01:57:55  tli
 * CSCdi36508: Rationalize parser file placement, part I
 * Rationalize cls dev dlsw dspu filesys irix lat les lex llc2 parser pgen
 * pt servers smf smrp snmp src-36-c1000 src-4k-c4000 src-68-c3000
 * src-68-c4000 src-c4000 src-rsp srt tcl ts ui
 *
 * Revision 2.1  1995/06/07  22:01:59  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/***************************************************************************
 * no bridge <group>
 */
EOLS	(bridge_no_eol, bridge_command, NO_BRIDGE);
TESTVAR	(bridge_no_test, bridge_no_eol, no_alt,
	 NONE, NONE, NONE,
	 sense, FALSE);

/***************************************************************************
 *	bridge <br-group> circuit-group <circ-group> pause <milliseconds>
 *	bridge <br-group> circuit-group <circ-group> source-based
 *
 * OBJ(int,1) = br-group
 * OBJ(int,2) = circ-group
 * OBJ(int,3) = milliseconds
 */

EOLS	(bridge_ccg_src_eol, bridge_command, BRIDGE_CCG_SRC);
KEYWORD	(bridge_ccg_src, bridge_ccg_src_eol, NONE, "source-based",
	 "Circuit-group source-based", PRIV_CONF);
PARAMS_KEYONLY(bridge_ccg_pause, bridge_ccg_src, "pause",
	       OBJ(int,3), 0, 10000,
	       bridge_command, BRIDGE_CCG_PAUSE,
	       "Circuit-group pause", "Circuit group pause interval",
	       PRIV_CONF); 
NUMBER  (bridge_ccg_number, bridge_ccg_pause, NONE, OBJ(int,2), 1, 9, 
	 "Circuit group number");
NVGENS	(bridge_ccg_nvgen, bridge_ccg_number, bridge_command, BRIDGE_CCG);
KEYWORD	(bridge_ccg, bridge_ccg_nvgen, bridge_no_test, "circuit-group", 
	 "Circuit-group", PRIV_CONF);

/***************************************************************************
 *	bridge <group> priority <priority>
 *
 * OBJ(int,1) = group
 * OBJ(int,2) = <priority>
 *
 */

PARAMS_KEYONLY	(bridge_priority, bridge_ccg, "priority", 
	 OBJ(int,2), 0, 65535,
	 bridge_command, BRIDGE_PRIORITY,
	 "Set bridge priority",
	 "Priority (low priority more likely to be root)", PRIV_CONF);

/***************************************************************************
 *	bridge <group> multicast-source
 *
 * OBJ(int,1) = group
 */

EOLS	(bridge_multicast1, bridge_command, BRIDGE_MCASTSRC);
KEYWORD	(bridge_multicast, bridge_multicast1, bridge_priority,
	 "multicast-source",
	 "Forward datagrams with multicast source addresses", PRIV_CONF);

/***************************************************************************
 *	bridge <group> max-age <seconds>
 *
 * OBJ(int,1) = group
 * OBJ(int,2) = <seconds>
 */

PARAMS_KEYONLY  (bridge_maxage, bridge_multicast, "max-age",
                 OBJ(int,2), 6, 200, bridge_command, BRIDGE_MAXAGE,
                 "Maximum allowed message age of received Hello BPDUs",
                 "Seconds", PRIV_CONF);


/***************************************************************************
 *	bridge <group> lat-service-filtering
 *
 * OBJ(int,1) = group
 */

EOLS	(bridge_latservfilter1, bridge_command, BRIDGE_LATFILTER);
KEYWORD	(bridge_latservfilter, bridge_latservfilter1, bridge_maxage,
	 "lat-service-filtering", "Perform LAT service filtering", PRIV_CONF);

/***************************************************************************
 * bridge <group> hello-time <seconds>
 * no bridge <group> hello-time <seconds>
 *
 * OBJ(int,1) = group
 * OBJ(int,2) = <seconds>
 *
 */

PARAMS_KEYONLY (bridge_hello, bridge_latservfilter,
		"hello-time", OBJ(int,2), 1, 10,
		bridge_command, BRIDGE_HELLOTIME,
		"Set interval between HELLOs", 
		"Seconds", PRIV_CONF);

/***************************************************************************
 *	bridge <group> forward-time <seconds>
 *	no bridge <group> forward-time [<seconds>]
 *
 * OBJ(int,1) = group
 * OBJ(int,2) = <seconds>
 *
 */

PARAMS_KEYONLY (bridge_forward, bridge_hello, 
		"forward-time", OBJ(int,2), 4, 200,
		bridge_command, BRIDGE_FORWARDTIME,
		"Set forwarding delay time",
		"Seconds", PRIV_CONF);

/***************************************************************************
 *	bridge <group> aging-time <seconds>
 *	no bridge <group> aging-time [<seconds>]
 *
 * OBJ(int,1) = group
 * OBJ(int,2) = <seconds>
 *
 */

PARAMS_KEYONLY (bridge_aging, bridge_forward, 
		"aging-time", OBJ(int,2), 10, 1000000,
		bridge_command, BRIDGE_AGINGTIME,
		"Set forwarding entry aging time",
		"Seconds", PRIV_CONF);

/***************************************************************************
 *	bridge <group> domain <domain-number>
 *	no bridge <group> domain [<domain-number>]
 *
 * OBJ(int,1) = group
 * OBJ(int,2) = <domain-number>
 *
 */

PARAMS_KEYONLY (bridge_domain, bridge_aging,
		"domain", OBJ(int,2), 1, 10,
		bridge_command, BRIDGE_DOMAIN,
		"Establish multiple bridging domains",
		"Domain number", PRIV_CONF);

/***************************************************************************
 * bridge <group> address <enet-addr> { forward | discard } [<interface>]
 * no bridge <group> address <enet-addr> [{ forward | discard } [<interface>]]
 *
 *
 * OBJ(int,1) = group
 * (*OBJ(hwaddr,1)) = Ethernet addr
 * OBJ(int,2) = BRIDGE_FORWARD or BRIDGE_DISCARD
 * OBJ(idb,1) = <interface>
 * OBJ(int,3) = DLCI of Frame Relay, or VC number for atm
 */
EOLS	(bridge_addr_eol, bridge_command, BRIDGE_ADDRESS);

NUMBER   (bridge_addr8, bridge_addr_eol, no_alt,
	  OBJ(int,3), 1, ATM_MAX_VC_DEFAULT,
	  "ATM VC Number");
	  
INTERFACE(bridge_addr7, bridge_addr8, bridge_addr_eol,
	  OBJ(idb,1), IFTYPE_ATM);

NUMBER  (bridge_addr6, bridge_addr_eol, bridge_addr_eol,
	 OBJ(int,3), 16, 1007,
	 "Frame Relay DLCI Range 16 - 1007");

INTERFACE(bridge_addr5, bridge_addr6, bridge_addr7,
	  OBJ(idb,1), (IFTYPE_ANYSUB & ~IFTYPE_ATM));

KEYWORD_ID(bridge_addr4, bridge_addr5, no_alt,
	   OBJ(int,2), BRIDGE_DISCARD,
	   "discard", "Discard datagrams from/to this address", PRIV_CONF);

KEYWORD_ID(bridge_addr2, bridge_addr5, bridge_addr4,
	   OBJ(int,2), BRIDGE_FORWARD,
	   "forward", "Forward datagrams from/to this address", PRIV_CONF);

NOPREFIX (bridge_addr1a, bridge_addr2, bridge_addr_eol);
ENADDR	(bridge_addr1, bridge_addr1a, no_alt,
	 OBJ(hwaddr,1), "Ethernet address");
NVGENS	(bridge_addrnvgen, bridge_addr1, bridge_command, BRIDGE_ADDRESS);
KEYWORD	(bridge_addr, bridge_addrnvgen, bridge_domain,
	 "address", "Block or forward a particular Ethernet address",
	 PRIV_CONF);

/***************************************************************************
 *	bridge <group> acquire
 *
 * OBJ(int,1) = group
 */

EOLS	(bridge_acquire1, bridge_command, BRIDGE_ACQUIRE);
KEYWORD	(bridge_acquire, bridge_acquire1, bridge_addr,
	 "acquire", "Dynamically learn new, unconfigured stations", PRIV_CONF);

/***************************************************************************
 * bridge <group> bridge { apollo | appletalk | clns | decnet |
 *			  ip | ipx | vines | xns }
 * 
 *
 * OBJ(int,1) = group
 * OBJ(int,2) = LINK_APOLLO or LINK_APPLETALK or
 *		LINK_CLNS or LINK_DECNET or
 *		LINK_IP or LINK_NOVELL or
 *		LINK_VINES or LINK_XNS
 */

EOLS	(bridge_bridge_eol, bridge_command, BRIDGE_BRIDGE);

PROTONAME(bridge_bridge_protocol, bridge_bridge_eol, no_alt,
	  OBJ(int,2), PMATCH_APOLLO PMATCH_APPLETALK PMATCH_CLNS
	  PMATCH_DECNET PMATCH_IP PMATCH_NOVELL PMATCH_VINES
	  PMATCH_XNS);
NVGENS	(bridge_bridge_nvgen, bridge_bridge_protocol,
         bridge_command, BRIDGE_BRIDGE);
KEYWORD	(bridge_bridge, bridge_bridge_nvgen, bridge_acquire,
	 "bridge", "Specify a protocol to be bridged in this bridge group",
	 PRIV_CONF);

/***************************************************************************
 * bridge <group> route { apollo | appletalk | clns | decnet |
 *			  ip | ipx | vines | xns }
 * 
 *
 * OBJ(int,1) = group
 * OBJ(int,2) = LINK_APOLLO or LINK_APPLETALK or
 *		LINK_CLNS or LINK_DECNET or
 *		LINK_IP or LINK_NOVELL or
 *		LINK_VINES or LINK_XNS
 */

EOLS	(bridge_route_eol, bridge_command, BRIDGE_ROUTE);

PROTONAME(bridge_route_protocol, bridge_route_eol, no_alt,
	  OBJ(int,2), PMATCH_APOLLO PMATCH_APPLETALK PMATCH_CLNS
	  PMATCH_DECNET PMATCH_IP PMATCH_NOVELL PMATCH_VINES
	  PMATCH_XNS);
NVGENS	(bridge_route_nvgen, bridge_route_protocol,
         bridge_command, BRIDGE_ROUTE);
KEYWORD	(bridge_route, bridge_route_nvgen, bridge_bridge,
	 "route", "Specify a protocol to be routed in this bridge group",
	 PRIV_CONF);

/***************************************************************************
 * bridge <group> management-domain <domain-name> vlan-id <vlan-id>
 * 
 *
 * OBJ(int,1) = group
 * OBJ(int,2) = LINK_APOLLO or LINK_APPLETALK or
 *		LINK_CLNS or LINK_DECNET or
 *		LINK_IP or LINK_NOVELL or
 *		LINK_VINES or LINK_XNS
 */

EOLS	(bridge_vlandomain_eol, bridge_command, BRIDGE_VLANDOMAIN);
NUMBER  (bridge_vlan_id, bridge_vlandomain_eol, no_alt, OBJ(int, 2),
	 1, 1005, "The VLAN ID associated with the bridge group");
KEYWORD (bridge_vlan, bridge_vlan_id, no_alt, "vlan-id", 
	 "Specify the VLAN ID associated with this bridge group",
	 PRIV_CONF); 
STRING  (bridge_vlandomain_name, bridge_vlan, no_alt, 
	 OBJ(string,1), "Name of the management domain");
NVGENS	(bridge_vlandomain_nvgen, bridge_vlandomain_name,
         bridge_command, BRIDGE_VLANDOMAIN);
KEYWORD	(bridge_vlandomain, bridge_vlandomain_nvgen, bridge_route,
	 "management-domain", 
	 "Specify a management domain for this bridge group",
	 PRIV_UNSUPPORTED);

/***************************************************************************
 * bridge <group> protocol { dec | ieee | ibm}
 * 
 *
 * OBJ(int,1) = group
 * OBJ(int,2) = BRIDGE_PROTO_DEC or BRIDGE_PROTO_IEEE or BRIDGE_PROTO_IBM
 */

EOLS	(bridge_protocol_eol, bridge_command, BRIDGE_PROTOCOL);

/* accept ast for ibm (request from customer) */
KEYWORD_ID(bridge_protocol_ast, bridge_protocol_eol, no_alt,
	   OBJ(int,2), SPAN_PROTO_IBM,
	   "ast", "IBM protocol", PRIV_CONF|PRIV_HIDDEN|PRIV_NONVGEN);

KEYWORD_ID(bridge_protocol_ibm, bridge_protocol_eol, bridge_protocol_ast,
	   OBJ(int,2), SPAN_PROTO_IBM,
	   "ibm", "IBM protocol", PRIV_CONF);
KEYWORD_ID(bridge_protocol_ieee, bridge_protocol_eol, bridge_protocol_ibm,
	   OBJ(int,2), SPAN_PROTO_IEEE,
	   "ieee", "IEEE 802.1 protocol", PRIV_CONF);
KEYWORD_ID(bridge_protocol_dec, bridge_protocol_eol, bridge_protocol_ieee,
	   OBJ(int,2), SPAN_PROTO_DEC,
	   "dec", "DEC protocol", PRIV_CONF);
NOPREFIX(bridge_protocol_no, bridge_protocol_dec, bridge_protocol_eol);
KEYWORD	(bridge_protocol, bridge_protocol_no, bridge_vlandomain,
	 "protocol", "Specify spanning tree protocol", PRIV_CONF);

/***************************************************************************
 * bridge <group-number> ...
 *
 */

ASSERT	(bridge_number_test, bridge_protocol, NONE,
	 !csb->nvgen || spanarray[GETOBJ(int,1)]);

NUMBER_NV(bridge_number, bridge_number_test, NONE,
	  OBJ(int,1), 1, SPAN_MAXINDEX - 1,
	  "Bridge Group number for Bridging.");

/***************************************************************************
 * bridge crb/irb
 *
 */

EOLS  (bridge_irb_eol, bridge_command, BRIDGE_IRB);
KEYWORD (bridge_irb, bridge_irb_eol, bridge_number,
	 "irb", "Integrated routing and bridging", PRIV_CONF);

EOLS  (bridge_crb_eol, bridge_command, BRIDGE_CRB);
KEYWORD (bridge_crb, bridge_crb_eol, bridge_irb,
	 "crb", "Concurrent routing and bridging", PRIV_CONF);

/***************************************************************************
 * bridge cmf <cgmp>
 *
 */

LINK_TRANS(cfg_bridge_cmf_return_here, NONE);
LINK_TRANS(cfg_bridge_cmf_extend_here, bridge_crb);

KEYWORD (config_bridge, cfg_bridge_cmf_extend_here, NONE,
         "bridge", "Bridge Group.", PRIV_CONF);

TEST_BOOLEAN(config_bridge_test, config_bridge, NONE,
             ALTERNATE, bridge_enable);

#undef  ALTERNATE
#define ALTERNATE       config_bridge_test
