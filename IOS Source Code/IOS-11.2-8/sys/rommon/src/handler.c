/* $Id: handler.c,v 3.3.30.3 1996/07/08 22:42:39 mwu Exp $
 * $Source: /release/112/cvs/Xsys/rommon/src/handler.c,v $
 *------------------------------------------------------------------
 * handler.c
 *
 * Oct, 95, Steve J. Zhang
 *
 * Copyright (c) 1995 - 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: handler.c,v $
 * Revision 3.3.30.3  1996/07/08  22:42:39  mwu
 * CSCdi60922:  11.2 Core Features Commit
 * Branch: California_branch
 *
 * Revision 3.3.30.2  1996/03/29  03:25:59  tkam
 * CSCdi52192:  ROMs forget crash details.
 * Branch: California_branch
 * Changed dbus initialization and system_returned() .
 *
 * Revision 3.3.30.1  1996/03/21  23:27:17  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.2.2.2  1996/02/08  08:58:04  gstovall
 * Branch: ELC_branch
 * Sync to ELC_baseline_960205.
 *
 * Revision 3.2.2.1  1995/12/01  04:20:24  mbeesley
 * Branch: ELC_branch
 * Reorganize Rom monitor header files to make it easier to port
 * to new platforms.
 *
 * Revision 3.3  1996/01/17  23:31:18  mansonw
 * CSCdi47058:  Fix the broken ROMMON build
 *
 * Revision 3.2  1995/11/17  18:42:06  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:05:43  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:23:49  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/10/06  23:11:24  szhang
 * placeholder for DBUS handlers in RSP2
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "monitor.h"
#include "signal_4k.h"
#include "dbus.h"
#include "dbstatus.h"
#include "command.h"
#if defined(RSP2)
#include "dclasic.h"
#endif

/*
 * Clear the command register in dbuscore. 
 */
static inline void clear_command (void)
{ 
    dbuscore_p->db.r.regs.cmd  = 0;
}

/*
 * Clear the control register in dbuscore. 
 */
static inline void clear_control (void)
{ 
    dbuscore_p->db.r.regs.ctrl = 0;
}

/*
 * dbus_handler()
 * Interrupt handler to process dbus commands from the RP.
 * 
 * The following commands won't generate interrupts:
 *   0x00      RO -- Physical Slot
 *   0x01      RO -- Last command/Address
 *   0x02      RW -- Read, status reg, write, board reset
 *   0x03      NA -- Not Accessible
 *   0x04      NA -- Not Accessible *   
 *  All other commands are handled in software and result in an
 *  interrupt to the processor.  The ones that we care about are:
 *   0x05      RW -- Control
 *   0x07      RW -- Microcode command/data
 *   0x08      RW -- Port Status
 *   0x09      RW -- Console Port
 *   0x0a      RW -- Debug   Port
 *   0x0b      RW -- Logger  Port
 *   0x0c-0x1f NA -- Not Accessible 
 *   0x20-0x3f NA -- Not Accessible
 *
 *  Access to non accsssible register will result in
 *  timeout and interrupt generate
 */
void dbus_handler ()
{
    uchar addrs;
    union {
	io_ports bits;
	uchar    all;
    } io_port_temp;
long retval;

    /* Get dbus command from Hardware.  Cast is necessary to keep the
       compiler from calling memcpy to copy contents of the data_port structure. */

    *((ushort *)&dbuscore_p->db.data_port) = *((ushort *)&db_hw->data_port); 
   
    /* 
     * If we did not respond to the command within 20usec, then report an
     * error and exit.
     */
    if( dbuscore_p->db.data_port.ad.addrs & PORT_CMD_FAIL) {
	/* The hardware already reported an internal error for us so
	   we should not respond to this request. We should clear the
	   error indication so that we have a chance to process
	   the next request */
	read_status();  /* Reading status clears error */
	internal_error(); /* Indicate error */
#if defined(RSP2)
	DCL_ASIC->int_eoi = INT_LEV6;
#endif
	return;
    } 

    addrs = dbus_addrs(dbuscore_p->db.data_port.ad.addrs);

    /* 
     * If this is a read, then return the approriate byte from the local
     * register structure and exit.
     * dbuscore_p->db.regs is a structure that is equivalent to a byte array of all
     *  of the dbus regs.  The address passed in through the data port serves
     *  as an index into this array.  The return byte is written into
     * the low order byte of the HW data_port register.
     */

    if(dbuscore_p->db.data_port.ad.addrs & PORT_DBUS_READ) {
	switch(addrs) {
	case DB_CONSOLE:
	    read_port(CONSOLE_PORT);
	    break;

	case DB_DEBUG:
	    read_port(DEBUG_PORT);
	    break;

	case DB_LOGGER:
	    read_port(LOGGER_PORT);
	    break; 

	case DB_MICROCODE:
	    ucode_command_data();  /* places response in db.r.regs.cmd */
	                           /* fall through to default to output data */
        default:
	/* command is for DB_CONTROL or undefined */
	/* just return the register value at that location */
	    write_data(dbuscore_p->db.r.r_array[addrs]);
	}
#if defined(RSP2)
	DCL_ASIC->int_eoi = INT_LEV6;
#endif
	return; /* read is finished, get out of here */
    }
    
    /*
     * Otherwise, this is a write from the DBUS.  Call the appropriate
     * routine based on the DBUS register. 
     */
#if defined(SVIP)
    write_status();
#endif
    switch (addrs) {
    case DB_CONTROL:
	control( &dbuscore_p->db.data_port);
	break;

    case DB_MICROCODE:
	ucode_command_data();
	break;
	
    case DB_PORT_STATUS:   /* RP can only set console/debug active bits */
	io_port_temp.all = dbuscore_p->db.data_port.dbus_data;

	dbuscore_p->db.r.regs.port_status.console_act = 
	    io_port_temp.bits.console_act;

	dbuscore_p->db.r.regs.port_status.debug_act = 
	    io_port_temp.bits.debug_act;
	break;

    case DB_CONSOLE:
	write_port(CONSOLE_PORT);
	break;

    case DB_DEBUG:
	write_port(DEBUG_PORT);
	break;

    case DB_LOGGER:          /* Master cannot write to logger port */
	db_hw->data_port.ad.data_out = BAD_DATA;   /* return bad data */
	break;

    default:
	/* command is undefined */
	dbuscore_p->db.r.r_array[addrs] = dbuscore_p->db.data_port.dbus_data;
    }

#if defined(RSP2)
    DCL_ASIC->int_eoi = INT_LEV6;
#endif
}

/* 
 * Initializes dbuscore variables after reset and at end of 
 * operational code initialization.
 */
void dbuscore_initialization (void)
{
    /* Read current eeprom values into data_array */
    /* read_2444_data((ushort *) (dbuscore_p->db.r.regs.eeprom)); */
}

/*
 * Initialization that takes place after a dbus reset.
 */
void dbus_early_init (void)
{
    dbuscore_initialization();

    /* Clear out the message queues */
    dbus_init();

    /* Clear dbus registers */
    clear_status();  
    clear_control();
    clear_command();

    /* Initialize some state variable */
    dbuscore_p->db.cstate = stopped;
    dbuscore_p->db.wstate  = awaiting_wcs_opcode;
#if defined(RSP2)
    status_rsp_ready(FALSE);
#endif
#if defined(SVIP)
    status_rsp_ready(TRUE);
#endif
}
