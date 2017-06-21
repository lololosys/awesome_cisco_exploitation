/* $Id: srb_core.c,v 3.1.2.2 1996/08/27 17:50:39 ppearce Exp $
 * $Source: /release/112/cvs/Xsys/srt/srb_core.c,v $
 *------------------------------------------------------------------
 * srb_core.c -- SRB core definitions
 *
 * August 1996, Paul Pearce
 *
 * Copyright (c) 1996-1997 by Cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: srb_core.c,v $
 * Revision 3.1.2.2  1996/08/27  17:50:39  ppearce
 * CSCdi66582:  Implement SRB vector table to cut size of SRB_CORE subsys
 * Branch: California_branch
 *   Remove debugging code -- it's taking up space in the SRB_CORE subsys
 *
 * Revision 3.1.2.1  1996/08/26  15:13:16  ppearce
 * CSCdi66582:  Implement SRB vector table to cut size of SRB_CORE subsys
 * Branch: California_branch
 *
 * Revision 3.1  1996/08/15  03:01:50  ppearce
 * Placeholder for SRB_CORE
 *
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#include "master.h"
#include "string.h"
#include "ieee.h"
#include "interface_private.h"
#include "subsys.h"
#include "srb_core.h"
#include "srb_core_registry.h"


/*
 * srbcore_return_max_hops()
 *
 */
ushort srbcore_return_max_hops (void)
{
    return(SRB_DEF_MAXHOPS);
}

/*
 * srbcore_return_invalid_ring_num()
 *
 */
ushort srbcore_return_invalid_ring_num (void)
{
    return(SRB_INVALID_RING_NUM);
}

/*
 * srbcore_return_invalid_bridge_num()
 *
 */
ushort srbcore_return_invalid_bridge_num (void)
{
    return(SRB_INVALID_BRIDGE_NUM);
}

/*
 * srbcore_return_default_oui()
 *
 */
ulong srbcore_return_default_oui (void)
{
    return(DEFAULT_OUI);
}


/* 
 * Global SRB vector table
 *
 */
srb_vector_table_t srbV = {

    /* 
     * Initialize all vectors in SRB vector table to inocuous routines
     *
     */
    (srbcore_setup_srb_t *)  &srbcore_setup_srb,
    (srbcore_modify_srb_t *) &srbcore_modify_srb,

    (srb_create_sb_t *)  &return_null,
    (srb_destroy_sb_t *) &return_false,

    (srb_sb_get_max_hops_t *) &srbcore_return_max_hops,
    (srb_sb_set_max_hops_t *) &return_null,

    (srb_sb_get_maxin_hops_t *) &srbcore_return_max_hops,
    (srb_sb_set_maxin_hops_t *) &return_null,

    (srb_sb_get_maxout_hops_t *) &srbcore_return_max_hops,
    (srb_sb_set_maxout_hops_t *) &return_null,
   
    (srb_sb_get_actualring_t *) &srbcore_return_invalid_ring_num,
    (srb_sb_set_actualring_t *) &return_null,

    (srb_sb_get_thisring_t *)   &srbcore_return_invalid_ring_num,
    (srb_sb_set_thisring_t *)   &return_null,

    (srb_sb_get_bridge_num_t *) &srbcore_return_invalid_bridge_num,
    (srb_sb_set_bridge_num_t *) &return_null,

    (srb_sb_get_bridging_enabled_t *) &return_false,

    (srb_sb_get_targetring_t *) &srbcore_return_invalid_ring_num,
    (srb_sb_set_targetring_t *) &return_null,

    (srb_sb_init_rng_br_rng_mask_t *) return_null,

    (srb_sb_get_rng_br_rng_f_t *) return_zero,
    (srb_sb_set_rng_br_rng_f_t *) return_null,

    (srb_sb_get_rng_br_rng_r_t *) return_zero,
    (srb_sb_set_rng_br_rng_r_t *) return_null,

    (srb_sb_get_proxy_explorer_t *) &return_false,
    (srb_sb_enable_proxy_explorer_t *) &return_null,
    (srb_sb_disable_proxy_explorer_t *) &return_null,

    (srb_sb_get_spanning_explorer_t *) &return_false,
    (srb_sb_enable_spanning_explorer_t *) &return_null,
    (srb_sb_disable_spanning_explorer_t *) &return_null,

    (srb_sb_get_oldsna_hack_t *) &return_false,
    (srb_sb_get_enabled_for_fddi_t *) &return_false,

    (srb_sb_get_rsrb_transit_oui_t *) &srbcore_return_default_oui,
    (srb_sb_set_rsrb_transit_oui_t *) &return_null,

    (srb_sb_get_triplet_t *) &return_null,
    (srb_sb_set_triplet_t *) &return_null,

    (srb_sb_incr_rx_counters_t *) &return_null,
    (srb_sb_incr_tx_nb_counters_t *) &return_null,
    (srb_sb_incr_tx_b_counters_t *) &return_null,

    (srb_sb_incr_span_in_counter_t *) &return_null,
    (srb_sb_incr_span_out_counter_t *) &return_null,

    (srb_sb_incr_allrte_in_counter_t *) &return_null,
    (srb_sb_incr_allrte_out_counter_t *) &return_null,

    (srb_sb_set_si_counters_t *) &return_null,

    (srb_sb_get_tx_drops_t *) &return_zero,
    (srb_sb_incr_rx_drops_t *) &return_null,
    (srb_sb_multi_incr_tx_drops_t *) &return_null,
    (srb_sb_incr_tx_drops_t *) &return_null

};


/*
 * srb_get_vector_table()
 *   Return pointer to SRB vector table
 *
 */
srb_vector_table_t *srb_get_vector_table (void)
{
    return(&srbV);
}


/* 
 * srbcore_init()
 *   SRB core subsystem initialization
 *
 */
static void srbcore_init (subsystype *subsys)
{
    reg_add_srb_get_vector_table(srb_get_vector_table, "srb_get_vector_table");
}

 
#define SRBCORE_MAJVER 1
#define SRBCORE_MINVER 0
#define SRBCORE_EDITVER 1
 
SUBSYS_HEADER(srbcore, 
              SRBCORE_MAJVER, SRBCORE_MINVER, SRBCORE_EDITVER,
              srbcore_init, SUBSYS_CLASS_PROTOCOL, NULL, NULL);


/*
 * srbcore_setup_srb()
 *
 */
void srbcore_setup_srb (hwidbtype *hwidb)
{
    if (IS_VIP(hwidb)) {
        reg_invoke_vip_setup_srb(hwidb->type, hwidb);
    } else {
        reg_invoke_setup_srb(hwidb->type, hwidb);
    }
}


/*
 * srbcore_modify_srb()
 *
 */
void srbcore_modify_srb (hwidbtype *hwidb, ulong parm1, ulong parm2)
{
    if (IS_VIP(hwidb)) {
        reg_invoke_vip_modify_srb(hwidb->type, hwidb, parm1, parm2);
    } else {
        reg_invoke_modify_srb(hwidb->type, hwidb, parm1, parm2);
    }
}
