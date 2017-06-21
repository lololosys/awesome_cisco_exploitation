/* $Id: if_c7100_ibm2692.h,v 3.1.60.3 1996/06/07 23:52:16 sthormod Exp $
 * $Source: /release/112/cvs/Xsys/pas/if_c7100_ibm2692.h,v $
 *------------------------------------------------------------------
 * if_c7100_ibm2692.h - IBM2692 4/16 Mbit Token Ring PCI PA
 *		        (Predator dependent parts)
 *
 * December 1995, HC Wang and Stig Thormodsrud
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: if_c7100_ibm2692.h,v $
 * Revision 3.1.60.3  1996/06/07  23:52:16  sthormod
 * CSCdi57928:  OIR of 4R Wedges Console (No Throttling Code)
 * Branch: California_branch
 *
 * Revision 3.1.60.2  1996/04/23  02:23:52  sthormod
 * CSCdi54994:  4R uses cpu to byte-swap
 * Branch: California_branch
 *
 * Revision 3.1.60.1  1996/03/21  23:22:49  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.1.2.5  1996/03/10  02:49:49  sthormod
 * CSCdi51196:  add tbridge switching path for predator 4R
 * Branch: ELC_branch
 *
 * Revision 3.1.2.4  1996/03/06  19:51:25  sthormod
 * CSCdi50859:  add srb switching path for predator 4R
 * Branch: ELC_branch
 * o  process & fast switching path for srb & explorers.
 * o  routing with rif field.
 * o  use dma to coalesce particle based packet.
 * o  process mac frames.
 *
 * Revision 3.1.2.3  1996/02/21  22:45:52  sthormod
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
 * Revision 3.1.2.2  1996/01/31  01:37:22  sthormod
 * Branch: ELC_branch
 * Add Predator 4R support.
 *
 * Revision 3.1.2.1  1995/12/19  23:32:29  sthormod
 * Branch: ELC_branch
 * Initial structure changes for porting 4R driver to predator.
 *
 * Revision 3.1  1995/12/19  20:55:14  sthormod
 * Placeholder for new development.
 *
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#if !defined(IBM2692_STATIC_INLINES_ONLY)

/*
 * Define rx/tx ring entries. Must be a power of 2!!
 */
#define IBM2692_TX_RING_ENTRIES		128
#define IBM2692_TX_RING_INDEX_MASK	(IBM2692_TX_RING_ENTRIES - 1)

#define IBM2692_RX_RING_ENTRIES		64
#define IBM2692_RX_RING_INDEX_MASK	(IBM2692_RX_RING_ENTRIES - 1)

/*
 * Define the minibuffer size used by IBM2692
 */
#define IBM2692_PARTICLE_SIZE 512

/*
 * Define address conversion macro's
 */ 
#define IBM2692_BUF_ADDR(x)     (((ulong)x & PCISRAM_ADDRESS_MASK) ?   \
                                ((ulong)x | PCISRAM_SWAP_MASK) :       \
                                ((ulong)x | PCIDRAM_SWAP_MASK))
#define PCI_VIEW_ADDR(x)	((ulong)(x))
#define CPU_VIEW_ADDR(x)	((ulong)(x))

/*
 * Define to control generation of keepalives
 */
#define IBM2692_LOCAL_KEEPALIVE      	TRUE

/*
 * Define to control bridging
 */
#define IBM2692_LOCAL_BRIDGE            TRUE

/*
 * Define IO mapped IO for VIP
 */
#define HERMON_MEMMAPPED_IO             TRUE

/*
 * Have the driver do packet accounting
 */
#define IBM2692_PKT_STATS		TRUE

/*
 * Throttle the interface if the Predator runs out of buffers
 */
#define IBM2692_THROTTLING              TRUE
#define IBM2692_BUFFER_THRESHOLD        4


/* 
 * Due to hermon bug, it can not be configured to have base address 
 * between 0x00 - 0x3ff
 */
#define PCI_IBM2692_DEVICE_BASE_OFFSET	0x400

/**********************************************************************
 * IBM2692 Static inlines
 **********************************************************************/

#else /* IBM2692_STATIC_INLINES_ONLY */

/*
 * Note: The routines ibm2692_particle_enqueue_inline() and
 *       ibm2692_transmit_inline() appear in both if_c7100_ibm2692.h
 *       and if_vip_ibm2692.h and yet they are essentially duplicates
 *       of each other.  This original reason for this was to handle
 *       the predator rev1 cpu that needed to have the cpu do the
 *       byte-swapping.  With the rev2 cpu we can let the hardware do
 *       the byte-swap (see CSCdi54994), but for now we'll leave the
 *       structural changes in place in case future platforms need to
 *       do cpu byte-swap.
 */

/*
 * ibm2692_particle_enqueue_inline
 */
static inline void ibm2692_particle_enqueue_inline (paktype *pak,
						    particletype *particle)
{
    particle_enqueue_inline(pak, particle);
}

/*
 * ibmtr_transmit_inline 
 */
static inline void ibm2692_transmit_inline (hwidbtype *idb,
					    paktype *pak, boolean release)
{
    int i, particle_count;
    tx_bfr_desc_t *tx_desc;
    tx_desc_shadow_t *tx_shadow_desc;
    ibm2692_instance_t *ds = INSTANCE(idb);
    hermon_reg_map *hermon_ptr;
    particletype *mp;

    hermon_ptr = ds->ibm2692_ptr;
    /*
     * if we have room on the transmit descriptor list
     */
    if (ds->tx_outstanding < IBM2692_TX_RING_ENTRIES) {
	tx_shadow_desc = &(ds->tx_desc_shadow[ds->tx_inptr]);

	/*
	 * If we're tbridging, we need to keep track of the source
	 * addrs so that when we recieve a broadcast we can know if it
	 * was transmitted by us.  
	 */
	if (idb->tr_ring_mode & RINGMODE_TBRIDGE) {
	    ieee_copy(((tring_hdr*)pak->datagramstart)->saddr, 
		      &tx_shadow_desc->tb_saddr[0]);
	}

	/*
	 * check for contigous packet, if so particle count == 0
	 */
	particle_count = pak_particle_count(pak);
	if (!particle_count) {
	    tx_desc = tx_shadow_desc->cpu_view_addr;
	    tx_shadow_desc->tx_pak = pak;
	    tx_desc->tx_status = 0x0;
	    /*
	     * Hermon tx bug, need to append one zero length buffer
	     * at the end of packet
	     */
	    tx_desc->bfr_count = 2;
	    tx_desc->frame_length = pak->datagramsize;
	    tx_desc->bfr[0].bptr = (uchar *)IBM2692_BUF_ADDR(pak->datagramstart);
	    tx_desc->bfr[0].b_count = pak->datagramsize;
	    /*
	     * use init block as this dummy buffer
	     */
	    tx_desc->bfr[1].bptr = (uchar *)IBM2692_BUF_ADDR(ds->mpc_init_bptr);
	    tx_desc->bfr[1].b_count = 0;
	    ibm2692_queue_tx_desc(idb);
	    return;
	} else {
	    tx_desc = tx_shadow_desc->cpu_view_addr;
	    tx_shadow_desc->tx_pak = NULL;
	    tx_desc->tx_status = 0x0;
	    /*
	     *  Hermon tx bug, need to append one zero length buffer
	     * at the end of packet
	     */
	    tx_desc->bfr_count = particle_count + 1;
	    tx_desc->frame_length = pak->datagramsize;
	    for (i = 0; i < particle_count; i++) {
	       mp = dequeue_inline(&pak->particlequeue);
	       tx_desc->bfr[i].bptr = (uchar *)IBM2692_BUF_ADDR(mp->data_start);
	       tx_shadow_desc->particle[i] = mp;
	       tx_desc->bfr[i].b_count = mp->data_bytes;
	    }
	    tx_desc->bfr[i].b_count = 0;
	    tx_desc->bfr[i].bptr = (uchar *)IBM2692_BUF_ADDR(ds->mpc_init_bptr);
	    ibm2692_queue_tx_desc(idb);
	    if (release) {
		datagram_done(pak);
	    }
	    return;
	}
    } else {
	ibm2692_output_enqueue(pak, idb);
    }
}

#endif /* IBM2692_STATIC_INLINES_ONLY */

/* end of file */
