/* $Id: if_isr_ibm2692.c,v 3.10.4.14 1996/08/29 07:31:41 abaindur Exp $
 * $Source: /release/112/cvs/Xsys/pas/if_isr_ibm2692.c,v $
 *------------------------------------------------------------------
 * if_isr_ibm2692.c - Platform independent interrupt service 
 *		      routines for 4R IBM Token Ring PCI port adaptor
 *
 * May 1995, HC Wang
 * 
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: if_isr_ibm2692.c,v $
 * Revision 3.10.4.14  1996/08/29  07:31:41  abaindur
 * CSCdi67514:  Spurious accesses while local switching on VIP
 * Branch: California_branch
 *  pak->if_output was not being set for the traffic shaping check.
 *  Bypass traffic shaping on the VIP for now.
 *
 * Revision 3.10.4.13  1996/08/28  13:06:01  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.10.4.12  1996/08/24  01:51:02  abaindur
 * CSCdi66979:  Traffic shaping broken on the c7200
 * Branch: California_branch
 *
 * Revision 3.10.4.11  1996/08/20  17:10:29  hwang
 * CSCdi66116:  4R PA if configured for tb operation could lock up
 * transmitter
 * Branch: California_branch
 *
 * Revision 3.10.4.10  1996/08/13  02:21:18  gstovall
 * CSCdi39484:  Giant packets output on ethernet interface
 * Branch: California_branch
 * Make sure we drop giants, even if they are just a wee bit too big.
 *
 * Revision 3.10.4.9  1996/06/07  23:52:19  sthormod
 * CSCdi57928:  OIR of 4R Wedges Console (No Throttling Code)
 * Branch: California_branch
 *
 * Revision 3.10.4.8  1996/06/07  02:26:34  hwang
 * CSCdi57022:  Tx at 12kpps Rx at 14kpps with 4r/4r vip2
 * Replace retparticle with retparticle_inline and other performance tweak.
 * Branch: California_branch
 *
 * Revision 3.10.4.7  1996/06/07  02:18:54  sthormod
 * CSCdi59800:  c7200 4r needs to recover from adapter check
 * Branch: California_branch
 *
 * Revision 3.10.4.6  1996/06/07  00:17:34  hwang
 * CSCdi59533:  4r driver need to implement srb command queue
 * Branch: California_branch
 *
 * Revision 3.10.4.5  1996/05/26  00:18:16  hwang
 * CSCdi58206:  srb testing locks up svip
 * Use particle based pak to xfr MAC frame passed by Hermon/MPC using arb
 * command.
 * Branch: California_branch
 *
 * Revision 3.10.4.4  1996/05/24  20:00:07  hwang
 * CSCdi58573:  4R receiver become deaf
 * Enable Hermon timerinterrupt to sample pci busmaster status
 * register every 500 ms, if it indicates receiver channel is
 * not enabled or disabled reenable receive channel.
 * Branch: California_branch
 *
 * Revision 3.10.4.3  1996/05/23  00:11:27  sthormod
 * CSCdi55611:  4r port adapter output queue wedges under stress...
 * Branch: California_branch
 *
 * Revision 3.10.4.2  1996/03/21  23:23:04  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.10.4.1  1996/03/18  21:34:24  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.2.9  1996/03/18  23:28:38  mbeesley
 * Branch: ELC_branch
 * Sync to V111_1_3
 *
 * Revision 3.3.2.8  1996/03/10  02:49:51  sthormod
 * CSCdi51196:  add tbridge switching path for predator 4R
 * Branch: ELC_branch
 *
 * Revision 3.3.2.7  1996/03/06  19:51:29  sthormod
 * CSCdi50859:  add srb switching path for predator 4R
 * Branch: ELC_branch
 * o  process & fast switching path for srb & explorers.
 * o  routing with rif field.
 * o  use dma to coalesce particle based packet.
 * o  process mac frames.
 *
 * Revision 3.3.2.6  1996/02/21  22:45:54  sthormod
 * CSCdi49158:  predator/4R keep alives dont work properly
 * Branch: ELC_branch
 * - add token-ring error support
 * - add keep alive code
 * - update lastoutput
 * - add ibm2692_version_string()
 * - remove ibm2692_linestate(), use default tring_linestate
 * - move ibm2692_periodic() to idb->device_periodic vector
 * - add idb->system_configured vector
 *
 * Revision 3.3.2.5  1996/02/08  08:47:45  gstovall
 * Branch: ELC_branch
 * Sync to ELC_baseline_960205.
 *
 * Revision 3.3.2.4  1996/01/31  01:37:24  sthormod
 * Branch: ELC_branch
 * Add Predator 4R support.
 *
 * Revision 3.3.2.3  1995/12/19  23:32:31  sthormod
 * Branch: ELC_branch
 * Initial structure changes for porting 4R driver to predator.
 *
 * Revision 3.3.2.2  1995/12/06  04:08:49  sthormod
 * Branch: ELC_branch
 * Bring in HC's code review changes.
 * Revision 3.10  1996/03/11  17:46:57  hwang
 * CSCdi50643:  4r driver doesnt handle mac frame correctly
 *
 * Revision 3.9  1996/02/29  01:02:47  hwang
 * CSCdi50254:  4R driver need to handle error counter overflow event
 * If lan_status_change arb indicates counter overflow, issue read log
 * srb command to retrieve error counts.
 *
 * Revision 3.8  1996/02/14  22:02:17  hwang
 * CSCdi48583:  vines packets not forwarded in 4r-4r transparent bridge
 * Reduce the size of reject address table, if tb is configured xmt one
 * packet at a time (no chained packet to avoid rejection addr tbl
 * overflow). When access the packet memory use cpu view address instead
 * of pci byte swap address.
 *
 * Revision 3.7  1996/01/31  18:59:04  hwang
 * CSCdi47842:  4R xmt channel disabled after xmt fifo underrun error
 * If tx error bits are set check the tx busmaster status, if tx is
 * disabled re enable it.
 *
 * Revision 3.6  1996/01/23  23:50:54  hwang
 * CSCdi46141:  rvip 4r/4r crashes with ipx traffic
 * Hermon tx bug work around, don't give hermon any packet to xmt before
 * end of chain tx interrupt.
 *
 * Revision 3.5.18.1  1996/01/24  22:47:41  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.5  1995/12/03  06:39:43  hwang
 * CSCdi44890:  incorporate code review comments into 4r driver
 *
 * Revision 3.3.2.1  1995/12/04  22:18:12  mbeesley
 * Branch: ELC_branch
 * Perform mini-sync - particle refcount changes.
 *
 * Revision 3.3  1995/11/17  18:53:58  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:21:05  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:57:31  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:21:12  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/06/07  22:32:52  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "sys_registry.h"
#include "media_registry.h"
#include "interface_private.h"
#include "packet.h"
#include "config.h"
#include "logger.h"
#include "subsys.h"
#include "parser.h"

#include "../os/pool.h"
#include "../os/particle.h"
#include "../os/buffers.h"
#include "../os/buffers_inline.h"
#include "../os/free.h"     /* memory_dump */

#include "../if/tring.h"
#include "../if/tring_private.h"
#include "../if/pci.h"
#include "../if/network.h"
#include "../if/tring_common.h"
#include "../if/traffic_shape.h"

#include "if_pas.h"
#include "msg_pas.c"			/* Not a typo, see logger.h */
#include "if_pas_ibm2692.h"
#include "msg_ibm2692.c"		/* Not a typo, see logger.h */


/*
 * send_mac_frame : using trb to tell mpc to xmt a mac frame
 */
static void send_mac_frame (hwidbtype *idb, tx_bfr_desc_t *tp)
{
    ibm2692_instance_t *ds = INSTANCE(idb);
    hermon_reg_map *hermon_ptr = ds->ibm2692_ptr;
    int length, i;
    trb_t	mac_frame;
    ushort	*dp, data_wrote;

    if (tokenevent_debug) {
	buginf("\n%s attempting to send mac frame via trb",
	       idb->hw_namestring); 
    }

    /*
     * set up the trb header first
     */
    mac_frame.trb.ret_code = MPC_INITIAL_STAT;
    mac_frame.trb.tx_error = 0;
    mac_frame.trb.length = tp->frame_length;
    data_wrote = 0;
    
    /*
     * send a mac frame to mpc through lap facility
     */
    ibm2692_lap_write(hermon_ptr, MPC_RAM, (ushort)(ds->trb_loc + 2),
		      (ushort *)&mac_frame.data, TRB_HDR_LEN-2);

    i = 0;
    while (data_wrote < tp->frame_length) {
 	dp = (ushort *)tp->bfr[i].bptr;
 	length = tp->bfr[i].b_count;
 	/*
 	 * move data from minibuffer to shared ram using lapd
 	 */
	ibm2692_lap_write(hermon_ptr, MPC_RAM, (ushort)(ds->trb_loc + 10), 
			  dp, length);

 	data_wrote += length;
    }
			  
    /*
     * this frame is ready for mpc to transmit, issue trb cmd to mpc
     */
    hermon_ptr->lisr_sum = SIF_TRB_CMD;
}

/*
 * ibm2692_update_tx_queue
 */
static inline void ibm2692_update_tx_queue (hwidbtype *idb,
					    ibm2692_instance_t *ds,
					    hermon_reg_map *hermon_ptr) 
{
    if (!ds->tx_outstanding) {
	/*
	 * no packet queued for transmit, turn off tx_in_progress flag
	 */
	ds->tx_in_progress = FALSE;
	return;
    }

    if (ds->tx_hw_queue_count != ds->tx_outstanding) {
	/*
	 * can't update the registers while the tx channel is active. 
	 */
	return;
    }

    /*
     * if we are configured for transparent bridging operation
     */
    if (idb->tr_ring_mode & RINGMODE_TBRIDGE) {
	tx_desc_shadow_t *tx_desc;
	
	tx_desc = &ds->tx_desc_shadow[ds->tx_hw_queue_head];
	add_reject_addr(idb, tx_desc->tb_saddr);
	/*
	 * feed hermon just one packt at a time, due to the size
	 * of source address rejection table size.
	 */
	hermon_ptr->tx2lfda = tx_desc->pci_view_addr;
	ds->tx_hw_queue_count -= 1;
	if (ds->tx_hw_queue_count) {
	    ds->tx_hw_queue_head++;
	    ds->tx_hw_queue_head &= IBM2692_TX_RING_INDEX_MASK;
	}
    } else {
	/*
	 * no transparent bridging, so we can give
	 * hermon a chained packet to transmit
	 */
	if (ds->tx_hw_queue_count == 1) {
	    /*
	     * only one packet in the queue, just modify txlfda
	     * register will send out this packet.
	     */
	    hermon_ptr->tx2lfda =
		ds->tx_desc_shadow[ds->tx_hw_queue_head].pci_view_addr;
	} else {
	    /*
	     * for multiple packet in queue, we need to modify
	     * txfda and txlfda to signal hermon that multiple
	     * packet in list
	     */
	    hermon_ptr->tx2fda =
		ds->tx_desc_shadow[ds->tx_hw_queue_head].pci_view_addr;
	    hermon_ptr->tx2lfda =
		ds->tx_desc_shadow[ds->tx_hw_queue_tail].pci_view_addr;
	}
	ds->tx_hw_queue_count = 0;
    }
}

/*
 * enable_halted_tx_channel
 */
static void enable_halted_tx_channel (hwidbtype *idb,
				      ibm2692_instance_t *ds,
				      hermon_reg_map *hermon_ptr) 

{
    tx_bfr_desc_t	*tp;
    tx_desc_shadow_t	*tds;
    int i;

    /*
     * For some reason (f.g. tx fifo underun, dma parity error, etc.)
     * the tx channel has flaked on us and needs to be restarted.
     * Rather than clearing the entire tx ring, we will only clear
     * those packets that the chipset is aware of and not those on the
     * tx_hw_queue.  Then if there are packets of the tx_hw_queue,
     * we'll gives those to the chipset. 
     */

    ds->tx_chan_disabled += 1;

    while (ds->tx_outstanding != ds->tx_hw_queue_count) {
	tds = &(ds->tx_desc_shadow[ds->tx_outptr]);
        tp = tds->cpu_view_addr;
	tp->tx_status = 0x0;

	/*
	 * check for contigous buffer packet
	 */
	if (tds->tx_pak) {
	    /*
	     * reset buffer pointer and length field
	     */
	    for (i = 0; i < tp->bfr_count-1; i++) {
		tp->bfr[i].bptr = NULL;
		tp->bfr[i].b_count = 0;
	    }
	    /*
	     * return particles to pool
	     */
	    datagram_done(tds->tx_pak);
	    tds->tx_pak = NULL;
        } else {
	    /*
	     * segmented packet, need to return all particles
	     */
            for (i = 0; i < tp->bfr_count-1; i++) {
		if (tds->particle[i]) {
		    retparticle(tds->particle[i]);
		    tds->particle[i] = NULL;
		}
		tp->bfr[i].bptr = NULL;
		tp->bfr[i].b_count = 0;
	    }
	    tp->bfr[i].bptr = NULL;	/* this is the dummy bfr */
	}
	tp->bfr_count = 0;
	tp->frame_length = 0;
	ds->tx_outstanding -= 1;
	ds->tx_outptr++;
	ds->tx_outptr &= IBM2692_TX_RING_INDEX_MASK;
    }

    /*
     * now that we've cleaned up, lets get her going again
     */
    hermon_ptr->bmctl_rum = ~TX2_DISABLED;

	ibm2692_update_tx_queue(idb, ds, hermon_ptr);
}

/*
 * hermon_tx_interrupt - Hermon bus master mode transmit interrupt handler
 */
void hermon_tx_interrupt (hwidbtype *idb, ibm2692_instance_t *ds,
			  hermon_reg_map *hermon_ptr)
{
    ulong       stat;
    tx_bfr_desc_t *p;
    int index;
    tx_desc_shadow_t *desc_ptr;

    if ((idb->state != IDBS_UP) || (ds->tx2_chnl_disabled))
        return;

    while (ds->tx_outstanding) {
	desc_ptr = &(ds->tx_desc_shadow[ds->tx_outptr]);
        p = desc_ptr->cpu_view_addr;
        stat = p->tx_status;     /* read descriptor status field */
        if (stat & TX_FDP) {  /* descriptor processed by Hermon */
 	    /*
 	     * check for tx error first before releasing buffers
 	     * we may trying to xmt out an mac frame and the chip set
 	     * is not happy about it. But we can't afford to check 
 	     * every out going packet for mac frame, it burns too much
 	     * cpu horsepower
 	     */
 	    if (stat & TX_ERROR_MASK) {
		do_tx_accounting(stat, ds);

		/*
		 * check for cancel frame 		 
		 */
 		if ((stat & CANCEL_FRAME) == CANCEL_FRAME) {
		    /*
		     * "This condition indicates that the hardware
		     * did not complete the transmit operation of this
		     * frame due to a local disable or reject of a MAC
		     * frame when the channel is not enabled to transmit
		     * MAC frames."  3-72 IBM LANStreamer Reference
		     *
		     * Use TRB to send the MAC frame.
 		     */
 		    send_mac_frame(idb, p);
		}

 		/*
 		 * need to check for busmaster status register
 		 * under some error condition, tx channel will be disabled
 		 */
 		if (hermon_ptr->bmctl_sum & TX2_DISABLED) {
		    /* 
		     * This is a is a software workaround for the hermon bug 
		     * detailed below..
		     * The problem occurs after an underrun while the FIFO 
		     * is spilling the remainder of the second frame after 
		     * the underrun. If the timing of the local bus side emptying 
		     * the FIFO exactly matches the time that the bus side 
		     * puts in an end of frame, the handshake between the local 
		     * and bus logic can lose the EOF and cause the Tx channel 
		     * to get stuck waiting for the last status to be posted.  
		     * This is cause when the 2 async events of 'FIFO empty' 
		     * and EOF_in occur at the same time on the second  frame 
		     * after and underrun.
		     * The software workarounds for this are 
		     * 1. Queue only one frame at a time, which is real bad 
		     *    for the performance point of view.
		     * 2. Wait for sometime (20 msecs) before you clean up the 
		     *    ring and and restart the transmits again. So, we flag 
		     *    the timer and wait for 20 msecs.
		     */
		    hermon_ptr->timer = TIMER_ENABLE | MS_20;
		    ds->tx2_chnl_disabled = TRUE;
		    return;
 		}
 	    }
 
	    if (desc_ptr->tx_pak) {
		/*
		 * contiguous buffer, do datagram_done
		 */
		datagram_done(desc_ptr->tx_pak);
		desc_ptr->tx_pak = NULL;
	    } else {
		/*
		 * segmented buffer, return individual particle to pool
		 */
		for (index = 0; index < p->bfr_count-1; index++) {
		    retparticle_inline(desc_ptr->particle[index]);  
		    desc_ptr->particle[index] = NULL;
		}
	    }
	    p->tx_status = 0x0;	/* clear status field */

            ds->tx_outstanding -= 1;
	    ds->tx_outptr++;
	    ds->tx_outptr &= IBM2692_TX_RING_INDEX_MASK;
	} else { /* descriptor not processed by Hermon yet */
	    /*
	     * need to check for status write failure condition
	     */
	    stat = hermon_ptr->tx2stat;
	    if (stat & TX_WER) {
		/*
		 * Hermon failed to write status in descriptor
		 */
		ds->tx_pci_stat_fail++;
		if (tokenevent_debug) {
		    buginf("\n%s tx status write failed %x",
			   idb->hw_namestring, stat);
		}
		if (desc_ptr->tx_pak) {
		     datagram_done(desc_ptr->tx_pak);
		     desc_ptr->tx_pak = NULL;
		} else {
		    for (index = 0; index < p->bfr_count-1; index++) {
			retparticle_inline(desc_ptr->particle[index]);
			desc_ptr->particle[index] = NULL;
		    }
		}
		 p->tx_status = 0x0; /* clear status field */
		 ds->tx_outstanding -= 1;
		 ds->tx_outptr++;
		 ds->tx_outptr &= IBM2692_TX_RING_INDEX_MASK;
	    }
	    break;
        }
    }

    /* 
     * check if we are done with tx queue
     */
    ibm2692_update_tx_queue(idb, ds, hermon_ptr);
 
    /* 
     * Check output queue for transmit
     */
    if (idb->output_qcount) 
	ibm2692_safe_start(idb);
}

/*
 * enable_halted_rx_channel
 */
static void enable_halted_rx_channel (ibm2692_instance_t *ds)
{
    int i;
    rx_bfr_desc_t *p1;
    rx_desc_shadow_t *rx_shadow_desc;
    hermon_reg_map      *hermon_ptr = ds->ibm2692_ptr;

    ds->rx_chan_disabled += 1;
    ibm2692_cleanup_pak(ds->rx_pak);
    /*
     * rx descriptor ring always has one particle attached to it
     */
    for (i = 0; i < IBM2692_RX_RING_ENTRIES; i++) {
	rx_shadow_desc =  &(ds->rx_desc_shadow[i]);
	p1 = rx_shadow_desc->cpu_view_addr;
	p1->rx_status = 0x0;
	p1->frame_length = 0;
	p1->buf_length = IBM2692_PARTICLE_SIZE;
    }
    ds->rx_start = 0;
    hermon_ptr->bmctl_rum = ~RX_DISABLED;
    hermon_ptr->rxbda = ds->rx_desc_shadow[0].pci_view_addr;
    hermon_ptr->rxlbda =
	ds->rx_desc_shadow[IBM2692_RX_RING_ENTRIES-1].pci_view_addr;
}

/*
 * Hermon receive channel interrupt handler
 */
static void hermon_rx_interrupt (hwidbtype *idb, ibm2692_instance_t *ds,
				  hermon_reg_map *hermon_ptr)
{
    rx_bfr_desc_t *p1;
    rx_desc_shadow_t *rx_shadow_desc;
    particletype *p, *r;
    ulong stat;
    int length, packet_size, i;

    if (idb->state != IDBS_UP)
        return;

    /*
     * start processing receive descriptors
     */
    for (i = 0; i < IBM2692_RX_RING_ENTRIES; i++) {
	rx_shadow_desc = &(ds->rx_desc_shadow[ds->rx_start]);
        p1 = rx_shadow_desc->cpu_view_addr;

        /*
	 * check for receive descriptor status
	 */
        stat = p1->rx_status;
        if (!(stat & RX_BDP)) {	/* this descriptor not processed by H/W */
            break;
        }

	/*
	 * flush giant packet or when throttled
	 */
	if (ds->flush_pkt || (IBM2692_THROTTLING && idb_is_throttled(idb))) {	
	    p1->rx_status = 0x0;
	    p1->frame_length = 0x0;
	    p1->buf_length = IBM2692_PARTICLE_SIZE;
	    ibm2692_return_buffer(ds);

	    if (stat & RX_EOF) {	/* end of frame reached */
		ds->flush_pkt = FALSE;
            }
	    continue;
	}

        if (stat & RX_ERROR_MASK) {	/* error in this mini buffer */
            /*
             * error in this mini buffer, flush segpak 
             * return this descriptor back to hardware
             */
	    p1->rx_status = 0x0;
	    p1->buf_length = IBM2692_PARTICLE_SIZE;
	    p1->frame_length = 0;
	    /*
	     * link descriptor to hardware queue
	     */
	    ibm2692_return_buffer(ds);
	    /*
	     * flush all particles in pak
	     */
	    ibm2692_cleanup_pak(ds->rx_pak);
	    /*
	     * do error accounting
	     */
	    ibm2692_rx_error_log(stat, ds);
	    idb->counters.input_error++; /* increment damaged frame count */
            /*
             * fix to CSCdj16191 - mark the rx_pak has been flushed so that
             * the rest of the particles in the list will be freed without
             * being further processed.
             */
            if (!(stat & RX_EOF))
               ds->flush_pkt = TRUE;
	    continue;
        } else {	/* no errors in this mini buffer */
	    packet_size = p1->frame_length;
	    r = rx_shadow_desc->particle;
	    p = ibm2692_get_particle_inline(idb, TRUE);
	    if (p) {
		p1->rx_status = 0;
		p1->buf_length = IBM2692_PARTICLE_SIZE;
		p1->frame_length = 0;
		p1->bptr = (uchar *)IBM2692_BUF_ADDR(p->data_start);
		rx_shadow_desc->particle = p;
		ibm2692_return_buffer(ds);
		/*
		 * if not end of frame, append this particle into pak
		 */
		if (!(stat & RX_EOF)) {
		    r->data_bytes = IBM2692_PARTICLE_SIZE;
		    ibm2692_particle_enqueue_inline(ds->rx_pak, r);
		    if (ds->rx_pak->datagramsize > idb->max_pak_size) {
			ds->flush_pkt = TRUE;
			idb->counters.giants++;
			ibm2692_cleanup_pak(ds->rx_pak);
		    }
		    continue;
		}

		/*
		 * end of frame
		 */
                packet_size -= IBM2692_CRC_SIZE;
		length = packet_size - ds->rx_pak->datagramsize;
		if (length <= 0) {
                    /*
                     * Last particle contains only the FCS (or a fragment
                     * thereof).  So, nuke the particle and adjust the
                     * datagramsize.  Note: we save the FCS before we nuke the
                     * particle in case we need to add it back on later for SRB.
                     */
                    ds->fcs_save = *r->data_start;
		    retparticle_inline(r); 
		    r = ds->rx_pak->particlequeue.qtail;
		    r->data_bytes -= (ds->rx_pak->datagramsize - packet_size);
		    ds->rx_pak->datagramsize = packet_size;
		} else {
                    /*
                     * Otherwise, just adjust the last particle's data length to 
                     * remove the FCS.  Datagramsize will be adjusted automatically.
                     */
		    r->data_bytes = length;
		    ibm2692_particle_enqueue_inline(ds->rx_pak, r);
		}

		if (packet_size > idb->max_pak_size) {
		    idb->counters.giants++;
		    ibm2692_cleanup_pak(ds->rx_pak);
		    continue;
		}
		ibm2692_process_receive_packet(idb, stat, ds->rx_pak);
	    } else {	/* no mini buffer available */
 		if (IBM2692_THROTTLING) {
 		    ibm2692_throttle(idb);
 		    ibm2692_cleanup_pak(ds->rx_pak);
 		}
		idb->counters.input_resource++;
		p1->rx_status = 0;
		p1->buf_length = IBM2692_PARTICLE_SIZE;
		p1->frame_length = 0;
		ibm2692_return_buffer(ds);
		ibm2692_cleanup_pak(ds->rx_pak);

		if (!(stat & RX_EOF)) {
		    ds->flush_pkt = TRUE;
                } 
	    }
        }	/* good packet */
    }
}

/*
 * queue_asb : queue an asb (adapter status block) on asb queue
 */
static void queue_asb (hwidbtype *idb, mpc_asb_t *ptr)
{
    ibm2692_instance_t *ds = INSTANCE(idb);
    mpc_asb_t	*asbp;

    if (ds->asb_queue_count < ASB_QUEUE_SIZE) {
	/*
	 * copy in data
	 */
	asbp = &(ds->sys_asb[ds->asb_inptr]);
	asbp->command = ptr->command;
	asbp->ret_code = ptr->ret_code;
	asbp->rcv_bfr = ptr->rcv_bfr;
	ds->asb_queue_count++;
	ds->asb_inptr++;
	ds->asb_inptr &= ASB_INDEX_MASK;
    } else {
	/*
	 * we are out of asb queue element
	 */
	if (tokenevent_debug) {
	    buginf("\n%s: MPC asb queue full", idb->hw_namestring);
	}
    }
}

/*
 * release_mac_bfr: using asb to inform mpc that mac buffer is free
 */
static void release_mac_bfr (hwidbtype *idb, ushort bfr)
{
    ibm2692_instance_t *ds = INSTANCE(idb);
    hermon_reg_map *hermon_ptr = ds->ibm2692_ptr;
    mpc_asb_t asb, *asbp;
    ushort	*ptr, length;
    boolean asb_free = TRUE;

    asbp = & asb;
    ptr = (ushort *)asbp;
    /*
     * read in asb through lap facility, asb will tell us if asb is free
     */
    length = MPC_ASB_SIZE;
    ibm2692_lap_read(hermon_ptr, MPC_RAM, ds->asb_loc, ptr, length);

    if (asbp->ret_code != ASB_AVAILABLE) {
	asb_free = FALSE;
	if (tokenevent_debug) {
	    buginf("\n%s: asb ret code %x", idb->hw_namestring,
		     asbp->ret_code);
	}
	/*
	 * asb is not available, tell mpc to free it
	 */
	hermon_ptr->lisr_sum = SIF_ASB_FREE;
    }
    /*
     * build asb
     */
    asbp = &asb;
    asbp->command = RECEIVE_DATA;
    asbp->ret_code = 0x0;
    asbp->rcv_bfr = bfr;

    /*
     * send asb to mpc
     */
    if (asb_free) {
	ptr = (ushort *)&asb;
	ibm2692_lap_write(hermon_ptr, MPC_RAM, ds->asb_loc, ptr, length);

	/*
	 * signal mpc to check for asb
	 */
        hermon_ptr->lisr_sum = SIF_ASB_RSP;
    } else {
	/*
	 * need to wait for asb free interrupt then issue asb free
	 */
	if (tokenevent_debug) {
	    buginf("\n%s: asb not free, queue asb", idb->hw_namestring);
	}
	queue_asb(idb, asbp);
    }

}

/*
 * get_mac_frame : MPC want to pass a MAC frame to system
 */
static void get_mac_frame (hwidbtype *idb, arb_t *ptr)
{
    ibm2692_instance_t *ds = INSTANCE(idb);
    hermon_reg_map *hermon_ptr;
    ushort *mac_ptr, mac_frame_length, bfr_length; 
    ushort mac_bfr, current_bfr, data_read;
    paktype *pak;
    mac_hdr_t	mac_bfr_header;

    hermon_ptr = ds->ibm2692_ptr;
    current_bfr = mac_bfr = ptr->receive_data.rcv_buff;
    pak = input_getbuffer(ptr->receive_data.frame_len + 1, idb);
    if (pak) {
	pak->datagramsize = ptr->receive_data.frame_len;
	mac_ptr = (ushort *)pak->datagramstart;
	mac_frame_length = ptr->receive_data.frame_len;
	data_read = 0;
	while (current_bfr) {
	    /*
	     * read in the buffer header first
	     */
	    ibm2692_lap_read(hermon_ptr, MPC_RAM, current_bfr,
			     (ushort *)&mac_bfr_header, ASB_MAC_HDR_SIZE);
	    bfr_length = mac_bfr_header.length;

	    /*
	     * copy in frame data in this buffer
	     */
	    ibm2692_lap_read(hermon_ptr, MPC_RAM, 
			     (current_bfr + ASB_MAC_HDR_SIZE),
			     mac_ptr, bfr_length);

	    data_read += bfr_length;	    
	    mac_ptr += bfr_length;

	    if (data_read == mac_frame_length) {
		ibm2692_process_mac_frame(idb, pak);
		break;
	    } else {
		current_bfr = mac_bfr_header.next_bfr;
	    }
	}
    }
    /*
     * release mac frame buffer
     */
    release_mac_bfr(idb, current_bfr);
}

/*
 * hermon system interface interrupt handler
 */
static void hermon_system_interrupt (hwidbtype *idb, ushort status,
				     ibm2692_instance_t *ds,
				     hermon_reg_map *hermon_ptr)
{
    int i, length;
    ushort *ptr, stat;
    uchar  arb_cmd;
    init_resp_t	*irtp;
    arb_t	*arbp;
    tokensb	*tsb;
    srb_cmd_block_t srb_cmd;

    /*
     * clear system interrupt
     */
    hermon_ptr->sisr_rum = ~status;

    if (status & ADP_CHK) {	/* adapter check interrupt */
	if (tokenevent_debug) {
	    buginf("\n%s: Adapter Check", idb->hw_namestring);
	}

	/*
	 * get adapter check status addr 
	 */
        ds->adp_chk_addr = hermon_ptr->lapwwc; 

	ibm2692_lap_read(hermon_ptr, MPC_RAM, ds->adp_chk_addr,
			 (ushort *)&ds->adp_chk_data, 8);
	ibm2692_adp_chk(idb); 
    } 

    if (status & SRB_RESP) {	/* SRB response interrupt */
        if (ds->mpc_init_flag) {	/* MPC init done */
            ds->srb_loc = hermon_ptr->lapwwo;	/* get address of srb */
	    ds->mpc_lapwwc = hermon_ptr->lapwwc; /* additional error info */
	    if (tokenevent_debug) {
		buginf("\n%s: init response, lapwwo %x lapwwc %x",
			idb->hw_namestring, ds->srb_loc, ds->mpc_lapwwc);
	    }
            get_init_resp(idb);
            ds->mpc_init_flag = FALSE;
	    ds->srb_in_use = FALSE;
            /*
             * check for init sucess/fail
             */
	    irtp = &ds->mpc_init_response;
            if ((irtp->init_result.command == INIT_RESP) && 
                (irtp->init_result.bring_up_code == MPC_OK_STAT)) {
		if (idb->tr_ring_mode & (RINGMODE_BRIDGE | RINGMODE_TBRIDGE)) {
                    config_bridge(idb);
		} else {
		    do_mpc_open(idb);
		}
            } else {	/* init failed notify RP */
		if (tokenevent_debug) {
                    buginf("\n%s: INIT Fail %x Lapwwc %x", idb->hw_namestring,
			ds->mpc_init_response.init_result.bring_up_code, 
			ds->mpc_lapwwc);
		}
		ibm2692_init_fail(idb, 
				  ds->mpc_init_response.init_result.bring_up_code); 
                ibm2692_reset(idb); 
                net_cstate(idb, IDBS_RESET);
            }
        }
        else if (ds->srb_in_use) {
            ds->srb_in_use = FALSE;
            read_sreqb_response(idb);
	    /*
	     * check if any pending srb command in queue
	     */
	    if (ds->srb_queue_count) {
		hermon_sreqb_command(idb, &ds->srb_queue[ds->srb_outptr]);
		ds->srb_outptr += 1;
		ds->srb_outptr &= MAX_SRB_QUEUE_MASK;
		ds->srb_queue_count -= 1;
	    }
	} else {
	    if (tokenevent_debug) {
		buginf("\n%s: unexpected SRB_RESP interrupt",
		       idb->hw_namestring); 
	    }
	}
    } 

    if (status & ASB_FREE) {	/* ASB free interrupt */
	if (ds->asb_queue_count) {
	    ptr = (ushort *)(&ds->sys_asb[ds->asb_outptr]);
	    length = MPC_ASB_SIZE;
	    ibm2692_lap_write(hermon_ptr, MPC_RAM, ds->asb_loc, ptr, length);
	    /*
	     * tell mpc to check out asb
	     */
	    hermon_ptr->lisr_sum = SIF_ASB_RSP;
	    ds->asb_queue_count -= 1;
	    ds->asb_outptr++;
	    ds->asb_outptr &= ASB_INDEX_MASK;
	}
    } 

    if (status & ARB_CMD) {	/* ARB command interrupt */
	arbp = &ds->mpc_arb;
	length = ARB_SIZE;
	ibm2692_lap_read(hermon_ptr, MPC_RAM, ds->arb_loc, 
			 (ushort *)&arbp->data, length); 
        arb_cmd = arbp->lan_status.command;	/* read command code */
        switch (arb_cmd) {
            case RECEIVE_DATA :
                ds->mac_bfr_addr = arbp->receive_data.rcv_buff;
                ds->mac_bfr_header_len = arbp->receive_data.header_len;
                ds->mac_bfr_length = arbp->receive_data.frame_len;
		if (tokenevent_debug) {
		    buginf("\n%s: MAC frame passed by MPC",
			   idb->hw_namestring); 
		    buginf("\nARB ");
		    for (i = 0; i < length; i++) {
			buginf("%x ", arbp->data[i]);
		    }
		    buginf("\nbfr addr %x", arbp->receive_data.rcv_buff);
		    buginf("\nbfr hdr len %x", arbp->receive_data.header_len);
		    buginf("\nbfr len %x", arbp->receive_data.frame_len);
		}
		get_mac_frame(idb, arbp);
                break;
            case LAN_STATUS_CHANGE :
		switch (arbp->lan_status.status) {
		case COUNTER_OVERFLOW:
		    /*
		     * mpc is about to roll over some of the counter
		     * nothing to panic, if no srb command pending,
		     * issue read log command to retrieve the error
		     * counter, don't even bother to send love letter.
		     */
		    srb_cmd.read_log.command = READ_LOG;
		    srb_cmd.read_log.ret_code = MPC_INITIAL_STAT;
		    hermon_sreqb_command(idb, &srb_cmd);
		    break;
		case SR_COUNTER_OVERFLOW:
		    /*
		     * mpc is about to roll over source route counter
		     */
		    srb_cmd.read_sr_counter.command = READ_SR_COUNT;
		    srb_cmd.read_sr_counter.ret_code = MPC_INITIAL_STAT;
		    hermon_sreqb_command(idb, &srb_cmd);
		    break;
		default:
		    /*
		     * this is not error counter over flow and source route
		     * counter over flow event, need to inform rp/rsp
		     * something bad has happened on the ring.
		     */
		    tsb = idb_use_hwsb_inline(idb, HWIDB_SB_TOKEN);
		    tsb->tr_ring_status = arbp->lan_status.status;
		    GET_TIMESTAMP(tsb->tr_status_time);
		    ibm2692_lan_status_change(idb, tsb->tr_ring_status);
		    idb_release_hwsb_inline(idb, HWIDB_SB_TOKEN);
		    break;
		}
		break;
	    default :
		/*
		 * unknown arb command??
		 */
		if (tokenevent_debug) {
		    buginf("\n%s: unknown arb command %x", 
			   idb->hw_namestring, arb_cmd);
		}
		break;
	    }
	hermon_ptr->lisr_sum = SIF_ARB_FREE;	/* tell mpc arb is free */
    } 
    
    if (status & TRB_RESP) {	/* TRB response interrupt */
	ushort retcode;
	/*
	 * The retcode is in the 2nd short word of the trb, so add 2
	 * to the trb_loc.
	 */
	ibm2692_lap_read(hermon_ptr, MPC_RAM, ds->trb_loc+2, &retcode, 2);
        if (retcode && (tokenevent_debug)) {
            buginf("\n%s: failed to transmit mac frame", idb->hw_namestring);
        }
    } 

    /* 
     * Timer interrupt: currently used to check the status of the
     * rx/tx channels.  If it has been disabled and we're not
     * throttled, then try to re-enable it. 
     *
     * Note: the reason that the chip disables the rx is that it can't
     * get a rx buffer.  Normally when this occurs the interrupt
     * status would indicate RX_NRBDA, but the silly hermon ucode only
     * sets that bit if the frame would have taken more than one
     * buffer.  So if we get a high load of smaller than particle size
     * frames, the rx may get shutdown without us knowing about it.
     * As a work around we use the timer interrupt to periodically
     * check it.
     */
    if ((status & TMR_INT) && (idb->state == IDBS_UP)) { 
	stat = hermon_ptr->bmctl_sum;
 	/*
 	 * check Rx channel
 	 */
 	if (!idb_is_throttled(idb)) {
 	    if ((stat & RX_DISABLED) || (!(stat & RX_ENABLED))) {
 		if (tokenevent_debug) {
 		    buginf("\n%s: rx channel disabled, bmctl %x",
 			   idb->hw_namestring, stat);
 		}
 		enable_halted_rx_channel(ds);
  	    }
 	}
	/*
	 * check Tx channel
	 */
	if (stat & TX2_DISABLED) {
	    if (tokenevent_debug) {
		buginf("\n%s: tx2 disabled, bmctl %x", 
		       idb->hw_namestring, stat); 
	    }
	    ds->tx2_chnl_disabled = FALSE;
	    hermon_ptr->timer = TIMER_ENABLE | MS_500;
	    enable_halted_tx_channel(idb, ds, hermon_ptr); 
	}
    } 
    
    /*
     * checking for any unused interrupt
     */
    if (!(status & (UNUSED_INT))) {
        return;
    }

    if (status & SCB_STAT_INT) {	/* SCB status port interrupt */
	if (tokenevent_debug) {
	    buginf("\n%s: SCB status port interrupt", idb->hw_namestring);
	}
    } 

    if (status & SCB_CNTL_INT) {	/* SCB control port interrupt */
	if (tokenevent_debug) {
	    buginf("\n%s: SCB control port interrupt", idb->hw_namestring);
	}
    } 

    if (status & SCB_SIG_INT) {	/* SCB signal port interrupt */
	if (tokenevent_debug) {
	    buginf("\n%s: SCB signalling port interrupt", idb->hw_namestring);
	}
    } 

    if (status & LAP_PERR_INT) {	/* LAP port parity error interrupt */
	if (tokenevent_debug) {
	    buginf("\n%s: LAP data parity error interrupt",
		   idb->hw_namestring); 
	}
    } 

    if (status & LAP_ACC_INT) {	
	/*
	 * LAP port access violation interrupt
	 * 
	 * verify that:
	 * -  lape is valid (MPC_RAM, MPC_ROM, MPC_REG)
	 * -  lapa is short word aligned
	 * -  lapwwo <= lapa <= lapwwc
	 */
	if (tokenevent_debug) {
	    buginf("\n%s: LAP access violation error interrupt",
		   idb->hw_namestring); 
	}
    } 

    if (status & PCI_PERR_INT) {	/* PCI parity error interrupt */
	if (tokenevent_debug) {
	    buginf("\n%s: PCI parity error interrupt", idb->hw_namestring);
	}
    }
}

/*
 * hermon_interrupt - hermon bus master mode interrupt service routine
 */
void hermon_interrupt (hwidbtype *idb)
{
    ibm2692_instance_t *ds = INSTANCE(idb);
    ushort s_status;
    hermon_reg_map *hermon_ptr;

    hermon_ptr = ds->ibm2692_ptr;
    s_status = hermon_ptr->sisr;	/* read system int status register */
    if (s_status & MI_IND) {	/* bus master interrupt, priority 1 */
	/*
	 * read bus master interrupt status and clear interrupt
	 */
	ds->hermon_misr = hermon_ptr->misr;
	hermon_ptr->misr = ~ds->hermon_misr;

        /*
         * process receiver interrupt first 
         */
        if (ds->hermon_misr & RX_INT_MASK) {
             hermon_rx_interrupt(idb, ds, hermon_ptr);
        }

        /*   
         * check for transmit channel 2 interrupt
         */
        if (ds->tx_outstanding) {
            hermon_tx_interrupt(idb, ds, hermon_ptr);
        }

	/*
	 * check for transmit channel 1 interrupt
	 * this channel is not used by current driver
	 * it should be set only after init, mpc will test it
	 */
        if (ds->hermon_misr & TX1_INT_MASK) {
	    hermon_ptr->misr = ~(ds->hermon_misr & TX1_INT_MASK);
	    if (!ds->mpc_init_flag) {
		if (tokenevent_debug) {
		    buginf("\n%s: Tx channel 1 false interrupt", 
			    idb->hw_namestring);
		}
            }
        }
    }

    /*
     * if no system interrupt just return
     */
    if (!(s_status & ~MI_IND)) {
	return;
    } else {
	hermon_system_interrupt(idb, s_status, ds, hermon_ptr);
    }
}

/*
 * ibm2692_fastsend : transmit this packet immediately, called from
 *                    interrupt level 
 */
void ibm2692_fastsend (hwidbtype *idb, paktype *pak)
{
    ibm2692_instance_t *ds = INSTANCE(idb);
    hermon_reg_map	*hermon_ptr;
    traffic_shape_t *shape;

    hermon_ptr = ds->ibm2692_ptr;
    /*
     * check for interface state, if not up drop this packet
     */
    if (idb->state != IDBS_UP) {
	idb->counters.output_total_drops++;
	datagram_done(pak); /* hmm, shouldn't it call cleanup_pak ??? */
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
	        ibm2692_cleanup_pak(pak);
		return;
	    }
	    /*
	     * Transfer necessary fields from old to new pak.
	     * Then flush old pak for reuse and take over new pak.
	     */
	    pas_stuff_pak_for_holdq(newpak, pak);
	    ibm2692_flush_pak(pak);
	    pak = newpak;
	}
	traffic_shape_defer_sending(shape, pak);
	return;
    }

    /*
     * if there are packets on the hold queue, or we don't have
     * room on the tx descriptor ring, queue this packet
     */
    if ((idb->output_qcount) || 
        (ds->tx_outstanding >= IBM2692_TX_RING_ENTRIES)) {
	if (pak->datagramsize > idb->max_pak_size) {
	    idb->counters.output_total_drops++;
	    datagram_done(pak); /* hmm, shouldn't it call cleanup_pak ??? */
	} else {
	    ibm2692_output_enqueue(pak, idb);

	    /*
	     * If the Tx ring if full, then try and clean some stuff up
	     * by calling hermon_tx_interrupt (which calls ibm2692_safe_start.
	     * Otherwise just call ibm2692_safe_start() directly.
	     */
	    if (ds->tx_outstanding >= IBM2692_TX_RING_ENTRIES) {
		hermon_tx_interrupt(idb, ds, hermon_ptr);
	    } else {
		ibm2692_safe_start(idb);
	    }
	} 
    } else {

	/*
	 * Do fastsend accounting
	 */
	if (IBM2692_PKT_STATS) {
	    ulong acct_proto = (ulong)pak->acct_proto;
	    
	    idb->counters.tx_cumbytes_fast[acct_proto] += pak->datagramsize;
	    idb->counters.outputs_fast[acct_proto] += 1;
	    if (*(pak->datagramstart) & 0x01) {
		idb->counters.output_broad++;
	    }
	    GET_TIMESTAMP(idb->lastoutput);
	}
	/* 
	 * Invalidate accounting protocol to prevent duplicate
	 * accounting. 
	 */
	pak->acct_proto = ACCT_PROTO_LAST;
	ibm2692_transmit_inline(idb, pak, FALSE);
    }
}
