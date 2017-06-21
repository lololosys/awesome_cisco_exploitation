/* $Id: cgmp.c,v 3.1.4.4 1996/09/10 05:31:34 lwei Exp $
 * $Source: /release/112/cvs/Xsys/ipmulticast/cgmp.c,v $
 *------------------------------------------------------------------
 * cgmp.c - Cisco Group Management Protocol (CGMP)
 *
 * January, 1996
 *
 * Copyright (c) 1996-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 * CGMP is a cisco proprietary protocol that allows an IOS system to program
 * the bridge forwarding tables in a Catalyst 5000. This is a short-term
 * solution until the Catalyst 5000 hardware can distinguish between IP
 * multicast data packets and IGMP Report packets.
 *
 *------------------------------------------------------------------
 * $Log: cgmp.c,v $
 * Revision 3.1.4.4  1996/09/10  05:31:34  lwei
 * CSCdi68465:  RP information is too difficult to change in last-hop
 *              routers
 * Branch: California_branch
 *
 * Revision 3.1.4.3  1996/07/23  18:46:57  ppearce
 * CSCdi63747:  Move srb_multiring out of SRB subblock
 * Branch: California_branch
 *   (1) srb_multiring doesn't really belong exclusively to SRB
 *         move srb_multiring out of SRB subblock and into swidb
 *         provide srb_multiring accessor routines
 *   (2) determine_rif() should take swidb as argument rather than hwidb
 *
 * Revision 3.1.4.2  1996/04/20  01:50:55  dino
 * CSCdi54104:  CGMP must support source-only systems in switched Cat5000
 *              network
 * Branch: California_branch
 *
 * Revision 3.1.4.1  1996/04/03  23:18:58  dino
 * CSCdi51598:  Problem of interoperability with switches
 * Branch: California_branch
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include <master.h>
#include <stdarg.h>
#include <ciscolib.h>
#include <media_registry.h>
#include <globs.h>
#include <logger.h>
#include <interface_private.h>
#include <packet.h>
#include <config.h>
#include <ieee.h>
#include <mgd_timers.h>
#include "../util/radix.h"
#include "../if/network.h"
#include "../if/rif_inline.h"
#include "../if/ether.h"
#include "../if/static_map.h"
#include "../ip/ip.h"
#include "../ip/ipaccess.h"
#include "igmp.h"
#include "mroute.h"
#include "cgmp.h"
#include "pim.h"
#include "ipmulticast_debug.h"


/*
 * cgmp_send_self
 *
 * Send CGMP Join/Leave for this system. This indicates to the switches where 
 * router ports are and when they go away. This function also sends joins
 * for groups the router is a member of.
 */
void cgmp_send_self (uchar type, idbtype *idb, ipaddrtype group)
{

    paktype            *pak;
    cgmptype           *cgmp;
    ulong              length;
    ethermultiaddrtype group_addr = ETHER_IP_MCAST;
        
    /*
     * Build packet.
     */
    length = sizeof(cgmptype) + sizeof(ieee_addrs);
    pak = getbuffer(length);
    if (!pak) return;

    cgmp = (cgmptype *) pak->network_start;
    cgmp->cgmp_version = CGMP_VERSION1;
    cgmp->cgmp_type = type;
    cgmp->cgmp_reserved[0] = 0;
    cgmp->cgmp_reserved[1] = 0;
    cgmp->cgmp_count = 1;

    /*
     * Assume GDA is 0000.0000.0000. If group address passed, then construct
     * MAC address.
     */
    ieee_zero(cgmp->cgmp_mac_addrs->daddr);
    if (group) {
	group_addr.bottom23 = group;
	ieee_copy(group_addr.addr, cgmp->cgmp_mac_addrs->daddr);
    }
    ieee_copy(idb->hwptr->hardware, cgmp->cgmp_mac_addrs->saddr);

    CGMP_BUGINF("\nCGMP: Sending self %s on %s", 
		(type == CGMP_JOIN_TYPE) ? "Join" : "Leave",
		idb_get_namestring(idb));
    CGMP_BUGINF("\n      GDA %e, USA %e", cgmp->cgmp_mac_addrs->daddr,
		cgmp->cgmp_mac_addrs->saddr);

    /*
     * Setup packet, encapsulate and send out.
     */
    pak->linktype = LINK_CGMP;
    pak->if_output = idb;
    if (pak_encap(idb->hwptr, pak, length, 0)) {
	datagram_out(pak);
    } else {
	CGMP_BUGINF("\nCGMP: Encapsulation failure");
	datagram_done(pak);
    }
}

/*
 * cgmp_send_join
 *
 * Build and send CGMP Join. Extract MAC addresses from IGMP Report.
 */
void cgmp_send_join (idbtype *idb, paktype *igmp, igmptype *igmp_hdr,
		     ipaddrtype source, ipaddrtype group)
{

    paktype    *pak;
    cgmptype   *cgmp;
    hwaddrtype srcmac;
    ethermultiaddrtype dstmac = ETHER_IP_MCAST;
    ulong      length;

    /*
     * Print appropriate debug to avoid using other debug commands.
     */
    if (igmp_hdr->type == IGMP_REPORT_TYPE || 
	igmp_hdr->type == IGMP_NEW_REPORT_TYPE) {
	CGMP_BUGINF("\nCGMP: Received IGMP Report on %s",
		    idb_get_namestring(idb));
	CGMP_BUGINF("\n      from %i for %i", source, group);
    }

    if (!idb->hwptr->extract_snpa) {
	CGMP_BUGINF("\nCGMP: Can't extract MAC addresses from IGMP Report");
	return;
    }

    /*
     * Pull MAC addresses out of IGMP Report.
     */
    (*idb->hwptr->extract_snpa)(igmp, &srcmac, SOURCE);
    dstmac.bottom23 = group;

    /*
     * Build CGMP Join packet.
     */
    length = sizeof(cgmptype) + sizeof(ieee_addrs);
    pak = getbuffer(length);
    if (!pak) return;

    cgmp = (cgmptype *) pak->network_start;
    ieee_copy(dstmac.addr, cgmp->cgmp_mac_addrs->daddr);
    ieee_copy(srcmac.addr, cgmp->cgmp_mac_addrs->saddr);

    CGMP_BUGINF("\nCGMP: Sending Join on %s", idb_get_namestring(idb));
    CGMP_BUGINF("\n      GDA %e, USA %e", dstmac.addr, srcmac.addr);

    cgmp_send_join_packet(pak, idb, 1);
}

/*
 * cgmp_send_proxy
 *
 * Send CGMP proxy Join for non-cisco/non-CGMP speaking routers. This allows
 * the Catalyst 5000 to forward IP multicast packets to router ports that
 * lead to non-cisco multicast routers. These proxy will be sent at an
 * interval equal to the router's DVMRP Report interval.
 */
void cgmp_send_proxy (paktype *pak, igmptype *igmp_hdr, ipaddrtype router, 
		      idbtype *idb)
{

    /*
     * This DVMRP Report may have come over a tunnel. Switches don't care
     * about this router.
     */
    if (!is_ieee802(idb->hwptr)) return;

    if (igmp_hdr->type == IGMP_DVMRP_TYPE) {
	CGMP_BUGINF("\nCGMP: Received DVMRP Report on %s from %i",
		    idb_get_namestring(idb), router);
    }

    /*
     * If we find a PIM router, we assume it is a cisco that can do CGMP.
     * Don't proxy for this guy.
     */
    if (pim_find_nbr(router, idb)) return;

    CGMP_BUGINF("\nCGMP: Do proxy for %i");

    /*
     * Modify destination MAC address. Set it to 0000.0000.0000 so the
     * switches know that this is a "all router ports" join.
     */
    ieee_zero(pak->addr_start);

    /*
     * Send Join.
     */
    cgmp_send_join(idb, pak, igmp_hdr, router, 0);
}

/*
 * cgmp_send_leave
 *
 * Send CGMP leave message for USA 0000.0000.0000 to tell switches that
 * they should delete their bridge forwarding table entry for the group.
 */
void cgmp_send_leave (ipaddrtype group, idbtype *idb, paktype *leave_pak)
{

    paktype            *pak;
    cgmptype           *cgmp;  
    ethermultiaddrtype dstmac = ETHER_IP_MCAST;
    hwaddrtype         srcmac;
    ulong              length;

    dstmac.bottom23 = group;

    /*
     * If a packet pointer is passed in, we are sending a leave for a
     * particular MAC address rather than for all MAC addresses. This
     * is for the "ip cgmp process-leaves" support".
     */
    if (leave_pak) {
	(*idb->hwptr->extract_snpa)(leave_pak, &srcmac, SOURCE);
    } else {
	ieee_zero(srcmac.addr);
    }

    /*
     * Build CGMP Leave packet.
     */
    length = sizeof(cgmptype) + sizeof(ieee_addrs);
    pak = getbuffer(length);
    if (!pak) return;

    cgmp = (cgmptype *) pak->network_start;
    cgmp->cgmp_version = CGMP_VERSION1;
    cgmp->cgmp_type = CGMP_LEAVE_TYPE;
    cgmp->cgmp_reserved[0] = 0;
    cgmp->cgmp_reserved[1] = 0;
    cgmp->cgmp_count = 1;

    ieee_copy(dstmac.addr, cgmp->cgmp_mac_addrs->daddr);
    ieee_copy(srcmac.addr, cgmp->cgmp_mac_addrs->saddr);

    CGMP_BUGINF("\nCGMP: Sending Leave on %s", idb_get_namestring(idb));
    CGMP_BUGINF("\n      GDA %e, USA %e", dstmac.addr, srcmac.addr);

    /*
     * Setup packet, encapsulate and send out.
     */
    pak->linktype = LINK_CGMP;
    pak->if_output = idb;
    if (pak_encap(idb->hwptr, pak, length, 0)) {
	datagram_out(pak);
    } else {
	CGMP_BUGINF("\nCGMP: Encapsulation failure");
	datagram_done(pak);
    }
}

/*
 * cgmp_send_cleanup
 *
 * Send a CGMP Leave message with GDA = 0 and USA = 0. This basically cleans
 * the brains out of all the switches on the interface
 */
void cgmp_send_cleanup (idbtype *idb)
{

    paktype  *pak;
    cgmptype *cgmp;  
    ulong    length;

    /*
     * Build CGMP Leave packet.
     */
    length = sizeof(cgmptype) + sizeof(ieee_addrs);
    pak = getbuffer(length);
    if (!pak) return;

    cgmp = (cgmptype *) pak->network_start;
    cgmp->cgmp_version = CGMP_VERSION1;
    cgmp->cgmp_type = CGMP_LEAVE_TYPE;
    cgmp->cgmp_reserved[0] = 0;
    cgmp->cgmp_reserved[1] = 0;
    cgmp->cgmp_count = 1;
    ieee_zero(cgmp->cgmp_mac_addrs->daddr);
    ieee_zero(cgmp->cgmp_mac_addrs->saddr);

    CGMP_BUGINF("\nCGMP: Sending cleanup Leave on %s",
		idb_get_namestring(idb));

    /*
     * Setup packet, encapsulate and send out.
     */
    pak->linktype = LINK_CGMP;
    pak->if_output = idb;
    if (pak_encap(idb->hwptr, pak, length, 0)) {
	datagram_out(pak);
    } else {
	CGMP_BUGINF("\nCGMP: Encapsulation failure");
	datagram_done(pak);
    }
}

/*
 * cgmp_snap_it
 *
 * Set up SNAP encapsulated packet with destination MAC address of
 * 0100.0cdd.dddd.
 */
static boolean cgmp_snap_it (uchar **daddr, int *enctype, 
			     uchar const **other_info)
{
    *enctype = ET_SNAP;
    *other_info = cisco_snap;
    *daddr = cgmp_address;
    return(TRUE);
}

/*
 * cgmp_ether_macaddr
 */
static boolean cgmp_ether_macaddr (paktype *pak, long address, int *enctype,
				   uchar **daddr, uchar const **other_info)
{
    return(cgmp_snap_it(daddr, enctype, other_info));
}

/*
 * cgmp_fddi_macaddr
 */
static boolean cgmp_fddi_macaddr (paktype *pak, long address, int *enctype,
				  uchar **daddr, uchar const **other_info,
				  ulong *rif_flags)
{
    *rif_flags = determine_rif(pak->if_output, 0);
    return(cgmp_snap_it(daddr, enctype, other_info));
}

/*
 * cgmp_token_macaddr
 */
static boolean cgmp_token_macaddr (paktype *pak, long address, int *enctype,
				   uchar **daddr, uchar const **other_info,
				   ulong *rif_flags)
{
    *rif_flags = determine_rif(pak->if_output, 0);
    return(cgmp_snap_it(daddr, enctype, other_info));
}

/*
 * cgmp_atm_macaddr
 */
static boolean cgmp_atm_macaddr (paktype *pak, long address, hwaddrtype *daddr,
				 uchar const **snap_info, staticmap_type **map,
				 boolean *broadcast)
{

    *broadcast = TRUE;

    /*
     * We want to use the same broadcast maps we use for IP.
     */
    *map = staticmap_broadcast(pak->if_output, LINK_IP, ET_ATM);

    daddr->length = 0;
    *snap_info = cisco_snap;
    return(TRUE);
}

/*
 * cgmp_process_leave_command
 *
 * Hidden command to send CGMP Leave messages when IGMP Leave messages
 * are received. This feature assumes 1) there are single host segments
 * off of Cat5k switches, and 2) the hosts send unconditional IGMP Leave
 * messages. 
 *
 * This is not an advertised feature. This was put in for a critical DSI
 * competitive account.
 */
void cgmp_process_leaves_command (parseinfo *csb)
{
    idbtype *idb;

    idb = csb->interface;

    if (csb->nvgen) {
	nv_write(idb->cgmp_process_leaves, "%s", csb->nv_command);
	return;
    }
    idb->cgmp_process_leaves = csb->sense;
}

/*
 * cgmp_batch_joins_command
 *
 * Hidden command to configure doing CGMP batch joins.
 *
 */
void cgmp_batch_joins_command (parseinfo *csb)
{
    idbtype     *idb;
    igmpidbtype *igmpidb;

    idb = csb->interface;

    if (csb->nvgen) {
	nv_write(idb->cgmp_do_batch_joins, "%s", csb->nv_command);
	return;
    }
    idb->cgmp_do_batch_joins = csb->sense;

    /*
     * If disabling batch joins, release buffer.
     */
    if (!csb->sense) {
	igmpidb = igmp_getidb(idb);
	if (igmpidb && igmpidb->cgmp_pak) {
	    retbuffer(igmpidb->cgmp_pak);
	    igmpidb->cgmp_pak = NULL;
	}
    }
}

/*
 * cgmp_commands
 *
 * Process "ip cgmp" interface subcommand.  Warning: Don't fetch the igmpidb
 * until after igmp_enabled_idb() is called.
 */
void cgmp_commands (parseinfo *csb) 
{
    idbtype *idb, *temp_idb;
    ulong   count;
    list_element *l_elt;
    igmpidbtype *igmpidb;
    
    idb = csb->interface;

    if (csb->nvgen) {
	nv_write(idb->cgmp_enabled, "%s", csb->nv_command);
	if (idb->cgmp_proxy) nv_add(TRUE, " proxy");
	return;
    }

    /*
     * Only allow on Ethernet, Fddi, Token Ring and ATM (including LANE).
     */
    if (!is_ieee802(idb->hwptr) && !is_atm(idb->hwptr)) {
	printf("CGMP not supported on non-802 media");
	return;
    }

    /*
     * Figure out if any interface is already configured for CGMP.
     * We'll use this to determine whether the encapsulators are
     * registered.
     */
    count = 0;
    FOR_ALL_IPMULTICAST_IDBS(temp_idb, l_elt) {
	if (temp_idb->cgmp_enabled) count++;
    }

    idb->cgmp_proxy = GETOBJ(int,1);

    if (csb->sense) {

	if (!idb->cgmp_enabled) {

	    /*
	     * Enable IGMP and create the igmpidb, if necessary. 
	     */
	    igmp_enable_idb(idb);
	    if (!idb->igmp_enabled)
		return;
	    
	    /*
	     * Register the encapsulators if this is the initial
	     * CGMP interface.
	     * Do this BEFORE sending the initial Join.
	     */
	    if (0 == count) {
		reg_add_media_ether_macaddr(LINK_CGMP, cgmp_ether_macaddr, 
					    "cgmp_ether_macaddr");
		reg_add_media_fddi_macaddr(LINK_CGMP, cgmp_fddi_macaddr, 
					   "cgmp_fddi_macaddr");
		reg_add_media_token_macaddr(LINK_CGMP, cgmp_token_macaddr, 
					    "cgmp_token_macaddr");
		reg_add_media_atm_macaddr(LINK_CGMP, cgmp_atm_macaddr,
					  "cgmp_atm_macaddr");
	    }

	    idb->cgmp_enabled = TRUE;

	}

	/*
	 * Trigger CGMP self Join whenever command is typed in (even if it
	 * is already enabled).
	 */
	cgmp_send_self(CGMP_JOIN_TYPE, idb, 0);
	igmpidb = igmp_getidb(idb);
 	mgd_timer_start(&igmpidb->cgmp_timer, igmpidb->query_interval);

    } else {

	/*
	 * Trigger CGMP self Leave whenever command is typed in (even if it
	 * is already disabled).
	 */
	if (count != 0)
	    cgmp_send_self(CGMP_LEAVE_TYPE, idb, 0);

	if (idb->cgmp_enabled) {

	    idb->cgmp_enabled = FALSE;

	    /*
	     * Disable periodic timer.
	     */
	    igmpidb = igmp_getidb(idb);
	    mgd_timer_stop(&igmpidb->cgmp_timer);

	    /*
	     * Disable IGMP.
	     */
	    igmp_disable_idb(idb);

	    /*
	     * De-register the encapsulators if this was the only
	     * CGMP interface.
	     * Do this AFTER sending the final Leave.
	     */
	    if (1 == count) {
		reg_delete_media_ether_macaddr(LINK_CGMP);
		reg_delete_media_fddi_macaddr(LINK_CGMP);
		reg_delete_media_ether_macaddr(LINK_CGMP);
	    }
	}
    }
}

/*
 * cgmp_clear_command
 *
 * Process the "clear ip cgmp [<interface>]" command.
 */
void cgmp_clear_command (parseinfo *csb)
{
    
    idbtype *idb;
    list_element *l_elt;
    
    idb = csb->interface;
    
    if (idb) {
	if (idb->cgmp_enabled) cgmp_send_cleanup(idb);
    } else {
	FOR_ALL_IPMULTICAST_IDBS(idb, l_elt) {
	    if (!idb->cgmp_enabled) continue;
	    cgmp_send_cleanup(idb);
	}
    }
}

/*
 * cgmp_sources_exist_rnwalk
 *
 * Check RPF inteface of (S,G) against interface group membership just
 * timed out.
 */
int cgmp_sources_exist_rnwalk (rntype *node, va_list args)
{
    boolean *found;
    mdbtype *mdb;
    idbtype *idb;

    mdb = (mdbtype *) node;
    idb = va_arg(args, idbtype *);
    found = va_arg(args, boolean *);

    if (mdb->if_input == idb) {
	*found = TRUE;
	return(1);
    }
    return(0);
}

/*
 * cgmp_sources_exist
 *
 * The last member has left on the switched network. We have sent a
 * CGMP Leave for the group to clear state. However, if there still exists
 * sources in the switched network, we want to clear out the member ports
 * on the switches but keep group state populated so we can forward only
 * in the direction of routers.
 *
 * This function looks at all (S,G)s which have their RPF interface on the
 * switched network. When one is found, there is at least 1 source in the
 * switched network. Return TRUE in this case.
 */
boolean cgmp_sources_exist (ipaddrtype group, idbtype *idb)
{
    gdbtype *gdb;
    boolean found;

    gdb = ip_get_gdb(group);
    if (!gdb) return(FALSE);

    found = FALSE;
    rn_walktree(gdb->radix_head->rnh_treetop, cgmp_sources_exist_rnwalk, idb,
		&found);
    return(found);
}

/*
 * cgmp_process_igmp_report
 *
 * Process IGMP report received.
 */
void cgmp_process_igmp_report (idbtype *idb, igmpidbtype *igmpidb, 
       paktype *pak, igmptype *igmp, ipaddrtype source, ipaddrtype group)
{

    /*
     * Don't process groups in the 224.0.0.x range. The Catalyst boxes
     * won't process them anyways.
     */
    if (IPMULTICAST_RSVD(group)) return;

    /*
     * If we are in the middle of a general query interval, save the MAC
     * address of the reporter and send a batch join later.
     */
    if (idb->cgmp_do_batch_joins) {
	if (TIMER_RUNNING(igmpidb->query_resp_timer) &&
	    ELAPSED_TIME(igmpidb->query_resp_timer) < CGMP_BATCH_TIME) {
	    cgmp_save_info(idb, igmpidb, pak, group);
	    return;
	} 
    }
    cgmp_send_join(idb, pak, igmp, source, group);
}

/*
 * cgmp_save_info
 *
 * This function is called when an IGMP report is received within the max
 * query response interval. This function saves the MAC address of the
 * IGMP reporter for each group reported in a buffer which will be the payload
 * for the CGMP batch join. So when the query response interval ends, we can 
 * send a batch CGMP Join message with multiple groups encoded in as few as 
 * messages as possible.
 *
 * Otherwise, for unsolicited IGMP reports, we send one CGMP Join per group.
 */
void cgmp_save_info (idbtype *idb, igmpidbtype *igmpidb, paktype *pak, 
		     ipaddrtype group)
{
    uchar    *ptr;
    cgmptype *cgmp;
    ulong    max, length;
    hwaddrtype srcmac;
    ethermultiaddrtype dstmac = ETHER_IP_MCAST;

    /*
     * Put GDA and USA in local variables.
     */
    dstmac.bottom23 = group;
    (*idb->hwptr->extract_snpa)(pak, &srcmac, SOURCE);
    
    /*
     * Determine if GDA/USA pair already in buffer, if so, don't be redundant.
     */
    if (cgmp_find_entry(igmpidb, dstmac.addr, srcmac.addr)) return;

    /*
     * Get new or already allocated buffer to store GDA/USA.
     */
    ptr = cgmp_getbuffer(idb, igmpidb);
    if (!ptr) return;

    ieee_copy(dstmac.addr, ptr);
    ptr += IEEEBYTES;
    ieee_copy(srcmac.addr, ptr);

    cgmp = (cgmptype *) igmpidb->cgmp_pak->network_start;
    cgmp->cgmp_count++;
    CGMP_BUGINF("\nCGMP: Save state for GDA %e USA %e", dstmac.addr, ptr);

    /*
     * If next GDA/USA pair doesn't fit in buffer, send this packet out now.
     */
    max = idb->hwptr->maxmtu - SNAP_HDRBYTES;
    length = sizeof(cgmptype) + cgmp->cgmp_count * CGMP_ENTRY_SIZE;
    if ((length + CGMP_ENTRY_SIZE) > max) {
	CGMP_BUGINF("\nCGMP: Sending batch Join on %s", 
		    idb_get_namestring(idb));
	cgmp_send_join_packet(igmpidb->cgmp_pak, idb, cgmp->cgmp_count);
	igmpidb->cgmp_pak = NULL;
    }
}

/*
 * cgmp_find_entry
 *
 * Find GDA/USA pair in buffer.
 */
boolean cgmp_find_entry (igmpidbtype *igmpidb, uchar *gda, uchar *usa)
{
    cgmptype *cgmp;
    uchar    *ptr;
    int      i;

    if (!igmpidb->cgmp_pak) return(FALSE);
    
    cgmp = (cgmptype *) igmpidb->cgmp_pak->network_start;
    ptr = (uchar *) cgmp->cgmp_mac_addrs;

    for (i = 0; i < cgmp->cgmp_count; i++) {
	if (ieee_equal(gda, ptr) && 
	    ieee_equal(usa, ptr + IEEEBYTES)) return(TRUE);
	ptr += CGMP_ENTRY_SIZE;
    }
    return(FALSE);
}

/*
 * cgmp_getbuffer
 *
 * Allocate MTU size buffer and point headers to beginning of CGMP header
 * and variable body part. Return insertion pointer for next GDA/USA entry.
 * Return entry count pointer so caller can increment.
 */
uchar *cgmp_getbuffer (idbtype *idb, igmpidbtype *igmpidb)

{
    uchar    *ptr;
    paktype  *pak;
    cgmptype *cgmp;

    if (!igmpidb->cgmp_pak) {
	pak = getbuffer(idb->hwptr->maxmtu);
	if (!pak) return(NULL);
    } else {
	pak = igmpidb->cgmp_pak;
    }

    cgmp = (cgmptype *) pak->network_start;

    if (!igmpidb->cgmp_pak) {
	cgmp->cgmp_count = 0;
	igmpidb->cgmp_pak = pak;
    }

    ptr = pak->network_start + sizeof(cgmptype);
    ptr += (cgmp->cgmp_count * CGMP_ENTRY_SIZE);
    return(ptr);
}

/*
 * cgmp_send_batch_join
 *
 * Scan IGMP cache for interface to search for reporters that have sent
 * IGMP reports during the last query interval.
 */
void cgmp_send_batch_join (idbtype *idb, igmpidbtype *igmpidb)
{
    cgmptype *cgmp;

    /*
     * Stop query response timer. If no packet was built over the query 
     * response interval, just return.
     */
    TIMER_STOP(igmpidb->query_resp_timer);
    if (!igmpidb->cgmp_pak) return;

    CGMP_BUGINF("\nCGMP: Sending batch Join on %s", idb_get_namestring(idb));

    cgmp = (cgmptype *) igmpidb->cgmp_pak->network_start;
    cgmp_send_join_packet(igmpidb->cgmp_pak, idb, cgmp->cgmp_count);
    igmpidb->cgmp_pak = NULL;
}

/*
 * cgmp_send_join_packet
 * 
 * Send CGMP Join message with many entries present.
 */
void cgmp_send_join_packet (paktype *pak, idbtype *idb, uchar count)
{
    cgmptype *cgmp;
    ulong    length;

    /*
     * Build fix part of CGMP header.
     */
    cgmp = (cgmptype *) pak->network_start;
    cgmp->cgmp_version = CGMP_VERSION1;
    cgmp->cgmp_type = CGMP_JOIN_TYPE;
    cgmp->cgmp_reserved[0] = 0;
    cgmp->cgmp_reserved[1] = 0;
    cgmp->cgmp_count = count;
    length = sizeof(cgmptype) + count * CGMP_ENTRY_SIZE;

    /*
     * Setup packet, encapsulate and send out.
     */
    pak->linktype = LINK_CGMP;
    pak->if_output = idb;
    if (pak_encap(idb->hwptr, pak, length, 0)) {
	datagram_out(pak);
    } else {
	CGMP_BUGINF("\nCGMP: Encapsulation failure");
	datagram_done(pak);
    }
}
