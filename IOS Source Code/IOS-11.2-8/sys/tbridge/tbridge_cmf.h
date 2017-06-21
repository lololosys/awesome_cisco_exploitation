/* $Id: tbridge_cmf.h,v 3.1.8.1 1996/05/04 05:08:52 wilber Exp $
 * $Source: /release/112/cvs/Xsys/tbridge/tbridge_cmf.h,v $
 *------------------------------------------------------------------
 * tbridge/tbridge_cmf.h
 *
 * April 1996, Tony Speakman
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Constrained Multicast Flooding (CMF)
 *------------------------------------------------------------------
 * $Log: tbridge_cmf.h,v $
 * Revision 3.1.8.1  1996/05/04  05:08:52  wilber
 * Branch: California_branch
 *
 * Integrated Routing and Bridging
 * - New file(tbridge_bvi.c) for Bridge-group Virtual Interface
 * functions.
 * - Obsolete tbridge_crb_inline.h with tbridge_irb_inline.h.
 * - Bridge/route decision(tbridge_irb_inline.h).
 * - Learn routable packets(DLLI_IRBPKT) in bridge table.
 * - Move locate_bte_inline() and tbridge_get_arpa_type_inline() to
 *   tbridge_common_inline.h.
 * - Rip out MTR transparent bridging support.
 *
 * Constrained Multicast Flooding
 * - Add the tbridge monitor.
 * - Add the tbridge registry.
 * - Add the entire CMF subsystem.
 * - Convert tbridge flood array to a linked list of tbifd's.
 * - Add the call outs to CMF from the LES, RSP, and PAS forwarding
 *   functions.
 *
 * Revision 3.1  1996/04/21  23:06:47  speakman
 * Placeholders for the CMF subsystem and eventual partition of the
 * bridging registries.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Some defines repeated from ip/ip.h to keep from having to
 * include ip/ip.h in the tbridge code.
 */

#define TBRIDGE_IP_MCAST_RSVD(ipaddr)	(0xe0000000 == (ipaddr & ~0x000000ff))

#define TBRIDGE_ALLSYSTEMS	0xe0000001	/* All systems */
#define TBRIDGE_ALLROUTERS	0xe0000002	/* All routers */
#define TBRIDGE_DVMRPROUTERS	0xe0000004	/* All DVMRP routers */
#define TBRIDGE_ALLSPFROUTERS	0xe0000005	/* All OSPF routers */

#define TBRIDGE_IP_PROTOCOL_OFFSET	9
#define TBRIDGE_IP_DSTADDR_OFFSET	16
#define TBRIDGE_IP_PROTOCOL_IGMP	2
#define TBRIDGE_IP_PROTOCOL_OSPF	89
