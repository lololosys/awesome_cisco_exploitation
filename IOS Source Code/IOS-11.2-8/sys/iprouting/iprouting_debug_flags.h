/* $Id: iprouting_debug_flags.h,v 3.3.10.2 1996/06/27 04:41:13 mikel Exp $
 * $Source: /release/112/cvs/Xsys/iprouting/iprouting_debug_flags.h,v $
 *------------------------------------------------------------------
 * iprouting_debug_flags.h -- Debugging flag declarations for IP routing
 *
 * May 1993, John Bashinski
 *
 * Copyright (c) 1993-1996 by Cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: iprouting_debug_flags.h,v $
 * Revision 3.3.10.2  1996/06/27  04:41:13  mikel
 * CSCdi37749:  <debug ip eigrp> help wrong
 * Branch: California_branch
 * clean up ip-eigrp AS debugging
 *
 * Revision 3.3.10.1  1996/04/05  03:46:37  rchandra
 * CSCdi53755:  BGP: flap dampening settings not flexible enough
 * Branch: California_branch
 * - add route-map support to set flap dampening parameters
 *
 * Revision 3.3  1996/02/20  21:18:17  bcole
 * CSCdi49357:  NHRP: Problems with debug ip nhrp, cache flushing
 *
 * Repair debug command backwards compatibility.  Fix ordering problem
 * when flushing cache during deconfiguration.
 *
 * Revision 3.2  1995/11/17  17:34:48  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:08:37  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/10/19  08:06:00  tli
 * CSCdi41042:  IP policy: local policy fails, debugging obtuse
 * Local policy packets weren't being checksummed correctly.
 * Give debugging its own command.
 *
 * Revision 2.2  1995/09/05  22:08:55  myeung
 * CSCdi39807:  OSPF need access-list debugging
 * - Add access-list debugging for SPF calculation and flooding
 * - Add separate debugging for intra-area, inter-area and external SPF
 *
 * Revision 2.1  1995/06/07  21:07:48  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * We get included (multiple times) from IP routing code to define
 * debugging flags. For this, we use some macros that get defined
 * by debug_macros.h. What definitions the macros get depends on
 * which magic flags have been set by the file that includes us.
 */
#include "../ui/debug_macros.h"

/* Now define the actual flags and the array that points to them */
DEBUG_ARRDECL(iprouting_debug_arr)

DEBUG_FLAG(bgp_debug, DEBUG_BGP, "BGP")
DEBUG_FLAG(bgpevent_debug, DEBUG_BGPEVENT, "BGP events")
DEBUG_FLAG(bgpkeep_debug, DEBUG_BGPKEEP, "BGP keepalives")
DEBUG_FLAG(bgpupdate_debug, DEBUG_BGPUPDATE, "BGP updates")
DEBUG_FLAG(bgpdamp_debug, DEBUG_BGPDAMP, "BGP dampening")
DEBUG_FLAG(egp_debug, DEBUG_EGP, "EGP")
DEBUG_FLAG(egpevent_debug, DEBUG_EGPEVENT, "EGP event")
DEBUG_FLAG(igrp_debug, DEBUG_IGRP, "IGRP protocol")
DEBUG_FLAG(igrp_debug_events, DEBUG_IGRP_EVENTS, "IGRP event")
DEBUG_FLAG(ipcache_debug, DEBUG_IPCACHE, "IP cache")
DEBUG_FLAG(ipigrp2_debug_as_target, DEBUG_IPIGRP2_AS_TARGET,
	   "IP-EIGRP AS Target Events")
DEBUG_FLAG(ipigrp2_debug_nei_target, DEBUG_IPIGRP2_NEI_TARGET,
	   "IP-EIGRP Neighbor Target Events")
DEBUG_FLAG(ipigrp2_debug, DEBUG_IPIGRP2, "IP-EIGRP Route Events")
DEBUG_FLAG(ipigrp2_debug_not, DEBUG_IPIGRP2_NOTIFICATIONS,
	   "IP-EIGRP Event notification")
DEBUG_FLAG(ipigrp2_debug_sum, DEBUG_IPIGRP2_SUMMARY,
	   "IP-EIGRP Summary route processing")
DEBUG_FLAG(ip_mobile_debug, DEBUG_IP_MOBILE, "IP mobility protocols")
DEBUG_FLAG(ospf_debug, DEBUG_OSPF, "OSPF")
DEBUG_FLAG(ospf_debug_packet, DEBUG_OSPF_PACKET, "OSPF packet")
DEBUG_FLAG(ospf_debug_retrans, DEBUG_OSPF_RETRANS,
	   "OSPF retransmission events")
DEBUG_FLAG(ospf_debug_adj, DEBUG_OSPF_ADJ, "OSPF adjacency events")
DEBUG_FLAG(ospf_debug_events, DEBUG_OSPF_EVENTS, "OSPF events")
DEBUG_FLAG(ospf_debug_flood, DEBUG_OSPF_FLOOD, "OSPF flooding")
DEBUG_FLAG(ospf_debug_spf, DEBUG_OSPF_SPF, "OSPF spf events")
DEBUG_FLAG(ospf_debug_spf_intra, DEBUG_OSPF_SPF_INTRA, "OSPF spf intra events")
DEBUG_FLAG(ospf_debug_spf_inter, DEBUG_OSPF_SPF_INTER, "OSPF spf inter events")
DEBUG_FLAG(ospf_debug_spf_ext, DEBUG_OSPF_SPF_EXT, "OSPF spf external events")
DEBUG_FLAG(ospf_debug_spf_monitor, DEBUG_OSPF_MONITOR, "OSPF spf monitoring")
DEBUG_FLAG(ospf_debug_lsa_gen, DEBUG_OSPF_LSA_GEN,
	   "OSPF summary lsa generation")
DEBUG_FLAG(ospf_debug_tree, DEBUG_OSPF_TREE, "OSPF database events")
DEBUG_FLAG(policy_debug, DEBUG_POLICY, "Policy routing") 
DEBUG_FLAG(rip_debug, DEBUG_RIP, "RIP protocol")
DEBUG_FLAG(rip_debug_events, DEBUG_RIP_EVENTS, "RIP event")
DEBUG_FLAG(route_debug, DEBUG_ROUTING, "IP routing")
DEBUG_FLAG(standby_debug, DEBUG_STANDBY, "Hot standby protocol")

DEBUG_ARRDONE
