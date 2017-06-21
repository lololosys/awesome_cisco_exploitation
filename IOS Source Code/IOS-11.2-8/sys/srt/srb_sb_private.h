/* $Id: srb_sb_private.h,v 3.1.2.6 1996/09/15 00:52:46 ppearce Exp $
 * $Source: /release/112/cvs/Xsys/srt/srb_sb_private.h,v $
 *------------------------------------------------------------------
 * srb_sb_private.h -- SRB subblock definition and inline routines
 *
 * July 1996, Paul Pearce
 *
 * ***** NOTE *****
 *
 * If you have the urge to include this file...DO NOT!
 *    this file describes the PRIVATE structure for the SRB subblock
 *
 * You should be accessing the SRB subblock fields 
 *    using one of the accessor routines defined in the SRB vector table
 *       (See ../srt/srb_core.h for details)
 *
 * ***** NOTE *****
 *
 * Copyright (c) 1996 by Cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: srb_sb_private.h,v $
 * Revision 3.1.2.6  1996/09/15  00:52:46  ppearce
 * CSCdi68885:  SRB subblock not setup properly for VIP interfaces
 * Branch: California_branch
 *   Add srb_create_sb_for_vip_inline() so that SRB subblock can be
 *     created for VIP when swidb is initialized
 *
 * Revision 3.1.2.5  1996/09/14  01:05:02  ppearce
 * CSCdi68885:  SRB subblock not setup properly for VIP interfaces
 * Branch: California_branch
 *   Previous commit breaks build for c7200- images
 *     Backout previous changes
 *     Create SRB_SB subsystem to init SRB vector table
 *       rather than init in  VIP reset routines
 *
 * Revision 3.1.2.4  1996/08/26  15:13:30  ppearce
 * CSCdi66582:  Implement SRB vector table to cut size of SRB_CORE subsys
 * Branch: California_branch
 *
 * Revision 3.1.2.3  1996/07/23  18:49:34  ppearce
 * CSCdi63747:  Move srb_multiring out of SRB subblock
 * Branch: California_branch
 *   (1) srb_multiring doesn't really belong exclusively to SRB
 *         move srb_multiring out of SRB subblock and into swidb
 *         provide srb_multiring accessor routines
 *   (2) determine_rif() should take swidb as argument rather than hwidb
 *
 * Revision 3.1.2.2  1996/07/18  20:29:55  ppearce
 * CSCdi63366:  Move SRB counters from hwidb into SRB subblock
 * Branch: California_branch
 *   - Decreases size of hwidb for non-SRB'd interfaces
 *   - Provides additional IOS-independence for SRB
 *   - Moves SRB a little closer to being completely swidb-based
 *
 * Revision 3.1.2.1  1996/07/09  13:31:34  ppearce
 * CSCdi59527:  ascii string in inline wastes code space
 * Branch: California_branch
 *   Just say "no" to inline abuse - SRB subblock inlines removed
 *
 * Revision 3.1  1996/07/02  18:24:04  ppearce
 * Placeholder files
 *
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#ifndef __SRB_SB_PRIVATE_H__
#define __SRB_SB_PRIVATE_H__

#include "string.h"
#include "ieee.h"
#include "srb_sb.h"


/*
 * SRB subblock structure
 * ----------------------
 *
 */
struct srb_sb_t_ {
 
    uchar  srb_max_hops;            /* Max hops for all routes explorer      */
    ushort srb_maxin_hops;          /* Max input hops for spanning explorer  */
    ushort srb_maxout_hops;         /* Max output hops for spanning explorer */

    ushort srb_actualring;          /* Ring number supplied by RPS           */

    ushort srb_thisring;            /* Ring number of locally attached ring  */
    uchar  srb_bridge_num;          /* Bridge number b/w local/remote rings  */
    ushort srb_targetring;          /* Ring number of remote ring            */

    ulong rng_br_rng_f;             /* Ring/Bridge/Vring (forward)           */
    ulong rng_br_rng_r;             /* Vring/Bridge/Ring (reverse)           */

    boolean srb_proxy_explorer;     /* SRB proxy explorer enabled?           */
    boolean srb_spanning_explorer;  /* Forward spanning explorers?           */

    boolean srb_oldsna_hack;        /* automatic change 02xx to explorer     */
    boolean srb_enabled_for_fddi;   /* SRB for FDDI enabled?                 */

    ulong rsrb_transit_oui;         /* Transit OUI for rsrb                  */

    ulong srb_rx_bytes;      /* Number of bytes received                    */
    ulong srb_rx_count;      /* Number of packets received                  */

    ulong srb_tx_nb_bytes;   /* Number of non-broadcast bytes transmitted   */
    ulong srb_tx_nb_count;   /* Number of non-broadcast packets transmitted */

    ulong srb_tx_b_bytes;    /* Number of broadcast bytes transmitted       */
    ulong srb_tx_b_count;    /* Number of broadcast packets transmitted     */

    ulong srb_span_in;       /* Number of spanning explorers received       */
    ulong srb_span_out;      /* Number of spanning explorers transmitted    */

    ulong srb_allrte_in;     /* Number of allroutes exploreres received     */
    ulong srb_allrte_out;    /* Number of allroutes exploreres transmitted  */

    ulong si_inputs_fast_srb;       
    ulong si_inputs_auto_srb;      

    ulong si_rx_cumbytes_fast_srb;
    ulong si_rx_cumbytes_auto_srb;

    ulong si_outputs_fast_srb;
    ulong si_outputs_auto_srb;

    ulong si_tx_cumbytes_fast_srb;
    ulong si_tx_cumbytes_auto_srb;

    ulong srb_rx_drops;      /* Number of receive packets dropped by SRB    */
    ulong srb_tx_drops;      /* Number of transmit packets dropped by SRB   */
};


/* 
 * SRB subblock routine externs
 *
 */
extern srb_sb_t *srb_create_sb(idbtype *);
extern boolean   srb_destroy_sb(idbtype *);

extern uchar srb_sb_get_max_hops(idbtype *);
extern void  srb_sb_set_max_hops(idbtype *, uchar);

extern ushort srb_sb_get_maxin_hops(idbtype *);
extern void   srb_sb_set_maxin_hops(idbtype *, ushort);

extern ushort srb_sb_get_maxout_hops(idbtype *);
extern void   srb_sb_set_maxout_hops(idbtype *, ushort);

extern ushort srb_sb_get_actualring(idbtype *);
extern void   srb_sb_set_actualring(idbtype *, ushort);

extern ushort srb_sb_get_thisring(idbtype *);
extern void   srb_sb_set_thisring(idbtype *, ushort);

extern uchar   srb_sb_get_bridge_num(idbtype *);
extern void    srb_sb_set_bridge_num(idbtype *, uchar);
extern boolean srb_sb_get_bridging_enabled(idbtype *);

extern ushort srb_sb_get_targetring(idbtype *);
extern void   srb_sb_set_targetring(idbtype *, ushort);

extern void  srb_sb_init_rng_br_rng_mask(idbtype *);

extern ulong srb_sb_get_rng_br_rng_f(idbtype *);
extern void  srb_sb_set_rng_br_rng_f(idbtype *, ulong);

extern ulong srb_sb_get_rng_br_rng_r(idbtype *);
extern void  srb_sb_set_rng_br_rng_r(idbtype *, ulong);

extern boolean srb_sb_get_proxy_explorer(idbtype *);
extern void    srb_sb_enable_proxy_explorer(idbtype *);
extern void    srb_sb_disable_proxy_explorer(idbtype *);

extern boolean srb_sb_get_spanning_explorer(idbtype *);
extern void    srb_sb_enable_spanning_explorer(idbtype *);
extern void    srb_sb_disable_spanning_explorer(idbtype *);

extern boolean srb_sb_get_oldsna_hack(idbtype *);
extern boolean srb_sb_get_enabled_for_fddi(idbtype *);

extern ulong srb_sb_get_rsrb_transit_oui(idbtype *);
extern void  srb_sb_set_rsrb_transit_oui(idbtype *, ulong);
 
extern void srb_sb_get_triplet(idbtype *, srb_triplet_t *);
extern void srb_sb_set_triplet(idbtype *, srb_triplet_t *);
 
extern void srb_sb_incr_rx_counters(idbtype *, ulong);
extern void srb_sb_incr_tx_nb_counters(idbtype *, ulong);
extern void srb_sb_incr_tx_b_counters(idbtype *, ulong);
 
extern void srb_sb_incr_span_in_counter(idbtype *);
extern void srb_sb_incr_span_out_counter(idbtype *);

extern void srb_sb_incr_allrte_in_counter(idbtype *);
extern void srb_sb_incr_allrte_out_counter(idbtype *);
 
extern void srb_sb_set_si_counters(idbtype *);
 
extern ulong srb_sb_get_tx_drops(idbtype *);
extern void  srb_sb_incr_rx_drops(idbtype *);
extern void  srb_sb_incr_tx_drops(idbtype *);
extern void  srb_sb_multi_incr_tx_drops(idbtype *, ulong);
 

/* 
 * srb_get_sb_inline()
 *   Return SRB subblock
 *
 */
static inline srb_sb_t *srb_get_sb_inline (idbtype *hwidb)
{
  return(idb_get_swsb(hwidb, SWIDB_SB_SRB));
}


/* 
 * 
 * srb_get_or_create_sb_inline()
 *   Get SRB subblock (create if necessary)
 */
static inline srb_sb_t *srb_get_or_create_sb_inline (idbtype *swidb)
{
  srb_sb_t *sb = srb_get_sb_inline(swidb);
  if (!sb) {
    sb = srb_create_sb(swidb);
  }
  return(sb);
}


/*
 * The following macros help with fast-switch/autonomous-switch accounting
 *
 */

static inline ulong srb_inputs_fast(hwidbtype *idb, srb_sb_t* sb)
{
  return(idb->counters.inputs_fast[ACCT_PROTO_SRB] - sb->si_inputs_fast_srb);
}

static inline ulong srb_inputs_auto(hwidbtype *idb, srb_sb_t* sb)
{
  return(idb->counters.inputs_auto[ACCT_PROTO_SRB] - sb->si_inputs_auto_srb);
}

static inline ulong srb_rx_cumbytes_fast(hwidbtype *idb, srb_sb_t *sb)
{
  return(idb->counters.rx_cumbytes_fast[ACCT_PROTO_SRB] - sb->si_rx_cumbytes_fast_srb);
}

static inline ulong srb_rx_cumbytes_auto(hwidbtype *idb, srb_sb_t *sb)
{
  return(idb->counters.rx_cumbytes_auto[ACCT_PROTO_SRB] - sb->si_rx_cumbytes_auto_srb);
}

static inline ulong srb_outputs_fast(hwidbtype *idb, srb_sb_t *sb)
{
  return(idb->counters.outputs_fast[ACCT_PROTO_SRB] - sb->si_outputs_fast_srb);
}

static inline ulong srb_outputs_auto(hwidbtype *idb, srb_sb_t *sb)
{
  return(idb->counters.outputs_auto[ACCT_PROTO_SRB] - sb->si_outputs_auto_srb);
}

static inline ulong srb_tx_cumbytes_fast(hwidbtype *idb, srb_sb_t *sb)
{
  return(idb->counters.tx_cumbytes_fast[ACCT_PROTO_SRB] - sb->si_tx_cumbytes_fast_srb);
}

static inline ulong srb_tx_cumbytes_auto(hwidbtype *idb, srb_sb_t *sb)
{
  return(idb->counters.tx_cumbytes_auto[ACCT_PROTO_SRB] - sb->si_tx_cumbytes_auto_srb);
}


/*
 * srb_create_sb_for_vip_inline()
 *
 *   Provide inline routine for VIP interfaces to invoke
 *     to create SRB subblock when swidb is created
 *
 * ---
 *
 *   Usually, the SRB subblock is created when the first SRB subblock
 *     field is initialized to some value; but the VIP seems to run
 *     its configuration at interrupt level and malloc of subblock fails
 *
 *   This is a *TOTAL HACK* to get SRB on the VIP working again
 *     please don't use this as an example for SRB subblock creation
 *       use the srb_create_sb vector in the SRB vector table
 *
 *   The VIP cannot use the srbV at swidb creation time
 *     because the SRB_SB subsys is not yet initialized
 *
 * --ppearce
 *
 */
static inline srb_sb_t *srb_create_sb_for_vip_inline (idbtype *swidb)
{
    srb_sb_t  *sb;
    swidb_sb_t sbtype = SWIDB_SB_SRB;
 
    /*
     * Create subblock (if it doesn't already exist)
     *
     */
    sb = idb_get_swsb(swidb, sbtype);
    if (!sb) {
        sb = malloc_named(sizeof(srb_sb_t), "VIP_SWIDB_SB: SRB Info");
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
 
} /* End srb_create_sb_for_vip_inline() */

#endif __SRB_SB_PRIVATE_H__
