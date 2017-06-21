/* $Id: srb_sb.c,v 3.1.2.8 1996/09/14 01:05:00 ppearce Exp $
 * $Source: /release/112/cvs/Xsys/srt/srb_sb.c,v $
 *------------------------------------------------------------------
 * srb_sb.c -- SRB subblock accessor routines
 *
 * July 1996, Paul Pearce
 *
 * Copyright (c) 1996 by Cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: srb_sb.c,v $
 * Revision 3.1.2.8  1996/09/14  01:05:00  ppearce
 * CSCdi68885:  SRB subblock not setup properly for VIP interfaces
 * Branch: California_branch
 *   Previous commit breaks build for c7200- images
 *     Backout previous changes
 *     Create SRB_SB subsystem to init SRB vector table
 *       rather than init in  VIP reset routines
 *
 * Revision 3.1.2.7  1996/08/26  15:13:26  ppearce
 * CSCdi66582:  Implement SRB vector table to cut size of SRB_CORE subsys
 * Branch: California_branch
 *
 * Revision 3.1.2.6  1996/08/17  21:44:10  ppearce
 * CSCdi66186:  75k crashes w/ALIGN-1-FATAL after multiple RSRB tests
 * Branch: California_branch
 *   - Check malloc return before initializing SRB subblock
 *   - Free SRB subblock when vRing unconfigured to prevent memory leak
 *
 * Revision 3.1.2.5  1996/08/12  02:16:23  ppearce
 * CSCdi64450:  CSCdi59527 broke c4500-boot-m image
 * Branch: California_branch
 *   Add srb_core_registry that will definitely be included w/SRB subblock
 *
 * Revision 3.1.2.4  1996/07/23  18:49:23  ppearce
 * CSCdi63747:  Move srb_multiring out of SRB subblock
 * Branch: California_branch
 *   (1) srb_multiring doesn't really belong exclusively to SRB
 *         move srb_multiring out of SRB subblock and into swidb
 *         provide srb_multiring accessor routines
 *   (2) determine_rif() should take swidb as argument rather than hwidb
 *
 * Revision 3.1.2.3  1996/07/21  04:34:17  ppearce
 * CSCdi63366:  Move SRB counters from hwidb into SRB subblock
 * Branch: California_branch
 *   srb_sb_set_si_counters causes build error in c100x images
 *     use SRB vector table registry instead of direct invocation
 *
 * Revision 3.1.2.2  1996/07/18  20:29:51  ppearce
 * CSCdi63366:  Move SRB counters from hwidb into SRB subblock
 * Branch: California_branch
 *   - Decreases size of hwidb for non-SRB'd interfaces
 *   - Provides additional IOS-independence for SRB
 *   - Moves SRB a little closer to being completely swidb-based
 *
 * Revision 3.1.2.1  1996/07/09  13:31:32  ppearce
 * CSCdi59527:  ascii string in inline wastes code space
 * Branch: California_branch
 *   Just say "no" to inline abuse - SRB subblock inlines removed
 *
 * Revision 3.1  1996/07/02  18:24:03  ppearce
 * Placeholder files
 *
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#include "master.h"
#include "interface_private.h"
#include "subsys.h"
#include "srb_core.h"
#include "srb_sb_private.h"


/*
 * srb_sb_vector_table__init()
 *   Initialize SRB subblock vector table accessor routines
 *
 */
static void srb_sb_vector_table_init (void) {

    srbV.srb_create_sb = &srb_create_sb;
    srbV.srb_destroy_sb = &srb_destroy_sb;

    srbV.srb_sb_get_max_hops = &srb_sb_get_max_hops;
    srbV.srb_sb_set_max_hops = &srb_sb_set_max_hops;

    srbV.srb_sb_get_maxin_hops = &srb_sb_get_maxin_hops;
    srbV.srb_sb_set_maxin_hops = &srb_sb_set_maxin_hops;

    srbV.srb_sb_get_maxout_hops = &srb_sb_get_maxout_hops;
    srbV.srb_sb_set_maxout_hops = &srb_sb_set_maxout_hops;

    srbV.srb_sb_get_actualring = &srb_sb_get_actualring;
    srbV.srb_sb_set_actualring = &srb_sb_set_actualring;

    srbV.srb_sb_get_thisring = &srb_sb_get_thisring;
    srbV.srb_sb_set_thisring = &srb_sb_set_thisring;

    srbV.srb_sb_get_bridge_num = &srb_sb_get_bridge_num;
    srbV.srb_sb_set_bridge_num = &srb_sb_set_bridge_num;
    srbV.srb_sb_get_bridging_enabled = &srb_sb_get_bridging_enabled;

    srbV.srb_sb_get_targetring = &srb_sb_get_targetring;
    srbV.srb_sb_set_targetring = &srb_sb_set_targetring;

    srbV.srb_sb_init_rng_br_rng_mask = &srb_sb_init_rng_br_rng_mask;

    srbV.srb_sb_get_rng_br_rng_f = &srb_sb_get_rng_br_rng_f;
    srbV.srb_sb_set_rng_br_rng_f = &srb_sb_set_rng_br_rng_f;

    srbV.srb_sb_get_rng_br_rng_r = &srb_sb_get_rng_br_rng_r;
    srbV.srb_sb_set_rng_br_rng_r = &srb_sb_set_rng_br_rng_r;

    srbV.srb_sb_get_proxy_explorer     = &srb_sb_get_proxy_explorer;
    srbV.srb_sb_enable_proxy_explorer  = &srb_sb_enable_proxy_explorer;
    srbV.srb_sb_disable_proxy_explorer = &srb_sb_disable_proxy_explorer;

    srbV.srb_sb_get_spanning_explorer     = &srb_sb_get_spanning_explorer;
    srbV.srb_sb_enable_spanning_explorer  = &srb_sb_enable_spanning_explorer;
    srbV.srb_sb_disable_spanning_explorer = &srb_sb_disable_spanning_explorer;

    srbV.srb_sb_get_oldsna_hack = &srb_sb_get_oldsna_hack;
    srbV.srb_sb_get_enabled_for_fddi = &srb_sb_get_enabled_for_fddi;

    srbV.srb_sb_get_rsrb_transit_oui = &srb_sb_get_rsrb_transit_oui;
    srbV.srb_sb_set_rsrb_transit_oui = &srb_sb_set_rsrb_transit_oui;

    srbV.srb_sb_get_triplet = &srb_sb_get_triplet;
    srbV.srb_sb_set_triplet = &srb_sb_set_triplet;

    srbV.srb_sb_incr_rx_counters    = &srb_sb_incr_rx_counters;
    srbV.srb_sb_incr_tx_nb_counters = &srb_sb_incr_tx_nb_counters;
    srbV.srb_sb_incr_tx_b_counters  = &srb_sb_incr_tx_b_counters;

    srbV.srb_sb_incr_span_in_counter  = &srb_sb_incr_span_in_counter;
    srbV.srb_sb_incr_span_out_counter = &srb_sb_incr_span_out_counter;

    srbV.srb_sb_incr_allrte_in_counter = &srb_sb_incr_allrte_in_counter;
    srbV.srb_sb_incr_allrte_out_counter = &srb_sb_incr_allrte_out_counter;

    srbV.srb_sb_set_si_counters = &srb_sb_set_si_counters;

    srbV.srb_sb_get_tx_drops = &srb_sb_get_tx_drops;
    srbV.srb_sb_incr_tx_drops = &srb_sb_incr_tx_drops;
    srbV.srb_sb_multi_incr_tx_drops = &srb_sb_multi_incr_tx_drops;
    srbV.srb_sb_incr_rx_drops = &srb_sb_incr_rx_drops;

}


/* SRB subblock management routines
 * --------------------------------
 *
 */


/* 
 * srb_create_sb()
 *   Create SRB subblock
 *
 */
srb_sb_t *srb_create_sb (idbtype *swidb)
{
    srb_sb_t  *sb;
    swidb_sb_t sbtype = SWIDB_SB_SRB;
 
    /*
     * Create subblock (if it doesn't already exist)
     *
     */
    sb = idb_get_swsb(swidb, sbtype);
    if (!sb) {
        sb = malloc_named(sizeof(srb_sb_t), "SWIDB_SB: SRB Info");
        if (!sb) {
          return(NULL);
        }
        idb_add_swsb(swidb, &sbtype, sb);
    }

    /*
     * Initialize subblock fields
     *
     *   Init all SRB subblock fields to zero with memset()
     *   Init all other SRB subblock fields that should not be zero
     *
     */

    memset(sb, 0, sizeof(srb_sb_t));

    sb->srb_max_hops    = SRB_DEF_MAXHOPS;
    sb->srb_maxin_hops  = SRB_DEF_MAXHOPS;
    sb->srb_maxout_hops = SRB_DEF_MAXHOPS;

    sb->srb_bridge_num = SRB_INVALID_BRIDGE_NUM;

    return(sb);

} /* End srb_create_sb() */


/* 
 * srb_destroy_sb()
 *   Delete SRB subblock - returns FALSE if not destroyed
 *
 */
boolean srb_destroy_sb (idbtype *swidb)
{
    srb_sb_t *sb;
    swidb_sb_t sbtype = SWIDB_SB_SRB;

    sb = srb_get_sb_inline(swidb); 
    if (!sb) {
        return(FALSE);
    }

    if (!idb_delete_swsb(swidb, sbtype)) {
        return(FALSE);
    }

    free(sb);
    return(TRUE);
}


/* SRB subblock accessor routines
 * ------------------------------
 *
 */


uchar srb_sb_get_max_hops (idbtype *swidb)
{
    srb_sb_t *sb = srb_get_sb_inline(swidb);
    if (sb)
      return(sb->srb_max_hops);
    else
      return(SRB_DEF_MAXHOPS);
}


void srb_sb_set_max_hops (idbtype *swidb, uchar max_hops)
{
    srb_sb_t *sb = srb_get_or_create_sb_inline(swidb);
    if (sb) {
      sb->srb_max_hops = max_hops;
    }
}


ushort srb_sb_get_maxin_hops (idbtype *swidb)
{
    srb_sb_t *sb = srb_get_sb_inline(swidb);
    if (sb)
      return(sb->srb_maxin_hops);
    else
      return(SRB_DEF_MAXHOPS);
}


void srb_sb_set_maxin_hops (idbtype *swidb, ushort maxin_hops)
{
    srb_sb_t *sb = srb_get_or_create_sb_inline(swidb);
    if (sb) {
      sb->srb_maxin_hops = maxin_hops;
    }
}


ushort srb_sb_get_maxout_hops (idbtype *swidb)
{
    srb_sb_t *sb = srb_get_sb_inline(swidb);
    if (sb)
      return(sb->srb_maxout_hops);
    else
      return(SRB_DEF_MAXHOPS);
}


void srb_sb_set_maxout_hops (idbtype *swidb, ushort maxout_hops)
{
    srb_sb_t *sb = srb_get_or_create_sb_inline(swidb);
    if (sb) {
      sb->srb_maxout_hops = maxout_hops;
    }
}


ushort srb_sb_get_actualring (idbtype *swidb)
{
    srb_sb_t *sb = srb_get_sb_inline(swidb);
    if (sb)
      return(sb->srb_actualring);
    else
      return(SRB_INVALID_RING_NUM);
}


void srb_sb_set_actualring (idbtype *swidb, ushort actualring)
{
    srb_sb_t *sb = srb_get_or_create_sb_inline(swidb);
    if (sb) {
      sb->srb_actualring = actualring;
    }
}


ushort srb_sb_get_thisring (idbtype *swidb)
{
    srb_sb_t *sb = srb_get_sb_inline(swidb);
    if (sb)
      return(sb->srb_thisring);
    else
      return(SRB_INVALID_RING_NUM);
}


void srb_sb_set_thisring (idbtype *swidb, ushort ringNum)
{
    srb_sb_t *sb = srb_get_or_create_sb_inline(swidb);
    if (sb) {
      sb->srb_thisring = ringNum;
    }
}


uchar srb_sb_get_bridge_num (idbtype *swidb)
{
    srb_sb_t *sb = srb_get_sb_inline(swidb);
    if (sb)
      return(sb->srb_bridge_num);
    else
      return(SRB_INVALID_BRIDGE_NUM);
}


void srb_sb_set_bridge_num (idbtype *swidb, uchar bridgeNum)
{
    srb_sb_t *sb = srb_get_or_create_sb_inline(swidb);
    if (sb) {
      sb->srb_bridge_num = bridgeNum;
    }
}


boolean srb_sb_get_bridging_enabled (idbtype *swidb)
{
    srb_sb_t *sb = srb_get_sb_inline(swidb);
    if (sb)
      return(sb->srb_bridge_num <= SRB_MAX_BRIDGE_NUM);
    else
      return(FALSE);
}


ushort srb_sb_get_targetring (idbtype *swidb)
{
    srb_sb_t *sb = srb_get_sb_inline(swidb);
    if (sb)
      return(sb->srb_targetring);
    else
      return(SRB_INVALID_RING_NUM);
}


void srb_sb_set_targetring (idbtype *swidb, ushort ringNum)
{
    srb_sb_t *sb = srb_get_or_create_sb_inline(swidb);
    if (sb) {
      sb->srb_targetring = ringNum;
    }
}


void srb_sb_init_rng_br_rng_mask (idbtype *swidb)
{
    srb_sb_t *sb;
    ushort localRn;
    uchar  bridgeNum;
    ushort targetRn;

    sb = srb_get_or_create_sb_inline(swidb);
    if (!sb) {
        return;
    }

    localRn   = sb->srb_thisring;
    bridgeNum = sb->srb_bridge_num;
    targetRn  = sb->srb_targetring;

    if ((localRn > 0) && (localRn <= SRB_MAX_RING_NUM)) {
        sb->rng_br_rng_f = ( (localRn   << 20) |
                             (bridgeNum << 16) |
                             (targetRn  << 4) );
        sb->rng_br_rng_r = ( (targetRn  << 20) |
                             (bridgeNum << 16) |
                             (localRn   << 4) );
    } else {
        sb->rng_br_rng_f = 0L;
        sb->rng_br_rng_r = 0L;
    }
}


ulong srb_sb_get_rng_br_rng_f(idbtype *swidb)
{
    srb_sb_t *sb = srb_get_sb_inline(swidb);
    if (sb)
      return(sb->rng_br_rng_f);
    else
      return(0L);
}

void srb_sb_set_rng_br_rng_f(idbtype *swidb, ulong rng_br_rng_f)
{
    srb_sb_t *sb;

    sb = srb_get_or_create_sb_inline(swidb);
    if (!sb) {
        return;
    }
    sb->rng_br_rng_f = rng_br_rng_f;
}


ulong srb_sb_get_rng_br_rng_r(idbtype *swidb)
{
    srb_sb_t *sb = srb_get_sb_inline(swidb);
    if (sb)
      return(sb->rng_br_rng_r);
    else
      return(0L);
}


void srb_sb_set_rng_br_rng_r(idbtype *swidb, ulong rng_br_rng_r)
{
    srb_sb_t *sb;

    sb = srb_get_or_create_sb_inline(swidb);
    if (!sb) {
        return;
    }
    sb->rng_br_rng_r = rng_br_rng_r;
}


boolean srb_sb_get_proxy_explorer (idbtype *swidb)
{
    srb_sb_t *sb = srb_get_sb_inline(swidb);
    if (sb)
      return(sb->srb_proxy_explorer);
    else
      return(FALSE);
}


void srb_sb_enable_proxy_explorer (idbtype *swidb)
{
    srb_sb_t *sb = srb_get_or_create_sb_inline(swidb);
    if (sb) {
      sb->srb_proxy_explorer = TRUE;
    }
}


void srb_sb_disable_proxy_explorer (idbtype *swidb)
{
    srb_sb_t *sb = srb_get_or_create_sb_inline(swidb);
    if (sb) {
      sb->srb_proxy_explorer = FALSE;
    }
}


boolean srb_sb_get_spanning_explorer (idbtype *swidb)
{
    srb_sb_t *sb = srb_get_sb_inline(swidb);
    if (sb)
      return(sb->srb_spanning_explorer);
    else
      return(FALSE);
}


void srb_sb_enable_spanning_explorer (idbtype *swidb)
{
    srb_sb_t  *sb, *vsb;
    hwidbtype *hwidb   = NULL;
    hwidbtype *hwvidb  = NULL;
    idbtype   *swvidb  = NULL;

    sb  = srb_get_or_create_sb_inline(swidb);
    if (!sb) {
      return;
    }

    sb->srb_spanning_explorer = TRUE;

    hwidb = swidb->hwptr;
    if (hwidb) {
      hwvidb = hwidb->vidb_link;
      swvidb = firstsw_or_null(hwvidb);
    }
    vsb = srb_get_or_create_sb_inline(swvidb);
    if (vsb) {
      vsb->srb_spanning_explorer = TRUE;
    }
    
}


void srb_sb_disable_spanning_explorer (idbtype *swidb)
{
    srb_sb_t  *sb, *vsb;
    hwidbtype *hwidb   = NULL;
    hwidbtype *hwvidb  = NULL;
    idbtype   *swvidb  = NULL;

    sb  = srb_get_or_create_sb_inline(swidb);
    if (!sb) {
      return;
    }

    sb->srb_spanning_explorer = FALSE;

    hwidb = swidb->hwptr;
    if (hwidb) {
      hwvidb = hwidb->vidb_link;
      swvidb = firstsw_or_null(hwvidb);
    }
    vsb = srb_get_or_create_sb_inline(swvidb);
    if (vsb) {
      vsb->srb_spanning_explorer = FALSE;
    }
    
}


boolean srb_sb_get_oldsna_hack (idbtype *swidb)
{
    srb_sb_t *sb = srb_get_sb_inline(swidb);
    if (sb)
      return(sb->srb_oldsna_hack);
    else
      return(FALSE);
}


boolean srb_sb_get_enabled_for_fddi (idbtype *swidb)
{
    srb_sb_t *sb = srb_get_sb_inline(swidb);
    if (sb)
      return(sb->srb_enabled_for_fddi);
    else
      return(FALSE);
}


ulong srb_sb_get_rsrb_transit_oui (idbtype *swidb)
{
    srb_sb_t *sb = srb_get_sb_inline(swidb);
    if (sb)
      return(sb->rsrb_transit_oui);
    else
      return(DEFAULT_OUI);
}



void srb_sb_set_rsrb_transit_oui (idbtype *swidb, ulong transit_oui)
{
    srb_sb_t *sb = srb_get_or_create_sb_inline(swidb);
    if (sb) {
      sb->rsrb_transit_oui = transit_oui;
    }
}


void srb_sb_get_triplet (idbtype *swidb, srb_triplet_t *srb_triplet)
{
    srb_sb_t *sb = srb_get_sb_inline(swidb);
    if (sb) {
        srb_triplet->localRing  = sb->srb_thisring;
        srb_triplet->bridgeNum  = sb->srb_bridge_num;
        srb_triplet->remoteRing = sb->srb_targetring;
    } else {
        srb_triplet->localRing  = SRB_INVALID_RING_NUM;
        srb_triplet->bridgeNum  = SRB_INVALID_BRIDGE_NUM;
        srb_triplet->remoteRing = SRB_INVALID_RING_NUM;
    }
}


void srb_sb_set_triplet (idbtype *swidb, srb_triplet_t *srb_triplet)
{
    srb_sb_t *sb = srb_get_or_create_sb_inline(swidb);
    if (sb) {
        sb->srb_thisring   = srb_triplet->localRing;
        sb->srb_bridge_num = srb_triplet->bridgeNum;
        sb->srb_targetring = srb_triplet->remoteRing;
    }
}


void srb_sb_incr_rx_counters (idbtype *swidb, ulong numBytes)
{
    srb_sb_t *sb = srb_get_sb_inline(swidb);
    if (sb) {
        sb->srb_rx_count++;
        sb->srb_rx_bytes += numBytes;
    }
}


void srb_sb_incr_tx_nb_counters (idbtype *swidb, ulong numBytes)
{
    srb_sb_t *sb = srb_get_sb_inline(swidb);
    if (sb) {
        sb->srb_tx_nb_count++;
        sb->srb_tx_nb_bytes += numBytes;
    }
}


void srb_sb_incr_tx_b_counters (idbtype *swidb, ulong numBytes)
{
    srb_sb_t *sb = srb_get_sb_inline(swidb);
    if (sb) {
        sb->srb_tx_b_count++;
        sb->srb_tx_b_bytes += numBytes;
    }
}


void srb_sb_incr_span_in_counter (idbtype *swidb)
{
    srb_sb_t *sb = srb_get_sb_inline(swidb);
    if (sb) {
        sb->srb_span_in++;
    }
}


void srb_sb_incr_span_out_counter (idbtype *swidb)
{
    srb_sb_t *sb = srb_get_sb_inline(swidb);
    if (sb) {
        sb->srb_span_out++;
    }
}


void srb_sb_incr_allrte_in_counter (idbtype *swidb)
{
    srb_sb_t *sb = srb_get_sb_inline(swidb);
    if (sb) {
        sb->srb_allrte_in++;
    }
}


void srb_sb_incr_allrte_out_counter (idbtype *swidb)
{
    srb_sb_t *sb = srb_get_sb_inline(swidb);
    if (sb) {
        sb->srb_allrte_out++;
    }
}


void srb_sb_set_si_counters (idbtype *swidb)
{
    hwidbtype *hwidb;
    srb_sb_t  *sb;

    
    if (!swidb) {
        return;
    } 

    hwidb = swidb->hwptr;
    if (!hwidb) {
        return;
    } 

    sb = srb_get_sb_inline(swidb);
    if (!sb) {
        return;
    } 

    sb->si_inputs_fast_srb = 
        hwidb->counters.inputs_fast[ACCT_PROTO_SRB];

    sb->si_rx_cumbytes_fast_srb = 
        hwidb->counters.rx_cumbytes_fast[ACCT_PROTO_SRB];

    sb->si_outputs_fast_srb = 
        hwidb->counters.outputs_fast[ACCT_PROTO_SRB];

    sb->si_tx_cumbytes_fast_srb = 
        hwidb->counters.tx_cumbytes_fast[ACCT_PROTO_SRB];

    sb->si_inputs_auto_srb = 
        hwidb->counters.inputs_auto[ACCT_PROTO_SRB];

    sb->si_rx_cumbytes_auto_srb = 
        hwidb->counters.rx_cumbytes_auto[ACCT_PROTO_SRB];

    sb->si_outputs_auto_srb = 
        hwidb->counters.outputs_auto[ACCT_PROTO_SRB];

    sb->si_tx_cumbytes_auto_srb = 
        hwidb->counters.tx_cumbytes_auto[ACCT_PROTO_SRB];

}


ulong srb_sb_get_tx_drops (idbtype *swidb)
{
    srb_sb_t *sb = srb_get_sb_inline(swidb);
    if (sb) {
        return(sb->srb_tx_drops);
    } else {
        return(0);
    }
}

void srb_sb_incr_tx_drops (idbtype *swidb)
{
    srb_sb_t *sb = srb_get_sb_inline(swidb);
    if (sb) {
        sb->srb_tx_drops++;
    }
}


void srb_sb_multi_incr_tx_drops (idbtype *swidb, ulong numDrops)
{
    srb_sb_t *sb = srb_get_sb_inline(swidb);
    if (sb) {
        sb->srb_tx_drops += numDrops;
    }
}



void srb_sb_incr_rx_drops (idbtype *swidb)
{
    srb_sb_t *sb = srb_get_sb_inline(swidb);
    if (sb) {
        sb->srb_rx_drops++;
    }
}


/*
 * srb_sb_init()
 *   SRB subblock subsystem initialization
 *
 */
static void srb_sb_init (subsystype *subsys)
{
    srb_sb_vector_table_init();
}
 
 
#define SRB_SB_MAJVER 1
#define SRB_SB_MINVER 0
#define SRB_SB_EDITVER 1
 
SUBSYS_HEADER(srb_sb,
              SRB_SB_MAJVER, SRB_SB_MINVER, SRB_SB_EDITVER,
              srb_sb_init, SUBSYS_CLASS_PROTOCOL,
              "seq: srbcore", 
              "req: srbcore");


