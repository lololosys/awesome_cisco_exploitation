/* $Id: if_vip_ibm2692.h,v 3.9.12.5 1996/06/07 23:52:30 sthormod Exp $
 * $Source: /release/112/cvs/Xsys/pas/if_vip_ibm2692.h,v $
 *------------------------------------------------------------------
 * if_vip_ibm2692.h - Checkers/Viper specific header file for 4R
 *		      IBM Token Ring PCI port adaptor
 *
 * May 1995, HC Wang
 * 
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: if_vip_ibm2692.h,v $
 * Revision 3.9.12.5  1996/06/07  23:52:30  sthormod
 * CSCdi57928:  OIR of 4R Wedges Console (No Throttling Code)
 * Branch: California_branch
 *
 * Revision 3.9.12.4  1996/05/13  04:58:14  hwang
 * CSCdi57003:  4R DFS support
 * Branch: California_branch
 *
 * Revision 3.9.12.3  1996/04/10  04:43:33  tboyle
 * CSCdi54205:  No room for more than 1 4R
 * Branch: California_branch
 *
 * Revision 3.9.12.2  1996/03/21  23:23:58  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.9.12.1  1996/03/18  21:34:50  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.2.9  1996/03/10  02:50:01  sthormod
 * CSCdi51196:  add tbridge switching path for predator 4R
 * Branch: ELC_branch
 *
 * Revision 3.2.2.8  1996/03/06  19:51:55  sthormod
 * CSCdi50859:  add srb switching path for predator 4R
 * Branch: ELC_branch
 * o  process & fast switching path for srb & explorers.
 * o  routing with rif field.
 * o  use dma to coalesce particle based packet.
 * o  process mac frames.
 *
 * Revision 3.2.2.7  1996/02/21  22:46:06  sthormod
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
 * Revision 3.2.2.6  1996/02/08  08:48:15  gstovall
 * Branch: ELC_branch
 * Sync to ELC_baseline_960205.
 *
 * Revision 3.2.2.5  1996/01/31  01:37:40  sthormod
 * Branch: ELC_branch
 * Add Predator 4R support.
 *
 * Revision 3.2.2.4  1996/01/16  01:43:42  dcarroll
 * Branch: ELC_branch
 * Sync ELC_branch between ELC_baseline_120995 and ELC_baseline_960110.
 *
 * Revision 3.2.2.3  1995/12/19  23:32:40  sthormod
 * Branch: ELC_branch
 * Initial structure changes for porting 4R driver to predator.
 *
 * Revision 3.2.2.2  1995/12/06  04:08:55  sthormod
 * Branch: ELC_branch
 * Bring in HC's code review changes.
 *
 * Revision 3.2.2.1  1995/12/04  22:18:44  mbeesley
 * Branch: ELC_branch
 * Perform mini-sync - particle refcount changes.
 * Revision 3.8  1996/02/05  03:37:10  hwang
 * CSCdi47198:  router with rvip (4R & 4T) hang after config SRT
 * ibm token ring chip if configured to transparent bridge mode it will
 * copy broad cast frame it transmitted, which causing the network
 * flooding.
 * Implement software filter to drop such frame received.
 *
 * Revision 3.7  1996/01/23  23:50:52  hwang
 * CSCdi46141:  rvip 4r/4r crashes with ipx traffic
 * Hermon tx bug work around, don't give hermon any packet to xmt before
 * end of chain tx interrupt.
 *
 * Revision 3.6  1996/01/13  02:51:44  rharagan
 * CSCdi46869:  Enable memory mapping for VIP
 * Change the default PCI access mode.
 *
 * Revision 3.5  1995/12/10  04:01:30  smackie
 * Add support for public particle pools. Make particle chunk growth less
 * aggressive for dynamic pools. (CSCdi45336)
 *
 * Revision 3.4  1995/12/03  06:39:45  hwang
 * CSCdi44890:  incorporate code review comments into 4r driver
 *
 * Revision 3.3  1995/11/28  02:26:20  smackie
 * Move particle refcount into local memory to improve handling speed. Add
 * utility functions to allow some particle internals to be removed from
 * the drivers. (CSCdi44505)
 *
 * Revision 3.2  1995/11/17  18:54:58  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:58:06  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:21:39  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/06/07  22:33:18  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#if !defined(IBM2692_STATIC_INLINES_ONLY)

/*
 * Include VIP header files
 */
#include "../src-vip/vip_memd.h"

#if defined(SVIP)

/*
 * Define rx/tx ring entries. Must be a power of 2!!
 */
#define IBM2692_TX_RING_ENTRIES		64
#define IBM2692_TX_RING_INDEX_MASK	(IBM2692_TX_RING_ENTRIES - 1)
                   
#define IBM2692_RX_RING_ENTRIES		64
#define IBM2692_RX_RING_INDEX_MASK	(IBM2692_RX_RING_ENTRIES - 1)

/*
 * Define the minibuffer size used by IBM2692
 */
#define IBM2692_PARTICLE_SIZE 512


#else /* RVIP */

/*
 * Define rx/tx ring entries. Must be a power of 2!!
 */
#define IBM2692_TX_RING_ENTRIES		128
#define IBM2692_TX_RING_INDEX_MASK	(IBM2692_TX_RING_ENTRIES - 1)

#define IBM2692_RX_RING_ENTRIES		128
#define IBM2692_RX_RING_INDEX_MASK	(IBM2692_RX_RING_ENTRIES - 1)

/*
 * Define the minibuffer size used by IBM2692
 */
#define IBM2692_PARTICLE_SIZE 512

#endif /* RVIP */

/*
 * Define the MEMD polling limits.
 */
#define IBM2692_TXPOLL_HIGH		(IBM2692_TX_RING_ENTRIES/2)
#define IBM2692_TXPOLL_LOW		(IBM2692_TX_RING_ENTRIES/2 - 8)

/*
 * Define address conversion macro's
 */ 
#define IBM2692_BUF_ADDR(x)	 ((ulong)(x) | 0x80000000)
#define CPU_VIEW_PCI_BFR_ADDR(x) ((ulong)(x) & 0x7fffffff)
#define PCI_VIEW_ADDR(x)	 ((ulong)(x))
#define CPU_VIEW_ADDR(x)	 ((ulong)(x))

/*
 * Define to control generation of keepalives
 */
#define IBM2692_LOCAL_KEEPALIVE      	FALSE

/*
 * Define to control bridging
 */
#define IBM2692_LOCAL_BRIDGE            FALSE

/*
 * Define IO mapped IO for VIP
 */
#define HERMON_MEMMAPPED_IO             TRUE

/*
 * Have the driver do packet accounting
 */
#define IBM2692_PKT_STATS		FALSE

/*
 * Never throttle on a VIP card
 */
#define IBM2692_THROTTLING              FALSE


/* 
 * Due to hermon bug, it can not be configured to have base address 
 * between 0x00 - 0x3ff
 */
#define PCI_IBM2692_DEVICE_BASE_OFFSET	0x400

/* 
 * options for CCB_CMD_VIP_4R_TEST 
 */
#define IO_TEST				0
#define RESET_HERMON			3
#define UNRESET_HERMON			4

/**********************************************************************
 * IBM2692 Static inlines
 **********************************************************************/

#else /* IBM2692_STATIC_INLINES_ONLY */

/*
 * ibm2692_particle_enqueue_inline 
 */
static inline void ibm2692_particle_enqueue_inline (paktype *pak,
						   particletype *particle)
{
    particle_enqueue_inline(pak, particle);
}

/*
 * ibmtr_transmit_inline 
 */
static inline void ibm2692_transmit_inline (hwidbtype *idb,
					    paktype *pak, boolean release)
{
    int i, particle_count;
    tx_bfr_desc_t *tx_desc;
    tx_desc_shadow_t *tx_shadow_desc;
    ibm2692_instance_t *ds = INSTANCE(idb);
    hermon_reg_map *hermon_ptr;
    particletype *mp;

    hermon_ptr = ds->ibm2692_ptr;
    /*
     * if we have room on the transmit descriptor list
     */
    if (ds->tx_outstanding < IBM2692_TX_RING_ENTRIES) {
	tx_shadow_desc = &(ds->tx_desc_shadow[ds->tx_inptr]);

	/*
	 * If we're tbridging, we need to keep track of the source
	 * addrs so that when we recieve a broadcast we can know if it
	 * was transmitted by us.  
	 */
	if (idb->tr_ring_mode & RINGMODE_TBRIDGE) {
	    ieee_copy(((tring_hdr*)pak->datagramstart)->saddr, 
		      &tx_shadow_desc->tb_saddr[0]);
	}

	/*
	 * check for contigous packet, if so particle count == 0
	 */
	particle_count = pak_particle_count(pak);
	if (!particle_count) {
	    tx_desc = tx_shadow_desc->cpu_view_addr;
	    tx_shadow_desc->tx_pak = pak;
	    tx_desc->tx_status = 0x0;
	    /*
	     * Hermon tx bug, need to append one zero length buffer
	     * at the end of packet
	     */
	    tx_desc->bfr_count = 2;
	    tx_desc->frame_length = pak->datagramsize;
	    tx_desc->bfr[0].bptr = (uchar *)IBM2692_BUF_ADDR(pak->datagramstart);
	    tx_desc->bfr[0].b_count = pak->datagramsize;
	    /*
	     * use init block as this dummy buffer
	     */
	    tx_desc->bfr[1].bptr = (uchar *)IBM2692_BUF_ADDR(ds->mpc_init_bptr);
	    tx_desc->bfr[1].b_count = 0;
	    ibm2692_queue_tx_desc(idb);
	    return;
	} else {
	    tx_desc = tx_shadow_desc->cpu_view_addr;
	    tx_shadow_desc->tx_pak = NULL;
	    tx_desc->tx_status = 0x0;
	    /*
	     *  Hermon tx bug, need to append one zero length buffer
	     * at the end of packet
	     */
	    tx_desc->bfr_count = particle_count + 1;
	    tx_desc->frame_length = pak->datagramsize;
	    for (i = 0; i < particle_count; i++) {
	        mp = dequeue_inline(&pak->particlequeue);
	        tx_desc->bfr[i].bptr = (uchar *)IBM2692_BUF_ADDR(mp->data_start);
	        tx_shadow_desc->particle[i] = mp;
	        tx_desc->bfr[i].b_count = mp->data_bytes;
	    }
	    tx_desc->bfr[i].b_count = 0;
	    tx_desc->bfr[i].bptr = (uchar *)IBM2692_BUF_ADDR(ds->mpc_init_bptr);
	    ibm2692_queue_tx_desc(idb);
	    if (release) {
		datagram_done(pak);
	    }
	    return;
	}
    } else {
	ibm2692_output_enqueue(pak, idb);
    }
}

/*
 * ibm2692_dfs_enabled
 */

static inline boolean ibm2692_dfs_enabled (hwidbtype *idb)
{
    return(idb->ip_routecache & IP_DISTRIBUTED_ROUTECACHE);
}

/*
 * ibm2692_ip_dfs
 */

static inline boolean ibm2692_ip_dfs (hwidbtype *idb, paktype *pak)
{
    pas_cbtype *pascb = INSTANCE(idb);

    return((*pascb->pak_dfs)(idb, pak));
}
#endif /* IBM2692_STATIC_INLINES_ONLY */

/* end of file */
