/* $Id: if_vip_mif68840.h,v 3.1.60.13 1996/08/26 15:11:13 ppearce Exp $
 * $Source: /release/112/cvs/Xsys/pas/if_vip_mif68840.h,v $
 *------------------------------------------------------------------
 * if_vip_mif68840.h - 1F MC68840 based PCI port adaptor support
 *                  (VIP specific components)
 *
 * August 1995, Farhad Sunavala
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: if_vip_mif68840.h,v $
 * Revision 3.1.60.13  1996/08/26  15:11:13  ppearce
 * CSCdi66582:  Implement SRB vector table to cut size of SRB_CORE subsys
 * Branch: California_branch
 *
 * Revision 3.1.60.12  1996/07/17  01:45:08  fsunaval
 * CSCdi59824:  RIF classification done on the VIP for the 1F.
 * Branch: California_branch
 * 1.Off-load RIF classification from the RSP to the VIP for the 1F.
 *   What's next, fragmentation?
 *
 * Revision 3.1.60.11  1996/06/10  22:21:47  mbeesley
 * CSCdi59552:  software forced reload due to memory corruption in
 * 2FPA-2FPA test. Use 32 byte burst, Memory Read Line on Predator. Also
 * remove debugging buginfs.
 * Branch: California_branch
 *
 * Revision 3.1.60.10  1996/06/06  12:04:00  dlobete
 * CSCdi57944:  DFS confused about rx ignores and tx drops
 * Branch: California_branch
 *
 * Revision 3.1.60.9  1996/05/31  01:26:23  fsunaval
 * CSCdi57339:  Implement throttling on the 1F.
 * Branch: California_branch
 * 1.Throttle the interface when no more particles available.  The
 *   interface is unthrottled when the cache has a certain threshold of
 *   particles in it.
 *
 * Revision 3.1.60.8  1996/05/28  07:06:53  fsunaval
 * CSCdi58107:  Long stress run in DFS gets malloc error in FDDI PA.
 * Branch: California_branch
 * 1.If RX particle has error, enqueue it before calling cleanup routine.
 *
 * Revision 3.1.60.7  1996/05/19  03:18:48  fsunaval
 * CSCdi55239:  FDDI driver needs to correctly mimic AF status bits
 * settings
 * Branch: California_branch
 *
 * Revision 3.1.60.6  1996/04/30  07:01:11  fsunaval
 * CSCdi51250:  SW-VIP1: Host MIB updating needs to be completed
 * Branch: California_branch
 * 1.Code changes for formation and transfer of SMT Messages to RSP
 *   in form of love letters.
 *
 * Revision 3.1.60.5  1996/04/12  17:43:19  kao
 * CSCdi53527:  Need EOIR support on VIP2
 * Branch: California_branch
 *
 * Revision 3.1.60.4  1996/04/11  04:39:45  fsunaval
 * CSCdi53600:  SW-SVIP: SVIP Fddi optimizations.
 * Branch: California_branch
 * 1.Take advantage of posted read hardware.
 * 2.Take advantage of Management interrupts.
 *
 * Revision 3.1.60.3  1996/03/28  03:33:26  fsunaval
 * CSCdi50796:  SW-VIP1: Transparent Bridge not taking fast-switching
 * Branch: California_branch
 *
 * Revision 3.1.60.2  1996/03/28  02:49:21  fsunaval
 * CSCdi51945:  SW-VIP1: fddi pa not showing neigbor mac-address.
 * Branch: California_branch
 *
 * Revision 3.1.4.10  1996/03/22  20:50:19  fsunaval
 * CSCdi51945:  SW-VIP1: fddi pa not showing neigbor mac-address.
 * Branch: ELC_branch
 *
 * Revision 3.1.4.9  1996/03/10  22:53:29  fsunaval
 * CSCdi50796:  Transparent Bridge not taking fast-switching.
 * Branch: ELC_branch
 * 1.FCI_STATION_ADDRESS was being set for all frames in promiscuous mode.
 *
 * Revision 3.1.4.8  1996/03/06  20:50:31  fsunaval
 * CSCdi50792:  Distributing switch not working for fddi-pa.
 * Branch: ELC_branch
 *
 * Revision 3.1.4.7  1996/02/26  18:43:43  fsunaval
 * CSCdi49792:  Provide bridging support for fddi on Predator
 * Branch: ELC_branch
 *
 * Revision 3.1.4.6  1996/02/26  04:39:23  fsunaval
 * CSCdi49791:  Incorporate code-review comments for Fddi port adapter
 * Branch: ELC_branch
 *
 * Revision 3.1.4.5  1996/02/17  05:54:34  fsunaval
 * CSCdi49008:  Fddi output counters are not reported and other fixes.
 * Branch: ELC_branch
 * 1.Output counts not increasing.
 * 2.Make FDDI on VIP memory-mapped.
 * 3.Distributed switching support.
 * 4.Reduce burst-size from 64 to 32.
 *
 * Revision 3.1.4.4  1996/02/05  07:00:14  fsunaval
 * Branch: ELC_branch
 * 1.Particle based fast-switching Fddi support and code cleanup.
 *
 * Revision 3.1.4.3  1996/01/25  12:10:10  fsunaval
 * Branch: ELC_branch
 * 1.Predator FDDI support.
 * 2.Patch CSCdi46358 (PA management interrupt) changes into ELC_branch.
 *
 * Revision 3.1.4.2  1996/01/16  01:43:48  dcarroll
 * Branch: ELC_branch
 * Sync ELC_branch between ELC_baseline_120995 and ELC_baseline_960110.
 *
 * Revision 3.1.4.1  1995/12/23  04:41:29  fsunaval
 * Branch: ELC_branch
 * 1.First pass at FDDI port adapter.
 *
 * Revision 3.1  1995/11/09  12:58:08  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/08/24  19:46:29  fsunaval
 * Placeholder files for new development.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __IF_VIP_MIF68840_H__
#define __IF_VIP_MIF68840_H__

#define MAX_LOVE_LETTERS    10
#define MSGS_PER_LOVELETTER 16
#define PROC_SLEEP_TIME     1000

/* 
 * Include VIP header files
 */
#include "../src-vip/vip_memd.h"
#include "../src-vip/vip_af.h"

/* Required for RIF support */
#include "../if/rif.h"
#include "../if/rif_inline.h"
#include "../srt/fddisrb.h"
#include "../if/fddi_inline.h"
#include "../hes/if_fci.h"  
#include "../srt/srb_sb_private.h"

/*
 * Required for fs_rif_match 
 */
#include "../if/tring.h"
#include "../srt/trsrb.h"
#include "../tbridge/tbridge_sde.h" /* SDE_ENCAPWORDS */
#include "../if/tring_inline.h"

/* externs */
extern ushort  ReadLineStateMulti(uInt16 stnID, uInt16 portID);
extern void    fddi_smt_debug(paktype *pak, boolean output);
extern boolean vip_send_loveletter(pas_cbtype *, uchar *, ushort);


/*
 * Define IO mapped IO for VIP
 */
#define MIF68840_MEMMAPPED_IO      TRUE 

/* 
 * Do not do any packet accounting on the VIP (host does it)
 */
#define MIF68840_PKT_STATS         FALSE

/*
 * Never throttle on a VIP card
 */
#define MIF68840_THROTTLING         FALSE

/*
 * Never throttle on a VIP card
 */
#define MIF68840_THROTTLING         FALSE

/*
 * Do we need to check for bridging on idb ?
 */
#define MIF68840_BRIDGING_CHECK    FALSE     

#if defined (SVIP)

/*
 * Define the MEMD polling limit. On Viper, we don't want 1F to ever
 * be limited IE poll as fast as possible.
 */
#define MIF68840_TXPOLL_HIGH      (MIF68840_TX_RING_ENTRIES * 4)
#define MIF68840_TXPOLL_LOW      (1)
 
#else /* RVIP */
 
/*
 * Define the MEMD polling limit. On Checkers stop polling when the
 * tx ring is half full.
 */
#define MIF68840_TXPOLL_HIGH      (MIF68840_TX_RING_ENTRIES / 2)
#define MIF68840_TXPOLL_LOW       (MIF68840_TX_RING_ENTRIES / 2 - 8)

#endif

/*
 * EEPROM Control, PCI bus command codes: Memory Read Multiple
 */
#define MIF68840_PLX_EEPROM_CONTROL 0x00017c00

/**********************************************************************
 * MIF68840 Static inlines
 **********************************************************************/

/*
 * mif68840_get_particle_inline:
 * Get a receive particle.
 */
static inline particletype *mif68840_get_particle_inline (hwidbtype *idb, 
                               boolean cache_ok)
{
    particletype *mp;

    /*
     * Try the cache
     */
    mp = pool_dequeue_cache(idb->pool);

    if (mp) {
        /*
         * Initialize start
         */
        mp->data_start = (uchar *)mp->data_block + 
            (PARTICLE_ENCAPBYTES - 16);
        return(mp);
    }


    /*
     * We did not get one from the cache, try a global pool
     */
    mp = pool_getparticle(idb->pool->fallback);

    /*
     * If we still do not have one, mark it as a nobuffer.
     */
    if (mp) {
        /*
         * Initialize start. 
         */
        DAS_DEBUG(0xabababab);
        mp->data_start = (uchar *)mp->data_block + 
            (PARTICLE_ENCAPBYTES - 16);
        return(mp);
    } else {
        idb->counters.output_nobuffers++;
        return(NULL);
    }
}

/*
 * mif68840_dfs_enabled:
 * Return TRUE if we are willing to DFS on this interface.
 */
static inline boolean mif68840_dfs_enabled (hwidbtype *idb)
{
    return(idb->ip_routecache & IP_DISTRIBUTED_ROUTECACHE);
}

/*
 * mif68840_ip_dfs:
 * Distributely fastswitch an IP datagram
 */
static inline boolean mif68840_ip_dfs (hwidbtype *idb, paktype *pak)
{
    pas_cbtype *pascb = INSTANCE(idb);

    return((*pascb->pak_dfs)(idb, pak));
}

static inline ulong mif68840_process_srb_inline (fddi_hdr *fddi, 
                        hwidbtype *idb, mif68840_instance *ds)
{
    fddirif_t *fddirif = (fddirif_t *)fddi;
    uint  riflen;
    ulong srb_status = 0;

    srb_status |= FCI_RXSRB_PRESENT;

    if (fddirif->rc_dlf & RC_D)
        srb_status |= FCI_RXSRB_DIR;
          
    if (is_fddi_explorer(fddirif))
        srb_status |= FCI_RXSRB_EXPLORER;

    riflen = fddi_riflen(fddirif);
        srb_status |= (riflen>>1);  /* note: fip doesn't do this field */

    if (ds->srbridge_on_idb) {
 
        uchar *rif_start, *rif_match;
        srb_sb_t *srb_sb;
 
        rif_start = &fddirif->rc_blth;

        srb_sb = idb_get_swsb_inline(idb->firstsw, SWIDB_SB_SRB);
        rif_match = fs_rif_match((srbroute_t *)rif_start, riflen,
                        srb_sb->rng_br_rng_f, srb_sb->rng_br_rng_r);

        if (rif_match) {
            srb_status |= FCI_RXSRB_MATCHED;

            /* rif offset is in words */
            srb_status |=
                ((rif_match - rif_start)<<7) & FCI_RXSRB_RDOFFSET;
        }
    }
    srb_status <<= 16;
    return(srb_status);
}

/*
 * mif68840_process_receive_llc_packet:
 * We have a good, completed Rx LLC packet. Do something with it.
 */
static inline void mif68840_process_receive_llc_packet (hwidbtype *idb,
                        mif68840_instance *ds,
                        register paktype *rx_pak,
                        int pak_size,
                        ulong rmd_status_length)
{
    fddi_hdr *fddi, *fddi_uncached;
    ulong    rx_mask;

    /* Null rif status */
    rx_pak->rif_status = 0;
  
    /*
     * Account for the 3 bytes of padding that the hw inserts.
     * Put datagramstart to the start of the packet and reduce
     * datagramsize by 3
     */
    rx_pak->datagramstart += 3;
    rx_pak->datagramsize -= 3;
    particle_clip_inline(rx_pak->particlequeue.qhead, -3);
    
    /* Stash the uncached address  */
    fddi_uncached = (fddi_hdr *) rx_pak->datagramstart;
  
    /*
     * Convert to cache view
     */
    CONVERT_PARTICLE_ADDR(rx_pak->datagramstart);
    fddi = (fddi_hdr *) rx_pak->datagramstart;
       
    /*
     * Invalidate 1st line. The second line will have to be invalidated
     * if packet is IP and DFS flow is enabled.
     * TBD. Do we want it cached ??
     */
    vip_cache_invalidate_line(&fddi->data[0]);
 
    /*
     * Is DA = MLA ??
     */
    rx_mask = rmd_status_length & MIF68840_RX_DA_MASK;
    if ((rx_mask == MIF68840_RX_LOCAL_MATCH)  || 
        (rx_mask == MIF68840_RX_CAM_MATCH)) {
 
        /* Set Unicast bit */
        rx_pak->mci_status = AF_STATION_ADDRESS;
 
        /*
         * If (RIF not present && IP && IP_hdl == 0x45 && TTL > 1)
         *     set IP bit;
         */
        if ((GETSHORT(&fddi->data[0]) == 0xaaaa) &&
               (GETSHORT(&fddi->data[6]) == 0x0800) &&
               (fddi->data[8] == 0x45) && (fddi->data[16] > 0x1)) {
 
            /* Set IP bit */
            rx_pak->mci_status |= AF_DOD_IP_PROTOCOL;
 
            /*
             * Check if DFS support is enabled
             */
            if (mif68840_dfs_enabled(idb)) {
 
                /*
                 * If the DFS routine returns TRUE, we are done with
                 * this packet.  If it returns FALSE, we increment the
                 * input drop count and exit from the interrupt.
                 */
                rx_pak->network_start = rx_pak->datagramstart +
                                    FDDI_SNAP_ENCAPBYTES;
                /*
                 * Invalidate one more line of cache for
                 * Darren's flow-switching access list crap
                 * TBD. Do we want it cached ??
                 */
                vip_cache_invalidate_line(&fddi->data[0] + 32);
 
                if (PAS_DEBUG_BOOL && fddi_debug) {
                buginf("\nLLC_dfs: PCI=0x%08x, rx_dsc=0x%08x, mci_sts=0x%08x",
                rx_pak->datagramstart, rmd_status_length, rx_pak->mci_status);
                }
 
                if (!mif68840_ip_dfs(idb, rx_pak)) {
                    mif68840_cleanup_pak(rx_pak);
                }
                mif68840_flush_pak(rx_pak);
                return;
            } /* if (mif68840_dfs_enabled(idb)) */
 
        } /* If (IP bit is set) */ else {
 
            /*
             * DA = MA and packet is not IP.
             * Right now, only check for RSRB stuff
             * Since the SA is uncached, use the uncached address
             */
 
            if (is_fddi_srb(fddi_uncached, FDDIT_RII)) {
 
                /* Call the SRB processing inline */
                rx_pak->rif_status = mif68840_process_srb_inline(fddi,idb,ds);
            }
        }
 
    } else {
 
        /* DA is not equal to MLA */
 
        /*
         * Check whether broadcast/multicast or unicast and set
         * multicast bit accordingly.
         * Since the DA is uncached, use the uncached address.
         */
        if (fddi_uncached->daddr[0] & 0x1)
            rx_pak->mci_status = AF_MULTICAST_ADDRESS;
        else
            rx_pak->mci_status = 0;
 
        /*
         * Check for SRB stuff.
         * Since the SA is uncached, use the uncached address
         */
        if (is_fddi_srb(fddi_uncached, FDDIT_RII)) {
 
            /* Call the SRB processing inline */
            rx_pak->rif_status = mif68840_process_srb_inline(fddi,idb,ds);
        }
    }
 
    /*
     * If bridging enabled, set bridging bit
     */
     rx_pak->mci_status |= ds->bridge;
 
    if (PAS_DEBUG_BOOL && fddi_debug) {
        buginf("\nLLC: PCI=0x%08x, rx_dsc=0x%08x, mci_sts=0x%08x, rif_status=0x%08x",
            fddi, rmd_status_length, rx_pak->mci_status, rx_pak->rif_status);
    }
 
    /*
     * Either DFS is turned off or the packet was not a supported
     * protocol. Just punt the frame to the host
     */
    if (!(*ds->pacb.pak_to_host)(idb, &ds->pacb, rx_pak)) {
        idb->counters.input_drops += 1;
        ds->llc_bh_drops++;
        mif68840_cleanup_pak(rx_pak);
    }
    mif68840_flush_pak(rx_pak);
}

/*
 * mif68840_process_receive_smt_packet:
 * We have a good, completed Rx SMT packet. Do something with it.
 */
static inline void mif68840_process_receive_smt_packet (hwidbtype *idb,
                        mif68840_instance *ds,
                        paktype *rx_pak,
                        int pak_size,
                        ulong rmd_status_length)
{
    register ushort rxtype;
 
    /*
     * Account for the 3 bytes of padding that the hw inserts.
     * Put datagramstart to the start of the packet and reduce
     * datagramsize by 3
     */
    rx_pak->datagramstart += 3;
    rx_pak->datagramsize -= 3;
       
    particle_clip_inline(rx_pak->particlequeue.qhead, -3);
 
    /*
     * Process the SMT packet.  We can save some time here.
     * Since the XDI code checks the destination address,
     * I skip it here.
     */
      
    rxtype = (rmd_status_length & MIF68840_RX_EAC_MASK) >> 13;
    rx_pak->mci_status = rxtype | MIF68840_RX_SMT_FRAME_STATUS;
 
    if (PAS_DEBUG_BOOL && fddi_smt_packet_debug) {
        buginf("\nSMT: PCI=0x%08x, rx_desc=0x%08x, mci_status=0x%08x",
          rx_pak->datagramstart, rmd_status_length, rx_pak->mci_status);
    }
 
    /*
     * Since the packet was not a supported protocol, just punt the
     * frame to the host
     */
    if (!(*ds->pacb.pak_to_host)(idb, &ds->pacb, rx_pak)) {
        idb->counters.input_drops += 1;
        ds->smt_bh_drops++;
        mif68840_cleanup_pak(rx_pak);
    }
    mif68840_flush_pak(rx_pak);

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
    } else {
        newpak = pak;
    }
    newpak->if_output = idb->firstsw;
    /*
     * Invalidate accounting protocol type to prevent
     * packet accounting in holdq_enqueue_fast() if accounting
     * flag is not set.
     */
    if (!MIF68840_PKT_STATS)
        newpak->acct_proto = ACCT_PROTO_LAST;
    holdq_enqueue_fast(idb, newpak);
}


/* end of file */
#endif __IF_VIP_MIF68840_H__
