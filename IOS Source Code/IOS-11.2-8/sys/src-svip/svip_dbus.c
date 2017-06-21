/* $Id: svip_dbus.c,v 3.1.62.1 1996/03/21 23:51:24 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/src-svip/svip_dbus.c,v $
 *------------------------------------------------------------------
 * svip_dbus.c : SVIP DBus support
 *
 * November 1995, David Getchell
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: svip_dbus.c,v $
 * Revision 3.1.62.1  1996/03/21  23:51:24  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.1.10.1  1996/02/10  00:21:59  mbeesley
 * CSCdi48581:  Add VIP2 support to ELC_branch
 * Branch: ELC_branch
 *
 * Revision 3.1  1995/12/26  21:18:37  getchell
 * Placeholder files for Viper development
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Include files 
 */
#include "master.h"
#include "stacks.h"

#include "../hes/dgbus.h"
#include "../src-vip/vip_dbus.h"

#include "svip_dbus.h"
#include "svip_pcmap.h"
#include "svip_asic.h"

/* TBD - do we want this here?  we need to deal with the whole issue of 
   how we support rvip and svip with the same dbus handler.
*/

/*
 * install_svip_dbus_handler :
 * Install the SVIP DBUS interrupt handler
 */
void install_svip_dbus_handler (void)
{
    ushort * ptr;
    int i;

    /* Required for RSP operation */
    dbus_cntl.r.regs.diag_control.value |= (MASTER_ENABLE | WCS_LOADED);
    
    dbus_cntl.dbus_reg = (dbus_hardware_regs *)(ADRSPC_DBUS);
    dbus_cntl.r.regs.diag_status.value = dbus_cntl.dbus_reg->status;
    dbus_cntl.r.regs.diag_status.value &= ~DBUS_SW_READY;
    dbus_cntl.dbus_reg->status = dbus_cntl.r.regs.diag_status.value;

    createlevel(LEVEL_DBUS, vip_dbus_interrupt_handler, "DBUS Interrupt");

    /*
     * Read the EEPROM data
     */
    ptr = (ushort *)(&dbus_cntl.r.regs.eeprom);
    for (i=0; i<16; i++) {

/* 
 * FIXME
 * can we address eeprom better?
 */	
	*ptr++ = CYA_ASIC->eeprom[(i*4)+3] & 0xffff;
    }

    /*
     * Set COLD_RESET as the reason in the reset_reg. This
     * will force the monitor to do a full initialization
     * if the RP ever resets us (which will require the ucode 
     * to be downloaded again)
     */
    dbus_cntl.dbus_reg->reset_reg = COLD_RESET;
}
