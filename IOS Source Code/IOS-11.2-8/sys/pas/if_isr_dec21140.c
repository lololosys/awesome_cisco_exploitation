/* $Id: if_isr_dec21140.c,v 3.8.10.14 1996/09/01 21:20:24 fowler Exp $
 * $Source: /release/112/cvs/Xsys/pas/if_isr_dec21140.c,v $
 *------------------------------------------------------------------
 * if_isr_dec21140.c - DEC 100e PA Interrupt service routines
 *		       (Platform independent)
 *
 * April 1995, Steve Jacobson
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: if_isr_dec21140.c,v $
 * Revision 3.8.10.14  1996/09/01  21:20:24  fowler
 * CSCdi60734:  Frame Relay rate enforcement is not effective
 *              Remove original fix due to its performance impact
 * Branch: California_branch
 *
 * Revision 3.8.10.13  1996/08/30  16:48:18  fowler
 * CSCdi60734:  Frame Relay rate enforcement is not effective
 *              Add registry to check for the need for media dependent
 *              traffic shaping
 * Branch: California_branch
 *
 * Revision 3.8.10.12  1996/08/29  07:31:38  abaindur
 * CSCdi67514:  Spurious accesses while local switching on VIP
 * Branch: California_branch
 *  pak->if_output was not being set for the traffic shaping check.
 *  Bypass traffic shaping on the VIP for now.
 *
 * Revision 3.8.10.11  1996/08/28  13:05:56  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.8.10.10  1996/08/26  18:23:34  feisal
 * CSCdi45144:  4E/1FE error handling cleanup
 * Branch: California_branch
 *
 * Revision 3.8.10.9  1996/08/24  01:51:00  abaindur
 * CSCdi66979:  Traffic shaping broken on the c7200
 * Branch: California_branch
 *
 * Revision 3.8.10.8  1996/08/13  02:21:15  gstovall
 * CSCdi39484:  Giant packets output on ethernet interface
 * Branch: California_branch
 * Make sure we drop giants, even if they are just a wee bit too big.
 *
 * Revision 3.8.10.7  1996/06/02  01:35:36  feisal
 * CSCdi55143:  Fast-Ethernet cable removal is not detected
 * Branch: California_branch
 *
 * Revision 3.8.10.6  1996/05/31  22:33:36  feisal
 * CSCdi56695:  ISL Broken on certain card
 * Branch: California_branch
 *
 * Revision 3.8.10.5  1996/05/13  22:52:55  mbeesley
 * CSCdi57397:  DEC21140 interrupt setting incorrect
 * Branch: California_branch
 *
 * Revision 3.8.10.4  1996/05/07  19:24:00  mbeesley
 * CSCdi56937:  Update DEC21140 errmsgs to include slot/subunit
 * Branch: California_branch
 *
 * Revision 3.8.10.3  1996/03/29  00:45:11  sjacobso
 * CSCdi45152:  PERFORMANCE: 1FE runs in promiscuous mode instead of
 * filtering
 * Branch: California_branch
 * Add code to allow dec21140 chip to run in perfect
 * address filtering mode whenever possible.  Modify
 * address filter write code to record information
 * about address filter entries that may affect perfect
 * filter operation.  Interface VIP-specific software
 * address filter to RVIP/SVIP/Predator common routines.
 *
 * Revision 3.8.10.2  1996/03/21  23:23:01  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.8.10.1  1996/03/18  21:34:22  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.2.12  1996/03/18  23:28:36  mbeesley
 * Branch: ELC_branch
 * Sync to V111_1_3
 *
 * Revision 3.3.2.11  1996/03/06  18:22:52  getchell
 * CSCdi50848:  single particle optimization
 * Branch: ELC_branch
 *
 * Revision 3.3.2.10  1996/02/16  03:28:12  mbeesley
 * CSCdi49178:  PAS ethernets have particle leak on errored frames
 * Branch: ELC_branch
 *
 * Revision 3.3.2.9  1996/02/10  00:30:34  mbeesley
 * CSCdi48581:  Add VIP2 support to ELC_branch
 * Branch: ELC_branch
 *
 * Revision 3.3.2.8  1996/02/01  07:54:44  mbeesley
 * CSCdi47969:  Allow coalesced fastswitching on ethernet
 *         o Let 4e/1fe driver coalesce frames before fs
 *         o Speed up 1fe tx particle reclaimation
 *         o Remove min length check on 1fe (chip pads)
 *         o Put 1fe rx particles on a more PCI bus friendly alignment
 * Branch: ELC_branch
 *
 * Revision 3.3.2.7  1996/01/29  19:04:26  wfried
 * Branch: ELC_branch
 * Use HW address filtering, disable rx side for better input throttling,
 * add loopback code, correct ignored frames counting, allow only MII media
 * on the Predator I/O card FE interface.
 *
 * Revision 3.3.2.6  1996/01/18  05:41:44  mbeesley
 * Branch: ELC_branch
 * Optimize 1fe and 4e driver. Fix 4e fastsend bug. Patch in Tx enable
 * fix for 1fe. Debug IP fastswitching.
 *
 * Revision 3.3.2.5  1996/01/16  01:42:28  dcarroll
 * Branch: ELC_branch
 * Sync ELC_branch between ELC_baseline_120995 and ELC_baseline_960110.
 *
 * Revision 3.3.2.4  1995/12/11  19:13:41  mbeesley
 * Branch: ELC_branch
 * Sync ELC_branch to ELC_baseline_120995.
 *
 * Revision 3.3.2.3  1995/12/05  06:35:42  mbeesley
 * Branch: ELC_branch
 * Safeguard static paktype headers from being freed.
 *
 * Revision 3.3.2.2  1995/12/04  22:18:10  mbeesley
 * Branch: ELC_branch
 * Perform mini-sync - particle refcount changes.
 *
 * Revision 3.3.2.1  1995/11/22  21:41:44  mbeesley
 * Initial commit of c7100 support.
 * Branch: ELC_branch
 *
 * Revision 3.8  1996/02/26  22:06:49  feisal
 * CSCdi49919:  sticky PAK_IS_ISL bit on paktypes on VIP/1FE
 *
 * Revision 3.7  1996/02/23  20:02:12  feisal
 * CSCdi45142:  Support VLAN ISL for new 1FE port adapter
 *
 * Revision 3.6  1995/12/19  20:24:15  gstovall
 * CSCdi45859:  Need to remove unused cruft from pas drivers
 * Nuke extra includes, tidy up comments, remove useless code.
 *
 * Revision 3.5  1995/11/28  03:35:11  getchell
 * CSCdi43281:  rVIP + FE adds xtra 4 Bytes to datagrams, sometimes, under
 * load
 *
 * Revision 3.4  1995/11/28  02:25:57  smackie
 * Move particle refcount into local memory to improve handling speed. Add
 * utility functions to allow some particle internals to be removed from
 * the drivers. (CSCdi44505)
 *
 * Revision 3.3  1995/11/17  18:53:53  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:21:03  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 * Revision 1.1.2.15  1995/11/21  00:37:52  getchell
 * Remove software padding operation for safe_send packets and increase
 * ring size
 * CSCdi43281:  rVIP + FE adds xtra 4 Bytes to datagrams, sometimes, under
 * load
 * Branch: VIP_branch
 *
 * Revision 1.1.2.14  1995/10/06  04:00:47  mbeesley
 * CSCdi41406:  rVIP Dies when a FIP gives it 70Kpps inside a 7K.
 * Fix brain dead null pointers in handling output hold queue.
 * Branch: VIP_branch
 *
 * Revision 3.1  1995/11/09  12:57:27  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:21:10  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/06/07  22:32:50  hampton
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

#include "../if/network.h"
#include "../if/ether.h"
#include "../if/ether_inline.h"
#include "../if/traffic_shape.h"
#include "../ip/ip.h"

#include "../snmp/snmp_api.h"
#include "../snmp/sr_old_lchassismib.h"
#include "../snmp/snmp_registry.h"

#include "../src-vip/vip_isl.h"

#include "if_pas.h"
#include "if_pas_dec21140.h"
#include "msg_dec21140.c"		/* Not a typo, see logger.h */
#include "../if/msg_datalink.c"		/* Not a typo, see logger.h */
#include "pas_id.h"

/*
 * dec21140_rx_interrupt:  
 * Handle DEC21140 RX interrupts.
 */
void dec21140_rx_interrupt (hwidbtype *idb)
{
    dec21140_instance_t *ds = INSTANCE(idb);
    dec21140_rx_desc_t *rmd;
    paktype *rx_pak = ds->rx_pak;
    particletype *mr, *mp;
    ulong rmd_status;
    int i, pak_size;

    /*
     * Loop thru the rx descriptor ring processing packets
     */
    for (i=0; i<DEC21140_RX_MAX_SPIN; i++) {

	/*
	 * Read the descriptor
	 */
	rmd = &ds->rxr[ds->rx_head];
	rmd_status = rmd->rx_status;

	if (rmd_status & DEC21140_RMD_OWN) {
	    /*
	     * DEC21140 still owns this one. Break out of the for
	     * loop down to flushing the data cache.
	     */
	    break;
	}

	/*
	 * Get a replacement particle
	 */
	mr = dec21140_get_particle_inline(idb, TRUE);
	if (!mr) {
	    /*
	     * No replacement. Clean up and give it back to the device.
	     * Also throttle the device as we are running out of resources.
	     */
	    if (rmd_status & DEC21140_RMD_ENP) {
		idb->counters.input_drops++;
		dec21140_set_rx_discard(ds, FALSE);
	    } else {
		dec21140_set_rx_discard(ds, TRUE);
	    }
	    rmd->rx_status = DEC21140_RMD_OWN;
	    if (DEC21140_THROTTLING) {
		dec21140_throttle(idb);
	    }
	    DEC21140_ADVANCE_RX_RING_INDEX(ds->rx_head);
	    continue;
	}

	/*
	 * Grab the particle associated with this first descriptor
	 */
	mp = ds->rxr_shadow[ds->rx_head];

	/*
	 * We have a replacement. Give it back to the rx ring.
	 * Then check for input errors on the particle received.
	 */
	rmd->buf_addr = DEC21140_BUF_ADDR(mr->data_start);
	rmd->rx_status = DEC21140_RMD_OWN;
	ds->rxr_shadow[ds->rx_head] = mr;
	DEC21140_ADVANCE_RX_RING_INDEX(ds->rx_head);

	if (((rmd_status & DEC21140_RMD_ERROR_MASK) != 0) &&
	    (rmd_status & DEC21140_RMD_ENP)) {

	    /*
	     * If ISL is not enabled process the error
	     */
	    if (!ds->pacb.isl_enabled) {
		dec21140_rx_error(rmd_status, idb, mp);
		continue;
	    }

	    /*
	     * If it was not a giant, or it was a giant and not
	     * an ISL frame, process the error
	     */
	    if (!(rmd_status & DEC21140_RMD_FRAME_TOO_LONG_ERROR) ||
		((rmd_status & DEC21140_RMD_FRAME_TOO_LONG_ERROR) &&
		 !vip_pak_is_isl(rx_pak, mp, ISL_PARTIAL))) {
		dec21140_rx_error(rmd_status, idb, mp);
		continue;
	    }

	    /*
	     * Its an ISL frame which could have other errors like CRC 
	     * errors, deal with them here
	     */
	    rmd_status &= ~DEC21140_RMD_FRAME_TOO_LONG_ERROR;
	    if ((rmd_status & DEC21140_RMD_ERROR_MASK) != 0) {
		dec21140_rx_error(rmd_status, idb, mp);
		continue;
	    }
	}

	/*
	 * This particle was not in error. However if rx_discard is set,
	 * it is either a particle in an error packet, (not the first
	 * particle), or the first good particle after an errored packet.
	 */
	if (ds->rx_discard) {
	    if (rmd_status & DEC21140_RMD_STP) {
		/*
		 * We had an error on the previous packet, but no
		 * end-of-packet notification. The present
		 * particle is in a good packet. So carry on.
		 */
		ds->rx_no_enp++;
		dec21140_set_rx_discard(ds, FALSE);
	    } else {
		if (rmd_status & DEC21140_RMD_ENP) {
		    /* 
		     * This is the last particle of an errored packet.
		     * So clear rx_discard.
		     */
		    dec21140_set_rx_discard(ds, FALSE);
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
	if (pak_particle_count(rx_pak) && (rmd_status & DEC21140_RMD_STP)) {
	    ds->rx_no_enp++;
	    dec21140_cleanup_pak(rx_pak);
	}

	/*
	 * If this particle is not the last particle in
	 * a packet, append it to rx_pak and continue.
	 */
	if (!(rmd_status & DEC21140_RMD_ENP)) {
	    mp->data_bytes = DEC21140_PARTICLE_SIZE;
	    particle_enqueue_inline(rx_pak, mp);
	    continue;
	}

	/*
	 * We have a completed packet to process. Strip off exterior CRC
	 */
	pak_size = ((rmd_status & DEC21140_RMD_RX_COUNT_MASK) >> 16) - 
	    DEC21140_CRC_SIZE;

	/*
	 * If ISL is enabled, and this is an ISL frame, strip off inner CRC
	 * and range check the packet.
	 */
	rx_pak->flags = 0;
	if (ds->pacb.isl_enabled) {
	    if (vip_pak_is_isl(rx_pak, mp, ISL_PARTIAL)) {
		rx_pak->flags |= PAK_IS_ISL;
		pak_size -= ISL_INNER_CRC_LEN;
		if (pak_size > (idb->max_pak_size + ISL_ENCAPS_FRONT)) {
		    idb->counters.giants++;
		    dec21140_cleanup_pak(rx_pak);  
		    retparticle(mp);
		    continue;
		}
	    }
	} else {
	    if (pak_size > idb->max_pak_size) {
		idb->counters.giants++;
		dec21140_cleanup_pak(rx_pak);
		retparticle(mp);
		continue;
	    }
	}

	/*
	 * Enuqueue last particle as long as it is not just CRC bytes
	 */
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
	 * Process the packet
	 */
	dec21140_process_receive_packet(idb, ds, rx_pak, rmd_status);
    }

    /*
     * Flush the Rx ring from the primary data cache
     */
    dec21140_rxring_flush(ds);
}

/*
 * dec21140_adjust_isl_frame:
 * Adjust an ISL frame to be transmitted
 */
static inline void dec21140_adjust_isl_frame (paktype *pak)
{
    ushort *ptr, length_delta;
    particletype *mp;

    /* 
     * Clear the pak->flags 
     */
    pak->flags = 0;

    /* 
     * Adjust the ISL frame. Rewrite last two bytes of DA to kick start
     * the ISL/MICE hardware
     */
    ptr = (ushort *)(pak->datagramstart + MICE_ISL_CRC_MASK_OFFSET);
    PUTSHORT(ptr, GETSHORT(ptr) | MICE_ISL_CRC_MASK);

    /*
     * Adjust the length in the ISL encapsulation to account
     * for the inner packet's CRC. Make sure that the inner
     * datagram is at least equal to the min. ethernet frame length (60)
     */
    ptr = (ushort *)(pak->datagramstart + ISL_ENCAPS_LEN_OFFSET);
    if (pak->datagramsize < ISL_ENCAPS_FRONT_AND_INNER_MIN) {
	PUTSHORT(ptr, ISL_ENCAPS_FRONT_AND_INNER_MIN - ISL_ENCAPS_DA_LEN 
		 - ISL_ENCAPS_SA_LEN - ISL_ENCAPS_LEN_LEN 
		 + ISL_INNER_CRC_LEN);
	length_delta = ISL_ENCAPS_FRONT_AND_INNER_MIN 
	    - pak->datagramsize + ISL_INNER_CRC_LEN;
	pak->datagramsize += length_delta;
	if (pak_has_particles(pak)) {
	    mp = pak->particlequeue.qtail;
	    mp->data_bytes += length_delta;
	}
    } else {
	PUTSHORT(ptr, pak->datagramsize - ISL_ENCAPS_DA_LEN 
		 - ISL_ENCAPS_SA_LEN - ISL_ENCAPS_LEN_LEN 
		 + ISL_INNER_CRC_LEN);
	pak->datagramsize += ISL_INNER_CRC_LEN;
	if (pak_has_particles(pak)) {
	    mp = pak->particlequeue.qtail;
	    mp->data_bytes += ISL_INNER_CRC_LEN;
	}
    }
}

/*
 * dec21140_pak_to_txring:
 * Give a packet to the tx ring. Check for scattered compiles out.
 */
static inline void dec21140_pak_to_txring (paktype *pak,
					   dec21140_instance_t *ds,
					   hwidbtype *idb,
					   boolean scattered)
{
    dec21140_tx_desc_t *tmd, tx_desc;
    int i, old_tx_tail, mcount;
    particletype *mp;
    ulong first_cl, last_cl;

    if (ds->pacb.isl_enabled && (pak->flags & PAK_IS_ISL)) {
	dec21140_adjust_isl_frame(pak);
    }

    if (!scattered) {

	/*
	 * Contigous packet. Check that we own this descriptor
	 */
	tmd = &ds->txr[ds->tx_tail];		
	if (PAS_DEBUG_BOOL && (tmd->tx_status & DEC21140_TMD_OWN)) {
	    errmsg(&msgsym(OWNERR, DEC21140), idb->slot, idb->subunit, pak);
	}

	/*
	 * Setup the descriptor
	 */
	ds->txr_shadow[ds->tx_tail].pak = pak;
	tmd->buf_addr = DEC21140_BUF_ADDR(pak->datagramstart);
	tmd->control_length = dec21140_tx_cl(pak->datagramsize, 
					     DEC21140_STARTEND);
	tmd->tx_status = DEC21140_TMD_OWN;
	DEC21140_ADVANCE_TX_RING_INDEX(ds->tx_tail);
	ds->tx_count += 1;
	
    } else {

 	if (pak_particle_count(pak) == 1) {
 
	    tmd = &ds->txr[ds->tx_tail];
	    if (PAS_DEBUG_BOOL && (tmd->tx_status & DEC21140_TMD_OWN)) {
		errmsg(&msgsym(OWNERR, DEC21140), 
		       idb->slot, idb->subunit, pak);
	    }
 	    mp = particle_dequeue_inline(pak);
 
	    /*
	     * Give this particle to the ring
	     */
	    tmd->buf_addr = DEC21140_BUF_ADDR(mp->data_start);
	    tx_desc.control_length =  dec21140_tx_cl(mp->data_bytes, 
						     DEC21140_STARTEND);
	    tx_desc.tx_status =  DEC21140_TMD_OWN;
  
	    move64((u64*)&tx_desc, (u64*)tmd);
  
	    ds->txr_shadow[ds->tx_tail].mp = mp;
	    ds->txr_shadow[ds->tx_tail].pak = DEC21140_MAGIC_PAK_PTR;
  
	    DEC21140_ADVANCE_TX_RING_INDEX(ds->tx_tail);
 	    ds->tx_count += 1;
 
 	} else {
 
	    /*
	     * Loop thru setting everything up except ownership
	     */
	    old_tx_tail = ds->tx_tail;
	    mcount = first_cl = last_cl = 0;
	
	    while (mp = particle_dequeue_inline(pak), mp) {
	    
		tmd = &ds->txr[ds->tx_tail];		
		if (PAS_DEBUG_BOOL && (tmd->tx_status & DEC21140_TMD_OWN)) {
		    errmsg(&msgsym(OWNERR, DEC21140), 
			   idb->slot, idb->subunit, pak);
		}
	    
		/*
		 * Give this particle to the ring
		 */
		tmd->buf_addr = DEC21140_BUF_ADDR(mp->data_start);
		if (mcount == 0) {
		    first_cl = dec21140_tx_cl(mp->data_bytes, DEC21140_START);
		} else {
		    last_cl = dec21140_tx_cl(mp->data_bytes, DEC21140_MIDDLE);
		    tmd->control_length = last_cl;
		}
		ds->txr_shadow[ds->tx_tail].mp = mp;
		DEC21140_ADVANCE_TX_RING_INDEX(ds->tx_tail);
		ds->tx_count += 1;
		mcount += 1;
	    }

	    /*
	     * The general case, more than one Tx particles.
	     * Handle end of packet, middle particles and then 
	     * start of packet
	     */
	    old_tx_tail = ds->tx_tail;
	    DEC21140_RETRACT_TX_RING_INDEX(old_tx_tail);
	    tmd = &ds->txr[old_tx_tail];
	    ds->txr_shadow[old_tx_tail].pak = DEC21140_MAGIC_PAK_PTR;
	    tmd->control_length = last_cl | (DEC21140_TMD_COMPLETION_INT |
					     DEC21140_TMD_ENP);
	    tmd->tx_status = DEC21140_TMD_OWN;
	    
	    for (i=1; i<mcount-1; i++) {
		DEC21140_RETRACT_TX_RING_INDEX(old_tx_tail);
		tmd = &ds->txr[old_tx_tail];
		tmd->tx_status = DEC21140_TMD_OWN;
	    }
	    DEC21140_RETRACT_TX_RING_INDEX(old_tx_tail);
	    tmd = &ds->txr[old_tx_tail];
	    tmd->control_length = first_cl;
	    tmd->tx_status = DEC21140_TMD_OWN;
	}
    }
}

/*
 * dec21140_safe_start:
 * Worker routine for starting or restarting output. Must be called
 * with interrupts disabled.
 */
static void dec21140_safe_start (hwidbtype *idb)
{
    dec21140_instance_t *ds = INSTANCE(idb);
    int empty_desc, particle_count;
    paktype *pak;

    /*
     * Loop putting packets on the Tx ring
     */
    while ((pak = (*idb->oqueue_dequeue)(idb))) {
	/*
	 * Sanity check for large packets
	 */
	if (pak->datagramsize > (DEC21140_MAX_PACKET + ISL_ENCAPS_FRONT)) {
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
	     */
	    if ((ds->tx_limited && ds->tx_count) ||
		(ds->tx_count >= DEC21140_TX_RING_ENTRIES)) {
		pak_requeue_head(idb, pak);
		pak = NULL;
		break;
	    }
	    
	    /*
	     * Do packet accounting
	     */
	    if (DEC21140_PKT_STATS) {
		idb->counters.tx_cumbytes += pak->datagramsize;
		TIMER_START(idb->xmittimer, TIMEOUT);
		idb->counters.outputs += 1;
		if (*(pak->datagramstart) & 0x01) {
		    idb->counters.output_broad++;
		}
	    }

	    /*
	     * Give this packet to the tx ring
	     */
	    dec21140_pak_to_txring(pak, ds, idb, FALSE);

	} else {

	    /*
	     * It is a scattered packet. Make sure there is room for all 
	     * particles on the Tx ring.
	     */
	    empty_desc = DEC21140_TX_RING_ENTRIES - ds->tx_count;
	    if ((ds->tx_limited && ds->tx_count) ||
		(particle_count >= empty_desc)) {
		pak_requeue_head(idb, pak);
		pak = NULL;
		break;
	    }
	    
	    /*
	     * Do packet accounting
	     */
	    if (DEC21140_PKT_STATS) {
		idb->counters.tx_cumbytes += pak->datagramsize;
		idb->counters.outputs += 1;
		if (*(pak->datagramstart) & 0x01) {
		    idb->counters.output_broad++;
		}
		TIMER_START(idb->xmittimer, TIMEOUT);
	    }

	    /*
	     * Give this packet to the tx ring
	     */
	    dec21140_pak_to_txring(pak, ds, idb, TRUE);

	    /*
	     * Give back the header
	     */
	    datagram_done(pak);
	}
    }
}

/*
 * dec21140_tx_interrupt:  
 * Handle DEC21140 transmit interrupts.
 */
void dec21140_tx_interrupt (hwidbtype *idb)
{
    dec21140_instance_t *ds = INSTANCE(idb);
    dec21140_tx_shadow_t *shadow_ptr;
    dec21140_tx_desc_t *tmd;
    particletype *mp;
    paktype *pak;
    int i;
    ulong tmd_status;

    /*
     * Reclaim transmitted buffers
     */
    for (i=0; i<DEC21140_TX_MAX_SPIN; i++) {

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
	    tmd_status = tmd->tx_status;
	    if (tmd_status & DEC21140_TMD_OWN) {
		/*
		 * But the DEC21140 still owns it.
		 */
		break;
	    }

	    /*
	     * Advance the head pointer
	     */
	    DEC21140_ADVANCE_TX_RING_INDEX(ds->tx_head);
	    
	    /*
	     * Check for errors. When we transmit a contigous frame,
	     * pak is non-null and mp is null. When we transmit 
	     * a scattered frame, mp is non-null and pak is null,
	     * except when it is the last particle in a frame, when
	     * pak gets set to DEC21140_MAGIC_PAK_PTR. So as long as pak is
	     * non-null, we can check for tx errors.
	     */
	    if (pak) {
		if (tmd_status & DEC21140_TMD_ERR_SUMMARY) {
		    dec21140_tx_error(tmd_status, idb);
		} else if (!ds->phy_link_active) {
		    dec21140_link_down(idb);
		} else if (!idb->ethernet_lineup) {
		    idb->ethernet_lineup = TRUE;
		    idb->reason = "Keepalive OK";
		    dec21140_lineproto_change(idb, TRUE);
		    dec21140_snmp_link_trap(DEC21140_LINK_UP, idb);
		}
	    }
	    
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
		if (DEC21140_PKT_STATS && pak) {
		    GET_TIMESTAMP(idb->lastoutput);
		}
		ds->tx_count--;
		retparticle_inline(mp);
	    } else {
		datagram_done(pak);
		if (DEC21140_PKT_STATS) {
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
	    buginf("DEC21140: Fatal transmit error on %s. Resetting\n",
		   idb->hw_namestring);
	}
	dec21140_init(idb, FALSE);
    }

    /*
     * And finally possibly restart output
     */
    if (idb->output_qcount) {
	dec21140_safe_start(idb);
    }
}

/*
 * dec21140_tx_start : 
 * Start or restart output. Raise interrupt level and call dec21140_safe_start
 */
void dec21140_tx_start (hwidbtype *idb)
{
    leveltype level;

    level = raise_interrupt_level(NETS_DISABLE);
    dec21140_safe_start(idb);
    reset_interrupt_level(level);
}

/*
 * dec21140_fastsend:
 * Transmit this packet immediately or enqueue it on the hold q.
 */
void dec21140_fastsend (hwidbtype *idb, paktype *pak)
{
    dec21140_instance_t *ds = INSTANCE(idb);
    int empty_desc, particle_count;
    traffic_shape_t *shape;

    if (idb->state != IDBS_UP) {
	idb->counters.output_total_drops++;
	datagram_done(pak);
	return;
    }

    dec21140_snoop_txisl_frames(&ds->pacb, pak);

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
	        dec21140_cleanup_pak(pak);
		return;
	    }
	    /*
	     * Transfer necessary fields from old to new pak.
	     * Then flush old pak for reuse and take over new pak.
	     */
	    pas_stuff_pak_for_holdq(newpak, pak);
	    dec21140_flush_pak(pak);
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
	dec21140_output_enqueue(pak, idb);

	/*
	 * If the Tx ring is full, then try and clean some stuff up
	 * by calling dec21140_tx_interrupt (which calls 
	 * dec21140_safe_start()).Otherwise just call dec21140_safe_start() 
	 * directly.
	 */
	if (ds->tx_count >= DEC21140_TX_RING_ENTRIES) {
	    dec21140_tx_interrupt(idb);
	} else {
	    dec21140_safe_start(idb);
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
	if (ds->tx_count >= DEC21140_TX_RING_ENTRIES) {
	    /*
	     * Since device drivers may be passing statically allocated
	     * paktypes thru fast switching code, the paktype that we have here
	     * needs to be duplicated.
	     */
	    dec21140_output_enqueue(pak, idb);
	    return;
	}
	dec21140_pak_to_txring(pak, ds, idb, FALSE);

    } else {

	/*
	 * The packet is scattered. Make sure there is room for it on the
	 * Tx ring. If not, queue it.
	 */
	empty_desc = DEC21140_TX_RING_ENTRIES - ds->tx_count;
	if (particle_count >= empty_desc) {
	    /*
	     * Since device drivers may be passing statically allocated
	     * paktypes thru fast switching code, the paktype that we have here
	     * needs to be duplicated.
	     */
	    dec21140_output_enqueue(pak, idb);
	    return;
	}
	dec21140_pak_to_txring(pak, ds, idb, TRUE);
    }
	
    /*
     * Do packet accounting
     */
    if (DEC21140_PKT_STATS) {
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
      
