/* $Id: if_c7100_s4t68360.c,v 3.1.44.20 1996/08/28 13:05:50 thille Exp $
 * $Source: /release/112/cvs/Xsys/pas/if_c7100_s4t68360.c,v $
 *------------------------------------------------------------------
 * if_c7100_s4t68360.c - C7100 PCI Adaptor s4t media driver
 *
 * December 1995, Greg Stovall
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: if_c7100_s4t68360.c,v $
 * Revision 3.1.44.20  1996/08/28  13:05:50  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.1.44.19  1996/08/13  02:21:10  gstovall
 * CSCdi39484:  Giant packets output on ethernet interface
 * Branch: California_branch
 * Make sure we drop giants, even if they are just a wee bit too big.
 *
 * Revision 3.1.44.18  1996/08/13  00:35:56  gstovall
 * CSCdi65758:  s4t driver clear datagramsize field with contiguous buffers
 * Branch: California_branch
 * Don't flush the packet if it has already been coalesced.
 *
 * Revision 3.1.44.17  1996/08/09  23:32:22  amcrae
 * CSCdi65046:  4T is reset during configuration, fails to restart,
 * disappears
 * Branch: California_branch
 * OIR driven restart has wrong retry count. Provide some delays when
 * checking AMCC registers. Will no one rid me of this troublesome board?
 *
 * Revision 3.1.44.16  1996/08/07  23:35:31  sthormod
 * CSCdi63522:  C7200: RSRB gets process switched on ether, serial, fddi
 * Branch: California_branch
 *
 * Revision 3.1.44.15  1996/07/23  00:04:45  gstovall
 * CSCdi62888:  OIR serial PA causes %ALIGN-1-FATAL pc=s4t_init
 * Branch: California_branch
 * When we nuke the instance, also reset the idb vectors to point at
 * benign functions.
 *
 * Revision 3.1.44.14  1996/07/11  14:21:00  fbordona
 * CSCdi62521:  DLSw circuit does not connect using FST over ATM peer
 *              Remove DLSw FST hooks from platform/media specific drivers.
 * Branch: California_branch
 *
 * Revision 3.1.44.13  1996/06/18  19:25:43  gstovall
 * CSCdi58572:  Serial line comes back from the dead
 * Branch: California_branch
 * Check physical line state before deciding if the line is up or not.
 *
 * Revision 3.1.44.12  1996/06/11  13:43:40  dcarroll
 * CSCdi59788:  Router crashes doing OIR on 4E an 4R port adapters
 * Branch: California_branch
 * Use pas_allocate_fspak() for rx_pak.
 *
 * Revision 3.1.44.11  1996/06/07  04:08:10  amcrae
 * CSCdi56758:  4T does not recover after power surge - should be restarted
 * Branch: California_branch
 * Added structure to carry 4T PA data. Added periodic watchdog check
 * and restart handling. Nuked some unused counters.
 *
 * Revision 3.1.44.10  1996/05/10  19:05:04  gstovall
 * CSCdi56862:  PPP: LCP not open, when testing ddr
 * Branch: California_branch
 * Make the pas serial code support the dialer.
 *
 * Revision 3.1.44.9  1996/05/07  23:21:10  gstovall
 * CSCdi56424:  OIR-insertion of 4T wedges the console
 * Branch: California_branch
 * Decrement nserialnets when yanking out a 4T.
 *
 * Revision 3.1.44.8  1996/05/06  16:42:59  gstovall
 * CSCdi54863:  Priority queuing not working on new interface
 * Branch: California_branch
 * Allow the various queueing options to be enabled on the 4T in a
 * predator.
 *
 * Revision 3.1.44.7  1996/04/30  16:49:48  gstovall
 * CSCdi55513:  Fix OIR problems
 * Branch: California_branch
 * Fix oir memory handling on 4t.
 *
 * Revision 3.1.44.6  1996/04/29  23:58:41  syiu
 * CSCdi56118:  Spurious dialer ddb when reg_invoke is better
 * Branch: California_branch
 *
 * Revision 3.1.44.5  1996/04/27  06:39:52  syiu
 * Branch: California_branch
 * Commit Dial1_branch
 *
 * Revision 3.1.44.4  1996/04/22  17:37:51  gstovall
 * CSCdi54890:  New serial driver should use common routine
 * Branch: California_branch
 * Use serial_queue_for_process().
 *
 * Revision 3.1.62.1  1996/04/26  01:01:07  syiu
 * Branch: Dial1_branch
 * Dial1_branch to IosSec_postmerge_960419
 *
 * Revision 3.1.44.3  1996/04/10  02:43:19  rramacha
 * CSCdi52294:  Add 4T support for VIP2
 * Branch: California_branch
 * VIP2 support for 4T
 *
 * Revision 3.1.44.2  1996/03/28  19:12:13  gstovall
 * CSCdi52675:  decnet fastswitching is broken on serial
 * Branch: California_branch
 * Make sure not to send particled packets to non-particle based fastswitch
 * routines.
 *
 * Revision 3.1.44.1  1996/03/21  23:22:54  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.1.2.8  1996/03/01  18:27:48  gstovall
 * CSCdi50508:  Particle based vines fastswitching not supported
 * Branch: ELC_branch
 * Since all the protocols that are fastswitched over serial can handle
 * particles, we do not need to hack up code to check and see if we
 * should coalesce packets before fastswitching them.  Also close up
 * some more holes that would lead to memory corruption.
 *
 * Revision 3.1.2.7  1996/02/21  18:01:52  gstovall
 * CSCdi48723:  %Software-forced reload, while testing serial hdlc to
 * serial x25.
 * Branch: ELC_branch
 * Set the pointers correctly when we grab a new particle.  While I am
 * rummaging around in there, clean up some less than optimal code.
 *
 * Revision 3.1.2.6  1996/02/10  01:05:13  gstovall
 * Branch: ELC_branch
 * Idle mark default bad, idle flag default good.
 *
 * Revision 3.1.2.5  1996/02/08  08:47:42  gstovall
 * Branch: ELC_branch
 * Sync to ELC_baseline_960205.
 *
 * Revision 3.1.2.4  1996/02/05  22:17:07  gstovall
 * Branch: ELC_branch
 * Handle changing of maxdgram, strip of the CRC length correctly,
 * implement loopback.
 *
 * Revision 3.1.2.3  1996/01/26  02:50:05  gstovall
 * Branch: ELC_branch
 * Oodles of fixes and enhancements:
 *         - Added support for the crc command to the predator 4T
 *         - Added support for the ignore-dcd command to the predator 4T
 *         - Do not keep stats on a VIP 4T
 *         - Pulled in a needed bugfix (CSCdi47390)
 *         - Revived the show controller command, it should work correctly
 *           with FDDI and token ring now.
 *
 * Revision 3.1.2.2  1996/01/24  04:16:35  gstovall
 * Branch: ELC_branch
 * Pull in some bug fixes from the 11.1 mainline, make fastswitching work
 * AND count correctly, notice if a cable gets pulled.
 *
 * Revision 3.1.2.1  1996/01/18  22:11:08  gstovall
 * Branch: ELC_branch
 * Preadator now knows the joy and splendor of serial interfaces.
 *
 * Revision 3.1  1996/01/18  21:56:00  gstovall
 * Placeholders for ELC_branch.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Include files
 */
#include "master.h"
#include "interface_private.h"
#include "fastswitch_registry.h"
#include "media_registry.h"
#include "sys_registry.h"
#include "../wan/dialer_registry.h"
#include "packet.h"
#include "logger.h"

#include "../os/pool.h"
#include "../os/particle.h"
#include "../os/buffers.h"
#include "../os/buffers_inline.h"

#include "../if/network.h"

#include "ieee.h"
#include "../wan/serial.h"
#include "../wan/serial_private.h"
#include "../wan/ppp.h"
#include "../wan/dialer.h"
#include "../wan/smds.h"
#include "../wan/atm_dxi.h"
#include "../ip/ip.h"
#include "../tcp/tcpcompress.h"
#include "../if/serial_inline.h"

#include "../snmp/ifmib_registry.h"

#include "if_pas.h" 
#include "if_pas_s4t68360.h"
#include "msg_pas.c"			/* Not a typo, see logger.h */

#include "../hes/if_fci.h"
#include "pas_id.h"
#include "../hes/parser_defs_ucode.h"
#include "../dev/flash_dvr_spec.h"
#include "../hes/ucode.h"
#include "../hes/slot.h"
#include "../if/linkdown_event.h"

/*
 * s4t_get_particle:
 * Get an rx particle for the device
 */
particletype *s4t_get_particle (hwidbtype *idb)
{
    return(s4t_get_particle_inline(idb, FALSE));
}

/*
 * s4t_show_instance:
 * Platform specific printout of the instance structure
 */
void s4t_show_hw_instance (s4t_instance_t *ds)
{
    
}


/*
 * s4t_snmp_link_trap:
 * SNMP trap event support.
 */
void s4t_snmp_link_trap (int trap, hwidbtype *idb)
{
    if (trap == S4T_LINK_UP) {
	reg_invoke_ifmib_link_trap(LINK_UP_TRAP, idb);
    } else {
	reg_invoke_ifmib_link_trap(LINK_DOWN_TRAP, idb);
    }
}

/*
 * s4t_lineproto_change:
 * Line protocol change notification.
 */
void s4t_lineproto_change (hwidbtype *idb, boolean state)
{
    /*
     * Not used on predator
     */
}

/*
 * s4t_init_buffer_pools:
 * Initialize buffer pools for this interface
 */
boolean s4t_init_buffer_pools (hwidbtype *idb)
{
    s4t_instance_t *ds;
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
				    S4T_PARTICLE_SIZE, 
				    POOL_SANITY,
				    32,
				    buffer_mempool);
	if (!pool) {
	    return(FALSE);
	}
	
	/*
	 * Add a fallback particle pool
	 */
	pool->fallback = pas_interface_fallback_pool(idb);
	
	/*
	 * Populate the pool and add a cache
	 */
	particles_in_pool = S4T_RX_RING_ENTRIES * 3;
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
	s4t_cleanup_pak(ds->rx_pak);
    }
    return(TRUE);
}

/*
 * s4t_set_maxdgram
 * Set max rx buffer size for interface.
 *
 */
boolean s4t_set_maxdgram (hwidbtype *hwidb, int buffersize, int maxdgram)
{
    s4t_instance_t *ds;

    ds = INSTANCE(hwidb);
    if_maxdgram_adjust(hwidb, maxdgram);
    ds->init_block->mtu = hwidb->max_buffer_size;
    s4t_init(hwidb);
    hwidb->counters.resets++;
    return(TRUE);
}

/*
 * s4t68360_idb_create:
 * Create an idb for an s4t68360 serial interface.
 */
hwidbtype *s4t68360_idb_create (s4t_pa *pa, uint port)
{
    hwidbtype *hwidb;

    hwidb = c7100_get_matching_idb(pa->slot, port, slots[pa->slot].pa_type,
				   IDBTYPE_S4T68360);
    if (hwidb) {
        slots[pa->slot].interface_list[port] = hwidb;
        slots[pa->slot].interface_count++;
    }

    return(hwidb);
}

/*
 * s4t_deallocate:
 * Clean up all resources for this interface.
 */
void s4t_deallocate (hwidbtype *hwidb)
{
    int ix;
    s4t_instance_t *ds;
    s4t_pa *pa;
    pooltype* pool;

    ds = INSTANCE(hwidb);
    pool = hwidb->pool;

    /*
     * Determine if an instance exists. If so, go through
     * and clean up.
     */
    if (ds) {
	/*
	 * Null out the function vectors, just in case they get called after
	 * the card is pulled, and they *do* get called.
	 */
	hwidb->soutput = (soutput_t)return_nothing;
	hwidb->reset = (reset_t)return_nothing;
	hwidb->shutdown = (shutdown_t)return_nothing;
	hwidb->device_periodic = NULL; 
	hwidb->enable = NULL;
	hwidb->txqlength = NULL;
	hwidb->show_controller = NULL;
	hwidb->fastsend = (fastsend_t)return_nothing;
	hwidb->show_serial_state = NULL;
	hwidb->set_maxdgram = (set_maxdgram_t)return_false;

	/*
	 * Free rx particles.
	 */
	if (ds->rxr_shadow) {
	    for (ix = 0; ix < S4T_RX_RING_ENTRIES; ix++) {
		particletype *mp;
		
		mp = ds->rxr_shadow[ix];
		if (mp) {
		    retparticle(mp);
		}
	    }
	    free(ds->rxr_shadow);
	}

	/*
	 * Free tx particles.
	 */
	if (ds->txr_shadow) {
	    for (ix = 0; ix < S4T_TX_RING_ENTRIES; ix++) {
		particletype *mp;
		paktype *pak;

		mp = ds->txr_shadow[ix].mp;
		pak = ds->txr_shadow[ix].pak;
		if (mp) {
		    retparticle(mp);
		}
		if (pak && (pak != S4T_MAGIC_PAK_PTR)) {
		    datagram_done(pak);
		}
	    }
	    free(ds->txr_shadow);
	}

	/*
	 * Free the blocks hanging off the instance.
	 */
	if (ds->rxr_malloc)
	    free(ds->rxr_malloc);
	if (ds->txr_malloc)
	    free(ds->txr_malloc);
	if (ds->init_block_malloc)
	    free(ds->init_block_malloc);
	if (ds->fw_stats)
	    free(ds->fw_stats);
	if (ds->rx_pak)
	    free(ds->rx_pak);

	/*
	 * Free the pa specific structure if nobody is referencing it.
	 */
	pa = ds->pa_info;
	pa->idbs[hwidb->subunit] = 0;
	if (--pa->references <= 0) {
	    s4t_info[pa->slot] = NULL;
	    free(pa);
	}
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

    nserialnets--;
}

/*
 * Registry function for setting transmit delay.
 */
static void s4t_set_transmit_delay (hwidbtype *hwidb, int value)
{
    hwidb->xmitdelay = value;
}

/*
 * s4t_platform_init:
 * Platform dependant initialization for MC68360 4T port adaptor interface.
 */
boolean s4t_platform_init (hwidbtype *hwidb)
{
    /*
     * Install this interface into the pa_jumptable
     */
    pas_install_port_instance(hwidb, TRUE);
    reg_add_media_transmit_delay(hwidb->type, s4t_set_transmit_delay,
			"s4t_set_transmit_delay");
    return(TRUE);
}

/*
 * s4t_post_coalesce_rx:
 * Perform post-coalescing processing on a recieved frame
 */
void s4t_post_coalesce_rx (hwidbtype *idb, paktype *pak)
{
    ushort     rxtype;

    rxtype = pak->rxtype;

       
    /*
     * 4T, HSSI, Mx serial on 7200 coalesce routine call 
     * idb->parse_packet but shouldn't
     * increment idb->counters.input_broad. See CSCdi87081
     */
    if (idb->enctype == ET_HDLC)  {
        if((*pak->datagramstart) & (HDLC_BCAST >> 8)) {
            idb->counters.input_broad--;
        }
    }
  
    /*
     * Handle receive frame special cases
     */
    switch(rxtype) {
	/*
	 * Consider a frame for bridging
	 */
      case RXTYPE_TRANSIT:
	if (idb->tbridge_on_hwidb) {
	    /*
	     * Mark as bridge encapsulation
	     */
	    pak->enctype = ET_BRIDGE;

	    if (reg_invoke_tbridge_forward(pak)) {
		return;
	    }
	}
	break;
	
	/*
	 * Consider a frame for RSRB
	 */
      case RXTYPE_SRB:
	if (reg_invoke_fs_rsrb_direct_input(idb, pak)) {
	    return;
	}
	break;

	/*
	 * Consider a frame for DLSw
	 */
      case RXTYPE_DLSW:
	if (reg_invoke_nim_dlsw_input(idb, pak)) {
	    return;
	}
	break;

      case RXTYPE_LEX:
	{ 
	    idbtype *lex_swidb = idb->firstsw->lex_swidb;
	    
	    /*
	     * Lex not bound to serial.
	     */
	    if (lex_swidb == NULL) {
		datagram_done(pak);
		return;
	    }
	    
	    /*
	     * Only bridge protocols that we are not routing.
	     */
	    if (BRIDGING_RXTYPE(idb->firstsw, pak->rxtype)) {
		/*
		 * Is there a bridge group on the lex interface.
		 */
		if (lex_swidb->tbridge_on_swidb) {
		    /*
		     * Mark as bridge encapsulation
		     */
		    pak->enctype = ET_BRIDGE;
		    /*
		     * Change the input interface so that it points to
		     * the lex interface.
		     */
		    change_if_input(pak, lex_swidb);
		    
		    if (reg_invoke_tbridge_forward(pak)) { 
			return;
		    }
		    change_if_input(pak, idb->firstsw);
		}
	    }
	}
	break;
	
	/*
	 * Consider a frame for FST
	 */
      case RXTYPE_DODIP:
	if (ROUTING_IF(idb->firstsw, LINK_IP)) {
	    iphdrtype *ip;
	    
	    ip = (iphdrtype *)ipheadstart(pak);
	    switch (ip->prot) {
	    case GRE_PROT:
	    case IPINIP_PROT:
		if (reg_invoke_tunnel_ip_les_fs(pak)) {
		    return;
		}
		break;
	    case FST_RSRB_PROT:
		if (reg_invoke_fs_rsrb_fst_input(idb, pak)) {
		    return;
		}
		break;
	    }
	}
	
      default:
	break;
    }
    
    serial_queue_for_process(idb, pak);
}

/*
 * s4t_periodic
 * Called every 10 seconds. Check the health of the PA.
 */
void s4t_periodic (hwidbtype *idb)
{
    if (idb->subunit == 0) {
	s4t_instance_t *ds = INSTANCE(idb);

	s4t_check_pa_health(ds->pa_info);
    }
}

/*
 * s4t_status_int_handle()
 * Do status interrupt handling
 */
void s4t_status_int_handle (hwidbtype *hwidb)
{
    s4t_instance_t *ds;
    uchar newstate;
    serialsb *ssb;

    ds = INSTANCE(hwidb);
    ssb = idb_get_hwsb_inline(hwidb, HWIDB_SB_SERIAL);

    if (hwidb->state != IDBS_ADMINDOWN) {
	/*
	 * Check for line up
	 */
	if (ds->port_status & ds->up_signal) {
	    /*
	     * Cool, it is
	     */
	    if ((ds->port_status & S4T_STATUS_LOOP) == 0) {
		newstate = IDBS_UP;
	    } else {
		/*
		 * Port is looped
		 */
		if (!hwidb->dte_interface) {
                    /*
                     * In DCE mode, make sure we are generating 
                     * the clock before declaring the link as up.
                     */
		    newstate = ssb->serial_clockindex ? IDBS_UP : IDBS_DOWN;
                } else {
                    newstate = IDBS_UP;
                }
	    }
	} else {
	    /*
	     * Bummer, line is down.
	     */
	    newstate = IDBS_DOWN;
	    hwidb->failCause = Serial_SignalLost;
	}
        if ((hwidb->state != newstate) &&
            (hwidb->state != IDBS_ADMINDOWN) &&
	    !reg_invoke_dialer_spoofing_int(hwidb)) {
            hwidb->counters.transitions++;
            net_cstate(hwidb, newstate);
        }

	/*
	 * If we are spoofing, that means that we are pretending that the
	 * idb state is IDBS_UP even though it isn't.  So, if carrier has
	 * come up (newstate == IDBS_UP) and we are spoofing, act the same
	 * as if idb->state != newstate.  Spoofing will be cleared in the
	 * timer routine which deals with bouncing signals.
	 */
	if ((reg_invoke_dialer_spoofing_int(hwidb) && newstate == IDBS_UP) &&
	    (hwidb->state != IDBS_ADMINDOWN)) {
	    hwidb->counters.transitions++;
	    net_cstate(hwidb, newstate);
	}
    }
}

/*
 * s4t_post_coalesce_fs:
 * Perform post-coalesce processing on a recieved frame which we have
 * not even tried to fastswitch yet.
 */
void s4t_post_coalesce_fs (hwidbtype *idb, paktype *pak)
{
    ushort rxtype;

    rxtype = pak->rxtype;

    /*
     * Attempt to fast switch this contigous packet.
     */
    switch(idb->enctype) {
      case ET_HDLC:
	if (reg_invoke_hdlc_fs(rxtype, pak)) {
	    return;
	}
	break;

      case ET_SMDS:
	if (reg_invoke_smds_fs(rxtype, pak)) {
	    return;
	}
	break;

      case ET_PPP:
	if (reg_invoke_ppp_fs(rxtype, pak)) {
	    return;
	}
	break;
    }

    /*
     * Do the remainder of the interrupt level processing
     */
    s4t_post_coalesce_rx(idb, pak);
}

/*
 * Set up the clockrate
 */
void s4t_init_clockrate (hwidbtype *hwidb)
{
    s4t_instance_t *ds = INSTANCE(hwidb);
    serialsb *ssb;

    ssb = idb_get_hwsb_inline(hwidb, HWIDB_SB_SERIAL);
    ds->init_block->tx_bit_rate =
	s4t_clock[ssb->serial_clockindex].bitrate_code;
}

/*
 *  s4t_is_really_dead_jim - 4T not responding anymore.
 *  Hit THE BIG RED BUTTON and give some CPR.
 */
void s4t_is_really_dead_jim (s4t_pa *pa)
{
    uint subunit;
    hwidbtype *idb;
    leveltype level;

    level = raise_interrupt_level(ALL_DISABLE);
/*
 *  Disable the PA interrupts, and kill off the hardware.
 */
    pas_netio_interrupts(pa->slot, FALSE);
    for (subunit = 0; subunit < S4T_MAX_PORTS; subunit++) {
	idb = pa->idbs[subunit];
	if (idb && idb->state != IDBS_ADMINDOWN) {
	    idb->status |= IDB_NOHARDWARE;
	}
    }
    slots[pa->slot].retry_ok = TRUE;
    slots[pa->slot].retry_count = 2;
    pas_deactivate_bay(pa->slot);
    reset_interrupt_level(level);
/*
 *  From here, OIR takes over.  It will try to reactivate the PA.
 */
}

/*
 * Report back if the line should be forced up or not
 */
boolean s4t_line_is_up (hwidbtype *hwidb)
{
    s4t_instance_t *ds;

    ds = INSTANCE(hwidb);

    if (ds->port_status & ds->up_signal) {
	return (TRUE);
    } else {
	return (FALSE);
    }
}
