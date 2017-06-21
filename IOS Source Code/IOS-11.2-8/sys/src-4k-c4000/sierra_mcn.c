/* $Id: sierra_mcn.c,v 3.2 1995/11/17 18:41:17 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/src-4k-c4000/sierra_mcn.c,v $
 *------------------------------------------------------------------
 * sierra_mcn.c - R4k C4000 specific Munich32 driver code
 *
 * July 1994, Michael Beesley
 *
 * Copyright (c) 1993-1994 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: sierra_mcn.c,v $
 * Revision 3.2  1995/11/17  18:41:17  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:22:54  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:53:34  hampton
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
     * write the wrapper address into the Level 2 jumptable.
     */
    set_slot_vector(slot, m32im_wrapper);

    /*
     * set up the IACK register on the NIM.
     */
    ds = MCN_INSTANCE(slot);
    ds->mcn_regs->m32_vector = slot;
}


