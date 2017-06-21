/* $Id: rvip_ukernel.c,v 3.1.20.3 1996/07/10 02:38:05 getchell Exp $
 * $Source: /release/112/cvs/Xsys/src-rvip/rvip_ukernel.c,v $
 *------------------------------------------------------------------
 * rvip_ukernel.c -- RVIP specific routines necessary to build
 *		     sub_ukernel.o 
 *
 * David Hampton, February 1996
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: rvip_ukernel.c,v $
 * Revision 3.1.20.3  1996/07/10  02:38:05  getchell
 * Added crash context mechanism which is shown via diag command and test
 * cbus (vc command)
 * CSCdi60415:  VIP difficult to debug in the field
 * Branch: California_branch
 *
 * Revision 3.1.20.2  1996/03/21  23:51:00  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.1.20.1  1996/03/18  22:03:25  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.1.10.2  1996/03/16  07:37:44  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.1.10.1  1996/03/09  05:13:20  hampton
 * Slim down the kernel of the router.
 * Branch: DeadKingOnAThrone_branch
 *
 * Revision 3.1  1996/02/21  21:52:32  hampton
 * Placeholders for Port Ready.
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
#include "../hes/dgbus.h"
#include "../os/gdb.h"
#include "../pas/if_pas.h"
#include "../src-vip/vip_dbus.h"
#include "logger.h"

/*
 * rvip_reload :
 * Checkers crashed or was reloaded
 */
void rvip_reload (int sig, int code, void *c)
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
    DAS_DEBUG(0xfe);

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
     * Signal an internal error and wait for a reset
     */
    r = (dbus_hardware_regs *)(ADRSPC_DBUS);
    r->reset_reg = COLD_RESET;
    vip_set_internal_error();
    for (;;) {
	;
    }
}

/*
 * vip_set_enable_led:
 * Control the enable led
 */
void vip_set_enable_led (boolean onoff)
{
    if (onoff) {
	IO_ASIC->cfgb &= ~(CFGB_ENBL_LED_PA0|CFGB_ENBL_LED_PA1);
    } else {
	IO_ASIC->cfgb |= (CFGB_ENBL_LED_PA0|CFGB_ENBL_LED_PA1);
    }
}

/*
 * rvip_reset_io :
 * Reset all the port adaptors
 */
ulong rvip_reset_io (void)
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
