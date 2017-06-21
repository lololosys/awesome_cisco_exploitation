/* $Id: if_vip_amdp2.h,v 3.9.12.10 1996/06/06 12:03:54 dlobete Exp $
 * $Source: /release/112/cvs/Xsys/pas/if_vip_amdp2.h,v $
 *------------------------------------------------------------------
 * if_vip_amdp2.h - 8e Presidio2 based PCI port adaptor support
 *		    (VIP specific components)
 *
 * April 1995, Dave Turner
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: if_vip_amdp2.h,v $
 * Revision 3.9.12.10  1996/06/06  12:03:54  dlobete
 * CSCdi57944:  DFS confused about rx ignores and tx drops
 * Branch: California_branch
 *
 * Revision 3.9.12.9  1996/05/07  01:22:41  mbeesley
 * CSCdi56710:  Correct AMDp2 fifo setting, update error messages
 * Branch: California_branch
 *
 * Revision 3.9.12.8  1996/04/23  17:13:48  gshen
 * CSCdi55375:  Need to have different address separation for IO and
 * memory based IO
 * Branch: California_branch
 *
 * Two new I/O address separation definitions:
 *
 * #define PCI_DEV_IO_MAPPED_IO_SEPARATION  0x0400 / * 1k * /
 * #define PCI_DEV_MEM_MAPPED_IO_SEPARATION 0x4000 / * 16k * /
 *
 * These changes have been code reviewed.
 *
 * Revision 3.9.12.7  1996/04/12  17:43:15  kao
 * CSCdi53527:  Need EOIR support on VIP2
 * Branch: California_branch
 *
 * Revision 3.9.12.6  1996/04/04  19:45:17  tboyle
 * CSCdi51923:  Dynamic particle sizes for VIP2 based on SRAM size
 * Branch: California_branch
 *
 * Revision 3.9.12.5  1996/03/30  03:13:23  sjacobso
 * CSCdi52412:  4e/8e: Resolve code review comment
 * Branch: California_branch
 * Convert code in #if defined(PREDATOR) construct to
 * platform dependent macro.
 *
 * Revision 3.9.12.4  1996/03/29  04:05:52  sjacobso
 * CSCdi52266:  4E/8E receive promiscuously, hurting performance
 * Branch: California_branch
 * Change write_address_filter code to examine address filter
 * entries for important status, and to store that status in the
 * software address filter.  The correct common routines for
 * adding MAC addresses are called.  Also, fix Predator logical
 * address filter code to work correctly with HSRP addresses.
 *
 * Revision 3.9.12.3  1996/03/29  00:45:22  sjacobso
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
 * Revision 3.9.12.2  1996/03/21  23:23:49  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.9.12.1  1996/03/18  21:34:43  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.2.10  1996/03/18  23:29:05  mbeesley
 * Branch: ELC_branch
 * Sync to V111_1_3
 *
 * Revision 3.2.2.9  1996/03/07  17:29:03  getchell
 * CSCdi50953:  viper dfs mods
 * Branch: ELC_branch
 *
 * Revision 3.2.2.8  1996/02/10  00:30:50  mbeesley
 * CSCdi48581:  Add VIP2 support to ELC_branch
 * Branch: ELC_branch
 *
 * Revision 3.2.2.7  1996/02/08  08:48:10  gstovall
 * Branch: ELC_branch
 * Sync to ELC_baseline_960205.
 *
 * Revision 3.2.2.6  1996/01/19  20:03:48  mbeesley
 * Branch: ELC_branch
 * Make "show bridge" more usefull. Remove unused static inline
 * for AMDP2 driver, fix process level packet accounting.
 *
 * Revision 3.2.2.5  1996/01/18  05:42:03  mbeesley
 * Branch: ELC_branch
 * Optimize 1fe and 4e driver. Fix 4e fastsend bug. Patch in Tx enable
 * fix for 1fe. Debug IP fastswitching.
 *
 * Revision 3.2.2.4  1996/01/16  01:43:29  dcarroll
 * Branch: ELC_branch
 * Sync ELC_branch between ELC_baseline_120995 and ELC_baseline_960110.
 *
 * Revision 3.2.2.3  1995/12/11  19:13:57  mbeesley
 * Branch: ELC_branch
 * Sync ELC_branch to ELC_baseline_120995.
 *
 * Revision 3.2.2.2  1995/12/04  22:18:38  mbeesley
 * Branch: ELC_branch
 * Perform mini-sync - particle refcount changes.
 *
 * Revision 3.2.2.1  1995/11/22  21:42:08  mbeesley
 * Initial commit of c7100 support.
 * Branch: ELC_branch
 *
 * Revision 3.9  1996/02/18  19:42:21  kao
 * CSCdi45136:  EOIR Problems with VIP card
 *
 * Revision 3.8  1996/01/19  03:03:21  dlobete
 * CSCdi44954:  Local-switching for DFS required
 * But that's not all! Also part of this jumbo Distributed IP
 * Switching commit:
 * - CSCdi43302: Illegal Access to Low Address crashes RVIP/4E (RSP2)
 * - DFS config which mirrors RSP route-cache setting
 *         (ie configurable distributed flow or optimal switching)
 * - Interrupt level IPC updates and invalidates for DFS cache operations
 * - Stay in closer synch with RSP cache for prefix invalidations
 *
 * Revision 3.7  1996/01/13  02:51:45  rharagan
 * CSCdi46869:  Enable memory mapping for VIP
 * Change the default PCI access mode.
 *
 * Revision 3.6  1995/12/10  04:01:25  smackie
 * Add support for public particle pools. Make particle chunk growth less
 * aggressive for dynamic pools. (CSCdi45336)
 *
 * Revision 3.5  1995/12/01  02:00:09  mbeesley
 * CSCdi44698:  PCI subsystem needs improvement
 *
 * Revision 3.4  1995/11/30  03:28:03  mbeesley
 * CSCdi44684:  Rx particle setup incorrect on 1fe, 4e
 *         o Make rx packet start on an 8 byte boundary
 *         o Populate Rx ring from private pool, not global
 *         o Remove previously deleted CVS logs that crept back in
 *
 * Revision 3.3  1995/11/28  03:27:02  getchell
 * Increase ring sizes to avoid holdq enqueing
 * CSCdi43906:  performance drops by 1/2 when traffic passes zero drops
 *
 * Revision 3.2  1995/11/17  18:54:46  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:57:58  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#if !defined(AMDP2_STATIC_INLINES_ONLY)

/*
 * Include VIP header files
 */
#include "../src-vip/vip_memd.h"
#include "../src-vip/vip_af.h"

#if defined(SVIP)

/*
 * Define tx ring entries. Must be a power of 2!!
 */
#define AMDP2_TX_RING_ENTRIES		128
#define AMDP2_TX_RING_INDEX_MASK       	0x7f
#define AMDP2_TX_RING_ENTRIES_LOG2	7

/*
 * Define rx ring entries. Must be a power of 2!!
 */
#define AMDP2_RX_RING_ENTRIES		32
#define AMDP2_RX_RING_INDEX_MASK       	0x1f
#define AMDP2_RX_RING_ENTRIES_LOG2	5

/*
 * Define the particle size used by AMDP2
 */
#define AMDP2_PARTICLE_SIZE		VIP_PARTICLE_BYTES

/* 
 * Align the receive packet such that the packet can be sniffed and 
 * switched with one cache line read
 */
#define AMDP2_RX_ALIGN                    8

#else /* RVIP */

/*
 * Define tx ring entries. Must be a power of 2!!
 */
#define AMDP2_TX_RING_ENTRIES		128
#define AMDP2_TX_RING_INDEX_MASK	0x7f
#define AMDP2_TX_RING_ENTRIES_LOG2	7

/*
 * Define rx ring entries. Must be a power of 2!!
 */
#define AMDP2_RX_RING_ENTRIES		128
#define AMDP2_RX_RING_INDEX_MASK	0x7f
#define AMDP2_RX_RING_ENTRIES_LOG2	7

/*
 * Define the particle size used by AMDP2
 */
#define AMDP2_PARTICLE_SIZE		512

#define AMDP2_RX_ALIGN                    8

#endif /* RVIP */

/*
 * Define the high and low water marks for the QE polling.
 */
#define AMDP2_POLL_HIGH		(AMDP2_TX_RING_ENTRIES/2)
#define AMDP2_POLL_LOW		(AMDP2_TX_RING_ENTRIES/2-8)

/*
 * Define rx ring attribute
 */
#define AMDP2_RX_RING_ATTRIBUTE		TLB_UNCACHED

/*
 * Define buffer pool sizes
 */
#define AMDP2_FAST_PARTICLE_POOL	256	/* SRAM particles */
#define AMDP2_SLOW_PARTICLE_POOL	0	/* DRAM particles */

/*
 * Define device default settings
 */
#define AMDP2_DEFAULT_PROMISCUOUS_MODE	FALSE

/*
 * Define Tx/Rx interrupt loop limits
 */
#define AMDP2_RX_MAX_SPIN		AMDP2_RX_RING_ENTRIES
#define AMDP2_TX_MAX_SPIN		AMDP2_TX_RING_ENTRIES

/*
 * Define address conversion macros
 */
#define AMDP2_BUF_ADDR(x)		((ulong)(x))
#define AMDP2_CNTL_ADDR(x)		((ulong)(x))

/*
 * Setup AMDp2 Latency timer
 */
#define AMDP2_LAT_TIMER			AMDP2_LAT_TIMER_255

/*
 * Define platform specific address filter init macro.
 */
#define RESET_AF_AND_UPDATE_MAC_ADDRESSES

/*
 * Define constants to control endianess of shared data
 */
#define AMDP2_DMA_BIGENDIAN		TRUE

/*
 * Define to control generation of keepalives
 */
#define AMDP2_LOCAL_KEEPALIVE		FALSE

/*
 * Define latency timer
 */
#define AMDP2_PCI_LATENCY		0x0100

/*
 * Do not do any packet accounting on the VIP (host does it)
 */
#define AMDP2_PKT_STATS			FALSE

/*
 * The driver should not throttle when it runs out of resources
 */
#define AMDP2_THROTTLING	      	FALSE

/**********************************************************************
 * AMDP2 Static inlines
 **********************************************************************/

#else /* AMDP2_STATIC_INLINES_ONLY */

/*
 * amdp2_get_particle_inline:
 * Get a receive particle.
 */
static inline particletype *amdp2_get_particle_inline (hwidbtype *idb, 
						       boolean cache_ok)
{
    particletype *mp;

    /*
     * Try the cache
     */
    mp = pool_dequeue_cache(idb->pool);

    /*
     * If we did not get one from the cache, try a global pool
     */
    if (!mp) {
	mp = pool_getparticle(idb->pool->fallback);
    }

    /*
     * If we still do not have one, mark it as a nobuffer.
     */
    if (mp) {
	/*
	 * Initialize start. The VIP sw address filter requires
	 * the frame to start on an 8 byte boundary.
	 */
	mp->data_start = (uchar *)mp->data_block + PARTICLE_ENCAPBYTES - 
                                                          AMDP2_RX_ALIGN;
	return(mp);
    } else {
	idb->counters.output_nobuffers++;
	return(NULL);
    }
}

/*
 * amdp2_dfs_enabled:
 * Return TRUE if we are willing to DFS on this interface.
 */
static inline boolean amdp2_dfs_enabled (hwidbtype *idb)
{
    return(idb->ip_routecache & IP_DISTRIBUTED_ROUTECACHE);
}

/*
 * amdp2_ip_dfs:
 * Distributely fastswitch an IP datagram
 */
static inline boolean amdp2_ip_dfs (hwidbtype *idb, paktype *pak)
{
    pas_cbtype *pascb = INSTANCE(idb);

    return((*pascb->pak_dfs)(idb, pak));
}

/*
 * amdp2_process_receive_packet:
 * We have a good, completed Rx packet. Do something with it.
 */
static inline void amdp2_process_receive_packet (hwidbtype *idb,
						 amdp2_instance_t *ds,
						 paktype *rx_pak,
						 ushort rxd_status)
{
    CONVERT_PARTICLE_ADDR(rx_pak->datagramstart);

    /*
     * First thing to do is filter and classify
     */
    rx_pak->mci_status = vip_ether_filter_classify(rx_pak->datagramstart,
						   ds->pacb.vip_af,
						   ds->pacb.vip_af_entries,
						   rxd_status);
    if (ether_debug) {
	buginf("AMDP2: mci_status=%#x, rx_pak->datagramstart=%#x size=%#x\n", 
	       rx_pak->mci_status, rx_pak->datagramstart, rx_pak->datagramsize);
    }
    if (!rx_pak->mci_status) {
	/*
	 * Drop the frame
	 */
	ds->filtered_pak += 1;
	amdp2_cleanup_pak(rx_pak);
	return;
    }

    /*
     * Check if DFS support is enabled
     */
    if (amdp2_dfs_enabled(idb) &&
	((rx_pak->mci_status & AF_DODIP_STATION) == AF_DODIP_STATION)) {

	/*
	 * Perform IP fast switching on the packet. There is several ways
	 * this can be done -
	 *
	 * - reg_invoke_ether_fs(RXTYPE_DODIP, rx_pak)
	 * 	This is most elegant, but would probably require
	 *	a set_if_input_inline() on the rx_pak and other such stuff
	 * 	to allow the DFS routine to know the inbound interface
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
        if (!amdp2_ip_dfs(idb, rx_pak)) {
	    amdp2_cleanup_pak(rx_pak);
	}
	amdp2_flush_pak(rx_pak);
	return;
    }

    /*
     * Either DFS is turned off or the packet was not a supported
     * protocol. Just punt the frame to the host
     */
    if (!(*ds->pacb.pak_to_host)(idb, &ds->pacb, rx_pak)) {
	idb->counters.input_drops += 1;
	amdp2_cleanup_pak(rx_pak);
    }
    amdp2_flush_pak(rx_pak);
}

/*
 * amdp2_output_enqueue:
 * Platform specific routine to enqueue a packet onto
 * the output holdq. On checkers, if the packet is scattered we reparent
 * end enqueue it as particles. If it is not scattered, it is queued directly.
 */
static inline void amdp2_output_enqueue (paktype *pak, hwidbtype *idb)
{
    paktype *newpak;

    if (pak_has_particles(pak)) {
	/*
	 * Reparent onto a new paktype header as we can not hold onto
	 * the one passed into us.
	 */
	newpak = particle_reparent(pak, idb->headerpool);
	if (!newpak) {
	    amdp2_cleanup_pak(pak);
	    idb->counters.output_total_drops++;
	    return;
	}
    } else {
	newpak = pak;
    }
    newpak->if_output = idb->firstsw;
    /*
     * Invalidate the accounting protocol type to prevent
     * packet accounting in holdq_enqueue_fast if the accounting
     * flag is not set.
     */
    if (!AMDP2_PKT_STATS) 
        newpak->acct_proto = ACCT_PROTO_LAST;   
    holdq_enqueue_fast(idb, newpak);
}

#endif /* AMDP2_STATIC_INLINES_ONLY */

/* end of file */
