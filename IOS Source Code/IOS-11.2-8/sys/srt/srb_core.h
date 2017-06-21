/* $Id: srb_core.h,v 3.1.2.1 1996/08/26 15:13:18 ppearce Exp $
 * $Source: /release/112/cvs/Xsys/srt/srb_core.h,v $
 *------------------------------------------------------------------
 * srb_core.h -- SRB core definitions
 *
 * August 1996, Paul Pearce
 *
 * Copyright (c) 1996 by Cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: srb_core.h,v $
 * Revision 3.1.2.1  1996/08/26  15:13:18  ppearce
 * CSCdi66582:  Implement SRB vector table to cut size of SRB_CORE subsys
 * Branch: California_branch
 *
 * Revision 3.1  1996/08/15  03:01:52  ppearce
 * Placeholder for SRB_CORE
 *
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __SRB_CORE_H__
#define __SRB_CORE_H__

typedef void (srbcore_setup_srb_t)(hwidbtype *);
typedef void (srbcore_modify_srb_t)(hwidbtype *, ulong, ulong);

#include "srb_sb.h"


/*
 * SRB vector table structure
 *
 */
struct srb_vector_table_t_ {

  /* 
   * SRB core vectors
   *   
   */
   srbcore_setup_srb_t  *srbcore_setup_srb;
   srbcore_modify_srb_t *srbcore_modify_srb;

  /* 
   * SRB subblock vectors 
   *
   */
  srb_create_sb_t  *srb_create_sb;
  srb_destroy_sb_t *srb_destroy_sb;

  srb_sb_get_max_hops_t *srb_sb_get_max_hops;
  srb_sb_set_max_hops_t *srb_sb_set_max_hops;

  srb_sb_get_maxin_hops_t *srb_sb_get_maxin_hops;
  srb_sb_set_maxin_hops_t *srb_sb_set_maxin_hops;

  srb_sb_get_maxout_hops_t *srb_sb_get_maxout_hops;
  srb_sb_set_maxout_hops_t *srb_sb_set_maxout_hops;

  srb_sb_get_actualring_t *srb_sb_get_actualring;
  srb_sb_set_actualring_t *srb_sb_set_actualring;

  srb_sb_get_thisring_t *srb_sb_get_thisring;
  srb_sb_set_thisring_t *srb_sb_set_thisring;

  srb_sb_get_bridge_num_t *srb_sb_get_bridge_num;
  srb_sb_set_bridge_num_t *srb_sb_set_bridge_num;
  srb_sb_get_bridging_enabled_t *srb_sb_get_bridging_enabled;

  srb_sb_get_targetring_t *srb_sb_get_targetring;
  srb_sb_set_targetring_t *srb_sb_set_targetring;

  srb_sb_init_rng_br_rng_mask_t *srb_sb_init_rng_br_rng_mask;

  srb_sb_get_rng_br_rng_f_t *srb_sb_get_rng_br_rng_f;
  srb_sb_set_rng_br_rng_f_t *srb_sb_set_rng_br_rng_f;

  srb_sb_get_rng_br_rng_r_t *srb_sb_get_rng_br_rng_r;
  srb_sb_set_rng_br_rng_r_t *srb_sb_set_rng_br_rng_r;

  srb_sb_get_proxy_explorer_t     *srb_sb_get_proxy_explorer;
  srb_sb_enable_proxy_explorer_t  *srb_sb_enable_proxy_explorer;
  srb_sb_disable_proxy_explorer_t *srb_sb_disable_proxy_explorer;

  srb_sb_get_spanning_explorer_t     *srb_sb_get_spanning_explorer;
  srb_sb_enable_spanning_explorer_t  *srb_sb_enable_spanning_explorer;
  srb_sb_disable_spanning_explorer_t *srb_sb_disable_spanning_explorer;

  srb_sb_get_oldsna_hack_t      *srb_sb_get_oldsna_hack;
  srb_sb_get_enabled_for_fddi_t *srb_sb_get_enabled_for_fddi;

  srb_sb_get_rsrb_transit_oui_t *srb_sb_get_rsrb_transit_oui;
  srb_sb_set_rsrb_transit_oui_t *srb_sb_set_rsrb_transit_oui;

  srb_sb_get_triplet_t *srb_sb_get_triplet;
  srb_sb_set_triplet_t *srb_sb_set_triplet;

  srb_sb_incr_rx_counters_t    *srb_sb_incr_rx_counters;
  srb_sb_incr_tx_nb_counters_t *srb_sb_incr_tx_nb_counters;
  srb_sb_incr_tx_b_counters_t  *srb_sb_incr_tx_b_counters;

  srb_sb_incr_span_in_counter_t  *srb_sb_incr_span_in_counter;
  srb_sb_incr_span_out_counter_t *srb_sb_incr_span_out_counter;

  srb_sb_incr_allrte_in_counter_t  *srb_sb_incr_allrte_in_counter;
  srb_sb_incr_allrte_out_counter_t *srb_sb_incr_allrte_out_counter;

  srb_sb_set_si_counters_t *srb_sb_set_si_counters;

  srb_sb_get_tx_drops_t        *srb_sb_get_tx_drops;
  srb_sb_incr_tx_drops_t       *srb_sb_incr_tx_drops;
  srb_sb_multi_incr_tx_drops_t *srb_sb_multi_incr_tx_drops;
  srb_sb_incr_rx_drops_t       *srb_sb_incr_rx_drops;

};
STRUCTURE_REF(srb_vector_table_t);


/* 
 * Globally defined SRB vector table
 *
 */
extern srb_vector_table_t srbV;

/*
 * --ppearce
 */ 
extern srbcore_setup_srb_t  srbcore_setup_srb;
extern srbcore_modify_srb_t srbcore_modify_srb;

#endif __SRB_CORE_H__
