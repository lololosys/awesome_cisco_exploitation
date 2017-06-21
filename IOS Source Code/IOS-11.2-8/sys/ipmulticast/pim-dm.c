/* $Id: pim-dm.c,v 3.10.8.8 1996/09/12 21:09:27 chwhite Exp $
 * $Source: /release/112/cvs/Xsys/ipmulticast/pim-dm.c,v $
 *------------------------------------------------------------------
 * pim-dm.c - Protocol Independent Multicast support. Dense-mode specific
 *            functions.
 *
 * November, 1993.
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 * PIM is a dynamic IP multicasting protocol used to build shared and
 * source-based trees to route multicast datagrams.
 *------------------------------------------------------------------
 * $Log: pim-dm.c,v $
 * Revision 3.10.8.8  1996/09/12  21:09:27  chwhite
 * CSCdi61325:  (*,G) JOINS received by the RP do not trigger (S,G) JOINS
 * to source
 * Branch: California_branch
 *
 * Revision 3.10.8.7  1996/09/10  05:31:52  lwei
 * CSCdi68465:  RP information is too difficult to change in last-hop
 *              routers
 * Branch: California_branch
 *
 * Revision 3.10.8.6  1996/08/21  01:21:19  lwei
 * CSCdi66170:  Sparse-dense/IGMP breaks triggered joins for sparse mode
 *              groups
 * Branch: California_branch
 *
 * Revision 3.10.8.5  1996/08/09  03:33:45  mleelani
 * CSCdi65587:  (S, G) state gratuitously created on receiving prunes.
 * Branch: California_branch
 * Create (S, G) state only on receiveing (S, G, RP-bit) prunes.
 *
 * Revision 3.10.8.4  1996/08/07  23:50:42  dino
 * CSCdi65425:  Make SPARSE_FLAG setting more consistent amoung rtrs in a
 *              PIM domain
 * Branch: California_branch
 *
 * Revision 3.10.8.3  1996/08/03  01:20:35  lwei
 * CSCdi60059:  rp entries are not timed out in the gdb
 * Branch: California_branch
 *
 * Revision 3.10.8.2  1996/06/18  01:46:50  hampton
 * Split the monolithic traffic array into smaller per-protocol traffic
 * arrays.  [CSCdi59224]
 * Branch: California_branch
 *
 * Revision 3.10.8.1  1996/03/18  20:19:00  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.6.2.4  1996/03/16  06:55:22  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.6.2.3  1996/03/13  01:38:45  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.6.2.2  1996/03/07  09:48:12  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.6.2.1  1996/02/20  01:01:34  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.10  1996/02/28  03:43:47  mleelani
 * CSCdi50113:  Mroutes not always added correctly on receiving
 * grafts/joins.
 * A valid mdb may not be always present on receiving grafts.
 *
 * Revision 3.9  1996/02/22  19:27:11  lwei
 * CSCdi49375:  Missing DVMRP graft on PIM/DVMRP border causes join latency
 *
 * Revision 3.8  1996/02/22  09:03:13  dino
 * CSCdi49566:  Allow control over which RPs are used in transit routers.
 *
 * Revision 3.7  1996/02/08  03:39:27  lwei
 * CSCdi48369:  IP multicast does not support ip pim sparse-dense
 *              configuration
 *
 * Revision 3.6  1996/01/22  18:18:55  dino
 * CSCdi47278:  Rp-bit Asserts are not buginf()
 *
 * Revision 3.5  1995/12/17  19:24:22  dino
 * CSCdi44742:  PIM should only switch to the SPT when traffic threshold
 *              is reached
 *
 * Revision 3.4  1995/11/30  04:42:11  mleelani
 * CSCdi44573:  IGMP doesnt process leave messages
 * o Add support for leave messages.
 * o Create idb sub-block for IGMP; use managed timers for IGMP timers.
 *
 * Revision 3.3  1995/11/21  23:12:09  dino
 * CSCdi43427:  cisco does not process DVMRP prunes.
 *
 * Revision 3.2  1995/11/17  17:30:11  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:01:38  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/08/17  01:58:41  dino
 * CSCdi30823:  PIM Assert metrics use 0 for directly connected sources.
 *
 * Revision 2.3  1995/07/24  07:33:45  hampton
 * Transition IP Multicast support to use the passive timer macros for all
 * its timers.  It no longer references the system clock directly.
 * [CSCdi37539]
 *
 * Revision 2.2  1995/07/01 07:46:15  dino
 * CSCdi27921:  Border router needs to send Registers to RP. ISPs want PIM
 *              over DVMRP unicast routing.
 *
 * Revision 2.1  1995/06/07  21:04:13  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <stdarg.h>
#include <ciscolib.h>
#include "interface_private.h"
#include "logger.h"
#include "packet.h"
#include "access.h"
#include "../ui/debug.h"
#include "../ip/ip.h"
#include "../ip/ipaccess.h"
#include "../iprouting/route.h"
#include "../ipmulticast/ipmulticast_debug.h"
#include "../ipmulticast/igmp.h"
#include "../ipmulticast/mroute.h"
#include "../ipmulticast/pim.h"
#include "../ipmulticast/dvmrp.h"


/*
 * pim_send_assert
 *
 * Send PIM assert message for a single (S,G) entry out the specified
 * interface.
 */
void pim_send_assert (gdbtype *gdb, mdbtype *mdb, midbtype *midb, 
		      ipaddrtype source_host)
{

    paktype        *pak, *pak_copy;
    igmptype       *igmp;
    pim_asserttype *pim;
    idbtype        *idb;
    int            size;
    ulong          metric, metric_pref;

    /*
     * If we are in Prune state for this interface and we would be the
     * forwarder if there were downstream members, tell the neighbor to
     * to Prune.
     */
    if (midb->state == MIDB_PRUNE_STATE && !midb->forwarder) return;

    /*
     * Rate limit Asserts.
     */
    if (ELAPSED_TIME(midb->last_assert) < PIM_ASSERT_FREQUENCY) return;

    idb = midb->idb;

    /*
     * Don't send Assert on point-to-pont links.
     */
    if (is_p2p(idb)) return;

    /*
     * Don't send Assert on interfaces with no PIM neighbors.
     */
    if (!idb->pim_nbrs) return;

    /*
     * Get packet buffer.
     */
    size = IPHEADERBYTES(NOPT) + IGMPHEADERBYTES + sizeof(pim_asserttype);

    pak = getbuffer(size);
    if (!pak) return;

    /*
     * Build IGMP fixed part of header.
     */
    igmp = (igmptype *) (ipheadstart(pak) + IPHEADERBYTES(NOPT));
    igmp->type = IGMP_PIM_TYPE;
    igmp->code = PIM_ASSERT_CODE;
    igmp->checksum = 0;
    igmp->address = PIM_VERSION1;

    /*
     * Build Assert part of PIM header.
     */
    pim = (pim_asserttype *) igmp->data;
    pim->group = mdb->group;
    pim->group_mask = MASK_HOST;
    pim->host_address = source_host;
    if (mdb == &gdb->mdb || (mdb->flags & RP_BIT_FLAG)) {
	pim_get_metric(gdb->rp.address, mdb->rpf_type, &metric_pref, 
		       &metric);
	metric_pref |= PIM_ASSERT_RP_BIT;
    } else {
	pim_get_metric(source_host, mdb->rpf_type, &metric_pref, &metric);
    }
    pim->metric_preference = metric_pref;
    pim->metric = metric;
    
    PIM_BUGINF(mdb->group, 
       ("\nPIM: Send Assert on %s for %i, source %i, metric [%d/%d]%s", 
	idb->namestring, mdb->group, source_host, metric_pref, metric,
	(metric_pref & PIM_ASSERT_RP_BIT) ? ", RP-bit" : " "));

    igmp->checksum = ipcrc((ushort *) igmp, size - IPHEADERBYTES(NOPT));

    /*
     * Make copy of packet if we are going to loop it back to ourselves.
     */
    pak_copy = NULL;
    if (midb->state == MIDB_FORWARD_STATE) {
	pak_copy = pak_duplicate(pak);
    }
    if (!pak_copy) {
	pak_copy = pak;
	pak = NULL;
    }
    
    /*
     * Send to IP.
     */
    ipwrite(pak_copy, size, IGMP_PROT, 0, ALLROUTERS, idb, NULL, 
	    &igmprouteparams);
    igmp_traffic.output_pim++;

    /*
     * Remember time Assert is sent.
     */
    GET_TIMESTAMP(midb->last_assert);

    /*
     * Send our assert to ourselves. This allows us to Prune on a transit
     * LAN if we are the last upstream router in Forward state.
     */
    if (pak) {
	pim_receive_assert(igmp, idb->ip_address, idb, sizeof(pim_asserttype));
	retbuffer(pak);
    }
}

/*
 * pim_receive_assert
 *
 * Process received Assert message. Decide if an outgoing interface
 * should be deleted from an (S,G) entry. This code eliminates sustained
 * duplicate packets on LANs.
 */
void pim_receive_assert (igmptype *igmp, ipaddrtype srcadr, idbtype *if_input,
			 int paklen)
{

    gdbtype        *gdb;
    mdbtype        *mdb;
    midbtype       *midb;
    pim_asserttype *pim;
    ulong          metric_pref, metric;
    boolean        we_win, direct_members, rp_bit, better;
    ipaddrtype     our_address, host_address;

    pim = (pim_asserttype *) igmp->data;

    /*
     * Validate packet length.
     */
    if (paklen < sizeof(pim_asserttype)) {
	PIM_BUGINF(0, ("\nPIM: Packet too short"));
	return;
    }
    if (pim->group_mask != MASK_HOST) {
	PIM_BUGINF(pim->group, ("\nPIM: Only support single group masks"));
	return;
    }

    /*
     * Shouldn't process Assert on NBMA-mode network. Sender is not configured
     * consistently.
     */
    if (if_input->pim_nbma_mode) {
	PIM_BUGINF(pim->group, ("\nPIM: Assert discarded on NBMA interface"));
	return;
    }

    host_address = pim->host_address;
    mdb = NULL;
    gdb = ip_get_gdb(pim->group);
    if (gdb) mdb = ip_get_best_mdb(gdb, host_address);

    rp_bit = (pim->metric_preference & PIM_ASSERT_RP_BIT);

    /*
     * If no state, create it.
     */
    if (!mdb) {
	pim_add_mroute(&gdb, &mdb, pim->group, host_address, MASK_HOST,
	       if_input, 0, MIDB_FORWARD_STATE, MDB_EXPIRATION_TIME, 
	       FALSE);
	if (!mdb) {
	    PIM_BUGINF(pim->group, 
		       ("\nPIM: Could not create multicast route"));
	    return;
	}

	/*
	 * If RP-bit is set in Assert, use an (S,G),RP-bit entry for Asserting.
	 * Set the RP-bit and point RPF parameters toward RP.
	 */
	if (rp_bit) {
	    mdb->flags |= RP_BIT_FLAG;
	    ip_rpf_check_mroute(gdb, mdb);
	    PIM_BUGINF(pim->group, ("\nPIM: Create RP-bit entry"));
	}
    }

    PIM_BUGINF(pim->group, ("\nPIM: Assert metric to %s %i is [%d/%d]", 
			    (rp_bit) ? "RP" : "source", host_address,
			    pim->metric_preference, pim->metric));

    /*
     * Point host address towards RP so we get metrics to the RP for (*,G)
     * Asserts or (S,G),RP-bit Asserts.
     */
    if (rp_bit) {
	host_address = gdb->rp.address;
    }

    /*
     * Treat interface with DVMRP neighbors as if there were direct members.
     */
    direct_members = (boolean) igmp_find_group(pim->group, if_input);
    direct_members |= (boolean) DVMRP_ACTIVE(if_input);

    /*
     * Received Assert on incoming interface.  Decide if this Assert sender 
     * should be RPF neighbor. Schedule join if there are downstream receivers.
     */
    if (mdb->if_input == if_input) {
	PIM_BUGINF(pim->group, ("\nPIM: Cached metric is [%d/%d]", 
				mdb->metric_pref, mdb->metric));

        /*
         * If the RP-bit in the assert does not agree with the RP-bit flag
         * in (S,G), don't change incoming interface based on this assert.
         */
        if ((mdb->flags & RP_BIT_FLAG) ^ rp_bit) return;

	better = pim_compare_metrics(srcadr, pim->metric_preference, 
		     pim->metric, mdb->rpf_nbr, mdb->metric_pref, mdb->metric);

	/*
	 * Same RPF neighbor. Check to see if it lost its path to the source.
	 * If so, clear flag so we can choose an RPF neighbor from the 
	 * unicast routing table.
	 */
	if (mdb->rpf_nbr == srcadr && 
	    (pim->metric_preference & ~PIM_ASSERT_RP_BIT) == PIM_ASSERT_INFINITY &&
	    mdb->use_assert_rpf) {
	    mdb->use_assert_rpf = 0;
	    PIM_BUGINF(pim->group, ("\nPIM: RPF nbr %i can't reach source", 
				    mdb->rpf_nbr));
	} else if (!better || mdb->rpf_nbr == srcadr) {
	    PIM_BUGINF(pim->group, ("\nPIM: Keep RPF nbr %i", mdb->rpf_nbr));
	} else {

	    /*
	     * Change RPF neighbor. Note that RPF neighbor was computed from
	     * Assert rather than from the unicast routing table.
	     */
	    mdb->rpf_nbr = srcadr;
	    mdb->use_assert_rpf = ASSERT_RPF_FACTOR;
	    PIM_BUGINF(pim->group, ("\nPIM: New RPF nbr %i", srcadr));
	    MROUTE_BUGINF(mdb->group, 
		  ("\nMRT: New RPF nbr %i from Assert for (%i/%d, %i)", 
		   srcadr, ip_radix2addr(mdb->source), 
		   ip_bitsinmask(ip_radix2addr(mdb->mask)), mdb->group));
	}
	
	/*
	 * Save metric from Assert packet if better than cached values.
	 */
	if (better) {
	    mdb->metric_pref = pim->metric_preference;
	    mdb->metric = pim->metric;
	}

	/*
	 * If there are directly connected members, the forwarder will not
	 * schedule a prune. If there is no direct members, and we have
	 * downstream receivers, send delayed join to tell forwarder to
	 * cancel prune.
	 *
	 * There are downstream receivers if the olist is non-NULL, or
	 * if this is the RP and the (S,G) iif is in the (*,G) olist.
	 */
	midb = ip_get_midb(&gdb->mdb.if_outputQ, if_input, 0, FALSE);
	if (direct_members) return;

	if (!MDB_OUTPUTQ_NULL(mdb) ||
	    (midb && ip_ouraddress(gdb->rp.address))) {
	    pim_schedule_join(mdb);
	}
	return;
    }

    /*
     * Received Assert on outgoing interface.
     */
    midb = ip_get_midb(&mdb->if_outputQ, if_input, 0, FALSE);
    if (!midb) return;

    /*
     * Get unicast metric for host address.
     */
    pim_get_metric(host_address, mdb->rpf_type, &metric_pref, &metric);
    if (mdb->flags & RP_BIT_FLAG) {
	metric_pref |= PIM_ASSERT_RP_BIT;
    }
    
    our_address = if_input->ip_address;
    if (!our_address && if_input->ip_unnumbered) {
	our_address = if_input->ip_unnumbered->ip_address;
    }

    we_win = !pim_compare_metrics(srcadr, pim->metric_preference, pim->metric, 
				  our_address, metric_pref, metric);
    PIM_BUGINF(pim->group, ("\nPIM: We %s, our metric [%d/%d]", (we_win) ?
			    "win" : "lose", metric_pref, metric));

    /*
     * If we have better metrics to source, we are forwarder for LAN.
     * Keep/put interface in Forward state. Schedule to prune interface if 
     * no directly connected members.
     */
    if (we_win) {
	midb->forwarder = TRUE;
	if (midb->state == MIDB_PRUNE_STATE) {
	    pim_add_mroute(&gdb, &mdb, mdb->group, ip_radix2addr(mdb->source),
		 ip_radix2addr(mdb->mask), if_input, 0, MIDB_FORWARD_STATE, 
		 MDB_EXPIRATION_TIME, FALSE);
	    PIM_BUGINF(pim->group,
		       ("\nPIM: Changed %s from Prune to Forward state",
			if_input->namestring));
	} else if (!direct_members) {
	    if (!TIMER_RUNNING(midb->prune_time)) {
		TIMER_START(midb->prune_time, PIM_PRUNE_DELAY);
		PIM_BUGINF(pim->group, ("\nPIM: Schedule to prune %s", 
					if_input->namestring));
	    }
	}
    } else {
	midb->forwarder = FALSE;

	/*
	 * We lost, prune interface now if we were in Forward state. If we
	 * were in Prune state, ignore Assert.	
	 */
	if (midb->state == MIDB_FORWARD_STATE) {
	    pim_prune_interface(gdb, mdb, midb, MDB_EXPIRATION_TIME);
	    return;
	}
    }

    PIM_BUGINF(pim->group, ("\nPIM: (%i/%d, %i) oif %s in %s state",
	    ip_radix2addr(mdb->source), 
	    ip_bitsinmask(ip_radix2addr(mdb->mask)), mdb->group,
	    if_input->namestring, midb_state_str[midb->state]));

    /*
     * If we won the Assert, send an Assert so everyone else knows we are
     * the winner. Asserts are rate-limited so there isn't one sent for
     * for each Assert received.
     */
    if (we_win) pim_send_assert(gdb, mdb, midb, pim->host_address);
}

/*
 * pim_send_graft
 *
 * Send PIM-DM Graft messages for this group to the various sources.
 */
void pim_send_graft (gdbtype *gdb)
{

    midbtype *midb;
    paktype  *ip, *pak;
    boolean  sent = FALSE;

    /*
     * For each dense-mode interface known for group, send a graft for all 
     * (S,G) that has incoming interface equal to the one we're processing.
     */
    for (midb = (midbtype *) gdb->mdb.if_outputQ.qhead; midb; 
	 midb = midb->next) {
	if (!IS_DENSE_MODE(midb->idb, gdb) && !DVMRP_ACTIVE(midb->idb)) {
		continue;
	}

	PIM_BUGINF(gdb->mdb.group,
		   ("\nPIM: Building Graft message for %i, %s:", 
		    gdb->mdb.group, midb->idb->namestring));

	ip = NULL;
	rn_walktree(gdb->radix_head->rnh_treetop, pim_build_graft_entry, 
		    midb->idb, gdb, &ip, &sent);
	if (!ip) PIM_BUGINF(gdb->mdb.group, (" no entries"));

	while (ip) {
	    pak = ip;
	    ip = (paktype *) ip->dataptr;
	    pim_send_graft_packet(pak, gdb->mdb.group);
	    sent = TRUE;
	}
    }

    /*
     * Set retransmission timer only if any of the (S,G)s were flagged to
     * be sent out.
     */
    if (sent) {
	TIMER_START(gdb->send_graft_time, PIM_GRAFT_FREQUENCY);
    } else {
	TIMER_STOP(gdb->send_graft_time);
    }
}

/*
 * pim_build_graft_entry
 *
 * Put (S,G) entry's for the same known RPF neighbor in separate packets.
 * This function will build a linked list of iptype's. Each element in the
 * linked list is a graft packet that will be unicast to a specific neighbor.
 * If any packet, exceeds the MTU it is sent out and another buffer is
 * allocated if there are more (S,G) entries for the specific RPF neighbor.
 *
 * This function uses the following fields ip PAKMACRO:
 *      pak->desthost
 *      pak->if_output
 *      pak->dataptr
 *      pak->length
 */
int pim_build_graft_entry (rntype *node, va_list args)
{
    
    mdbtype       *mdb;
    gdbtype       *gdb;
    idbtype       *idb;
    paktype       **pak;
    paktype       *ip;
    pim_addrtype  *pim_addr;
    pim_grouptype *pim_group;
    ipaddrtype    addr;
    boolean       *sent, new_pak;
    uchar         *str;

    mdb = (mdbtype *) node;
    idb = va_arg(args, idbtype *);
    gdb = va_arg(args, gdbtype *);

    /*
     * Check flag so we know to possibly retransmit this (S,G).
     */
    if (!mdb->send_graft) return(0);

    /*
     * We are sending grafts upstream. If the interface we are processing
     * is not an incoming interface for (S,G), then skip it.
     */
    if (idb != mdb->if_input) return(0);

    /*
     * If by some chance, the upstream router is not a PIM router (i.e.
     * static route pointing to a non-multicast router), don't send graft
     * to it. If the upstream router is a DVMRP router, send a DVMRP Graft
     * message.
     */
    if (!pim_find_nbr(mdb->rpf_nbr, mdb->if_input)) {
	if (mdb->rpf_type == RPF_DVMRP || DVMRP_ACTIVE(mdb->if_input)) {
	    dvmrp_send_graft(gdb, mdb);
	}
	return(0);
    }

    pak = va_arg(args, paktype **);
    sent = va_arg(args, boolean *);

    /*
     * This indicates that there exist an (S,G) with the send_graft flag
     * sent. Used to run the retransmission timer.
     */
    *sent = TRUE;

    /*
     * See if buffer is allocated for RPF neighbor. If so, add entry to
     * it. Otherwise, allocate new buffer. If there is no room in existing
     * buffer for entry, send packet and get new buffer.
     */
    new_pak = FALSE;
    for (ip = *pak; ip; ip = (paktype *) ip->dataptr) {
	if (mdb->rpf_nbr == ip->desthost) break;
    }
    if (ip && (ip->length + PIM_ADDRTYPE_SIZE) > mdb->if_input->ip_mtu) {
	pim_delink_packet(pak, ip);
	pim_send_graft_packet(ip, mdb->group);
	ip = NULL;
    }

    if (!ip) {
	ip = getbuffer(mdb->if_input->ip_mtu);
	if (!ip) return(1);

	ip->length = IPHEADERBYTES(NOPT) + IGMPHEADERBYTES + sizeof(pimtype) + 
	    + sizeof(pim_grouptype);
	ip->desthost = mdb->rpf_nbr;
	ip->if_output = mdb->if_input;
	ip->dataptr = (*pak) ? (uchar *) *pak : NULL;
	*pak = ip;
	new_pak = TRUE;
    }

    /*
     * Set up address pointers in packet to store join-list count and
     * source address.
     */
    pim_group = (pim_grouptype *) 
	(((char *) ipheadstart(ip)) + IPHEADERBYTES(NOPT) + IGMPHEADERBYTES +
	 sizeof(pimtype));
    if (new_pak) {
	pim_group->num_join_sources = 0;
	pim_addr = (pim_addrtype *) (pim_group + 1);
    } else {
	pim_addr = (pim_addrtype *) (((char *) pim_group) + 
	     sizeof(pim_grouptype) + 
	     pim_group->num_join_sources * PIM_ADDRTYPE_SIZE);
    }

    /*
     * Insert source adddress, mask count in packet.	
     */
    pim_addr->pad = 0;
    pim_addr->sparse_bit = 0;
    pim_addr->wc_bit = 0;
    pim_addr->rp_bit = 0;
    pim_addr->mask_length = ip_bitsinmask(ip_radix2addr(mdb->mask));
    addr = ip_radix2addr(mdb->source);
    PUTLONG(pim_addr->address, addr);

    if (PIM_DEBUG(mdb->group)) {
	str = (pim_group->num_join_sources % 5) ? " " : "\n     ";
	buginf("%s%i/%d", str, addr, pim_addr->mask_length, mdb->group);
    }

    /*
     * Add to number of sources in packet and adjust size of packet.	
     */
    pim_group->num_join_sources++;
    ip->length += PIM_ADDRTYPE_SIZE;
    return(0);
}

/*
 * pim_send_graft_packet
 *
 * A Graft packet body has been built. Fill in fixed header stuff and
 * send down to IP.
 */
void pim_send_graft_packet (paktype *pak, ipaddrtype group)
{

    igmptype      *igmp;
    pimtype       *pim;
    pim_grouptype *pim_group;
    idbtype       *idb;
    ulong         length;
    ipaddrtype    nbr;

    length = pak->length;
    nbr = (pak->desthost) ? pak->desthost : ALLROUTERS;
    idb = pak->if_output;

    /*
     * Build IGMP fixed part of header.
     */
    igmp = (igmptype *) (ipheadstart(pak) + IPHEADERBYTES(NOPT));
    igmp->type = IGMP_PIM_TYPE;
    igmp->code = PIM_GRAFT_CODE;
    igmp->checksum = 0;
    igmp->address = PIM_VERSION1;

    /*
     * Build PIM fixed part of header. The join-list is built in
     * pim_build_graft_entry().
     */
    pim = (pimtype *) igmp->data;
    pim->address = nbr;
    pim->reserved1 = 0;
    pim->reserved2 = 0;
    pim->holdtime = (PIM_MESSAGE_INTERVAL * PIM_MULTIPLIER) / ONESEC;
    pim->maddr_length = sizeof(ipaddrtype);
    pim->addr_length = sizeof(ipaddrtype);
    pim->num_groups = 1;

    /*
     * Build PIM group part of message. Number of join sources was computed
     * when body of packet was built.
     */
    pim_group = (pim_grouptype *) pim->data;
    pim_group->group = group;
    pim_group->group_mask = MASK_HOST;
    pim_group->num_prune_sources = 0;

    igmp->checksum = ipcrc((ushort *) igmp, length - IPHEADERBYTES(NOPT));

    PIM_BUGINF(group, ("\nPIM: Send Graft to %i (%s)", nbr, idb->namestring));

    /*
     * If unnumbered interface, make sure we send it out that interface and
     * not the one IP might pick.
     */
    if (idb->ip_unnumbered) pak->if_output = idb;

    /*
     * Send to IP.
     */
    ipwrite(pak, length, IGMP_PROT, 0, nbr, idb, NULL, &igmprouteparams);
    igmp_traffic.output_pim++;
}

/*
 * pim_receive_graft
 *
 * Process received Graft packet. For each (S,G) enable outgoing interface
 * if in prune state. If Graft received on an incoming interface for (S,G)
 * ignore. If an (S,G) entry was in Prune state (all interfaces in Prune
 * state), trigger a Graft upstream.
 */
void pim_receive_graft (igmptype *igmp, ipaddrtype srcadr, idbtype *if_input,
			ulong paklen)
{

    gdbtype      *gdb;
    mdbtype      *mdb;
    pimtype      *pim;
    pim_parmtype parm;
    ipaddrtype   addr, mask;
    boolean      set_flag;

    /*
     * Packet length passed in does not include IP and IGMP headers.
     * Leave length of PIM message body.
     */
    pim = (pimtype *) igmp->data;
    pim_init_parm(&parm, pim, paklen);

    /*
     * Validate fixed PIM header.
     */
    if (!pim_validate_header(pim, igmp->code)) return;

    /*
     * If not addressed to us, ignore.
     */
    if (!ip_ifaddress(if_input, pim->address)) return;

    /*
     * Parse each entry in packet.
     */
    gdb = NULL;
    while (pim_parse_packet(&parm)) {

	/*
	 * Find (*,G) entry. If it doesn't exist, we will create the
	 * state since we have source information to propagate upstream.
	 */
	if (!gdb || gdb->mdb.group != parm.group->group) {
	    gdb = ip_get_gdb(parm.group->group);
	}

	mask = ip_bitcount2mask(parm.source->mask_length);
	addr = GETLONG(parm.source->address);
	mdb = (gdb) ? ip_get_mdb(gdb, addr, mask) : NULL;

	PIM_BUGINF(parm.group->group, ("\nPIM: Join-list: (%i/%d, %i)", addr, 
		parm.source->mask_length, parm.group->group));

	/*
	 * If the entry was in prune state or did not exist, trigger a 
	 * graft upstream. If entry just went out of negative cache state
	 * there might be an upstream router that has a negative cache
	 * entry. In this case mdb->send_graft will be true and we want
	 * to trigger a Graft message.
	 */
	set_flag = (!mdb || MDB_OUTPUTQ_NULL(mdb) || mdb->send_graft);

	/*
	 * Add interface to outgoing list.
	 */
	pim_add_mroute(&gdb, &mdb, parm.group->group, addr, mask, if_input, 0,
		       MIDB_FORWARD_STATE, pim->holdtime, FALSE);
	if (!mdb) continue;

	/*
	 * Don't trigger graft if received on an incoming interface for
	 * this (S,G).
	 */
	if (mdb->if_input == if_input) continue;

	/*
	 * Don't trigger graft if source is on incoming interface. We are
	 * at top of source tree.
	 */
	if (samecable(ip_radix2addr(mdb->source)) == mdb->if_input) continue;

	/*
	 * If RPF neighbor is not known or does not do PIM, don't send
	 * graft.
	 */
	if (!mdb->rpf_nbr || !pim_find_nbr(mdb->rpf_nbr, mdb->if_input)) {
	    continue;
	}

	/*
	 * Flag group for triggered graft, if any.
	 */
	if (set_flag) {
	    mdb->send_graft = TRUE;
	    if (!TIMER_RUNNING(gdb->send_graft_time))
		GET_TIMESTAMP(gdb->send_graft_time);
	}
    }
}

/*
 * pim_send_graft_ack
 *
 * Send Graft-Ack packet because Graft was received error-free.
 */
void pim_send_graft_ack (paktype *pak, ulong length)
{

    igmptype *igmp;
    pimtype  *pim;
    idbtype  *idb;

    /*
     * Change code from Graft to Graft-Ack.
     */
    igmp = (igmptype *) (ipheadstart(pak) + IPHEADERBYTES(NOPT));
    pim = (pimtype *) igmp->data;
    igmp->code = PIM_GRAFT_ACK_CODE;
    igmp->checksum = 0;
    pim->address = ((iphdrtype *) ipheadstart(pak))->srcadr;

    /*
     * Incoming length does not include IP and IGMP headers. Add in
     * IGMP header length, checksum, then add in IP header length.
     */
    length += IGMPHEADERBYTES;
    igmp->checksum = ipcrc((ushort *) igmp, length);
    length += IPHEADERBYTES(NOPT);

    idb = pak->if_input;
    clear_if_input(pak);

    PIM_BUGINF(0, ("\nPIM: Send Graft-Ack on %s to %i", idb->namestring, 
		   pim->address));

    /*
     * Send to IP. Packet is unicast to the sender of Graft.
     */
    ipwrite(pak, length, IGMP_PROT, 0, pim->address, idb, NULL, 
	    &igmprouteparams);
    igmp_traffic.output_pim++;
}

/*
 * pim_receive_graft_ack
 *
 * Processing incoming Graft-Ack packet. For each (S,G), clear send_graft
 * flag so it is not retransmitted.
 */
void pim_receive_graft_ack (igmptype *igmp, ipaddrtype srcadr, 
			    idbtype *if_input, ulong paklen)
{

    pimtype      *pim;
    gdbtype      *gdb;
    mdbtype      *mdb;
    uchar        *str;
    pim_parmtype parm;
    ipaddrtype   addr, mask;
    int          count;

    /*
     * Packet length passed in does not include IP and IGMP headers.
     * Leave length of PIM message body.
     */
    pim = (pimtype *) igmp->data;
    pim_init_parm(&parm, pim, paklen);

    /*
     * Validate fixed PIM header.
     */
    if (!pim_validate_header(pim, igmp->code)) return;

    /*
     * If not addressed to us, ignore.
     */
    if (!ip_ifaddress(if_input, pim->address)) return;

    /*
     * Parse each entry in packet.
     */
    count = 0;
    gdb = NULL;
    while (pim_parse_packet(&parm)) {
	if (!gdb || gdb->mdb.group != parm.group->group) {
	    gdb = ip_get_gdb(parm.group->group);
	    if (gdb) {
		PIM_BUGINF(parm.group->group,
			   ("\n     Group %i:", parm.group->group));
	    }
	}
	
	/*
	 * Don't know about this group, ignore ack for this group.
	 */
	if (!gdb) {
	    if (pim_parse_getnext_group(&parm)) continue;
	    return;
	}
	
	mask = ip_bitcount2mask(parm.source->mask_length);
        addr = GETLONG(parm.source->address);
	mdb = ip_get_mdb(gdb, addr, mask);
	if (!mdb) continue;

	if (PIM_DEBUG(mdb->group)) {
	    str = (count % 5) ? " " : "\n     ";
	    count++;
	    buginf("%s%i/%d", str, addr, parm.source->mask_length);
	}

	/*
	 * Clear flag so we don't retransmit Graft anymore.
	 */
	mdb->send_graft = FALSE;
    }
}
