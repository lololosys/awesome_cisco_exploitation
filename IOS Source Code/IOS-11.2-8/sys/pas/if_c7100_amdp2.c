/* $Id: if_c7100_amdp2.c,v 3.1.68.10 1996/08/28 13:05:39 thille Exp $
 * $Source: /release/112/cvs/Xsys/pas/if_c7100_amdp2.c,v $
 *------------------------------------------------------------------
 * if_c7100_amdp2.c - AMDP2 10 Mbit Ethernet PCI PA
 *		      (Predator dependent parts)
 *
 * November 1995, Dave Carroll
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: if_c7100_amdp2.c,v $
 * Revision 3.1.68.10  1996/08/28  13:05:39  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.1.68.9  1996/08/07  23:35:20  sthormod
 * CSCdi63522:  C7200: RSRB gets process switched on ether, serial, fddi
 * Branch: California_branch
 *
 * Revision 3.1.68.8  1996/07/11  14:20:52  fbordona
 * CSCdi62521:  DLSw circuit does not connect using FST over ATM peer
 *              Remove DLSw FST hooks from platform/media specific drivers.
 * Branch: California_branch
 *
 * Revision 3.1.68.7  1996/05/21  09:59:56  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.1.68.6  1996/05/14  02:38:45  dcarroll
 * CSCdi57529:  Decrement interface counters on OIR removal
 * Branch: California_branch
 * Decrement interface counters after OIR.
 *
 * Revision 3.1.68.5  1996/05/07  01:22:27  mbeesley
 * CSCdi56710:  Correct AMDp2 fifo setting, update error messages
 * Branch: California_branch
 *
 * Revision 3.1.68.4  1996/05/01  13:28:02  dcarroll
 * CSCdi54895:  oir fix for ethernet port adapter
 * Branch: California_branch
 * OIR support for pas ethernets.
 *
 * Revision 3.1.68.3  1996/04/23  16:28:49  feisal
 * CSCdi52937:  loss of carrier detected on 5EF/VIP2
 * Branch: California_branch
 *
 * Revision 3.1.68.2  1996/03/29  04:04:52  sjacobso
 * CSCdi52266:  4E/8E receive promiscuously, hurting performance
 * Branch: California_branch
 * Change write_address_filter code to examine address filter
 * entries for important status, and to store that status in the
 * software address filter.  The correct common routines for
 * adding MAC addresses are called.  Also, fix Predator logical
 * address filter code to work correctly with HSRP addresses.
 *
 * Revision 3.1.68.1  1996/03/21  23:22:41  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.1.2.12  1996/02/19  23:23:50  mbeesley
 * CSCdi49335:  Predator needs support for DMA driven coalesce
 * Branch: ELC_branch
 *
 * Revision 3.1.2.11  1996/02/19  05:25:28  dcarroll
 * CSCdi49299:  Fix 4e frame miss count handling
 * Branch: ELC_branch
 * Disable miss frame counter overflow interrupt.
 *
 * Revision 3.1.2.10  1996/02/13  01:43:02  mbeesley
 * CSCdi48778:  IO DRAM gets exhausted
 *         o Reduce AMDp2 and FDDI rx particle pool
 *         o Allocate more DRAM for IO on small images
 *         o Add cached view of packet DRAM
 *         o Use packet sram as a fast heap on rev 1 boxes
 * Branch: ELC_branch
 *
 * Revision 3.1.2.9  1996/02/10  00:30:26  mbeesley
 * CSCdi48581:  Add VIP2 support to ELC_branch
 * Branch: ELC_branch
 *
 * Revision 3.1.2.8  1996/02/08  08:47:39  gstovall
 * Branch: ELC_branch
 * Sync to ELC_baseline_960205.
 *
 * Revision 3.1.2.7  1996/02/01  07:54:40  mbeesley
 * CSCdi47969:  Allow coalesced fastswitching on ethernet
 *         o Let 4e/1fe driver coalesce frames before fs
 *         o Speed up 1fe tx particle reclaimation
 *         o Remove min length check on 1fe (chip pads)
 *         o Put 1fe rx particles on a more PCI bus friendly alignment
 * Branch: ELC_branch
 *
 * Revision 3.1.2.6  1996/01/16  01:42:13  dcarroll
 * Branch: ELC_branch
 * Sync ELC_branch between ELC_baseline_120995 and ELC_baseline_960110.
 *
 * Revision 3.1.2.5  1995/12/22  05:08:57  wfried
 * Branch: ELC_branch
 * Fix unaligned accesses that can happen when accessing ethernet header
 * after packet is coalesced/copied.
 *
 * Revision 3.1.2.4  1995/12/11  19:13:37  mbeesley
 * Branch: ELC_branch
 * Sync ELC_branch to ELC_baseline_120995.
 *
 * Revision 3.1.2.3  1995/12/05  06:35:35  mbeesley
 * Branch: ELC_branch
 * Safeguard static paktype headers from being freed.
 *
 * Revision 3.1.2.2  1995/11/30  02:04:33  dcarroll
 * Branch: ELC_branch
 *
 * Revision 3.1.2.1  1995/11/22  21:41:32  mbeesley
 * Initial commit of c7100 support.
 * Branch: ELC_branch
 *
 * Revision 3.1  1995/11/20  22:07:17  mbeesley
 * Add placeholder files for future development.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Include files
 */
#include "master.h"
#include "../ui/common_strings.h"
#include "sys_registry.h"
#include "fastswitch_registry.h"
#include "media_registry.h"
#include "interface_private.h"
#include "packet.h"
#include "config.h"
#include "ttysrv.h"
#include "logger.h"
#include "subsys.h"
#include "sched.h"
#include "parser.h"

#include "../os/pool.h"
#include "../os/particle.h"
#include "../os/buffers.h"
#include "../os/buffers_inline.h"

#include "../if/network.h"
#include "../if/ether.h"
#include "../if/ether_inline.h"
#include "../if/pci.h"
#include "../ip/ip.h"

#include "../snmp/snmp_api.h"
#include "../snmp/ifmib_registry.h"

#include "../parser/parser_actions.h"
#include "../parser/macros.h"

#include "../hes/if_fci.h"

#include "if_pas.h"
#include "if_pas_amdp2.h"
#include "msg_amdp2.c"			/* Not a typo, see logger.h */
#include "msg_pas.c"			/* Not a typo, see logger.h */
#include "pas_id.h"

#include "../hes/parser_defs_ucode.h"
#include "../dev/flash_dvr_spec.h"
#include "../hes/ucode.h"
#include "../hes/slot.h"

#include "../src-4k-c7100/c7100_macaddr.h"
#include "ieee.h"

/*
 * amdp2_get_particle:
 * Get an rx particle for the device
 */
particletype *amdp2_get_particle (hwidbtype *idb)
{
    return(amdp2_get_particle_inline(idb, FALSE));
}

/*
 * amdp2_idb_create:
 * Create an idb for an AMDP2 ethernet interface. Try to get
 * it from the list of previously removed interfaces first.
 */
hwidbtype *amdp2_idb_create (uint slot, uint port)
{
    hwidbtype *idb;

    idb = c7100_get_matching_idb(slot, port, slots[slot].pa_type, IDBTYPE_AMDP2);
    if (idb) {
        slots[slot].interface_list[port] = idb;
        slots[slot].interface_count++;
    }
    return(idb);
}

/*
 * amdp2_deallocate:
 * Clean up all resourcse for this interface when the Port
 * adapter is removed.
 */
void amdp2_deallocate (hwidbtype *hwidb)
{
    int ix;
    amdp2_instance_t *ds;
    pooltype *pool;

    ds = INSTANCE(hwidb);
    pool = hwidb->pool;

    /*
     * Determine if an instance exists. If so, go through
     * and clean up.
     */
    if (ds) {
	/*
	 * Free rx particles.
	 */
	if (ds->rxr_shadow) {
	    for (ix = 0; ix < AMDP2_RX_RING_ENTRIES; ix++) {
		particletype *mp;
		mp = ds->rxr_shadow[ix];
		if (mp) {
		    retparticle(mp);
		}
	    }
	}
	
	/*
	 *  Free tx particles or paks.
	 */
	if (ds->txr_shadow) {
	    for (ix = 0; ix < AMDP2_TX_RING_ENTRIES; ix++) {
		particletype *mp;
		paktype *pak;

		mp = ds->txr_shadow[ix].mp;
		pak = ds->txr_shadow[ix].pak;
		if (mp) {
		    retparticle(mp);
		}
		if (pak && (pak != AMDP2_MAGIC_PAK_PTR)) {
		    datagram_done(pak);
		}
	    }
	}
	
	/*
	 * Free the blocks hanging off the instance.
	 */
	if (ds->rxr_malloc)
	    free(ds->rxr_malloc);
	if (ds->txr_malloc)
	    free(ds->txr_malloc);
	if (ds->rxr_shadow)
	    free(ds->rxr_shadow);
	if (ds->txr_shadow)
	    free(ds->txr_shadow);
	if (ds->ib_malloc)
	    free(ds->ib_malloc);
	if (ds->rx_pak)
	    free(ds->rx_pak);

	/*
	 * Now free the instance.
	 */
	free(ds);
	INSTANCE(hwidb) = NULL;
    }

    /*
     * Now destroy the pool.
     */
    if (pool) {
	pool_destroy(pool);
    }
    hwidb->pool = NULL;
    hwidb->pool_group = POOL_GROUP_INVALID;

    /*
     * Decrement the interface counters (keeps show hard correct)
     */
    nethernets--;
}

/*
 * af_create_filter
 * allocate a af_entry_type record and initialize the filter table ptr
 */
static boolean
amdp2_af_create_filter (af_entry_type ***filter)

{
    if (*filter)
		return(TRUE);

    *filter = malloc_named((sizeof(af_entry_type *)) * NETHASHLEN,
			  "AF filter");

    if (!*filter) {
		return(FALSE);
    }
    return(TRUE);
}

/*
 * amdp2_af_locate_entry_inline
 *
 * Locate an AF entry by MAC address and type
 */
 
static inline
af_entry_type *amdp2_af_locate_entry_inline (uchar const *mac_addr,
                     af_entry_type **filter,
                     ulong const type, uchar const *mask)
 
{
    af_entry_type *entry;
    uint hash_entry;
 
    if (mask) {
	hash_entry = 0;
    } else {
	hash_entry = AF_HASH(mac_addr);
    }
    for (entry = filter[hash_entry]; entry; entry = entry->next)
        if ((ieee_equal((uchar *)entry->mac_addr.sword, mac_addr)) &&
            (mask?(ieee_equal((uchar *)entry->mask.sword, mask)):1) &&
            (entry->type == type))
        break;
 
    return(entry);
}
 
/*
 * amdp2_add_af_entry
 * add a mac addr entry into address filters based on mask
 */
void amdp2_add_af_entry (hwidbtype *idb, uchar *mac, uint type, uchar *mask)
{
    amdp2_instance_t *ds = INSTANCE(idb);
    af_entry_type **filter, *entry, *last;
    uint hash_entry;

    if (onintstack())
	return;

	/*
	 * decide which table for storing the address entry
	 */
	if (mask == NULL) {
    	filter = ds->mac_filter;
    	if (!filter) {
			if (!amdp2_af_create_filter(&ds->mac_filter)) {
	    		return;
			}
    		filter = ds->mac_filter;
    	}
	} else {
		filter = ds->mc_mac_filter;
    	if (!filter) {
			if (!amdp2_af_create_filter(&ds->mc_mac_filter)) {
	    		return;
			}
			filter = ds->mc_mac_filter;
    	}
	}

    /* Check for an exact duplicate */
    entry = amdp2_af_locate_entry_inline(mac, filter, type, mask);

    if (entry)
	return;

    entry = malloc_named(sizeof(af_entry_type), "AF entry");

    if (!entry) {
	return;
    }

    entry->next = NULL;
    entry->type = type;
    entry->hits = 0;

    ieee_copy(mac, (uchar *)entry->mac_addr.sword);
    if (mask) {
	ieee_copy(mask, (uchar *)entry->mask.sword);
	/*
	 * for address range put their entry into a straight list 
	 * without hash
	 */
	hash_entry = 0;
    } else {
	hash_entry = AF_HASH(mac);
    }

    /* Locate the end of the hash chain */
    for (last = filter[hash_entry]; last && last->next; last = last->next);

    if (last)
	last->next = entry;
    else
	filter[hash_entry] = entry;

    return;
}

/*
 * amdp2_rm_filter
 * delete and free all AF entry per filter
 */
void amdp2_rm_filter (af_entry_type **filter)
{
    af_entry_type *prev_entry, *entry, *next;
    int hash;
    leveltype level;

    if (!filter)
	return;

    for (hash = 0; hash < NETHASHLEN; hash++) {

	level = raise_interrupt_level(ALL_DISABLE);
	
	prev_entry = NULL;
	for (entry = filter[hash]; entry; ) {
	    next = entry->next;

		if (prev_entry) {
		    prev_entry->next = entry->next;
		} else {
		    filter[hash] = entry->next;
		}
		free((void *)entry);
	    entry = next;
	}

	reset_interrupt_level(level);
    }

    return;
}

/*
 * amdp2_af_clear
 * delete all AF entry we put in
 */
void amdp2_af_clear (hwidbtype *idb)
{
    amdp2_instance_t *ds = INSTANCE(idb);

    if (onintstack())
	return;

    amdp2_rm_filter(ds->mac_filter);
    amdp2_rm_filter(ds->mc_mac_filter);

	ds->need_af_check = FALSE;	
}

/*
 * amdp2_address_filter_write
 *
 * This is the 7200 version of "listen". In addition to put in
 * the multicast addr, it also put the mac addr into soft addr filter
 */
static void amdp2_address_filter_write (idbtype *swidb, uchar *mac,
                     		uchar fsm)
{
	hwidbtype *idb = (hwidbtype *)swidb->hwptr;

	/*
	 * do the user request first
	 */
	amdp2_add_multicast_address(swidb, mac, fsm);

	if (fsm == FCI_HSRP_STATION_ADDR) {
		/*
		 * add our own mac address and broadcast address
	 	 */	
		amdp2_add_af_entry(idb, idb->hardware, FCI_STATION_ADDRESS, NULL);
		amdp2_add_af_entry(idb, (uchar *)baddr, FCI_MULTICAST_ADDRESS, NULL);
	}
	amdp2_add_af_entry(swidb->hwptr, mac, fsm, NULL);

}

/*
 * amdp2_show_filter
 * print out address filter tables
 */
void
amdp2_show_filter (af_entry_type **filter)
{
    int hash, length;
    af_entry_type *entry;

    if (!filter)
	return;

    for (hash = 0; hash < NETHASHLEN; hash++) {
	for (entry = filter[hash], length = 0; entry;
	    entry = entry->next, length++) {
 	    printf("  0x%02x:%-3d  %e  %e %-9d\n", hash, length,
		   entry->mac_addr.byte, entry->mask.byte, entry->hits);
	}
    }
}

/*
 * amdp2_af_show_filter
 * display the filter content
 */
void
amdp2_af_show_filter (amdp2_instance_t *ds)
{

	printf(" Software MAC address filter(hash:length/addr/mask/hits):\n");
	amdp2_show_filter(ds->mac_filter);
	amdp2_show_filter(ds->mc_mac_filter);
}

/*
 * amdp2_address_filter_write_mc
 * add range of multicast addr into mc_addr_table
 */
void amdp2_address_filter_write_mc (idbtype *swidb, uchar *address, uchar *mask)
{
	hwidbtype *idb = swidb->hwptr;
	amdp2_instance_t *ds = INSTANCE(idb);
	uchar addrp[IEEEBYTES];

	amdp2_add_multicast_range(swidb, address, mask);

	/* 
	 * add mc addr range to mc filter table, mask it first before put it
	 */
	ieee_copy(address, addrp);
	ieee_mask(addrp, mask);	
	amdp2_add_af_entry(idb, addrp, FCI_MULTICAST_ADDRESS, mask);

	if (ds->need_af_check == FALSE) {
		/*
		 * add our own mac address and broadcast address
	 	 */	
		amdp2_add_af_entry(idb, idb->hardware, FCI_STATION_ADDRESS, NULL);
		amdp2_add_af_entry(idb, (uchar *)baddr, FCI_MULTICAST_ADDRESS, NULL);
		ds->need_af_check = TRUE;
	}
}

/*
 * amdp2_platform_init:
 * Platform dependant initialization for Presidio2 port adaptor interface.
 */
boolean amdp2_platform_init (hwidbtype *idb)
{
    amdp2_instance_t *ds = INSTANCE(idb);
    amdp2_regs_t *reg;

	/*
	 * override the default "listen" and "listen_range" server for 7200
	 */
	idb->listen = amdp2_address_filter_write;
	idb->listen_range = amdp2_address_filter_write_mc;

    /*
     * Configure BCRs. All these settings are one shots, and do
     * not get effected by resets.
     */
    ds = INSTANCE(idb);
    reg = ds->amdp2_regaddr;

    reg->rap = AMDP2_CSR0;
    reg->rdp = AMDP2_CSR0_STOP;
    DELAY(5);
    
    reg->rap = AMDP2_BCR18;
    reg->bdp = AMDP2_BCR18_BREADE | AMDP2_BCR18_BWRITE;
    reg->rap = AMDP2_BCR20;
    reg->bdp = AMDP2_BCR20_SSIZE32 | AMDP2_BCR20_SW_STYLE3;
    
    /* 
     * Link Status Enable & Pulse Stretcher on by default
     */
    reg->rap = AMDP2_BCR4;
    reg->bdp = AMDP2_BCR4_LNKSTE | AMDP2_BCR4_PSE;   

    /*
     * Disable pin outputs for LED1, LED2, LED3.
     * Add specific pattern to low-order nibble to avoid logic error
     * in silicon of rev B0 chip.  Otherwise, the error can cause
     * the chip to enter STOP state or fail to init.
     */
    reg->rap = AMDP2_BCR5;
    reg->bdp = AMDP2_BCR5_LEDDIS | 0x7;
    reg->rap = AMDP2_BCR6;
    if (get_pa_type(ds->pacb.pa_bay) == PA_HARDWARE_5E)
        reg->bdp = AMDP2_BCR6_LEDPOL | AMDP2_BCR6_FDLSE;
    else
        reg->bdp = AMDP2_BCR6_LEDDIS;
    reg->rap = AMDP2_BCR7;
    reg->bdp = AMDP2_BCR7_LEDDIS;

    /*
     * Install this interface into the pa_jumptable
     */
    pas_install_port_instance(idb, TRUE);
    return(TRUE);
}

/*
 * amdp2_show_instance:
 * Platform specific printout of the instance structure
 */
void amdp2_show_instance (amdp2_instance_t *ds)
{
    amdp2_af_show_filter(ds);
    /*
     * Display C7100 specific stuff
     */
    printf(" spurious_idon=%d, filtered_pak=%d, throttled=%d, enabled=%d, "
	   "disabled=%d\n",
	   ds->spurious_idon, ds->filtered_pak, ds->throttled,
	   ds->enabled, ds->disabled);
    printf(" rx_framing_err=%d, rx_overflow_err=%d, rx_buffer_err=%d, "
	   "rx_bpe_err=%d\n",
	   ds->rx_framing_err, ds->rx_overflow_err, ds->rx_buffer_err,
	   ds->rx_bpe_err);
    printf(" rx_soft_overflow_err=%d, rx_no_enp=%d, rx_discard=%d, "
	   "rx_miss_count=%d\n",
	   ds->rx_soft_overflow_err, ds->rx_no_enp, ds->rx_discard,
	   ds->rx_miss_count);
    printf(" tx_one_col_err=%d, tx_more_col_err=%d, tx_no_enp=%d, "
	   "tx_deferred_err=%d\n",
	   ds->tx_one_col_err, ds->tx_more_col_err, ds->tx_no_enp,
	   ds->tx_deferred_err);
    printf(" tx_underrun_err=%d, tx_late_collision_err=%d, "
	   "tx_loss_carrier_err=%d\n",
	   ds->tx_underrun_err, ds->tx_late_collision_err, 
	   ds->tx_loss_carrier_err);
    printf(" tx_exc_collision_err=%d, tx_buff_err=%d, fatal_tx_err=%d\n",
	   ds->tx_exc_collision_err, ds->tx_buff_err, ds->fatal_tx_err);
    printf(" hsrp_conf=%d, need_af_check=%d\n", 
	   ds->hsrp_configured, ds->need_af_check);
}

/*
 * amdp2_snmp_link_trap:
 * SNMP trap event support.
 */
void amdp2_snmp_link_trap (int trap, hwidbtype *idb)
{
    if (trap == AMDP2_LINK_UP) {
	reg_invoke_ifmib_link_trap(LINK_UP_TRAP, idb);
    } else {
	reg_invoke_ifmib_link_trap(LINK_DOWN_TRAP, idb);
    }
}

/*
 * amdp2_get_default_hw_address:
 * Get a default mac address for this interface
 */
void amdp2_get_default_hw_address (hwidbtype *idb, uchar addr[IEEEBYTES])
{
    c7100_assign_mac_addr(idb, addr);
}

/*
 * amdp2_init_buffer_pools:
 * Initialize buffer pools for this interface
 */
boolean amdp2_init_buffer_pools (hwidbtype *idb)
{
    amdp2_instance_t *ds;
    pooltype *pool;
    int particles_in_pool;
    mempool *buffer_mempool;

    /*
     * Init the particle pool
     */
    if (!idb->pool) {
	/*
	 * See which memory pool this interfaces buffers should use
	 * and create the buffer pool.
	 */
	buffer_mempool = pas_buffer_mempool(idb);

	if (idb->pool_group == POOL_GROUP_INVALID)
            idb->pool_group = pool_create_group();

	pool = particle_pool_create(idb->hw_namestring,
				    idb->pool_group,
				    AMDP2_PARTICLE_SIZE, 
				    POOL_SANITY,
				    32,
				    buffer_mempool);
	if (!pool) {
	    return(FALSE);
	}
	
	/*
	 * Add a fallback pool
	 */
	pool->fallback = pas_interface_fallback_pool(idb);
	
	/*
	 * Populate the pool and add a cache
	 */
	particles_in_pool = AMDP2_RX_RING_ENTRIES * 2;
	pool_adjust(pool, 0, particles_in_pool, particles_in_pool, TRUE);
	particle_pool_create_cache(pool, particles_in_pool);
	pool_adjust_cache(pool, particles_in_pool);
	idb->pool = pool;
    }

    /*
     * Init the header pool
     */
    if (!idb->headerpool) {
	idb->headerpool = pas_interface_header_pool(idb);
    }

    /*
     * Init the static rx_pak
     */
    ds = INSTANCE(idb);
    if (!ds->rx_pak) {
	ds->rx_pak = pas_allocate_fspak();
	if (!ds->rx_pak) {
	    return(FALSE);
	}
	amdp2_cleanup_pak(ds->rx_pak);
    }
    return(TRUE);
}

/*
 * amdp2_update_vendor_states:
 * Do OEM specific stuff (not applicable to C7100)
 */
void amdp2_update_vendor_states (hwidbtype *idb)
{
}

/*
 * amdp2_post_coalesce_rx:
 * Perform post-coalescing processing on a recieved frame
 */
void amdp2_post_coalesce_rx (hwidbtype *idb, paktype *rx_pak, iqueue_t iq)
{
    ether_hdr *ether;
    ushort rxtype = rx_pak->rxtype;

    /*
     * Setup encap pointers
     */
    ether = (ether_hdr *)rx_pak->datagramstart;
    
    /*
     * check for RSRB or DLSw Direct encaps packets
     */
    if (rxtype == RXTYPE_SRB) {
	if (reg_invoke_fs_rsrb_direct_input(idb, rx_pak)) {
	    return;
	}
    }

    /*
     * If it is IP, check for tunneled stuff
     */
    if (ROUTING_RXTYPE(idb->firstsw, rxtype) && 
	(rxtype == RXTYPE_DODIP || rxtype == RXTYPE_SNAP_DODIP)) {
	iphdrtype *ip;
	
	ip = (iphdrtype *)ipheadstart(rx_pak); 
	switch (ip->prot) {
	case GRE_PROT:
	case IPINIP_PROT:
	    if (reg_invoke_tunnel_ip_les_fs(rx_pak)) {
		return;
	    }
	    break;
	case FST_RSRB_PROT:
	    if (reg_invoke_fs_rsrb_fst_input(idb, rx_pak)) {
		return;
	    }
	    break;
	}
    }
    
    /*
     * Count input broadcasts:
     */
    if (ether->daddr[0] & 1) {
	idb->counters.input_broad++;
    }

    /*
     * Since we received a packet, the interface is up:
     */
    if (!idb->ethernet_lineup) {
	idb->ethernet_lineup = TRUE;
	idb->reason = "Keepalive OK";
	amdp2_snmp_link_trap(AMDP2_LINK_UP, idb);
    }
    
    /*
     * We did not fast switch the datagram.
     * Pass it up to process level
     */
    idb->counters.rx_cumbytes += rx_pak->datagramsize;
    idb->counters.inputs++;
    if (rx_pak->enctype == ET_LLC2) {
	llc_input(rx_pak);
    } else {
	(*(idb->iqueue)) (idb, rx_pak);
    }
}

/*
 * amdp2_post_coalesce_fs:
 * Perform post-coalesce processing on a recieved frame which we have
 * not even tried to fastswitch yet.
 */
void amdp2_post_coalesce_fs (hwidbtype *idb, paktype *rx_pak, iqueue_t iq)
{
    /*
     * Attempt to fast switch this contigous packet.
     */
    if (reg_invoke_ether_fs(rx_pak->rxtype, rx_pak)) {
	return;
    }

    /*
     * Do the remainder of the interrupt level processing
     */
    amdp2_post_coalesce_rx(idb, rx_pak, iq);
}

/* end of file */
      
