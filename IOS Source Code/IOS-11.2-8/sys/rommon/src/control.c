/* $Id: control.c,v 3.4.4.1 1996/03/21 23:26:49 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/rommon/src/control.c,v $
 *------------------------------------------------------------------
 * control.c
 *
 * Oct, 95, Steve J. Zhang
 *
 * Copyright (c) 1995 - 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: control.c,v $
 * Revision 3.4.4.1  1996/03/21  23:26:49  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.2.2.3  1996/03/19  01:00:57  mbeesley
 * Branch: ELC_branch
 * Sync to V111_1_3
 *
 * Revision 3.2.2.2  1996/02/08  08:57:50  gstovall
 * Branch: ELC_branch
 * Sync to ELC_baseline_960205.
 *
 * Revision 3.2.2.1  1995/12/01  04:20:00  mbeesley
 * Branch: ELC_branch
 * Reorganize Rom monitor header files to make it easier to port
 * to new platforms.
 *
 * Revision 3.4  1996/03/11  05:59:51  tkam
 * CSCdi51218:  Need to implement fault history mechanism
 * Added fault history support and misc. cleanup.
 *
 * Revision 3.3  1996/01/17  23:30:34  mansonw
 * CSCdi47058:  Fix the broken ROMMON build
 *
 * Revision 3.2  1995/11/17  18:41:19  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:05:00  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:23:41  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/10/06  23:11:14  szhang
 * placeholder for DBUS handlers in RSP2
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "monitor.h"
#include "dbus.h"
#include "dbstatus.h"

#include "signal_4k.h"
#include "monlib.h"
#include "mon_boot.h"

/* local static functions */
static void run_operational_code(void);

/* 
 * run_operational_code
 * Start the operational code.  
 * This calls kernel_init() to execute final initialization
 * of operational code.
 */
static void run_operational_code (void)
{
    struct pib pib;

    dbuscore_p->db.r.regs.status &= ~DBUS_RSP_READY;
    write_status();

#ifdef DEBUG
    printf("status = %b\n", dbuscore_p->db.r.regs.status);
    pib.mifp = (struct mon_iface *)0xa0002d18;
    pib.argc = 0;
    pib.argv = (char **)0;
#endif
    /* 
     * Setup physical and logical slot dbus registers.
     */

    if(dbuscore_p->db.start_addr == 0) {
	wcs_parity_error();
	internal_error();
    }

    /* cache_flush(0, -1); */

#ifdef DEBUG
    printf("Running system code.\n");
#endif
	/*
	 * Call kernel_init()
	 */
    DCL_ASIC->int_eoi = INT_LEV6;

    launch(2, (struct plb *)0, (struct pib *)0, dbuscore_p->db.start_addr); 
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
	    write_status();
	    /* do nothing as ROM code is running anyway */
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
	/* do nothing as the ROM code is running */
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
	status_rsp_not_ready(TRUE); 
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













