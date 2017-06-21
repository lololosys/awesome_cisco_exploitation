/* $Id: xx_mcn.c,v 3.2 1995/11/17 18:43:47 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/src-68-c4000/xx_mcn.c,v $
 *------------------------------------------------------------------
 * xx_mcn.c - 68k C4000 specific Munich32 driver code
 *
 * July 1994, Michael Beesley
 *
 * Copyright (c) 1993-1994 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: xx_mcn.c,v $
 * Revision 3.2  1995/11/17  18:43:47  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:25:31  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:55:14  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "logger.h"
#include "../les/if_les.h"
#include "../les/if_c4000_mcn.h"
#include "../les/if_c4000_m32.h"

/*
 * Static declarations
 */
static uint m32_int_vector [] = {
    (uint)m32im_slot0,
    (uint)m32im_slot1,
    (uint)m32im_slot2,
};

/*
 * =====================================================================
 * mcn_set_vectors -
 *
 * Description:
 * Set the Munich32 interrupt vector register based on slot.
 *
 * Parameters:
 * slot - the slot the NIM is in that needs its vectors set
 *
 * Returns:
 * nothing
 * =====================================================================
 */
void mcn_set_vectors (uint slot)
{
    mcn_instance_t *ds = NULL;
    
    /*
     * write the wrapper address into the vector address space.
     * every slot has its own vector address space.
     */
    set_slot_vector(slot, 
                    0,  /* always one interrupt per NIM */
                    m32_int_vector[slot]); /* wrapper is slot dependent */

    /*
     * set the offset into the vector address space for this
     * nim. since the MCN has only one vectored interrupt 
     * (munich32), the offset will always be zero.
     */
    ds = MCN_INSTANCE(slot);
    ds->mcn_regs->m32_vector = get_slot_vector(slot, 0);
}


