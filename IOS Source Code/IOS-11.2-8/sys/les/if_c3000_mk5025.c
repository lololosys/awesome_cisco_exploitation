/* $Id: if_c3000_mk5025.c,v 3.3.62.3 1996/04/27 06:32:42 syiu Exp $
 * $Source: /release/112/cvs/Xsys/les/if_c3000_mk5025.c,v $
 *------------------------------------------------------------------
 * if_c3000_mk5025.c - Pancake Network serial interface driver
 * 
 * March 1990, Chris Shaker
 *
 * Copyright (c) 1990-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 * PLEASE NOTE: I am now using the convention that RAP must be 0 whenever
 * interrupts are enabled so that the level 4 interrupt routine does not
 * have to write RAP = 0 to save time.
 *------------------------------------------------------------------
 * $Log: if_c3000_mk5025.c,v $
 * Revision 3.3.62.3  1996/04/27  06:32:42  syiu
 * Branch: California_branch
 * Commit Dial1_branch
 *
 * Revision 3.3.20.2  1996/04/26  01:41:05  syiu
 * Branch: Dial1_branch
 * Dial1_branch to IosSec_postmerge_960419
 *
 * Revision 3.3.20.1  1996/02/23  20:51:40  lbustini
 * Branch: Dial1_branch
 * Move dialer fields out of the idb.
 *
 * Revision 3.3.62.2  1996/04/03  01:59:36  sagarwal
 * CSCdi51875:  Serial Int. MTU change doesnt get new buffers if pulsetime
 * confged
 * Branch: California_branch
 *  Compare current bufferpool size against idb maxdgram instead of
 *  relying on boolean passed to controller init to remember mtu
 *  changes across DTR pulse intervals
 *
 * Revision 3.3.62.1  1996/03/18  20:41:16  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.26.3  1996/03/13  01:49:02  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.3.26.2  1996/03/09  05:07:29  hampton
 * Slim down the kernel of the router.
 * Branch: DeadKingOnAThrone_branch
 *
 * Revision 3.3.26.1  1996/02/20  14:41:12  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.3  1995/11/17  17:38:31  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:57:41  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:31:06  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/07/18  06:47:14  hampton
 * Miscellaneous changes to eliminate direct references to the system
 * clock and to convert to always using the timer macros to access timers.
 * [CSCdi37217]
 *
 * Revision 2.3  1995/06/21 03:00:59  gstovall
 * CSCdi35731:  Cleanup unused cruft in idb
 * Nuke some unused vectors and variables from the hwidb.
 *
 * Revision 2.2  1995/06/19  06:47:03  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.1  1995/06/07  21:33:11  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../../boot/cpu.h"
#include "interface_private.h"
#include "stacks.h"
#include "logger.h"
#include "if_les.h"
#include "if_les_mk5025.h"  
#include "../if/linkdown_event.h"
#include "../if/network.h"
#include "config.h"
#include "../wan/dialer_registry.h"


/*
 * mk5025_enable_modem_interrupt()
 *
 * Enables modem control line interrupts for a given
 * interface.
 */
void mk5025_enable_modem_interrupt (mk5025_instance_t *ds)
{
    ds->regaddr->signal_control |= MODEM_CONTROL_INT;
}


/*
 * mk5025_disable_modem_interrupt()
 *
 * Disables modem control line interrupts for a given
 * interface.
 */
void mk5025_disable_modem_interrupt (mk5025_instance_t *ds)
{
    ds->regaddr->signal_control &= ~MODEM_CONTROL_INT;
}


void mk5025_get_regaddr (hwidbtype *idb)
{
    mk5025_instance_t *ds;

    ds = INSTANCE;

    /* find the register address for the MK5025, save the
     * IDB ptr for level 4 int wrapper and save the bit mask
     * used to reset this interface. */
    if (idb->unit == 0)
    {
        ds->regaddr = (mk5025_hw_reg_t *)ADRSPC_NETWORK_B;
        network_b_idb = idb;	
        ds->reset_mask = CONTROL_RESET_NETWORK_B;
    }
    else if (idb->unit == 1)
    {
	ds->regaddr = (mk5025_hw_reg_t *)ADRSPC_NETWORK_C;
        network_c_idb = idb;	
        /* this bit mask is brut specific because brut is currently
         * the only 3 port box and brut has its own mask values */
        ds->reset_mask = BRUT_RESET_NETWORK_C;
    }
    else /* illegal unit number */
    {
        buginf("\nillegal unit number = %d", idb->unit);
        crashdump(0);
    }
}


/*
 * mk5025_set_vectors()
 * Setup interrupt type stuff
 */
void mk5025_set_vectors (hwidbtype *idb)
{
    if (idb->unit == 0) {
	pan_int_array[1].function = (ulong)level4_pan_t0;
	pan_int_array[1].idb1 = idb;
    } else {
	pan_int_array[2].function = (ulong)level4_pan_t1;
	pan_int_array[2].idb1 = idb;
    }
}


/*
 * mk5025_init_config_options()
 *
 * Set the interface state options available through the config
 * file in NVRAM or loaded via the network
 * 
 */
void mk5025_init_config_options (hwidbtype *idb)
{
 /*
  * The IGS has no applicable control options
  */
}


void mk5025_enter_transparent_mode (hwidbtype *idb)
{
    volatile ushort *control_register;
    mk5025_instance_t *ds;
    unsigned short loopback_mask = 0;

    ds = INSTANCE;

    /*
     * Assert DTR before checking state of DCD so that DTR->DCD
     * loopbacks can work:
     */
     ds->regaddr->signal_control = 0;
    if (serial_debug)
        buginf("\nMK5(%d): Asserting %s ", idb->unit, "DTR");

    /*
     * Enter Transparent mode:
     */
    ds->regaddr->rap = MK5025_CSR1;
    ds->regaddr->csr = MK5025_CSR1_UPRIM_TRANS | MK5025_CSR1_UAV;

    /*
     * Say we're up or down based on DCD:
     */
    if (ds->regaddr->signal_control & SERIAL_DCD)
        net_cstate(idb, IDBS_DOWN);
    else
        net_cstate(idb, IDBS_UP);
    
    /* set the control register and loopback bit mask based on the 
     * processor type. the brut box has different values for these
     * variables.
     */
    if (cookie->processor == PROCESSOR_BRUT) {
        control_register = (volatile ushort *)ADRSPC_CONTROL3;
        if (idb->unit == 0)
            loopback_mask = BRUT_SERIAL_LOOPBACK_B;
        else if (idb->unit == 1) 
            loopback_mask = BRUT_SERIAL_LOOPBACK_C;
        else {
            buginf("\nillegal unit number = %d", idb->unit);
            crashdump(0);
        }
    }
    else { /* not a brut box */
        control_register = (volatile ushort *)ADRSPC_CONTROL;
        loopback_mask = CONTROL_SERIAL_LOOP; /* Enable Loopback */
    }
    if (idb->loopback)
        *control_register |= loopback_mask; /* Enable Loopback */
    else
        *control_register &= ~loopback_mask; /* Disable Loopback */
    
    /*
     * Enable interrupts:
     */
    ds->regaddr->rap = MK5025_CSR0;
    ds->regaddr->csr = MK5025_CSR0_INEA;

}


/*
 * Reset the MK5025:
 */
void mk5025_reset (hwidbtype *idb)
{
    volatile u_short *control;
    mk5025_instance_t *ds;

    ds = INSTANCE;

    /* if this is a BRUT box use system control register 3 for 
     * reseting the interface, else use system control register 1. 
     * the reset mask is the same for both
     */
    if (cookie->processor == PROCESSOR_BRUT)
        control = (volatile u_short *)ADRSPC_CONTROL3;
    else
        control = (volatile u_short *)ADRSPC_CONTROL;

    if (ds->reset_mask) {
        *control |= ds->reset_mask;
        /* wait a moment for the reset to take place, the mk5025
         * requires only 30 nanoseconds so this should give 
         * plenty of time */
        asm("nop");
        asm("nop");
        *control &= ~ds->reset_mask;
    } else {
	buginf("\nMK5(%d): bad reset_mask = (%#x), ds = (%#x) at RESET",
	       idb->unit, ds->reset_mask, ds);
        crashdump(0);
    }
}


/*
 * Stop the mk5025:
 * Must be called with network interfaces disabled.
 */
void mk5025_stop (hwidbtype *idb, mk5025_hw_reg_t *reg_addr)
{
    /*
     * Deassert DTR:
     */
    reg_addr->signal_control = SERIAL_DTR;
    if (serial_debug)
        buginf("\nMK5(%d): Deasserting %s ", idb->unit, "DTR");

    /*
     * If already stopped, return. This is because if you try
     * to stop the mk5025 when it is already stopped an error
     * occurs.
     */
    reg_addr->rap = MK5025_CSR0;
    if (reg_addr->csr & MK5025_CSR0_STOP)
	return;

    /*
     * Clear any error or interrupt bits in MK5025_CSR0:
     */
    reg_addr->csr = MK5025_CSR0_ERRORS | MK5025_CSR0_INTERRUPTS;

    /*
     * Clear User error and Primitive Available and issue the stop primitive:
     */
    reg_addr->rap = MK5025_CSR1;
    reg_addr->csr = MK5025_CSR1_UERR | MK5025_CSR1_PAV;
    reg_addr->csr = MK5025_CSR1_UPRIM_STOP | MK5025_CSR1_UAV;
    reg_addr->rap = MK5025_CSR0; /* Leave RAP = 0 for interrupts */
}


/*********************************************************************
*    mk5025_pan_delta_interrupt()
*
*    This function is a interrupt service routine for a level 3
*    modem control interrupt. An interrupt is generated whenever
*    a serial port sees a change in CTS, DCD or DSR. This function
*    checks which serial port caused the interrupt and then changes
*    the state of the interface depending on the cause of the interrupt.
*    This function was written to service one serial interface at a
*    time. If two interfaces interrupt at the same time, one will
*    be serviced and the other's interrupt will not have been cleared
*    so it will be serviced soon after.
*
*    Code for processing modem control change interrupts does not 
*    have to be as concerned with speed as other ISR's such as 
*    those processing packet transmit/receive interrupts.
*********************************************************************/
static ushort mk5025_pan_delta_interrupt (hwidbtype *idb)
{
    mk5025_instance_t *ds;
    serial_register_t state;
    int newstate;
    ushort status;

    /* set the instance ptr based on the idb */
    ds = INSTANCE;

    /*
     * Get the new serial state by reading the serial state register 
     * and then clear the interrupt by writing to it (preserving DTR).
     */
    state = ds->regaddr->signal_control;
    ds->regaddr->interrupt_control = state;

    status = 0;
    if (cookie->processor == PROCESSOR_BRUT) {
	/*
	 * Mask out the cable type from control register 3
	 */
	if (idb->unit == 0)
	    status = ((*(volatile ushort *)ADRSPC_CONTROL3) & 
		      BRUT_CABLE_MASK_B) >> BRUT_CABLE_SHIFT_B;
	else if (idb->unit == 1)
	    status = ((*(volatile ushort *)ADRSPC_CONTROL3) & 
		      BRUT_CABLE_MASK_C) >> BRUT_CABLE_SHIFT_C;
	else {
	    /*
	     * Illegal unit number
	     */
	    buginf("\nmk5025_pan_delta_interrupt(): illegal unit = %d",
		   idb->unit);
	    return(0);
	}
    } else {
	status = ((*status_ptr) & STATUS_CABLE) >> 
	    STATUS_CABLE_SHIFT;
    }

    /*
     * Check interface up/down status, maybe start some output.
     */
    if (idb->state != IDBS_ADMINDOWN) {
	if (serial_debug)
	    buginf("\nMK5(%d): New serial state = 0x%02x",
		   idb->unit, state);
	if (((cookie->processor == PROCESSOR_BRUT) && 
	     (status != BRUT_STATUS_CABLE_NONE)) || 
	    (status != STATUS_CABLE_NONE)) {
	    if (state & SERIAL_DCD) {
		newstate = IDBS_DOWN; /* No DCD, line is down */
	        idb->failCause = Serial_SignalLost;
		if (serial_debug)
		    buginf("\nMK5(%d): DCD is down.", idb->unit);
	    } else {
		newstate = IDBS_UP;
		if (serial_debug)
		    buginf("\nMK5(%d): DCD is up.", idb->unit);
	    }
	} else {
	    newstate = IDBS_DOWN;
            idb->failCause = Serial_SignalLost;
	    if (serial_debug)
		buginf("\nMK5(%d): Cable is unplugged.", idb->unit);
	}

	if ((idb->state != newstate) &&
	    (idb->state != IDBS_ADMINDOWN) &&
	    !reg_invoke_dialer_spoofing_int(idb)) {
	    idb->counters.transitions++;
	    net_cstate(idb, newstate);
	}

        /*
         * If we are spoofing, that means that we are pretending that the
         * idb state is IDBS_UP even though it isn't.  So, if carrier has
         * come up (newstate == IDBS_UP) and we are spoofing, act the same
         * as if idb->state != newstate.  Spoofing will be cleared in the
         * timer routine which deals with bouncing signals.
         */
	if ((reg_invoke_dialer_spoofing_int(idb) && 
		(newstate == IDBS_UP)) &&
	        (idb->state != IDBS_ADMINDOWN)) {
	    idb->counters.transitions++;
	    net_cstate(idb, newstate);
	}
    }

    if (++ds->flap_count > ds->flap_rate) {
	mk5025_disable_modem_interrupt(ds);
	ds->flap_enabled = TRUE;
    }

    if (!ds->flap_enabled)
	if (!TIMER_RUNNING_AND_SLEEPING(ds->flap_time)) {
	    TIMER_START(ds->flap_time, ONESEC);
	    ds->flap_count = 0;
	}
    
    return(state);
}


/*
 * Front-end for interrogating serial line parameters.
 * This is neccesary because the reading the state register
 * will clear outstanding modem control interrupts.
 */
ushort mk5025_get_interface_state (hwidbtype *idb, boolean transition_valid)
{
    ulong  transitions;
    ushort state;
    
    transitions = idb->counters.transitions;
    
    /*
     * Fetch the line control bits
     */
    state = mk5025_pan_delta_interrupt(idb);

    if (transition_valid) {
	/*
	 * Restore transition count as we spoofed the
	 * interrupt call
	 */
	idb->counters.transitions = transitions;
    }

    return(state);
}


/*
 * init_mk5025_idb -- set up an idb for an mk5025 chip
 */
static void init_mk5025_idb (int which)
{
    hwidbtype *idb;

    /*
     * Set up storage, add IDB to interface queue:
     */
    idb = idb_create();
    if (!idb) {
	crashdump(0);
    }

    idb->unit = which;

    mk5025_init_idb_common(idb);

    /*
     * Always a DTE
     */
    idb->dte_interface = TRUE;

    /*
     * Initialize the interface:
     */
    mk5025_init(idb);
}


/*********************************************************************
*    analyze_mk5025()
*
*    This function reads the cookie to figure out how many serial
*    interfaces there are. It setups the level 3 interrupt and
*    initializes the serial interface(s). This function returns the 
*    number of SERIAL interfaces.
*********************************************************************/
static int analyze_mk5025 (void)
{
    int num_serial_interfaces = 0;

    if (cpu_type != CPU_CRISTAL && cpu_type != CPU_CANCUN)
	if ((cookie->interfaces == INTERFACE_1E1T) ||
	    (cookie->interfaces == INTERFACE_1R1T)) 
	    {
		/* this box has one serial interface, setup the interrupt and
		 * initialize the interace structure for the serial interface
		 */
		createlevel(SERIAL_DELTA_INTLEVEL, 
			    (void (*)(void))mk5025_pan_delta_interrupt,
			    "Serial interface state change interrupt");
		init_mk5025_idb(0);
		num_serial_interfaces = 1;
	    }
	else if ((cookie->interfaces == INTERFACE_1E2T) ||
		 (cookie->interfaces == INTERFACE_1R2T))
	    {
		/* this box has two serial interfaces, setup the interrupt and
		 * initialize the interface structure for the serial interfaces
		 */
		createlevel(SERIAL_DELTA_INTLEVEL, 
			    (void (*)(void))mk5025_pan_delta_interrupt,
			    "Serial interface state change interrupt");
		init_mk5025_idb(0);
		init_mk5025_idb(1);
		num_serial_interfaces = 2;
	    }

    return(num_serial_interfaces);
} /* analyze_mk5025() */


/*
 * MK5025 subsystem initialisation entry point
 */
void mk5025_subsys_init (subsystype *subsys)
{
    /*
     * Discover all the MK5025 serial interfaces in system
     */
    nserialnets += analyze_mk5025();
}


/*
 * Show MK5025 structure pointers
 */
void mk5025_show_structure_info (hwidbtype *idb)
{
    mk5025_instance_t *ds = INSTANCE;

    printf("idb %#x, ds %#x, regaddr %#x, reset_mask %#x\n",
	   idb, ds, ds->regaddr, ds->reset_mask);
}


/*
 * Show MK5025 cable state:
 */
void mk5025_show_cable_state (hwidbtype *idb)
{
    ushort status;

    /*
     * A run time check of the processor type is required 
     * becauase the brut box has its own cable type masks 
     * and cable type values
     */
    if (cookie->processor == PROCESSOR_BRUT) {
	/*
	 * Mask out the cable type from control register 3
	 */
	if (idb->unit == 0)
	    status = ((*(volatile ushort *)ADRSPC_CONTROL3) & 
		      BRUT_CABLE_MASK_B) >> BRUT_CABLE_SHIFT_B;
	else if (idb->unit == 1)
	    status = ((*(volatile ushort *)ADRSPC_CONTROL3) & 
		      BRUT_CABLE_MASK_C) >> BRUT_CABLE_SHIFT_C;
	else {
	    /*
	     * Illegal unit number
	     */
	    printf("\n illegal idb unit number = %d",idb->unit);
	    return;
	}

	switch (status) {
	  case BRUT_STATUS_CABLE_NONE:
	    printf("No");
	    break;
	  case BRUT_STATUS_CABLE_422:
	    printf("530A or 422");
	    break;
	  case BRUT_STATUS_CABLE_V24:
	    printf("V.24 (RS-232) or 423");
	    break;
	  case BRUT_STATUS_CABLE_449:
	    printf("V.11 (RS-449 or X.21)");
	    break;
	  case BRUT_STATUS_CABLE_V35:
	    printf("V.35");
	    break;
	  default:
	    printf("Unknown (%d)",status);
	}
	printf(" serial cable attached\n");

    } else {
	status = ((*status_ptr) & STATUS_CABLE) >> 
	    STATUS_CABLE_SHIFT;
	switch (status) {
	  case STATUS_CABLE_NONE:
	    printf("No");
	    break;
	  case STATUS_CABLE_232:
	    printf("V.24 (RS-232)");
	    break;
	  case STATUS_CABLE_449:
	    printf("V.11 (RS-449 or X.21)");
		break;
	  case STATUS_CABLE_V35:
	    printf("V.35");
	    break;
	  default:
	    printf("Unknown (%d)",status);
	    }
	printf(" serial cable attached\n");
    }
}

void mk5025_clockrate_command (parseinfo *csb)
{
    /*
     * Nothing to do on the 3000
     */
}
