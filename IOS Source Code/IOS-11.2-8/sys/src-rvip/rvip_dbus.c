/* $Id: rvip_dbus.c,v 3.1.62.1 1996/03/21 23:50:43 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/src-rvip/rvip_dbus.c,v $
 *------------------------------------------------------------------
 * rvip_dbus.c : RVIP DBus support
 *
 * November 1995, David Getchell
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: rvip_dbus.c,v $
 * Revision 3.1.62.1  1996/03/21  23:50:43  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.1.10.1  1996/02/10  00:19:03  mbeesley
 * CSCdi48581:  Add VIP2 support to ELC_branch
 * Branch: ELC_branch
 *
 * Revision 3.1  1995/12/27  20:19:47  getchell
 * Placeholder for VIPER development
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
#include "../ts/nonvol_x2444.h"
#include "../src-vip/vip_dbus.h"

#include "rvip_asic.h"
#include "rvip_dbus.h"
#include "rvip_pcmap.h"

/*
 * vip_x2444_io:
 * Manipulate the X2444 Serial EPROM
 */
static ushort vip_x2444_io (enum X2444_CMD cmd, int write_bits, int read_bits)
{
    int i;
    ushort value = 0;

    /*
     * Enable the X2444
     */
    IO_ASIC->cookie &= ~(X2444_SK | X2444_DI);	/* Clear clock and data  */
    IO_ASIC->cookie |= X2444_CE;               	/* Set chip enable       */
    flush_io_wbuffer();                         /* Flush write buffering */
    wastetime(X2444_DELAY);	             	/* And wait for the chip */

    /*
     * Give the command to the X2444
     */
    for (i = 0; i < write_bits; i++) {
	if (cmd & 0x1) {
	    IO_ASIC->cookie |= X2444_DI;    	/* Set the data in bit   */
	} else {
	    IO_ASIC->cookie &= ~X2444_DI;   	/* Clear the data in bit */
	}
	flush_io_wbuffer();                     /* Flush write buffer    */
	wastetime(X2444_DELAY);		
	IO_ASIC->cookie |= X2444_SK;           	/* Set the clock bit     */
	flush_io_wbuffer();                     /* Flush write buffer    */
	wastetime(X2444_DELAY);		
	IO_ASIC->cookie &= ~X2444_SK;          	/* Reset the clock bit   */
	flush_io_wbuffer();                     /* Flush write buffer    */
	wastetime(X2444_DELAY);		
	cmd >>= 1;		             	/* Shift right one bit   */
    }

    IO_ASIC->cookie |= X2444_DI;             	/* Set data in for read  */
    flush_io_wbuffer();                         /* Flush write buffer    */
    wastetime(X2444_DELAY);			

    /*
     * Read the response from the X2444
     */
    for (i = 0; i < read_bits; i++) {
	if (IO_ASIC->cookie & X2444_DO)
	    value |= (1 << i);
	IO_ASIC->cookie |= X2444_SK;         	/* Set the clock bit      */
	flush_io_wbuffer();                     /* Flush write buffer     */
	wastetime(X2444_DELAY);		
	IO_ASIC->cookie &= ~X2444_SK;       	/* Clear the clock bit    */
	flush_io_wbuffer();                     /* Flush write buffer     */
	wastetime(X2444_DELAY);		
    }

    /*
     * Disable the X2444
     */
    wastetime(X2444_DELAY);
    IO_ASIC->cookie &= ~X2444_CE;            	/* Clear chip enable      */
    flush_io_wbuffer();                         /* Flush write buffer     */

    return(value);
}

/*
 * install_rvip_dbus_handler :
 * Install the RVIP DBUS interrupt handler
 */
void install_rvip_dbus_handler (void)
{
    ushort *ptr;
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
    vip_x2444_io(X2444_CMD_RCL, 8, 0);		/* recall data to x2444 ram */
    wastetime(1000);				/* blow away some time */
    ptr = (ushort *)(&dbus_cntl.r.regs.eeprom);
    for (i=0; i<16; i++) {
	*ptr++ = vip_x2444_io(X2444_CMD_READ | (i * 2), 8, 16);
    }

    /*
     * Set COLD_RESET as the reason in the reset_reg. This
     * will force the monitor to do a full initialization
     * if the RP ever resets us (which will require the ucode 
     * to be downloaded again)
     */
    dbus_cntl.dbus_reg->reset_reg = COLD_RESET;
}

/* end of file */



