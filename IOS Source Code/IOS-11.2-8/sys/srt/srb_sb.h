/* $Id: srb_sb.h,v 3.1.2.7 1996/08/26 15:13:28 ppearce Exp $
 * $Source: /release/112/cvs/Xsys/srt/srb_sb.h,v $
 *------------------------------------------------------------------
 * srb_sb.h -- SRB subblock definition and accessor routines
 *
 * April 1996, Paul Pearce
 *
 * Copyright (c) 1996 by Cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: srb_sb.h,v $
 * Revision 3.1.2.7  1996/08/26  15:13:28  ppearce
 * CSCdi66582:  Implement SRB vector table to cut size of SRB_CORE subsys
 * Branch: California_branch
 *
 * Revision 3.1.2.6  1996/07/23  18:49:30  ppearce
 * CSCdi63747:  Move srb_multiring out of SRB subblock
 * Branch: California_branch
 *   (1) srb_multiring doesn't really belong exclusively to SRB
 *         move srb_multiring out of SRB subblock and into swidb
 *         provide srb_multiring accessor routines
 *   (2) determine_rif() should take swidb as argument rather than hwidb
 *
 * Revision 3.1.2.5  1996/07/21  04:34:13  ppearce
 * CSCdi63366:  Move SRB counters from hwidb into SRB subblock
 * Branch: California_branch
 *   srb_sb_set_si_counters causes build error in c100x images
 *     use SRB vector table registry instead of direct invocation
 *
 * Revision 3.1.2.4  1996/07/18  20:29:53  ppearce
 * CSCdi63366:  Move SRB counters from hwidb into SRB subblock
 * Branch: California_branch
 *   - Decreases size of hwidb for non-SRB'd interfaces
 *   - Provides additional IOS-independence for SRB
 *   - Moves SRB a little closer to being completely swidb-based
 *
 * Revision 3.1.2.3  1996/07/09  06:06:15  ppearce
 * CSCdi59527: inlines waste code space
 * Branch: California_branch
 *   Just say "no" to inline abuse - SRB subblock inlines removed
 *
 * Revision 3.1.2.2  1996/05/22  17:06:57  ppearce
 * CSCdi58099:  SRB fastswitching performance
 * Branch: California_branch
 *   Use direct accesses to SRB subblock fields rather than accessors
 *
 * Revision 3.1.2.1  1996/05/17  12:14:36  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.1.4.2  1996/05/06  00:01:56  ppearce
 * Sync to IbuMod_Calif_baseline_960504
 *
 * Revision 3.1.4.1  1996/04/26  15:23:51  ppearce
 * IBU modularity - SRB subblock (part 1)
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.1  1996/04/26  12:18:42  ppearce
 * Add placeholders for IbuMod_Calif_branch
 *
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#ifndef __SRB_SB_H__
#define __SRB_SB_H__

#include "interface.h"


/*
 * SRB subblock field defines 
 *
 */
#define SRB_DEF_MAXHOPS 7
#define SRB_MIN_MAXHOPS 1
#define SRB_MAX_MAXHOPS 7

#define SRB_INVALID_RING_NUM      0
#define SRB_INVALID_BRIDGE_NUM 0xFF
#define SRB_MAX_RING_NUM       4095
#define SRB_MAX_BRIDGE_NUM       15


/* 
 * Forward reference for SRB subblock structure
 *
 */
STRUCTURE_REF(srb_sb_t);


/* 
 * SRB triplet structure (localRing, bridgeNumber, remoteRing)
 *
 */
struct srb_triplet_t_ {
    ushort localRing;
    uchar  bridgeNum;
    ushort remoteRing;
};
STRUCTURE_REF(srb_triplet_t);


/* 
 * SRB subblock vector typedefs
 *
 */

typedef srb_sb_t *srb_create_sb_t(idbtype *);
typedef boolean   srb_destroy_sb_t(idbtype *);

typedef uchar srb_sb_get_max_hops_t(idbtype *);
typedef void  srb_sb_set_max_hops_t(idbtype *, uchar);

typedef ushort srb_sb_get_maxin_hops_t(idbtype *);
typedef void   srb_sb_set_maxin_hops_t(idbtype *, ushort);
 
typedef ushort srb_sb_get_maxout_hops_t(idbtype *);
typedef void   srb_sb_set_maxout_hops_t(idbtype *, ushort);
 
typedef ushort srb_sb_get_actualring_t(idbtype *);
typedef void   srb_sb_set_actualring_t(idbtype *, ushort);

typedef ushort srb_sb_get_thisring_t(idbtype *);
typedef void   srb_sb_set_thisring_t(idbtype *, ushort);

typedef uchar   srb_sb_get_bridge_num_t(idbtype *);
typedef void    srb_sb_set_bridge_num_t(idbtype *, uchar);
typedef boolean srb_sb_get_bridging_enabled_t(idbtype *);

typedef ushort srb_sb_get_targetring_t(idbtype *);
typedef void   srb_sb_set_targetring_t(idbtype *, ushort);

typedef void  srb_sb_init_rng_br_rng_mask_t(idbtype *);

typedef ulong srb_sb_get_rng_br_rng_f_t(idbtype *);
typedef void  srb_sb_set_rng_br_rng_f_t(idbtype *, ulong);

typedef ulong srb_sb_get_rng_br_rng_r_t(idbtype *);
typedef void  srb_sb_set_rng_br_rng_r_t(idbtype *, ulong);
 
typedef boolean srb_sb_get_proxy_explorer_t(idbtype *);
typedef void srb_sb_enable_proxy_explorer_t(idbtype *);
typedef void srb_sb_disable_proxy_explorer_t(idbtype *);

typedef boolean srb_sb_get_spanning_explorer_t(idbtype *);
typedef void srb_sb_enable_spanning_explorer_t(idbtype *);
typedef void srb_sb_disable_spanning_explorer_t(idbtype *);

typedef boolean srb_sb_get_enabled_for_fddi_t(idbtype *);
 
typedef boolean srb_sb_get_oldsna_hack_t(idbtype *);
 
typedef ulong srb_sb_get_rsrb_transit_oui_t(idbtype *);
typedef void  srb_sb_set_rsrb_transit_oui_t(idbtype *, ulong);

typedef void srb_sb_get_triplet_t(idbtype *, srb_triplet_t *);
typedef void srb_sb_set_triplet_t(idbtype *, srb_triplet_t *);
 
typedef void srb_sb_incr_rx_counters_t(idbtype *, ulong);
typedef void srb_sb_incr_tx_nb_counters_t(idbtype *, ulong);
typedef void srb_sb_incr_tx_b_counters_t(idbtype *, ulong);
 
typedef void srb_sb_incr_span_in_counter_t(idbtype *);
typedef void srb_sb_incr_span_out_counter_t(idbtype *);
 
typedef void srb_sb_incr_allrte_in_counter_t(idbtype *);
typedef void srb_sb_incr_allrte_out_counter_t(idbtype *);
 
typedef void srb_sb_set_si_counters_t(idbtype *);
 
typedef ulong srb_sb_get_tx_drops_t(idbtype *);
typedef void  srb_sb_incr_rx_drops_t(idbtype *);
typedef void  srb_sb_incr_tx_drops_t(idbtype *);
typedef void  srb_sb_multi_incr_tx_drops_t(idbtype *, ulong);
 

#endif __SRB_SB_H__
