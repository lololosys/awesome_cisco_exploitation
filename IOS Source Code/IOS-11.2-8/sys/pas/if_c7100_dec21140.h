/* $Id: if_c7100_dec21140.h,v 3.1.68.9 1996/08/28 07:45:55 abaindur Exp $
 * $Source: /release/112/cvs/Xsys/pas/if_c7100_dec21140.h,v $
 *------------------------------------------------------------------
 * if_c7100_dec21140.h -- definitions for DEC 21140 100mb Ether PA
 *			  (Predator dependent parts)
 *
 * November 1995, Walter Friedrich
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: if_c7100_dec21140.h,v $
 * Revision 3.1.68.9  1996/08/28  07:45:55  abaindur
 * CSCdi67069:  Data cache use needs to be turned ON for c7200 IP flow
 * switching
 * Branch: California_branch
 *  Fix mixup of cached and uncached region pointers. Fix cases where
 *  converted addresses were not restored if flow switching failed.
 *
 * Revision 3.1.68.8  1996/08/24  01:50:52  abaindur
 * CSCdi66979:  Traffic shaping broken on the c7200
 * Branch: California_branch
 *
 * Revision 3.1.68.7  1996/08/20  18:20:42  tboyle
 * CSCdi64994:  vip1 FE output stuck, lost packets, bad dual full-duplex
 * performance
 * Branch: California_branch
 *
 * Revision 3.1.68.6  1996/07/20  14:59:04  ssangiah
 * CSCdi63599:  Configuring ISL bridging crashes the c7200.
 * Branch: California_branch
 *
 * Revision 3.1.68.5  1996/06/21  21:24:40  mbeesley
 * CSCdi59509:  FastEther init code can loop forever
 * Branch: California_branch
 *
 * Revision 3.1.68.4  1996/05/31  22:33:34  feisal
 * CSCdi56695:  ISL Broken on certain card
 * Branch: California_branch
 *
 * Revision 3.1.68.3  1996/05/24  00:49:35  mbeesley
 * CSCdi58316:  Need to fully support CPQ on new interfaces
 * Branch: California_branch
 *
 * Revision 3.1.68.2  1996/05/07  00:23:53  mbeesley
 * CSCdi56157:  Enable IP Flow/Optimum on Ethernets
 * Branch: California_branch
 *
 * Revision 3.1.68.1  1996/03/21  23:22:46  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.1.2.11  1996/03/18  23:28:33  mbeesley
 * Branch: ELC_branch
 * Sync to V111_1_3
 *
 * Revision 3.1.2.10  1996/03/07  00:19:12  mbeesley
 * CSCdi50649:  DEC21140 in Store-and-Forward mode
 *         o Put device in tx cut-thru, 512 byte tx fifo threshold
 *         o Increase rx pool, eliminate use of fallback pool
 *         o Handle rx fifo overrun and error interrupt correctly
 * Branch: ELC_branch
 *
 * Revision 3.1.2.9  1996/03/03  08:40:15  wfried
 * CSCdi50555:  Fast Ether multicast filtering broken and unthrottling not
 * working
 * Branch: ELC_branch
 * Avoid multiple setting of single multicast address in hw addr filter,
 * register pool cache throttling callback function.
 *
 * Revision 3.1.2.8  1996/02/19  23:23:54  mbeesley
 * CSCdi49335:  Predator needs support for DMA driven coalesce
 * Branch: ELC_branch
 *
 * Revision 3.1.2.7  1996/02/10  00:30:30  mbeesley
 * CSCdi48581:  Add VIP2 support to ELC_branch
 * Branch: ELC_branch
 *
 * Revision 3.1.2.6  1996/02/01  07:54:43  mbeesley
 * CSCdi47969:  Allow coalesced fastswitching on ethernet
 *         o Let 4e/1fe driver coalesce frames before fs
 *         o Speed up 1fe tx particle reclaimation
 *         o Remove min length check on 1fe (chip pads)
 *         o Put 1fe rx particles on a more PCI bus friendly alignment
 * Branch: ELC_branch
 *
 * Revision 3.1.2.5  1996/01/29  19:04:23  wfried
 * Branch: ELC_branch
 * Use HW address filtering, disable rx side for better input throttling,
 * add loopback code, correct ignored frames counting, allow only MII media
 * on the Predator I/O card FE interface.
 *
 * Revision 3.1.2.4  1996/01/18  05:41:39  mbeesley
 * Branch: ELC_branch
 * Optimize 1fe and 4e driver. Fix 4e fastsend bug. Patch in Tx enable
 * fix for 1fe. Debug IP fastswitching.
 *
 * Revision 3.1.2.3  1996/01/16  01:42:19  dcarroll
 * Branch: ELC_branch
 * Sync ELC_branch between ELC_baseline_120995 and ELC_baseline_960110.
 *
 * Revision 3.1.2.2  1995/12/12  04:53:55  mbeesley
 * Branch: ELC_branch
 * Allow "show pci bridge" to display all bridges. Change the 1fe GPC
 * register settings for Predator IO card 1fe. Fix some amdp2 error
 * messages. And turn power off on unrecognized port adaptors.
 *
 * Revision 3.1.2.1  1995/11/22  21:41:38  mbeesley
 * Initial commit of c7100 support.
 * Branch: ELC_branch
 *
 * Revision 3.1  1995/11/20  22:07:21  mbeesley
 * Add placeholder files for future development.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#if !defined(DEC21140_STATIC_INLINES_ONLY)

/*
 * Include flow/optimum switching header files
 */
#include "../ipmulticast/igmp.h"
#include "../ip/ipfast_flow.h"

/*
 * Define tx ring entries. Must be a power of 2!!
 */
#define DEC21140_TX_RING_ENTRIES	256
#define DEC21140_TX_RING_INDEX_MASK	0xff

/*
 * Define rx ring entries. Must be a power of 2!!
 */
#define DEC21140_RX_RING_ENTRIES	128
#define DEC21140_RX_RING_INDEX_MASK	0x7f

/*
 * Define 21140 configuration modes
 */
#define DEC21140_CSR6_INITIAL_VALUE  				\
			(DEC21140_CSR6_MUST_BE_ONE |		\
			 DEC21140_CSR6_MEDIA_PORT_SCRAMBLE | 	\
			 DEC21140_CSR6_MEDIA_PORT_MII_SYM_PCS |	\
			 DEC21140_CSR6_HEARTBEAT_DISABLE |	\
			 DEC21140_CSR6_PORT_SELECT_MII_SYM |    \
			 DEC21140_CSR6_TX_TR_512)

#define DEC21140_CSR6_MICE_INITIAL_VALUE			\
			(DEC21140_CSR6_MUST_BE_ONE |		\
			 DEC21140_CSR6_HEARTBEAT_DISABLE |	\
			 DEC21140_CSR6_PORT_SELECT_MII_SYM |    \
			 DEC21140_CSR6_TX_TR_512)

/*
 * dec21140 CSR0 platform specific values
 */
#define DEC21140_CSR0_PROG_BURST_LEN DEC21140_CSR0_PROG_BURST_LEN_32_BYTES 
#define DEC21140_CSR0_CACHE_ALIGN DEC21140_CSR0_CACHE_ALIGN_32_BYTES
#define DEC21140_CSR0_BAR			0x00000002

/*
 * Define rx ring attribute
 */
#define DEC21140_RX_RING_ATTRIBUTE    	TLB_UNCACHED

/*
 * Define buffer pool sizes. This is to accomodate a full
 * recieve ring, a full 1fe transmit ring and a full output
 * hold queue assuming 40 packet limit, each packet with three
 * particles (mtu sized) without throttling.
 */
#define DEC21140_FAST_PARTICLE_POOL	400	/* SRAM particles */

/*
 * Define the minibuffer size used by DEC21140
 */
#define DEC21140_PARTICLE_SIZE		512

/*
 * Define device default settings
 */
#define DEC21140_DEFAULT_PROM_MODE     FALSE

/*
 * Define Tx/Rx interrupt loop limits
 */
#define DEC21140_RX_MAX_SPIN		DEC21140_RX_RING_ENTRIES
#define DEC21140_TX_MAX_SPIN		DEC21140_TX_RING_ENTRIES

/*
 * Define address conversion macro's
 */
#define DEC21140_BUF_ADDR(x)		((ulong)(x))
#define DEC21140_CNTL_ADDR(x)		((ulong)(x))

/*
 * Define constants to control endianess of shared data
 */
#define DEC21140_RING_BIGENDIAN		TRUE
#define DEC21140_IB_BIGENDIAN		TRUE
#define DEC21140_DMA_BIGENDIAN		TRUE

/*
 * Define to control generation of keepalives
 */
#define DEC21140_LOCAL_KEEPALIVE      	TRUE

/*
 * Defines to control default RJ45/MII and half/full duplex
 */
#define DEC21140_DEFAULT_RJ45MII	FEIP_RJ45
#define DEC21140_DEFAULT_FULLDUPLEX	FALSE

/*
 * Define Memory mapped IO for Predator
 */
#define DEC21140_MEMMAPPED_IO		TRUE

/*
 * Have the driver do packet accounting
 */
#define DEC21140_PKT_STATS	       	TRUE

/*
 * Throttle the interface if the Predator runs out of buffers
 */
#define DEC21140_THROTTLING		TRUE
#define DEC21140_BUFFER_THRESHOLD	14

/*
 * Prototypes
 */
extern void	dec21140_post_coalesce_rx(hwidbtype *, paktype *, iqueue_t iq);
extern void	dec21140_post_coalesce_fs(hwidbtype *, paktype *, iqueue_t iq);

/**********************************************************************
 * DEC21140 Static inlines
 **********************************************************************/

#else /* DEC21140_STATIC_INLINES_ONLY */

/*
 * Define macro form lineproto change. It is not used on Predator.
 */
#define dec21140_lineproto_change(a, b)

/*
 * dec21140_get_particle_inline:
 * Get an rx particle.
 */
static inline particletype *dec21140_get_particle_inline (hwidbtype *idb, 
							  boolean cache_ok)
{
    particletype *mp;

    /*
     * Try the buffer cache.
     */
    mp = pool_dequeue_cache(idb->pool);

    /*
     * If we do not have one, mark it as a nobuffer.
     */
    if (mp) {
	/*
	 * Initialize start. Note, the DEC21140 requires long word aligned
	 * buffers. And the PCI buses are much more efficient if things
	 * are aligned on a 32 byte boundary. So set the start 28 bytes into
	 * the particle.
	 */
	mp->data_start = mp->data_block->data_area + (PARTICLE_ENCAPBYTES - 4);
	return(mp);
    } else {
	idb->counters.output_nobuffers++;
	return(NULL);
    }
}

/*
 * dec21140_process_receive_packet:
 * We have a good, completed Rx packet. Do something with it.
 */
static inline void dec21140_process_receive_packet (hwidbtype *idb,
						    dec21140_instance_t *ds,
						    paktype *rx_pak,
						    int pak_size)
{
    ether_hdr *ether;

    /*
     * If bridging is turned off and flow switching is enabled
     * then try and optimum/flow switch this packet if it is IP
     */
    if ((idb->ip_routecache & (IP_FLOW_ROUTECACHE|IP_OPTIMUM_ROUTECACHE)) && 
	!idb->tbridge_on_hwidb) {

	uchar 	  *start, *old_start;
	iphdrtype *ip;

	old_start = rx_pak->datagramstart;
	if (C7100_FLOW_THRU_DCACHE) {
	    /*
	     * Convert datagramstart pointer to cached, write thru address
	     * and flush the first 3 lines of the packet from the data
	     * cache (ensuring no stale data from a previous packet)
	     */
	    start = CONVERT_ADDR_CWT(old_start);
	    invalidate_data_lines(start, 3);
	} else {
	    start = old_start;
	}

	/*
	 * Check that it is IP
	 */
	ether = (ether_hdr *)start;
	if (ether->type_or_len == TYPE_IP10MB) {

	    ip = (iphdrtype *)ether->data;
	    rx_pak->datagramstart = start;
	    if ((*idb->ip_turbo_fs)(rx_pak, ip, idb) == IPFLOWOK) {
		dec21140_flush_pak(rx_pak);
		return;
	    }
	    rx_pak->datagramstart = old_start;
	}
    }

    /*
     * Classify the frame
     */
    rx_pak->rxtype = ether_decode_encapsulation_inline(rx_pak,
		         (ether_hdr *)rx_pak->datagramstart);

    /*
     * Default the input software idb to the major
     * interface.  Subsequent tbridge code may determine
     * a different value if sub-interface bridging is
     * configured.
     *
     * Note, we do not increment the input qcount here. The packet will not
     * go onto the input queue until it is colaesced. The contigous
     * packet will then be marked with PAK_INPUTQ and the input qcount will
     * increment as long as this interface's limit is not exceeded.
     */
    rx_pak->if_input = idb->firstsw;
    
    /*
     * If this interface is listening promiscuously, throw the packet at 
     * the software MAC filter in the tbridge code where the bridge versus 
     * route decision will be made. Do this only for non-ISL frames since 
     * the bridging code expects the ISL frames to have the ISL header
     * stripped off. Bridging of ISL frames is handled by the callback
     * for the ether fast-switch registry.
     */
    if (rx_pak->rxtype != RXTYPE_ISL) {
        if (idb->tbridge_on_hwidb && reg_invoke_tbridge_forward(rx_pak)) {
    	    dec21140_flush_pak(rx_pak);
	    return;
        }
    }

    /*
     * If the packet is for a protocol that is not presently fastswitchable
     * as a particled packet, coalesce it before doing further processing.
     * If it is a supported protocol, try and fastswitch it in particles.
     */
    if (c7100_scattered_fs_protocol(rx_pak->rxtype)) {
	if (reg_invoke_ether_fs(rx_pak->rxtype, rx_pak)) {
	    dec21140_flush_pak(rx_pak);
	    return;
	}
    } else {
	ether = (ether_hdr *)(rx_pak->datagramstart);
	if (!c7100_dma_pak_coalesce(idb, rx_pak, dec21140_post_coalesce_fs,
				    (uchar *)(&ether->type_or_len))) {
	    dec21140_throttle(idb);
	}
	return;
    }

    /*
     * Coalesce the packet and send it on
     */
    ether = (ether_hdr *)(rx_pak->datagramstart);
    if (!c7100_dma_pak_coalesce(idb, rx_pak, dec21140_post_coalesce_rx,
				(uchar *)(&ether->type_or_len))) {
	dec21140_throttle(idb);
    }
}

/*
 * dec21140_output_enqueue:
 * Platform specific routine to enqueue a packet onto
 * the output holdq. On Predator, if the packet is scattered we reparent
 * and enqueue it as particles. If it is not scattered, it is queued directly.
 */
static inline void dec21140_output_enqueue (paktype *pak, hwidbtype *idb)
{
    paktype *newpak;
   
    if (pak_has_particles(pak)) {
	/*
	 * Reparent onto a new paktype header as we can not hold onto
	 * the one passed into us.
	 */
	newpak = particle_reparent(pak, idb->headerpool);
	if (!newpak) {
	    dec21140_cleanup_pak(pak);
	    idb->counters.output_total_drops++;
	    return;
	}
	newpak->flags = pak->flags;
	if (idb->priority_list) {
	    pas_stuff_pak_for_holdq(newpak, pak);
	}
    } else {
	newpak = pak;
    }
    /*
     * Invalidate the accounting protocol type to prevent
     * duplicate packet accounting in holdq_enqueue_fast() if
     * accounting flag is not set.
     */
    if (!DEC21140_PKT_STATS)
	newpak->acct_proto = ACCT_PROTO_LAST;
    holdq_enqueue_fast(idb, newpak);
}	

/*
 * dec21140_snoop_txisl_frames:
 * Do nothing for predator. We do not need to snoop ISL frames on Tx
 * as the fast/process switching code will have set pak->flags already.
 */
static inline void dec21140_snoop_txisl_frames (pas_cbtype *pacb,
                                                paktype *pak)
{
}

#endif /* DEC21140_STATIC_INLINES_ONLY */

/* end of file */
