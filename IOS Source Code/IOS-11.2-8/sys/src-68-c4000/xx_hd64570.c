/* $Id: xx_hd64570.c,v 3.3 1995/11/17 18:43:43 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/src-68-c4000/xx_hd64570.c,v $
 *------------------------------------------------------------------
 * xx_hd64570.c - 68K C4000 specific HD64570 driver code
 *
 * March 1994, Mohsen Mortazavi
 *
 * Copyright (c) 1994-1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: xx_hd64570.c,v $
 * Revision 3.3  1995/11/17  18:43:43  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:31:17  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:25:28  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:55:10  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "interface_private.h"
#include "logger.h"
#include "../les/if_les.h"
#include "../les/if_les_hd64570.h"

/*
 * Static function arrays for interrupt setups
 */
static uint qsim_int_vector [] = {
    (uint)qsim_slot0a,
    (uint)qsim_slot1a,
    (uint)qsim_slot2a,
    (uint)qsim_slot0b,
    (uint)qsim_slot1b,
    (uint)qsim_slot2b,
};


/*
 * hd64570_set_vectors()
 *
 * Set the interrupt wrappers for this serial interface
 */
void hd64570_set_vectors (hwidbtype *idb)
{
    hd64570_instance_t *ds;
    int vector_offset;
    int index;

    ds = INSTANCE;

    set_slot_idb(idb);

    /*
     * Each port pair (0 & 1) and (2 & 3) share the same wrapper.
     */
    if (idb->subunit < 2)
        vector_offset = 0;
    else
        vector_offset = 1;

    index = idb->slot + (vector_offset * C4000_NUM_SLOTS);

    /*
     * Set up interrupt vector table entry and NIM vector register.
     */
    set_slot_vector(idb->slot,
                    vector_offset,
                    qsim_int_vector[index]);

    ds->regaddr->vector_base = get_slot_vector(idb->slot, vector_offset);

#ifdef DEBUG
    buginf("qsim_slot%d = %x\n",idb->slot,
        (XX_START_VECTOR_ADDR + (idb->slot * XX_VECTOR_SEPARATION)));
#endif
}

/*
 * hd64570_get_wrapper_base()
 * Get the base address for the secondary wrappers for this slot.
 */
void *hd64570_get_wrapper_base (uint slot)
{
    return ((void *)(NIM_SCRATCH_BASE + slot * NIM_SCRATCH_BYTES_PER_SLOT));
}


/*
 * hd64570_check_version:
 * Certain C4000 platforms, namely sierra, need the NIMs to be of a
 * certain rev level.  If the NIM isn't of that NIM level, whine about
 * it.
 */
void hd64570_check_version (int slot)
{
    /*
     * The XX can handle all the NIMs with no NIM version restriction
     */
}
