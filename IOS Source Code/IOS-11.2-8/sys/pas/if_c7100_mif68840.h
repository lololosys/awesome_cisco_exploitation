/* $Id: if_c7100_mif68840.h,v 3.1.36.10 1996/09/13 20:18:34 mbeesley Exp $
 * $Source: /release/112/cvs/Xsys/pas/if_c7100_mif68840.h,v $
 *------------------------------------------------------------------
 * if_c7100_mif68840.h - 1F MIF68840 based PCI port adapter support 
 *                   (Predator specific components)

 * Jan 1996, Farad Sunavala
 *
 * Copyright (c) 1996-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: if_c7100_mif68840.h,v $
 * Revision 3.1.36.10  1996/09/13  20:18:34  mbeesley
 * CSCdi69010:  c7200 sometimes forwards an IP packet with a ttl of 1
 * Branch: California_branch
 *
 * Revision 3.1.36.9  1996/08/28  07:45:57  abaindur
 * CSCdi67069:  Data cache use needs to be turned ON for c7200 IP flow
 * switching
 * Branch: California_branch
 *  Fix mixup of cached and uncached region pointers. Fix cases where
 *  converted addresses were not restored if flow switching failed.
 *
 * Revision 3.1.36.8  1996/08/24  01:50:54  abaindur
 * CSCdi66979:  Traffic shaping broken on the c7200
 * Branch: California_branch
 *
 * Revision 3.1.36.7  1996/06/10  22:21:40  mbeesley
 * CSCdi59552:  software forced reload due to memory corruption in
 * 2FPA-2FPA test. Use 32 byte burst, Memory Read Line on Predator. Also
 * remove debugging buginfs.
 * Branch: California_branch
 *
 * Revision 3.1.36.6  1996/05/31  01:26:16  fsunaval
 * CSCdi57339:  Implement throttling on the 1F.
 * Branch: California_branch
 * 1.Throttle the interface when no more particles available.  The
 *   interface is unthrottled when the cache has a certain threshold of
 *   particles in it.
 *
 * Revision 3.1.36.5  1996/05/24  00:49:37  mbeesley
 * CSCdi58316:  Need to fully support CPQ on new interfaces
 * Branch: California_branch
 *
 * Revision 3.1.36.4  1996/05/07  00:38:52  mbeesley
 * CSCdi56232:  Enable IP Flow/Optimum on fddi
 * Branch: California_branch
 *
 * Revision 3.1.36.3  1996/04/24  02:13:13  fsunaval
 * CSCdi55257:  Need to speed up fddi process level
 * Branch: California_branch
 * 1.Use the DMA coalescer.
 *
 * Revision 3.1.36.2  1996/04/11  04:42:47  fsunaval
 * CSCdi53237:  alignment error 32 , Memory allocation failed from pool PCI
 * Branch: California_branch
 *
 * Revision 3.1.36.1  1996/03/21  23:22:53  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.1.2.5  1996/02/26  18:43:44  fsunaval
 * CSCdi49792:  Provide bridging support for fddi on Predator
 * Branch: ELC_branch
 *
 * Revision 3.1.2.4  1996/02/26  04:39:18  fsunaval
 * CSCdi49791:  Incorporate code-review comments for Fddi port adapter
 * Branch: ELC_branch
 *
 * Revision 3.1.2.3  1996/02/17  05:54:28  fsunaval
 * CSCdi49008:  Fddi output counters are not reported and other fixes.
 * Branch: ELC_branch
 * 1.Output counts not increasing.
 * 2.Make FDDI on VIP memory-mapped.
 * 3.Distributed switching support.
 * 4.Reduce burst-size from 64 to 32.
 *
 * Revision 3.1.2.2  1996/02/05  07:00:08  fsunaval
 * Branch: ELC_branch
 * 1.Particle based fast-switching Fddi support and code cleanup.
 *
 * Revision 3.1.2.1  1996/01/25  12:10:03  fsunaval
 * Branch: ELC_branch
 * 1.Predator FDDI support.
 * 2.Patch CSCdi46358 (PA management interrupt) changes into ELC_branch.
 *
 * Revision 3.1  1996/01/25  07:37:16  fsunaval
 * 1.Placeholder files for new development.
 *
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __IF_C7100_MIF68840_H__
#define __IF_C7100_MIF68840_H__

/*
 * Define Memory mapped IO for Predator
 */
#define MIF68840_MEMMAPPED_IO       TRUE

/* 
 * Have the driver do packet accounting
 */
#define MIF68840_PKT_STATS          TRUE 

/*
 * Throttle the interface if the Predator runs out of buffers
 */
#define MIF68840_THROTTLING         TRUE 
#define MIF68840_BUFFER_THRESHOLD   14

/*
 * Do we need to check for bridging on idb ?
 */
#define MIF68840_BRIDGING_CHECK     TRUE

/*
 * EEPROM Control, PCI bus command codes: Memory Read Line
 */
#define MIF68840_PLX_EEPROM_CONTROL 0x00017e00

/* 
 * Required for RIF support 
 */
#include "../if/rif.h"
#include "../if/rif_inline.h"
#include "../srt/fddisrb.h"

#include "../if/fddi_inline.h"

/*
 * Include flow/optimum switching header files
 */
#include "../ipmulticast/igmp.h"
#include "../ip/ipfast_flow.h"

/*
 * Prototypes
 */
extern hwidbtype *mif68840_idb_create(uint slot, uint port);
extern void mif68840_post_coalesce_rx_llc(hwidbtype *,paktype *,iqueue_t iq);
extern void mif68840_post_coalesce_rx_smt(hwidbtype *idb, paktype *rx_pak);
extern void mif68840_post_coalesce_fs(hwidbtype *, paktype *, iqueue_t iq);
extern void fddi_smt_debug(paktype *pak, boolean output);

/**********************************************************************
 * MIF68840 Static inlines
 **********************************************************************/

/* 
 * mif68840_get_particle_inline:
 * Get an rx particle.
 */
static inline particletype *mif68840_get_particle_inline (hwidbtype *idb,
                              boolean cache_ok)
{
    particletype *mp = NULL;
  
    /*
     * Try the buffer cache. 
     */
    mp = pool_dequeue_cache(idb->pool);
 
    /*
     * If we do not have one, mark it as a nobuffer.
     */
    if (mp) {

        /*
         * Initialize start
         */
        mp->data_start = mp->data_block->data_area + (PARTICLE_ENCAPBYTES - 4);
        return(mp);
    } else {
        idb->counters.output_nobuffers++;
        return(NULL);
    }
}

/*
 * mif68840_process_receive_llc_packet:
 * We have a good, completed Rx LLC packet. Do something with it.
 */
static inline void mif68840_process_receive_llc_packet (hwidbtype *idb,
                        register mif68840_instance *ds,
                        register paktype *rx_pak,
                        int pak_size,
                        register ulong rmd_status_length)
{
    fddi_hdr        *fddi;

    /*
     * Make sure the pak flags field is clean
     */
    rx_pak->flags = 0;

    /* 
     * Account for the 3 bytes of padding that the hw inserts.
     * Put datagramstart to the start of the packet and reduce 
     * datagramsize by 3
     */
    rx_pak->datagramstart += 3;
    rx_pak->datagramsize -= 3;

    particle_clip_inline(rx_pak->particlequeue.qhead, -3);

    /*
     * If bridging is turned off and flow switching is enabled
     * then try and optimum/flow switch this packet if it is IP
     */
    if ((idb->ip_routecache & (IP_FLOW_ROUTECACHE|IP_OPTIMUM_ROUTECACHE)) && 
	!idb->tbridge_on_hwidb) {

	uchar 	  *start, *old_start;
	iphdrtype *ip;
	snap_hdr  *snap;
	ushort    lsap, riflen;

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
	fddi = (fddi_hdr *)start;
	riflen = get_fddi_riflen(fddi, 0x1);
	snap = (snap_hdr *)(&fddi->data[riflen]);
	lsap = GETSHORT(&snap->dsap);
	if (riflen == 0 &&
	    (lsap == (SAP_SNAP | SAP_SNAP << 8)) &&
	    (snap->type == TYPE_IP10MB)) {

	    ip = (iphdrtype *)(snap->data);
	    rx_pak->datagramstart = start;
	    if ((*idb->ip_turbo_fs)(rx_pak, ip, idb) == IPFLOWOK) {
		mif68840_flush_pak(rx_pak);
		return;
	    }
	    rx_pak->datagramstart = old_start;
	}
    }

    fddi = (fddi_hdr *)rx_pak->datagramstart;

    /*
     * Decode FDDI encapsulation
     */
    rx_pak->rxtype = 
        fddi_decode_encapsulation_inline(rx_pak, fddi, 0x1);
    
    /* 
     * Set rxrifwordlen so that we don't do it again in
     * fddi_parse_packet 
     */
    idb->rxrifwordlen = rx_pak->riflen / 2;

    if (PAS_DEBUG_BOOL && fddi_debug) {
        ds->llc_inputs++;
        buginf("\n%s: (LLC %d) dstart=0x%08x, encap=%d, sts=0x%08x", 
            idb->hw_namestring, ds->llc_inputs, rx_pak->datagramstart,
            rx_pak->rxtype, rmd_status_length);
    }

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
    if ((idb->tbridge_on_hwidb) &&  reg_invoke_tbridge_forward(rx_pak)) {
        mif68840_flush_pak (rx_pak);
        return;
    }

    /* 
     * If the packet is for a protocol that is not presently fastswitchable 
     * as a particled packet, coalesce it before doing further processing. 
     * If it is a supported protocol, try and fastswitch it in particles. 
     */ 
    if (c7100_scattered_fs_protocol(rx_pak->rxtype)) { 
        if (reg_invoke_fddi_fs(rx_pak->rxtype, rx_pak)) { 
            mif68840_flush_pak(rx_pak); 
            return; 
        } 
    } else { 
        c7100_dma_pak_coalesce(idb, rx_pak, mif68840_post_coalesce_fs,
                    (uchar *) rx_pak->info_start);
        return; 
    } 

    /*
     * Coaleasce the packet and send it on
     */
    c7100_dma_pak_coalesce(idb, rx_pak, mif68840_post_coalesce_rx_llc,
                    (uchar *) rx_pak->info_start);
}

/*
 * mif68840_process_receive_smt_packet:
 * We have a good, completed Rx SMT packet. Do something with it.
 */
static inline void mif68840_process_receive_smt_packet (hwidbtype *idb,
                        mif68840_instance *ds,
                        register paktype *rx_pak,
                        int pak_size,
                        ulong rmd_status_length)
{
    MACHdrType *machdr;
    machdr = (MACHdrType *)(rx_pak->datagramstart);
	
	/* Make DA and SA in Fddi form for the XDI code */
    ieee_swap((uchar *)(machdr->DA), (uchar *)(machdr->DA));
    ieee_swap((uchar *)(machdr->SA), (uchar *)(machdr->SA));

    /* 
     * Account for the 3 bytes of padding that the hw inserts.
     * Put datagramstart to the start of the packet and reduce 
     * datagramsize by 3
     */

    rx_pak->datagramstart += 3;
    rx_pak->datagramsize  -= 3;

    particle_clip_inline(rx_pak->particlequeue.qhead, -3);

    if (fddi_smt_packet_debug) {
        ds->smt_inputs++;
        buginf("\n%s: (SMT %d) dstart=0x%08x, sts=0x%08x", 
            idb->hw_namestring, ds->smt_inputs, rx_pak->datagramstart,
            rmd_status_length);
    }

    rx_pak->mci_status = (rmd_status_length & MIF68840_RX_EAC_MASK) >> 13;
    rx_pak->if_input = idb->firstsw;
    rx_pak->network_start = rx_pak->datagramstart + FDDI_SNAP_ENCAPBYTES;

    /*
     * Coaleasce the packet and send it on
     */
    c7100_pak_coalesce(idb, rx_pak, mif68840_post_coalesce_rx_smt);
}

/*
 * mif68840_output_enqueue:
 * Platform specific routine to enqueue a packet onto
 * the output holdq. On checkers, if the packet is scattered we reparent
 * and enqueue it as particles. If it is not scattered, it is queued directly.
 */
static inline void mif68840_output_enqueue (paktype *pak, hwidbtype *idb)
{
    paktype *newpak;
    
    if (pak_has_particles(pak)) {

        /*
         * Reparent onto a new paktype header as we can not hold onto
         * the one passed into us.
         */
        newpak = particle_reparent(pak, idb->headerpool);
        if (!newpak) {
            mif68840_cleanup_pak(pak);
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
     * Invalidate accounting protocol type to prevent
     * packet accounting in holdq_enqueue_fast() if the 
     * accounting flag is not set.
     */
    if (!MIF68840_PKT_STATS)
	newpak->acct_proto = ACCT_PROTO_LAST;
    holdq_enqueue_fast(idb, newpak);
}


/* end of file */
#endif __IF_VIP_MIF68840_H__
