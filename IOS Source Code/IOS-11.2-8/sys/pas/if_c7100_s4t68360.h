/* $Id: if_c7100_s4t68360.h,v 3.1.44.14 1996/09/13 20:18:35 mbeesley Exp $
 * $Source: /release/112/cvs/Xsys/pas/if_c7100_s4t68360.h,v $
 *------------------------------------------------------------------
 * if_c7100_s4t68360.h - defines for C7100 s4t driver
 *
 * December 1995, Greg Stovall
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: if_c7100_s4t68360.h,v $
 * Revision 3.1.44.14  1996/09/13  20:18:35  mbeesley
 * CSCdi69010:  c7200 sometimes forwards an IP packet with a ttl of 1
 * Branch: California_branch
 *
 * Revision 3.1.44.13  1996/08/28  07:45:59  abaindur
 * CSCdi67069:  Data cache use needs to be turned ON for c7200 IP flow
 * switching
 * Branch: California_branch
 *  Fix mixup of cached and uncached region pointers. Fix cases where
 *  converted addresses were not restored if flow switching failed.
 *
 * Revision 3.1.44.12  1996/08/26  23:05:25  rharagan
 * CSCdi66480:  WFQ refuses to be disabled on serial port adapters
 * Branch: California_branch
 * Disable WFQ on VIP, enable WFQ on c7200 for S4T.
 *
 * Revision 3.1.44.11  1996/08/24  01:50:55  abaindur
 * CSCdi66979:  Traffic shaping broken on the c7200
 * Branch: California_branch
 *
 * Revision 3.1.44.10  1996/08/23  04:47:21  xtang
 * CSCdi59553:  clockrate 250000 is not configurable on FSIP in 75xx
 * Branch: California_branch
 *              added 250000 Hz clockrate to s4t68360 4T
 *              port adaptor. The corresponding microcode change are in
 *              cvs -d /micro s4t68360
 *
 * Revision 3.1.44.9  1996/07/17  05:00:05  rramacha
 * CSCdi63042:  Enable IP Flow/Optimum on 4T
 * Branch: California_branch
 *
 * Revision 3.1.44.8  1996/05/24  00:49:46  mbeesley
 * CSCdi58316:  Need to fully support CPQ on new interfaces
 * Branch: California_branch
 *
 * Revision 3.1.44.7  1996/04/30  22:39:57  gstovall
 * CSCdi55850:  Serial PAs reports Elapse time for download and report
 * version
 * Branch: California_branch
 * Clean up the 4T error messages.
 *
 * Revision 3.1.44.6  1996/04/30  16:49:50  gstovall
 * CSCdi55513:  Fix OIR problems
 * Branch: California_branch
 * Fix oir memory handling on 4t.
 *
 * Revision 3.1.44.5  1996/04/22  17:33:10  gstovall
 * CSCdi54526:  Need to speed up serial process level
 * Branch: California_branch
 * Use the DMA coalescer.
 *
 * Revision 3.1.44.4  1996/04/10  02:43:20  rramacha
 * CSCdi52294:  Add 4T support for VIP2
 * Branch: California_branch
 * VIP2 support for 4T
 *
 * Revision 3.1.44.3  1996/04/04  19:03:31  gstovall
 * CSCdi51393:  software forced reload in validblock, checkheaps
 * Branch: California_branch
 * Ding dong, the which is dead.  Close up the hole that caused this
 * beasty.
 *
 * Revision 3.1.44.2  1996/03/28  19:12:15  gstovall
 * CSCdi52675:  decnet fastswitching is broken on serial
 * Branch: California_branch
 * Make sure not to send particled packets to non-particle based fastswitch
 * routines.
 *
 * Revision 3.1.44.1  1996/03/21  23:22:55  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.1.2.9  1996/03/06  19:09:43  gstovall
 * CSCdi50773:  SMDS fastswitching fails without smds dxi-mode.
 * Branch: ELC_branch
 * Deal with the messed up encapsize in a better way.
 *
 * Revision 3.1.2.8  1996/03/01  23:33:11  gstovall
 * CSCdi49899:  %Software-forced reload occurred while verifying smds over
 * serial.
 * Branch: ELC_branch
 * Memory corruption, Be Gone!
 *
 * Revision 3.1.2.7  1996/03/01  18:27:49  gstovall
 * CSCdi50508:  Particle based vines fastswitching not supported
 * Branch: ELC_branch
 * Since all the protocols that are fastswitched over serial can handle
 * particles, we do not need to hack up code to check and see if we
 * should coalesce packets before fastswitching them.  Also close up
 * some more holes that would lead to memory corruption.
 *
 * Revision 3.1.2.6  1996/02/23  22:09:49  gstovall
 * CSCdi48723:  %Software-forced reload, while testing serial hdlc to
 * serial x25.
 * Branch: ELC_branch
 * The missing piece, network_start needs to be setup for the pak copy
 * routines to work as expected.
 *
 * Revision 3.1.2.5  1996/02/21  18:01:53  gstovall
 * CSCdi48723:  %Software-forced reload, while testing serial hdlc to
 * serial x25.
 * Branch: ELC_branch
 * Set the pointers correctly when we grab a new particle.  While I am
 * rummaging around in there, clean up some less than optimal code.
 *
 * Revision 3.1.2.4  1996/02/14  01:58:01  gstovall
 * CSCdi48946:  Pull in 4T bugfixes from mainline
 * Branch: ELC_branch
 * Bring the ELC 4T driver up to date.
 *
 * Revision 3.1.2.3  1996/02/05  22:17:09  gstovall
 * Branch: ELC_branch
 * Handle changing of maxdgram, strip of the CRC length correctly,
 * implement loopback.
 *
 * Revision 3.1.2.2  1996/01/26  02:50:07  gstovall
 * Branch: ELC_branch
 * Oodles of fixes and enhancements:
 *         - Added support for the crc command to the predator 4T
 *         - Added support for the ignore-dcd command to the predator 4T
 *         - Do not keep stats on a VIP 4T
 *         - Pulled in a needed bugfix (CSCdi47390)
 *         - Revived the show controller command, it should work correctly
 *           with FDDI and token ring now.
 *
 * Revision 3.1.2.1  1996/01/18  22:11:10  gstovall
 * Branch: ELC_branch
 * Preadator now knows the joy and splendor of serial interfaces.
 *
 * Revision 3.1  1996/01/18  21:56:01  gstovall
 * Placeholders for ELC_branch.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#if !defined(S4T_STATIC_INLINES_ONLY)

/*
 * Include flow/optimum switching header files
 */
#include "../ipmulticast/igmp.h"
#include "../ip/ipfast_flow.h"

/*
 * Configurable delays where needed of '360 download.
 */

#define S4T_POWERUP       10000 /* 10 ms min delay, 1000 ms max */
#define S4T_STARTUP       3000 /* 3 ms min delay, 300 ms max */

/*
 * Define tx ring entries. Must be a power of 2!!
 */

#define S4T_TX_RING_ENTRIES		64
#define S4T_TX_RING_INDEX_MASK		0x3f

/*
 * Define rx ring entries. Must be a power of 2!!
 */

#define S4T_RX_RING_ENTRIES		64
#define S4T_RX_RING_INDEX_MASK		0x3f

/*
 * Define rx ring attribute
 */
#define S4T_RX_RING_ATTRIBUTE		TLB_UNCACHED

/*
 * Define buffer pool sizes
 */
#define S4T_FAST_PARTICLE_POOL	256	/* SRAM particles */
#define S4T_SLOW_PARTICLE_POOL	0	/* DRAM particles */

/*
 * Define the particle size used by S4T
 * FIXME: should be 512 when configurable in 4t PA.
 */
#define S4T_PARTICLE_SIZE		512

 /*
 * Define Tx/Rx interrupt loop limits
 */
#define S4T_RX_MAX_SPIN		S4T_RX_RING_ENTRIES
#define S4T_TX_MAX_SPIN		S4T_TX_RING_ENTRIES

/*
 * Define address conversion macro's
 */
#define S4T_BUF_ADDR(x)		        ((ulong)(x))
#define S4T_CNTL_ADDR(x)		((ulong)(x))

#define S4T_MAX_PORTS 4

#define S4T_MAX_MTU 18024            /* max sized packet */
#define S4T_MIN_MTU 2               /* min sized packet */
#define S4T_PCIMEM_CACHE_ATTRIBUTE    TLB_UNCACHED
#define S4T_DS_SIZE (sizeof(s4t_instance_t))
#define S4T_DSCRPTR_SIZE (sizeof(s4t_dscrptr_t))  /* 8 bytes */


typedef enum {S4T_PORT_0 = 0,
              S4T_PORT_1 = 1,
              S4T_PORT_2 = 2,
              S4T_PORT_3 = 3} S4T_PORTS;

/*
 * Have the driver do packet accounting
 */
#define S4T_PKT_STATS			TRUE

/*
 * The driver should throttle when it runs out of resources
 */
#define S4T_THROTTLING		        TRUE
#define SERIAL_QUEUE_POLICY             QUEUE_FAIR

#else /*S4T_STATIC_INLINES_ONLY */

extern void s4t_post_coalesce_rx(hwidbtype *idb, paktype *pak);
extern void s4t_post_coalesce_fs(hwidbtype *idb, paktype *pak);

/*
 * s4t_giveup_cpu :
 * Return TRUE when the driver should give up the CPU
 */
static inline boolean s4t_giveup_cpu (void)
{
    /*
     * Give up the cpu if sched interval is set and it is time
     */
    if (sched_run_interval && AWAKE(sched_run_time)) {
	return(TRUE);
    } else {
	return(FALSE);
    }
}

/*
 * s4t_get_particle_inline:
 * Get a receive particle.
 */
static inline particletype *s4t_get_particle_inline (hwidbtype *idb, 
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
	 * Initialize start
	 */
	mp->data_start = particle_center(mp);
	return(mp);
    } else {
	idb->counters.output_nobuffers++;
	if (serial_debug) {
	    buginf("\n get_particle(): need a particle, failed!");
	}
	return(NULL);
    }
}
/*
 * s4t_fastsend_stats :
 * Do accounting for fast send
 */
static inline void s4t_fastsend_stats (paktype *pak, hwidbtype *idb)
{
    ulong acct_proto = (ulong)pak->acct_proto;
    
    idb->counters.tx_cumbytes_fast[acct_proto] += pak->datagramsize;
    idb->counters.outputs_fast[acct_proto] += 1;
    TIMER_START(idb->xmittimer, TIMEOUT);
}

/*
 * s4t_dfs_enabled:
 * Return TRUE if we are willing to DFS on this interface.
 */
static inline boolean s4t_dfs_enabled (hwidbtype *idb)
{
    return(FALSE);
}

/*
 * s4t_ip_dfs:
 * Distributed fastswitch an IP datagram
 */
static inline boolean s4t_ip_dfs (hwidbtype *idb, paktype *pak)
{
    return(FALSE);
}

/*
 * s4t_flow_switchable:
 * Decide whether opt/flow switchable
 */
static inline boolean s4t_flow_switchable (hwidbtype *idb) 
{
    if ((idb->ip_routecache & (IP_FLOW_ROUTECACHE|IP_OPTIMUM_ROUTECACHE)) && 
	!idb->tbridge_on_hwidb && 
	((idb->enctype == ET_HDLC) || (idb->enctype == ET_PPP)))
   	return(TRUE);
    else
	return(FALSE);
}

/*
 * s4t_ip_flow:
 * Optimum/Flow switch an IP datagram
 */
static inline boolean s4t_ip_flow (hwidbtype *idb, paktype *pak)
{
    iphdrtype *ip;

    ip = (iphdrtype *)pak->network_start;
    return ((*idb->ip_turbo_fs)(pak, ip, idb) == IPFLOWOK); 
}

/*
 * s4t_process_receive_packet:
 * We have a good, completed Rx packet. Do something with it.
 */
static inline void s4t_process_receive_packet (hwidbtype *idb,
					       s4t_instance_t *ds,
					       paktype *pak,
					       int pak_size)
{
    char *datagramstart, *old_start;
    ushort     rxtype;
    boolean flow_switch_on = FALSE;

    /*
     * Make sure the pak flags field is clean
     */
    pak->flags = 0;

    /*
     * Default the input software idb to the major
     * interface.  Subsequent tbridge code may determine
     * a different value if sub-interface bridging is
     * configured.
     *
     * Note, we do not increment the input qcount here. The packet will not
     * go onto the input queue until it is coalesced. The contigous
     * packet will then be marked with PAK_INPUTQ and the input qcount will
     * increment as long as this interface's limit is not exceeded.
     */
    pak->network_start = pak->datagramstart;
    pak->if_input = idb->firstsw;
    datagramstart = pak->datagramstart;
    old_start = datagramstart;

    /*
     * Check whether bridging is turned off, opt/flow switching is enabled
     * and encap is suitable for opt/flow switching
     */
    if (s4t_flow_switchable(idb)) {
	flow_switch_on = TRUE;
	if (C7100_FLOW_THRU_DCACHE) {
	    /*
	     * Convert datagramstart pointer to cached, write thru address
	     * and flush the first 3 lines of the packet from the data
	     * cache (ensuring no stale data from a previous packet)
	     */
	    datagramstart = CONVERT_ADDR_CWT(old_start);
	    invalidate_data_lines(datagramstart, 3);
	}
    }
    
    /*
     * Classify the packet
     */
    switch(idb->enctype) {
      case ET_HDLC:
	rxtype = hdlc_decode_encapsulation_inline(pak,
						  (vhdlctype *)datagramstart);
	break;
	
      case ET_FRAME_RELAY:
	/*
	 * Hack alert!  
	 * Since frame realy uses an all in one routine, and doesn't have a
	 * seperate classification function, we actually try to fastswitch
	 * the packet here.  Since all the protocols we fastswitch over
	 * frame relay can handle particles, this will work.
	 */
	if (reg_invoke_fr_pvc_switch_or_fs(pak)) {
	    s4t_flush_pak(pak);
	    return;
	}
	rxtype = pak->rxtype;
	
	break;

      case ET_SMDS:
	rxtype = smds_decode_encapsulation_inline(pak, datagramstart);
	
	/*
	 * Hack alert!  The above encaps decode routine is broken, adjust
	 * the encsize to make it right again.
	 */
	if (idb->smds_pdb != NULL) {
	    if (idb->smds_pdb->dximode) {
		pak->encsize += SMDS_DXIBYTES;
	    } else {
		pak->encsize += SMDS_CISCOBYTES;
	    }
	}
	
	break;

      case ET_PPP:
	rxtype = ppp_decode_encapsulation_inline(pak,
						 (ppp_hdr *)datagramstart);
	break;

      case ET_ATM_DXI:
	/*
	 * Hack alert!  
	 * Since ATM DXI uses an all in one routine, and doesn't have a
	 * seperate classification function, we actually try to fastswitch
	 * the packet here.  Since all the protocols we fastswitch over
	 * ATM DXI can handle particles, this will work.
	 */
	if (atm_dxi_classify_pak_and_fs_les(pak)) {
	    s4t_flush_pak(pak);
	    return;
	}
	rxtype = pak->rxtype;
	
	break;

      default:
	rxtype = RXTYPE_UNKNOWN;
	pak->rxtype = rxtype;
	
	/*
	 * This is needed for the pak copy stuff
	 */
	pak->network_start = pak->datagramstart;
	break;
    }

    /*
     * If the packet is for a protocol that is not presently fastswitchable
     * as a particled packet, coalesce it before doing further processing.
     * If it is a supported protocol, try and fastswitch it in particles.
     */
    if (c7100_scattered_fs_protocol(pak->rxtype)) {
	switch(idb->enctype) {
	  case ET_HDLC:
	    if (flow_switch_on && (rxtype == RXTYPE_DODIP)) {
		pak->datagramstart = datagramstart;
		if (s4t_ip_flow(idb, pak)) {
		    s4t_flush_pak(pak);
		    return;
	        }
	    }
	    /*
	     * Get back Uncached address for pak members
	     */
	    pak->datagramstart = old_start;	
	    if (flow_switch_on && C7100_FLOW_THRU_DCACHE)
		rxtype = hdlc_decode_encapsulation_inline(pak,
						  (vhdlctype *)old_start);

	    if (reg_invoke_hdlc_fs(rxtype, pak)) {
		s4t_flush_pak(pak);
		return;
	    }
	    break;
	    
	  case ET_SMDS:
	    if (reg_invoke_smds_fs(rxtype, pak)) {
		s4t_flush_pak(pak);
		return;
	    }
	    break;
	    
	  case ET_PPP:
	    if (flow_switch_on && (rxtype == RXTYPE_DODIP)) {
		pak->datagramstart = datagramstart;
		if (s4t_ip_flow(idb, pak)) {
		    s4t_flush_pak(pak);
		    return;
	        }
	    }
	    /*
	     * Get back Uncached address for pak members
	     */
	    pak->datagramstart = old_start;	
	    if (flow_switch_on && C7100_FLOW_THRU_DCACHE)
		rxtype = ppp_decode_encapsulation_inline(pak,
						 (ppp_hdr *)old_start);

	    if (reg_invoke_ppp_fs(rxtype, pak)) {
		s4t_flush_pak(pak);
		return;
	    }
	    break;
	    
	  default:
	    break;
	}
    } else {
	c7100_dma_pak_coalesce(idb, pak, s4t_post_coalesce_fs,
			       pak->datagramstart);
	return;
    }

    /*
     * Couldn't fastswitch, coaleasce the packet and send it on
     */
    c7100_dma_pak_coalesce(idb, pak, s4t_post_coalesce_rx, pak->datagramstart);
}

/*
 * s4t_output_enqueue:
 * Platform specific routine to enqueue a packet onto
 * the output holdq. On checkers, if the packet is scattered we reparent
 * end enqueue it as particles. If it is not scattered, it is queued directly.
 */
static inline void s4t_output_enqueue (paktype *pak, hwidbtype *idb)
{
    paktype *newpak;

    if (pak_has_particles(pak)) {
	/*
	 * Reparent onto a new paktype header as we can not hold onto
	 * the one passed into us.
	 */
	newpak = particle_reparent(pak, idb->headerpool);
	if (!newpak) {
	    s4t_cleanup_pak(pak);
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
     * packet accounting in holdq_enqueue_fast() if the 
     * accounting flag is not set.
     */
    if (!S4T_PKT_STATS)
	newpak->acct_proto = ACCT_PROTO_LAST;
    holdq_enqueue_fast(idb, newpak);
}

#endif /* S4T_STATIC_INLINES_ONLY */
