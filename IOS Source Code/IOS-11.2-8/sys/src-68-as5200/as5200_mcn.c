/* $Id: as5200_mcn.c,v 1.1.62.1 1996/06/16 21:19:29 vnguyen Exp $
 * $Source: /release/112/cvs/Xsys/src-68-as5200/as5200_mcn.c,v $
 *------------------------------------------------------------------
 * as5200_mcn.c - 68k AS5200 specific Munich32 driver code
 *
 * December, 1995 - Jonathan Chiung-Hsi Lin
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: as5200_mcn.c,v $
 * Revision 1.1.62.1  1996/06/16  21:19:29  vnguyen
 * Finally committing Brasil to California_branch
 *
 * Revision 1.1  1995/12/20  00:08:11  jchlin
 * Placeholders for new development.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "logger.h"
#include "../les/if_les.h"
#include "../les/if_as5200_mcn.h"
#include "../les/if_c4000_m32.h"

/*
 * Static declarations
 */

/*
 * =====================================================================
 * mcn_dual_m32_interrupt()
 *
 * Description:
 * Set the Munich32 interrupt vector into the level 4 interrupt wrapper.
 * This routine is used for setting up dual m32 on the T1/E1 board.
 *
 * Parameters:
 * slot - the slot the NIM is in that needs its vectors set
 *
 * Returns:
 * nothing
 * =====================================================================
 */
static void mcn_dual_m32_interrupt (uint slot)
{
    mcn_instance_t           *ds;
    register volatile ushort *status_ptr;
    register ushort           status;

    ds         = MCN_INSTANCE(slot);

    if (ds==NULL) {
        buginf("Error: Wrong m32 interurpt in slot %d\n", slot);
        return;
    }

    status_ptr = &ds->mcn_regs->pri_status;
    status     = *status_ptr;

    if (status & MCN_M32_1_INTERRUPRT) {
        *status_ptr  = ~MCN_M32_1_INTERRUPRT;     /* clear the interrupt */
        m32_interrupt(slot, 0);                   /* subunit == 0 */
        status       = *status_ptr;
    }
    
    if (status & MCN_M32_2_INTERRUPRT) {
        *status_ptr  = ~MCN_M32_2_INTERRUPRT;     /* clear the interrupt */
        m32_interrupt(slot, 1);                   /* subunit == 1 */
    }
}


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
    /*
     * register level 4 interrupt for this slot, to handle NIM
     * processor interrupts
     */
    
    switch(BRASIL_BOARD_ID(slot)) {
    case BRASIL_T1_DPRI_PLUS_CSU:
    case BRASIL_E1_DPRI_PLUS_MODEMS:
        nim_register_hi_irq_handler((ushort) slot, mcn_dual_m32_interrupt,
                                    NIM_HI_IRQ_M32, (ushort) slot);
        break;

    default:
        buginf("Error: Board ID is not recognized. Cannot setup ISR in the wrapper "
               "for m32 at slot %d\n", slot);
    }
}


