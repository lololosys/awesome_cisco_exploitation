/* $Id: if_c4000_mk5025.c,v 3.4.44.4 1996/09/09 01:34:24 xtang Exp $
 * $Source: /release/112/cvs/Xsys/les/if_c4000_mk5025.c,v $
 *------------------------------------------------------------------
 * C4000 specific serial driver routines
 *
 * 4/92, Steve Southam, Marciano Pitargue
 *
 * Copyright (c) 1992-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: if_c4000_mk5025.c,v $
 * Revision 3.4.44.4  1996/09/09  01:34:24  xtang
 * CSCdi56632:  unknown clock rates on 4T PA, FSIP
 * Branch: California_branch
 *         add ALT_HELPS to use csb->which in different
 *         driver procedures, and print the
 *         driver specific info. In this case, the clockrate
 *         option table, and csb->which = CFG_INT_CLOCKRATE
 *
 * Revision 3.4.44.3  1996/04/27  06:32:56  syiu
 * Branch: California_branch
 * Commit Dial1_branch
 *
 * Revision 3.4.8.2  1996/04/26  01:41:49  syiu
 * Branch: Dial1_branch
 * Dial1_branch to IosSec_postmerge_960419
 *
 * Revision 3.4.8.1  1996/02/23  20:51:46  lbustini
 * Branch: Dial1_branch
 * Move dialer fields out of the idb.
 *
 * Revision 3.4.44.2  1996/04/03  01:59:39  sagarwal
 * CSCdi51875:  Serial Int. MTU change doesnt get new buffers if pulsetime
 * confged
 * Branch: California_branch
 *  Compare current bufferpool size against idb maxdgram instead of
 *  relying on boolean passed to controller init to remember mtu
 *  changes across DTR pulse intervals
 *
 * Revision 3.4.44.1  1996/03/18  20:41:56  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.4.12.2  1996/03/13  01:49:26  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.4.12.1  1996/01/24  22:23:50  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.4  1996/01/06  03:22:36  hampton
 * Remove extraneous includes of if_timer.h.  Rename if_timer.h and move
 * it to the 'os' directory.  [CSCdi46482]
 *
 * Revision 3.3  1995/11/17  17:39:47  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:58:49  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:32:04  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/07/18  06:47:28  hampton
 * Miscellaneous changes to eliminate direct references to the system
 * clock and to convert to always using the timer macros to access timers.
 * [CSCdi37217]
 *
 * Revision 2.2  1995/06/19 06:47:26  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.1  1995/06/07  21:34:59  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * if_xx_mk5025.c  DosXX Network serial interface driver
 *
 *	March 1990, Chris Shaker
 *
 * PLEASE NOTE: I am now using the convention that RAP must be 0 whenever
 * interrupts are enabled so that the level 4 interrupt routine does not
 * have to write RAP = 0 to save time.
 */

#include "master.h"
#include "interface_private.h"
#include "sys_registry.h"
#include "media_registry.h"
#include "config.h"
#include "../wan/dialer_registry.h"
#include "logger.h"
#include "if_les.h"
#include "if_les_mk5025.h"
#include "../if/linkdown_event.h"
#include "if_les_serial.h"
#include "../wan/serial.h"
#include "../wan/serial_private.h"
#include "../if/network.h"


/*
 * Clockrate/divider table for the i8254 on the DSIM
 */
static xxclocktype xxclock[NXXCLOCKS] = {
    {0, 0},
    {1200, 6667}, {2400, 3333}, {4800, 1667}, {9600, 833},
    {19200, 417}, {38400, 208}, {56000, 143}, {64000, 125},
    {72000, 111}, {125000, 64}, {148000, 54}, {250000, 32},
    {500000, 16}, {800000, 10}, {1000000, 8}, {1300000, 6},
    {2000000, 4}, {4000000, 2}
};    


/*
 * mk5025_enable_modem_interrupt()
 *
 * Enables modem control line interrupts for a given
 * interface.
 */
void mk5025_enable_modem_interrupt (mk5025_instance_t *ds)
{
    ds->regaddr->control &= ~DSIM_CONTROL_INTR_EN;
}


/*
 * mk5025_disable_modem_interrupt()
 *
 * Disables modem control line interrupts for a given
 * interface.
 */
void mk5025_disable_modem_interrupt (mk5025_instance_t *ds)
{
    ds->regaddr->control |= DSIM_CONTROL_INTR_EN;
}


/*
 * mk5025_i8254_init()
 *
 * Called to initialise and start the i8254 for a particular channel
 * Should only be called for interfaces that are functioning as DCEs
 */
static void mk5025_i8254_init (hwidbtype *idb)
{
    mk5025_instance_t *ds;
    i8254_t           *i8254;
    int  index;
    uint speed;
    serialsb *ssb;

    ds = INSTANCE;
    ssb = idb_get_hwsb_inline(idb, HWIDB_SB_SERIAL);

    index = ssb->serial_clockindex;
    speed = xxclock[index].counter;
    i8254 = &ds->dsimaddr->i8254;
    
    if (C4000_EVEN_UNIT) {
        i8254->control  = I8254_CH_0 | I8254_MODE_3 | I8254_RL_LSB_MSB;
	usecdelay(5);
        i8254->channel0 = speed & 0xff;
	usecdelay(5);
        i8254->channel0 = (speed >> 8) & 0xff;
	usecdelay(5);
    } else {
        i8254->control  = I8254_CH_1 | I8254_MODE_3 | I8254_RL_LSB_MSB;
	usecdelay(5);
        i8254->channel1 = speed & 0xff;
	usecdelay(5);
        i8254->channel1 = (speed >> 8) & 0xff;
	usecdelay(5);
    }
}


/*
 * mk5025_i8254_stop()
 * 
 * Stops the i8254 after it's started running. Used for DCEs that have an
 * undefined clockrate (so we can use external clock generators) or DTEs
 */
static void mk5025_i8254_stop (hwidbtype *idb)
{
    mk5025_instance_t *ds;
    i8254_t           *i8254;

    ds = INSTANCE;

    i8254 = &ds->dsimaddr->i8254;
    
    /*
     * The i8254, being a fetid pile of dog turd, doesn't have a
     * reset line. So we have to stall the state machine indefinitely
     * by writing to the control register and never writing the
     * following 2 bytes. Hurrah for another Intel glory! ;-)
     */
    if (C4000_EVEN_UNIT) {
        i8254->control  = I8254_CH_0 | I8254_MODE_3 | I8254_RL_LSB_MSB;
    } else {
        i8254->control  = I8254_CH_1 | I8254_MODE_3 | I8254_RL_LSB_MSB;
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
    mk5025_instance_t *ds;
    serialsb *ssb;

    ds = INSTANCE;
    ssb = idb_get_hwsb_inline(idb, HWIDB_SB_SERIAL);

    /*
     * Set DTE or DCE
     */
    if (ds->regaddr->control & DSIM_CONTROL_DTE_DCE_SEL) {
	idb->dte_interface = TRUE;
    } else {
	idb->dte_interface = FALSE;
    }
    /*
     * If DCE and we have a valid rate, initialize the clock
     * generator chip
     */
    if (!idb->dte_interface && (ssb->serial_clockindex > 0))
	mk5025_i8254_init(idb);
    else
	mk5025_i8254_stop(idb);

    if (ssb->invert_txc) {
	/*
	 * If we're a DTE then set the value, else clear
	 * the outstanding setting
	 */
	if (idb->dte_interface)
	    ds->regaddr->control |= DSIM_CONTROL_DTE_TI_TXC;
	else
	    ds->regaddr->control &= ~DSIM_CONTROL_DTE_TI_TXC;
    }
    else
	ds->regaddr->control &= ~DSIM_CONTROL_DTE_TI_TXC;

    if (ssb->dce_terminal_timing_enable) {
	/*
	 * If we're a DCE then set the value, else clear
	 * the outstanding setting
	 */
	if (!idb->dte_interface)
	    ds->regaddr->control |= DSIM_CONTROL_DCE_TXC_SCTE;
        else
	    ds->regaddr->control &= ~DSIM_CONTROL_DCE_TXC_SCTE;
    }
    else
	ds->regaddr->control &= ~DSIM_CONTROL_DCE_TXC_SCTE;
}


/*
 * mk5025_enter_transparent_mode()
 *
 * Kick the mk5025 into transparent mode so it can to pass data.
 * Also check the current modem control line state and set the
 * interface state according to those signals
 */
void mk5025_enter_transparent_mode (hwidbtype *idb)
{
    mk5025_instance_t *ds;

    ds = INSTANCE;

    /*
     * If DTE, assert DTR; if DCE, assert DSR.  It's the same HW bit.
     * Do this before checking for DCD if we are a DTE, so that
     * DTR->DCD loopbacks will work:
     */
    ds->regaddr->control |= DSIM_CONTROL_DTE_DTR;
    if (serial_debug) {
        if (ds->regaddr->control & DSIM_CONTROL_DTE_DCE_SEL)
            buginf("\nMK5(%d): Asserting %s ", idb->unit, "DTR");
        else
            buginf("\nMK5(%d): Asserting %s ", idb->unit, "DSR");
    }
    /*
     * Enter Transparent mode:
     */
    ds->regaddr->rap = MK5025_CSR1;
    ds->regaddr->csr = MK5025_CSR1_UPRIM_TRANS | MK5025_CSR1_UAV;

    /*
     * Set V.35 Loopback state:
     */
    if (idb->loopback) {
        ds->regaddr->control |= DSIM_CONTROL_DTE_LTST;
        if (serial_debug)
            buginf("\nMK5(%d): Asserting %s ", idb->unit, "LTST");
    } else {
        ds->regaddr->control &= ~DSIM_CONTROL_DTE_LTST;
        if (serial_debug)
            buginf("\nMK5(%d): Deasserting %s ", idb->unit, "LTST");
    }
    /*
     * If DCE assert DCD
     *
     * N.B. We may lose an interrupt with this read. However
     * the following poll of interface state will catch that
     * transition and Do The Right Thing.
     */
    if (!(ds->regaddr->control & DSIM_CONTROL_DTE_DCE_SEL)) {
        ds->regaddr->control |= DSIM_CONTROL_DCE_DCD;
        if (serial_debug)
            buginf("\nMK5(%d): Asserting %s ", idb->unit, "DCD");
	idb->dte_interface = FALSE;
    } else {
	idb->dte_interface = TRUE;
    }

    /*
     * Set interface state
     */
    (void)mk5025_get_interface_state(idb, FALSE);

    /*
     * Enable MK5025 interrupts
     */
    ds->regaddr->rap = MK5025_CSR0;
    ds->regaddr->csr = MK5025_CSR0_INEA;
}


/*
 * Stop the mk5025:
 * Must be called with network interfaces disabled.
 */
void mk5025_stop (hwidbtype *idb, mk5025_hw_reg_t *reg_addr)
{
    /*
     * If DTE deassert DTR, if DCE deassert DSR:
     */
    reg_addr->control &= ~DSIM_CONTROL_DTE_DTR;
    if (serial_debug)
        if (reg_addr->control & DSIM_CONTROL_DTE_DCE_SEL)
            buginf("\nMK5(%d): Deasserting %s ", idb->unit, "DTR");
        else
            buginf("\nMK5(%d): Deasserting %s ", idb->unit, "DSR");

    /*
     * If DCE deassert DCD:
     */
    if (!reg_addr->control & DSIM_CONTROL_DTE_DCE_SEL) {
	reg_addr->control &= ~DSIM_CONTROL_DCE_DCD;
        if (serial_debug)
            buginf("\nMK5(%d): Deasserting %s ", idb->unit, "DCD");
    }

    /*
     * If Already stopped, return:
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
    state = mk5025_xx_delta_interrupt(idb);

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
 * Handle modem control state changes
 */
ushort mk5025_xx_delta_interrupt (hwidbtype *idb)
{
    mk5025_instance_t *ds;
    ushort state;
    int newstate;

    ds = INSTANCE;

    /*
     * Get the new serial state and clear the interrupt (preserving DTR)
     */
    state = ds->regaddr->control;

    /*
     * Check interface up/down status, maybe start some output.
     */
    if (idb && (idb->state != IDBS_ADMINDOWN)) {
	if (serial_debug)
	    buginf("\nMK5(%d): New serial state = 0x%02x",
		   idb->unit, state);

        if (DSIM_CABLE_TYPE(state) != STATUS_CABLE_NONE) {
	    if (state & DSIM_CONTROL_DTE_DCE_SEL) {
		/* DTE */
		idb->dte_interface = TRUE;
		if (state & DSIM_CONTROL_DTE_DCD) {
		    newstate = IDBS_DOWN; /* No DCD, line is down */

                    /* NmEvent Hook 1 */
                    idb->failCause = Serial_SignalLost;

                    if (serial_debug)
			buginf("\nMK5(%d): DCD is down.", idb->unit);
		} else {
		    newstate = IDBS_UP;
		    if (serial_debug)
			buginf("\nMK5(%d): DCD is up.", idb->unit);
		}
	    } else {
		/* DCE */
		idb->dte_interface = FALSE;
		if (state & DSIM_CONTROL_DCE_DTR) {
		    newstate = IDBS_DOWN; /* No DTR, line is down */

                    /* NmEvent Hook 1 */
	            idb->failCause = Serial_SignalLost;

		    if (serial_debug)
			buginf("\nMK5(%d): DTR is down.", idb->unit);
		} else {
		    newstate = IDBS_UP;
		    if (serial_debug)
			buginf("\nMK5(%d): DTR is up.", idb->unit);
		}
	    }
        } else {
            /*
             * This should be unnecessary. But the hardware can leave
             * these signal lines floating around so it's best if we
             * don't take chances as the state "wrapper" could get
             * confused by the returning bogus states.
             *
             * The default case is for the interface to be a DTE with
             * the incoming DCD unasserted. If any other signal lines
             * prove to be a problem, they should be masked here.
             */
            state |= (DSIM_CONTROL_DTE_DCE_SEL | DSIM_CONTROL_DTE_DCD);
	    
            newstate = IDBS_DOWN;

            /* NmEvent Hook 1 */
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
 * mk5025_clockrate_command()
 *
 * Called at NVRAM parse time to setup the required clockrate for
 * the interface when used as a DCE
 */
void mk5025_clockrate_command (parseinfo *csb)
{
    hwidbtype *idb = csb->interface->hwptr;
    int  i;
    int  rate = 0;
    ushort status;
    serialsb *ssb;

    if (csb->sense) {
      rate = GETOBJ(int,1);
    }

    if ((csb->which == CFG_INT_CLOCKRATE) && (rate == 0)) {
        printf("\n  \t%s\n", "Speed (bits per second)");

	for (i = 1; i < NXXCLOCKS; i++) {
	    printf("  %d\n", xxclock[i].rate);
	}
	return;
    }

    status = mk5025_get_interface_state(idb, TRUE);

    ssb = idb_use_hwsb_inline(idb, HWIDB_SB_SERIAL);

    if (csb->nvgen) {
	if (!(status & DSIM_CONTROL_DTE_DCE_SEL)) {
	    nv_write(ssb->serial_clockindex,
		     "%s %d", csb->nv_command,
		     xxclock[ssb->serial_clockindex].rate);
	}
	idb_release_hwsb_inline(idb, HWIDB_SB_SERIAL);
	return;
    }

    /*
     * Scan up through the array looking for the
     * divider index
     */
    for (i = 0; i < NXXCLOCKS; i++) {
	if (rate == xxclock[i].rate)
	    break;
    }

    if (i == NXXCLOCKS) {
	printf("\nunknown clock rate");
	idb_release_hwsb_inline(idb, HWIDB_SB_SERIAL);
	return;
    }

    /*
     * Always store the clock index irregardless of
     * whether the cable is DTE or DCE
     */
    ssb->serial_clockindex = i;
    
    if (!(status & DSIM_CONTROL_DTE_DCE_SEL) || !csb->sense) {
	/*
	 * Reset the interface for the clockrate to take
	 * effect
	 */
	mk5025_init(idb);
    }
}

/*
 * Show MK5025 structure pointers
 */
void mk5025_show_structure_info (hwidbtype *idb)
{
    mk5025_instance_t *ds = INSTANCE;

    printf("NIM slot %d, NIM type code %d, NIM version %d\n",
	   idb->slot, idb->nim_type, idb->nim_version);
    printf("idb = 0x%x, driver structure at 0x%x, regaddr = 0x%x\n",
	   idb, ds, ds->regaddr);
}


/*
 * Show MK5025 cable state
 */
void mk5025_show_cable_state (hwidbtype *idb)
{
    ushort status, cable_state, cable_dte;
    serialsb *ssb;

    status = mk5025_get_interface_state(idb, TRUE);
    cable_state = DSIM_CABLE_TYPE(status);
    cable_dte   = (status & DSIM_CONTROL_DTE_DCE_SEL);
    ssb = idb_get_hwsb_inline(idb, HWIDB_SB_SERIAL);

    if (cable_state == STATUS_CABLE_NONE) {
	printf("No serial cable attached\n");
	return;
    }

    /*
     * Print the applique type
     */
    if (cable_dte)
	printf("DTE ");
    else
	printf("DCE ");

    /*
     * Print up the interface type
     */
    switch (cable_state) {
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
        printf("Unknown (%d)", cable_state);
    }
    printf(" serial cable attached");

    /*
     * If a DCE, print the clockrate too
     */
    if (!cable_dte) {

	if (ssb->serial_clockindex)
	    printf(", clockrate %d", xxclock[ssb->serial_clockindex].rate);
	else
	    printf(", no clock");
    }	    

    printf("\n");
}


/*
 * init_mk5025_idb -- set up an idb for an mk5025 chip
 */
static void init_mk5025_idb (uint slot, uint unit, uint subunit)
{
    hwidbtype *idb;
    mk5025_instance_t *ds;

    /*
     * Set up storage, add IDB to interface queue
     */
    idb = idb_create();
    if (!idb) {
	crashdump(0);
    }
    
    idb->slot = slot;		/* save our NIM slot number */
    idb->unit = unit;		/* unit number */
    idb->subunit = subunit;	/* subunit number */

    mk5025_init_idb_common(idb);

    idb->serial_flags |= SERIAL_FLAGS_CAN_INT_TX | SERIAL_FLAGS_NO_DTE_INT_TX |
	SERIAL_FLAGS_CAN_CLK_INV| SERIAL_FLAGS_NO_DCE_CLK_INV;

    /*
     * Initialize the interface:
     */
    mk5025_init(idb);

     /*
      * Save our NIM type code, and NIM version:
      */
    ds = INSTANCE;
    idb->nim_type = ((ushort) ds->dsimaddr->id_prom[0] & 0xff);
    idb->nim_version = ((ushort) ds->dsimaddr->id_prom[1] & 0xff);
}


/*
 * mk5025_analyze_interface ()
 *
 * Initialize a MK5025 serial NIM at the given slot.
 *
 */
static void mk5025_analyze_interface (int slot)
{
    int subunit = 0;
    ushort type;

    type = nim_get_type(slot);
    if (type == XX_SERIAL_SINGLE) {
	init_mk5025_idb(slot, nserialnets++, subunit++);
    }
    if (type == XX_SERIAL_DUAL) {
	init_mk5025_idb(slot, nserialnets++, subunit++);
	init_mk5025_idb(slot, nserialnets++, subunit++);
    }
}


/*
 * MK5025 subsystem initialisation entry point
 */
void mk5025_subsys_init (subsystype *subsys)
{
    /*
     * Register the NIM IDs for the various MK5025 NIMs
     */
    reg_add_analyze_interface(XX_SERIAL_SINGLE,
			      mk5025_analyze_interface,
			      "mk5025_analyze_interface");
    reg_add_analyze_interface(XX_SERIAL_DUAL,
			      mk5025_analyze_interface,
			      "mk5025_analyze_interface");
}
