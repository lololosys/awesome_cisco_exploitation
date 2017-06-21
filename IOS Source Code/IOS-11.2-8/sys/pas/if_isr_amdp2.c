/* $Id: if_isr_amdp2.c,v 3.6.44.10 1996/09/01 21:20:21 fowler Exp $
 * $Source: /release/112/cvs/Xsys/pas/if_isr_amdp2.c,v $
 *------------------------------------------------------------------
 * if_isr_amdp2.c - 8e Presidio2 based PCI port adaptor support
 *		    (Interrupt service routines)
 *
 * Dave Turner, April 1995
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: if_isr_amdp2.c,v $
 * Revision 3.6.44.10  1996/09/01  21:20:21  fowler
 * CSCdi60734:  Frame Relay rate enforcement is not effective
 *              Remove original fix due to its performance impact
 * Branch: California_branch
 *
 * Revision 3.6.44.9  1996/08/30  16:48:16  fowler
 * CSCdi60734:  Frame Relay rate enforcement is not effective
 *              Add registry to check for the need for media dependent
 *              traffic shaping
 * Branch: California_branch
 *
 * Revision 3.6.44.8  1996/08/29  07:31:35  abaindur
 * CSCdi67514:  Spurious accesses while local switching on VIP
 * Branch: California_branch
 *  pak->if_output was not being set for the traffic shaping check.
 *  Bypass traffic shaping on the VIP for now.
 *
 * Revision 3.6.44.7  1996/08/28  13:05:53  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.6.44.6  1996/08/24  01:50:57  abaindur
 * CSCdi66979:  Traffic shaping broken on the c7200
 * Branch: California_branch
 *
 * Revision 3.6.44.5  1996/08/13  02:21:12  gstovall
 * CSCdi39484:  Giant packets output on ethernet interface
 * Branch: California_branch
 * Make sure we drop giants, even if they are just a wee bit too big.
 *
 * Revision 3.6.44.4  1996/05/07  01:22:31  mbeesley
 * CSCdi56710:  Correct AMDp2 fifo setting, update error messages
 * Branch: California_branch
 *
 * Revision 3.6.44.3  1996/03/29  04:05:09  sjacobso
 * CSCdi52266:  4E/8E receive promiscuously, hurting performance
 * Branch: California_branch
 * Change write_address_filter code to examine address filter
 * entries for important status, and to store that status in the
 * software address filter.  The correct common routines for
 * adding MAC addresses are called.  Also, fix Predator logical
 * address filter code to work correctly with HSRP addresses.
 *
 * Revision 3.6.44.2  1996/03/21  23:22:59  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.6.44.1  1996/03/18  21:34:20  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.4.2.12  1996/03/06  18:22:54  getchell
 * CSCdi50848:  single particle optimization
 * Branch: ELC_branch
 *
 * Revision 3.4.2.11  1996/02/16  03:28:09  mbeesley
 * CSCdi49178:  PAS ethernets have particle leak on errored frames
 * Branch: ELC_branch
 *
 * Revision 3.4.2.10  1996/02/10  00:30:32  mbeesley
 * CSCdi48581:  Add VIP2 support to ELC_branch
 * Branch: ELC_branch
 *
 * Revision 3.4.2.9  1996/01/19  20:03:46  mbeesley
 * Branch: ELC_branch
 * Make "show bridge" more usefull. Remove unused static inline
 * for AMDP2 driver, fix process level packet accounting.
 *
 * Revision 3.4.2.8  1996/01/18  05:41:42  mbeesley
 * Branch: ELC_branch
 * Optimize 1fe and 4e driver. Fix 4e fastsend bug. Patch in Tx enable
 * fix for 1fe. Debug IP fastswitching.
 *
 * Revision 3.4.2.7  1996/01/17  23:37:39  dcarroll
 * Branch: ELC_branch
 * Make 4e fully tx polled.
 *
 * Revision 3.4.2.6  1996/01/16  01:42:25  dcarroll
 * Branch: ELC_branch
 * Sync ELC_branch between ELC_baseline_120995 and ELC_baseline_960110.
 *
 * Revision 3.4.2.5  1995/12/13  04:38:04  dcarroll
 * Branch: ELC_branch
 * Fix 4e fastsend accounting
 *
 * Revision 3.4.2.4  1995/12/11  19:13:40  mbeesley
 * Branch: ELC_branch
 * Sync ELC_branch to ELC_baseline_120995.
 *
 * Revision 3.4.2.3  1995/12/05  06:35:39  mbeesley
 * Branch: ELC_branch
 * Safeguard static paktype headers from being freed.
 *
 * Revision 3.4.2.2  1995/12/04  22:18:06  mbeesley
 * Branch: ELC_branch
 * Perform mini-sync - particle refcount changes.
 *
 * Revision 3.4.2.1  1995/11/22  21:41:41  mbeesley
 * Initial commit of c7100 support.
 * Branch: ELC_branch
 *
 * Revision 3.6  1995/12/19  20:24:13  gstovall
 * CSCdi45859:  Need to remove unused cruft from pas drivers
 * Nuke extra includes, tidy up comments, remove useless code.
 *
 * Revision 3.5  1995/11/28  02:25:55  smackie
 * Move particle refcount into local memory to improve handling speed. Add
 * utility functions to allow some particle internals to be removed from
 * the drivers. (CSCdi44505)
 *
 * Revision 3.4  1995/11/20  04:23:57  mbeesley
 * CSCdi43892:  AMDP2 driver can produce runt frames
 *
 * Revision 3.3  1995/11/17  18:53:49  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:21:01  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:57:25  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:21:08  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/06/07  22:32:48  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Include files
 */
#include "master.h"
#include "sys_registry.h"
#include "fastswitch_registry.h"
#include "interface_private.h"
#include "packet.h"
#include "logger.h"

#include "../os/pool.h"
#include "../os/particle.h"
#include "../os/buffers.h"
#include "../os/buffers_inline.h"

#include "../if/ether.h"
#include "../if/network.h"
#include "../if/ether_inline.h"
#include "../if/linkdown_event.h"
#include "../if/traffic_shape.h"
#include "../ip/ip.h"

#include "../smf/smf_public.h"

#include "if_pas.h"
#include "if_pas_amdp2.h"
#include "msg_amdp2.c"			/* Not a typo, see logger.h */
#include "../if/msg_datalink.c"		/* Not a typo, see logger.h */

/*
 * amdp2_rx_interrupt:
 * Rx interrupt handler for Presidio chip
 */
void amdp2_rx_interrupt (hwidbtype *idb)
{
    amdp2_instance_t *ds = INSTANCE(idb);
    amdp2_rx_desc_t rmd, *rmdptr;
    particletype *mr, *mp;
    paktype *rx_pak;
    int i, pak_size;

    /*
     * Loop thru the rx descriptor ring processing packets
     */
    for (i=0; i<AMDP2_RX_MAX_SPIN; i++) {

	/*
	 * Read the descriptor
	 */
	rmdptr = &ds->rxr[ds->rx_head];
	move64((u64*)rmdptr, (u64*)&rmd);

	if (rmd.status & AMDP2_RMD_OWN) {
	    /*
	     * Presidio still owns this one. Break out of the for
	     * loop down to flushing the data cache.
	     */
	    break;
	}

	/*
	 * Get a replacement particle
	 */
	mr = amdp2_get_particle_inline(idb, TRUE);
	if (!mr) {
	    /*
	     * No replacement. Clean up and give it back to the device.
	     * Also throttle the device as we are running out of resources.
	     */
	    if (rmd.status & AMDP2_RMD_ENP) {
		idb->counters.input_drops++;
		amdp2_set_rx_discard(ds, FALSE);
	    } else {
		amdp2_set_rx_discard(ds, TRUE);
	    }
	    rmdptr->status = AMDP2_RMD_OWN;
	    if (AMDP2_THROTTLING) {
		amdp2_throttle(idb);
	    }
	    AMDP2_ADVANCE_RX_RING_INDEX(ds->rx_head);
	    continue;
	}

	/*
         * Grab the particle associated with this descriptor
	 */
	mp = ds->rxr_shadow[ds->rx_head];

	/*
	 * We have a replacement. Give it back to the rx ring.
	 * Then check for input errors on the received particle.
	 */
	rmdptr->buf_addr = AMDP2_BUF_ADDR(mr->data_start);
	rmdptr->status = AMDP2_RMD_OWN;
	ds->rxr_shadow[ds->rx_head] = mr;
	AMDP2_ADVANCE_RX_RING_INDEX(ds->rx_head);

	if ((rmd.status & AMDP2_RMD_ERR) != 0) {
	    amdp2_rx_error(rmd.status, idb, mp);
	    continue;
	}

	/*
	 * This particle was not in error. However if rx_discard is set,
	 * it is either a particle in an error packet (not the first 
	 * particle), or the first good particle after an errored packet.
	 */
	if (ds->rx_discard) {
	    if (rmd.status & AMDP2_RMD_STP) {
		/*
		 * We had an error on the previous packet, but no
		 * end-of-packet notification. The present
		 * particle is in a good packet. So carry on.
		 */
		ds->rx_no_enp++;
		amdp2_set_rx_discard(ds, FALSE);
	    } else {
		if (rmd.status & AMDP2_RMD_ENP) {
		    /* 
		     * This is the last particle of an errored packet.
		     * So clear rx_discard.
		     */
		    amdp2_set_rx_discard(ds, FALSE);
		}
		retparticle(mp);	/* return this particle */
		continue;
	    }
	}

	/*
	 * So we are here with a good particle, without rx_discard set.
	 * If this particle is the start of a packet, make sure
	 * that there was an end of packet before it. IE catch the condition
	 * where "end of packet" particle got lost.
	 */
	rx_pak = ds->rx_pak;
	if (pak_particle_count(rx_pak) && (rmd.status & AMDP2_RMD_STP)) {
	    ds->rx_no_enp++;
	    amdp2_cleanup_pak(rx_pak);
	}

	/*
	 * If this particle is not the last particle in
	 * a packet, append it to rx_pak and continue.
	 */
	if (!(rmd.status & AMDP2_RMD_ENP)) {
	    mp->data_bytes = AMDP2_PARTICLE_SIZE;
	    particle_enqueue_inline(rx_pak, mp);
	    continue;
	}

	/*
	 * We have a completed packet to process 
	 */
	pak_size = rmd.pkt_size - AMDP2_CRC_SIZE;
	if (rx_pak->datagramsize >= pak_size) {
	    /*
	     * The last particle had just CRC bytes. Return it.
	     */
	    retparticle_inline(mp);
	    
	    /*
	     * And adjust the tail particle
	     */
	    mp = rx_pak->particlequeue.qtail;
	    mp->data_bytes -= (rx_pak->datagramsize - pak_size);
	    rx_pak->datagramsize = pak_size;

	} else {
	    /*
	     * We've got a full packet. Work out the trailing byte count (by
	     * subtracting the amount of data we have so far from the amount
	     * recorded by the controller). 
	     */
	    mp->data_bytes = pak_size - rx_pak->datagramsize;
	    particle_enqueue_inline(rx_pak, mp);
	}

	/*
	 * Range check the packet size
	 */
	if (pak_size > idb->max_pak_size) {
	    idb->counters.giants++;
	    amdp2_cleanup_pak(rx_pak);
	    continue;
	}

	/*
	 * Process the packet
	 */
	amdp2_process_receive_packet(idb, ds, rx_pak, rmd.status);
    }

    /*
     * Flush the Rx ring from the primary data cache
     */
    amdp2_rxring_flush(ds);
}

/*
 * amdp2_pak_to_txring:
 * Put a packet on the tx ring.
 */
static inline void amdp2_pak_to_txring (paktype *pak, 
					amdp2_instance_t *ds,
					hwidbtype *idb,
					boolean scattered)
{
    amdp2_tx_desc_t *tmd;
    int i, old_tx_tail, mcount;
    particletype *mp;
    amdp2_tx_control_t tx_cntl;

    if (!scattered) {

	/*
	 * Contigous packet. Check that we own this descriptor
	 */
	tmd = &ds->txr[ds->tx_tail];		
	if (PAS_DEBUG_BOOL && (tmd->cntl.s.status & AMDP2_TMD_OWN)) {
	    errmsg(&msgsym(OWNERR, AMDP2), idb->slot, idb->subunit, pak);
	}

	/*
	 * Setup the descriptor
	 */
	ds->txr_shadow[ds->tx_tail].pak = pak;
	tmd->buf_addr = AMDP2_BUF_ADDR(pak->datagramstart);
	tx_cntl.buf_size = -(pak->datagramsize); 
	tx_cntl.status = (AMDP2_TMD_OWN|AMDP2_TMD_STP|AMDP2_TMD_ENP);
	tmd->cntl.w = *(ulong *)(&tx_cntl);

	AMDP2_ADVANCE_TX_RING_INDEX(ds->tx_tail);
	ds->tx_count += 1;
	
    } else {
	
 	if (pak_particle_count(pak) == 1) {
 
	    tmd = &ds->txr[ds->tx_tail];
	    
	    if (PAS_DEBUG_BOOL && (tmd->cntl.s.status & AMDP2_TMD_OWN)) {
		errmsg(&msgsym(OWNERR, AMDP2), idb->slot, idb->subunit, pak);
	    }
  
 	    mp = particle_dequeue_inline(pak);
 
	    /*
	     * Give this particle to the ring
	     */
	    tmd->buf_addr = AMDP2_BUF_ADDR(mp->data_start);
  
	    tx_cntl.buf_size = -(mp->data_bytes);
	    tx_cntl.status = (AMDP2_TMD_OWN|AMDP2_TMD_STP|AMDP2_TMD_ENP);
	    tmd->cntl.w = *(ulong *)&tx_cntl;
  
	    ds->txr_shadow[ds->tx_tail].mp = mp;
	    ds->txr_shadow[ds->tx_tail].pak = AMDP2_MAGIC_PAK_PTR;
  
	    AMDP2_ADVANCE_TX_RING_INDEX(ds->tx_tail);
 	    ds->tx_count += 1;
 
 	} else {
 
	    /*
	     * Loop thru setting everything up except ownership
	     */
	    old_tx_tail = ds->tx_tail;
	    mcount = 0;
	
	    while (mp = particle_dequeue_inline(pak), mp) {
	    
		tmd = &ds->txr[ds->tx_tail];		
		if (PAS_DEBUG_BOOL && (tmd->cntl.s.status & AMDP2_TMD_OWN)) {
		    errmsg(&msgsym(OWNERR, AMDP2), 
			   idb->slot, idb->subunit, pak);
		}

		/*
		 * Give this particle to the ring
		 */
		tmd->buf_addr = AMDP2_BUF_ADDR(mp->data_start);
		tmd->cntl.s.buf_size = -(mp->data_bytes);
		ds->txr_shadow[ds->tx_tail].mp = mp;
		AMDP2_ADVANCE_TX_RING_INDEX(ds->tx_tail);
		ds->tx_count += 1;
		mcount += 1;
	    }
	
	    /*
	     * The general case, more than one Tx particle.
	     * Handle end of packet, middle particles and then 
	     * start of packet
	     */
	    old_tx_tail = ds->tx_tail;
	    AMDP2_RETRACT_TX_RING_INDEX(old_tx_tail);
	    tmd = &ds->txr[old_tx_tail];
	    ds->txr_shadow[old_tx_tail].pak = AMDP2_MAGIC_PAK_PTR;
	    tmd->cntl.s.status = (AMDP2_TMD_OWN|AMDP2_TMD_ENP);
	    
	    for (i=1; i<mcount-1; i++) {
		AMDP2_RETRACT_TX_RING_INDEX(old_tx_tail);
		tmd = &ds->txr[old_tx_tail];
		tmd->cntl.s.status = (AMDP2_TMD_OWN);
	    }
	    AMDP2_RETRACT_TX_RING_INDEX(old_tx_tail);
	    tmd = &ds->txr[old_tx_tail];
	    tmd->cntl.s.status = (AMDP2_TMD_OWN|AMDP2_TMD_STP);
	}
    }
}

/*
 * amdp2_safe_start:
 * Worker routine for starting or restarting output. Must be called
 * with interrupts disabled.
 */
static void amdp2_safe_start (hwidbtype *idb)
{
    amdp2_instance_t *ds = INSTANCE(idb);
    int empty_desc, particle_count;
    paktype *pak;

    /*
     * Loop putting packets on the Tx ring
     */
    while (pak = (*idb->oqueue_dequeue)(idb), pak) {
	/*
	 * Sanity check for large packets
	 */
	if (pak->datagramsize > AMDP2_MAX_PACKET) {
	    idb->counters.output_total_drops++;
	    errmsg(&msgsym(TOOBIG, LINK), idb->hw_namestring, 
		   pak->datagramsize);
	    datagram_done(pak);
	    continue;
	}

	/*
	 * Queue it for transmit
	 */
	particle_count = pak_particle_count(pak);
	if (!particle_count) {
	    /*
	     * This is a contigous packet. Thats easy.
	     * First thing to do is make sure there is a place on tx ring.
	     */
	    if ((ds->tx_limited && ds->tx_count) ||
		(ds->tx_count >= AMDP2_TX_RING_ENTRIES)) {
		pak_requeue_head(idb, pak);
		pak = NULL;
		break;
	    }

	    /*
	     * Do packet accounting
	     */
	    if (AMDP2_PKT_STATS) {
		idb->counters.tx_cumbytes += pak->datagramsize;
		idb->counters.outputs += 1;
		if (*(pak->datagramstart) & 0x01) {
		    idb->counters.output_broad++;
		}
		TIMER_START(idb->xmittimer, TIMEOUT);
	    }

	    /*
	     * Give this packet to the txring
	     */
	    amdp2_pak_to_txring(pak, ds, idb, FALSE);

	} else {

	    /*
	     * It is a scattered packet. Make sure there is room 
	     * for all particles on the Tx ring.
	     */
	    empty_desc = AMDP2_TX_RING_ENTRIES - ds->tx_count;
	    if ((ds->tx_limited && ds->tx_count) ||
		(particle_count >= empty_desc)) {
		pak_requeue_head(idb, pak);
		pak = NULL;
		break;
	    }

	    /*
	     * Do packet accounting
	     */
	    if (AMDP2_PKT_STATS) {
		idb->counters.tx_cumbytes += pak->datagramsize;
		idb->counters.outputs += 1;
		if (*(pak->datagramstart) & 0x01) {
		    idb->counters.output_broad++;
		}
		TIMER_START(idb->xmittimer, TIMEOUT);
	    }

	    /*
	     * Give this packet to the txring
	     */
	    amdp2_pak_to_txring(pak, ds, idb, TRUE);

	    /*
	     * Give back the pakheader
	     */
	    datagram_done(pak);
	}
    }
}

/*
 * amdp2_tx_interrupt :
 * Process and reclaim all buffers that have been transmitted
 */
void amdp2_tx_interrupt (hwidbtype *idb)
{
    amdp2_instance_t *ds = INSTANCE(idb);
    amdp2_tx_shadow_t *shadow_ptr;
    amdp2_tx_desc_t *tmd;
    particletype *mp;
    paktype *pak;
    int i;
    ushort tmd_status;
    
    /*
     * Reclaim transmitted buffers
     */
    for (i=0; i<AMDP2_TX_MAX_SPIN; i++) {

	/*
	 * Read the tx shadow ring
	 */
	shadow_ptr = &ds->txr_shadow[ds->tx_head];
	mp = shadow_ptr->mp;
	pak = shadow_ptr->pak;

	/*
	 * Process completed descriptors
	 */
	if (mp || pak) {

	    /*
	     * We have a buffer here
	     */
	    tmd = &ds->txr[ds->tx_head];
	    tmd_status = tmd->cntl.s.status;
	    if (tmd_status & AMDP2_TMD_OWN) {
		/*
		 * But the presidio still owns it.
		 */
		break;
	    }

	    /*
	     * Check for errors
	     */
	    if (tmd_status & AMDP2_TMD_ERR_SUMMARY) {
		amdp2_tx_error(tmd_status, ds->tx_head, idb);
	    } else if (!idb->ethernet_lineup) {
	    	idb->ethernet_lineup = TRUE;
	    	idb->reason = "Keepalive OK";
		amdp2_lineproto_change(idb, TRUE);
		amdp2_snmp_link_trap(AMDP2_LINK_UP, idb);
	    }

	    /*
	     * Advance the head pointer
	     */
	    AMDP2_ADVANCE_TX_RING_INDEX(ds->tx_head);
	    
	    /*
	     * Clear the shadow
	     */
	    shadow_ptr->mp = NULL;
	    shadow_ptr->pak = NULL;

	    /*
	     * Do packet Tx accounting and clean up
	     */
	    if (mp) {
		/*
		 * If pak is also set, then this is a particle that was
		 * the last in an outgoing packet.
		 */
		if (AMDP2_PKT_STATS && pak) {
		    GET_TIMESTAMP(idb->lastoutput);
		}
		ds->tx_count--;
		retparticle_inline(mp);
	    } else {
		datagram_done(pak);
		if (AMDP2_PKT_STATS) {
		    GET_TIMESTAMP(idb->lastoutput);
		}
		ds->tx_count--;
	    }
	} else {
	    /*
	     * No more, quit now
	     */
	    break;
	}
    }

    /*
     * Attempt to recover from any fatal Tx errors we noticed
     */
    if (ds->fatal_tx_err) {
	if (ether_debug) {
	    buginf("AMDP2(%d): Fatal transmit error on %s. Resetting\n",
		   idb->unit, idb->hw_namestring);
	}
	amdp2_init(idb, FALSE);
    }

    /*
     * And finally possibly restart output
     */
    if (idb->output_qcount) {
	amdp2_safe_start(idb);
    }
}

/*
 * amdp2_tx_start : 
 * Start or restart output. Raise interrupt level and call amdp2_safe_start
 */
void amdp2_tx_start (hwidbtype *idb)
{
    leveltype level;

    level = raise_interrupt_level(NETS_DISABLE);
    amdp2_safe_start(idb);
    reset_interrupt_level(level);
}

/*
 * amdp2_fastsend:
 * Transmit this packet immediately or enqueue it on the hold q.
 */
void amdp2_fastsend (hwidbtype *idb, paktype *pak)
{
    amdp2_instance_t *ds = INSTANCE(idb);
    int empty_desc, particle_count;
    traffic_shape_t *shape;
    
    if (idb->state != IDBS_UP) {
	idb->counters.output_total_drops++;
	datagram_done(pak);
	return;
    }

    if (TRAFFIC_SHAPING(pak, shape)) {
	/*
	 * determine whether there is a window available to send 
	 * the message within the rate control bucket
	 */
        if (pak_has_particles(pak)) {

	    paktype *newpak;

	    /*
	     * Reparent onto a new paktype header as we can not hold onto
	     * the one passed into us.
	     */
	    newpak = particle_reparent(pak, idb->headerpool);
	    if (!newpak) {
		idb->counters.output_total_drops++;
	        amdp2_cleanup_pak(pak);
		return;
	    }
	    /*
	     * Transfer necessary fields from old to new pak.
	     * Then flush old pak for reuse and take over new pak.
	     */
	    pas_stuff_pak_for_holdq(newpak, pak);
	    amdp2_flush_pak(pak);
	    pak = newpak;
	}
	traffic_shape_defer_sending(shape, pak);
	return;
    }

    /*
     * If there are packets on the holdq queue, or we are limited
     * to one packet at a time on the tx ring and the tx ring is not
     * empty, queue the packet on the output hold queue.
     */
    if (idb->output_qcount ||
	(ds->tx_limited && ds->tx_count)) {
	/*
	 * Since device drivers may be passing statically allocated
	 * paktypes thru fast switching code, the paktype that we have here
	 * needs to be duplicated.
	 */
	amdp2_output_enqueue(pak, idb);

	/*
	 * If the Tx ring is full, then try and clean some stuff up
	 * by calling amdp2_tx_interrupt (which calls amdp2_safe_start()).
	 * Otherwise just call amdp2_safe_start() directly.
	 */
	if (ds->tx_count >= AMDP2_TX_RING_ENTRIES) {
	    amdp2_tx_interrupt(idb);
	} else {
	    amdp2_safe_start(idb);
	}
	return;
    }

    /*
     * Holdq is empty. so try and transmit it directly.
     */
    particle_count = pak_particle_count(pak);
    if (!particle_count) {
	/*
	 * It is a contigous packet. This is easy. See if there is
	 * room on the ring. If not, enqueue it.
	 */
	if (ds->tx_count >= AMDP2_TX_RING_ENTRIES) {
	    /*
	     * Since device drivers may be passing statically allocated
	     * paktypes thru fast switching code, the paktype that we have here
	     * needs to be duplicated.
	     */
	    amdp2_output_enqueue(pak, idb);
	    return;
	}
	amdp2_pak_to_txring(pak, ds, idb, FALSE);

    } else {

	/*
	 * The packet is scattered. Make sure there is room for it on the
	 * Tx ring. If not, queue it.
	 */
	empty_desc = AMDP2_TX_RING_ENTRIES - ds->tx_count;
	if (particle_count >= empty_desc) {
	    /*
	     * Since device drivers may be passing statically allocated
	     * paktypes thru fast switching code, the paktype that we have
	     * here needs to be duplicated.
	     */
	    amdp2_output_enqueue(pak, idb);
	    return;
	}
	amdp2_pak_to_txring(pak, ds, idb, TRUE);
    }

    /*
     * Do fastsend accounting
     */
    if (AMDP2_PKT_STATS) {
	ulong acct_proto = (ulong)pak->acct_proto;
	
	idb->counters.tx_cumbytes_fast[acct_proto] += pak->datagramsize;
	idb->counters.outputs_fast[acct_proto] += 1;
	TIMER_START(idb->xmittimer, TIMEOUT);
	if (*(pak->datagramstart) & 0x01) {
	    idb->counters.output_broad++;
	}
    }
}

/* end of file */
