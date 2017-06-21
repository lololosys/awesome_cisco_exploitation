/* $Id: if_c3000_hd64570.c,v 3.5.12.6 1996/08/09 19:51:04 srane Exp $
 * $Source: /release/112/cvs/Xsys/les/if_c3000_hd64570.c,v $
 *------------------------------------------------------------------
 * if_c3000_hd64570.c - C3000 driver for Hitachi serial controller
 *
 * August 1992, Greg Stovall
 *
 * Copyright (c) 1992-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: if_c3000_hd64570.c,v $
 * Revision 3.5.12.6  1996/08/09  19:51:04  srane
 * CSCdi64156:  2500 remains in loopback mode when DTE cable is plugged in
 * Branch: California_branch
 * Routine hd64570_enter_transparent_mode() is modified to check for cable
 * present before asserting DCD
 *
 * Revision 3.5.12.5  1996/06/17  08:33:37  jhernand
 * Commit Modular Cobra to California_branch
 *
 * Revision 3.5.12.4  1996/06/13  19:07:51  etrehus
 * Commit boa to California Branch
 *
 * Revision 3.5.12.3  1996/04/27  06:32:38  syiu
 * Branch: California_branch
 * Commit Dial1_branch
 *
 * Revision 3.4.8.3  1996/04/26  01:40:57  syiu
 * Branch: Dial1_branch
 * Dial1_branch to IosSec_postmerge_960419
 *
 * Revision 3.4.8.2  1996/04/17  03:33:31  syiu
 * Branch: Dial1_branch
 * Sync Dial1_branch to V111_1_3
 *
 * Revision 3.4.8.1  1996/02/23  20:51:37  lbustini
 * Branch: Dial1_branch
 * Move dialer fields out of the idb.
 *
 * Revision 3.5.12.2  1996/04/03  01:59:12  sagarwal
 * CSCdi51875:  Serial Int. MTU change doesnt get new buffers if pulsetime
 * confged
 * Branch: California_branch
 *  Compare current bufferpool size against idb maxdgram instead of
 *  relying on boolean passed to controller init to remember mtu
 *  changes across DTR pulse intervals
 *
 * Revision 3.5.12.1  1996/03/18  20:41:10  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.4.12.4  1996/03/13  01:49:00  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.4.12.3  1996/03/09  05:07:23  hampton
 * Slim down the kernel of the router.
 * Branch: DeadKingOnAThrone_branch
 *
 * Revision 3.4.12.2  1996/02/20  01:08:17  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.5  1996/02/13  21:26:07  rbatz
 * CSCdi30258:  Fixed the following:
 *                  Duplicate half-duplex commands.
 *                  Could not do half-duplex stun.
 *                  Could not tune half-duplex timers for bstun.
 *
 * Revision 3.4.12.1  1996/01/24  22:22:57  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.4  1995/12/13  01:06:15  jbalestr
 * CSCdi45427:  RTS timeout loses bsc frames
 *     Added txCompNotify hook on RTS timeout event
 *
 * Revision 3.3  1995/11/17  17:38:17  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:57:28  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:30:56  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.7  1995/09/28  15:02:47  ssangiah
 * CSCdi41201:  DSR not deasserted on administratively shutdown DCE
 *              interfaces of the 2500 series platforms.
 *
 * Revision 2.6  1995/09/03  01:06:42  arothwel
 * CSCdi39398:  Stop BSC crashing: abort outstanding Tx/Rx DMA ops.
 *
 * Revision 2.5  1995/08/29  14:19:13  mgmiles
 * CSCdi28337:  Show controller mci: No DCE cable attached
 * Removed the "DCE/DTE" phrase from the "cable not present" message
 * in the c3000 and c2500 builds.
 *
 * Revision 2.4  1995/07/18  06:47:10  hampton
 * Miscellaneous changes to eliminate direct references to the system
 * clock and to convert to always using the timer macros to access timers.
 * [CSCdi37217]
 *
 * Revision 2.3  1995/06/19 06:46:59  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.2  1995/06/13  21:36:41  vnguyen
 * CSCdi34135:  DTR kept high on shutdown interfaces on 2500 with attached
 * cable
 * Deassert modem signals for shutdown serial interfaces when plugging in
 * cable
 *
 * Revision 2.1  1995/06/07  21:32:51  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../../boot/cpu.h"
#include "interface_private.h"
#include "media_registry.h"
#include "logger.h"
#include "stacks.h"
#include "../if/network.h"

#include "if_les.h"
#include "if_les_hd64570.h"

#include "../wan/serial.h"
#include "../wan/serial_parser.h"
#include "../os/free.h"
#include "../if/linkdown_event.h"
#include "../bstun/bstun.h"
#include "../wan/serial.h"
#include "../wan/dialer_registry.h"
#include "../wan/serial_private.h"


/*
 * hd64570_enable_modem_interrupt()
 *
 * Enables modem control line interrupts for a given
 * interface.
 */
void hd64570_enable_modem_interrupt (hwidbtype *idb)
{
    hd64570_instance_t *ds;

    ds = INSTANCE;

    *(ds->serial_dev_reg) |= DEVICE_SERIAL_INT_ENB;
}


/*
 * hd64570_disable_modem_interrupt()
 *
 * Disables modem control line interrupts for a given
 * interface.
 */
void hd64570_disable_modem_interrupt (hwidbtype *idb)
{
    hd64570_instance_t *ds;
    
    ds = INSTANCE;

    /* 
     * WARNING: Hack for C2500s.  We do not want to disable
     * modem interrupts if the serial port is shutdown since
     * HW raises the modem control signal (i.e. DTR) when a 
     * cable is plugged in even though the serial is shutdown.
     * We need to be interrupted to reset the modem line
     * correctly.
     */
    if (cpu_type == CPU_CANCUN) {
        if (idb && (idb->state != IDBS_ADMINDOWN)) {
            *(ds->serial_dev_reg) &= ~DEVICE_SERIAL_INT_ENB;
        } 
    } else {
        *(ds->serial_dev_reg) &= ~DEVICE_SERIAL_INT_ENB;
    }
}


/*
 * hd64570_setup_int()
 * Setup interrupt type stuff
 */
void hd64570_setup_int (hwidbtype *idb)
{
    int inputType;
    
    /* find the type of the interface currently being configured */
    if ( idb->poll_protocol_flags & PP_ACTIVE){
	inputType = 2;
    }
    else if (idb->hdx_flags & HDX_FLAG_ON) {
	inputType = 1;
    }
    else {
	inputType = 0;
    }
  
    if (idb->unit == 0) {
	if ((pan_int_array[3].function == (ulong)level4_nop) ||
	    (pan_int_array[3].function == (ulong)level4_cristal_tnil) ||
	    (pan_int_array[3].function == (ulong)level4_cristal_thdnil) ||
	    (pan_int_array[3].function == (ulong)level4_cristal_bscnil)) {
	    /* no wrapper on channel 1 */
	    switch ( inputType ){
	      case 0:
		pan_int_array[3].function = (ulong)level4_cristal_tnil;
		break;
	      case 1:
		pan_int_array[3].function = (ulong)level4_cristal_thdnil;
		break;
	      case 2:
		pan_int_array[3].function = (ulong)level4_cristal_bscnil;
		break;
	    }
	}
	else if ((pan_int_array[3].function == (ulong)level4_cristal_nilt) ||
		 (pan_int_array[3].function == (ulong)level4_cristal_tt) ||
		 (pan_int_array[3].function == (ulong)level4_cristal_thdt) ||
		 (pan_int_array[3].function == (ulong)level4_cristal_bsct)) {
	    /* serial on channel 1 */
	    switch ( inputType ){
	      case 0:
		pan_int_array[3].function = (ulong)level4_cristal_tt;
		break;
	      case 1:
		pan_int_array[3].function = (ulong)level4_cristal_thdt;
		break;
	      case 2:
		pan_int_array[3].function = (ulong)level4_cristal_bsct;
		break;
	    }
	}
	else if ((pan_int_array[3].function == (ulong)level4_cristal_nilthd) ||
		 (pan_int_array[3].function == (ulong)level4_cristal_tthd) ||
		 (pan_int_array[3].function == (ulong)level4_cristal_thdthd) ||
		 (pan_int_array[3].function == (ulong)level4_cristal_bscthd)) {
	    /* half duplex on channel 1 */
	    switch ( inputType ){
	      case 0:
		pan_int_array[3].function = (ulong)level4_cristal_tthd;
		break;
	      case 1:
		pan_int_array[3].function = (ulong)level4_cristal_thdthd;
		break;
	      case 2:
		pan_int_array[3].function = (ulong)level4_cristal_bscthd;
		break;
	    }
	}
	else if ((pan_int_array[3].function == (ulong)level4_cristal_nilbsc) ||
		 (pan_int_array[3].function == (ulong)level4_cristal_tbsc) ||
		 (pan_int_array[3].function == (ulong)level4_cristal_thdbsc) ||
		 (pan_int_array[3].function == (ulong)level4_cristal_bscbsc)) {
	    /* bsc on channnel 1 */
	    switch ( inputType ){
	      case 0:
		pan_int_array[3].function = (ulong)level4_cristal_tbsc;
		break;
	      case 1:
		pan_int_array[3].function = (ulong)level4_cristal_thdbsc;
		break;
	      case 2:
		pan_int_array[3].function = (ulong)level4_cristal_bscbsc;
		break;
	    }
	} else {
	    buginf("\nUnknown interrupt wrapper");
	}
	
	pan_int_array[3].idb1 = idb;
    } else {
	/*
	 * configure second port
	 */
	if ((pan_int_array[3].function == (ulong)level4_nop) ||
	    (pan_int_array[3].function == (ulong)level4_cristal_nilt) ||
	    (pan_int_array[3].function == (ulong)level4_cristal_nilthd) ||
	    (pan_int_array[3].function == (ulong)level4_cristal_nilbsc)) {
	    /*
	     * currently no interrupt wrapper on channel 0
	     */
	    switch ( inputType ){
	      case 0:
		pan_int_array[3].function = (ulong)level4_cristal_nilt;
		break;
	      case 1:
		pan_int_array[3].function = (ulong)level4_cristal_nilthd;
		break;
	      case 2:
		pan_int_array[3].function = (ulong)level4_cristal_nilbsc;
		break;
	    }
	}
	else if ((pan_int_array[3].function == (ulong)level4_cristal_tnil) ||
		 (pan_int_array[3].function == (ulong)level4_cristal_tt)||
		 (pan_int_array[3].function == (ulong)level4_cristal_tthd) ||
		 (pan_int_array[3].function == (ulong)level4_cristal_tbsc)) {
	    /* serial on channel 0 */
	    switch ( inputType ){
	      case 0:
		pan_int_array[3].function = (ulong)level4_cristal_tt;
		break;
	      case 1:
		pan_int_array[3].function = (ulong)level4_cristal_tthd;
		break;
	      case 2:
		pan_int_array[3].function = (ulong)level4_cristal_tbsc;
		break;
	    }
	}
	else if ((pan_int_array[3].function == (ulong)level4_cristal_thdnil) ||
		(pan_int_array[3].function == (ulong)level4_cristal_thdt) ||
		(pan_int_array[3].function == (ulong)level4_cristal_thdthd) ||
		(pan_int_array[3].function == (ulong)level4_cristal_thdbsc)) {
	    /* half duplex on channnel 0 */
	    switch ( inputType ){
	      case 0:
		pan_int_array[3].function = (ulong)level4_cristal_thdt;
		break;
	      case 1:
		pan_int_array[3].function = (ulong)level4_cristal_thdthd;
		break;
	      case 2:
		pan_int_array[3].function = (ulong)level4_cristal_thdbsc;
		break;
	    }
	}
	else if ((pan_int_array[3].function == (ulong)level4_cristal_bscnil)||
		(pan_int_array[3].function == (ulong)level4_cristal_bsct) ||
		(pan_int_array[3].function == (ulong)level4_cristal_bscthd) ||
		(pan_int_array[3].function == (ulong)level4_cristal_bscbsc)) {
	    /* bsc on channel 0 */
	    switch ( inputType ){
	      case 0:
		pan_int_array[3].function = (ulong)level4_cristal_bsct;
		break;
	      case 1:
		pan_int_array[3].function = (ulong)level4_cristal_bscthd;
		break;
	      case 2:
		pan_int_array[3].function = (ulong)level4_cristal_bscbsc;
		break;
	    }
	} else {
	    buginf("\nUnknown interrupt wrapper");
	}
	
	pan_int_array[3].idb2 = idb;
    }
}


/*
 * pan_dual_serial_delta_int()
 * This function is a interrupt service routine for a level 3
 * modem control interrupt. An interrupt is generated whenever
 * a serial port sees a change in CTS, DCD or DSR. This function
 * checks which serial port caused the interrupt and then changes
 * the state of the interface depending on the cause of the interrupt.
 * This function was written to service one serial interface at a
 * time. If two interfaces interrupt at the same time, one will
 * be serviced and the other's interrupt will not have been cleared
 * so it will be serviced soon after.
 *
 * Code for processing modem control change interrupts does not 
 * have to be as concerned with speed as other ISR's such as 
 * those processing packet transmit/receive interrupts.
 */
static void pan_dual_serial_delta_int (void)
{
    hwidbtype *idb;             /* idb pointer */
    hd64570_instance_t *ds;   /* Instance pointer */
    ushort *state;            /* serial modem line state */
    boolean saw_int;          /* This interface saw a modem interrupt */
    int newstate;             /* temp state */
    int i;
    volatile ushort tmp;      /* to clear timer interrupts */
    int cable_present;
    serialsb *ssb;
    

    /*
     * Loop for all serial channels
     */
    for (i=0; i < MAX_HD_SERIAL_CHAN; i++) {
	if (i == 0)
	    idb = network_c0_idb;
	else
	    idb = network_c1_idb;

        /*
         * In certain cases (like for FIJI),
         * only 1 serial port may be used, skip the second one
         * to prevent problems from attempting to change the
         * state of the unused interface
         */
        if ((idb == (hwidbtype *)BAD_ADDRESS) || (idb == NULL))
          continue;

	/* set the instance ptr based on the idb */
	ds = INSTANCE;
	ssb = idb_get_hwsb_inline(idb, HWIDB_SB_SERIAL);

	state = ds->serial_dev_reg;
	if (*state & DEVICE_SERIAL_INT_CLR) {
	    if (serial_debug)
		buginf("\nHD(%d): got an interrupt state = 0x%04x", idb->unit,
		       *state);
	    saw_int = TRUE;
	} else
	    saw_int = FALSE;

	*state &= ~DEVICE_SERIAL_INT_CLR;

        newstate = idb->state;	
	cable_present = FALSE;
	if (serial_debug)
	    buginf("\nHD(%d): New serial state = 0x%04x\n",
	            idb->unit, *state);

	/* 
	 * WARNING: Terrible Cancun hack here.  We need to decide if
	 * the is no cable connected.  The bits to determine cable
	 * status are spread all over hell.  The following is an attempt
	 * to try to make this all a little more orderly.
	 * hopefully they will fix the ASIC and we can get the cable
	 * info out of just one register.
	 */
	if (cpu_type == CPU_CANCUN) {
	    unsigned long state_snap;
	    state_snap = (((*state) >> CRISTAL_CABLE_SHIFT) &
	  	          CRISTAL_CABLE_MASK);
	    if ((ds->msci_reg->st3_st2) & CANCUN_CTS_MASK) {
	        state_snap |= CANCUN_CID2_BIT; 
  	    }
	    cable_present = (state_snap != CANCUN_CABLE_NONE);
	} else {
	    cable_present = (((*state) >> CRISTAL_CABLE_SHIFT) & 
	 		     CRISTAL_CABLE_MASK) != CRISTAL_CABLE_NONE;
	}

	if (cable_present) {
            /*
             * Check interface up/down status, maybe start some output.
             */
	    if (*state & DEVICE_SERIAL_DTE) {
                if (idb && (idb->state != IDBS_ADMINDOWN)) {
	 	    if (*state & ds->up_signal) {
		        newstate = IDBS_UP;
		        if (serial_debug)
			    buginf("\nHD(%d): %s is up.", idb->unit,
				    (ssb->ignore_dcd ? "DSR" :"DCD"));
		     } else {
		 	 newstate = IDBS_DOWN; /* No DCD, line is down */

                         /* NmEvent Hook 1 */
                         idb->failCause = Serial_SignalLost;

			 if (serial_debug)
			     buginf("\nHD(%d): %s is down.", idb->unit,
			 	    (ssb->ignore_dcd ? "DSR" :"DCD"));
		     }
                 } else {
                      /*
                       * Port is shutdown so need to clear all modem signals.
                       */
                      *state &= ~(DEVICE_SERIAL_INT_ENB |
                                  DEVICE_SERIAL_CTS     |
                                  DEVICE_SERIAL_RTS     |
                                  DEVICE_SERIAL_DTR     |
                                  DEVICE_SERIAL_DSR     |
                                  DEVICE_SERIAL_DCD);
                 }
	    } else {
                if (idb && (idb->state != IDBS_ADMINDOWN)) {
		    if (*state & DEVICE_SERIAL_DTR) {
			newstate = IDBS_UP;
			if (serial_debug)
			    buginf("\nHD(%d): DTR is up.", idb->unit);
		    } else {
			newstate = IDBS_DOWN; /* No DTR, line is down */

                        /* NmEvent Hook 1 */
                        idb->failCause = Serial_SignalLost;

			if (serial_debug)
			    buginf("\nHD(%d): DTR is down.", idb->unit);
		    }
		    if (*state & DEVICE_SERIAL_LTST) {
			ds->msci_reg->ctl_md2 |= HD64570_MD2_LOOP;
		    } else {	
			ds->msci_reg->ctl_md2 &= ~HD64570_MD2_LOOP;
		    }
		} else {
                     /*
                      * Port is shutdown so need to clear all modem signals.
                      */
                     *state &= ~(DEVICE_SERIAL_INT_ENB |
                                 DEVICE_SERIAL_CTS     |
                                 DEVICE_SERIAL_RTS     |
                                 DEVICE_SERIAL_DTR     |
                                 DEVICE_SERIAL_DSR     |
                                 DEVICE_SERIAL_DCD);
                }
            } 
        } else {
	    /* 
	     * since no cable is present we need to clear all modem
	     * control bits.  Cancun had a problem where unplugging the 
	     * cable caused the interface to go into DCE mode.  The
	     * software then asserted DCD. When a DTE cable was plugged 
	     * back in the formally written DCD bit was remapped (via 
	     * the ASIC) to be read from the LTST bit.  Then a bit set
	     * would "set" the LTST bit and the remote system would
	     * see loopback and we'd have a loopback cable when none
	     * was requested.
	     */
	    *state &= ~(DEVICE_SERIAL_INT_ENB |
	                DEVICE_SERIAL_LTST    |
			DEVICE_SERIAL_CTS     |
			DEVICE_SERIAL_RTS     |
			DEVICE_SERIAL_DSR     |
			DEVICE_SERIAL_DTR     |
			DEVICE_SERIAL_DCD);
	    newstate = IDBS_DOWN;

            /* NmEvent Hook 1 */
            idb->failCause = Serial_SignalLost;

	    if (serial_debug)
	        buginf("\nHD(%d): Cable is unplugged.", idb->unit);
        }

        if (idb->state != IDBS_ADMINDOWN) {
	    if ((idb->state != newstate) &&
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

    	    /*
	     * Check for loopback
	     */
	    if (!(idb->dte_interface)) {
	        if (*state & DEVICE_SERIAL_LTST)
	            ds->msci_reg->ctl_md2 |= HD64570_MD2_LOOP;
	        else
	            ds->msci_reg->ctl_md2 &= ~HD64570_MD2_LOOP;
	    }

	    if (idb->hdx_flags & HDX_FLAG_ON) {
  	        /*
	         * Are we trying to send while in HDX mode?
	         */
	        if (ds->hdx_state == HDX64570_DTE_TX_START)
	            if (*state & DEVICE_SERIAL_CTS) {
	                if (idb->unit == 0) {
                            ds->hd64570_reg->timer_1_tepr_tcsr = 0;
		            tmp = ds->hd64570_reg->timer_1_tepr_tcsr;
		            tmp = ds->hd64570_reg->timer_1_tcnt;
		        } else {
		            ds->hd64570_reg->timer_3_tepr_tcsr = 0; 
		            tmp = ds->hd64570_reg->timer_3_tepr_tcsr;
		            tmp = ds->hd64570_reg->timer_3_tcnt;
                        }

    		        ds->hdx_state = HDX64570_DTE_TX;
		        hd64570_tx_HDX_start(idb);
		    }
		
	        /*
	         * Check for RTS in HDX mode, if we see it, fire up the
	         * CTS delay timer.
	         */
	        if (!(idb->dte_interface)) {
	            if ((*state & DEVICE_SERIAL_RTS) && 
	  	        !(*state & DEVICE_SERIAL_CTS)) {
		        ds->hdx_state = HDX64570_DCE_RX_START;
		        if (idb->unit == 0) {
		            ds->hd64570_reg->timer_0_tcnt = 0;
		            ds->hd64570_reg->timer_0_tconr =
                                ssb->hdx_cts_delay * HD64570_COUNTS_PER_MSEC +
                                HDX_CTS_DELAY_OFFSET;
                            ds->hd64570_reg->timer_0_tepr_tcsr =
                                HD64570_TIMER_PRE| HD64570_TCSR_ECMI |
				    HD64570_TCSR_TME;
	   	        } else {
		            ds->hd64570_reg->timer_2_tcnt = 0;
		            ds->hd64570_reg->timer_2_tconr =
                                ssb->hdx_cts_delay * HD64570_COUNTS_PER_MSEC +
                                HDX_CTS_DELAY_OFFSET;
                            ds->hd64570_reg->timer_2_tepr_tcsr =
                                HD64570_TIMER_PRE | HD64570_TCSR_ECMI |
                                HD64570_TCSR_TME;
	  	        }
	            }

		    /*
		     * Check for drop in RTS
		     */
		    if ((!(*state & DEVICE_SERIAL_RTS)) && 
		        (*state & DEVICE_SERIAL_CTS)) {
		        *(ds->serial_dev_reg) &= ~DEVICE_SERIAL_CTS;
		        /*
		         * See if we got any output to take care of
		         */
	   	        hd64570_tx_HDX_start(idb);
		    }
	        }
	    }
        }

	if (saw_int) {
	    if (++ds->flap_count > ds->flap_rate) {
		hd64570_disable_modem_interrupt(idb);
		ds->flap_enabled = TRUE;
	    }
	    
	    if (!ds->flap_enabled)
		if (!TIMER_RUNNING_AND_SLEEPING(ds->flap_time)) {
		    TIMER_START(ds->flap_time, ONESEC);
		    ds->flap_count = 0;
		}
	}
    }
}


/*
 * hd64570_RX_HDX_BSC_interrupt()
 * hd64570 receive interrupt handler for half_duplex BSC
 */
void hd64570_RX_HDX_BSC_interrupt (hwidbtype *idb)
{
    hd64570_instance_t *ds;
    uchar rxChar;
    uchar *rxLReg;
    boolean overrun = FALSE;
    
    ds = (hd64570_instance_t *)INSTANCE;

    rxLReg = (uchar*)(&ds->msci_reg->trb)+1;

    /*
     * Process all receive characters:
     */

    while (TRUE) {

       /* check for receive overrun happening */
       if (ds->msci_reg->st3_st2 & HD64570_ST2_OVRN ) {
         overrun = TRUE;
       }

       if (ds->msci_reg->st1_st0 & HD64570_ST0_RX_RDY) {
	 /* at least 1 Rx character in FIFO */

	 rxChar = *rxLReg;
	 
	 (*idb->rx_char_int)(idb, rxChar, overrun );
	 
	 if ( overrun )
	     break;
	 
       } else {
	   break;
       }
   }
}


/*
 * hd64570_dte_tx_end()
 * Drop the signals, we be done
 */
static void hd64570_dte_tx_end (hd64570_instance_t *ds)
{
    *(ds->serial_dev_reg) &= ~DEVICE_SERIAL_RTS;
}


/*
 * hd64570_dce_up_cts()
 * Raise CTS
 */
static void hd64570_dce_up_cts (hd64570_instance_t *ds)
{
    if (ds->hdx_state != HDX64570_DCE_TX)
	*(ds->serial_dev_reg) |= DEVICE_SERIAL_CTS; 
}


/*
 * hd64570_dte_timeout()
 * Halt dte bringup
 */
static void hd64570_dte_timeout (hd64570_instance_t *ds)
{
    *(ds->serial_dev_reg) &= ~DEVICE_SERIAL_RTS; 
}


/*
 * hd64570_timer_interrupt()
 * Check for timer interrupts
 */
void hd64570_timer_interrupt (hwidbtype *in_idb)
{
    hwidbtype *idb;
    hd64570_instance_t *ds;
    uchar *hd64570_isr2;
    volatile ushort tmp;

    idb = in_idb;
    ds = INSTANCE;

    hd64570_isr2 = (uchar *)&(ds->hd64570_reg->isr2);
    
    /*
     * Check for the DTE timeout timers
     */
    if (*hd64570_isr2 & HD64570_ISR2_T1IRQ) {
	idb = network_c0_idb;
	ds = INSTANCE;

	/* Disable/clear the interrupt. */
	ds->hd64570_reg->timer_1_tepr_tcsr = 0;
	tmp = ds->hd64570_reg->timer_1_tepr_tcsr;
	tmp = ds->hd64570_reg->timer_1_tcnt;

        if (ds->hdx_state == HDX64570_DTE_TX_START) {
	    /*
	     * Check for the DTE timeout timers
	     */
	    if (!hd64570_dte_tx_ok(ds)) {
	        idb->rts_timeout_count++;
		hd64570_dte_timeout(ds);

		/* Blow out the holdqueues */
		holdq_reset(idb);

                /* BSC code expects tx complete on all frames */
                if (idb->poll_protocol_flags & PP_ACTIVE) {
                    (*idb->tx_comp_notify)(idb);
                } 
	    }
	} else if (ds->hdx_state == HDX64570_DTE_TX_DMA_DONE) {
	    /*
	     * Transmitter have had enough time to start.
             * Enable TX idle interrupt. so we know when the
             * transmitter is done (it has gone to idle state).
	     */
	    ds->hdx_state = HDX64570_DTE_TX_TRANSMITING;
	    ds->msci_reg->ie1_ie0 |= HD64570_IE1_IDLE;
        } else if (ds->hdx_state == HDX64570_DTE_TX_TRANSMIT_DONE) {
            /*
             * It is now safe to drop the RTS.
             */
            ds->hdx_state = HDX64570_NULL;
            hd64570_dte_tx_end(ds);
        }
    }

    /*
     * Check for the CTS delay timers
     */
    if (*hd64570_isr2 & HD64570_ISR2_T0IRQ) {
	idb = network_c0_idb;
	ds = INSTANCE;

	/* Disable/clear the interrupt. */
	ds->hd64570_reg->timer_0_tepr_tcsr = 0;
	tmp = ds->hd64570_reg->timer_0_tepr_tcsr;
	tmp = ds->hd64570_reg->timer_0_tcnt;
        
	if (ds->hdx_state != HDX64570_DCE_TX)
	    hd64570_dce_up_cts(ds);
    }

    if (*hd64570_isr2 & HD64570_ISR2_T3IRQ) {
	idb = network_c1_idb;
	ds = INSTANCE;

	/* Disable/clear the interrupt. */
	ds->hd64570_reg->timer_3_tepr_tcsr = 0;
	tmp = ds->hd64570_reg->timer_3_tepr_tcsr;
	tmp = ds->hd64570_reg->timer_3_tcnt;

	if (ds->hdx_state == HDX64570_DTE_TX_START) {
	    /*
	     * Check for the DTE timeout timers
	     */
	    if (!hd64570_dte_tx_ok(ds)) {
                idb->rts_timeout_count++;
		hd64570_dte_timeout(ds);

		/* Blow out the holdqueues */
		holdq_reset(idb);

                /* BSC code expects tx complete on all frames */
                if (idb->poll_protocol_flags & PP_ACTIVE) {
                    (*idb->tx_comp_notify)(idb);
                } 
	    }
	} else if (ds->hdx_state == HDX64570_DTE_TX_DMA_DONE) {
	    /*
	     * Transmitter have had enough time to start.
             * Enable TX idle interrupt. so we know when the
             * transmitter is done (it has gone to idle state).
	     */
	    ds->hdx_state = HDX64570_DTE_TX_TRANSMITING;
	    ds->msci_reg->ie1_ie0 |= HD64570_IE1_IDLE;
        } else if (ds->hdx_state == HDX64570_DTE_TX_TRANSMIT_DONE) {
            /*
             * It is now safe to drop the RTS.
             */
            ds->hdx_state = HDX64570_NULL;
            hd64570_dte_tx_end(ds);
        }
    }

    if (*hd64570_isr2 & HD64570_ISR2_T2IRQ) {
	idb = network_c1_idb;
	ds = INSTANCE;

	/* Disable/clear the interrupt. */
	ds->hd64570_reg->timer_2_tepr_tcsr = 0;
	tmp = ds->hd64570_reg->timer_2_tepr_tcsr;
	tmp = ds->hd64570_reg->timer_2_tcnt;

	if (ds->hdx_state != HDX64570_DCE_TX)
	    hd64570_dce_up_cts(ds);
    }
}


/*
 * hd64570_msci_reset
 * Reset this channel
 */
void hd64570_msci_reset (hwidbtype *idb)
{
    hd64570_instance_t *ds;

    /*
     * Set up pointers
     */
    ds = INSTANCE;

    /*
     * Deassert DTR, DSR, DCD, RTS, and CTS
     */
    *(ds->serial_dev_reg) &= ~DEVICE_SERIAL_DTR & ~DEVICE_SERIAL_DCD &
	~DEVICE_SERIAL_CTS & ~DEVICE_SERIAL_RTS & ~DEVICE_SERIAL_DSR;
    /*
     * serial debug msg
     * DTE: dropping DTR, RTS
     * DCE: dropping DSR, CTS, DCD
     */
    if (serial_debug)
        if (*ds->serial_dev_reg & DEVICE_SERIAL_DTE)
            buginf("\nHD(%d): Deasserting %s ", idb->unit, "DTR and RTS");
        else
            buginf("\nHD(%d): Deasserting %s ", idb->unit, "DSR, CTS and DCD");

    /*
     * Reset the serial channel
     */
    ds->msci_reg->cmd = HD64570_CMD_CHAN_RST;
    ds->dmac_rx_reg->dcr_dir = HD64570_DCR_ABORT;
    ds->dmac_rx_reg->dmr_dsr = 0;
    ds->dmac_rx_reg->dcr_dir = HD64570_DCR_CLR_FCT;
    ds->dmac_tx_reg->dcr_dir = HD64570_DCR_ABORT;
    ds->dmac_tx_reg->dmr_dsr = 0;
    ds->dmac_tx_reg->dcr_dir = HD64570_DCR_CLR_FCT;
}


/*
 * hd64570_enter_transparent_mode()
 */
void hd64570_enter_transparent_mode (hwidbtype *idb)
{
    volatile ushort *serial_control;
    hd64570_instance_t *ds;
    int cable_present = FALSE;
    
    ds = INSTANCE;
    
    serial_control = ds->serial_dev_reg;
    
    /*
     * Turn on modem control interrupts
     */
    hd64570_enable_modem_interrupt(idb);
    
    /*
     * WARNING: Terrible Cancun hack here.  We need to decide if
     * the is no cable connected.  The bits to determine cable
     * status are spread all over hell.  The following is an attempt
     * to try to make this all a little more orderly.
     * hopefully they will fix the ASIC and we can get the cable
     * info out of just one register.
     */
    if (cpu_type == CPU_CANCUN) {
        unsigned long state_snap;
        state_snap = (((*serial_control) >> CRISTAL_CABLE_SHIFT) &
                      CRISTAL_CABLE_MASK);
        if ((ds->msci_reg->st3_st2) & CANCUN_CTS_MASK) {
            state_snap |= CANCUN_CID2_BIT;
        }
        cable_present = (state_snap != CANCUN_CABLE_NONE);
    }
    else {
        cable_present = (((*serial_control) >> CRISTAL_CABLE_SHIFT) &
                         CRISTAL_CABLE_MASK) != CRISTAL_CABLE_NONE;
    }

    if (cable_present) {

        /*
         * If DTE, assert DTR; If DCE, assert DSR.
         * Do this before checking for DCD if we are a DTE,
         * so that DTR->DCD loopbacks will work:
         */
        if (*serial_control & DEVICE_SERIAL_DTE) {
	    *serial_control |= DEVICE_SERIAL_DTR;
            if (serial_debug)
                buginf("\nHD(%d): Asserting %s ", idb->unit, "DTR");
        } else {
	    *serial_control |= DEVICE_SERIAL_DSR;
            if (serial_debug)
                buginf("\nHD(%d): Asserting %s ", idb->unit, "DSR");
        } 
        /*
         * If DCE assert DCD and assert/deassert CTS
         */
        if (!(*serial_control & DEVICE_SERIAL_DTE)) {
	    *serial_control |= DEVICE_SERIAL_DCD;
	    if (idb->hdx_flags & HDX_FLAG_ON) {
	        *serial_control &= ~DEVICE_SERIAL_CTS;
                if (serial_debug)
                    buginf("\nHD(%d): Asserting %s; Deasserting %s ", idb->unit, "DCD", "CTS");
	    } else {
	        *serial_control |= DEVICE_SERIAL_CTS;
                if (serial_debug)
                    buginf("\nHD(%d): Asserting %s ", idb->unit, "DCD and CTS");
            }
        }

        /*
         * Say we're up or down based on DCD/DSR or DTR:
         */
        if (*serial_control & DEVICE_SERIAL_DTE) {
	    if (*serial_control & ds->up_signal)
	        net_cstate(idb, IDBS_UP); 
	    else
	        net_cstate(idb, IDBS_DOWN);
        } else {
	    if (*serial_control & DEVICE_SERIAL_DTR)
	        net_cstate(idb, IDBS_UP);
	    else
	        net_cstate(idb, IDBS_DOWN);
        }
    
        /*
         * Check for loopback
         * If we're Boa with an integrated CSU/DSU don't assert LTST
         * as loopbacks have to be initiated by a command write to CSU/DSU
         */
        if ((idb->loopback) && (!ds->csu_dsu_info)) {
	    *serial_control |= DEVICE_SERIAL_LTST;
            if (serial_debug)
                buginf("\nHD(%d): Asserting %s ", idb->unit, "LTST");
        } else {
	    *serial_control &= ~DEVICE_SERIAL_LTST;
            if (serial_debug)
                buginf("\nHD(%d): Deasserting %s ", idb->unit, "LTST");
        }

        /*
         * If DTE assert DTR and assert/deassert RTS:
         */
        if (idb->hdx_flags & HDX_FLAG_ON) {
	    *serial_control |= DEVICE_SERIAL_DTR;
	    *serial_control &= ~DEVICE_SERIAL_RTS;
            if (serial_debug)
                buginf("\nHD(%d): Asserting %s; Deasserting %s ", idb->unit, "DTR", "RTS");
        } else {
	    *serial_control |= DEVICE_SERIAL_DTR | DEVICE_SERIAL_RTS;
            if (serial_debug)
                buginf("\nHD(%d): Asserting %s ", idb->unit, "DTR and RTS");
        }
    }
    else {
        /*
         * since no cable is present we need to clear all modem
         * control bits.  Cancun had a problem where unplugging the
         * cable caused the interface to go into DCE mode.  The
         * software then asserted DCD. When a DTE cable was plugged
         * back in the formally written DCD bit was remapped (via
         * the ASIC) to be read from the LTST bit.  Then a bit set
         * would "set" the LTST bit and the remote system would
         * see loopback and we'd have a loopback cable when none
         * was requested.
         */
        *serial_control &= ~(DEVICE_SERIAL_INT_ENB |
                    DEVICE_SERIAL_LTST |
                    DEVICE_SERIAL_CTS |
                    DEVICE_SERIAL_RTS |
                    DEVICE_SERIAL_DSR |
                    DEVICE_SERIAL_DTR |
                    DEVICE_SERIAL_DCD);
        /*
         * serial debug msg
         * DTE: dropping DTR, RTS
         * DCE: dropping DSR, CTS, DCD, LTST
         */
        if (serial_debug) {
            if (*serial_control & DEVICE_SERIAL_DTE)
                buginf("\nHD(%d): Deasserting %s ", idb->unit, "DTR and RTS");
            else
                buginf("\nHD(%d): Deasserting %s ", idb->unit, "DSR, CTS, DCD and LTST");
        }

        net_cstate(idb, IDBS_DOWN);
    }


    /*
     * Enable RX, and TX.
     */
    hd64570_dmer_pcr_write(ds, HD64570_EVEN_BYTE_ACCESS, HD64570_DMER_DME);
    ds->msci_reg->cmd = HD64570_CMD_TX_ENB;
    ds->msci_reg->cmd = HD64570_CMD_RX_ENB;
}

/*
 * Set up the port's fifo thresholds for the transmit and receive channels.
 */
void hd64570_set_fifo_limits (hwidbtype *idb)
{
    hd64570_instance_t *ds;
    volatile hd64570_msci_reg_t *msci_reg;

    ds = INSTANCE;

    msci_reg = ds->msci_reg;

    if (idb->poll_protocol_flags & PP_ACTIVE) {
        msci_reg->rrc = HD64570_RX_BSC_MARKER;
    } else {
        msci_reg->rrc = HD64570_RX_MARKER;
    }
    msci_reg->trc1_trc0 = HD64570_TX_HIGH_MARKER | HD64570_TX_LOW_MARKER;
}


/*
 * hd64570_init_config()
 * Initialize/configure the modem control bits.
 */
void hd64570_init_config (hwidbtype *idb)
{
    hd64570_instance_t *ds;
    int index;
    serialsb *ssb;
    
    ds = INSTANCE;

    ssb = idb_get_hwsb_inline(idb, HWIDB_SB_SERIAL);

    /*
     * If DCE set up clock rate
     */
    if (!(*(ds->serial_dev_reg) & DEVICE_SERIAL_DTE)) {
	idb->dte_interface = FALSE;
       
	index = ssb->serial_clockindex;

	/*
	 * Set up receiver, if no index then use RXC as not to generate any
	 * clock.
	 */
	if ((ssb->dce_terminal_timing_enable) || (index == 0)) {
	    ds->msci_reg->txs_rxs = HD64570_RXS_RXC;
	} else {
	    ds->msci_reg->txs_rxs = HD64570_RXS_BRG | 
                                    hd64570_clock[index].divider;
	}
	
	/*
	 * Set up transmitter and baud rate, if no index then use TXC as not
	 * to generate any clock.
	 */
	if (index == 0) {
	    ds->msci_reg->txs_rxs |= HD64570_TXS_TXC; 
	} else {
	    ds->msci_reg->txs_rxs |= HD64570_TXS_BRG | 
                                    hd64570_clock[index].divider << 8;
	    ds->msci_reg->tmc_idl |= hd64570_clock[index].counter << 8;
	}
    } else {
	idb->dte_interface = TRUE;
	ds->msci_reg->txs_rxs = HD64570_RXS_RXC | HD64570_TXS_TXC;
    }
	

    if (ssb->invert_txc) {
	if (!(*(ds->serial_dev_reg) & DEVICE_SERIAL_TXC_INV)) 
	    *(ds->serial_dev_reg) |= DEVICE_SERIAL_TXC_INV;
	else
	    *(ds->serial_dev_reg) &= ~DEVICE_SERIAL_TXC_INV;
    }

    /*
     * As a DTE,
     * If ignore-dcd command is issued, use DSR
     * instead of DCD as the link up/down indicator.
     */
    if (*(ds->serial_dev_reg) & DEVICE_SERIAL_DTE) {
        if (ssb->ignore_dcd) 
            ds->up_signal = DEVICE_SERIAL_DSR;
        else    
            ds->up_signal = DEVICE_SERIAL_DCD;
    }
}


/*
 * hd64570_dmer_pcr_write()
 * Does a 8 bit access when writing either register.
 * Note: These two register can only be accessed in 8 bit mode.
 */
void hd64570_dmer_pcr_write (hd64570_instance_t *ds,
			     boolean even_flag,
			     u_char value)
{
    hd64570_reg_t *hd64570_reg;
    u_char *byte;

    hd64570_reg = (hd64570_reg_t *)ds->hd64570_reg;

    byte = (u_char *)&hd64570_reg->dmer_pcr;
    if (!even_flag)
        byte++;

    *byte = value;
}

/*
 * hd64570_dte_tx_ok()
 * Check to see if we're allowed to transmit in HDX mode
 */
boolean hd64570_dte_tx_ok (hd64570_instance_t *ds)
{
    if ((*(ds->serial_dev_reg) & DEVICE_SERIAL_CTS) && 
	(*(ds->serial_dev_reg) & DEVICE_SERIAL_RTS))
	return(TRUE);

    return(FALSE);
}


/*
 * hd64570_dte_tx_request()
 * Start doing the modem signal dance to start TX
 */
boolean hd64570_dte_tx_request (hwidbtype *idb)
{
    hd64570_instance_t *ds;
    serialsb *ssb;

    ds = INSTANCE;
    ssb = idb_get_hwsb_inline(idb, HWIDB_SB_SERIAL);

    if (!(*(ds->serial_dev_reg) & DEVICE_SERIAL_RTS))
	*(ds->serial_dev_reg) |= DEVICE_SERIAL_RTS;

    if ((*(ds->serial_dev_reg) & DEVICE_SERIAL_CTS) == 0) {
	ds->hdx_state = HDX64570_DTE_TX_START;

	/*
	 * start dte_timeout timer
	 */
	if (idb->unit == 0) {
	    ds->hd64570_reg->timer_1_tcnt = 0;
	    ds->hd64570_reg->timer_1_tconr = ssb->hdx_rts_timeout * HD64570_COUNTS_PER_MSEC;
	    ds->hd64570_reg->timer_1_tepr_tcsr = 
		HD64570_TIMER_PRE | HD64570_TCSR_ECMI | HD64570_TCSR_TME;
	} else {
	    ds->hd64570_reg->timer_3_tcnt = 0;
	    ds->hd64570_reg->timer_3_tconr = ssb->hdx_rts_timeout * HD64570_COUNTS_PER_MSEC;
	    ds->hd64570_reg->timer_3_tepr_tcsr = 
		HD64570_TIMER_PRE | HD64570_TCSR_ECMI | HD64570_TCSR_TME;
	}
        return (FALSE);
    } else {
	/*
	 * CTS is already up.
	 */
        return (TRUE);
    }
}


/*
 * hd64570_dce_tx_ok()
 * Check to see if it's OK to send while DCE HDX
 */
boolean hd64570_dce_tx_ok (hd64570_instance_t *ds)
{
    if (*(ds->serial_dev_reg) & DEVICE_SERIAL_CTS)
	return(FALSE);

    return(TRUE);
}

/*
 * Show HD64570 structure pointers
 */
void
hd64570_show_structure_info (hwidbtype *idb)
{
    hd64570_instance_t *ds = INSTANCE;
 
    printf("idb = 0x%x, driver structure at 0x%x\n", idb, ds);
}


/*
 * hd64570_show_cable_state()
 */
void hd64570_show_cable_state (hwidbtype *idb)
{
    hd64570_instance_t *ds = INSTANCE;
    ushort status;
    serialsb *ssb;

    ssb = idb_get_hwsb_inline(idb, HWIDB_SB_SERIAL);

    printf("HD unit %d, ", idb->unit);

    /* 
     * Check cable type.
     */
    status = *(ds->serial_dev_reg);

    status = (status >> CRISTAL_CABLE_SHIFT) & CRISTAL_CABLE_MASK;

    /*
     * If we have a CSU/DSU module, we need more information
     */
    reg_invoke_csu_dsu_get_status_value(idb, ds, &status);

    if (cpu_type == CPU_CANCUN) {
	if ((ds->msci_reg->st3_st2) & CANCUN_CTS_MASK) {
		status |= CANCUN_CID2_BIT; 
	    }
	switch (status) {
	  case CANCUN_CABLE_NONE:
	    printf("No ");
	    break;
	  case CANCUN_CABLE_232:
	    printf("RS-232 ");
	    break;
	  case CANCUN_CABLE_449:
	    printf("RS-449 ");
	    break;
	  case CANCUN_CABLE_V35:
	    printf("V.35 ");
	    break;
	  case CANCUN_CABLE_X21:
	    printf("X.21 ");
	    break;
	  case CANCUN_CABLE_RS530:
	    printf("RS-530 ");
	    break;
	  default:
            if (!reg_invoke_csu_dsu_show_cable_state(status, TRUE)) {
	        printf("Unknown (%d) ",status);
	    }
	}

    } else {
	switch (status) {
	  case CRISTAL_CABLE_NONE:
	    printf("No ");
	    break;
	  case CRISTAL_CABLE_232:
	    printf("RS-232 ");
	    break;
	  case CRISTAL_CABLE_449:
	    printf("V.11 (RS-449 or X.21) ");
	    break;
	  case CRISTAL_CABLE_V35:
	    printf("V.35 ");
	    break;
	  default:
	    printf("Unknown (%d) ",status);
	}
    }
    if ((status == CRISTAL_CABLE_NONE) || (status == CANCUN_CABLE_NONE))
        ;                /* Neither cable present, print nothing */
    else if (reg_invoke_csu_dsu_show_cable_state(status, FALSE)) {
	/* For CSU/DSU modules, print nothing more */
	printf("\n");
	return;
    }
    else if (!(*(ds->serial_dev_reg) & DEVICE_SERIAL_DTE))
	printf("DCE ");
    else
	printf("DTE ");
	
    printf("cable");

    /*
     * If a DCE, print the clockrate too
     */
    if (!idb->dte_interface) {
        if (ssb->serial_clockindex)
            printf(", clockrate %d",
		   hd64570_clock[ssb->serial_clockindex].rate);
    }
    printf("\n");
}
 
/*
 * Print out DTR, CTS, etc. on serial nim
 */
void hd64570_show_serial_state (hwidbtype* idb, char* leader)
{
    hd64570_instance_t *ds;
    ushort modem_control;
    const char *fmt_str = "     DCD=%s  DSR=%s  DTR=%s  RTS=%s  CTS=%s";
 
    ds = INSTANCE;
 
    /*
     * Be courteous, print all hand shaking signals.
     */
    modem_control = *(ds->serial_dev_reg);
    printf("\n");
    printf(fmt_str, 
        ((modem_control & DEVICE_SERIAL_DCD) ? "up" : "down"),
        ((modem_control & DEVICE_SERIAL_DSR) ? "up" : "down"),
        ((modem_control & DEVICE_SERIAL_DTR) ? "up" : "down"),
        ((modem_control & DEVICE_SERIAL_RTS) ? "up" : "down"),
        ((modem_control & DEVICE_SERIAL_CTS) ? "up" : "down"));
}


/*
 * int hd64570_get_cable_type (hwidbtype *idb)
 */
int hd64570_get_cable_type (hwidbtype *idb)
{
    hd64570_instance_t *ds = INSTANCE;
    ushort status;
 
    /*
     * Check cable type.
     */
    status = *(ds->serial_dev_reg);
    
    status = (status >> CRISTAL_CABLE_SHIFT) & CRISTAL_CABLE_MASK;
    
    if (cpu_type == CPU_CANCUN) {
	/* Get the CID2 bit from the SCA's CTS0 pin. */
	if ((ds->msci_reg->st3_st2) & CANCUN_CTS_MASK) {
	    status |= CANCUN_CID2_BIT;
	}
    }
    return((int) status);
}


/*
 * init_hd64570_idb()
 * Initialize the idbs we'll use and kick the controller
 */
static void init_hd64570_idb (int which)
{
    hwidbtype *idb;              /* idb pointer */
    hd64570_instance_t *ds;    /* instance pointer */

    /*
     * Set up idb storage:
     */
    idb = idb_create();
    if (!idb) {
	crashdump(0);
    }

    idb->unit    = which;
    idb->subunit = which;
    idb->app_type = APPTYPE_NONE;

    hd64570_init_idb_common(idb);

    idb->serial_flags |= SERIAL_FLAGS_NO_DCE_CLK_INV;

    /*
     * Set up address pointers
     */
    ds = INSTANCE;
    ds->hd64570_reg = (hd64570_reg_t *)HD64570_BASE_ADDR;
    if (which == 0) {
	ds->msci_reg = &ds->hd64570_reg->msci_ch0;
	ds->dmac_rx_reg = &ds->hd64570_reg->msci_ch0_rx_dma;
	ds->dmac_tx_reg = &ds->hd64570_reg->msci_ch0_tx_dma;
	ds->serial_dev_reg = (ushort *)ADRSPC_DEVICE_SERIAL_0;
	network_c0_idb = idb;
    } else {
	ds->msci_reg = &ds->hd64570_reg->msci_ch1;
	ds->dmac_rx_reg = &ds->hd64570_reg->msci_ch1_rx_dma;
	ds->dmac_tx_reg = &ds->hd64570_reg->msci_ch1_tx_dma;
	ds->serial_dev_reg = (ushort *)ADRSPC_DEVICE_SERIAL_1;
	network_c1_idb = idb;
    }

    hd64570_clock = hd64570_clock1;

    /*
     * Initialize the Boa driver if an integrated CSU/DSU is present
     */
    ds->csu_dsu_info = (void *) reg_invoke_csu_dsu_init(idb, which);

    /*
     * Initialize the interface
     */
    hd64570_init(idb);
}


/*
 * analyze_hd64570 - Create IDBs for HD64570's
 */
static int analyze_hd64570 (void)
{
    int count;
    int i;

    count = count_hd64570(cpu_type, cookie);	/* How many interfaces? */

    if (!count)
      return(count);		/* Nothing to do */


    /*
     * Set up the interrupt handler
     */

    createlevel(SERIAL_DELTA_INTLEVEL, pan_dual_serial_delta_int,
		"Serial interface state change interrupt");

    /*
     * Initialize the IDBs
     */

    for (i = 0; i < count; i++) {
	init_hd64570_idb(i);
    }

    return(count);
}


/*
 * HD64570 subsystem initialisation entry point
 */
void hd64570_subsys_init (subsystype *subsys)
{
    /*
     * Discover all the HD64570 serial interfaces in system
     */
    nserialnets += analyze_hd64570();

    reg_add_media_half_or_full_duplex_cmd(IDBTYPE_HD64570, 
                                          serial_half_duplex_command,
                                          "serial_half_duplex_command");
}
