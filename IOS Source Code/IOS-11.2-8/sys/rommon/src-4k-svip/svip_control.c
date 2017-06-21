/* $Id: svip_control.c,v 1.1.6.1 1996/07/08 22:43:02 mwu Exp $
 * $Source: /release/112/cvs/Xsys/rommon/src-4k-svip/svip_control.c,v $
 *------------------------------------------------------------------
 * svip_control.c 
 *
 * Mar, 96. Paul Greenfield
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: svip_control.c,v $
 * Revision 1.1.6.1  1996/07/08  22:43:02  mwu
 * CSCdi60922:  11.2 Core Features Commit
 * Branch: California_branch
 *
 * Revision 1.1  1996/03/23  01:22:57  pgreenfi
 * Placeholder files for src-4k-svip.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "monitor.h"
#include "dbus.h"
#include "dbstatus.h"

/* local static functions */
static void run_operational_code(void);

/* 
 * stop_operational_code
 * Disable interrupts and return to dbus polling loop. 
 * This function does not return.
 */
void stop_operational_code (void)  
{
/* DDDMARK 
    disable_interrupts();
    poll_cause();  */ /* Jump to dbus polling loop, this will 
		       not return. This is not a function, but
		       an assembler tag. */
}

/* 
 * run_operational_code
 * Start the operational code.  
 * This calls kernel_init() to execute final initialization
 * of operational code.
 */
static void run_operational_code (void)
{
	static int (*jump_xstart)() = (int (*)())0xbfc00000;

    /* 
     * Setup physical and logical slot dbus registers.
     */
/*
    dbuscore_p->physical_slot = db_hw->physical_slot & PHYSICAL_SLOT_MASK;
    dbuscore_p->logical_slot = db_hw->logical_slot & LOGICAL_SLOT_MASK;
*/
    if(dbuscore_p->db.start_addr == 0) {
	wcs_parity_error();
	internal_error();
	return;
    }
	status_rsp_not_ready(TRUE); 
/* DDDMARK
	clear_dram_line();
*/
	/*
	 * Call kernel_init()
	 */
        cache_flush(0, -1);

	((void (*)(void) )dbuscore_p->db.start_addr)();
    /*
     * If the system code use 'return', dbus handler 
     * restart the rom monitor code.
     */
        jump_xstart();     
}

/*
 * control
 * Processes dbus requests over the dbus to register 5, control.
 * Implements fsm to determine when to start or stop the 
 * operational code.
 */
void control (data_port_type *local_dp)
{
    boolean started;

    /* 
     * Find out whether or not the operational code is currently
     * running and set a boolean flag so that we will know later.
     */
    if ((dbuscore_p->db.r.regs.ctrl & CNTL_MASTER_ENABLE) &&
	(dbuscore_p->db.r.regs.ctrl & CNTL_WCS_LOADED )   &&
	!(dbuscore_p->db.r.regs.ctrl & CNTL_WCS_CMD_ENABLE) ){
	started = TRUE;
    } else {
	started = FALSE;
    }

    /* 
     * Grab the new control register from the dbus
     */
    dbuscore_p->db.r.regs.ctrl = local_dp->dbus_data; 

    /*
     * Set the Enable LED according to the value in the control register.
     */
/*
    if( dbuscore_p->db.r.regs.ctrl & CNTL_LED) {
	enable_led(LED_ENABLED);
    } else {
	disable_led(LED_ENABLED);
    }
*/	

    /* master_enable = 0  ||
     * wcs_loaded    = 0  ||
     * cmd_enable    = 1, then
     * We should be stopped
     */
    if (!(dbuscore_p->db.r.regs.ctrl & CNTL_MASTER_ENABLE) ||
	!(dbuscore_p->db.r.regs.ctrl & CNTL_WCS_LOADED )   ||
	(dbuscore_p->db.r.regs.ctrl & CNTL_WCS_CMD_ENABLE) ) {
	/* 
	 * If we are not stopped, then stop us
	 */
	if (dbuscore_p->db.cstate != stopped ) {
	    dbuscore_p->db.cstate = stopped;
	    dbuscore_p->db.wstate = awaiting_wcs_opcode;
/* 	    write_status(); */
	    stop_operational_code();
	}
    }

    /* command_enable = 0 &&
     * master_enable  = 0 &&
     * wcs_loaded     = 0, then load
     * operational code from local rom  
     * This card does not do anything when given this command.
     */
    if (!(dbuscore_p->db.r.regs.ctrl & CNTL_MASTER_ENABLE) &&
	!(dbuscore_p->db.r.regs.ctrl & CNTL_WCS_CMD_ENABLE) &&
	!(dbuscore_p->db.r.regs.ctrl & CNTL_WCS_LOADED ) )  {
	dbuscore_p->db.r.regs.ctrl |= CNTL_WCS_LOADING;   /* We are loading
						 code from ROM */
	/* DDDMARK
	load_operational_code_from_rom();  */ /* NOP call */
	/* 
	 * Turn on LOADED and turn off LOADING bits
	 */
	dbuscore_p->db.r.regs.ctrl |= CNTL_WCS_LOADED;
	dbuscore_p->db.r.regs.ctrl &= ~((uchar) CNTL_WCS_LOADING);  
	stop_operational_code();
    }

    /* master_enable = 1  &&
     * wcs_loaded    = 1  &&
     * cmd_enable    = 0, then
     * Start up the operational code 
     */
    else if ((dbuscore_p->db.r.regs.ctrl & CNTL_MASTER_ENABLE) &&
	     (dbuscore_p->db.r.regs.ctrl & CNTL_WCS_LOADED )   &&
	     !(dbuscore_p->db.r.regs.ctrl & CNTL_WCS_CMD_ENABLE) ){

	if(started == TRUE) {
	    return;  /* Do not start ucode if it is already running */
	}

	/* 
	 * Before starting kernel_init, turn off the cip_ready bit.
	 * This lets the rp know that we are not ready for dbus 
	 * commands.  We will turn it back on when the operational
	 * code initialization completes.
	 */

	dbuscore_p->db.cstate = running;
	run_operational_code();  /* call kernel_init() */
	dbuscore_p->db.cstate = stopped;  /* If we get to this
						point, there was an
						error and the
						operational code did
						not get started. */
    }

    /* command_enable = 1
     * Go to command_enabled state, await WCS commands.
     */
    else if (dbuscore_p->db.r.regs.ctrl & CNTL_WCS_CMD_ENABLE ) {
	dbuscore_p->db.cstate = cmd_enabled;      
    }
}
