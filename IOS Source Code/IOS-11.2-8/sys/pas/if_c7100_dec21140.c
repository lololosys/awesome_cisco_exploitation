/* $Id: if_c7100_dec21140.c,v 3.1.66.12 1996/08/28 13:05:41 thille Exp $
 * $Source: /release/112/cvs/Xsys/pas/if_c7100_dec21140.c,v $
 *------------------------------------------------------------------
 * if_c7100_dec21140.c - DEC21140 100 Mbit Ethernet PCI PA
 *		         (Predator dependent parts)
 *
 * November 1995, Walter Friedrich
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: if_c7100_dec21140.c,v $
 * Revision 3.1.66.12  1996/08/28  13:05:41  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.1.66.11  1996/08/07  23:35:22  sthormod
 * CSCdi63522:  C7200: RSRB gets process switched on ether, serial, fddi
 * Branch: California_branch
 *
 * Revision 3.1.66.10  1996/07/11  14:20:54  fbordona
 * CSCdi62521:  DLSw circuit does not connect using FST over ATM peer
 *              Remove DLSw FST hooks from platform/media specific drivers.
 * Branch: California_branch
 *
 * Revision 3.1.66.9  1996/06/21  21:24:39  mbeesley
 * CSCdi59509:  FastEther init code can loop forever
 * Branch: California_branch
 *
 * Revision 3.1.66.8  1996/05/24  20:32:46  ssangiah
 * CSCdi58375:  Need to add ISL support for new platform.
 * Add ISL support for the new c7200 platform.
 * Branch: California_branch
 *
 * Revision 3.1.66.7  1996/05/21  09:59:58  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.1.66.6  1996/05/13  20:26:12  dcarroll
 * CSCdi56363:  OIR-insertion of 4E/8E/4T PAs should  bring-up the ports
 * in shut
 * Branch: California_branch
 * OIR'd adapters must stay shutdown through initializatio
 *
 * Revision 3.1.66.5  1996/05/07  23:48:07  mbeesley
 * CSCdi56954:  DEC21140 show controller not correct for IO card interface
 * Branch: California_branch
 *
 * Revision 3.1.66.4  1996/04/29  23:44:01  mbeesley
 * CSCdi55449:  1fe PA driver needs to clean-up after OIR
 * Branch: California_branch
 *
 * Revision 3.1.66.3  1996/04/05  05:32:04  dcarroll
 * CSCdi53595:  allow for multiple passes through pas idb init code
 * Branch: California_branch
 * Break up the init idb routines and keep track of idb init state.
 *
 * Revision 3.1.66.2  1996/03/29  04:05:05  sjacobso
 * CSCdi52266:  4E/8E receive promiscuously, hurting performance
 * Branch: California_branch
 * Change write_address_filter code to examine address filter
 * entries for important status, and to store that status in the
 * software address filter.  The correct common routines for
 * adding MAC addresses are called.  Also, fix Predator logical
 * address filter code to work correctly with HSRP addresses.
 *
 * Revision 3.1.66.1  1996/03/21  23:22:44  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.1.2.12  1996/03/07  00:19:09  mbeesley
 * CSCdi50649:  DEC21140 in Store-and-Forward mode
 *         o Put device in tx cut-thru, 512 byte tx fifo threshold
 *         o Increase rx pool, eliminate use of fallback pool
 *         o Handle rx fifo overrun and error interrupt correctly
 * Branch: ELC_branch
 *
 * Revision 3.1.2.11  1996/03/03  08:40:13  wfried
 * CSCdi50555:  Fast Ether multicast filtering broken and unthrottling not
 * working
 * Branch: ELC_branch
 * Avoid multiple setting of single multicast address in hw addr filter,
 * register pool cache throttling callback function.
 *
 * Revision 3.1.2.10  1996/02/19  23:23:53  mbeesley
 * CSCdi49335:  Predator needs support for DMA driven coalesce
 * Branch: ELC_branch
 *
 * Revision 3.1.2.9  1996/02/10  00:30:29  mbeesley
 * CSCdi48581:  Add VIP2 support to ELC_branch
 * Branch: ELC_branch
 *
 * Revision 3.1.2.8  1996/02/01  07:54:42  mbeesley
 * CSCdi47969:  Allow coalesced fastswitching on ethernet
 *         o Let 4e/1fe driver coalesce frames before fs
 *         o Speed up 1fe tx particle reclaimation
 *         o Remove min length check on 1fe (chip pads)
 *         o Put 1fe rx particles on a more PCI bus friendly alignment
 * Branch: ELC_branch
 *
 * Revision 3.1.2.7  1996/01/29  19:04:21  wfried
 * Branch: ELC_branch
 * Use HW address filtering, disable rx side for better input throttling,
 * add loopback code, correct ignored frames counting, allow only MII media
 * on the Predator I/O card FE interface.
 *
 * Revision 3.1.2.6  1996/01/16  01:42:17  dcarroll
 * Branch: ELC_branch
 * Sync ELC_branch between ELC_baseline_120995 and ELC_baseline_960110.
 *
 * Revision 3.1.2.5  1995/12/22  05:08:58  wfried
 * Branch: ELC_branch
 * Fix unaligned accesses that can happen when accessing ethernet header
 * after packet is coalesced/copied.
 *
 * Revision 3.1.2.4  1995/12/12  04:53:54  mbeesley
 * Branch: ELC_branch
 * Allow "show pci bridge" to display all bridges. Change the 1fe GPC
 * register settings for Predator IO card 1fe. Fix some amdp2 error
 * messages. And turn power off on unrecognized port adaptors.
 *
 * Revision 3.1.2.3  1995/12/11  19:13:38  mbeesley
 * Branch: ELC_branch
 * Sync ELC_branch to ELC_baseline_120995.
 *
 * Revision 3.1.2.2  1995/12/05  06:35:36  mbeesley
 * Branch: ELC_branch
 * Safeguard static paktype headers from being freed.
 *
 * Revision 3.1.2.1  1995/11/22  21:41:35  mbeesley
 * Initial commit of c7100 support.
 * Branch: ELC_branch
 *
 * Revision 3.1  1995/11/20  22:07:20  mbeesley
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
#include "if_pas_dec21140.h"
#include "msg_dec21140.c"		/* Not a typo, see logger.h */
#include "msg_pas.c"			/* Not a typo, see logger.h */

#include "../src-4k-c7100/c7100_macaddr.h"

#include "pas_id.h"
#include "../hes/parser_defs_ucode.h"
#include "../dev/flash_dvr_spec.h"
#include "../hes/ucode.h"
#include "../hes/slot.h"

/*
 * dec21140_get_particle:
 * Get an rx particle for the device
 */
particletype *dec21140_get_particle (hwidbtype *idb)
{
    particletype *mp = NULL;

    if (idb->pool) {
	mp = dec21140_get_particle_inline(idb, FALSE);
    }
    if (!mp) {
	/*
	 * Try harder to get a particle. The init code needs
	 * a couple of particles for address filter setup.
	 */
	mp = pool_getparticle(pas_interface_fallback_pool(idb));
    }
    return(mp);
}

/*
 * dec21140_idb_create:
 * Create an idb for an DEC21140 ethernet interface. We should look for
 * a corresponding idb that may be arround due to hot-swap
 */
hwidbtype *dec21140_idb_create (uint slot, uint port)
{
    hwidbtype *idb;

    idb = c7100_get_matching_idb(slot, port, slots[slot].pa_type, IDBTYPE_FEIP);
    if (idb) {
        slots[slot].interface_list[port] = idb;
        slots[slot].interface_count++;
    }
    return(idb);
}

/*
 * dec21140_deallocate:
 * Clean up all resourcse for this interface. FIXME.
 */
void dec21140_deallocate (hwidbtype *hwidb)
{
    dec21140_instance_t *ds;
    particletype *mp;
    pooltype *pool;
    paktype *pak;
    int i;

    ds = INSTANCE(hwidb);
    pool = hwidb->pool;

    /*
     * Determine if an instance exists. If so, go through
     * and clean up.
     */
    if (ds) {
	/*
	 * Free rx particles, ring and shadow
	 */
	if (ds->rxr_shadow) {
	    for (i=0; i < DEC21140_RX_RING_ENTRIES; i++) {
		mp = ds->rxr_shadow[i];
		if (mp) {
		    retparticle(mp);
		}
	    }
	    free(ds->rxr_shadow);
	}
	if (ds->rxr_malloc) {
	    free(ds->rxr_malloc);
	}

	/*
	 *  Free tx particles, paks, shadow and ring
	 */
	if (ds->txr_shadow) {
	    for (i=0; i < DEC21140_TX_RING_ENTRIES; i++) {
		mp = ds->txr_shadow[i].mp;
		pak = ds->txr_shadow[i].pak;
		if (mp) {
		    retparticle(mp);
		}
		if (pak && (pak != DEC21140_MAGIC_PAK_PTR)) {
		    datagram_done(pak);
		}
	    }
	    free(ds->txr_shadow);
	}
	if (ds->txr_malloc) {
	    free(ds->txr_malloc);
	}

	/*
	 * Free up remaining tentacles hanging off the instance and
	 * the instance itself.
	 */
	if (ds->ib_malloc) {
	    free(ds->ib_malloc);
	}
	if (ds->rx_pak) {
	    free(ds->rx_pak);
	}
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
    nfethernets--;
    nethernets--;
}

/*
 * dec21140_platform_init:
 * Platform dependant initialization for Presidio2 port adaptor interface.
 */
boolean dec21140_platform_init (hwidbtype *idb)
{
    dec21140_instance_t *ds = INSTANCE(idb);
    dec21140_regs_t *reg;

    /*
     * Configure the control registers
     */
    reg = ds->dec21140_regaddr;
    if (DEC21140_DMA_BIGENDIAN) {
	/*
	 * Setup global bus parameters with the DMA engine in big-endian
	 */
	reg->csr0  = (DEC21140_CSR0_TX_AUTO_POLL_200_US
		      | DEC21140_CSR0_CACHE_ALIGN
		      | DEC21140_CSR0_PROG_BURST_LEN
		      | DEC21140_CSR0_BIG_ENDIAN
		      | DEC21140_CSR0_DESCRPT_SKIP_LENGTH
		      | DEC21140_CSR0_BAR);
    } else {
	/*
	 * Setup global bus parameters with the DMA engine in little-endian
	 */
	reg->csr0  = (DEC21140_CSR0_TX_AUTO_POLL_200_US
		      | DEC21140_CSR0_CACHE_ALIGN
		      | DEC21140_CSR0_PROG_BURST_LEN
		      | DEC21140_CSR0_DESCRPT_SKIP_LENGTH
		      | DEC21140_CSR0_BAR);
    }

    /* 
     * Disable all interrupts and stop rx, tx processes
     */
    reg->csr7  = DEC21140_CSR7_DISABLE_ALL_INTERRUPTS;
    reg->csr6  = (DEC21140_CSR6_INITIAL_VALUE
		  | DEC21140_CSR6_RECEIVE_STOP
		  | DEC21140_CSR6_TRANSMIT_STOP);

    /* 
     * Configure these pins on the general purpose port as outputs
     */
    if (idb->slot == 0) {
	/*
	 * IO card 1fe. Set all the bits in the general purpose register
         * CSR12 to be outputs and clear all the bits.
	 */
	reg->csr12 = DEC21140_CSR12_GPC | DEC21140_CSR12_MICE_OUTPUT_MASK;
	reg->csr12 = 0x00;
    } else {
	/*
	 * Regular port adaptor
	 */
	reg->csr12 = (DEC21140_CSR12_GPC
		      | DEC21140_CSR12_OUTPUT_MASK);
	reg->csr12 = (DEC21140_CSR12_INITIAL_VALUE
		      & ~DEC21140_CSR12_MICRO_TX_ENABLE);
    }

    /*
     * Install this interface into the pa_jumptable
     */
    pas_install_port_instance(idb, TRUE);
    return(TRUE);
}

/*
 * dec21140_show_instance:
 * Platform specific printout of the instance structure
 */
void dec21140_show_instance (dec21140_instance_t *ds)
{
    /*
     * Display C7100 specific stuff
     */
    printf(" throttled=%d, enabled=%d, disabled=%d\n",
	   ds->throttled, ds->enabled, ds->disabled);
    printf(" rx_fifo_overflow=%d, rx_no_enp=%d, rx_discard=%d\n",
	   ds->rx_fifo_overflow, ds->rx_no_enp, ds->rx_discard);
    printf(" tx_underrun_err=%d, tx_jabber_timeout=%d, tx_carrier_loss=%d\n",
	   ds->tx_underrun_err, ds->tx_jabber_timeout, ds->tx_carrier_loss);
    printf(" tx_no_carrier=%d, tx_late_collision=%d, tx_excess_coll=%d\n",
	   ds->tx_no_carrier, ds->tx_late_collision, ds->tx_excess_coll);
    printf(" tx_collision_cnt=%d, tx_deferred=%d, fatal_tx_err=%d,"
           " tbl_overflow=%d\n",
           ds->tx_collision_count, ds->tx_deferred, ds->fatal_tx_err,
           ds->addr_table_overflow);
}

/*
 * dec21140_snmp_link_trap:
 * SNMP trap event support.
 */
void dec21140_snmp_link_trap (int trap, hwidbtype *idb)
{
    if (trap == DEC21140_LINK_UP) {
	reg_invoke_ifmib_link_trap(LINK_UP_TRAP, idb);
    } else {
	reg_invoke_ifmib_link_trap(LINK_DOWN_TRAP, idb);
    }
}

/*
 * dec21140_get_default_hw_address:
 * Get a default mac address for this interface
 */
void dec21140_get_default_hw_address (hwidbtype *idb, uchar addr[IEEEBYTES])
{
    c7100_assign_mac_addr(idb, addr);
}

/*
 * dec21140_init_buffer_pools:
 * Initialize buffer pools for this interface
 */
boolean dec21140_init_buffer_pools (hwidbtype *idb)
{
    dec21140_instance_t *ds;
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
				    DEC21140_PARTICLE_SIZE, 
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
	particles_in_pool = DEC21140_FAST_PARTICLE_POOL;
	pool_adjust(pool, 0, particles_in_pool, particles_in_pool, TRUE);
	particle_pool_create_cache(pool, particles_in_pool);
	pool_adjust_cache(pool, particles_in_pool);
	idb->pool = pool;
	pool->hwidb = idb;
	pool_set_cache_threshold(pool,
                                 DEC21140_BUFFER_THRESHOLD,
                                 dec21140_cache_threshold);

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
	dec21140_cleanup_pak(ds->rx_pak);
    }
    return(TRUE);
}

/*
 * dec21140_update_vendor_states:
 * Do OEM specific stuff (not applicable to C7100)
 */
void dec21140_update_vendor_states (hwidbtype *idb)
{
}

/*
 * dec21140_default_media:
 * Return the default media type, MII or RJ45. On C7100, this is always RJ45.
 */
int dec21140_default_media (hwidbtype *idb)
{
    if (idb->slot == 0)
	return(FEIP_MII);
    else
	return(FEIP_RJ45);
}

/*
 * dec21140_default_duplex:
 * Return the default half/full duplex setting. On C7100, always half duplex.
 */
boolean dec21140_default_duplex (hwidbtype *idb)
{
    return(FALSE);
}

/*
 * dec21140_post_coalesce_rx:
 * Perform post-coalesce processing on a recieved frame which we have
 * already tried to fastswitch.
 */
void dec21140_post_coalesce_rx (hwidbtype *idb, paktype *rx_pak, iqueue_t iq)
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
	dec21140_snmp_link_trap(DEC21140_LINK_UP, idb);
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
 * dec21140_post_coalesce_fs:
 * Perform post-coalesce processing on a recieved frame which we have
 * not even tried to fastswitch yet.
 */
void dec21140_post_coalesce_fs (hwidbtype *idb, paktype *rx_pak, iqueue_t iq)
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
    dec21140_post_coalesce_rx(idb, rx_pak, iq);
}

/* end of file */
      
