/* $Id: if_c4000_m32.c,v 3.2.60.1 1996/06/16 21:15:35 vnguyen Exp $
 * $Source: /release/112/cvs/Xsys/les/if_c4000_m32.c,v $
 *------------------------------------------------------------------
 * if_c4000_m32.c
 *
 * 20 December 1993, Michael Hubenthal
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 * This file contains the the driver and support routines for 
 * Siemens' Munich32 HDLC controller. 
 *
 *------------------------------------------------------------------
 * $Log: if_c4000_m32.c,v $
 * Revision 3.2.60.1  1996/06/16  21:15:35  vnguyen
 * Finally committing Brasil to California_branch
 *
 * Revision 3.2  1995/11/17  17:39:32  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:31:53  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:34:36  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "if_les.h"
#include "logger.h"

#if defined(XX) | defined(SIERRA)
#   include "if_c4000_mcn.h"
#endif
 
#if defined(BRASIL)
#   include "if_as5200_mcn.h"
#endif

#include "if_c4000_m32.h"

/*
 * =====================================================================
 * m32_get_regs()
 *
 * Description:
 * get the starting address of the munich32 register set
 *
 * Parameters:
 * slot - slot the munich32 is in
 * unit - the unit of munich32 under the slot
 *
 * Returns:
 * the address of the munich32 register set
 * =====================================================================
 */
m32_regs_t *m32_get_regs (ulong slot, ulong subunit)
{
    m32_regs_t *reg_addr;

    reg_addr = (m32_regs_t *)((uint)GET_SLOT_BASE(slot) +
                         (uint)MCN_M32_REG_OFFSET(subunit));
    return(reg_addr);
}

/*
 * =====================================================================
 * m32_set_vectors()
 *
 * Description:
 * set the Low IRQ NIM vector to the wrapper for this slot
 *
 * Parameters:
 * slot - slot the munich32 is in
 *
 * Returns:
 * nothing
 * =====================================================================
 */
void m32_set_vector (ulong slot)
{
    mcn_set_vectors(slot);
}

/*
 * =====================================================================
 * m32_reset_chip()
 *
 * Description:
 * reset the Munich32 chip
 *
 * Parameters:
 * slot - slot the munich32 is in
 *
 * Returns:
 * nothing
 * =====================================================================
 */
void m32_reset_chip (ulong slot)
{
    mcn_instance_t *mcn_instance;

    mcn_instance = MCN_INSTANCE(slot);
    mcn_reset_m32(mcn_instance);
}
