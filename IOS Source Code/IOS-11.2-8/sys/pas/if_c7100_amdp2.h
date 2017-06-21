/* $Id: if_c7100_amdp2.h,v 3.1.68.10 1996/09/13 20:18:30 mbeesley Exp $
 * $Source: /release/112/cvs/Xsys/pas/if_c7100_amdp2.h,v $
 *------------------------------------------------------------------
 * if_c7100_amdp2.h -- definitions for AMDP2 Ether PA (10mb)
 *		       (Predator dependent parts)
 *
 * November 1995, Dave Carroll
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: if_c7100_amdp2.h,v $
 * Revision 3.1.68.10  1996/09/13  20:18:30  mbeesley
 * CSCdi69010:  c7200 sometimes forwards an IP packet with a ttl of 1
 * Branch: California_branch
 *
 * Revision 3.1.68.9  1996/08/24  01:50:50  abaindur
 * CSCdi66979:  Traffic shaping broken on the c7200
 * Branch: California_branch
 *
 * Revision 3.1.68.8  1996/05/24  04:11:18  mbeesley
 * CSCdi58493:  Tweak PCI settings for new arbiters
 * Branch: California_branch
 *
 * Revision 3.1.68.7  1996/05/24  00:49:31  mbeesley
 * CSCdi58316:  Need to fully support CPQ on new interfaces
 * Branch: California_branch
 *
 * Revision 3.1.68.6  1996/05/07  01:22:29  mbeesley
 * CSCdi56710:  Correct AMDp2 fifo setting, update error messages
 * Branch: California_branch
 *
 * Revision 3.1.68.5  1996/05/07  00:23:51  mbeesley
 * CSCdi56157:  Enable IP Flow/Optimum on Ethernets
 * Branch: California_branch
 *
 * Revision 3.1.68.4  1996/04/23  17:13:36  gshen
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
 * Revision 3.1.68.3  1996/03/30  03:12:59  sjacobso
 * CSCdi52412:  4e/8e: Resolve code review comment
 * Branch: California_branch
 * Convert code in #if defined(PREDATOR) construct to
 * platform dependent macro.
 *
 * Revision 3.1.68.2  1996/03/29  04:05:02  sjacobso
 * CSCdi52266:  4E/8E receive promiscuously, hurting performance
 * Branch: California_branch
 * Change write_address_filter code to examine address filter
 * entries for important status, and to store that status in the
 * software address filter.  The correct common routines for
 * adding MAC addresses are called.  Also, fix Predator logical
 * address filter code to work correctly with HSRP addresses.
 *
 * Revision 3.1.68.1  1996/03/21  23:22:43  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.1.2.8  1996/02/19  23:23:52  mbeesley
 * CSCdi49335:  Predator needs support for DMA driven coalesce
 * Branch: ELC_branch
 *
 * Revision 3.1.2.7  1996/02/10  00:30:28  mbeesley
 * CSCdi48581:  Add VIP2 support to ELC_branch
 * Branch: ELC_branch
 *
 * Revision 3.1.2.6  1996/02/01  07:54:41  mbeesley
 * CSCdi47969:  Allow coalesced fastswitching on ethernet
 *         o Let 4e/1fe driver coalesce frames before fs
 *         o Speed up 1fe tx particle reclaimation
 *         o Remove min length check on 1fe (chip pads)
 *         o Put 1fe rx particles on a more PCI bus friendly alignment
 * Branch: ELC_branch
 *
 * Revision 3.1.2.5  1996/01/19  20:03:43  mbeesley
 * Branch: ELC_branch
 * Make "show bridge" more usefull. Remove unused static inline
 * for AMDP2 driver, fix process level packet accounting.
 *
 * Revision 3.1.2.4  1996/01/18  05:41:38  mbeesley
 * Branch: ELC_branch
 * Optimize 1fe and 4e driver. Fix 4e fastsend bug. Patch in Tx enable
 * fix for 1fe. Debug IP fastswitching.
 *
 * Revision 3.1.2.3  1996/01/16  01:42:15  dcarroll
 * Branch: ELC_branch
 * Sync ELC_branch between ELC_baseline_120995 and ELC_baseline_960110.
 *
 * Revision 3.1.2.2  1995/12/13  04:38:03  dcarroll
 * Branch: ELC_branch
 * Fix 4e fastsend accounting
 *
 * Revision 3.1.2.1  1995/11/22  21:41:34  mbeesley
 * Initial commit of c7100 support.
 * Branch: ELC_branch
 *
 * Revision 3.1  1995/11/20  22:07:19  mbeesley
 * Add placeholder files for future development.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#if !defined(AMDP2_STATIC_INLINES_ONLY)

/*
 * Include flow/optimum switching header files
 */
#include "../ipmulticast/igmp.h"
#include "../ip/ipfast_flow.h"

/*
 * Define tx ring entries. Must be a power of 2!!
 */
#define AMDP2_TX_RING_ENTRIES		128
#define AMDP2_TX_RING_INDEX_MASK       	0x7f
#define AMDP2_TX_RING_ENTRIES_LOG2	7

/*
 * Define rx ring entries. Must be a power of 2!!
 */
#define AMDP2_RX_RING_ENTRIES		64
#define AMDP2_RX_RING_INDEX_MASK	0x3f
#define AMDP2_RX_RING_ENTRIES_LOG2	6

/*
 * Define rx ring attribute
 */
#define AMDP2_RX_RING_ATTRIBUTE    	TLB_UNCACHED

/*
 * Define buffer pool sizes
 */
#define AMDP2_FAST_PARTICLE_POOL	128	/* SRAM particles */
#define AMDP2_SLOW_PARTICLE_POOL	0	/* DRAM particles */

/*
 * Define the minibuffer size used by AMDP2
 */
#define AMDP2_PARTICLE_SIZE		512

/*
 * Align Rx particles to 32 byte boundaries
 */
#define AMDP2_RX_ALIGN			4

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
#define RESET_AF_AND_UPDATE_MAC_ADDRESSES				\
{									\
    idbtype *swidb;							\
 									\
    amdp2_ladrf_clear(idb);						\
 									\
    /*									\
     * clear software address filter					\
     */									\
    amdp2_af_clear(idb);						\
    /*									\
     * Refresh any mac filters						\
     */									\
    FOR_ALL_SWIDBS_ON_HW(idb, swidb) {					\
        reg_invoke_tbridge_smf_update(idb->hardware,			\
                                      swidb,				\
                                      SMF_MACTYP_OWN_UNICAST |		\
                                      SMF_ACTION_RECEIVE);		\
        if (!(system_loading | bootstrap_enable)) {			\
            reg_invoke_media_interesting_addresses(swidb);		\
        }								\
    }									\
}

/*
 * Define constants to control endianess of shared data
 */
#define AMDP2_DMA_BIGENDIAN		TRUE

/*
 * Define to control generation of keepalives
 */
#define AMDP2_LOCAL_KEEPALIVE      	TRUE

/*
 * Define latency timer
 */
#define AMDP2_PCI_LATENCY		0x0100

/*
 * Have the driver do packet accounting
 */
#define AMDP2_PKT_STATS			TRUE

/*
 * The driver should throttle when it runs out of resources
 */
#define AMDP2_THROTTLING		TRUE

/*
 * Prototypes
 */
extern void	amdp2_post_coalesce_rx(hwidbtype *, paktype *, iqueue_t);
extern void	amdp2_post_coalesce_fs(hwidbtype *, paktype *, iqueue_t);

extern void	amdp2_af_clear(hwidbtype *);	

/**********************************************************************
 * AMDP2 Static inlines
 **********************************************************************/

#else /* AMDP2_STATIC_INLINES_ONLY */

/*
 * Define macro for lineproto change.  It is not used on Predator.
 */
#define amdp2_lineproto_change(a, b)

/*
 * af_locate_address_inline
 * see if there is a hash hit for this mac_addr in the filter
 */
static inline
af_entry_type *amdp2_af_locate_address_inline (uchar const *mac_addr,
					   af_entry_type **filter)

{
    af_entry_type *entry;
    uchar *addrp;

    for (entry = filter[AF_HASH(mac_addr)]; entry; entry = entry->next) {
	addrp = entry->mac_addr.byte;
	if ((GETLONG(&addrp[2]) == GETLONG(&mac_addr[2])) &&
	    (GETSHORT(&addrp[0]) == GETSHORT(&mac_addr[0])))
	    break;
    }

    return(entry);
}
/*
 * amdp2_af_match_inline:
 * return matching status if destination mac addr matched 
 * anything in addr filter.
 */
static inline boolean amdp2_af_match_inline (hwidbtype *idb, uchar *dest)
{
    amdp2_instance_t *ds = INSTANCE(idb);
    af_entry_type *entry, **filter;

    filter = ds->mac_filter;

    if (!filter)
        return(FALSE);

    entry = amdp2_af_locate_address_inline(dest, filter);

    if (!entry) {
        return(FALSE);
    }

    entry->hits++;

    return(TRUE);
}

/*
 * amdp2_mc_addr_check_inline:
 * check if this is a mc addr and fit into our mc addr range
 */
static inline boolean amdp2_mc_addr_check_inline (hwidbtype *idb, uchar *dest)
{
    amdp2_instance_t *ds = INSTANCE(idb);
    af_entry_type *entry, **filter;
	ulong matching_long;
	ushort matching_short;

    filter = ds->mc_mac_filter;

    if (!filter)
        return(FALSE);

	/*
	 * do nothing if it is not a multicast addr
	 */
	if ((dest[0] & 0x01) == 0)
		return(FALSE);

	/*
	 * now, go thru the table for checking if it matched
	 * the method is to or the dest addr with that of mask
	 * and compair with what stored in "mac_addr" which is
	 * pre-or'ed before it was stored there.
	 */	
	for (entry = filter[0]; entry; entry = entry->next) {
		uchar *addrp;

		addrp = entry->mask.byte;
		matching_long = GETLONG(&dest[2]) | GETLONG(&addrp[2]);
		matching_short = GETSHORT(&dest[0]) | GETSHORT(&addrp[0]);
		addrp = (uchar *)entry->mac_addr.byte;	
		if ((GETLONG(&addrp[2]) == matching_long) &&
	    	(GETSHORT(&addrp[0]) == matching_short)) {
			entry->hits++;
	    	return(TRUE);
		}
	}
	return(FALSE);
}

/*
 * amdp2_get_particle_inline:
 * Get an rx particle.
 */
static inline particletype *amdp2_get_particle_inline (hwidbtype *idb, 
						       boolean cache_ok)
{
    particletype *mp = NULL;

    /*
     * If cache_ok, try the buffer cache. This check will compile out.
     */
    if (cache_ok) {
	mp = pool_dequeue_cache(idb->pool);
    }

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
	 * Initialize start. Note that the PCI bus is much more efficient
	 * if things start on a 32 byte boundary, so set the start
	 * 28 bytes into the particle.
	 */
	mp->data_start = mp->data_block->data_area +
	    (PARTICLE_ENCAPBYTES - AMDP2_RX_ALIGN);
	return(mp);
    } else {
	idb->counters.output_nobuffers++;
	return(NULL);
    }
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
    ether_hdr *ether;
	ushort status;

	/*
	 * validate packet destination only if address filtering is needed
	 * (e.g., in promiscuous mode or multicast overflow mode)
	 */
	if (!idb->tbridge_on_hwidb && ds->need_af_check) {
        /*
         * Check if we should accept this packet
         */
        status = amdp2_af_match_inline(idb, rx_pak->datagramstart);

        if (status == 0) {
		/* 
		 * not in single addr table, how about addr range table
		 */
		status = amdp2_mc_addr_check_inline(idb, rx_pak->datagramstart);
		if (status == 0) {
            	/*
             	 * Drop the frame
             	 */
            	ds->filtered_pak += 1;
            	amdp2_cleanup_pak(rx_pak);
            	return;
		}
        }
	}

    /*
     * Make sure the pak flags field is clean
     */
    rx_pak->flags = 0;

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
		amdp2_flush_pak(rx_pak);
		return;
	    }
	    rx_pak->datagramstart = old_start;
	}
    }
    
    /*
     * Classify the packet
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
     * If this interface is listening promiscuously,
     * throw the packet at the software MAC filter in
     * the tbridge code where the bridge versus route
     * decision will be made.
     */
    if (idb->tbridge_on_hwidb && reg_invoke_tbridge_forward(rx_pak)) {
	amdp2_flush_pak(rx_pak);
	return;
    }

    /*
     * If the packet is for a protocol that is not presently fastswitchable
     * as a particled packet, coalesce it before doing further processing.
     * If it is a supported protocol, try and fastswitch it in particles.
     */
    if (c7100_scattered_fs_protocol(rx_pak->rxtype)) {
	if (reg_invoke_ether_fs(rx_pak->rxtype, rx_pak)) {
	    amdp2_flush_pak(rx_pak);
	    return;
	}
    } else {
	ether = (ether_hdr *)(rx_pak->datagramstart);
	c7100_dma_pak_coalesce(idb, rx_pak, amdp2_post_coalesce_fs,
			       (uchar *)(&ether->type_or_len));
	return;
    }

    /*
     * Coalesce the packet and send it on
     */
    ether = (ether_hdr *)(rx_pak->datagramstart);
    c7100_dma_pak_coalesce(idb, rx_pak, amdp2_post_coalesce_rx,
			   (uchar *)(&ether->type_or_len));
}

/*
 * amdp2_output_enqueue:
 * Platform specific routine to enqueue a packet onto
 * the output holdq. On Predator, if the packet is scattered we reparent
 * and enqueue it as particles. If it is not scattered, it is queued directly.
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
	if (idb->priority_list) {
	    pas_stuff_pak_for_holdq(newpak, pak);
	}
    } else {
	newpak = pak;
    }
    /*
     * Invalidate the accounting protocol type to prevent
     * packet accounting in holdq_enqueue_fast() if accounting
     * flag is not set
     */
    if (!AMDP2_PKT_STATS)
	 newpak->acct_proto = ACCT_PROTO_LAST;
    holdq_enqueue_fast(idb, newpak);
}

#endif /* AMDP2_STATIC_INLINES_ONLY */

/* end of file */
