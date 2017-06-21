/* $Id: ip_media.h,v 3.5.12.2 1996/04/15 21:16:00 bcole Exp $
 * $Source: /release/112/cvs/Xsys/ip/ip_media.h,v $
 *------------------------------------------------------------------
 * ip_media.h -- media specific prototypes and definitions for IP
 *
 * 29-April-1993, Kirk Lougheed
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ip_media.h,v $
 * Revision 3.5.12.2  1996/04/15  21:16:00  bcole
 * CSCdi54649:  IP Host Modularity, IP Automatic Stub Routing feature
 * commit
 * Branch: California_branch
 *
 * Revision 3.5.12.1  1996/04/07  01:15:27  speakman
 * CSCdi48117:  ARPs for local non-bridge-group IP addresses dont get
 * bridged
 * Branch: California_branch
 * Modify the ARP code to bridge ARP's for any IP address not associated
 * with an interface in the same bridge group.  This permits CRB to be
 * used in a looped-back configuration to provide de facto IRB
 * functionality.
 *
 * Revision 3.5  1996/02/13  19:03:53  rja
 * CSCdi43596:  ARP throttling not working
 * Replumbed IPv4 ARP Request software so that the ARP throttling
 * sanity checks always occur and aren't accidentally bypassed by
 * some callers.  Removed one now dead function in ip_arp.c
 *
 * Revision 3.4  1995/11/30  22:58:19  bcole
 * CSCdi43875: HSRP ARP table entries contain bia instead of virtual MAC
 * address
 *
 * During ARP table refresh, don't revise dynamic IP aliases using the
 * interface's BIA.  Be a little more intelligent about which interfaces
 * to send gratuitous ARPs out.
 *
 * Revision 3.3  1995/11/19  06:05:59  fox
 * CSCdi43820:  routes thrash on ipcp_up()
 * Don't send gratuitous ARP on the interface the peer is on.
 * Rename ip_send_gratuitous_arps() as ip_arp_send_gratuitous_arps().
 * Remove route_remove_neighbor() as it did the same thing as
 * iprouting_delete_conn_route().
 *
 * Revision 3.2  1995/11/17  09:33:40  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:55:34  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/10/23  20:57:20  fox
 * CSCdi42602:  Neighbor routes are lost if routing table is cleared
 * Move neighbor address support to the WAN code.
 * Call the WAN code if an interface's routes are being modified.
 *
 * Revision 2.2  1995/08/24  00:47:18  gchristy
 * CSCdi37313:  IP mobility crashes router
 * - punt if ip_arp_incomplete() fails
 *
 * Revision 2.1  1995/06/07  20:57:27  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "../if/arp.h"

/***********************************************************************
 *
 *                      Externs and Prototypes
 *
 ***********************************************************************/

/*
 * ip_media.c
 */
extern void ip_macaddrs_update(idbtype *);
extern uchar *ip_transit_bridging(ipaddrtype, idbtype *);

/*
 * ip_arp.c
 */
extern boolean ip_arp_forus(ipaddrtype, ipaddrtype, ushort, spantype *);
extern idbtype *ip_arp_route(ipaddrtype, boolean);
extern arptype *ip_arp_table_lookup(idbtype *, ipaddrtype);
extern arptype *ip_arp_lookup_and_resolve(idbtype *, ipaddrtype);
extern boolean ip_arp_assist(ipaddrtype, ipaddrtype, idbtype *, uint, int,
			     uchar *);
extern void ip_arp_input(paktype *);
extern void ip_arp_unsolicited(idbtype *, ipaddrtype, uchar *, uchar *);
extern void ip_arp_announcement(void);
extern arptype *ip_arp_table_add(idbtype *, ipaddrtype, uchar *, int, int);
extern void ip_arp_invalidate(ipaddrtype);
extern void ip_arp_glean(paktype *);
extern void ip_arp_resolve(idbtype *, ipaddrtype);
extern void ip_arp_refresh(arptype *);
extern boolean ip_arp_fscache_invalidate(arptype *);
extern void ip_arp_add_interface(idbtype *, ipaddrtype);
extern void ip_arp_reload(idbtype *);
extern boolean ip_arp_incomplete(idbtype *, ipaddrtype);
extern void ip_arp_send_gratuitous_arps(idbtype *, ipaddrtype);

extern boolean ip_gratuitous_arps;	/* generate gratuitous arps */
