/* $Id: if_isr_mif68840.c,v 3.2.58.22 1996/09/01 21:20:26 fowler Exp $
 * $Source: /release/112/cvs/Xsys/pas/if_isr_mif68840.c,v $
 *------------------------------------------------------------------
 * if_isr_mif68840.c - 1F mif68840 based PCI port adaptor support
 *                  (Interrupt service routines)
 *
 * August 1995, Farhad Sunavala
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: if_isr_mif68840.c,v $
 * Revision 3.2.58.22  1996/09/01  21:20:26  fowler
 * CSCdi60734:  Frame Relay rate enforcement is not effective
 *              Remove original fix due to its performance impact
 * Branch: California_branch
 *
 * Revision 3.2.58.21  1996/08/30  16:48:23  fowler
 * CSCdi60734:  Frame Relay rate enforcement is not effective
 *              Add registry to check for the need for media dependent
 *              traffic shaping
 * Branch: California_branch
 *
 * Revision 3.2.58.20  1996/08/29  07:31:43  abaindur
 * CSCdi67514:  Spurious accesses while local switching on VIP
 * Branch: California_branch
 *  pak->if_output was not being set for the traffic shaping check.
 *  Bypass traffic shaping on the VIP for now.
 *
 * Revision 3.2.58.19  1996/08/28  13:06:05  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.2.58.18  1996/08/24  01:51:05  abaindur
 * CSCdi66979:  Traffic shaping broken on the c7200
 * Branch: California_branch
 *
 * Revision 3.2.58.17  1996/08/22  18:47:23  dujihara
 * CSCdi65826:  Error messages do not specify slot/subunit
 * Branch: California_branch
 * Added slot (bay) and unit to error messages.
 *
 * Revision 3.2.58.16  1996/08/13  02:21:21  gstovall
 * CSCdi39484:  Giant packets output on ethernet interface
 * Branch: California_branch
 * Make sure we drop giants, even if they are just a wee bit too big.
 *
 * Revision 3.2.58.15  1996/06/18  04:22:04  fsunaval
 * CSCdi60432:  Bug in throttling code (1F).
 * Branch: California_branch
 *
 * Revision 3.2.58.14  1996/06/10  22:21:42  mbeesley
 * CSCdi59552:  software forced reload due to memory corruption in
 * 2FPA-2FPA test. Use 32 byte burst, Memory Read Line on Predator. Also
 * remove debugging buginfs.
 * Branch: California_branch
 *
 * Revision 3.2.58.13  1996/05/31  01:26:18  fsunaval
 * CSCdi57339:  Implement throttling on the 1F.
 * Branch: California_branch
 * 1.Throttle the interface when no more particles available.  The
 *   interface is unthrottled when the cache has a certain threshold of
 *   particles in it.
 *
 * Revision 3.2.58.12  1996/05/29  18:27:34  fsunaval
 * CSCdi58107:  Long stress run in DFS gets malloc error in FDDI PA.
 * Branch: California_branch
 *
 * Revision 3.2.58.11  1996/05/28  07:06:55  fsunaval
 * CSCdi58107:  Long stress run in DFS gets malloc error in FDDI PA.
 * Branch: California_branch
 * 1.If RX particle has error, enqueue it before calling cleanup routine.
 *
 * Revision 3.2.58.10  1996/05/21  17:52:49  fsunaval
 * CSCdi55267:  1F - Fix indentation.
 * Branch: California_branch
 *
 * Revision 3.2.58.9  1996/05/19  03:18:42  fsunaval
 * CSCdi55239:  FDDI driver needs to correctly mimic AF status bits
 * settings
 * Branch: California_branch
 *
 * Revision 3.2.58.8  1996/05/18  07:05:15  fsunaval
 * CSCdi54705:  SW-VIP2: Unnecessary message for RX data overrun.
 * Branch: California_branch
 *
 * Revision 3.2.58.7  1996/05/12  01:58:20  fsunaval
 * CSCdi52405:  Router Not Learning CLNS Paths
 * Branch: California_branch
 *
 * Revision 3.2.58.6  1996/05/07  00:38:53  mbeesley
 * CSCdi56232:  Enable IP Flow/Optimum on fddi
 * Branch: California_branch
 *
 * Revision 3.2.58.5  1996/05/06  19:27:44  fsunaval
 * CSCdi56768:  Update trt_neg
 * Branch: California_branch
 * 1.Pass one more parameter throught the MIB loveletter.
 *
 * Revision 3.2.58.4  1996/04/11  04:39:47  fsunaval
 * CSCdi53600:  SW-SVIP: SVIP Fddi optimizations.
 * Branch: California_branch
 * 1.Take advantage of posted read hardware.
 * 2.Take advantage of Management interrupts.
 *
 * Revision 3.2.58.3  1996/04/03  04:23:14  fsunaval
 * CSCdi52374:  Fddi ring overrun observed while testing IP routing.
 * Branch: California_branch
 * 1.Kick-start the rings when RX overrun occurs.
 *
 * Revision 3.2.58.2  1996/03/28  02:49:25  fsunaval
 * CSCdi51945:  SW-VIP1: fddi pa not showing neigbor mac-address.
 * Branch: California_branch
 *
 * Revision 3.2.2.7  1996/03/22  20:50:22  fsunaval
 * CSCdi51945:  SW-VIP1: fddi pa not showing neigbor mac-address.
 * Branch: ELC_branch
 *
 * Revision 3.2.2.6  1996/03/09  22:14:38  fsunaval
 * CSCdi50760:  Particle-based Appletalk fast-switching broken on Fddi.
 * Branch: ELC_branch
 *
 * Revision 3.2.2.5  1996/02/26  04:39:19  fsunaval
 * CSCdi49791:  Incorporate code-review comments for Fddi port adapter
 * Branch: ELC_branch
 *
 * Revision 3.2.2.4  1996/02/17  05:54:30  fsunaval
 * CSCdi49008:  Fddi output counters are not reported and other fixes.
 * Branch: ELC_branch
 * 1.Output counts not increasing.
 * 2.Make FDDI on VIP memory-mapped.
 * 3.Distributed switching support.
 * 4.Reduce burst-size from 64 to 32.
 *
 * Revision 3.2.2.3  1996/02/05  07:00:09  fsunaval
 * Branch: ELC_branch
 * 1.Particle based fast-switching Fddi support and code cleanup.
 *
 * Revision 3.2.2.2  1996/01/25  12:10:04  fsunaval
 * Branch: ELC_branch
 * 1.Predator FDDI support.
 * 2.Patch CSCdi46358 (PA management interrupt) changes into ELC_branch.
 *
 * Revision 3.2.2.1  1995/12/23  04:41:25  fsunaval
 * Branch: ELC_branch
 * 1.First pass at FDDI port adapter.
 *
 * Revision 3.2  1995/11/17  18:54:00  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:57:32  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:21:14  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/08/24  19:46:26  fsunaval
 * Placeholder files for new development.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
/*
 * Include files
 */
#include "master.h"
#include "logger.h"
#include "../pas/msg_mif68840.c"	/* Not a typo, see logger.h */
#include "../pas/msg_pas.c"		/* Not a typo, see logger.h */
#include "../if/msg_datalink.c"		/* Not a typo, see logger.h */
#include "sys_registry.h"
#include "fastswitch_registry.h"
#include "interface_private.h"
#include "packet.h"
#include "config.h"
#include "ttysrv.h"   
#include "subsys.h"
#include "sched.h"
#include "parser.h"
#include "mgd_timers.h"
    
#include "../os/pool.h"  
#include "../os/particle.h"
#include "../os/buffers.h"
#include "../os/buffers_inline.h"
    
#include "../if/fddi.h"
#include "../if/pci.h"
#include "../if/network.h" 
#include "../if/traffic_shape.h"
#include "../ip/ip.h"

#include "../xdi/xdihdrsif.h"
#include "../xdi/fddihdr.h"
#include "../xdi/smtmsg.h"
#include "../xdi/csphdr.h"
#include "../xdi/cspglbl.h"
#include "../xdi/cspproto.h"
#include "../xdi/motomac.h"
#include "../xdi/mibdefs.h"
#include "../xdi/mibglbl.h"
#include "../xdi/mibproto.h"
#include "../xdi/fbmframe.h"
#include "../xdi/fbmhdr.h"
#include "../xdi/fbmglbl.h"      

#include "../if/if_fddi.h"
#include "../if/if_fddi_public.h"
#include "../if/if_xdi_public.h"

#include "if_pas.h"
#include "if_pas_mif68840.h"    /* includes platform-specific file */


/* FIXME */
extern void hack(ulong x);
extern void CSPManagementInterrupt (ulong sw_intr);


/*
 * mif68840_rx_llc_interrupt:
 * Rx interrupt handler for MIF68840.
 */
void mif68840_rx_llc_interrupt (hwidbtype *idb)
{  
    register mif68840_instance *ds = INSTANCE(idb);
    mif68840_rx_desc *rmd;
    register ulong rmd_status_length;
    register paktype *rx_pak;
    register particletype *mr, *mp;
    register ulong i;
    ulong old_rx_head, pak_size;

    PAS_DEBUG(DAS_DEBUG(0x55555555));

    rx_pak = ds->rx_pak_llc;

    /*
     * Loop thru the rx llc descriptor ring processing packets
     */
    for (i=0; i < MIF68840_RX_MAX_SPIN; i++) {

        /* Save the rx ring index */
        old_rx_head = ds->rx_llc_head;

        /*
         * Read the descriptor
         */
        rmd = &ds->rxr_llc[old_rx_head];
        rmd_status_length = rmd->rx_status_length;

        if (rmd_status_length & MIF68840_RX_OWN) {

            /*
             * IFDDI still owns this one. Break out of the for
             * loop down to flushing the data cache.
             */
            break;
        }

        /*
         * Increment the head
         */
        mp = ds->rxr_llc_shadow[old_rx_head];
        ds->rx_llc_head = (old_rx_head + 1) & (MIF68840_RX_LLC_RING_ENTRIES-1);
        /*
         * Get a replacement particle
         */
        mr = mif68840_get_particle_inline(idb, TRUE);
        if (!mr)  {
            /*
             * No replacement.  Clean up and give it back to the device.
             */
            rmd->rx_status_length = MIF68840_RX_STATUS_LENGTH_DEFAULT;

            if (rmd_status_length & MIF68840_RX_LAST) {
                mif68840_cleanup_pak(rx_pak);
                idb->counters.input_drops++;
                ds->llc_mr_drops++;
                ds->rx_discard = FALSE;
                if (MIF68840_THROTTLING) {
                    mif68840_throttle(idb);
                }
            } else {
                ds->rx_discard = TRUE;
            }
            continue;
        }

        /*
         * We have a replacement. Give it back to the rx ring.
         */
        rmd->rx_buffer_addr = MIF68840_BUF_ADDR(mr->data_start);
        rmd->rx_status_length = MIF68840_RX_STATUS_LENGTH_DEFAULT;
        ds->rxr_llc_shadow[old_rx_head] = mr;

        /*
         * So we are here with a good particle, without rx_discard set.
         * If this particle is the start of a packet, make sure
         * that there was an end of packet before it. IE catch the condition
         * where "end of packet" particle got lost.
         */
        if (pak_particle_count(rx_pak) &&
            (rmd_status_length & MIF68840_RX_FIRST)) {
            ds->rx_no_llc_ep++;
            mif68840_cleanup_pak(rx_pak);
        } 
         
        /*
         * If this particle is not the last particle in
         * a packet, append it to rx_pak and continue.
         */  
        if (!(rmd_status_length & MIF68840_RX_LAST)) {
            mp->data_bytes = MIF68840_PARTICLE_SIZE;
            particle_enqueue_inline(rx_pak, mp);
            continue;
        }

        /* Last particle in the packet */
        /* Enqueue it first to clean up all the memory mess */
        pak_size = rmd_status_length & MIF68840_RX_LENGTH;
        mp->data_bytes = pak_size - rx_pak->datagramsize;
        particle_enqueue_inline(rx_pak, mp);

        if ((rx_pak->datagramsize != pak_size) ||
             (mp->data_bytes > MIF68840_PARTICLE_SIZE)) {
                if (fddi_debug) {
                buginf("\nrx_llc_mis: %s, pksize=%d, dgsize=%d, mpbytes=%d",
                    idb->hw_namestring, pak_size,
                    rx_pak->datagramsize, mp->data_bytes);
                }
            ds->rx_llc_mis++;
            mif68840_cleanup_pak(rx_pak);
            continue;
        }

        /* Check for errors */
        if (rmd_status_length  & MIF68840_RX_ERROR) {
            mif68840_rx_error(rmd_status_length, idb, mp);
            mif68840_cleanup_pak(rx_pak);
            continue;
        }

        /* We have a completed error-free packet to process */
        /* Paranoia check */
        if (ds->rx_discard) {
            mif68840_cleanup_pak(rx_pak);
            idb->counters.input_drops++;
            ds->llc_mr_drops++;
            ds->rx_discard = FALSE;
            continue;
        }

        /*
         * Range check the packet size
         */
        if (pak_size >= FDDI_FRAME_SIZE) {
            idb->counters.giants++;
            mif68840_cleanup_pak(rx_pak);
            continue;
        }

        /*
         * Process the packet     
         */
        mif68840_process_receive_llc_packet(idb, ds, rx_pak, pak_size, 
            rmd_status_length);
    }

    if (pak_particle_count(rx_pak) && ds->rxovr_4) {
        mif68840_cleanup_pak(rx_pak);
    }

    /*
     * Flush the Rx ring from the primary data cache
     */
    mif68840_rxring_flush(ds);

    PAS_DEBUG(DAS_DEBUG(0x66666600 | i));
}

/*
 * mif68840_rx_smt_interrupt:
 * Rx interrupt handler for MIF68840.
 */
void mif68840_rx_smt_interrupt (hwidbtype *idb)
{  
    mif68840_instance *ds = INSTANCE(idb);
    mif68840_rx_desc *rmd;
    particletype *mr, *mp;
    register paktype *rx_pak;
    register ulong rmd_status_length, old_rx_head;
    ulong i, pak_size;

    rx_pak = ds->rx_pak_smt;

    /*
     * Loop thru the rx smt descriptor ring processing packets
     */
    for (i=0; i < MIF68840_RX_MAX_SPIN; i++) {

        /* Save the rx ring index */
        old_rx_head = ds->rx_smt_head;

        /*
         * Read the descriptor
         */
        rmd = &ds->rxr_smt[old_rx_head];
        rmd_status_length = rmd->rx_status_length;

        if (rmd_status_length & MIF68840_RX_OWN) {
            
            /*
             * IFDDI still owns this one. Break out of the for
             * loop down to flushing the data cache.
             */
            break;
        }

        /*
         * Increment the head
         */
        mp = ds->rxr_smt_shadow[old_rx_head];
        ds->rx_smt_head = (old_rx_head + 1) & (MIF68840_RX_SMT_RING_ENTRIES-1);

        /*
         * Get a replacement particle
         */
        mr = mif68840_get_particle_inline(idb, TRUE);
        if (!mr)  {
            /*
             * No replacement.  Clean up and give it back to the device.
             */
            rmd->rx_status_length = MIF68840_RX_STATUS_LENGTH_DEFAULT;

            if (rmd_status_length & MIF68840_RX_LAST) {
                mif68840_cleanup_pak(rx_pak);
                idb->counters.input_drops++;
                ds->smt_mr_drops++;
                ds->rx_discard = FALSE;
                break;
            } else {
                ds->rx_discard = TRUE;
                continue;
            }
        }

        /*
         * We have a replacement. Give it back to the rx ring.
         */
        rmd->rx_buffer_addr = MIF68840_BUF_ADDR(mr->data_start);
        rmd->rx_status_length = MIF68840_RX_STATUS_LENGTH_DEFAULT;
        ds->rxr_smt_shadow[old_rx_head] = mr;

        /*
         * So we are here with a good particle, without rx_discard set.
         * If this particle is the start of a packet, make sure
         * that there was an end of packet before it. IE catch the condition
         * where "end of packet" particle got lost.
         */
        if (pak_particle_count(rx_pak) &&
            (rmd_status_length & MIF68840_RX_FIRST)) {
            ds->rx_no_smt_ep++;
            mif68840_cleanup_pak(rx_pak);
        }

        /*
         * If this particle is not the last particle in
         * a packet, append it to rx_pak and continue.
         */  
        if (!(rmd_status_length & MIF68840_RX_LAST)) {
            mp->data_bytes = MIF68840_PARTICLE_SIZE;
            particle_enqueue_inline(rx_pak, mp);
            continue;
        }

        /* Last particle in the packet */
        /* Enqueue it first to avoid all the memory mess */
        pak_size = rmd_status_length & MIF68840_RX_LENGTH;
        mp->data_bytes = pak_size - rx_pak->datagramsize;
        particle_enqueue_inline(rx_pak, mp);

        if ((rx_pak->datagramsize != pak_size) ||
             (mp->data_bytes > MIF68840_PARTICLE_SIZE)) {
                buginf("\nrx_smt_mis: %s, pksize=%d, dgsize=%d, mpbytes=%d",
                    idb->hw_namestring, pak_size,
                    rx_pak->datagramsize, mp->data_bytes);
            ds->rx_smt_mis++;
            mif68840_cleanup_pak(rx_pak);
            continue;
        }

        /* Check for errors */
        if (rmd_status_length  & MIF68840_RX_ERROR) {
            mif68840_rx_error(rmd_status_length, idb, mp);
            mif68840_cleanup_pak(rx_pak);
            continue;
        }

        /* We have a completed error-free packet to process */
        /* Paranoia check */
        if (ds->rx_discard) {
            mif68840_cleanup_pak(rx_pak);
            idb->counters.input_drops++;
            ds->smt_mr_drops++;
            ds->rx_discard = FALSE;
	    continue;
        }

    
        /*
         * Range check the packet size
         */
        if (pak_size > idb->max_pak_size) {
            idb->counters.giants++;
            mif68840_cleanup_pak(rx_pak);
            continue;
        }

        /*
         * Process the packet     
         */
        mif68840_process_receive_smt_packet(idb, ds, rx_pak, pak_size,
            rmd_status_length);
    }
    
    /*
     * Flush the Rx ring from the primary data cache
     */
    mif68840_rxring_flush(ds);
}

/*   
 * mif68840_safe_start:
 * Worker routine for starting or restarting output. Must be called
 * with interrupts disabled.
 */        
static void mif68840_safe_start (hwidbtype *idb)
{
    mif68840_instance *ds = INSTANCE(idb);
    mif68840_tx_desc *tmd, *tmd1;
    int empty_desc, old_tx_tail, mcount, particle_count;  
    particletype *mp;
    ulong first_cl;
    paktype *pak;

    PAS_DEBUG(DAS_DEBUG(0x12341234));
 
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
                (ds->tx_count >= MIF68840_TX_RING_ENTRIES)) {
                pak_requeue_head(idb, pak);
                pak = NULL;
                return;
            }
        
            /*
             * Check that we own this descriptor
             */
            tmd = &ds->txr[ds->tx_tail];
       
            if (PAS_DEBUG_BOOL && (tmd->tx_status_length & MIF68840_TX_OWN)) {
                errmsg(&msgsym(OWNERR, MIF68840), idb->slot, idb->subunit, pak);
            }
 
            /* Add the three MIF68840 specific transmit bytes before the FC */
            PUTLONG((pak->datagramstart - 4), PREMAC);
 
            /*
             * Setup the descriptor and shadow
             */
            ds->txr_shadow[ds->tx_tail].pak = pak;

            /* ds->txr_shadow[ds->tx_tail].mp is NULL. Don't do it again. */

            tmd->tx_buffer_addr = MIF68840_BUF_ADDR((pak->datagramstart - 3));
            tmd->tx_status_length = MIF68840_TX_FIRST_LAST_BUFFER |
                                    (pak->datagramsize + 3);
 
            if (PAS_DEBUG_BOOL && fddi_debug) {
                buginf("\n%s: safestart(co), sts=0x%08x, addr=0x%08x",
                idb->hw_namestring, tmd->tx_status_length,tmd->tx_buffer_addr);
            }

            ds->tx_count += 1;
            /* advance the tail pointer */
            ds->tx_tail = (ds->tx_tail + 1) & (MIF68840_TX_RING_ENTRIES-1);
 
            /* 
             * Do packet accounting
             */
            if (MIF68840_PKT_STATS) {
   
                idb->counters.tx_cumbytes += pak->datagramsize;
                idb->counters.outputs += 1;
                TIMER_START(idb->xmittimer,3*ONESEC + idb->keep_period*ONESEC);
                if (*(pak->datagramstart) & 0x01) {
                    idb->counters.output_broad++;
                }
            }

        } else {

        /*
         * The packet is scattered. Make sure there is room for it on the
         * Tx ring. If not, queue it.
         */
        empty_desc = MIF68840_TX_RING_ENTRIES - ds->tx_count;
        if ((ds->tx_limited && ds->tx_count) ||
                (particle_count > empty_desc)) {
            pak_requeue_head(idb, pak);
            pak = NULL;
            return;
        }

        /* 
         * Loop thru setting everything up except ownership
         */
        old_tx_tail = ds->tx_tail;
        mcount = 1;
        first_cl = 0;
 
        while (mp = particle_dequeue_inline(pak), mp) {
 
            tmd = &ds->txr[ds->tx_tail];
            if (PAS_DEBUG_BOOL && (tmd->tx_status_length & MIF68840_TX_OWN)) {
                errmsg(&msgsym(OWNERR, MIF68840), idb->slot, idb->subunit, pak);
            }
 
            /*
             * Give this particle to the ring
             */
            if (mcount == 1) {
 
                /* Add the 3 MIF68840 specific transmit bytes before the FC */
                PUTLONG((mp->data_start - 4), PREMAC);
 
                /* Handle special case of one and only one particle count */
                if (mcount == particle_count) {

                    tmd->tx_buffer_addr = MIF68840_BUF_ADDR(mp->data_start - 3);
                    tmd->tx_status_length = MIF68840_TX_FIRST_LAST_BUFFER |
                                        (mp->data_bytes + 3);
                    if (PAS_DEBUG_BOOL && fddi_debug) {
                        buginf("\n%s: safestart(scfl),sts=0x%08x, addr=0x%08x",
                            idb->hw_namestring, tmd->tx_status_length, 
                            tmd->tx_buffer_addr);
                    }

                    ds->txr_shadow[ds->tx_tail].mp = mp;
                    ds->txr_shadow[ds->tx_tail].pak = MIF68840_MAGIC_PAK_PTR;
 
                } else {
 
                    /* This is first particle but there are more to follow */
                    first_cl = MIF68840_TX_FIRST_BUFFER | (mp->data_bytes + 3);
 
                    tmd->tx_buffer_addr = MIF68840_BUF_ADDR(mp->data_start - 3);
                    ds->txr_shadow[ds->tx_tail].mp = mp;

                    if (PAS_DEBUG_BOOL && fddi_debug) {
                        buginf("\n%s: safestart(scf), sts=0x%08x, addr=0x%08x",
                            idb->hw_namestring, tmd->tx_status_length, 
                            tmd->tx_buffer_addr);
                    }
                }
 
            } else if (mcount == particle_count) {
 
                /* Last particle in packet */
                tmd->tx_buffer_addr = MIF68840_BUF_ADDR(mp->data_start);
                tmd->tx_status_length = MIF68840_TX_LAST_BUFFER | 
                                                    (mp->data_bytes);
                ds->txr_shadow[ds->tx_tail].mp = mp;
                ds->txr_shadow[ds->tx_tail].pak = MIF68840_MAGIC_PAK_PTR;
 
                if (PAS_DEBUG_BOOL && fddi_debug) {
                    buginf("\n%s: safestart(scl), sts=0x%08x, addr=0x%08x",
                        idb->hw_namestring, tmd->tx_status_length, 
                        tmd->tx_buffer_addr);
                }
                /*
                 * The last particle has been written. Go and write the first
                 * tx_status_length
                 */
                tmd1 = &ds->txr[old_tx_tail];
                tmd1->tx_status_length = first_cl;
 
            } else {
 
                /* Intermediate particle in packet */
                tmd->tx_buffer_addr = MIF68840_BUF_ADDR(mp->data_start);
                tmd->tx_status_length = MIF68840_TX_INTERMEDIATE_BUFFER |
                                    (mp->data_bytes);
                ds->txr_shadow[ds->tx_tail].mp = mp;

                if (PAS_DEBUG_BOOL && fddi_debug) {
                    buginf("\n%s: safestart(sci), sts=0x%08x, addr=0x%08x",
                        idb->hw_namestring, tmd->tx_status_length, 
                        tmd->tx_buffer_addr);
                }
            }
 
            /* advance the tail pointer */
            ds->tx_tail = (ds->tx_tail + 1) & (MIF68840_TX_RING_ENTRIES-1);
            ds->tx_count += 1;
            mcount += 1;
        } /* while (mp = particle_dequeue_inline(pak), mp) */

        /* 
         * Do packet accounting
         */
        if (MIF68840_PKT_STATS) {
   
            idb->counters.tx_cumbytes += pak->datagramsize;
            idb->counters.outputs += 1;
            TIMER_START(idb->xmittimer, 3*ONESEC + idb->keep_period*ONESEC);
            if (*(pak->datagramstart) & 0x01) {
                idb->counters.output_broad++;
            }
        }

        /*
         * Give back the pakheader
         */
        datagram_done(pak);

        } /* if (!particle_count) (scattered) */

    } /* while (pak = (*idb->oqueue_dequeue)(idb), pak) */

    /* Do a RING_RDY to ring3 */
    ds->mif68840_regaddr->ifddi[FCR] = WRITE_RING_RDY_REG_3;

    PAS_DEBUG(DAS_DEBUG(0x56785678));
}

/*
 * mif68840_tx_interrupt:
 * Tx interrupt handler for MIF68840.
 */
void mif68840_tx_interrupt (hwidbtype *idb)
{
    register mif68840_instance *ds = INSTANCE(idb);
    particletype *mp;
    mif68840_tx_desc *tmd;
    paktype *pak;
    int i;
    ulong tmd_status;
    register mif68840_tx_shadow *tx_shadow;

    PAS_DEBUG(DAS_DEBUG(0xabcdabcd));

    /*
     * Reclaim transmitted buffers
     */
    for (i=0; i<MIF68840_TX_MAX_SPIN; i++) {

        tx_shadow = &ds->txr_shadow[ds->tx_head];
        mp = tx_shadow->mp;
        pak= tx_shadow->pak;

        if ((mp || pak)) {
       
            /* We have a buffer here */
            tmd = &ds->txr[ds->tx_head];
            tmd_status = tmd->tx_status_length;

            if (tmd_status & MIF68840_TX_OWN)  {

                /* But the MIF68840 still owns it.  */
                break;
            }

            if (PAS_DEBUG_BOOL && fddi_debug)
                buginf("\ntxint, head=%d, tmd=0x%08x, tmd_status = 0x%08x",
                    ds->tx_head, tmd, tmd_status);

            /* advance the head pointer */
            ds->tx_head = (ds->tx_head + 1) & (MIF68840_TX_RING_ENTRIES-1);
  
            /*
             * Check for errors. When we transmit a contigous frame,
             * pak is non-null and mp is null. When we transmit
             * a scattered frame, mp is non-null and pak is null,
             * except when it is the last particle in a frame, when
             * pak gets set to MIF68840_MAGIC_PAK_PTR. So as long as pak is
             * non-null, we can check for tx errors.
             */

            if (pak) {
                if (tmd_status & MIF68840_TX_ERROR) {
                    mif68840_tx_error(tmd_status, idb);
                }
            }

            /*
             * Clear the shadow
             */
            tx_shadow->mp = NULL;
            tx_shadow->pak = NULL;  
           
            /*
             * Do packet Tx accounting and clean up
             */
            if (mp) {
                /*
                 * If pak is also set, then this is a particle that was
                 * the last in an outgoing packet.
                 */
                if (MIF68840_PKT_STATS && pak) {
                    GET_TIMESTAMP(idb->lastoutput);
                }
                ds->tx_count--;
                retparticle_inline(mp);
            } else {
                datagram_done(pak);
                if (MIF68840_PKT_STATS) {
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

    PAS_DEBUG(DAS_DEBUG(0xbbbbbb00 | ds->tx_count));

    /*
     * And finally possibly restart output
     */
    if (idb->output_qcount) {
        mif68840_safe_start(idb);
    }

    PAS_DEBUG(DAS_DEBUG(0xdeadbeef));
}

/*
 * mif68840_tx_start :
 * Start or restart output. Raise interrupt level and call mif68840_safe_start
 */
void mif68840_tx_start (hwidbtype *idb)
{
    leveltype level;
 
    level = raise_interrupt_level(NETS_DISABLE);
    mif68840_safe_start(idb);
    reset_interrupt_level(level);
}

/*
 * mif68840_fastsend:
 * Transmit this packet immediately or enqueue it on the hold q.
 */
void mif68840_fastsend(hwidbtype *idb, paktype *pak)
{
    register mif68840_instance *ds = INSTANCE(idb);
    register mif68840_tx_desc *tmd, *tmd1;
    int empty_desc, old_tx_tail, mcount, particle_count;  
    particletype *mp;
    ulong first_cl;
    traffic_shape_t *shape;

    PAS_DEBUG(DAS_DEBUG(0xaaaaaaaa));

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
	        mif68840_cleanup_pak(pak);
		return;
	    }
	    /*
	     * Transfer necessary fields from old to new pak.
	     * Then flush old pak for reuse and take over new pak.
	     */
	    pas_stuff_pak_for_holdq(newpak, pak);
	    mif68840_flush_pak(pak);
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
        mif68840_output_enqueue(pak, idb);

        /*
         * If the Tx ring is full, then try and clean some stuff up
         * by calling mif68840_tx_interrupt (which calls
         * mif68840_safe_start()).Otherwise just call mif68840_safe_start()
         * directly.
         */
        if (ds->tx_count >= MIF68840_TX_RING_ENTRIES) {
            mif68840_tx_interrupt(idb);
        } else {
            mif68840_safe_start(idb);
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
        if (ds->tx_count >= MIF68840_TX_RING_ENTRIES) {
            /*
             * Since device drivers may be passing statically allocated
             * paktypes thru fast switching code, the paktype that we have here
             * needs to be duplicated.
             */
            mif68840_output_enqueue(pak, idb);
            return;
        }

        /*
         * Check that we own this descriptor
         */
        tmd = &ds->txr[ds->tx_tail];
       
        if (PAS_DEBUG_BOOL && (tmd->tx_status_length & MIF68840_TX_OWN)) {
            errmsg(&msgsym(OWNERR, MIF68840), idb->slot, idb->subunit, pak);
        }
 
        /* Add the three MIF68840 specific transmit bytes before the FC */
        PUTLONG((pak->datagramstart - 4), PREMAC);
 
        /*
         * Setup the descriptor and shadow
         */
        ds->txr_shadow[ds->tx_tail].pak = pak;

        /* ds->txr_shadow[ds->tx_tail].mp is NULL. No need to do it again. */

        tmd->tx_buffer_addr = MIF68840_BUF_ADDR((pak->datagramstart - 3));
        tmd->tx_status_length = MIF68840_TX_FIRST_LAST_BUFFER |
                                (pak->datagramsize + 3);
 
        if (PAS_DEBUG_BOOL && fddi_debug) {
            buginf("\n%s: fastsend(co), sts=0x%08x, addr=0x%08x", 
                idb->hw_namestring, tmd->tx_status_length,tmd->tx_buffer_addr);
        }

        ds->tx_count += 1;
        /* advance the tail pointer */
        ds->tx_tail = (ds->tx_tail + 1) & (MIF68840_TX_RING_ENTRIES-1);
 
    } else {

        /*
         * The packet is scattered. Make sure there is room for it on the
         * Tx ring. If not, queue it.
         */
        empty_desc = MIF68840_TX_RING_ENTRIES - ds->tx_count;
        if (particle_count > empty_desc) {

            PAS_DEBUG(DAS_DEBUG(0xdeaddead));
            /*
             * Since device drivers may be passing statically allocated
             * paktypes thru fast switching code, the paktype that we have here
             * needs to be duplicated.
             */
            mif68840_output_enqueue(pak, idb);
            return;
        }
     
        /* 
         * Loop thru setting everything up except ownership
         */
        old_tx_tail = ds->tx_tail;
        mcount = 1;
        first_cl = 0;
 
        while (mp = particle_dequeue_inline(pak), mp) {
 
            tmd = &ds->txr[ds->tx_tail];
            if (PAS_DEBUG_BOOL && (tmd->tx_status_length & MIF68840_TX_OWN)) {
                errmsg(&msgsym(OWNERR, MIF68840), idb->slot, idb->subunit, pak);
            }
 
            /*
             * Give this particle to the ring
             */
            if (mcount == 1) {
 
                /* Add the 3 MIF68840 specific transmit bytes before the FC */
                PUTLONG((mp->data_start - 4), PREMAC);
 
                /* Handle special case of one and only one particle count */
                if (mcount == particle_count) {
                    tmd->tx_buffer_addr=MIF68840_BUF_ADDR(mp->data_start - 3);
                    tmd->tx_status_length = MIF68840_TX_FIRST_LAST_BUFFER |
                                        (mp->data_bytes + 3);
                    if (PAS_DEBUG_BOOL && fddi_debug) {
                        buginf("\n%s: fastsend(scfl), sts=0x%08x, addr=0x%08x",
                            idb->hw_namestring, tmd->tx_status_length, 
                            tmd->tx_buffer_addr);
                    }
                    ds->txr_shadow[ds->tx_tail].mp = mp;
                    ds->txr_shadow[ds->tx_tail].pak = MIF68840_MAGIC_PAK_PTR;

                    /* advance the tail pointer */
                    ds->tx_tail=(ds->tx_tail+1) & (MIF68840_TX_RING_ENTRIES-1);
                    ds->tx_count += 1;
                    break;
                } else {
 
                    /* This is first particle but there are more to follow */
                    first_cl = MIF68840_TX_FIRST_BUFFER | (mp->data_bytes + 3);
            
                    tmd->tx_buffer_addr = MIF68840_BUF_ADDR(mp->data_start - 3);
                    ds->txr_shadow[ds->tx_tail].mp = mp;

                    if (PAS_DEBUG_BOOL && fddi_debug) {
                        buginf("\n%s: fastsend(scf), sts=0x%08x, addr=0x%08x",
                        idb->hw_namestring, first_cl, tmd->tx_buffer_addr);
                    }

                    /* advance the tail pointer */
                    ds->tx_tail=(ds->tx_tail+1) & (MIF68840_TX_RING_ENTRIES-1);
                    ds->tx_count += 1;
                    mcount += 1;
                }

            } else if (mcount == particle_count) {
 
                /* Last particle in packet */
                tmd->tx_buffer_addr = MIF68840_BUF_ADDR(mp->data_start);
                tmd->tx_status_length = MIF68840_TX_LAST_BUFFER |
                                            mp->data_bytes;
 
                ds->txr_shadow[ds->tx_tail].mp = mp;
                ds->txr_shadow[ds->tx_tail].pak = MIF68840_MAGIC_PAK_PTR;
 
                if (PAS_DEBUG_BOOL && fddi_debug) {
                    buginf("\n%s: fastsend(scl), sts=0x%08x, addr=0x%08x",
                        idb->hw_namestring, tmd->tx_status_length, 
                        tmd->tx_buffer_addr);
                }

                /*
                 * The last particle has been written. Go and write the first
                 * tx_status_length
                 */
                tmd1 = &ds->txr[old_tx_tail];
                tmd1->tx_status_length = first_cl;
 
                /* advance the tail pointer */
                ds->tx_tail = (ds->tx_tail + 1) & (MIF68840_TX_RING_ENTRIES-1);
                ds->tx_count += 1;
                mcount += 1;
                break;

            } else {
 
                /* Intermediate particle in packet */
                tmd->tx_buffer_addr = MIF68840_BUF_ADDR(mp->data_start);
                tmd->tx_status_length = MIF68840_TX_INTERMEDIATE_BUFFER |
                                    (mp->data_bytes);
                ds->txr_shadow[ds->tx_tail].mp = mp;
 
                if (PAS_DEBUG_BOOL && fddi_debug) {
                buginf("\n%s: fastsend(sc), mcount=%d, sts=0x%08x,addr=0x%08x",
                    idb->hw_namestring, mcount, tmd->tx_status_length, 
                    tmd->tx_buffer_addr);
                }

                /* advance the tail pointer */
                ds->tx_tail = (ds->tx_tail + 1) & (MIF68840_TX_RING_ENTRIES-1);
                ds->tx_count += 1;
                mcount += 1;
            }
 
        } /* while (mp = particle_dequeue_inline(pak), mp) */

    } /* if (!particle_count) */

    /* 
     * Do fastsend accounting
     */
    if (MIF68840_PKT_STATS) {
        ulong acct_proto = (ulong)pak->acct_proto;
   
        idb->counters.tx_cumbytes_fast[acct_proto] += pak->datagramsize;
        idb->counters.outputs_fast[acct_proto] += 1;
        TIMER_START(idb->xmittimer, 3*ONESEC + idb->keep_period*ONESEC);
        if (*(pak->datagramstart + 1) & 0x01) {
            idb->counters.output_broad++;
        }
    }

    /* Do a RING_RDY to ring3 */
    ds->mif68840_regaddr->ifddi[FCR] = WRITE_RING_RDY_REG_3;

    PAS_DEBUG(DAS_DEBUG(0xeee00000 | ds->tx_count));
}

/*
 * mif68840_error_interrupt:
 * Error interrupt handler for MIF68840.
 */
void mif68840_error_interrupt (ulong sr1, hwidbtype *idb)
{
    mif68840_instance *ds=INSTANCE(idb);
    mif68840_regs     *ptr=ds->mif68840_regaddr;
 
    if (sr1 & RECEIVE_OV_ERROR_5) {
        FCR_IS_FREE;
        ptr->ifddi[FCR] = WRITE_RING_RDY_REG_5;
    }
 
    if (sr1 & RECEIVE_OV_ERROR_4) {
        FCR_IS_FREE;
        ptr->ifddi[FCR] = WRITE_RING_RDY_REG_4;
        /*
         * The ignore interrupt coming in at the NMI level will
         * slaughter us.  Set a flag and shut it off.
         * If we ever reach process level, the periodic function
         * will revive it. Sorry Curt, the ignore count goes kaput here.
         */
        ptr->mac[INTERRUPT_MASK_REG_B] = 0;
        ds->rxovr_4 = TRUE;
    }
 
 
    if (sr1 & INTERNAL_OP_ERROR) {
        buginf("\nInternal Operation Error, SR1 = 0x%08x", sr1);
        buginf("\nIER =0x%02x",read_fcr(ds, READ_INTERNAL_ERROR_STATUS_REG));
    }
 
    if (PAS_DEBUG_BOOL && fddi_debug) {
        buginf("\n%s: SR1 = 0x%08x", idb->hw_namestring, sr1);
        buginf("\nds->rx_llc_head=%d (0x%08x)", ds->rx_llc_head,
            (ulong)ds->rxr_llc + (ulong)ds->rx_llc_head * 8);
        PAS_DEBUG(DAS_DEBUG(0x22222222));
    }
}
/* FIXME */
void hack (ulong x)
{
    DAS_DEBUG(x);
}
