/* $Id: vip_af.c,v 3.2.62.3 1996/04/05 04:17:45 sjacobso Exp $
 * $Source: /release/112/cvs/Xsys/src-vip/vip_af.c,v $
 *------------------------------------------------------------------
 * vip_af.c - VIP software address filter support
 *
 * Michael Beesley, June 1995
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: vip_af.c,v $
 * Revision 3.2.62.3  1996/04/05  04:17:45  sjacobso
 * CSCdi52859:  New AF and new ISL code not fully integrated
 * Branch: California_branch
 * Cause dec21140 address filter write code to set the correct
 * ISL enable flags on receipt of an ISL entry.
 *
 * Revision 3.2.62.2  1996/03/29  00:45:53  sjacobso
 * CSCdi45152:  PERFORMANCE: 1FE runs in promiscuous mode instead of
 * filtering
 * Branch: California_branch
 * Add code to allow dec21140 chip to run in perfect
 * address filtering mode whenever possible.  Modify
 * address filter write code to record information
 * about address filter entries that may affect perfect
 * filter operation.  Interface VIP-specific software
 * address filter to RVIP/SVIP/Predator common routines.
 *
 * Revision 3.2.62.1  1996/03/21  23:52:16  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.2.2.1  1996/03/18  23:36:32  mbeesley
 * Branch: ELC_branch
 * Sync to V111_1_3
 *
 * Revision 3.2  1995/11/17  18:51:34  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:29:43  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:29:32  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/06/28  23:44:34  mbeesley
 * Add placeholder files for new development.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
 
/*
 * Include files
 */
#include "master.h"
#include "interface_private.h"
#include "packet.h"
#include "logger.h"

#include "../os/free.h"
#include "../os/pool.h"
#include "../os/particle.h"
#include "../os/buffers.h"
#include "../os/buffers_inline.h"

#include "../pas/if_pas.h"

#include "vip_memd.h"
#include "vip_af.h"
#include "vip_isl.h"

/*
 * vip_address_filter_allocate:  
 * Malloc the address filter structure and initialize pointer in pas_cbtype
 */
boolean vip_address_filter_allocate (hwidbtype *idb)
{
    pas_cbtype *pascb = INSTANCE(idb);

    /*
     * Allocate an address filter block in DRAM.  This block must be 8 byte
     * aligned to work correctly.  Cache line aligned is slightly faster.
     */
    pascb->vip_af = mempool_malloc_aligned(MEMPOOL_CLASS_FAST, 
					   VIP_AF_BYTES, VIP_AF_ALIGNMENT);
    if (!pascb->vip_af) {
        return(FALSE);
    }
    pascb->isl_enabled = FALSE;
    idb->isl_vlan_on_hwidb = FALSE;
    return(TRUE);
}
 
/*
 * vip_address_filter_init:  
 * Clear the address filter index and number of entries in the pascb
 */
void vip_address_filter_init (hwidbtype *idb)
{
    pas_cbtype *pascb = INSTANCE(idb);
    vip_af_entry_t *af_ptr;

    /*
     * Initialize address filter short index to point to the first
     * short word (word 0) in the address filter.
     */
    pascb->vip_af_index = 0;

    /*
     * Initialize the number of address filter entries to zero.
     */
    pascb->vip_af_entries = 0;

    /*
     * Initialize the address filter status to 0.
     */
    af_ptr = pascb->vip_af;
    if (!af_ptr) {
	return;
    }
    af_ptr->status_or_pad = 0;

    /*
     * Initialize the ISL support
     */
    pascb->isl_enabled = FALSE;
    idb->isl_vlan_on_hwidb = FALSE;
}
 
/*
 * vip_address_filter_print:  
 * Print address filter information from pascb
 */
void vip_address_filter_print (pas_cbtype *pascb)
{
    leveltype level;
    vip_af_entry_t *af, *af_ptr;
    int i, af_index, af_entries;

    /*
     * Check that there is an address filter
     */
    if (!pascb->vip_af) {
	return;
    }
    
    /*
     * Get some memory
     */
    af = malloc(VIP_AF_BYTES);
    if (!af) {
        return;
    }
 
    /*
     * Lock out interrupts and copy the address filter
     */
    level = raise_interrupt_level(ALL_DISABLE);
    bcopy(pascb->vip_af, af, VIP_AF_BYTES);
    af_index = pascb->vip_af_index;
    af_entries = pascb->vip_af_entries;
    reset_interrupt_level(level);

    /*
     * Print out the address filter
     */
    af_ptr = af;
    printf(" VIP addr_filter=%#x, af_index=%d, af_entries=%d, "
           "af_stat=0x%-04x\n",
	   pascb->vip_af, af_index, af_entries, af_ptr->status_or_pad);
    for (i=0; i<af_entries; i++) {
	printf("  Addr=%-04x.%-04x.%-04x, Mask=%-04x.%-04x.%-04x, "
	       "Filter=0x%-04x\n",
	       af_ptr->match[0], af_ptr->match[1], af_ptr->match[2],
	       af_ptr->mask[0], af_ptr->mask[1], af_ptr->mask[2],
	       af_ptr->filter);
	af_ptr += 1;
    }
    free(af);
}

/* end of file */

