/* $Id: serial_parser.c,v 3.6.8.6 1996/09/10 00:52:47 snyder Exp $
 * $Source: /release/112/cvs/Xsys/wan/serial_parser.c,v $
 *------------------------------------------------------------------
 * serial_parser.c -- common serial interface parser routines
 *
 * Sept 1995, Greg Stovall
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: serial_parser.c,v $
 * Revision 3.6.8.6  1996/09/10  00:52:47  snyder
 * CSCdi68568:  more constant opportunities, save memory
 *              156 out of image,
 *              116 out of data section
 * Branch: California_branch
 *
 * Revision 3.6.8.5  1996/06/17  08:36:55  jhernand
 * Commit Modular Cobra to California_branch
 *
 * Revision 3.6.8.4  1996/05/17  20:50:20  wmay
 * CSCdi57863:  Error message not displayed during encap command - reenable
 * interrupts for printf error output for config commands
 * Branch: California_branch
 *
 * Revision 3.6.8.3  1996/04/27  06:38:17  syiu
 * Branch: California_branch
 * Commit Dial1_branch
 *
 * Revision 3.2.20.3  1996/04/26  00:00:34  syiu
 * Branch: Dial1_branch
 * Dial1_branch to IosSec_postmerge_960419
 *
 * Revision 3.2.20.2  1996/04/17  00:02:24  syiu
 * Branch: Dial1_branch
 * Sync Dial1_branch to V111_1_3
 *
 * Revision 3.2.20.1  1996/02/23  21:32:00  lbustini
 * Branch: Dial1_branch
 * Move dialer fields out of the idb.
 *
 * Revision 3.6.8.2  1996/03/22  00:15:23  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.6.8.1  1996/03/18  22:48:39  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.2.3  1996/03/18  23:57:50  mbeesley
 * Branch: ELC_branch
 * Sync to V111_1_3
 *
 * Revision 3.2.2.2  1996/02/08  10:01:54  gstovall
 * Branch: ELC_branch
 * Sync to ELC_baseline_960205.
 *
 * Revision 3.2.2.1  1996/01/26  02:51:38  gstovall
 * Branch: ELC_branch
 * Oodles of fixes and enhancements:
 *         - Added support for the crc command to the predator 4T
 *         - Added support for the ignore-dcd command to the predator 4T
 *         - Do not keep stats on a VIP 4T
 *         - Pulled in a needed bugfix (CSCdi47390)
 *         - Revived the show controller command, it should work correctly
 *           with FDDI and token ring now.
 *
 * Revision 3.6  1996/02/28  21:45:26  rbatz
 * CSCdi50239: Commands "bsc fdx" and "sdlc hdx" not correctly
 *             translated to "full-duplex" and "half-duplex".
 *
 * Revision 3.5  1996/02/27  18:44:39  rbatz
 * CSCdi49933:  Command "full-duplex" displayed incorrectly as
 *              "full_duplex".  Also, make 11.0, 11.1 "show int"
 *              for half-duplex the same.
 *
 * Revision 3.4  1996/02/13  21:27:03  rbatz
 * CSCdi30258:  Fixed the following:
 *                  Duplicate half-duplex commands.
 *                  Could not do half-duplex stun.
 *                  Could not tune half-duplex timers for bstun.
 *
 * Revision 3.3  1996/01/29  18:47:38  rharagan
 * CSCdi47442:  Router crashes  at address 0x4AFC4AFF
 * Fix crash in cbus_interrupt() if packet received while changing serial
 * encaps
 *
 * Revision 3.2.26.1  1996/01/24  23:32:09  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.2  1995/11/17  00:50:37  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:50:59  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/10/17  23:06:47  gstovall
 * Placeholders for Talisker_branch.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "interface_private.h"
#include "serial.h"
#include "serial_private.h"
#include "config.h"
#include "sys_registry.h"
#include "media_registry.h"
#include "packet.h"
#include "dialer.h"
#include "dialer_registry.h"
#include "../if/network.h"
#include "../parser/parser_defs_parser.h"
#include "../parser/parser_defs_half_duplex.h"
#include "../parser/parser_defs_priority.h"
#include "../util/itemlist.h"
#include "../if/priority_private.h"
#include "serial_parser.h"

static const char generic_only_msg[] = 
    "\nThis command applies only to %s interfaces";
static const char hdx_only_msg[] = "half-duplex";
static const char dce_only_msg[] = "DCE";
static const char dte_only_msg[] = "DTE";

static const char generic_unsupport_msg[] =
    "\nThis command is not supported on this interface";

static ulong send_state_config (hwidbtype *hwidb, ulong opcode,
				ushort item_type, ulong value)
{
    ONE_ITEM_LIST(tmp, item_type, sizeof(ulong), (void *)value);

    /*
     * Send the request
     */
    (*hwidb->state_config)(hwidb, opcode, &tmp_list);

    return(tmp_item.u.l_value);
}


/*
 * serial_setencap
 * Set serial encapsulation if we are permitted
 */
void serial_setencap (parseinfo *csb, idbtype *idb, int enctype,
		      boolean output)
{
    int oldenctype;
    hwidbtype *hwidb;
    leveltype level;
    boolean check;
    hwidb = idb->hwptr;

    /* 
     * Check whether fancy queuing was supported on the interface
     * before changing the encapsulation
     */
    check = fair_queue_supported(hwidb);
   
    /*
     * If we're a member of a hunt group, do not allow encapsulation changes
     */
    if (reg_invoke_dialer_huntgroup_member(hwidb)) {
	printf("\nCannot change encapsulation of hunt group member");
	return;
    }

    if (is_ddr(hwidb) && !dialer_encap(enctype)) {
	printf("\nEncapsulation not allowed on dialer interfaces");
	return;
    }
    oldenctype = hwidb->enctype;

    level = raise_interrupt_level(NETS_DISABLE);
    
    if (!reg_used_media_serial_setup(enctype)) {
	if (output) {
	    reset_interrupt_level(level);
	    printf("\nEncapsulation %s not supported by this software",
		   encstring(enctype));
	    level = raise_interrupt_level(NETS_DISABLE);
	}
	if (oldenctype == enctype)
	    oldenctype = hwidb->enctype_def;
	serial_setencap(NULL, idb, oldenctype, FALSE);
	reset_interrupt_level(level);
	return;
    }
    
    hwidb->enctype = enctype;
    set_subif_link_type(idb);	/* It might have changed! */
    
    /*
     * Reset the fastswitching flavor. The new encapsulation is responsible
     * for setting this up.
     */
    hwidb->fast_switch_type = FS_UNKNOWN;

     /*
      * Certain serial protocol encapsulations don't play well with
      * Weighted Fair Queueing, and disable it by setting queueing to
      * FCFS as part of their configuration (i.e. X25).  The result was
      * that when another encapsulation which could utilize WFQ was
      * configured, it would be necessary to _manually_ enable WFQ for
      * that interface.  (This was seen on autoinstall_over_serial()).
      * The fix is to enable possibility of WFQ, and allow
      * media_serial_setup() to reset queueing algorithm as
      * appropriate...
      */
   
    if (fair_queue_supported(hwidb)) {
	/* 
	 * If the queuing algorithm on the interface is FIFO by
	 * default, change it to default wfq since the changed
	 * encapsulation now supports fancy queuing. Don't set queuing
	 * algorithm to default (wfq) if fancy queuing algorithm is
	 * already enabled on that interface.  
	 */
	if ((hwidb->queue_algorithm == QUEUE_FCFS) && !check )
 	    hwidb->queue_algorithm = QUEUE_FAIR_DEF;
    } else { 
	    /*
	     * Changed encapsulation doesn't support fancy queuing. Set
	     * queing_algorithm to default FIFO.
	     */ 
	    hwidb->queue_algorithm = QUEUE_FCFS;
    }

    if (!reg_invoke_media_serial_setup(enctype, csb, hwidb)) {
	if (output) {
	    reset_interrupt_level(level);
	    printf("\nError in encaps setup.  Encapsulation not changed.");
	    level = raise_interrupt_level(NETS_DISABLE);
	}
	if (oldenctype == enctype)
	    oldenctype = hwidb->enctype_def;
	serial_setencap(NULL, idb, oldenctype, FALSE);
	reset_interrupt_level(level);
	return;
    }
    reset_interrupt_level(level);
}


/*
 * serial_encapsulation
 * Set encapsulation type for a serial interface
 */
void serial_encapsulation (parseinfo *csb)
{
    idbtype *swidb = csb->interface;
    hwidbtype *hwidb;

    hwidb = swidb->hwptr;

    if (!csb->sense) {
	serial_setencap(csb, swidb, hwidb->enctype_def, TRUE);
	reg_invoke_media_serial_encap_changed(hwidb, hwidb->enctype_def);
    } else {
	serial_setencap(csb, swidb, GETOBJ(int,1), TRUE);
	reg_invoke_media_serial_encap_changed(hwidb, GETOBJ(int,1));
    }
}


/*
 * downloop_command - (un)set flag to force protocol line down when
 * loopback condition is detected.
 */
void serial_downloop_command (parseinfo *csb)
{
    hwidbtype *hwidb = csb->interface->hwptr;
    boolean downiflooped;
    
    /*
     * Get the current state
     */
    downiflooped = send_state_config(hwidb, IDB_CONTROL_READ_CONFIG,
				     SERIAL_HW_DOWNIFLOOPED, 0);

    if ((hwidb->enctype != ET_HDLC) && (hwidb->enctype != ET_PPP) &&
	(csb->resolvemethod == RES_MANUAL)) {
        printf("\n%%Encapsulation must be HDLC or PPP");
	
	downiflooped = send_state_config(hwidb, IDB_CONTROL_SET_CONFIG,
					 SERIAL_HW_DOWNIFLOOPED, FALSE);
        return;
    }

    if (csb->nvgen) {
	nv_write(downiflooped, csb->nv_command);
	return;
    }

    downiflooped = send_state_config(hwidb, IDB_CONTROL_SET_CONFIG,
				     SERIAL_HW_DOWNIFLOOPED, csb->sense);
}


/*
 * serial_clockrate_command
 *
 * Allow the user to set the clockrate for a given serial interface
 */
void serial_clockrate_command (parseinfo *csb)
{
    send_state_config(csb->interface->hwptr, IDB_CONTROL_SET_CONFIG,
		      SERIAL_HW_SET_CLOCKRATE, (ulong)csb);
}


/*
 * clocksource_command
 *
 * Allow the user to set the clocsource for a G.703 serial interface
 */
void serial_clocksource_command (parseinfo *csb)
{
    ushort internal = 0; /* clock from line is default */
    hwidbtype *hwidb;
    boolean clocksource_enabled;
    
    hwidb = csb->interface->hwptr;

    /*
     * Get the current state
     */
    clocksource_enabled = send_state_config(hwidb, IDB_CONTROL_READ_CONFIG,
					    SERIAL_HW_CLOCKSOURCE_ENABLED, 0);
    
    if (csb->nvgen) {
        nv_write(clocksource_enabled, "clock source internal");
        return;
    }
    
    if ((GETOBJ(int,1) ==  G703_INTERNAL) && csb->sense) 
	internal = 1;

    /*
     * Set the config
     */
    clocksource_enabled = send_state_config(hwidb, IDB_CONTROL_SET_CONFIG,
					    SERIAL_HW_CLOCKSOURCE_ENABLED,
					    internal);
    (*hwidb->reset)(hwidb);
}


/*
 * crc4_command
 *
 * Allow the user to enable crc4 for G.703 serial interface
 */
void serial_crc4_command (parseinfo *csb)
{
    hwidbtype *hwidb;
    boolean crc4_enabled;
    
    hwidb = csb->interface->hwptr;

    /*
     * Get the current status
     */
    crc4_enabled = send_state_config(hwidb, IDB_CONTROL_READ_CONFIG,
				     SERIAL_HW_CRC4_ENABLED, 0);

    if (csb->nvgen) {
        nv_write(crc4_enabled, "%s", csb->nv_command);
        return;
    }
    
    /*
     * Set the config
     */
    crc4_enabled = send_state_config(hwidb, IDB_CONTROL_SET_CONFIG,
				     SERIAL_HW_CRC4_ENABLED, csb->sense);
}


/*
 * timeslot16_command
 *
 * Allow the user to enable timeslot16 for a G.703 serial interface
 */
void serial_timeslot16_command (parseinfo *csb)
{
    hwidbtype *hwidb; 
    boolean timeslot16_enabled;

    hwidb = csb->interface->hwptr;

    /*
     * Get the current status
     */
    timeslot16_enabled = send_state_config(hwidb, IDB_CONTROL_READ_CONFIG,
					   SERIAL_HW_TIMESLOT16_ENABLED, 0);

    if (csb->nvgen) {
        nv_write(timeslot16_enabled, csb->nv_command);
        return;
    }

    /*
     * Set the config
     */
    timeslot16_enabled = send_state_config(hwidb, IDB_CONTROL_SET_CONFIG,
					   SERIAL_HW_TIMESLOT16_ENABLED,
					   csb->sense);
}


/*
 * timeslot_command
 *
 * Allow the user to enable timeslot for a G.703 serial interface
 */
void serial_timeslot_command (parseinfo *csb)
{
    hwidbtype *hwidb;
    ushort start_slot, stop_slot;
    serialsb *ssb;

    hwidb = csb->interface->hwptr;
    ssb = idb_use_hwsb_inline(hwidb, HWIDB_SB_SERIAL);

    if (csb->nvgen) {
        nv_write(ssb->start_slot, "%s %d-%d",
		 csb->nv_command, ssb->start_slot, ssb->stop_slot);
	idb_release_hwsb_inline(hwidb, HWIDB_SB_SERIAL);
        return;
    }

    /* setting the start_slot to zero disables framing */
    if (csb->sense) 
	start_slot = GETOBJ(int,1);
    else 
	start_slot = 0;

    stop_slot = GETOBJ(int,2);

    if (start_slot > stop_slot) {
	printf("\nStart slot must not be greater than stop slot.");
	printf("\nFraming left as: start slot %d, stop slot %d",
	       ssb->start_slot, ssb->stop_slot);
	idb_release_hwsb_inline(hwidb, HWIDB_SB_SERIAL);
	return;
    } else {
	ssb->start_slot = start_slot;
	ssb->stop_slot = stop_slot;
    }
    if (!start_slot) {
	printf("\nStart slot is set to zero, framing is disabled.");
    } 

    (*hwidb->reset)(hwidb);
    idb_release_hwsb_inline(hwidb, HWIDB_SB_SERIAL);
}


/*
 * serial_internal_transmit_clock_command
 *
 * Allow the user to set the terminal timing for a given serial interface
 */
void serial_internal_transmit_clock_command (parseinfo *csb)
{
    hwidbtype *hwidb; 
    boolean dce_tte;

    hwidb = csb->interface->hwptr;

    if ((hwidb->serial_flags & SERIAL_FLAGS_NO_DTE_INT_TX) &&
	hwidb->dte_interface) {
	if (csb->resolvemethod == RES_MANUAL) {
	    printf("\n%%Not allowed on DTE interface");
	}
	return;
    }

    /*
     * Get the current status
     */
    dce_tte = send_state_config(hwidb, IDB_CONTROL_READ_CONFIG,
				SERIAL_HW_DCE_TERMINAL_TIMING_ENABLE, 0);
    
    if (csb->nvgen) {
	nv_write(dce_tte, csb->nv_command);
	return;
    }

    /*
     * Set the config
     */
    dce_tte = send_state_config(hwidb, IDB_CONTROL_SET_CONFIG,
				SERIAL_HW_DCE_TERMINAL_TIMING_ENABLE,
				csb->sense);
}


/*
 * serial_invert_transmit_clock_command
 *
 * Allow the user to invert the transmit clock for a given serial interface
 */
void serial_invert_transmit_clock_command (parseinfo *csb)
{
    hwidbtype *hwidb;
    boolean invert_txc;

    hwidb = csb->interface->hwptr;

    if ((hwidb->serial_flags & SERIAL_FLAGS_NO_DCE_CLK_INV) &&
	!hwidb->dte_interface) {
	if (csb->resolvemethod == RES_MANUAL) {
	    printf("\n%%Not allowed on DCE interface");
	}
	return;
    }

    /*
     * Get the current status
     */
    invert_txc = send_state_config(hwidb, IDB_CONTROL_READ_CONFIG,
				   SERIAL_HW_INVERT_TXC, 0);

    if (csb->nvgen) {
	nv_write(invert_txc, csb->nv_command);
	return;
    }

    /*
     * Set the config
     */
    invert_txc = send_state_config(hwidb, IDB_CONTROL_SET_CONFIG,
				   SERIAL_HW_INVERT_TXC, csb->sense);
}


/*
 * serial_nzri_command
 *
 * Allow the user to set the NRZ/NZRI mode for a given serial interface
 */
void serial_nrzi_command (parseinfo *csb)
{
    hwidbtype *hwidb;
    boolean nrzi_enable;
  
    hwidb = csb->interface->hwptr;

    /*
     * Get the current status
     */
    nrzi_enable = send_state_config(hwidb, IDB_CONTROL_READ_CONFIG,
				    SERIAL_HW_NRZI_ENABLE, 0);

    if (csb->nvgen) {
	nv_write(nrzi_enable, csb->nv_command);
	return;
    }

    /*
     * Set the config
     */
    nrzi_enable = send_state_config(hwidb, IDB_CONTROL_SET_CONFIG,
				    SERIAL_HW_NRZI_ENABLE, csb->sense);
}


/*
 * serial_restart_delay_command
 */
void serial_restart_delay_command (parseinfo *csb)
{
    hwidbtype *hwidb;
    serialsb *ssb;
    
    hwidb = csb->interface->hwptr;
    ssb = idb_use_hwsb_inline(hwidb, HWIDB_SB_SERIAL);

    if (csb->nvgen) {
	nv_write((ssb->serial_restartdelay != ssb->serial_restartdelay_def),
		"%s %d", csb->nv_command, ssb->serial_restartdelay / ONESEC);
	idb_release_hwsb_inline(hwidb, HWIDB_SB_SERIAL);
	return;
    }

    if (ssb->serial_restartdelay_def == 0) {
	printf("\nCan't set restart-delay for this interface");
	idb_release_hwsb_inline(hwidb, HWIDB_SB_SERIAL);
	return;
    }
    
    if (!csb->sense) {
	/* reset to default */
	ssb->serial_restartdelay = ssb->serial_restartdelay_def;
	idb_release_hwsb_inline(hwidb, HWIDB_SB_SERIAL);
	return;
    }
    ssb->serial_restartdelay = GETOBJ(int,1);
    ssb->serial_restartdelay *= ONESEC;
    if ((ssb->serial_restartdelay == 0)
	&& (TIMER_RUNNING(ssb->serial_restarttimer))) {
	TIMER_STOP(ssb->serial_restarttimer);
    }

    idb_release_hwsb_inline(hwidb, HWIDB_SB_SERIAL);
}

/*
 * serial_rx_sync_disable_command
 *
 * Allow the user to disable the receive synchronizer (5IN1 applique)
 */
void serial_rx_sync_disable_command (parseinfo *csb)
{
    hwidbtype *hwidb;
    boolean rx_sync_disable;

    hwidb = csb->interface->hwptr;

    if (!(hwidb->serial_flags & SERIAL_FLAGS_CAN_DIS_RX_SYNC)) {
	if (csb->resolvemethod == RES_MANUAL) {
	    printf("\n%%Command not allowed on this interface");
	}
	return;
    }

    /*
     * Get the current status
     */
    rx_sync_disable = send_state_config(hwidb, IDB_CONTROL_READ_CONFIG,
					SERIAL_HW_RX_SYNC_DISABLE, 0);    

    if (csb->nvgen) {
        nv_write(rx_sync_disable, csb->nv_command);
        return;
    }

    /*
     * Set the config
     */
    rx_sync_disable = send_state_config(hwidb, IDB_CONTROL_SET_CONFIG,
					SERIAL_HW_RX_SYNC_DISABLE, csb->sense);
}


/*
 * idle_command
 * set the idle character transmitted between frames for serial interfaces
 */
void serial_idle_command (parseinfo *csb)
{
    hwidbtype *hwidb;
    int command;
    
    hwidb = csb->interface->hwptr;
    if (hwidb == NULL) 
      return;
    
    if (csb->nvgen) {
 	nv_write(hwidb->serial_flags & SERIAL_FLAGS_DO_ALT_FLAG, 
 		 "%s marks",csb->nv_command);
 	return;
    }
    
    hwidb->serial_flags &= ~SERIAL_FLAGS_DO_ALT_FLAG;
    command = GETOBJ(int,1);
    if (csb->sense && (command == IDLE_MARKS) )
      hwidb->serial_flags |= SERIAL_FLAGS_DO_ALT_FLAG;
    (*hwidb->reset)(hwidb);
}


/*
 * serial_ignore_dcd_command
 *
 * Allow the user to configure DSR instead of DCD as link up/down indicator.
 */
void serial_ignore_dcd_command (parseinfo *csb)
{
    hwidbtype *hwidb;
    boolean ignore_dcd;
    
    hwidb = csb->interface->hwptr;
    
    if (!(hwidb->serial_flags & SERIAL_FLAGS_CAN_IGNORE_DCD)) {
        if (csb->resolvemethod == RES_MANUAL)
            printf(generic_unsupport_msg);
        return;
    }
    if (!hwidb->dte_interface) {
        if (csb->resolvemethod == RES_MANUAL)
            printf(generic_only_msg, dte_only_msg);
        return;
    }

    /*
     * Get the current status
     */
    ignore_dcd = send_state_config(hwidb, IDB_CONTROL_READ_CONFIG,
				   SERIAL_HW_IGNORE_DCD, 0);
    
    if (csb->nvgen) {
        nv_write(ignore_dcd, csb->nv_command);
        return;
    }

    /*
     * Set the config
     */
    ignore_dcd = send_state_config(hwidb, IDB_CONTROL_SET_CONFIG,
				   SERIAL_HW_IGNORE_DCD, csb->sense);    

}


static void serial_ignore_loopback_command (parseinfo *csb)
{
    hwidbtype *hwidb;
    boolean ignore_loopback;
    
    hwidb = csb->interface->hwptr;
        
    if (csb->nvgen) {
        ignore_loopback = send_state_config(hwidb, IDB_CONTROL_READ_CONFIG,
					    SERIAL_HW_IGNORE_LOCAL_LOOPBACK,0);
        nv_write(ignore_loopback, csb->nv_command);
        return;
    }

    if (hwidb->dte_interface) {
      printf(generic_only_msg, dce_only_msg);
      return;
    }

    /*
     * Set the config
     */
    send_state_config(hwidb, IDB_CONTROL_SET_CONFIG,
		      SERIAL_HW_IGNORE_LOCAL_LOOPBACK,
		      csb->sense);    
}


void serial_ignore_hw_command (parseinfo *csb)
{
  switch(GETOBJ(int,1)) {
    case IGNORE_LOCAL_LOOPBACK:
      serial_ignore_loopback_command(csb);
      break;
    default:
      break;
  }
}


/*
 * crc_command  
 * Parsing for 32-bit Ethernet CRC's on serial interfaces.
 */
void serial_crc_command (parseinfo *csb)
{
    hwidbtype *hwidb;
    boolean crc_32_enabled;
    boolean new_crc_32_enabled;

    hwidb = csb->interface->hwptr;

    /*
     * Get the current value
     */
    crc_32_enabled = send_state_config(hwidb, IDB_CONTROL_READ_CONFIG,
				       SERIAL_HW_CRC_32_ENABLED, 0);

    if (csb->nvgen) {
        nv_write((GETOBJ(int,1) == 32) && crc_32_enabled,
                  "%s", csb->nv_command);
        return;
    }
    if (csb->sense && (GETOBJ(int,1) == 16))
        csb->sense = 0;  /* Send controller default CRC of 16 */

    if (csb->sense && (GETOBJ(int,1) == 32)) {
        new_crc_32_enabled = TRUE;
    } else {
        new_crc_32_enabled = FALSE;
    }

    /*
     * Set the config
     */
    crc_32_enabled = send_state_config(hwidb, IDB_CONTROL_SET_CONFIG,
				       SERIAL_HW_CRC_32_ENABLED,
				       new_crc_32_enabled);    

    (*hwidb->reset)(hwidb);
    if (hwidb->counters.resets > 0)
	hwidb->counters.resets--;
}


/*
 * Functions to support serial half-duplex and related parameters
 */

/*
 * serial_half_duplex_reset_params
 */
static void serial_half_duplex_reset_params (hwidbtype *hwidb, boolean hdx)
{
    if (hdx) {
        hwidb->hdx_flags |= HDX_FLAG_ON;
    }
    else {
        hwidb->hdx_flags &= ~HDX_FLAG_ON;
    }

    send_state_config(hwidb, IDB_CONTROL_SET_CONFIG,
                      SERIAL_HW_CONTROLLED_CARRIER, FALSE);
   
    send_state_config(hwidb, IDB_CONTROL_SET_CONFIG,
                      SERIAL_HW_CTS_DELAY, HDX_CTS_DELAY_DEFAULT);

    send_state_config(hwidb, IDB_CONTROL_SET_CONFIG,
                      SERIAL_HW_RTS_TIMEOUT, HDX_RTS_TIMEOUT_DEFAULT);

    send_state_config(hwidb, IDB_CONTROL_SET_CONFIG,
                      SERIAL_HW_RTS_DROP_DELAY, HDX_RTS_DROP_DELAY_DEFAULT);

    send_state_config(hwidb, IDB_CONTROL_SET_CONFIG,
                      SERIAL_HW_CTS_DROP_TIMEOUT, 
                      HDX_CTS_DROP_TIMEOUT_DEFAULT);

    send_state_config(hwidb, IDB_CONTROL_SET_CONFIG,
                      SERIAL_HW_DCD_TXSTART_DELAY, 
                      HDX_DCD_TXSTART_DELAY_DEFAULT);

    send_state_config(hwidb, IDB_CONTROL_SET_CONFIG,
                      SERIAL_HW_DCD_DROP_DELAY, HDX_DCD_DROP_DELAY_DEFAULT);

    send_state_config(hwidb, IDB_CONTROL_SET_CONFIG,
                      SERIAL_HW_TRANSMIT_DELAY, HDX_TRANSMIT_DELAY_DEFAULT);

    send_state_config(hwidb, IDB_CONTROL_SET_CONFIG,
                      SERIAL_HW_HDX_LOG_ENABLE, FALSE);
}

/*
 * serial_half_duplex_nvgens
 */
static void serial_half_duplex_nvgens (hwidbtype *hwidb)
{
    boolean half_duplex;
    boolean flag;
    ulong timer_val;

    /* Determine if we are currently in half-duplex mode */
    half_duplex = hwidb->hdx_flags & HDX_FLAG_ON;

    /* Now write it out */
    if (hwidb->enctype == ET_BSTUN) {
        /*
         * BSTUN default is half-duplex
         */
        nv_write(!half_duplex, "full-duplex");
    }
    else {
        nv_write(half_duplex,  "half-duplex");
    }

    if (half_duplex && !hwidb->dte_interface) {
        flag = send_state_config(hwidb, IDB_CONTROL_READ_CONFIG,
                                 SERIAL_HW_CONTROLLED_CARRIER, 0);
        nv_write(flag, "half-duplex controlled-carrier");
    }

    if (half_duplex && hwidb->dte_interface) {
        timer_val = send_state_config(hwidb, IDB_CONTROL_READ_CONFIG,
                                      SERIAL_HW_RTS_TIMEOUT, 0);
        nv_write((timer_val != HDX_RTS_TIMEOUT_DEFAULT), 
                 "half-duplex timer rts-timeout %d", timer_val);
    }

    if (half_duplex && !hwidb->dte_interface) {
        timer_val = send_state_config(hwidb, IDB_CONTROL_READ_CONFIG,
                                      SERIAL_HW_CTS_DELAY, 0);
        nv_write((timer_val != HDX_CTS_DELAY_DEFAULT),
                 "half-duplex timer cts-delay %d", timer_val);
    }

    if (half_duplex && hwidb->dte_interface) {
        timer_val = send_state_config(hwidb, IDB_CONTROL_READ_CONFIG,
                                      SERIAL_HW_RTS_DROP_DELAY, 0);
        nv_write((timer_val != HDX_RTS_DROP_DELAY_DEFAULT),
                 "half-duplex timer rts-drop-delay %d", timer_val);
    }

    if (half_duplex && hwidb->dte_interface) {
        timer_val = send_state_config(hwidb, IDB_CONTROL_READ_CONFIG,
                                      SERIAL_HW_CTS_DROP_TIMEOUT, 0);
        nv_write((timer_val != HDX_CTS_DROP_TIMEOUT_DEFAULT),
                 "half-duplex timer cts-drop-timeout %d", timer_val);
    }

    if (half_duplex && !hwidb->dte_interface) {
        timer_val = send_state_config(hwidb, IDB_CONTROL_READ_CONFIG,
                                      SERIAL_HW_DCD_TXSTART_DELAY, 0);
        nv_write((timer_val != HDX_DCD_TXSTART_DELAY_DEFAULT),
                "half-duplex timer dcd-txstart-delay %d", timer_val);
    }

    if (half_duplex && !hwidb->dte_interface) {
        timer_val = send_state_config(hwidb, IDB_CONTROL_READ_CONFIG,
                                      SERIAL_HW_DCD_DROP_DELAY, 0);
        nv_write((timer_val != HDX_DCD_DROP_DELAY_DEFAULT),
                "half-duplex timer dcd-drop-delay %d", timer_val);
    }

    if (half_duplex) {
        timer_val = send_state_config(hwidb, IDB_CONTROL_READ_CONFIG,
                                      SERIAL_HW_TRANSMIT_DELAY, 0);
        nv_write((timer_val != HDX_TRANSMIT_DELAY_DEFAULT),
                 "half-duplex timer transmit-delay %d", timer_val);
    }

    if (half_duplex) {
        flag = send_state_config(hwidb, IDB_CONTROL_READ_CONFIG,
                                 SERIAL_HW_HDX_LOG_ENABLE, 0);
        nv_write(flag, "half-duplex log enable");
    }

    return;
}

/*
 * serial_set_half_duplex()
 */
static void serial_set_half_duplex (parseinfo *csb)
{
    hwidbtype *hwidb = csb->interface->hwptr;
    boolean saved_role;
    boolean half_duplex;

    half_duplex = hwidb->hdx_flags & HDX_FLAG_ON; 
    saved_role  = half_duplex; 

    if (!csb->sense) {
        if (csb->which == CFG_HDX_SET_HALF_DUPLEX) {
            /*
             * no half-duplex
             *    no-op for BSTUN
             *    reset to full-duplex otherwise
             */
            if (hwidb->enctype != ET_BSTUN)
                half_duplex = FALSE;
        }
        else {
            /*
             * no full-duplex
             *    reset to half-duplex for BSTUN
             *    no-op otherwise
             */
            if (hwidb->enctype == ET_BSTUN)
                half_duplex = TRUE;
        }
    }
    else {
        if (csb->which ==  CFG_HDX_SET_HALF_DUPLEX) {
            /* 
             *  half-duplex
             */
            half_duplex = TRUE;
        }
        else {
            /*
             *  full-duplex
             */
            half_duplex = FALSE;
        }
    }

    /* If the flags haven't changed then we are done! */
    if (saved_role == half_duplex) 
        return;

    serial_half_duplex_reset_params(hwidb, half_duplex);
}

/*
 * serial_half_duplex_command
 */
void serial_half_duplex_command (parseinfo *csb)
{
    hwidbtype *hwidb  = csb->interface->hwptr;
    ulong value;
 
    if (!(hwidb->hdx_flags & HDX_FLAG_CAPABLE)) {
          if (!csb->nvgen)
               printf("\nThe command %s is not supported for %s.", 
                         csb->nv_command, hwidb->hw_namestring);
        return;
    }

    if (csb->nvgen) {
        if (csb->which == CFG_HDX_SET_HALF_DUPLEX)
            serial_half_duplex_nvgens(hwidb);
        return;
    }

    switch (csb->which) {
        case CFG_HDX_RESET_PARAMS:
            serial_half_duplex_reset_params(hwidb, csb->sense);
            break;

        case CFG_HDX_SET_HALF_DUPLEX:
        case CFG_HDX_SET_FULL_DUPLEX:
            serial_set_half_duplex(csb);
            break;

        case CFG_HDX_CONTROLLED_CARRIER:
            if (hwidb->dte_interface) {
                printf(generic_only_msg, dce_only_msg);
                break;
            }
            if (!(hwidb->hdx_flags & HDX_FLAG_ON)) {
                printf(generic_only_msg, hdx_only_msg);
                break;
            }

            send_state_config(hwidb, IDB_CONTROL_SET_CONFIG,
                              SERIAL_HW_CONTROLLED_CARRIER, csb->sense);
            break;

        case CFG_HDX_RTS_TIMEOUT:
            if (!hwidb->dte_interface) {
                printf(generic_only_msg, dte_only_msg);
                break;
            }
            if (!(hwidb->hdx_flags & HDX_FLAG_ON)) {
                printf(generic_only_msg, hdx_only_msg);
                break;
            }
  
            /* Set the value */
            if (!csb->sense)
                 value = HDX_RTS_TIMEOUT_DEFAULT;
            else 
                 value = GETOBJ(int, 1);

            /* Set the config */
            send_state_config(hwidb, IDB_CONTROL_SET_CONFIG,
                              SERIAL_HW_RTS_TIMEOUT, value);    
            break;

        case CFG_HDX_CTS_DELAY:
            if (hwidb->dte_interface) {
                printf(generic_only_msg, dce_only_msg);
                break;
            }
            if (!(hwidb->hdx_flags & HDX_FLAG_ON)) {
                printf(generic_only_msg, hdx_only_msg);
                break;
            }

            /* Set the value */
            if (!csb->sense)
                value = HDX_CTS_DELAY_DEFAULT;
            else
                value = GETOBJ(int, 1);

            /* Set the config */
            send_state_config(hwidb, IDB_CONTROL_SET_CONFIG,
                              SERIAL_HW_CTS_DELAY, value);
            break;

        case CFG_HDX_RTS_DROP_DELAY:
            if (!hwidb->dte_interface) {
                printf(generic_only_msg, dte_only_msg);
                break;
            }
            if (!(hwidb->hdx_flags & HDX_FLAG_ON)) {
                printf(generic_only_msg, hdx_only_msg);
                break;
            }

            /* Set the value */
            if (!csb->sense)
                value = HDX_RTS_DROP_DELAY_DEFAULT;
            else
                value = GETOBJ(int, 1);

            /* Set the config */
            send_state_config(hwidb, IDB_CONTROL_SET_CONFIG,
                              SERIAL_HW_RTS_DROP_DELAY, value);
            break;

        case CFG_HDX_CTS_DROP_TIMEOUT:
            if (!hwidb->dte_interface) {
                printf(generic_only_msg, dte_only_msg);
                break;
            }
            if (!(hwidb->hdx_flags & HDX_FLAG_ON)) {
                printf(generic_only_msg, hdx_only_msg);
                break;
            }

            /* Set the value */
            if (!csb->sense)
                value = HDX_CTS_DROP_TIMEOUT_DEFAULT;
            else
                value = GETOBJ(int, 1);

            /* Set the config */
            send_state_config(hwidb, IDB_CONTROL_SET_CONFIG,
                              SERIAL_HW_CTS_DROP_TIMEOUT, value);
            break;

        case CFG_HDX_DCD_TXSTART_DELAY:
            if (hwidb->dte_interface) {
                printf(generic_only_msg, dce_only_msg);
                break;
            }
            if (!(hwidb->hdx_flags & HDX_FLAG_ON)) {
                printf(generic_only_msg, hdx_only_msg);
                break;
            }

            /* Set the value */
            if (!csb->sense)
                value = HDX_DCD_TXSTART_DELAY_DEFAULT;
            else
                value = GETOBJ(int, 1);

            /* Set the config */
            send_state_config(hwidb, IDB_CONTROL_SET_CONFIG,
                              SERIAL_HW_DCD_TXSTART_DELAY, value);
            break;

        case CFG_HDX_DCD_DROP_DELAY:
            if (hwidb->dte_interface) {
                printf(generic_only_msg, dce_only_msg);
                break;
            }
            if (!(hwidb->hdx_flags & HDX_FLAG_ON)) {
                printf(generic_only_msg, hdx_only_msg);
                break;
            }

            /* Set the value */
            if (!csb->sense)
                value = HDX_DCD_DROP_DELAY_DEFAULT;
            else
                value = GETOBJ(int, 1);

            /* Set the config */
            send_state_config(hwidb, IDB_CONTROL_SET_CONFIG,
                              SERIAL_HW_DCD_DROP_DELAY, value);
            break;

        case CFG_HDX_TRANSMIT_DELAY:
            if (!(hwidb->hdx_flags & HDX_FLAG_ON)) {
                printf(generic_only_msg, hdx_only_msg);
                break;
            }

            /* Set the value */
            if (!csb->sense)
                value = HDX_TRANSMIT_DELAY_DEFAULT;
            else
                value = GETOBJ(int, 1);

            /* Set the config */
            send_state_config(hwidb, IDB_CONTROL_SET_CONFIG,
                              SERIAL_HW_TRANSMIT_DELAY, value);
            break;

        case CFG_HDX_TEST_RTS:
            send_state_config(hwidb, IDB_CONTROL_SET_CONFIG,
                              SERIAL_HW_TEST_RTS, csb->sense);
            break;

        case CFG_HDX_TEST_CTS:
            send_state_config(hwidb, IDB_CONTROL_SET_CONFIG,
                              SERIAL_HW_TEST_CTS, csb->sense);
            break;

        case CFG_HDX_TEST_DTR:
            send_state_config(hwidb, IDB_CONTROL_SET_CONFIG,
                              SERIAL_HW_TEST_DTR, csb->sense);
            break;

        case CFG_HDX_TEST_DSR:
            send_state_config(hwidb, IDB_CONTROL_SET_CONFIG,
                              SERIAL_HW_TEST_DSR, csb->sense);
            break;

        case CFG_HDX_TEST_DCD:
            send_state_config(hwidb, IDB_CONTROL_SET_CONFIG,
                              SERIAL_HW_TEST_DCD, csb->sense);
            break;

        case CFG_HDX_TEST_LOOPBACK:
            send_state_config(hwidb, IDB_CONTROL_SET_CONFIG,
                              SERIAL_HW_TEST_LOOPBACK, csb->sense);
            break;

        case CFG_HDX_TEST_TRANSMIT:
            if (csb->sense)
                value = GETOBJ(int, 1);
            else
                value = 0;
            send_state_config(hwidb, IDB_CONTROL_SET_CONFIG,
                              SERIAL_HW_TEST_TRANSMIT, value);
            break;

        case CFG_HDX_LOG_ENABLE:
            if (!(hwidb->hdx_flags & HDX_FLAG_ON)) {
                printf(generic_only_msg, hdx_only_msg);
                break;
            }
            send_state_config(hwidb, IDB_CONTROL_SET_CONFIG,
                              SERIAL_HW_HDX_LOG_ENABLE, csb->sense);
            break;

        case CFG_HDX_LOG_CLEAR:
            if (!(hwidb->hdx_flags & HDX_FLAG_ON)) {
                printf(generic_only_msg, hdx_only_msg);
                break;
            }
            send_state_config(hwidb, IDB_CONTROL_SET_CONFIG,
                              SERIAL_HW_HDX_LOG_CLEAR, TRUE);
            break;

        default:
            if (!csb->nvgen)
                 printf("\nThis command is not supported for %s.", 
                           hwidb->hw_namestring);
            return;
    }
}

/*
 * serial_half_duplex_translate_old_cmds
 */
void serial_half_duplex_translate_old_cmds (parseinfo *csb)
{
    hwidbtype *hwidb = csb->interface->hwptr;
    ulong msecval;
    ulong count; 
    ulong maxval;

    /* We need to know the type of hardware here
     * to adjust the values to milliseconds properly
     */
    if (hwidb->type == IDBTYPE_HD64570) {
        count   =   HD64570_COUNTS_PER_MSEC;
        maxval  =   HD64570_MAX_MSEC_CTS_RTS;
    } 
    else if (hwidb->type == IDBTYPE_MCISERIAL) {
        count   =   MCI_COUNTS_PER_MSEC;
        maxval  =   HD64570_MAX_MSEC_CTS_RTS;
    }
    else {
        count = 1;
        maxval = 64000;
    }

    msecval = 0;
    if (csb->which == CFG_HDX_RTS_TIMEOUT) {
        /* Adjust counts to millisec */
        if (GETOBJ(int,1) % count)
            msecval = 1;
        msecval += GETOBJ(int,1) / count;
        if (msecval > maxval)
            msecval = maxval;
        GETOBJ(int,1) = msecval;
        printf("\nConverting value to %d millisec %s", msecval,
                 msecval == HDX_RTS_TIMEOUT_DEFAULT ? 
                 "which is the default in msec" : " ");
    }
    else if (csb->which == CFG_HDX_CTS_DELAY) {
        if (GETOBJ(int,1) % count)
            msecval = 1;
        msecval += GETOBJ(int,1) / count;
        if (msecval > maxval)
            msecval = maxval;
        GETOBJ(int,1) = msecval;
        printf("\nConverting value to %d millisec %s", msecval,
               msecval == HDX_CTS_DELAY_DEFAULT ? 
               "which is the default in msec" : " ");
    }

    /*
     *  Pass everything to serial_half_duplex_command
     */
    serial_half_duplex_command(csb);
    return;
}

/*
 * physical_layer_command()
 * Sets the physical layer on a serial interface to the requested one -
 * synchronous or asynchronous as requested.
 */
void physical_layer_command (parseinfo *csb)
{
    hwidbtype *hwidb;

    hwidb = csb->interface->hwptr;

    /*
     * The sync physical-layer is the default and so will not be
     * NV-generated.
     */
    if (csb->nvgen) {
	/*
	 * Only for interfaces with multiple possibilities!
	 */
	if (hwidb->serial_flags & SERIAL_FLAGS_SYNCASYNC)
	    nv_add((hwidb->status&IDB_ASYNC), "\n %s async", csb->nv_command);
        return;
    }

    /*
     * The default physical-layer for the serial interface is sync.
     * If the 'no' keyword was specified, set the physical layer for this
     * interface to be sync.
     */
    if (!csb->sense)
        csb->which = PHY_LAYER_SYNC;

    /*
     * Modify the physical layer for this interface by making the appropriate
     * registry call. The registry will check to make sure that the
     * interface's physical layer is not already requested one.
     */
    if (!reg_invoke_serial_physical_layer(hwidb, csb, csb->which)) {
        printf("\n%%Error: Unknown physical layer\n");
    }
}

