/* $Id: dbus_exceptn.c,v 3.1.36.1 1996/03/21 23:26:54 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/rommon/src/dbus_exceptn.c,v $
 *------------------------------------------------------------------
 * dbus_exceptn.c
 *
 * Jan 96, mansonw
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: dbus_exceptn.c,v $
 * Revision 3.1.36.1  1996/03/21  23:26:54  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.1.4.1  1996/02/08  08:57:54  gstovall
 * Branch: ELC_branch
 * Sync to ELC_baseline_960205.
 *
 * Revision 3.1  1996/02/01  02:27:11  mansonw
 * CSCdi47569:  HSA slave refuses to switch to master state
 * Fixed the HSA slave to gain master during hot insertion/removal
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "monitor.h"
#include "console.h"
#include "cpu_mips.h"
#include "signal_4k.h"
#include "mon_defs.h"
#include "bitdef.h"
#include "dis_mips.h"
#include "error.h"
#include "mon_boot.h"
#include "dclasic.h"
#include "memdasic.h"
#include "dgbus.h"
#include "ci_dbus.h"
#include "queue.h"

extern vc_t vc_intf;

/* 
 * dcl_int
 * Handle external interrupts from DCL
 */
static inline void 
dcl_int(r4k_context_t *frame)
{
    register unsigned char i;
    unsigned char vect;

    i = (DCL_ASIC->int_vect >> 2);  /* read interrupt vector */
    
    vect = DCL_ASIC->int_vmi;       /* read pending bit mask */
   
    if (i != 0)
      i -= 1;

    if ((i == 7) || (vect & (1<<i))) {
       printf("\n*** DCL Interrupt Level %d error ***\n", i);
       printf("*** Valid Masked Register, bit %d not zero (0x%x)\n",
              i, vect);
    }

    if (i == 6)
	if (REG_IRSP2DBUS_FPGA->DBusFPGAIntStatus & INT_STATUS_IRSP2DBUS_FPGA_DBUS_SLAVE_REQ)
	    dbus_handler();
        else
	    (*vc_intf.rom_dbus_handler)(frame);
    else
    /*
     * Dispatch the handler
     */
	(*vc_intf.rom_dcl_interrupt)(frame, i);
} /* dcl_int */


/*
 * handle_interrupt :
 * Handle system interrupts
*/
static inline void
handle_interrupt(r4k_context_t *frame, char *str)
{
    long sreg, cause, intrpt;

    sreg = frame->sreg & CAUSE_IMASK;
    cause = frame->cause & CAUSE_IMASK;

    intrpt = sreg & cause;

    if (intrpt & CAUSE_IBIT2) {
	dcl_int(frame);
	return;
    } else 
	(*vc_intf.rom_handle_interrupt)(frame, str);
}


/*
 * general_exception:
 * Handle all other exceptions (besides the cache)
 */
void
general_exception(r4k_context_t *frame)
{
    int cause;

    cause = (frame->cause & CAUSE_MASK) >> CAUSE_SHIFT;
    if (cause)
	(*vc_intf.rom_general_exception)(frame);
    else
	handle_interrupt(frame, "*** CPU Interrupt ***"); 
}












