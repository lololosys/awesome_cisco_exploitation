/* $Id: if_c7100_ibm2692.c,v 3.1.60.19 1996/09/13 20:18:32 mbeesley Exp $
 * $Source: /release/112/cvs/Xsys/pas/if_c7100_ibm2692.c,v $
 *------------------------------------------------------------------
 * if_c7100_ibm2692.c - IBM2692 4/16 Mbit Token Ring PCI PA
 *		        (Predator dependent parts)
 *
 * December 1995, HC Wang and Stig Thormodsrud 
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: if_c7100_ibm2692.c,v $
 * Revision 3.1.60.19  1996/09/13  20:18:32  mbeesley
 * CSCdi69010:  c7200 sometimes forwards an IP packet with a ttl of 1
 * Branch: California_branch
 *
 * Revision 3.1.60.18  1996/08/28  13:05:44  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.1.60.17  1996/08/26  15:10:54  ppearce
 * CSCdi66582:  Implement SRB vector table to cut size of SRB_CORE subsys
 * Branch: California_branch
 *
 * Revision 3.1.60.16  1996/08/20  00:10:20  mbeesley
 * CSCdi66438:  show version should display amount of packet sram
 * Branch: California_branch
 *
 * Revision 3.1.60.15  1996/08/07  23:35:25  sthormod
 * CSCdi63522:  C7200: RSRB gets process switched on ether, serial, fddi
 * Branch: California_branch
 *
 * Revision 3.1.60.14  1996/07/23  18:47:51  ppearce
 * CSCdi63747:  Move srb_multiring out of SRB subblock
 * Branch: California_branch
 *   (1) srb_multiring doesn't really belong exclusively to SRB
 *         move srb_multiring out of SRB subblock and into swidb
 *         provide srb_multiring accessor routines
 *   (2) determine_rif() should take swidb as argument rather than hwidb
 *
 * Revision 3.1.60.13  1996/07/09  06:03:56  ppearce
 * CSCdi59527: inlines waste code space
 * Branch: California_branch
 *   Just say "no" to inline abuse - SRB subblock inlines removed
 *
 * Revision 3.1.60.12  1996/06/07  23:52:14  sthormod
 * CSCdi57928:  OIR of 4R Wedges Console (No Throttling Code)
 * Branch: California_branch
 *
 * Revision 3.1.60.11  1996/06/07  02:18:52  sthormod
 * CSCdi59800:  c7200 4r needs to recover from adapter check
 * Branch: California_branch
 *
 * Revision 3.1.60.10  1996/05/29  01:28:59  sthormod
 * CSCdi58443:  spurious mem access in tring driver
 * Branch: California_branch
 *
 * Revision 3.1.60.9  1996/05/23  00:52:54  sthormod
 * CSCdi57953:  Enable IP Flow/Optimum on token ring
 * Branch: California_branch
 *
 * Revision 3.1.60.8  1996/05/17  11:39:58  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.1.46.3  1996/05/05  23:42:20  ppearce
 * Sync to IbuMod_Calif_baseline_960504
 *
 * Revision 3.1.46.2  1996/04/26  15:22:36  ppearce
 * IBU modularity - SRB subblock (part 1)
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.1.46.1  1996/04/03  21:13:45  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.1.60.7  1996/05/14  02:38:53  dcarroll
 * CSCdi57529:  Decrement interface counters on OIR removal
 * Branch: California_branch
 * Decrement interface counters after OIR.
 *
 * Revision 3.1.60.6  1996/05/07  00:48:36  sthormod
 * CSCdi55832:  4r PA driver needs to clean-up after OIR
 * Branch: California_branch
 *
 * Revision 3.1.60.5  1996/04/30  17:35:59  sthormod
 * CSCdi54135:  XNS fastswitching broken on 4r PA
 * Branch: California_branch
 *
 * Revision 3.1.60.4  1996/04/23  02:23:50  sthormod
 * CSCdi54994:  4R uses cpu to byte-swap
 * Branch: California_branch
 *
 * Revision 3.1.60.3  1996/03/28  02:00:01  sthormod
 * CSCdi52811:  4R SMT stats not displayed
 * Branch: California_branch
 *
 * Revision 3.1.60.2  1996/03/27  19:46:37  hwang
 * CSCdi52699:  pma pci 16 bit read work around
 * Branch: California_branch
 *
 * Revision 3.1.60.1  1996/03/21  23:22:47  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.1.2.9  1996/03/13  16:53:07  sthormod
 * CSCdi51335:  SegV exception while configuring decnet for tokenring
 * Branch: ELC_branch
 * Don't forget to bit swap those mac addresses.
 *
 * Revision 3.1.2.8  1996/03/10  02:49:47  sthormod
 * CSCdi51196:  add tbridge switching path for predator 4R
 * Branch: ELC_branch
 *
 * Revision 3.1.2.7  1996/03/06  19:51:22  sthormod
 * CSCdi50859:  add srb switching path for predator 4R
 * Branch: ELC_branch
 * o  process & fast switching path for srb & explorers.
 * o  routing with rif field.
 * o  use dma to coalesce particle based packet.
 * o  process mac frames.
 *
 * Revision 3.1.2.6  1996/02/21  22:45:50  sthormod
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
 * Revision 3.1.2.5  1996/02/13  03:45:05  sthormod
 * CSCdi48780:  Allow coalesced fastswitching on tokenring
 * Branch: ELC_branch
 * Coalesce the packet before fastswitching if we don't support particle
 * based fastswitching for a given protocol.
 *
 * Revision 3.1.2.4  1996/02/08  08:47:40  gstovall
 * Branch: ELC_branch
 * Sync to ELC_baseline_960205.
 *
 * Revision 3.1.2.3  1996/01/31  01:37:20  sthormod
 * Branch: ELC_branch
 * Add Predator 4R support.
 *
 * Revision 3.1.2.2  1996/01/16  01:42:22  dcarroll
 * Branch: ELC_branch
 * Sync ELC_branch between ELC_baseline_120995 and ELC_baseline_960110.
 *
 * Revision 3.1.2.1  1995/12/19  23:32:28  sthormod
 * Branch: ELC_branch
 * Initial structure changes for porting 4R driver to predator.
 *
 * Revision 3.1  1995/12/19  20:55:12  sthormod
 * Placeholder for new development.
 *
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Include files
 */
#include "master.h"
#include "sys_registry.h"
#include "fastswitch_registry.h"
#include "media_registry.h"
#include "interface_private.h"
#include "packet.h"
#include "config.h"
#include "logger.h"
#include "subsys.h"
#include "parser.h"
#include "ieee.h"

#include "../os/pool.h"
#include "../os/particle.h"
#include "../os/buffers.h"
#include "../os/buffers_inline.h"

#include "../if/network.h"
#include "../if/tring.h"
#include "../if/tring_common.h"
#include "../if/tring_private.h"
#include "../if/rif.h"
#include "../if/rif_util.h"
#include "../if/pci.h"
#include "../if/msg_tr.c"		/* Not a typo, see logger.h */

#include "../srt/trsrb.h"
#include "../srt/srb_core.h"
#include "../srt/srb_multiring.h"
#include "../srt/srt_registry.h"
#include "../srt/fastsrb.h"
#include "../tbridge/tbridge_sde.h"

#include "../if/rif_inline.h"
#include "../if/tring_inline.h"

#include "../ip/ip.h"

#include "if_pas.h"
#include "pas_id.h"
#include "if_pas_ibm2692.h"
#include "msg_ibm2692.c"		/* Not a typo, see logger.h */
#include "msg_pas.c"			/* Not a typo, see logger.h */

#include "../hes/if_fci.h"                 /* required for slots.h */
#include "../hes/parser_defs_ucode.h"   /* required for slots.h */
#include "../dev/flash_dvr_spec.h"         /* required for slots.h */
#include "../hes/ucode.h"
#include "../hes/slot.h"

#include "../src-4k-c7100/c7100_macaddr.h"

#include "../ibm/lanmgr.h"
#include "../ibm/ibmnm.h"

/*
 * Include flow/optimum switching header files
 */
#include "../ipmulticast/igmp.h"
#include "../ip/ipfast_flow.h"


/*
 * ibm2692_idb_create:
 * Create an idb for an IBM2692 token ring interface. Try to get
 * it from the list of previously removed interfaces first.
 */
hwidbtype *ibm2692_idb_create (uint slot, uint port)
{
    hwidbtype *idb;

    idb = c7100_get_matching_idb(slot, port, slots[slot].pa_type, IDBTYPE_IBM2692);
    if (idb) {
        slots[slot].interface_list[port] = idb;
        slots[slot].interface_count++;
    }
    return(idb);
}

/*
 * ibm2692_deallocate:
 * Clean up all resourcse for this interface when the Port
 * adapter is removed.
 */
void ibm2692_deallocate (hwidbtype *hwidb)
{
    ibm2692_instance_t *ds;
    pooltype* pool;

    ds = INSTANCE(hwidb);
    pool = hwidb->pool;

    /*
     * Determine if an instance exists. If so, go through
     * and clean up.
     */
    if (ds) {
	int i, j;
	tx_desc_shadow_t  *tx_desc_ptr;
	rx_desc_shadow_t  *rx_desc_ptr;
	tx_bfr_desc_t     *bfr_desc;
	particletype      *mp;

	/*
	 * Free rx particles.
	 */
	for (i = 0; i < IBM2692_RX_RING_ENTRIES; i++) {
	    rx_desc_ptr = &(ds->rx_desc_shadow[i]);
   	    mp = rx_desc_ptr->particle;
 	    if (mp) {
 		retparticle(mp);
 	    }
 	}

	/*
	 *  Free tx particles or paks.
	 */
	for (i = 0; i < IBM2692_TX_RING_ENTRIES; i++) {
 	    tx_desc_ptr = &(ds->tx_desc_shadow[i]);
  	    if (tx_desc_ptr->tx_pak) {
 		datagram_done(tx_desc_ptr->tx_pak);
 	    } else {
		bfr_desc = tx_desc_ptr->cpu_view_addr;
  		for (j = 0; j < bfr_desc->bfr_count-1; j++) {
		    mp = tx_desc_ptr->particle[j];
		    if (mp) {
			retparticle(mp);
		    }
 		}
 	    }
 	}

	/*
	 * Free the blocks hanging off the instance.
	 */
	if (ds->mpc_init_rxdesc)
	    free(ds->mpc_init_rxdesc);
	if (ds->mpc_init_bptr)
	    free(ds->mpc_init_bptr);
	if (ds->txr_malloc)
	    free(ds->txr_malloc);
	if (ds->rxr_malloc)
	    free(ds->rxr_malloc);
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
     * Now free the counters subblock
     */
    if (hwidb->vip4r_info) {
	free(hwidb->vip4r_info);
	hwidb->vip4r_info = NULL;
    }
    /*
     * Decrement the interface counters (keeps show hard correct)
     */
    nTRs--;
}

/*
 * ibm2692_platform_init:
 * Platform dependant initialization for Presidio2 port adaptor interface.
 */
boolean ibm2692_platform_init (hwidbtype *idb)
{
    /*
     * Install this interface into the pa_jumptable
     */
    pas_install_port_instance(idb, TRUE);
    return(TRUE);
}

/*
 * ibm2692_show_instance:
 * Platform specific printout of the instance structure
 */
void ibm2692_show_instance (ibm2692_instance_t *ds)
{
    /*
     * Display C7100 specific stuff
     */
}

/*
 * ibm2692_snmp_link_trap:
 * SNMP trap event support.
 */
void ibm2692_snmp_link_trap (int trap, hwidbtype *idb)
{
}

/*
 * ibm2692_get_default_hw_address:
 * Get a default mac address for this interface
 */
void ibm2692_get_default_hw_address (hwidbtype *idb, uchar addr[IEEEBYTES])
{
    c7100_assign_mac_addr(idb, addr);
    ieee_swap(addr, addr);
}

/*
 * ibm2692_init_buffer_pools:
 * Initialize buffer pools for this interface
 */
boolean ibm2692_init_buffer_pools (hwidbtype *idb)
{
    ibm2692_instance_t *ds;
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
				    IBM2692_PARTICLE_SIZE, 
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
	particles_in_pool = IBM2692_RX_RING_ENTRIES * 2;
	pool_adjust(pool, 0, particles_in_pool, particles_in_pool, TRUE);
	particle_pool_create_cache(pool, particles_in_pool);
	pool_adjust_cache(pool, particles_in_pool);
	idb->pool = pool;
	pool->hwidb = idb;
	pool_set_cache_threshold(pool, IBM2692_BUFFER_THRESHOLD,
				 ibm2692_cache_threshold); 
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
	ibm2692_cleanup_pak(ds->rx_pak);
    }
    return(TRUE);
}

/* 
 * ibm2692_process_mac_frame
 */
void ibm2692_process_mac_frame (hwidbtype *idb, paktype *pak)
{
    if (!(idb->tr_ring_mode & RINGMODE_NEED_MAC)) {
	datagram_done(pak);
	return;
    }
    idb->counters.rx_cumbytes += pak->datagramsize;
    idb->counters.inputs++;
    pak->flags |= PAK_INPUTQ;
    pak->riflen = tr_riflen_pak(pak);
    pak->enctype = ET_MAC;
    (*(idb->iqueue))(idb, pak);
}

/*
 * ibm2692_post_coalesce_rx:
 * Perform post-coalescing processing on a received frame
 */
void ibm2692_post_coalesce_rx (hwidbtype *idb, paktype *rx_pak, iqueue_t iq)
{
    tring_hdr *trh;
    sap_hdr *llc1;
    ushort rxtype = rx_pak->rxtype;
    boolean ui;

    /*
     * Setup encap pointers
     */
    trh = (tring_hdr *)rx_pak->datagramstart;

    /*
     * Check for MAC frames
     */
    if ((trh->fc & FC_TYPE_MASK) == FC_TYPE_MAC) {
	ibm2692_process_mac_frame(idb, rx_pak);
	return;
    }

    /*
     * check for RSRB, etc
     */
    if (rxtype == RXTYPE_SRB) {
	if (reg_invoke_fs_rsrb_direct_input(idb, rx_pak)) {
	    return;
	}
    } else if (ROUTING_RXTYPE(idb->firstsw, rxtype) && 
	       rxtype == RXTYPE_SNAP_DODIP) {
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
     * if its an explorer and we're in a storm - drop it
     */
    if (srbV.srb_sb_get_thisring(idb->firstsw) && 
          is_tr_srb_pak(rx_pak)) {
           if (is_tr_explorer_pak(rx_pak)) {
        	if (reg_invoke_srt_flush_explorer(idb, rx_pak->datagramsize)) {
                    idb->counters.input_drops++;
	            datagram_done(rx_pak);
                    return;
                }
            }
        rx_pak->flags |= PAK_SRB_IN;
    }
    
    /*
     * We did not fast switch the datagram.
     * Pass it up to process level
     */
    idb->counters.rx_cumbytes += rx_pak->datagramsize;
    idb->counters.inputs++;

    idb->rxrifwordlen = btow(get_tr_riflen_pak(rx_pak));
    (*idb->parse_packet)(idb, rx_pak, 0);
    rx_pak->fcs_type = FCS_TYPE_TR;

    /*                                                                  
     *  It's a LLC packet.  If it is a broadcast from us, drop it.      
     */                                                                 
    if ((trh->daddr[0] & TR_GROUP) && 
	ieee_equal_sans_rii(trh->saddr, idb->hardware)) {           
	datagram_done(rx_pak);                                                
	return;
    }                                                                   

    llc1 = (sap_hdr *)rx_pak->info_start;
    
    ui = ((llc1->control & ~LLC1_P) == LLC1_UI);	
    if (srb_multiring_any_enabled(idb->firstsw) || (!ui)) {	
	rif_update_llc(idb, 0, trh->saddr, NULL,
		   (srbroute_t *)(rx_pak->riflen ? trh->data : NULL),
		       llc1, RIF_AGED); 
    }

    if (tokenring_debug) {
	tr_print_packet(idb, " in:", trh, (ulong *)llc1, 0, rx_pak->datagramsize);
    }

    /*                                                                  
     * ET_NOVELL_ETHER and ET_UNSUPPORTED are not supported -           
     *   Those two kinds of packets will be classified as               
     *   LINK_ILLEGAL as linktype, and will be bridged.                 
     */                                                                 
    if ((rx_pak->enctype != ET_NOVELL_ETHER) &&                           
 	(rx_pak->enctype != ET_UNSUPPORTED)) {                            
 	switch (llc1->control & ~LLC1_P) {
 	case LLC1_XID:
 	    tr_process_xid(idb, rx_pak, FALSE, FALSE);
	    datagram_done(rx_pak);
 	    break;
 	case LLC1_TEST:
 	    tr_process_test(idb, rx_pak, FALSE);
	    datagram_done(rx_pak);
 	    break;
 	case LLC1_UI:
 	    (*(idb->iqueue)) (idb, rx_pak);
 	    break;
 	default:
 	    reg_invoke_llc2_input(rx_pak);
 	    break;
 	}	
 	return;
    }	
     
    (*(idb->iqueue)) (idb, rx_pak);
}

/*
 * ibm2692_post_coalesce_fs
 * Perform post-coalesce processing on a received frame which we have
 * not even tried to fastswitch yet.
 */
void ibm2692_post_coalesce_fs (hwidbtype *idb, paktype *rx_pak, iqueue_t iq)
{
    /*
     * Attempt to fast switch this contigous packet.
     */
    if (reg_invoke_token_fs(rx_pak->rxtype, rx_pak)) {
	return;
    }

    /*
     * Do the remainder of the interrupt level processing
     */
    ibm2692_post_coalesce_rx(idb, rx_pak, iq);
}

/*
 * ibm2692_post_coalesce_srb
 */
void ibm2692_post_coalesce_srb (hwidbtype *idb, paktype *rx_pak, iqueue_t iq)
{
    rx_pak->riflen = tr_riflen_pak(rx_pak);

    /*
     * Upon entry to this function, rx_pak will contain the token ring
     * FCS and the FCS will be included in the length (datagramsize).
     * SRB assumes that the FCS is in there but
     * is *not* included in the length.  We must set the FCS type and
     * adjust the length so FCS is not counted.
     */
    rx_pak->fcs_type = FCS_TYPE_TR;
    rx_pak->datagramsize -= IBM2692_CRC_SIZE;

    /*
     * Try to fastswitch it if it's not an explorer
     */
    if (!is_tr_explorer_pak(rx_pak)) {
	rx_pak->rif_match = NULL;
	/*
	 * call fastsrb routines
	 */
	if ((*idb->tr_srb_fastswitch)(idb, (void *)rx_pak, idb->tr_bridge_idb)) {
	    return;
	}
    }

    /*
     * Process switch srb and explorers
     */
    idb->rxrifwordlen = btow(get_tr_riflen_pak(rx_pak));
    ((trrif_t *)rx_pak->datagramstart)->ac = AC_PRI4;
    srb_common_setup(idb, rx_pak, 0);

    idb->counters.rx_cumbytes += rx_pak->datagramsize;
    idb->counters.inputs++;
}

/*
 * ibm2692_process_receive_packet
 */
void ibm2692_process_receive_packet (hwidbtype *idb, ulong stat, 
				     paktype *rx_pak)
{
    /*
     * Make sure the pak flags field is clean
     */
    rx_pak->flags = 0;

    /*
     * If bridging is turned off and flow switching is enabled
     * then try and optimum/flow switch this packet if it is IP
     */
    if ((idb->ip_routecache & (IP_FLOW_ROUTECACHE|IP_OPTIMUM_ROUTECACHE)) && 
 	!idb->tbridge_on_hwidb && !idb->tr_bridge_idb) {
	
 	uchar 	  *start, *old_start;
 	iphdrtype *ip;
 	snap_hdr  *snap;
 	ushort    lsap, riflen;
 	tring_hdr *trh;
	
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
 	trh = (tring_hdr *)start;
	
 	/*
 	 * Check that it is IP
 	 */
 	riflen = get_tr_riflen(trh);
 	snap = (snap_hdr *)&(trh->data[riflen]);
 	lsap = GETSHORT(&snap->dsap);
 	if (riflen == 0 &&
 	    (lsap == (SAP_SNAP | SAP_SNAP << 8)) &&
	    (snap->type == TYPE_IP10MB)) {
	    
 	    ip = (iphdrtype *)(snap->data);
 	    rx_pak->datagramstart = start;
 	    if ((*idb->ip_turbo_fs)(rx_pak, ip, idb) == IPFLOWOK) {
 		ibm2692_flush_pak(rx_pak);
 		return;
 	    }
 	    rx_pak->datagramstart = old_start;
 	}
    }

    /* 
     * Make sure pak->if_input is set before calling
     * tring_decode_encaps.
     */
    rx_pak->if_input = idb->firstsw;

    /*
     * Classify the packet
     */
    rx_pak->rxtype = tring_decode_encaps(rx_pak);

    /*
     * Check for MAC frames
     */
    if ((((tring_hdr *)rx_pak->datagramstart)->fc & FC_TYPE_MASK) 
	== FC_TYPE_MAC) {
	goto ibm2692_process_level;
    }

    /*
     * Check for SRB
     */
    if ((stat & ((ulong)ROUTE_ADDR_MATCH | RX_IMAT | RX_GMAT | RX_FMAT)) ==
	(ulong)ROUTE_ADDR_MATCH) {
 	/*
 	 * MAC broadcasts will have RX_FMAT set but could be routed or
 	 * bridged depending on what is configured, so don't try to
 	 * bridge it here.
 	 */
        int fcs_fragment_size;
        particletype *new_particle;
        particletype *last_particle;
        ibm2692_instance_t *ds = INSTANCE(idb);

        /*
         * Note: SRB expects the token ring FCS to be in the packet buffer.
         * So, we must add it back on here before we coalesce.
         */
        last_particle = rx_pak->particlequeue.qtail;
        if (last_particle->data_bytes <= (IBM2692_PARTICLE_SIZE - IBM2692_CRC_SIZE)) {
            /*
             * Entire FCS is still left in the particle.  Just adjust the
             * particle's data length to pick up the FCS.
             */
            last_particle->data_bytes += IBM2692_CRC_SIZE;
        } else {
            /*
             * Only a fragment (or none) of the FCS still left in the particle.  Get the
             * remainder from the FCS save area and add it on in a new tail particle.
             */
            fcs_fragment_size = IBM2692_CRC_SIZE - 
                (IBM2692_PARTICLE_SIZE - last_particle->data_bytes);
            new_particle = ibm2692_get_particle_inline(idb, TRUE);
            if (new_particle) {
                ibm2692_particle_enqueue_inline(rx_pak, new_particle);
                last_particle->data_bytes = IBM2692_PARTICLE_SIZE;
                new_particle->data_bytes = fcs_fragment_size;
                *new_particle->data_start = ds->fcs_save;
            } else {
                idb->counters.input_resource++;
                ibm2692_cleanup_pak(rx_pak);
                return;
            }
        }
        rx_pak->datagramsize += IBM2692_CRC_SIZE;
        
	c7100_dma_pak_coalesce(idb, rx_pak, ibm2692_post_coalesce_srb,
			       (uchar *)rx_pak->info_start);
	return;
    }

    /*
     * if we are in tb or srb mode, filter packets that we transmitted
     */
    if (stat & TRANB_ADDR_MATCH) {
	boolean dmat, imat, mcast;

	dmat = imat = mcast = FALSE;
	if (stat & (ulong)DEST_ADDR_MATCH)
	    dmat = TRUE;
	
	if (stat & RX_IMAT) {
	    imat = TRUE;
	    stat &= ~RX_FMAT;  /* hermon sets this bit incorrectly */
	}
	
	if (stat & (RX_GMAT|RX_FMAT|RX_RIBC|RX_NRIBC)) 
	    mcast = TRUE;

	/*
	 * In transparent bridge mode we will receive any broadcast
	 * packet that we transmitted.  So check & drop.
	 */
	if ((stat & TRANB_ADDR_MATCH) && mcast) {
	    tring_hdr *trh = (tring_hdr *)rx_pak->datagramstart;
	    if (in_reject_list(idb, &trh->saddr[0])) {
		ibm2692_cleanup_pak(rx_pak);
		return;
	    }
	}

	/*
	 * oh, for me?
	 */
	if (imat || dmat) {
	    goto ibm2692_fastswitch;
	}
    } 

    /*
     * Check for tbridging
     */
    if (idb->tbridge_on_hwidb && reg_invoke_tbridge_forward(rx_pak)) {
	ibm2692_cleanup_pak(rx_pak);	
	return;
    }
  
 ibm2692_fastswitch:

    /*
     * If the packet is for a protocol that is not presently fastswitchable
     * as a particled packet, coalesce it before doing further processing.
     * If it is a supported protocol, try and fastswitch it in particles.
     */
    if (c7100_scattered_fs_protocol(rx_pak->rxtype)) {
	if (reg_invoke_token_fs(rx_pak->rxtype, rx_pak)) {
	    ibm2692_cleanup_pak(rx_pak);
	    return;
	}
    } else {
	c7100_dma_pak_coalesce(idb, rx_pak, ibm2692_post_coalesce_fs,
			       (uchar *)rx_pak->info_start);
	return;
    }

 ibm2692_process_level:

    /*
     * Coalesce the packet and send it on
     */
    if (!c7100_dma_pak_coalesce(idb, rx_pak, ibm2692_post_coalesce_rx,
			   (uchar *)rx_pak->info_start)) {
	ibm2692_throttle(idb);
    }
}

/*
 * ibm2692_adp_chk
 */
void ibm2692_adp_chk (hwidbtype *hwidb)
{
    ibm2692_instance_t *ds = INSTANCE(hwidb);

    /*
     * An adapter check indicates that a serious problem has been
     * detected by the adapter.  The adapter must be reset
     * following an adapter check.  The first 2 bytes of the
     * adapter check data are the Adapter Check Reason Codes and
     * are defined as: 
     * 
     *  Code  Description
     *  ====  ===========
     *  8000  Reserved for device driver use
     *  4000  (Reserved)
     *  2000  Adapter internal cache parity error
     *  1000  Adapter illegal operation
     *  0800  Adapter bus parity error
     *  0400  (Reserved)
     *  0200  (Reserved)
     *  0100  Parity error -- protocol handler local bus
     *  0080  Parity error -- protocol handler transmit FIFO
     *  0040  Parity error -- protocol handler receive FIFO
     *  0020  Protocol handler transmit FIFO underrun
     *  0010  Protocol handler receive FIFO overrun
     *  0008  Unrecognized interrupt
     *  0004  Unrecognized error interrupt
     *  0003  (Reserved)
     *  0002  Unrecognized supervisory request
     *  0001  Program detected error
     *
     * LAN Technical Reference 5-7
     */
    errmsg(&msgsym(ADPCHK, TR), hwidb->hw_namestring, 
	   ds->adp_chk_data[0],
	   ds->adp_chk_data[1],
	   ds->adp_chk_data[2],
	   ds->adp_chk_data[3]);

    net_cstate(hwidb, IDBS_DOWN);
}

/*
 * ibm2692_init_fail
 */
void ibm2692_init_fail (hwidbtype *hwidb, ushort ret_code)
{
    ibm2692_instance_t *ds = INSTANCE(hwidb);
    char *ver;

    /*
     *  Init diagnostics for the interface have failed.  Not much
     *  we can do at this point.
     * 
     *  Code  Meaning
     *  ====  =======
     *  0000  All diags completed successfully
     *  0020  Processor initialization failed
     *  0022  Microcode program storage diagnostics failed
     *  0024  Adapter RAM diagnostics failed
     *  0026  Adapter microprocessor instruction test failed
     *  0028  Adapter microprocessor interrupt test failed
     *  002A  System interface hardware test failed
     *  002C  Token-ring protocol handler test failed (digital wrap)
     *  002E  Busmaster function test failed
     *  0030  Address match RAM test failed
     *  0032  Address match Control Addressable Memory test failed
     *  003C  Token-ring protocol handler test failed (analog wrap)
     *
     * LAN Technical Reference 7-6
     */
    ver = ibm2692_version_string((uchar *)&ds->mpc_ucode_level);
    if (tokenevent_debug)
	buginf("\n%s init fail %x, version %s",
	       hwidb->hw_namestring,
	       ret_code, ver);
    errmsg(&msgsym(DIAGFAIL, TR), hwidb->hw_namestring,
	   ret_code, ver);

    net_cstate(hwidb, IDBS_RESET);
    (*hwidb->reset)(hwidb);
}

/*
 * ibm2692_lan_status_change
 */
void ibm2692_lan_status_change (hwidbtype *hwidb, ushort status)
{
    tokensb *tsb = idb_use_hwsb_inline(hwidb, HWIDB_SB_TOKEN);
    boolean display;

    /*
     * Change in lan status
     *
     * LAN Technical Reference 10-12
     */
    if (tokenevent_debug)
	buginf("\n%s ring status change %x",
	       hwidb->hw_namestring,
	       status);

    /*
     * Should we display a message for this ring status.
     */
    display = FALSE;
    if (tsb->tr_ring_status & RNG_WIRE_FAULT) {
	if (!(tsb->tr_ring_status & RNG_WIRE_FAULT))
	    display = TRUE;
	else if (AWAKE(tsb->tr_next_buginf_time))
	    display = TRUE;
    }
    
    /*
     * Call lanmgr_ringstat() here, because the interface may not be UP yet
     * The tr_ringstat_common() does not call it.
     */    
    if (hwidb->state == IDBS_UP)
	reg_invoke_media_tring_ring_status(hwidb, tsb->tr_ring_status);
    if (tr_ringstat_common(hwidb, tsb->tr_ring_status, display)) {
	TIMER_START(tsb->tr_next_buginf_time, TR_BUGINF_INTERVAL);
    }
    idb_release_hwsb_inline(hwidb, HWIDB_SB_TOKEN);
}

/*
 * ibm2692_open_fail
 */
void ibm2692_open_fail (hwidbtype *hwidb)
{
    ibm2692_instance_t *ds = INSTANCE(hwidb);
    tokensb *tsb = idb_use_hwsb_inline(hwidb, HWIDB_SB_TOKEN);
    ushort status;
    int phase, error;

    if (tokenevent_debug) 
	buginf("\n%s open failed, ret_code %04x, error_code %04x, version %s", 
	       hwidb->hw_namestring,
	       ds->mpc_srb.open_response.ret_code,
	       ds->mpc_srb.open_response.open_error_code,
	       ibm2692_version_string((uchar *)&ds->mpc_ucode_level));
    
    status = ds->mpc_srb.open_response.open_error_code;
    phase = ((status & 0x00F0) >> 4);
    error = (status & 0x000F);
    tsb->ringOpenStatus = error;
    TR_OpenError(hwidb, error, phase, TRUE);
    net_cstate(hwidb, IDBS_DOWN);
    idb_release_hwsb_inline(hwidb, HWIDB_SB_TOKEN);
}

/*
 * ibm2692_open_okay
 */
void ibm2692_open_okay (hwidbtype *hwidb)
{
    ibm2692_instance_t *ds = INSTANCE(hwidb);

    /*
     * This routine is not really necessary for predator, but vip
     * needs it to send a loveletter to the RP/RSP.
     */
    if (tokenevent_debug)
	buginf("\n%s open OK, version %s",
	       hwidb->hw_namestring, 
	       ibm2692_version_string((uchar *)&ds->mpc_ucode_level));
    
    ibm2692_get_smt_stats(hwidb);
}

/*
 * ibm2692_cfg_brg_fail
 */
void ibm2692_cfg_brg_fail (hwidbtype *hwidb)
{
    ibm2692_instance_t *ds = INSTANCE(hwidb);

    if (tokenevent_debug)
	buginf("\n%s cfg brigde failed %x", hwidb->hw_namestring, 
	       ds->mpc_srb.configure_bridge.ret_code);
}

/*
 * ibm2692_modify_srb
 */
void ibm2692_modify_srb (hwidbtype *hwidb, ulong type, ulong data)
{
    if (type == TR_MODIFY_SPAN)
        return;

    (*hwidb->reset)(hwidb);
}

/*
 * ibm2692_set_bridge_filters
 */
void ibm2692_set_bridge_filters (hwidbtype *idb)
{
    ibm2692_instance_t *ds;
    ushort *snap, *llc1, *llc2;
    idbtype *swidb = idb->firstsw;
    
    if ((idb->state != IDBS_UP) && (idb->state != IDBS_INIT))
	return;
    
    ds = INSTANCE(idb);
    if (!ds)
	return;
    
    snap = ds->snapcodes;
    llc1 = ds->llc1codes;
    llc2 = ds->llc2codes;
    
    if (ROUTING_IF(swidb, LINK_CLNS)) {
	*llc1++ = (ushort) SAP_CLNS << 8 | SAP_CLNS;
    }
    if (ROUTING_IF(swidb, LINK_NOVELL))  {
	*llc1++ = (ushort) SAP_NOVELL << 8 | SAP_NOVELL;
	*snap++ = TYPE_NOVELL1;
    }
    if (ROUTING_IF(swidb, LINK_XNS)) {
	*llc1++ = (ushort) SAP_3COM << 8 | SAP_3COM;
	*snap++ = TYPE_XNS;
    }
    if (ROUTING_IF(swidb, LINK_VINES)) {
	*llc1++ = (ushort) SAP_VINES << 8 | SAP_VINES;
	*snap++ = TYPE_VINES2;
	*snap++ = TYPE_VINES_ECHO2;
    }
    if (ROUTING_IF(swidb, LINK_IP))  {
	*snap++ = TYPE_IP10MB;
    }
    if (ROUTING_IF(swidb, LINK_DECNET))  {
	*snap++ = TYPE_DECNET;
    }
    if (ROUTING_IF(swidb, LINK_APPLETALK))  {
	*snap++ = TYPE_ETHERTALK;
    }
    *snap = 0;
    *llc1 = 0;
    *llc2 = 0;
}

/*
 * ibm2692_set_addresses
 */
boolean ibm2692_set_addresses (hwidbtype *hwidb)
{
    if (hwidb->state != IDBS_UP)
	return(FALSE);

    tr_functional_init(hwidb);
    set_ibm2692_functional_address(hwidb);

    return(TRUE);
}

/*
 * ibm2692_get_smt_stats 
 */
void ibm2692_get_smt_stats (hwidbtype *hwidb)
{
    ibm2692_instance_t *ds;
    vip4r_data_t *pa4r;

    ds = INSTANCE(hwidb);             /* source */
    pa4r = INSTANCE_4R_STATS(hwidb);  /* destination */

    /* 
     * copy SMT data
     */ 
    bcopy(&ds->mpc_parms_table, &pa4r->smt, sizeof(smt_parms_table));

    /*
     * copy addresss table data
     */
    bcopy(&ds->mpc_addr_table, &pa4r->addr_tbl, sizeof(addr_table));

    /*
     * copy ucode level
     */
    bcopy(&ds->mpc_ucode_level, &pa4r->ucode_version, MAC_VERS_LEN);

    /*
     * copy ring utilization
     */
    pa4r->ring_util = ds->mpc_utilization;
}

/*
 * MPC_ERR_UPDATE_SB
 *
 * tsb->prev_foo += mpc->foo; mpc->foo = 0;
 */
#define MPC_ERR_UPDATE_SB(foo)             \
{                                          \
    tsb->prev_ ## foo += mpc-> ## foo;     \
    mpc-> ## foo = 0;                      \
}

/*
 * TX_ERR_UPDATE
 *
 * tx_prev->foo += tx->foo; tx->foo = 0;
 */
#define TX_ERR_UPDATE(foo)                 \
{                                          \
    tx_prev-> ## foo += tx-> ## foo;       \
    tx-> ## foo = 0;                       \
}    

/*
 * RX_ERR_UPDATE
 *
 * rx_prev->foo += rx->foo; rx->foo = 0;
 */
#define RX_ERR_UPDATE(foo)                 \
{                                          \
    rx_prev-> ## foo += rx-> ## foo;       \
    rx-> ## foo = 0;                       \
}    

/*
 * ibm2692_add_counters
 */
void ibm2692_add_counters (hwidbtype *hwidb)
{
    /*
     * These counters really could be clean up a bit to reduce the
     * duplication.  Moving "mpc_err", "tx_err", and "rx_err" from the
     * ds to the idb 4r subblock might be a good start.
     */

    ibm2692_instance_t *ds;
    vip4r_data_t *pa4r;    
    tokensb *tsb;
    mpc_errors *mpc;
    tx_errors *tx, *tx_prev;
    rx_errors *rx, *rx_prev;

    ds = INSTANCE(hwidb);               
    tsb = idb_use_hwsb(hwidb, HWIDB_SB_TOKEN);
    tring_update_counters(tsb);
    idb_release_hwsb(hwidb, HWIDB_SB_TOKEN);
    
    pa4r = INSTANCE_4R_STATS(hwidb);
    if (!pa4r) 
	return;

    mpc = &ds->mpc_err;
    MPC_ERR_UPDATE_SB(err_line);
    MPC_ERR_UPDATE_SB(err_internal);
    MPC_ERR_UPDATE_SB(err_burst);
    MPC_ERR_UPDATE_SB(err_arifci);
    MPC_ERR_UPDATE_SB(err_abort);
    MPC_ERR_UPDATE_SB(err_lostframe);
    MPC_ERR_UPDATE_SB(err_copy);
    MPC_ERR_UPDATE_SB(err_rcvcongestion);
    MPC_ERR_UPDATE_SB(err_token);

    tx = &ds->tx_err;
    tx_prev = &pa4r->tx_prev;
    TX_ERR_UPDATE(tx_pci_read_error);
    TX_ERR_UPDATE(tx_icd_read_parity_error);
    TX_ERR_UPDATE(tx_icd_write_parity_error);
    TX_ERR_UPDATE(tx_icd_addr_parity_error);
    TX_ERR_UPDATE(tx_stat_write_error);
    TX_ERR_UPDATE(tx_fifo_underrun);
    TX_ERR_UPDATE(tx_fifo_parity_error);

    rx = &ds->rx_err;
    rx_prev = &pa4r->rx_prev;
    RX_ERR_UPDATE(rx_pci_read_parity_error);
    RX_ERR_UPDATE(rx_int_read_parity_error);
    RX_ERR_UPDATE(rx_int_write_parity_error);
    RX_ERR_UPDATE(rx_int_pci_address_bfr);
    RX_ERR_UPDATE(rx_fifo_parity_error);
    RX_ERR_UPDATE(rx_no_data_bfr);
    RX_ERR_UPDATE(rx_pci_write_error);
    RX_ERR_UPDATE(rx_fifo_overrun);
    RX_ERR_UPDATE(rx_parity_error);
    RX_ERR_UPDATE(rx_mpc_error);
    RX_ERR_UPDATE(rx_imp_abrt_frm);
    RX_ERR_UPDATE(rx_exp_abrt_frm);
}

/*
 * ibm2692_rev_okay
 */
boolean ibm2692_rev_okay (int pa_bay)
{
    /*
     * The rev 1 predator NPE150 cpu has no hardware support for
     * byte-swapping the token ring frames, so it has to be done 
     * with the cpu.  The code now only supports hardware assisted 
     * byte-swap, so here we verify that we're running on at least 
     * a rev 2 NPE150 cpu.
     */

    if ((c7100_cpucard_type() == C7100_CPU_NPE150) &&
	(c7100_cpucard_version() < C7100_VERSION_REV2)) {
	errmsg(&msgsym(OLDREV, IBM2692), pa_bay);
	return FALSE;
    }
    
    return TRUE;
}

/* end of file */
