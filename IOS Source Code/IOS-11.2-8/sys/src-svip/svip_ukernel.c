/* $Id: svip_ukernel.c,v 3.1.2.3 1996/07/10 02:38:08 getchell Exp $
 * $Source: /release/112/cvs/Xsys/src-svip/svip_ukernel.c,v $
 *------------------------------------------------------------------
 * svip_ukernel.c -- Viper specific routines necessary to build
 *		     sub_ukernel.o 
 *
 * Michael Beesley, March 1996
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: svip_ukernel.c,v $
 * Revision 3.1.2.3  1996/07/10  02:38:08  getchell
 * Added crash context mechanism which is shown via diag command and test
 * cbus (vc command)
 * CSCdi60415:  VIP difficult to debug in the field
 * Branch: California_branch
 *
 * Revision 3.1.2.2  1996/06/07  05:10:45  getchell
 * CSCdi59206:  EMT_RELOAD, EMT_CPU_EXCEPTION does call svip_reload()
 * Branch: California_branch
 *
 * Revision 3.1.2.1  1996/03/21  23:51:41  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.1  1996/03/19  04:07:26  mbeesley
 * Placeholder files for Port Ready
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Include files
 */

#include "master.h"
#include "interface_private.h"
#include "ttysrv.h"
#include "logger.h"
#include "packet.h"

#include "../hes/dgbus.h"
#include "../os/gdb.h"

#include "../pas/if_pas.h"

#include "../src-vip/vip_dbus.h"

#include "../src-svip/svip_asic.h"
#include "../src-svip/svip_nevada.h"

/*
 * svip_reload :
 * Viper crashed or was reloaded
 * NOTE!!!! This routine should only be called by emt_trap in asm_4k_svip.S
 */
void svip_reload (int sig, int code, void *c)
{
    int i;
    ulong sreg;
    dbus_hardware_regs *r;
    r4k_context_t *ptr = (r4k_context_t *)c;
    char trace_msg[130];
    
    SAVE_CALLER();

    /*
     * Debug write cycle so the DAS can trigger
     */
    *(volatile ulong *)(R4K_K1BASE + 0x24C) = 0x22222232;

    /*
     * Turn off the enable led
     */
    vip_set_enable_led(FALSE);

    /*
     * Tell the RP we are ready and enable interrupts
     */
    vip_set_ready(TRUE);
    set_interrupt_level(NMI_LEVEL);
    sreg = getcp0_sreg();
    sreg |= SREG_IE;
    setcp0_sreg(sreg);

    /*
     * Send crash dump context to RSP
     */
    vip_crash_dump(c);

    ttyprintf(CONTTY, "\n\nSystem Reload called from %#x, context=%#x\n",
	      caller(), c);

    sprintf(trace_msg, "System Reload called from %#x, context=%#x",
	    caller(), c);
    vip_logger_write_str(LOG_ALERT, trace_msg);

    vip_context_dump(sig, code, ptr);

    /*
     * Wait for the console output to drain. Do not wait more than
     * 10 seconds.
     */
    for (i=0; i<10000; i++) {
	if (vip_dbus_txempty()) {
	    break;
	}
	usecdelay(1000);		/* wait 1 ms */
    }
    
    /*
     * Signal an internal error and return to emt caller
     */
    r = (dbus_hardware_regs *)(ADRSPC_CYA_ASIC);
    r->reset_reg = COLD_RESET;
    vip_set_internal_error();
}

/*
 * vip_set_enable_led:
 * Control the enable led
 */
void vip_set_enable_led (boolean onoff)
{
    if (onoff) {
	IO_ASIC->cr1 |= (CR1_ENB_PA0_LED | CR1_ENB_PA1_LED);
    } else {
	IO_ASIC->cr1 &= ~(CR1_ENB_PA0_LED | CR1_ENB_PA1_LED);
    }
}

/*
 * svip_reset_io :
 * Reset all the port adaptors
 */
ulong svip_reset_io (void)
{
    return(0);
}

/*
 * platform_fault_history:
 * Save platform dependent fault history
 */
void platform_fault_history (int sig, int code, r4k_context_t *sc,
			     char *fh, int fhsize)
{
}

/* end of file */

