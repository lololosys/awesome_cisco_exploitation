/* $Id: if_vip_s4t68360.h,v 3.8.18.8 1996/08/26 23:05:38 rharagan Exp $
 * $Source: /release/112/cvs/Xsys/pas/if_vip_s4t68360.h,v $
 *------------------------------------------------------------------
 * if_vip_s4t68360.h - defines for vip s4t driver
 *
 * 3/02/95   H.M. Robison
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: if_vip_s4t68360.h,v $
 * Revision 3.8.18.8  1996/08/26  23:05:38  rharagan
 * CSCdi66480:  WFQ refuses to be disabled on serial port adapters
 * Branch: California_branch
 * Disable WFQ on VIP, enable WFQ on c7200 for S4T.
 *
 * Revision 3.8.18.7  1996/06/18  06:45:15  amcrae
 * CSCdi60053:  Ping on 4T fails with MTU 8000 on vip2
 * Branch: California_branch
 * VIP2 transmit ring enlarged. Sanity check for many particled packets
 *
 * Revision 3.8.18.6  1996/06/06  12:04:02  dlobete
 * CSCdi57944:  DFS confused about rx ignores and tx drops
 * Branch: California_branch
 *
 * Revision 3.8.18.5  1996/05/23  21:05:09  rramacha
 * CSCdi58548:  CSCdi58249 broke RVIP, SVIP image build.
 * Branch: California_branch
 *
 * Revision 3.8.18.4  1996/05/22  23:28:22  rramacha
 * CSCdi58249:  Enable IP DFS on 4T
 * Branch: California_branch
 *  o receive serial encap cmd
 *  o turn-on DFS on 4T
 *  o decode HDLC, PPP encapsulation on the received packet
 *
 * Revision 3.8.18.3  1996/04/10  02:43:30  rramacha
 * CSCdi52294:  Add 4T support for VIP2
 * Branch: California_branch
 * VIP2 support for 4T
 *
 * Revision 3.8.18.2  1996/03/21  23:24:05  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.8.18.1  1996/03/18  21:34:54  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.2.5  1996/02/14  01:58:17  gstovall
 * CSCdi48946:  Pull in 4T bugfixes from mainline
 * Branch: ELC_branch
 * Bring the ELC 4T driver up to date.
 *
 * Revision 3.2.2.4  1996/02/08  08:48:19  gstovall
 * Branch: ELC_branch
 * Sync to ELC_baseline_960205.
 *
 * Revision 3.2.2.3  1996/01/26  02:50:17  gstovall
 * Branch: ELC_branch
 * Oodles of fixes and enhancements:
 *         - Added support for the crc command to the predator 4T
 *         - Added support for the ignore-dcd command to the predator 4T
 *         - Do not keep stats on a VIP 4T
 *         - Pulled in a needed bugfix (CSCdi47390)
 *         - Revived the show controller command, it should work correctly
 *           with FDDI and token ring now.
 *
 * Revision 3.2.2.2  1996/01/16  01:43:55  dcarroll
 * Branch: ELC_branch
 * Sync ELC_branch between ELC_baseline_120995 and ELC_baseline_960110.
 *
 * Revision 3.2.2.1  1995/12/11  19:14:01  mbeesley
 * Branch: ELC_branch
 * Sync ELC_branch to ELC_baseline_120995.
 *
 * Revision 3.7  1996/02/04  09:32:52  rharagan
 * CSCdi45869:  4T does not receive data when cabled as rs-232 DCE
 * Set no_txce control based on sensed cable type, handle rx errors.
 *
 * Revision 3.6  1996/01/09  22:32:46  rharagan
 * CSCdi46625:  Holdq dequeueing broken for 4T
 * Pick up 4T version CSCdi41406. Sweep up a bit.
 *
 * Revision 3.5  1995/12/23  01:14:36  rharagan
 * CSCdi45811:  4T interface linestate does not refletct modem control
 * lines
 * Straighten out changes in linestate, continue to bring in code review
 * comments
 *
 * Revision 3.4  1995/12/10  04:01:33  smackie
 * Add support for public particle pools. Make particle chunk growth less
 * aggressive for dynamic pools. (CSCdi45336)
 *
 * Revision 3.3  1995/12/02  05:50:29  rharagan
 * CSCdi41966:  Messages between 4T PA & driver should be per interface
 * Move to single interface commands, incorporate code review comments.
 *
 * Revision 3.2  1995/11/17  18:55:04  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:58:11  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:21:41  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/06/07  22:33:20  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
#if !defined(S4T_STATIC_INLINES_ONLY)
/*
 * Include VIP header files
 */

#include "../src-vip/vip_memd.h"
#include "../src-vip/vip_af.h"

/*
 * Configurable delays where needed of '360 download.
 */

#define S4T_POWERUP       10000 /* 10 ms min delay, 1000 ms max */
#define S4T_STARTUP       3000 /* 3 ms min delay, 300 ms max */

#if defined(SVIP)
 
/*
 * Define tx ring entries. Must be a power of 2!!
 */
 
#define S4T_TX_RING_ENTRIES		128
#define S4T_TX_RING_INDEX_MASK		(S4T_TX_RING_ENTRIES-1)
 
/*
 * Define rx ring entries. Must be a power of 2!!
 */
  
#define S4T_RX_RING_ENTRIES		32
#define S4T_RX_RING_INDEX_MASK		(S4T_RX_RING_ENTRIES-1)
 
#else /* RVIP */
 
/*
 * Define tx ring entries. Must be a power of 2!!
 */
 
#define S4T_TX_RING_ENTRIES		64
#define S4T_TX_RING_INDEX_MASK		(S4T_TX_RING_ENTRIES-1)

/*
 * Define rx ring entries. Must be a power of 2!!
 */
 
#define S4T_RX_RING_ENTRIES		64
#define S4T_RX_RING_INDEX_MASK		(S4T_RX_RING_ENTRIES-1)
 
#endif /* SVIP or RVIP */

/*
 * Define MEMD polling limits.
 */
#define S4T_TXPOLL_HIGH			(S4T_TX_RING_ENTRIES/2)
#define S4T_TXPOLL_LOW			(S4T_TX_RING_ENTRIES/4)

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
 * Use the platform defined particle size.
 * 512 for RVIP, 128 for SVIP.
 */
#define S4T_PARTICLE_SIZE		VIP_PARTICLE_BYTES

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
 * Do not do any packet accounting on the VIP (host does it)
 */
#define S4T_PKT_STATS	        FALSE

/*
 * The driver should not throttle when it runs out of resources
 */
#define S4T_THROTTLING	      	FALSE
#define SERIAL_QUEUE_POLICY     QUEUE_FCFS

#else /*S4T_STATIC_INLINES_ONLY */

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
    return(idb->ip_routecache & IP_DISTRIBUTED_ROUTECACHE);
}

/*
 * s4t_ip_dfs:
 * Distributed fastswitch an IP datagram
 */
static inline boolean s4t_ip_dfs (hwidbtype *idb, paktype *pak)
{
    pas_cbtype *pascb = INSTANCE(idb);
    
    return((*pascb->pak_dfs)(idb, pak));
}

/*
 * s4t_fast_switchable:
 * Decide whether the given encap is DFS switchable
 */
static inline boolean s4t_encap_dfs_switchable (encapstype encap) 
{
    if ((encap == ET_HDLC) || (encap == ET_PPP))
 	return(TRUE);
    else
  	return(FALSE);
}

/*
 * s4t_process_receive_packet:
 * We have a good, completed Rx packet. Do something with it.
 */
static inline void s4t_process_receive_packet (hwidbtype *idb,
						 s4t_instance_t *ds,
						 paktype *rx_pak,
						 int pak_size)
{

    ushort rxtype;

    /*
     * Check if DFS support is enabled and encapsulation is DFS switchable
     */
    if (s4t_dfs_enabled(idb) && s4t_encap_dfs_switchable(idb->enctype)) {
	
	rxtype = vip_serial_decode_encap_inline(idb, rx_pak);
	
	if (rxtype == RXTYPE_DODIP) {
	    if (!s4t_ip_dfs(idb, rx_pak)) {
		s4t_cleanup_pak(rx_pak);
	    } 
	    s4t_flush_pak(rx_pak);
	    return;
	}
    }
    
    /*
     * Either DFS is turned off or the packet was not a supported
     * protocol. Just punt the frame to the host
     */
    if (!(*ds->pacb.pak_to_host)(idb, &ds->pacb, rx_pak)) {
	idb->counters.input_drops += 1;
	s4t_cleanup_pak(rx_pak);
    } else {
	idb->counters.rx_cumbytes += pak_size;
	idb->counters.inputs += 1;    
    }
    s4t_flush_pak(rx_pak);
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
    } else {
	newpak = pak;
    }
    newpak->if_output = idb->firstsw;
    /*
     * Invalidate the accounting protocol type to prevent
     * packet accounting in holdq_enqueue_fast if accounting
     * flag is not set.
     */
    if (!S4T_PKT_STATS)
        newpak->acct_proto = ACCT_PROTO_LAST;
    holdq_enqueue_fast(idb, newpak);
}
#endif /* S4T_STATIC_INLINES_ONLY */















