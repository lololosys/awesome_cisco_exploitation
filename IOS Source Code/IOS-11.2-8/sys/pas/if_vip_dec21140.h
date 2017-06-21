/* $Id: if_vip_dec21140.h,v 3.8.28.9 1996/08/20 18:20:51 tboyle Exp $
 * $Source: /release/112/cvs/Xsys/pas/if_vip_dec21140.h,v $
 *------------------------------------------------------------------
 * if_vip_dec21140.h -- definitions for DEC 21140 100mb Ether PA
 *			(VIP dependent parts)
 *
 * February 1995, Steve Jacobson
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: if_vip_dec21140.h,v $
 * Revision 3.8.28.9  1996/08/20  18:20:51  tboyle
 * CSCdi64994:  vip1 FE output stuck, lost packets, bad dual full-duplex
 * performance
 * Branch: California_branch
 *
 * Revision 3.8.28.8  1996/06/06  12:03:56  dlobete
 * CSCdi57944:  DFS confused about rx ignores and tx drops
 * Branch: California_branch
 *
 * Revision 3.8.28.7  1996/05/31  22:33:46  feisal
 * CSCdi56695:  ISL Broken on certain card
 * Branch: California_branch
 *
 * Revision 3.8.28.6  1996/04/21  00:55:08  getchell
 * CSCdi55156:  VIP2 FE performance suboptimal under heavy load
 * Branch: California_branch
 *
 * Revision 3.8.28.5  1996/04/12  17:43:17  kao
 * CSCdi53527:  Need EOIR support on VIP2
 * Branch: California_branch
 *
 * Revision 3.8.28.4  1996/04/04  19:45:18  tboyle
 * CSCdi51923:  Dynamic particle sizes for VIP2 based on SRAM size
 * Branch: California_branch
 *
 * Revision 3.8.28.3  1996/03/29  00:45:30  sjacobso
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
 * Revision 3.8.28.2  1996/03/21  23:23:53  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.8.28.1  1996/03/18  21:34:47  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.2.10  1996/03/18  23:29:06  mbeesley
 * Branch: ELC_branch
 * Sync to V111_1_3
 *
 * Revision 3.2.2.9  1996/03/07  17:29:08  getchell
 * CSCdi50953:  viper dfs mods
 * Branch: ELC_branch
 *
 * Revision 3.2.2.8  1996/02/10  00:30:55  mbeesley
 * CSCdi48581:  Add VIP2 support to ELC_branch
 * Branch: ELC_branch
 *
 * Revision 3.2.2.7  1996/02/08  08:48:11  gstovall
 * Branch: ELC_branch
 * Sync to ELC_baseline_960205.
 *
 * Revision 3.2.2.6  1996/01/29  19:04:40  wfried
 * Branch: ELC_branch
 * Use HW address filtering, disable rx side for better input throttling,
 * add loopback code, correct ignored frames counting, allow only MII media
 * on the Predator I/O card FE interface.
 *
 * Revision 3.2.2.5  1996/01/18  05:42:05  mbeesley
 * Branch: ELC_branch
 * Optimize 1fe and 4e driver. Fix 4e fastsend bug. Patch in Tx enable
 * fix for 1fe. Debug IP fastswitching.
 *
 * Revision 3.2.2.4  1996/01/16  01:43:36  dcarroll
 * Branch: ELC_branch
 * Sync ELC_branch between ELC_baseline_120995 and ELC_baseline_960110.
 *
 * Revision 3.2.2.3  1995/12/11  19:13:58  mbeesley
 * Branch: ELC_branch
 * Sync ELC_branch to ELC_baseline_120995.
 *
 * Revision 3.2.2.2  1995/12/04  22:18:42  mbeesley
 * Branch: ELC_branch
 * Perform mini-sync - particle refcount changes.
 *
 * Revision 3.2.2.1  1995/11/22  21:42:10  mbeesley
 * Initial commit of c7100 support.
 * Branch: ELC_branch
 *
 * Revision 3.8  1996/01/19  03:03:23  dlobete
 * CSCdi44954:  Local-switching for DFS required
 * But that's not all! Also part of this jumbo Distributed IP
 * Switching commit:
 * - CSCdi43302: Illegal Access to Low Address crashes RVIP/4E (RSP2)
 * - DFS config which mirrors RSP route-cache setting
 *         (ie configurable distributed flow or optimal switching)
 * - Interrupt level IPC updates and invalidates for DFS cache operations
 * - Stay in closer synch with RSP cache for prefix invalidations
 *
 * Revision 3.7  1996/01/13  02:51:47  rharagan
 * CSCdi46869:  Enable memory mapping for VIP
 * Change the default PCI access mode.
 *
 * Revision 3.6  1995/12/10  04:01:28  smackie
 * Add support for public particle pools. Make particle chunk growth less
 * aggressive for dynamic pools. (CSCdi45336)
 *
 * Revision 3.5  1995/12/01  02:00:10  mbeesley
 * CSCdi44698:  PCI subsystem needs improvement
 *
 * Revision 3.4  1995/11/30  03:28:05  mbeesley
 * CSCdi44684:  Rx particle setup incorrect on 1fe, 4e
 *         o Make rx packet start on an 8 byte boundary
 *         o Populate Rx ring from private pool, not global
 *         o Remove previously deleted CVS logs that crept back in
 *
 * Revision 3.3  1995/11/28  03:35:12  getchell
 * CSCdi43281:  rVIP + FE adds xtra 4 Bytes to datagrams, sometimes, under
 * load
 *
 * Revision 3.2  1995/11/17  18:54:52  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:58:02  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:21:36  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/06/07  22:33:14  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#if !defined(DEC21140_STATIC_INLINES_ONLY)

/*
 * Include VIP header files
 */
#include "../src-vip/vip_memd.h"
#include "../src-vip/vip_af.h"

#if defined(SVIP)

/*
 * Define tx ring entries. Must be a power of 2!!
 */
#define DEC21140_TX_RING_ENTRIES       	128
#define DEC21140_TX_RING_INDEX_MASK	0x7f

/*
 * Define rx ring entries. Must be a power of 2!!
 */
#define DEC21140_RX_RING_ENTRIES       	64
#define DEC21140_RX_RING_INDEX_MASK	0x3f

/*
 * Define the minibuffer size used by DEC21140
 */
#define DEC21140_PARTICLE_SIZE		VIP_PARTICLE_BYTES

/*
 * Define the MEMD polling limits. 
 */
#define DEC21140_TXPOLL_HIGH		(DEC21140_TX_RING_ENTRIES - 16)
#define DEC21140_TXPOLL_LOW		(DEC21140_TX_RING_ENTRIES - 24)

/* 
 * Align the receive packet such that the packet can be sniffed and 
 * switched with one cache line
 */

#define DEC21140_RX_ALIGN                 8

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
#define DEC21140_CSR0_PROG_BURST_LEN DEC21140_CSR0_PROG_BURST_LEN_128_BYTES 
#define DEC21140_CSR0_CACHE_ALIGN DEC21140_CSR0_CACHE_ALIGN_128_BYTES
#define DEC21140_CSR0_BAR			0x00000000
 
#else /* RVIP */

/*
 * Define tx ring entries. Must be a power of 2!!
 */
#define DEC21140_TX_RING_ENTRIES	256
#define DEC21140_TX_RING_INDEX_MASK	0xff

/*
 * Define rx ring entries. Must be a power of 2!!
 */
#define DEC21140_RX_RING_ENTRIES	256
#define DEC21140_RX_RING_INDEX_MASK	0xff

/*
 * Define the minibuffer size used by DEC21140
 */
#define DEC21140_PARTICLE_SIZE		512

/*
 * Define the MEMD polling limit. On Checkers stop polling when the
 * tx ring is half full.
 */
#define DEC21140_TXPOLL_HIGH		(DEC21140_TX_RING_ENTRIES/2)
#define DEC21140_TXPOLL_LOW		(DEC21140_TX_RING_ENTRIES/2-8)


#define DEC21140_RX_ALIGN                 8

/*
 * On Checkers use store and forward to avoid causing TXFIFO
 * errors on VIP1 which happens on low loads under full-duplex
 */
#define DEC21140_CSR6_INITIAL_VALUE  				\
			(DEC21140_CSR6_MUST_BE_ONE |		\
			 DEC21140_CSR6_MEDIA_PORT_SCRAMBLE | 	\
			 DEC21140_CSR6_MEDIA_PORT_MII_SYM_PCS |	\
			 DEC21140_CSR6_HEARTBEAT_DISABLE |	\
			 DEC21140_CSR6_PORT_SELECT_MII_SYM |    \
			 DEC21140_CSR6_STORE_AND_FORWARD)

#define DEC21140_CSR6_MICE_INITIAL_VALUE			\
			(DEC21140_CSR6_MUST_BE_ONE |		\
			 DEC21140_CSR6_HEARTBEAT_DISABLE |	\
			 DEC21140_CSR6_PORT_SELECT_MII_SYM |    \
			 DEC21140_CSR6_STORE_AND_FORWARD)

/*
 * dec21140 CSR0 platform specific values
 */
#define DEC21140_CSR0_PROG_BURST_LEN DEC21140_CSR0_PROG_BURST_LEN_32_BYTES 
#define DEC21140_CSR0_CACHE_ALIGN DEC21140_CSR0_CACHE_ALIGN_32_BYTES
#define DEC21140_CSR0_BAR			0x00000002

#endif /* RVIP */

/*
 * Define rx ring attribute
 */
#define DEC21140_RX_RING_ATTRIBUTE    	TLB_UNCACHED

/*
 * Define buffer pool sizes
 */
#define DEC21140_FAST_PARTICLE_POOL	256	/* SRAM particles */
#define DEC21140_SLOW_PARTICLE_POOL	0	/* DRAM particles */

/*
 * Define device default settings
 */
#define DEC21140_DEFAULT_PROM_MODE	FALSE

/*
 * Define Tx/Rx interrupt loop limits
 */
#define DEC21140_RX_MAX_SPIN		DEC21140_RX_RING_ENTRIES
#define DEC21140_TX_MAX_SPIN		DEC21140_TX_RING_ENTRIES

/*
 * Define address conversion macros
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
#define DEC21140_LOCAL_KEEPALIVE      	FALSE

/*
 * Defines to control default RJ45/MII and half/full duplex
 */
#define DEC21140_DEFAULT_RJ45MII	FEIP_RJ45
#define DEC21140_DEFAULT_FULLDUPLEX	FALSE

/*
 * Define memory mapped IO for VIP
 */
#define DEC21140_MEMMAPPED_IO		TRUE

/*
 * Do not do any packet accounting on the VIP (host does it)
 */
#define DEC21140_PKT_STATS	     	FALSE

/*
 * Never throttle on a VIP card
 */
#define DEC21140_THROTTLING	    	FALSE

/**********************************************************************
 * DEC21140 Static inlines
 **********************************************************************/

#else /* DEC21140_STATIC_INLINES_ONLY */

/*
 * dec21140_get_particle_inline:
 * Get an rx particle.
 */
static inline particletype *dec21140_get_particle_inline (hwidbtype *idb, 
							  boolean cache_ok)
{
    particletype *mp;

    /*
     * Try the cache
     */
    mp = pool_dequeue_cache(idb->pool);
    if (mp) {
	/*
	 * Initialize start. 
	 */
	mp->data_start = (uchar *)mp->data_block + PARTICLE_ENCAPBYTES - 
	                                                    DEC21140_RX_ALIGN;
	return(mp);
    }

    /*
     * We did not get one from the cache, try a global pool
     */
    mp = pool_getparticle(idb->pool->fallback);

    /*
     * If we still do not have one, mark it as a nobuffer.
     */
    if (mp) {
	/*
	 * Initialize start. 
	 */
	mp->data_start = (uchar *)mp->data_block + PARTICLE_ENCAPBYTES - 
	                                                    DEC21140_RX_ALIGN;
	return(mp);
    } else {
	idb->counters.output_nobuffers++;
	return(NULL);
    }
}

/*
 * dec21140_dfs_enabled:
 * Return TRUE if we are willing to DFS on this interface.
 */
static inline boolean dec21140_dfs_enabled (hwidbtype *idb)
{
    return(idb->ip_routecache & IP_DISTRIBUTED_ROUTECACHE);
}

/*
 * dec21140_ip_dfs:
 * Distributely fastswitch an IP datagram
 */
static inline boolean dec21140_ip_dfs (hwidbtype *idb, paktype *pak)
{
    pas_cbtype *pascb = INSTANCE(idb);

    return((*pascb->pak_dfs)(idb, pak));
}

/*
 * dec21140_process_receive_packet:
 * We have a good, completed Rx packet. Do something with it.
 */
static inline void dec21140_process_receive_packet (hwidbtype *idb,
						    dec21140_instance_t *ds,
						    paktype *rx_pak,
						    ulong rxd_status)
{
    CONVERT_PARTICLE_ADDR(rx_pak->datagramstart);

    /*
     * First thing to do is filter and classify
     */
    rx_pak->mci_status = vip_ether_filter_classify(
						   rx_pak->datagramstart,
						   ds->pacb.vip_af,
						   ds->pacb.vip_af_entries,
						   rxd_status);
    if (ether_debug) {
	buginf("DEC21140: mci_status=%#x, rx_pak->datagramstart=%#x\n", 
	       rx_pak->mci_status, rx_pak->datagramstart);
    }
    if (!rx_pak->mci_status) {
	/*
	 * Drop the frame
	 */
	ds->filtered_pak += 1;
	dec21140_cleanup_pak(rx_pak);
	return;
    }

    /*
     * Check if DFS support is enabled
     */
    if (dec21140_dfs_enabled(idb) &&
        ((rx_pak->mci_status & AF_DODIP_STATION) == AF_DODIP_STATION)) {
 
        /*
         * Perform IP fast switching on the packet. There is several ways
         * this can be done -
         *
         * - reg_invoke_ether_fs(RXTYPE_DODIP, rx_pak)
         *      This is most elegant, but would probably require
         *      a set_if_input_inline() on the rx_pak and other such stuff
         *      to allow the DFS routine to know the inbound interface
         *
         * - Hard call to the IP DFS routine
         *
         * - Function vector in either pas_cbtype or vip_memd_ifcntl_t
         *   to the IP DFS routine.
         *
         * If the DFS routine returns TRUE, we are done with this packet.
         * If it returns FALSE, then we should punt it to the host
         */
	vip_cache_invalidate_line(rx_pak->datagramstart + 40);
        rx_pak->network_start = rx_pak->datagramstart + ETHER_ARPA_ENCAPBYTES;
        if (!dec21140_ip_dfs(idb, rx_pak)) {
	    dec21140_cleanup_pak(rx_pak);
	}
	dec21140_flush_pak(rx_pak);
	return;
    }

    /*
     * Either DFS is turned off or the packet was not a supported
     * protocol. Just punt the frame to the host
     */
    if (!(*ds->pacb.pak_to_host)(idb, &ds->pacb, rx_pak)) {
	idb->counters.input_drops += 1;
	dec21140_cleanup_pak(rx_pak);
    }
    dec21140_flush_pak(rx_pak);
}

/*
 * dec21140_output_enqueue:
 * Platform specific routine to enqueue a packet onto
 * the output holdq. On checkers, if the packet is scattered we reparent
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
    } else {
	newpak = pak;
    }
    newpak->if_output = idb->firstsw;
    /*
     * Invalidate the accounting protocol type to prevent
     * packet accounting in holdq_enqueue_fast() if the accounting
     * flag is not set.
     */
    if (!DEC21140_PKT_STATS)
        newpak->acct_proto = ACCT_PROTO_LAST;
    holdq_enqueue_fast(idb, newpak);
}

/*
 * dec21140_snoop_txisl_frames:
 * check the packet to see if its an ISL packet and mark it as such
 */
static inline void dec21140_snoop_txisl_frames (pas_cbtype *pacb,
						paktype *pak)
{
    if (pacb->isl_enabled && vip_pak_is_isl(pak, NULL, ISL_COMPLETE)) {
	pak->flags |= PAK_IS_ISL;
    }
}

#endif /* DEC21140_STATIC_INLINES_ONLY */

/* end of file */
