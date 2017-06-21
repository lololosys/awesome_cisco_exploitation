/* $Id: if_isr_s4t68360.c,v 3.16.4.13 1996/08/29 07:31:46 abaindur Exp $
 * $Source: /release/112/cvs/Xsys/pas/if_isr_s4t68360.c,v $
 *------------------------------------------------------------------
 * if_isr_s4t68360.c - Interrupt handlers for s4t VIP PA
 *
 * 3/02/95   H.M. Robison
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: if_isr_s4t68360.c,v $
 * Revision 3.16.4.13  1996/08/29  07:31:46  abaindur
 * CSCdi67514:  Spurious accesses while local switching on VIP
 * Branch: California_branch
 *  pak->if_output was not being set for the traffic shaping check.
 *  Bypass traffic shaping on the VIP for now.
 *
 * Revision 3.16.4.12  1996/08/28  13:06:08  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.16.4.11  1996/08/24  01:51:09  abaindur
 * CSCdi66979:  Traffic shaping broken on the c7200
 * Branch: California_branch
 *
 * Revision 3.16.4.10  1996/08/23  06:00:47  drowell
 * CSCdi65984:  Zero length packet from a S4T PA causes SEGV crash
 * Branch: California_branch
 * Catch zero length packets and count them as runts.
 *
 * Revision 3.16.4.9  1996/08/22  18:45:48  dujihara
 * CSCdi65825:  Error messages do not specify slot/subunit
 * Branch: California_branch
 * Added slot (bay) and unit to error messages.
 *
 * Revision 3.16.4.8  1996/08/13  02:21:24  gstovall
 * CSCdi39484:  Giant packets output on ethernet interface
 * Branch: California_branch
 * Make sure we drop giants, even if they are just a wee bit too big.
 *
 * Revision 3.16.4.7  1996/06/18  06:45:13  amcrae
 * CSCdi60053:  Ping on 4T fails with MTU 8000 on vip2
 * Branch: California_branch
 * VIP2 transmit ring enlarged. Sanity check for many particled packets
 *
 * Revision 3.16.4.6  1996/06/07  04:08:12  amcrae
 * CSCdi56758:  4T does not recover after power surge - should be restarted
 * Branch: California_branch
 * Added structure to carry 4T PA data. Added periodic watchdog check
 * and restart handling. Nuked some unused counters.
 *
 * Revision 3.16.4.5  1996/04/30  23:25:19  amcrae
 * CSCdi54862:  4T interrupt handler hogs cpu
 * Branch: California_branch
 * Return value of interrupt pending bit in ISR handler. Remove loop.
 *
 * Revision 3.16.4.4  1996/04/30  22:40:00  gstovall
 * CSCdi55850:  Serial PAs reports Elapse time for download and report
 * version
 * Branch: California_branch
 * Clean up the 4T error messages.
 *
 * Revision 3.16.4.3  1996/04/10  02:43:22  rramacha
 * CSCdi52294:  Add 4T support for VIP2
 * Branch: California_branch
 * VIP2 support for 4T
 *
 * Revision 3.16.4.2  1996/03/21  23:23:09  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.16.4.1  1996/03/18  21:34:27  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.2.11  1996/03/18  23:28:41  mbeesley
 * Branch: ELC_branch
 * Sync to V111_1_3
 *
 * Revision 3.2.2.10  1996/03/08  22:09:53  gstovall
 * CSCdi51111:  Need new 4T firmware
 * Branch: ELC_branch
 * Suck in some of Andrews magic firmware, plus update the driver to deal
 * with it.
 *
 * Revision 3.2.2.9  1996/02/21  18:01:55  gstovall
 * CSCdi48723:  %Software-forced reload, while testing serial hdlc to
 * serial x25.
 * Branch: ELC_branch
 * Set the pointers correctly when we grab a new particle.  While I am
 * rummaging around in there, clean up some less than optimal code.
 *
 * Revision 3.2.2.8  1996/02/14  01:58:03  gstovall
 * CSCdi48946:  Pull in 4T bugfixes from mainline
 * Branch: ELC_branch
 * Bring the ELC 4T driver up to date.
 *
 * Revision 3.2.2.7  1996/02/08  08:47:47  gstovall
 * Branch: ELC_branch
 * Sync to ELC_baseline_960205.
 *
 * Revision 3.2.2.6  1996/01/26  02:50:09  gstovall
 * Branch: ELC_branch
 * Oodles of fixes and enhancements:
 *         - Added support for the crc command to the predator 4T
 *         - Added support for the ignore-dcd command to the predator 4T
 *         - Do not keep stats on a VIP 4T
 *         - Pulled in a needed bugfix (CSCdi47390)
 *         - Revived the show controller command, it should work correctly
 *           with FDDI and token ring now.
 *
 * Revision 3.2.2.5  1996/01/24  04:16:39  gstovall
 * Branch: ELC_branch
 * Pull in some bug fixes from the 11.1 mainline, make fastswitching work
 * AND count correctly, notice if a cable gets pulled.
 *
 * Revision 3.2.2.4  1996/01/18  22:11:12  gstovall
 * Branch: ELC_branch
 * Preadator now knows the joy and splendor of serial interfaces.
 *
 * Revision 3.2.2.3  1996/01/16  01:42:32  dcarroll
 * Branch: ELC_branch
 * Sync ELC_branch between ELC_baseline_120995 and ELC_baseline_960110.
 *
 * Revision 3.2.2.2  1995/12/11  19:13:43  mbeesley
 * Branch: ELC_branch
 * Sync ELC_branch to ELC_baseline_120995.
 *
 * Revision 3.2.2.1  1995/12/04  22:18:15  mbeesley
 * Branch: ELC_branch
 * Perform mini-sync - particle refcount changes.
 *
 * Revision 3.16  1996/03/10  00:59:38  rharagan
 * CSCdi51192:  4T buffer leak when replacement buffer unavailable
 * Straighten out some non-standard indentation as well.
 *
 * Revision 3.15  1996/03/05  07:28:31  amcrae
 * CSCdi50610:  4T interfaces die at low temperatures
 * Moved transmit start to after ring update.
 * Rearranged firmware counters.
 *
 * Revision 3.14  1996/02/09  01:53:51  rharagan
 * CSCdi48311:  4T PA will hang rvip if download fails
 * Use variable delay to bound timeout for '360 download and init.
 *
 * Revision 3.13  1996/02/08  02:16:44  rharagan
 * CSCdi48368:  4T driver should throw away one byte tx pkts
 * Throw'em away, don't propagate them.
 *
 * Revision 3.12  1996/02/05  18:42:04  rharagan
 * CSCdi45869:  4T does not receive data when cabled as rs-232 DCE
 * Do not qualify rx error handling w/ debug flag.
 *
 * Revision 3.11  1996/02/05  17:50:55  rharagan
 * CSCdi45869:  4T does not receive data when cabled as rs-232 DCE
 * Sweep in missing file from prior commit.
 *
 * Revision 3.10  1996/01/20  04:59:24  amcrae
 * CSCdi47215:  4T exception processing doesnt signal host
 * CSCdi44907:  Fixed macros overlooked in previous commit
 *
 * Revision 3.9  1996/01/13  03:24:07  amcrae
 * CSCdi45724:  4T f/w is not answering mbx msgs during an interface reset
 * CSCdi43260:  VIP 4T has no implementation for transmit-delay
 * New interrupt scheme added. Firmware counter update added.
 *
 * Revision 3.8  1996/01/09  22:32:40  rharagan
 * CSCdi46625:  Holdq dequeueing broken for 4T
 * Pick up 4T version CSCdi41406. Sweep up a bit.
 *
 * Revision 3.7  1995/12/23  01:14:29  rharagan
 * CSCdi45811:  4T interface linestate does not refletct modem control
 * lines
 * Straighten out changes in linestate, continue to bring in code review
 * comments
 *
 * Revision 3.6  1995/12/19  20:24:17  gstovall
 * CSCdi45859:  Need to remove unused cruft from pas drivers
 * Nuke extra includes, tidy up comments, remove useless code.
 *
 * Revision 3.5  1995/12/14  07:41:03  rharagan
 * CSCdi42045:  tx0 output hung (800E - queue full) when pinging on serial
 * 4T
 * Strip out now unneeded tx pkt fragmentation, lose unwieldy ddelays on
 * mbx reads.
 *
 * Revision 3.4  1995/12/02  05:50:23  rharagan
 * CSCdi41966:  Messages between 4T PA & driver should be per interface
 * Move to single interface commands, incorporate code review comments.
 *
 * Revision 3.3  1995/11/28  02:26:01  smackie
 * Move particle refcount into local memory to improve handling speed. Add
 * utility functions to allow some particle internals to be removed from
 * the drivers. (CSCdi44505)
 *
 * Revision 3.2  1995/11/17  18:54:03  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:57:33  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:21:15  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/06/07  22:32:54  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
/*
 * Include files
 */
#include "master.h"
#include "interface_private.h"
#include "sys_registry.h"
#include "fastswitch_registry.h"
#include "media_registry.h"
#include "packet.h"
#include "logger.h"

#include "../os/pool.h"
#include "../os/particle.h"
#include "../os/buffers.h"
#include "../os/buffers_inline.h"

#include "../if/network.h"

#include "ieee.h"
#include "../wan/serial.h"
#include "../wan/ppp.h"
#include "../wan/smds.h"
#include "../wan/atm_dxi.h"
#include "../ip/ip.h"
#include "../if/serial_inline.h"
#include "../if/traffic_shape.h"

#include "if_pas.h"
#include "if_pas_s4t68360.h"
#include "msg_s4t68360.c"		/* Not a typo, see logger.h */
#include "../if/msg_datalink.c"		/* Not a typo, see logger.h */

#ifdef DATA_CORRUPTION 
static void debug_mb (char *string, ushort length, volatile uchar *mb)
{
    int i;
    int printcount = 0;
    
    buginf("\n");
    buginf(string);
    buginf(": MB length = %d bytes, %#lx", length, mb);
    if (length > S4T_PARTICLE_SIZE)
	length = S4T_PARTICLE_SIZE;
    for(i = 0; i < length; i++) {
	if (!(printcount++ % 16))
	    buginf("\n");
	buginf("%02x", *mb++);
	if (!(printcount & 1))
	    buginf(" ");
    }
    buginf("\n");
    
}
#endif

/*
 * s4t_rx_interrupt:
 * Rx interrupt handler for s4t68360 PA
 */
void s4t_rx_interrupt (hwidbtype *idb)
{
    s4t_instance_t *ds = INSTANCE(idb);
    s4t_dscrptr_t rmd, *rmdptr;
    particletype *mr, *mp;
    paktype *rx_pak;
    int i, pak_size, rmd_length;

    /*
     * Loop thru the rx descriptor ring processing packets
     */
    for (i=0; i < S4T_RX_MAX_SPIN; i++) {

	/*
	 * Read the descriptor
	 */
	rmdptr = &ds->rxr[ds->rx_head];
 	move64((u64*)rmdptr, (u64*)&rmd);
	if (rmd.status & S4T_RXD_OWN) {
	    /*
	     * s4t68360 still owns this one. Break out of the for
	     * loop down to flushing the data cache.
	     */
	    break;
	}

	/*
	 * Get a replacement particle
	 */
	mr = s4t_get_particle_inline(idb, TRUE);
	if (!mr) {
	    /*
	     * No replacement. Clean up and give it back to the device.
	     * Also throttle the device as we are running out of resources.
	     */
	    if (rmd.status & S4T_RXD_EOP) {
		idb->counters.input_drops++;
		s4t_set_rx_discard(ds, FALSE);
	    } else {
		s4t_set_rx_discard(ds, TRUE);
	    }
 	    rmdptr->status = S4T_RXD_OWN;
 	    if (S4T_THROTTLING) {
 		s4t_throttle(idb);
 	    }
  	    S4T_ADVANCE_RX_RING_INDEX(ds->rx_head);
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
	rmd_length = rmd.length; /* save a copy of the length field */
	rmdptr->buf_addr = S4T_BUF_ADDR(mr->data_start);
	rmdptr->status = S4T_RXD_OWN;
	ds->rxr_shadow[ds->rx_head] = mr;
  	S4T_ADVANCE_RX_RING_INDEX(ds->rx_head);

	if (rmd.status & S4T_RXD_FATAL_ERRS) {
	    s4t_rx_error(idb, rmd.status, mp);
	    continue;
	}

	/*
	 * This particle was not in error. However if rx_discard is set,
	 * it is the first good minibuffer after an errored packet or
	 * a particle in a frame rx'd when a replacement buffer 
	 * was unavailable. 
	 */
	if (ds->rx_discard) {
	    if (rmd.status & S4T_RXD_STP) {
		/*
		 * We had an error on the previous packet, but no
		 * end-of-packet notification. The present
		 * particle is in a good packet. So carry on.
		 */
		ds->rx_no_enp++;
		s4t_set_rx_discard(ds, FALSE);
	    } else {
		if (rmd.status & S4T_RXD_EOP) {
		    /* 
		     * This is the last particle of an errored packet.
		     * So clear rx_discard.
		     */
		    s4t_set_rx_discard(ds, FALSE);

		}
		retparticle(mp);        /* return this particle */
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
	if (pak_particle_count(rx_pak) && (rmd.status & S4T_RXD_STP)) {
	    ds->rx_no_enp++;
	    s4t_cleanup_pak(rx_pak);
	}

	/*
	 * If this particle is not the last particle in
	 * a packet, append it to rx_pak and continue.
	 */
	if (!(rmd.status & S4T_RXD_EOP)) {
	    mp->data_bytes = S4T_PARTICLE_SIZE;
	    particle_enqueue_inline(rx_pak, mp);
	    continue;
	}

	/*
	 * We have a completed packet to process 
	 */
	pak_size = rmd_length - ds->crc_size;
	if (rx_pak->datagramsize >= pak_size) {
	    /*
	     * The last particle had just CRC bytes. Return it.
	     */
	    retparticle_inline(mp);
	    
	    /*
	     * If this was the only particle, discard the packet and count it
	     * as a runt.
	     */
	    mp = rx_pak->particlequeue.qtail;
	    if (!mp) {
	        idb->counters.runts++;
		s4t_cleanup_pak(rx_pak);
		continue;
	    }

	    /*
	     * And adjust the tail particle
	     */
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
	    s4t_cleanup_pak(rx_pak);
	    continue;
	}
	
	/*
	 * Process the packet
	 */
	s4t_process_receive_packet(idb, ds, rx_pak, pak_size);
    }
    
    /*
     * Flush the Rx ring from the primary data cache
     */
    s4t_rxring_flush(ds);
}

/*
 * s4t_pak_to_txring:
 * Put a packet on the tx ring.
 */
static inline void s4t_pak_to_txring (paktype *pak, 
				      s4t_instance_t *ds,
				      hwidbtype *idb,
				      boolean scattered)
{
    s4t_dscrptr_t *tmd;
    int i, old_tx_tail, mcount, old_count;
    particletype *mp; 
    
    /*
     * Remember old count on ring in case we need to send a start
     * transmit message to the 4T.
     */
    old_count = ds->tx_count;
    
    if (!scattered) {
	
	/*
	 * Contigous packet. Check that we own this descriptor
	 */
	tmd = &ds->txr[ds->tx_tail];		
	if (PAS_DEBUG_BOOL && (tmd->status & S4T_TXD_OWN)) {
	    errmsg(&msgsym(OWNERR, S4T68360), idb->slot, idb->subunit, pak);
	    return;
	}
	
	/*
	 * Setup the descriptor
	 */
	ds->txr_shadow[ds->tx_tail].pak = pak;
	tmd->buf_addr = S4T_BUF_ADDR(pak->datagramstart);
	tmd->length = pak->datagramsize;
	tmd->status = (S4T_TXD_OWN|S4T_TXD_STP|S4T_TXD_EOP);
	S4T_ADVANCE_TX_RING_INDEX(ds->tx_tail);
	ds->tx_count += 1;

    } else {
	
	/*
	 * Loop thru setting everything up except ownership
	 */
	old_tx_tail = ds->tx_tail;
	mcount = 0;
	
	while (mp = particle_dequeue_inline(pak), mp) {
	    
	    tmd = &ds->txr[ds->tx_tail];		
	    if (PAS_DEBUG_BOOL && (tmd->status & S4T_TXD_OWN)) {
		errmsg(&msgsym(OWNERR, S4T68360),  idb->slot, idb->subunit, pak);
	    }
	    
	    /*
	     * Give this particle to the ring
	     */
	    
	    tmd->buf_addr = S4T_BUF_ADDR(mp->data_start);
	    tmd->length = mp->data_bytes;
	    ds->txr_shadow[ds->tx_tail].mp = mp;
	    ds->txr_shadow[ds->tx_tail].pak = NULL;
	    S4T_ADVANCE_TX_RING_INDEX(ds->tx_tail);
	    ds->tx_count += 1;
	    mcount += 1;
	}
	/*
	 * Now loop thru the same descriptors, setting status. Optimize
	 * for the case of one particle.
	 */
	if (mcount == 1) {			

	    tmd = &ds->txr[old_tx_tail];
	    ds->txr_shadow[old_tx_tail].pak = S4T_MAGIC_PAK_PTR;
	    tmd->status = (S4T_TXD_OWN|S4T_TXD_STP|S4T_TXD_EOP);

	} else {
	    
	    /*
	     * The general case, more than one Tx particle.
	     * Handle end of packet, middle particles and then 
	     * start of packet
	     */
	    old_tx_tail = ds->tx_tail;
	    S4T_RETRACT_TX_RING_INDEX(old_tx_tail);
	    tmd = &ds->txr[old_tx_tail];
	    ds->txr_shadow[old_tx_tail].pak = S4T_MAGIC_PAK_PTR;
	    tmd->status = (S4T_TXD_OWN|S4T_TXD_EOP);
	    
	    for (i=1; i< mcount-1; i++) {
		S4T_RETRACT_TX_RING_INDEX(old_tx_tail);
		tmd = &ds->txr[old_tx_tail];
		tmd->status = S4T_TXD_OWN;
	    }
	    S4T_RETRACT_TX_RING_INDEX(old_tx_tail);
	    tmd = &ds->txr[old_tx_tail];
	    tmd->status = (S4T_TXD_OWN|S4T_TXD_STP);

	}
    }
    /*
     * If no other packets were on the ring, then send
     * a message to the 4T to force an immediate ring poll.
     */
    if (old_count == 0) {
	(void) s4t_start_transmit(idb);
	ds->tx_started++;
    }
    return;
}

/*
 * s4t_safe_start:
 * Worker routine for starting or restarting output. Must be called
 * with interrupts disabled.
 */
static void s4t_safe_start (hwidbtype *idb)
{
    s4t_instance_t *ds = INSTANCE(idb);
    int empty_desc, particle_count;
    paktype *pak;
    
    /*
     * Loop putting packets on the Tx ring
     */
    while (pak = (*idb->oqueue_dequeue)(idb), pak) {
	/*
	 * Sanity check for large packets
	 */
	if (pak->datagramsize > idb->max_pak_size) {
	    idb->counters.output_total_drops++;
	    errmsg(&msgsym(TOOBIG, LINK), idb->hw_namestring, 
		   pak->datagramsize);
	    datagram_done(pak);
	    continue;
	}
	
	if (pak->datagramsize < S4T_MIN_PACKET) {
	    idb->counters.output_total_drops++;
	    errmsg(&msgsym(TOOSMALL, S4T68360), idb->hw_namestring, 
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
		(ds->tx_count >= S4T_TX_RING_ENTRIES)) {
		ds->tx_fullring++;
		pak_requeue_head(idb, pak);
		pak = NULL;
		break;
	    }
	    s4t_pak_to_txring(pak, ds, idb, FALSE);
	} else {
	    
	    /*
	     * It is a scattered packet. Make sure the packet there is
	     * room for all particles
	     * on the Tx ring.
	     */
	    if (particle_count > S4T_TX_RING_ENTRIES) {
		idb->counters.output_total_drops++;
		errmsg(&msgsym(RINGSIZE, S4T68360), idb->hw_namestring);
		datagram_done(pak);
		continue;
	    }
	    empty_desc = S4T_TX_RING_ENTRIES - ds->tx_count;
	    if ((ds->tx_limited && ds->tx_count) ||
		(particle_count > empty_desc)) {
		ds->tx_fullring++;
		pak_requeue_head(idb, pak);
		pak = NULL;
		break;
	    }

	    /*
	     * Give this packet to the txring
	     */
	    s4t_pak_to_txring(pak, ds, idb, TRUE);
	    
	    /*
	     * Give back the pakheader
	     */
            datagram_done(pak);
	}

	/*
	 * Do packet accounting
	 */
	if (S4T_PKT_STATS) {
	    idb->counters.tx_cumbytes += pak->datagramsize;
	    TIMER_START(idb->xmittimer, TIMEOUT);
	    idb->counters.outputs += 1;
	}
    }
}

/*
 * s4t_tx_interrupt :
 * Process and reclaim all buffers that have been transmitted
 */
void s4t_tx_interrupt (hwidbtype *idb)
{
    s4t_instance_t *ds = INSTANCE(idb);
    s4t_tx_shadow_t *shadow_ptr;
    particletype *mp;
    s4t_dscrptr_t *tmd;
    paktype *pak;
    int i;
    ushort tmd_status;
    
    /*
     * Reclaim transmitted buffers
     */
    for (i=0; i<S4T_TX_MAX_SPIN; i++) {
	
	
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
	    tmd_status = tmd->status;
	    if (tmd_status & S4T_TXD_OWN) {
		/*
		 * But the s4t68360 still owns it.
		 */
		break;
	    }

	    /*
	     * Check for errors, only valid at EOP
	     */
	    if ((tmd_status & S4T_TXD_EOP) && 
		(tmd_status & S4T_TXD_ALL_ERRS)){
		s4t_tx_error(tmd_status, ds->tx_head, idb);
	    } 
	    /*
	     * Advance the head pointer
	     */
	    S4T_ADVANCE_TX_RING_INDEX(ds->tx_head);
	    
	    /*
	     * Clear the shadow
	     */
	    shadow_ptr->mp = NULL;
	    shadow_ptr->pak = NULL;

	    /*
	     * Do clean up
	     */
	    if (mp) {
		/*
		 * If pak is also set, then this is a particle that was
		 * the last in an outgoing packet.
		 */
		if (S4T_PKT_STATS && pak) {
		    GET_TIMESTAMP(idb->lastoutput);
		}

		ds->tx_count--;
		retparticle_inline(mp);
	    } else {
		datagram_done(pak);
		if (S4T_PKT_STATS) {
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
	if (serial_debug) {
	    buginf("\nS4T(%d): Fatal transmit error on %s. Resetting\n",
		   idb->unit, idb->hw_namestring);
	}
	s4t_init(idb);
    }

    /*
     * And finally possibly restart output
     */
    if (idb->output_qcount) {
 	s4t_safe_start(idb);
    }
}


/*
 * s4t_tx_start : 
 * Start or restart output. Raise interrupt level and call s4t_safe_start
 */
void s4t_tx_start (hwidbtype *idb)
{
    leveltype level;

    level = raise_interrupt_level(NETS_DISABLE);
    s4t_safe_start(idb);
    reset_interrupt_level(level);
}

/*
 * s4t_fastsend:
 * Transmit this packet immediately or enqueue it on the hold q.
 */
void s4t_fastsend (hwidbtype *idb, paktype *pak)
{
    s4t_instance_t *ds = INSTANCE(idb);
    int empty_desc, particle_count;
    traffic_shape_t *shape;
    
    if (idb->state != IDBS_UP) {
	idb->counters.output_total_drops++;
	if (pak_has_particles(pak)) {
	    s4t_cleanup_pak(pak);
	} else {
	    datagram_done(pak);
	}
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
	        s4t_cleanup_pak(pak);
		return;
	    }
	    /*
	     * Transfer necessary fields from old to new pak.
	     * Then flush old pak for reuse and take over new pak.
	     */
	    pas_stuff_pak_for_holdq(newpak, pak);
	    s4t_flush_pak(pak);
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
	s4t_output_enqueue(pak, idb);

	/*
	 * If the Tx ring is full, then try and clean some stuff up
	 * by calling s4t_tx_interrupt (which calls s4t_safe_start()).
	 * Otherwise just call s4t_safe_start() directly.
	 */
	if (ds->tx_count >= S4T_TX_RING_ENTRIES) {
	    s4t_tx_interrupt(idb);
	} else {
	    s4t_safe_start(idb);
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
	if (ds->tx_count >= S4T_TX_RING_ENTRIES) {
	    /*
	     * Since device drivers may be passing statically allocated
	     * paktypes thru fast switching code, the paktype that we have here
	     * needs to be duplicated.
	     */
	    s4t_output_enqueue(pak, idb);
	    return;
	}

	/*
	 * This is a contigous packet. Thats easy.
	 * First thing to do is make sure that the packet is
	 * large enough, and there is a place on tx ring.
	 */
	if (pak->datagramsize < S4T_MIN_PACKET) {
	    idb->counters.output_total_drops++;
	    errmsg(&msgsym(TOOSMALL, S4T68360), idb->hw_namestring, 
		   pak->datagramsize);
	    datagram_done(pak);
	    return;
	}
	if ((ds->tx_limited && ds->tx_count) ||
	    (ds->tx_count >= S4T_TX_RING_ENTRIES)) {
	    pak_requeue_head(idb, pak);
	    pak = NULL;
	    return;
	}
	s4t_pak_to_txring(pak, ds, idb, FALSE);
    } else {
	
	/*
	 * The packet is scattered. Make sure there is room for it on the
	 * Tx ring. If not, queue it.
	 */
	if (particle_count > S4T_TX_RING_ENTRIES) {
	    idb->counters.output_total_drops++;
	    errmsg(&msgsym(RINGSIZE, S4T68360), idb->hw_namestring);
	    datagram_done(pak);
	    return;
	}
	empty_desc = S4T_TX_RING_ENTRIES - ds->tx_count;
	if (particle_count > empty_desc) {
	    /*
	     * Since device drivers may be passing statically allocated
	     * paktypes thru fast switching code, the paktype that we have
	     * here needs to be duplicated.
	     */
	    s4t_output_enqueue(pak, idb);
	    return;
	}

	s4t_pak_to_txring(pak, ds, idb, TRUE);
    }
    
    /*
     * Do packet accounting
     */
    if (S4T_PKT_STATS) {
	idb->counters.tx_cumbytes_fast[pak->acct_proto] += pak->datagramsize;
	idb->counters.outputs_fast[pak->acct_proto]++;
	TIMER_START(idb->xmittimer, TIMEOUT);
    }
}

/*
 *	s4t_status_interrupt:
 *	Status of control lines or cable has changed.
 *	Reread the status.
 */
void s4t_status_interrupt (hwidbtype *idb)
{
    if (!s4t_read_port_status(idb)) {
	errmsg(&msgsym(STOPFAIL, S4T68360), idb->hw_namestring, 
               "read port status ");
    }

    /*
     * Do something based on port status
     */
    s4t_status_int_handle(idb);
}

static inline void check_isr_cause ( hwidbtype *idb, unsigned char cause)
{
    if (idb && cause) {
	if (cause & S4T_INT_CAUSE_RX) {
	    s4t_rx_interrupt(idb);
	}
	if (cause & S4T_INT_CAUSE_TX) {
	    s4t_tx_interrupt(idb); 
	}
	if (cause & S4T_INT_CAUSE_STAT)
	    s4t_status_interrupt(idb);
    }
}
/*
 *	s4t_interrupt_handler:
 *	Front interrupt handler for 4T.
 *	Reads interrupt cause from AMCC register and dispatches
 *	interrupt handlers.
 *  Returns true if more work to do.
 */
ulong s4t_interrupt_handler (hwidbtype **isr_idb_table, s4t_regs_t *regs)
{
    union {
	unsigned long w;
	unsigned char per_port[4];
    } cause;
    
    if (regs->mbx_status_reg & S5933_IN_INT_STATUS_MASK) {
/*
 *  Clear mailbox interrupt flag.
 */
	regs->int_ctrl_reg |= S5933_INT_CTRL_IMBOX_INTR;
	cause.w = regs->in_mbx4;
	if (cause.per_port[1]) {
	    errmsg(&msgsym(PANIC, S4T68360), isr_idb_table[0]->slot,
		   cause.per_port[2], cause.per_port[3]);
		return(0);
	}
	check_isr_cause(isr_idb_table[0], (cause.per_port[2]>>4) & 0xF);
	check_isr_cause(isr_idb_table[1], cause.per_port[2] & 0xF);
	check_isr_cause(isr_idb_table[2], (cause.per_port[3]>>4) & 0xF);
	check_isr_cause(isr_idb_table[3], cause.per_port[3] & 0xF);
    }
/*
 *  If more interrupts are pending, then return true.
 */
	return(regs->mbx_status_reg & S5933_IN_INT_STATUS_MASK);
}
