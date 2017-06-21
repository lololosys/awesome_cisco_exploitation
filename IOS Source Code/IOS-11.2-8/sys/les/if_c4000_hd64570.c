/* $Id: if_c4000_hd64570.c,v 3.7.12.6 1996/08/28 12:57:57 thille Exp $
 * $Source: /release/112/cvs/Xsys/les/if_c4000_hd64570.c,v $
 *------------------------------------------------------------------
 * if_c4000_hd64570.c - C4000 driver for Hitachi serial controller (4T NIM).
 *
 * March 1994, Mohsen Mortazavi
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: if_c4000_hd64570.c,v $
 * Revision 3.7.12.6  1996/08/28  12:57:57  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.7.12.5  1996/06/17  08:33:45  jhernand
 * Commit Modular Cobra to California_branch
 *
 * Revision 3.7.12.4  1996/05/17  11:26:20  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.7.2.3  1996/05/05  23:31:36  ppearce
 * Sync to IbuMod_Calif_baseline_960504
 *
 * Revision 3.7.2.2  1996/04/03  19:58:28  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.7.2.1  1996/02/28  21:24:09  ppearce
 * CSCdi50230:  IBU modularity - Remove unnecessary header file includes
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.7.12.3  1996/04/27  06:32:51  syiu
 * Branch: California_branch
 * Commit Dial1_branch
 *
 * Revision 3.5.8.3  1996/04/26  01:41:30  syiu
 * Branch: Dial1_branch
 * Dial1_branch to IosSec_postmerge_960419
 *
 * Revision 3.5.8.2  1996/04/17  03:34:11  syiu
 * Branch: Dial1_branch
 * Sync Dial1_branch to V111_1_3
 *
 * Revision 3.5.8.1  1996/02/23  20:51:44  lbustini
 * Branch: Dial1_branch
 * Move dialer fields out of the idb.
 *
 * Revision 3.7.12.2  1996/04/03  01:59:18  sagarwal
 * CSCdi51875:  Serial Int. MTU change doesnt get new buffers if pulsetime
 * confged
 * Branch: California_branch
 *  Compare current bufferpool size against idb maxdgram instead of
 *  relying on boolean passed to controller init to remember mtu
 *  changes across DTR pulse intervals
 *
 * Revision 3.7.12.1  1996/03/18  20:41:40  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.6.2.3  1996/03/13  01:49:20  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.6.2.2  1996/02/20  01:08:33  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.7  1996/02/13  21:26:11  rbatz
 * CSCdi30258:  Fixed the following:
 *                  Duplicate half-duplex commands.
 *                  Could not do half-duplex stun.
 *                  Could not tune half-duplex timers for bstun.
 *
 * Revision 3.6.2.1  1996/01/24  22:23:26  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.6  1996/01/19  01:54:16  jbalestr
 * CSCdi47126:  change from bdc hdx to bsc fdx locks up 4500
 *
 * Revision 3.5  1996/01/06  03:22:30  hampton
 * Remove extraneous includes of if_timer.h.  Rename if_timer.h and move
 * it to the 'os' directory.  [CSCdi46482]
 *
 * Revision 3.4  1995/12/13  01:06:17  jbalestr
 * CSCdi45427:  RTS timeout loses bsc frames
 *     Added txCompNotify hook on RTS timeout event
 *
 * Revision 3.3  1995/11/17  17:39:21  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:58:27  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:31:45  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/09/03  01:06:45  arothwel
 * CSCdi39398:  Stop BSC crashing: abort outstanding Tx/Rx DMA ops.
 *
 * Revision 2.3  1995/07/18  06:47:23  hampton
 * Miscellaneous changes to eliminate direct references to the system
 * clock and to convert to always using the timer macros to access timers.
 * [CSCdi37217]
 *
 * Revision 2.2  1995/06/19 06:47:19  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.1  1995/06/07  21:34:21  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "interface_private.h"
#include "sys_registry.h"
#include "logger.h"
#include "../les/msg_hd64570.c"		/* Not a typo, see logger.h */
#include "../if/network.h"
#include "if_les.h"
#include "if_les_hd64570.h"
#include "../wan/dialer_registry.h"
#include "../wan/serial.h"
#include "../wan/serial_private.h"
#include "../wan/serial_parser.h"
#include "../os/free.h"
#include "../if/linkdown_event.h"
#include "media_registry.h"

/*
 * Current index to the wrapper type array. 
 * Each port pair share a single wrapper and for each slot. 
 */
static int sca0_wrapper_index[C4000_NUM_SLOTS];
static int sca1_wrapper_index[C4000_NUM_SLOTS];


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

    *ds->int_reg |= QSIM_INT_MODEM_INTR_ENABLE;
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
    
    *ds->int_reg &= ~QSIM_INT_MODEM_INTR_ENABLE;
}


/*
 * hd64570_setup_int()
 * 
 * Setup the primary and secondary interrupt wrappers for this port.
 */
void hd64570_setup_int (hwidbtype *idb)
{
    void **wrapper;
    int current_index;
    int new_index;
    u_char index_mask;
    
    /*
     * Setup the primary interrupt wrapper for this port.
     */
    hd64570_set_vectors(idb);

    /*
     * Now, setup the secondary wrappers for the port.   
     */
    wrapper = (void **)hd64570_get_wrapper_base(idb->slot);

    /*
     * Each port pair share a single wrapper and for each slot. 
     */
    if (idb->subunit < 2)
        current_index = sca0_wrapper_index[idb->slot];
    else {
        wrapper++;
        current_index = sca1_wrapper_index[idb->slot];
    }

    /*
     * Compute the new index.
     */
    if (idb->poll_protocol_flags & PP_ACTIVE) {
        index_mask = HALF_DUPLEX_BSC_INDEX;
    } else if (idb->hdx_flags & HDX_FLAG_ON) {
        index_mask = HALF_DUPLEX_SDLC_INDEX;
    } else {
        index_mask = FULL_DUPLEX_SDLC_INDEX;
    }
        
        
    if (HD64570_EVEN(idb->subunit))
        new_index = (current_index & 0x15) | index_mask << 1;
    else
        new_index = (current_index & ~0x15) | index_mask;

    /* 
     * Update the current index.
     */
    if (idb->subunit < 2)
        sca0_wrapper_index[idb->slot] = new_index;
    else
        sca1_wrapper_index[idb->slot] = new_index;
    
    /*
     * Now assign the wrapper. 
     */
    switch (new_index) {

        case FDX_SDLC_P0:
            *wrapper = qsim_t0;
            break;

        case FDX_SDLC_P1:
            *wrapper = qsim_0t;
            break;

        case HDX_SDLC_P0:
            *wrapper = qsim_thd0;
            break;

        case HDX_SDLC_P1:
            *wrapper = qsim_0thd;
            break;

        case FDX_SDLC_P0 | FDX_SDLC_P1:
            *wrapper = qsim_tt;
            break;

        case HDX_SDLC_P0 | HDX_SDLC_P1:
            *wrapper = qsim_thdthd;
            break;

        case HDX_SDLC_P0 | FDX_SDLC_P1:
            *wrapper = qsim_thdt;
            break;

        case FDX_SDLC_P0 | HDX_SDLC_P1:
            *wrapper = qsim_tthd;
            break;

        case HDX_BSC_P0:
            *wrapper = qsim_thd0_bsc;
            break;

        case HDX_BSC_P1:
            *wrapper = qsim_0thd_bsc;
            break;

        case HDX_BSC_P0 | HDX_BSC_P1:
            *wrapper = qsim_thd_bsc_thd_bsc;
            break;

        case HDX_BSC_P0 | FDX_SDLC_P1:
            *wrapper = qsim_thd0_bsc_t;
            break;

        case FDX_SDLC_P0 | HDX_BSC_P1:
            *wrapper = qsim_t_0thd_bsc;
            break;

        case HDX_BSC_P0 | HDX_SDLC_P1:
            *wrapper = qsim_thd0_bsc_0thd;
            break;

        case HDX_SDLC_P0 | HDX_BSC_P1:
            *wrapper = qsim_thd0_0thd_bsc;
            break;

        default:
            *wrapper = (void *)BAD_ADDRESS;
            break;
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
    leveltype level;
    boolean overrun = FALSE;

    ds = (hd64570_instance_t *)INSTANCE;

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
	    
	    /*---- Absolutely no interruptions while in byte access mode. */
	    level = raise_interrupt_level(ALL_DISABLE);
	    
	    /*
	     * Go into byte access mode and set access to ODD byte
	     */
	    ds->regaddr->ch_A_int |= QSIM_INT_SCA_BYTE_MODE | QSIM_INT_SCA_A0;
	    
	    rxChar = ds->msci_reg->trb;
	    
	    /* Go back to word access mode. */
	    ds->regaddr->ch_A_int &= ~(QSIM_INT_SCA_BYTE_MODE |
				       QSIM_INT_SCA_A0);
	    
	    /*---- Allow higher level interrupts again. */
	    reset_interrupt_level(level);
	    
	    (*idb->rx_char_int)(idb, rxChar, overrun );
	    
	    if ( overrun )
		break;
	    
        } else {
            break;
	}
    }
}


/*
 * hd64570_read_port_id()
 * 
 * Reads the port ID from the 5/1 port adapter, and 
 * sets the 'dte' field of 'idb' accordingly.
 */
void hd64570_read_port_id (hwidbtype *idb)
{
    volatile ushort *modem_reg;
    hd64570_instance_t *ds;
    uchar port_id = 0;
    int i;
    u_char tmp;
    
    ds = INSTANCE;
    
    modem_reg = ds->mdm_reg;
   
    for (i = 0; i < 8 ; i++) {
	/* Zero out the control bits. */
	*modem_reg &= ~PA_CONTROL_BITS; 

	/* Read the ith LSB. */
	*modem_reg |= (i << PA_CONTROL_SHIFT);	
	tmp = (u_char)(*modem_reg & QSIM_MDM_SENSE1);
        if (tmp)
	    port_id |= 1 << i;
    }

    /* Leave the control bits at zero. */
    *modem_reg &= ~PA_CONTROL_BITS;

    if (port_id == PA_NONE) {
        /*
         * No Port Adapter!!! We know it was there during init.
         * Most likely a problem with the back panel's contacts.
         * Bring the interface down, and let the user know.
         */
        errmsg(&msgsym(BADPORTADAPTER, HD), idb->subunit, idb->slot); 
        printf("\nHD(%d): WARNING: 4T: Port Adapter fault. !!!\n", idb->unit);
        net_cstate(idb, IDBS_ADMINDOWN);
        return;
    }
	
    if ((port_id == PA_G703_DTE) || (port_id == PA_G703_DCE)) {
        /* G703 appliques are always a DTE */
        idb->dte_interface = TRUE;
	idb->serial_flags |= SERIAL_FLAGS_NO_CLOCKRATE;
        if (port_id == PA_G703_DTE)
            idb->app_type = APPTYPE_G703_UNBAL;
        else
            idb->app_type = APPTYPE_G703_BAL;
    } else {
        /* 5/1 DTE cable IDs are even. */
        idb->dte_interface = HD64570_EVEN(port_id) ? TRUE : FALSE;
        idb->app_type = APPTYPE_5IN1;
    }

    if (serial_debug) {
        if (ds->port_id != port_id) {
            if (ds->port_id != PA_NO_CABLE && port_id == PA_NO_CABLE)
                buginf("\nHD(%d): Cable is unplugged.\n", idb->unit);
            else 
                buginf("\nHD(%d): New Cable (id=%d) is plugged in.\n", 
                       idb->unit, port_id);
        }
    }

    /* Save the new port id. */
    ds->port_id = port_id;
}


/*
 * hd64570_modem_interrupt()
 * This function is a interrupt service routine for the  modem control 
 * interrupt. An interrupt is generated whenever a serial port sees a 
 * change in BDSR, BCTS, BDCD or BLOOP. This function changes the state 
 * of the interface depending on the cause of the interrupt.
 *
 * Code for processing modem control change interrupts does not 
 * have to be as concerned with speed as other ISR's such as 
 * those processing packet transmit/receive interrupts.
 */

void hd64570_modem_interrupt (hwidbtype *idb)
{
    hd64570_instance_t *ds;
    ushort modem_reg;
    int newstate;
    volatile ushort tmp;
    serialsb *ssb;

    /* set the instance ptr based on the idb */
    ds = INSTANCE;

    modem_reg = *ds->mdm_reg;

    ssb = idb_get_hwsb_inline(idb, HWIDB_SB_SERIAL);

    /*
     * Check interface up/down status, maybe start some output.
     */
    if (idb && (idb->state != IDBS_ADMINDOWN)) {

        if (serial_debug) {
            buginf("\nHD(%d): New serial modem register = 0x%04x\n",
                   idb->unit, modem_reg);
        }

        /*
         * For any change in DCD/DTR, reads the cable id 
         * to determine the possibly new DTE/DCE mode.
         */
        if ((modem_reg & ds->up_signal) ||
            (!(modem_reg & ds->up_signal) && idb->state == IDBS_UP))
                hd64570_read_port_id(idb);
            
        if (modem_reg & ds->up_signal) {
            /*  Line is up. */
            if (serial_debug) {
                buginf("\nHD(%d): %s is up.\n", idb->unit, 
                       idb->dte_interface ? (ssb->ignore_dcd ? "DSR" :"DCD") :
                       "DTR");
            }

            if ((modem_reg & QSIM_MDM_LOOP) == 0) {
                    newstate = IDBS_UP;
            } else {
                /* loopback mode. */
                if (!idb->dte_interface) {
                    /*
                     * In DCE mode, make sure we are generating 
                     * the clock before declaring the link as up.
                     */
                   newstate = ssb->serial_clockindex ? IDBS_UP : IDBS_DOWN;
                } else {
                    newstate = IDBS_UP;
                }
            }
        } else {
            /* Line is down. */
            newstate = IDBS_DOWN;
            idb->failCause = Serial_SignalLost;
            if (serial_debug) {
                buginf("\nHD(%d): %s is down.\n", idb->unit, 
                       idb->dte_interface ? (ssb->ignore_dcd ? "DSR" : "DCD") :
                       "DTR");
            }
        }
        
        if ((idb->state != newstate) &&
            (idb->state != IDBS_ADMINDOWN) &&

	    !reg_invoke_dialer_spoofing_int(idb)) {
            idb->counters.transitions++;
            net_cstate(idb, newstate);
        }

        if (!idb->dte_interface) {
            /*
             * For a DCE, loopback may be set remotely by the DTE.
             * Make sure the loopback state in the idb structure
             * matches the BLOOP signal on the 5/1 adapter.
             */
	    if (idb->loopback) {
		if ((modem_reg & QSIM_MDM_LOOP) == 0) {  
		    /*
                     * DTE side must have unset the 
                     * loopback mode, set the idb to no loop.
		     */ 
		    idb->loopback = FALSE;
		}
	    } else {
		if (modem_reg & QSIM_MDM_LOOP) {
		    /*
                     * DTE side must have set the
                     * loopback mode, set the idb to loop
		     */
		    idb->loopback = TRUE;
		}
	    }
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

        if (idb->hdx_flags & HDX_FLAG_ON) {
            /*
             * Are we trying to send while in HDX mode?
             */
            if (ds->hdx_state == HDX64570_DTE_TX_START) {
                if (modem_reg & QSIM_DTE_CTS) {
                    /* 
                     * CTS is up, disable (and clear) dte timeout timer. 
                     */
                    if (HD64570_EVEN(idb->subunit)) {
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
            }

            /*
             * Check for RTS in HDX mode, if we see it, fire up the
             * CTS delay timer.
             */
            if (!idb->dte_interface) {
                if ((modem_reg & QSIM_DCE_RTS) &&
                    !(modem_reg & QSIM_DCE_CTS)) {

                    ds->hdx_state = HDX64570_DCE_RX_START;
                    if (HD64570_EVEN(idb->subunit)) {
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
                if ((!(modem_reg & QSIM_DCE_RTS)) &&
                    (modem_reg & QSIM_DCE_CTS)) {

                    *ds->mdm_reg &= ~QSIM_DCE_CTS;
                    /*
                     * See if we got any output to take care of
                     */
                    hd64570_tx_HDX_start(idb);
                }
            }
        }
    }

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

    if (idb->app_type == APPTYPE_5IN1) {
	/*
	 * If DTE, deassert DTR and RTS; If DCE, deassert DCD, CTS.
	 */
	*ds->mdm_reg &= ~(QSIM_MDM_BDTR | QSIM_MDM_BRTS);
        if (serial_debug) {
            if (idb->dte_interface)
                buginf("\nHD(%d): Deasserting %s ", idb->unit, "DTR and RTS");
            else
                buginf("\nHD(%d): Deasserting %s ", idb->unit, "DCD and CTS");
        }
	/*
	 * Reset the serial channel
	 */
	*ds->mdm_reg |= QSIM_MDM_RESET;

	/*
	 * Wait a bit
	 */
	usecdelay(5);
	*ds->mdm_reg &= ~QSIM_MDM_RESET;
    }

    ds->adapter_flags = 0;
    ds->msci_reg->cmd = HD64570_CMD_CHAN_RST;
    ds->dmac_rx_reg->dcr_dir = HD64570_DCR_ABORT;
    ds->dmac_rx_reg->dmr_dsr = 0;
    ds->dmac_rx_reg->dcr_dir = HD64570_DCR_CLR_FCT;
    ds->dmac_tx_reg->dcr_dir = HD64570_DCR_ABORT;
    ds->dmac_tx_reg->dmr_dsr = 0;
    ds->dmac_tx_reg->dcr_dir = HD64570_DCR_CLR_FCT;
}

/*
 * hd64570_loopback()
 * Set/unset the 5/1 adapter in to loopback mode.
 */
static void hd64570_loopback (hwidbtype *idb, boolean set_flag)
{
    volatile ushort *modem_reg;
    hd64570_instance_t *ds;
    ushort cmd = 0;
    
    ds = INSTANCE;
    
    if (idb->app_type != APPTYPE_5IN1)
        return;

    if (set_flag && ds->port_id == PA_X21_DTE) {
        errmsg(&msgsym(BADLOOPCABLE, HD), idb->subunit, idb->slot);
        return;
    }
    
    modem_reg = ds->mdm_reg;

    /*
     * clk==CTRL2, data=CTRL1, ena=CTRL3, active high
     */

    if (ds->adapter_flags & PA_INVERT_TXC) {
        /*
         * Since invert-txc also uses CTRL2 as clock, 
         * active high, retain its state.
         */
        cmd = PA_CTRL0;
    }

    if (set_flag) {
        /* Enable loopback. */
        cmd |= PA_CTRL3 | PA_CTRL1;
	ds->adapter_flags |= PA_LOOP;
    } else {
        /* Disable loopback. */
        cmd |= PA_CTRL3;
	ds->adapter_flags &= ~PA_LOOP;
    }
        
    *modem_reg &= ~PA_CONTROL_BITS;
    *modem_reg |= cmd;                        /* prepare cmd */
    *modem_reg |= cmd | PA_CTRL2;              /* write cmd */
    *modem_reg |= cmd;
    *modem_reg &= ~PA_CONTROL_BITS;
}


/*
 * hd64570_enter_transparent_mode()
 */
void hd64570_enter_transparent_mode (hwidbtype *idb)
{
    volatile ushort *modem_reg;
    hd64570_instance_t *ds;
    
    ds = INSTANCE;
    
    modem_reg = ds->mdm_reg;
    
    /*
     * Turn on modem control interrupts
     */
    hd64570_enable_modem_interrupt(idb);
    
    if (idb->app_type == APPTYPE_5IN1) {
	/*
	 * Assert DTR if we are a DTE, and 
	 * assert DCD/DSR if we are a DCE.
	 */
	*modem_reg |= QSIM_MDM_BDTR;
        if (serial_debug) {
            if (idb->dte_interface)
                buginf("\nHD(%d): Asserting %s ", idb->unit, "DTR");
            else
                buginf("\nHD(%d): Asserting %s ", idb->unit, "DSR and DCD");
        }
	/*
	 * If DCE assert/deassert CTS
	 */
	if (!idb->dte_interface) {
	    if (idb->hdx_flags & HDX_FLAG_ON) {
		*modem_reg &= ~QSIM_DCE_CTS; 
                if (serial_debug)
                    buginf("\nHD(%d): Deassert %s ", idb->unit, "CTS");
	    } else {
		*modem_reg |= QSIM_DCE_CTS; 
                if (serial_debug)
                    buginf("\nHD(%d): Assert %s ", idb->unit, "CTS");
            }
	}

	/*
	 * Say we're up or down based on DCD or DTR:
	 */
	if (*modem_reg & ds->up_signal)
	    net_cstate(idb, IDBS_UP); 
	else
	    net_cstate(idb, IDBS_DOWN);

	/*
	 * Check for loopback
	 */
	hd64570_loopback(idb, idb->loopback);

	/*
	 * If DTE assert DTR and assert/deassert RTS:
	 */
	if (idb->dte_interface) {
	    *modem_reg |= QSIM_DTE_DTR;
	    if (idb->hdx_flags & HDX_FLAG_ON) {
		*modem_reg &= ~QSIM_DTE_RTS;
                if (serial_debug)
                    buginf("\nHD(%d): Asserting %s; Deasserting %s ", idb->unit, "DTR", "RTS");
	    } else {
		*modem_reg |= QSIM_DTE_RTS;
                if (serial_debug)
                    buginf("\nHD(%d): Asserting %s ", idb->unit, "DTR and RTS");
            }
	}
    } else if (is_g703(idb)) {
        /*
         * For G.703 lines, just say we're up or down based on DCD.
         */
        if (*modem_reg & ds->up_signal)
            net_cstate(idb, IDBS_UP);
        else
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

    /*
     * Set the fifo threshold for the receive channel (all port the same).
     */
    if (idb->poll_protocol_flags & PP_ACTIVE) {
        msci_reg->rrc = HD64570_RX_BSC_MARKER;
    } else {
        msci_reg->rrc = HD64570_RX_MARKER;
    }

    /*
     * Set the fifo threshold for the transmit channel.
     * Tango around the Hitachi controller bug.
     */
    switch (idb->subunit) {
    
      case 0:
        msci_reg->trc1_trc0 = 
            HD64570_P0_TX_HIGH_MARKER | HD64570_P0_TX_LOW_MARKER; 
        break;
        
      case 1:
        msci_reg->trc1_trc0 = 
            HD64570_P1_TX_HIGH_MARKER | HD64570_P1_TX_LOW_MARKER; 
        break;
        
      case 2:
        msci_reg->trc1_trc0 = 
            HD64570_P2_TX_HIGH_MARKER | HD64570_P2_TX_LOW_MARKER; 
        break;
	
      case 3:
        msci_reg->trc1_trc0 = 
            HD64570_P3_TX_HIGH_MARKER | HD64570_P3_TX_LOW_MARKER; 
        break;
	
      default:
        break;
    }
}


/*
 * hd64570_dmer_pcr_write()
 * Does a 8 bit access when writing either register.
 * Note: dmer and pcr registers can only be accessed in 8 bit
 *       due to a brian deadness of the Hitachi controller.
 */
void hd64570_dmer_pcr_write (hd64570_instance_t *ds,
			     boolean even_flag, 
			     u_char value)
{
    hd64570_reg_t *hd64570_reg;

    hd64570_reg = (hd64570_reg_t *)ds->hd64570_reg;

    /* Go into byte access mode */
    ds->regaddr->ch_A_int |= QSIM_INT_SCA_BYTE_MODE;

    if (even_flag) {
        /* Write the even byte. */
        ds->regaddr->ch_A_int &= ~QSIM_INT_SCA_A0;
        hd64570_reg->dmer_pcr = value << 8;
    } else {
        /* Write the odd byte. */
        ds->regaddr->ch_A_int |= QSIM_INT_SCA_A0;
        hd64570_reg->dmer_pcr = value;
    }

    /* Go back to word access mode. */
    ds->regaddr->ch_A_int &= ~(QSIM_INT_SCA_BYTE_MODE | QSIM_INT_SCA_A0);
}



/*
 * hd64570_dte_tx_ok()
 * Check to see if we're allowed to transmit in HDX mode
 */
boolean hd64570_dte_tx_ok (hd64570_instance_t *ds)
{
    if ((*ds->mdm_reg & QSIM_DTE_CTS) && 
	(*ds->mdm_reg & QSIM_DTE_RTS))
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

    if (!(*ds->mdm_reg & QSIM_DTE_RTS))
	*ds->mdm_reg |= QSIM_DTE_RTS;

    if ((*ds->mdm_reg & QSIM_DTE_CTS) == 0) {
	ds->hdx_state = HDX64570_DTE_TX_START;

	/*
	 * start dte_timeout timer
	 */
	if (HD64570_EVEN(idb->subunit)) {
	    ds->hd64570_reg->timer_1_tcnt = 0;
	    ds->hd64570_reg->timer_1_tconr = ssb->hdx_rts_timeout * HD64570_COUNTS_PER_MSEC;
	    ds->hd64570_reg->timer_1_tepr_tcsr = HD64570_TIMER_PRE |
		HD64570_TCSR_ECMI | HD64570_TCSR_TME;
	} else {
	    ds->hd64570_reg->timer_3_tcnt = 0;
	    ds->hd64570_reg->timer_3_tconr = ssb->hdx_rts_timeout * HD64570_COUNTS_PER_MSEC;
	    ds->hd64570_reg->timer_3_tepr_tcsr = HD64570_TIMER_PRE |
		HD64570_TCSR_ECMI | HD64570_TCSR_TME;
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
 * hd64570_dte_tx_end()
 * Drop the signals, we be done
 */
static void hd64570_dte_tx_end (hd64570_instance_t *ds)
{
    *ds->mdm_reg &= ~QSIM_DTE_RTS;
}


/*
 * hd64570_dce_tx_ok()
 * Check to see if it's OK to send while DCE HDX
 */
boolean hd64570_dce_tx_ok (hd64570_instance_t *ds)
{
    if (*ds->mdm_reg & QSIM_DCE_CTS)
	return(FALSE);

    return(TRUE);
}


/*
 * hd64570_dce_up_cts()
 * Raise CTS
 */
static void hd64570_dce_up_cts (hd64570_instance_t *ds)
{
    if (ds->hdx_state != HDX64570_DCE_TX)
	*ds->mdm_reg |= QSIM_DCE_CTS; 
}


/*
 * hd64570_dte_timeout()
 * Halt dte bringup
 */
static void hd64570_dte_timeout (hd64570_instance_t *ds)
{
    *ds->mdm_reg &= ~QSIM_DTE_RTS; 
}


/*
 * hd64570_timer_interrupt()
 * Check for timer interrupts. 
 */
void hd64570_timer_interrupt (hwidbtype *port1_idb, hwidbtype *port0_idb)
{
    hwidbtype *idb;
    hd64570_instance_t *ds;
    ushort *hd64570_isr2;
    volatile ushort tmp;

    if (PORT_PRESENT(port0_idb)) {
        idb = port0_idb;
        ds = INSTANCE;
        hd64570_isr2 = (ushort *)&(ds->hd64570_reg->isr2);

	if (*hd64570_isr2 & HD64570_ISR2_T1IRQ) {

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

            /* Disable/clear the interrupt. */
	    ds->hd64570_reg->timer_0_tepr_tcsr = 0;
	    tmp = ds->hd64570_reg->timer_0_tepr_tcsr;
	    tmp = ds->hd64570_reg->timer_0_tcnt;

	    if (ds->hdx_state != HDX64570_DCE_TX)
		hd64570_dce_up_cts(ds);
	}
    }

    if (PORT_PRESENT(port1_idb)) {
        idb = port1_idb;
        ds = INSTANCE;
        hd64570_isr2 = (ushort *)&(ds->hd64570_reg->isr2);

        if (*hd64570_isr2 & HD64570_ISR2_T3IRQ) {

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

	/* 
	 * Check for the CTS delay timers 
	 */
	if (*hd64570_isr2 & HD64570_ISR2_T2IRQ) {

            /* Disable/clear the interrupt. */
            ds->hd64570_reg->timer_2_tepr_tcsr = 0;
            tmp = ds->hd64570_reg->timer_2_tepr_tcsr;
            tmp = ds->hd64570_reg->timer_2_tcnt;

	    if (ds->hdx_state != HDX64570_DCE_TX)
		hd64570_dce_up_cts(ds);
	}
    }
}


/*
 * Show HD64570 structure pointers
 */
void hd64570_show_structure_info (hwidbtype *idb)
{
    hd64570_instance_t *ds = INSTANCE;

    printf("NIM slot %d, NIM type code %d, NIM version %d\n",
	   idb->slot, idb->nim_type, idb->nim_version);
    printf("idb = 0x%x, driver structure at 0x%x, regaddr = 0x%x\n",
	   idb, ds, ds->regaddr);
}


/*
 * Show the cable state
 */
void hd64570_show_cable_state (hwidbtype *idb)
{
    hd64570_instance_t *ds;
    serialsb *ssb;

    ds = INSTANCE;
    ssb = idb_get_hwsb_inline(idb, HWIDB_SB_SERIAL);

    if (ds->port_id == PA_NONE) {
	printf("No Port Adapter present.\n");
	return;
    }

    if (ds->port_id == PA_G703_DTE)
        printf("G.703 Unbalanced");
    else if (ds->port_id == PA_G703_DCE)
        printf("G.703 Balanced");
    else {
	/*
	 * 5/1 applique
	 */
        printf("Universal Serial: ");
	if (idb->dte_interface)
	    printf("DTE ");
	else {
	    if (ds->port_id != PA_NO_CABLE)
		printf("DCE ");
	}

	/*
	 * Print up the interface type
	 */
	switch (ds->port_id & ~1) {        /* convert to DTE for this test */

	case PA_NONE:
	    printf("No");
	    break;
	    
	case PA_RS232_DTE:
	    printf("V.24 (RS-232)");
	    break;

	case PA_V35_DTE:
	    printf("V.35");
	    break;

	case PA_RS449_DTE:
	    printf("V.11 (RS-449)");
	    break;

	case PA_X21_DTE:
	    printf("V.11 (X.21)");
	    break;

	case PA_RS232_NRZI_DTE:
	    printf("V.24 (RS-232 NRZI)");
	    break;

	case PA_G703_DTE:
	    printf("G.703");
	    break;

	case PA_RS530_DTE:
	    printf("RS-530");
	    break;

	default:
	    printf("Unknown cable id (%d)", ds->port_id);
	}
	printf(" cable");

	/*
	 * If a DCE, print the clockrate too
	 */
	if (!idb->dte_interface) {
	    if (ssb->serial_clockindex)
		printf(", clockrate %d", 
                       hd64570_clock[ssb->serial_clockindex].rate);
	    else {
		if (ds->port_id != PA_NO_CABLE)
		    printf(", no clock");
	    }
	}   
    }
    printf("\n");
}


/*
 * hd64570_dce_no_txce()
 * Set the 5/1 adapter in a DCE mode to use the 
 * local clock for sampling the received data.
 * set_flag = TRUE  --> no txce, use local clock.
 * set_flag = FLASE --> use txce.
 */
static void hd64570_dce_no_txce (hwidbtype *idb, boolean set_flag)
{
    volatile ushort *modem_reg;
    hd64570_instance_t *ds;
    ushort cmd;
    
    ds = INSTANCE;
    
    modem_reg = ds->mdm_reg;
    
    /* 
     * clk = CTRL4, data = CTRL0, enable = CTRL3, active high.
     */
    if (set_flag) {
        /* Enable no_txce. */
        cmd = PA_CTRL3 | PA_CTRL0;
    } else {
        /* Disable no_txce. */
        cmd = PA_CTRL3;
    }

    *modem_reg &= ~PA_CONTROL_BITS;
    *modem_reg |= cmd;                       /* prepare cmd */
    *modem_reg |= cmd | PA_CTRL4;             /* write cmd */
    *modem_reg |= cmd;                    
    *modem_reg &= ~PA_CONTROL_BITS;
}


/*
 * hd64570_invert_txc()
 * Set the 5/1 adapter in a DTE mode to 
 * invert the TXC clock, to clock out TXD.
 * set_flag = TRUE  --> invert
 * set_flag = FLASE --> do not invert.
 */
static void hd64570_invert_txc (hwidbtype *idb, boolean set_flag)
{
    volatile ushort *modem_reg;
    hd64570_instance_t *ds;
    ushort cmd = 0;
    
    ds = INSTANCE;
    
    modem_reg = ds->mdm_reg;
    
    /* 
     * clk = CTRL2, data = CTRL0, enable = CTRL3, active high.
     */

    if (ds->adapter_flags & PA_LOOP) {
        /*
         * Since loop also uses CTRL2 as clock, retain its state.
         */
        cmd = PA_CTRL1;
    }

    if (set_flag) {
        /* Enable clock invert. */
        cmd |= PA_CTRL3 | PA_CTRL0;
        ds->adapter_flags |= PA_INVERT_TXC;
    } else {
        /* Disable clock invert. */
        cmd |= PA_CTRL3;
        ds->adapter_flags &= ~PA_INVERT_TXC;
    }
        
    *modem_reg &= ~PA_CONTROL_BITS;
    *modem_reg |= cmd;                        /* prepare cmd */
    *modem_reg |= cmd | PA_CTRL2;              /* write cmd */
    *modem_reg |= cmd;
    *modem_reg &= ~PA_CONTROL_BITS;
}


/*
 * hd64570_rx_sync_disable()
 * Set/unset the rx-sync-disable bit in the 5/1 port adapter.
 * set_flag = TRUE --> disable receive synchronizer.
 * set_flag = FALSE  --> enable receive synchronizer (default).
 * Note, 1. This command is not published and is for debugging only.  
 *       2. This routine clear the DCD invert bit in the port adapter.
 */
static void hd64570_rx_sync_disable (hwidbtype *idb, boolean set_flag)
{
    volatile ushort *modem_reg;
    hd64570_instance_t *ds;
    ushort cmd;
    
    ds = INSTANCE;
    
    modem_reg = ds->mdm_reg;
    
    /* 
     * clk = CTRL4, data = CTRL0, enable = CTRL3, active low.
     */
    if (set_flag) {
        cmd = PA_CTRL0;
    } else {
        cmd = 0;
    }

    *modem_reg &= ~PA_CONTROL_BITS;
    *modem_reg |= cmd;                       /* prepare cmd */
    *modem_reg |= cmd | PA_CTRL4;             /* write cmd */
    *modem_reg |= cmd;                    
    *modem_reg &= ~PA_CONTROL_BITS;
}


/*
 * hd64570_g703_print_version -  print out the g.703 version info.
 */
static void hd64570_g703_print_version (void)
{
    /*
     *  DO NOT CHANGE THE FOLLOWING G.703 VERSION IDENTIFICATION
     *  Put in place for BABT certification, others to follow perhaps.
     *  our certifications are keyed to this identification and any change
     *  will mean large commitments of time and money for recertification
     */
    printf("G.703/E1 software, Version 1.0.\n");
}

/*
 * hd64570_read_g703()
 * Read the status of the G.703 interface
 */
static uchar hd64570_read_g703 (hwidbtype *idb) 
{
    volatile ushort *modem_reg;
    hd64570_instance_t *ds;
    int bit_num;
    uchar status = 0;

    ds = INSTANCE;

    modem_reg = ds->mdm_reg;

    for (bit_num=0; bit_num < 8; bit_num++) {

        /* Read this bit of the status register. */
        *modem_reg = ((bit_num << PA_CONTROL_SHIFT) | G703_ENHANCED);
        if (bit_num != 7) {
	    if ((*modem_reg & QSIM_MDM_SENSE2) == 0) {
		if (*modem_reg & QSIM_MDM_SENSE1) {
		    status |= 1 << bit_num;
                }
            }
        } 

        if (bit_num == 3) {
            /*    
	     * Insert the presently configured state of the loopback bit.
             */
            if (idb->loopback) {
		*modem_reg = QSIM_MDM_CTRL1 | G703_ENHANCED;
		*modem_reg = QSIM_MDM_CTRL1 | G703_CLOCK | G703_ENHANCED;
            } else {
		*modem_reg = G703_ENHANCED;
		*modem_reg = G703_CLOCK | G703_ENHANCED;
            }
        }
    }
    
    /* Leave the control bits at zero. */
    *modem_reg &= ~PA_CONTROL_BITS;

    return (status);
}


/*
 * Configure G.703/G.704 applique.
 */
static void hd64570_setup_g703 (hwidbtype *idb, ushort setup_bits)
{
    volatile ushort *modem_reg;
    hd64570_instance_t *ds;
    int bit_num;

    ds = INSTANCE;

    modem_reg = ds->mdm_reg;

    for (bit_num=0; bit_num < 16; bit_num++) {
        if ((setup_bits >> bit_num) & 1) {
            *modem_reg = G703_DATA | G703_ENHANCED;
            *modem_reg = G703_DATA | G703_CLOCK | G703_ENHANCED;
        } else {
            *modem_reg = G703_ENHANCED;
            *modem_reg = G703_CLOCK | G703_ENHANCED;
        }
    }

    /* Write strobe, load shifted-in word. */
    *modem_reg = QSIM_MDM_CTRL4 | G703_ENHANCED;
    *modem_reg &= ~QSIM_MDM_CTRL4;

    /* Record the new applique settings. */
    ds->g703_setup = setup_bits;

#ifdef DEBUG
    printf("setup_g703: setup_bits = 0x%x, status = 0x%x\n",
            setup_bits, hd64570_read_g703(idb));
#endif

    /*
     * Write loopback state back to G.703 applique
     * otherwise it is lost due to a hardware bug.
     */
    if (idb->loopback) {
	*modem_reg = QSIM_MDM_CTRL1 | G703_ENHANCED;
	*modem_reg = QSIM_MDM_CTRL1 | G703_CLOCK | G703_ENHANCED;
    } else {
	*modem_reg = G703_ENHANCED;
	*modem_reg = G703_CLOCK | G703_ENHANCED;
    }
}


/*
 * hd64570_g703_periodic()
 * Collect stats for a G.703 applique.
 */
static void hd64570_g703_periodic (hwidbtype *idb)
{
    hd64570_instance_t *ds;
    uchar status;
    serialsb *ssb;

    ds = INSTANCE;
    ssb = idb_use_hwsb_inline(idb, HWIDB_SB_SERIAL);

    status = hd64570_read_g703(idb);

    if (ssb->start_slot) {

        /*
         * Collect stats if framing is enabled. 
         */
	if (status & G703_AIS)
	    ssb->rx_ais++;

	if (status & G703_RAI)
	    ssb->rx_rai++;

	if (status & G703_LOS)
	    ssb->rx_los++;

	if (status & G703_LOF)
	    ssb->rx_lof++;
    }

    ds->g703_signals = status & G703_SIGNALS;
    idb_release_hwsb_inline(idb, HWIDB_SB_SERIAL);
}


/*
 * Print out DTR, CTS, etc. on serial nim, 
 * print out the G.703 specific info if the port adaptor is G.703 type
 */
void hd64570_show_serial_state (hwidbtype *idb, char * leader)
{
    hd64570_instance_t *ds;
    ushort modem_control;
    uchar signals;
    const char *fmt_str1 = "     DCD %s, BER %s, NELR %s, FELR %s";
    const char *fmt_str2 = "     DCD=%s  DSR=%s  DTR=%s  RTS=%s  CTS=%s";

    ds = INSTANCE;

    /*
     * Be courteous, print all hand shaking signals.
     */
    modem_control = *ds->mdm_reg;
    signals = ds->g703_signals;

    printf("\n");
    if (idb->dte_interface) {
        if (is_g703(idb)) {
                  printf(fmt_str1,
                        (modem_control & QSIM_DTE_DCD) ? "up" : "down",
                        (signals & G703_BER) ? "active" : "inactive",
                        (signals & G703_NELR) ? "active" : "inactive",
                        (signals & G703_FELR) ? "active" : "inactive");
          } else {
                  printf(fmt_str2, 
                      ((modem_control & QSIM_DTE_DCD) ? "up" : "down"),
                      ((modem_control & QSIM_DTE_DSR) ? "up" : "down"),
                      ((modem_control & QSIM_DTE_DTR) ? "up" : "down"),
                      ((modem_control & QSIM_DTE_RTS) ? "up" : "down"),
                      ((modem_control & QSIM_DTE_CTS) ? "up" : "down"));
         } 
    } else {
        printf(fmt_str2,
            ((modem_control & QSIM_DCE_DCD) ? "up" : "down"),
            ((modem_control & QSIM_DCE_DCD) ? "up" : "down"),
            ((modem_control & QSIM_DCE_DTR) ? "up" : "down"),
            ((modem_control & QSIM_DCE_RTS) ? "up" : "down"),
            ((modem_control & QSIM_DCE_CTS) ? "up" : "down"));
    }

}


/*
 * hd64570_init_config()
 *
 * Set the interface state based on the config file 
 * in NVRAM or loaded via the network
 */
void hd64570_init_config (hwidbtype *idb)
{
    hd64570_instance_t *ds;
    int index;
    serialsb *ssb;

    ds = INSTANCE;
    ssb = idb_get_hwsb_inline(idb, HWIDB_SB_SERIAL);

    if (idb->nim_version < 16) {
          hd64570_clock = hd64570_clock1;
      } else {
          hd64570_clock = hd64570_clock2;
     }

    /*
     * Read the Cable ID to determine the current DTE/DCE mode.
     */
    hd64570_read_port_id(idb);

    /*
     * If DCE set up clock rate
     * Note, G703 interfaces are all DTE, so we won't set up any clocks.
     */
    if (!idb->dte_interface) {
        index = ssb->serial_clockindex;

        /*
         * Set up receiver.
         * Determine the source of the clock for sampling the
         * received data, and let the 5/1 adapter know about it.
         */
        ds->msci_reg->txs_rxs = HD64570_RXS_RXC;

        if (ssb->dce_terminal_timing_enable || index == 0)
            hd64570_dce_no_txce(idb, FALSE);    
        else
            hd64570_dce_no_txce(idb, TRUE);    
	
        /*
         * Set up transmitter and baud rate, if no index 
	 * then use TXC as not to generate any clock.
         */
        if (index == 0) {
            ds->msci_reg->txs_rxs |= HD64570_TXS_TXC;
        } else {
            ds->msci_reg->txs_rxs |= HD64570_TXS_BRG | 
                                     hd64570_clock[index].divider << 8;
            ds->msci_reg->tmc_idl |= hd64570_clock[index].counter << 8;
        }
    } else {
        ds->msci_reg->txs_rxs = HD64570_RXS_RXC | HD64570_TXS_TXC;
    }

    if (idb->app_type == APPTYPE_5IN1) {
	/*
	 * Invert the TXC clock if user insists on it.
	 * Should be taken care of by 5/1 automatically.
	 */
	hd64570_invert_txc(idb, ssb->invert_txc);

	/*
	 * Enable/disable the rx_sync on the 5/1 port adapter.
	 */
	hd64570_rx_sync_disable(idb, ssb->rx_sync_disable);

        /*
         * If ignore-dcd command is issued, use DSR 
         * instead of DCD as the link up/down indicator.
         */
	if (ssb->ignore_dcd) {
		/*
		 * As a DTE, use DSR instead of DCD. 
                 * As a DCE, since DSR and DTR are tied together,
                 * checking BDSR will check DTR (John Chapman's magic).
		 */
		ds->up_signal = QSIM_MDM_BDSR;
	} else {
		/*
		 * Use DCD or DTR.
		 */
		ds->up_signal = QSIM_MDM_BDCD;
	}
    } else if (is_g703(idb)) {
        ushort setup = ds->g703_setup;

        /*
         * For G.703 always use the DCD as the link up/down indicator.
         */
        ds->up_signal = QSIM_MDM_BDCD;

        /*
         * Determine the setup for G.703 applique based on the configuration.
         */
        if (ssb->crc4_enabled)
            setup |= G703_CRC4;
        else
            setup &= ~G703_CRC4;

        setup &= ~G703_STOP_SLOT;
        if (ssb->stop_slot) {
            /*
             * Change in G.703 spec, add one to stop slot only,
             * take modulo 32.
             */
            setup |= (((ssb->stop_slot + 1) << 1) & G703_STOP_SLOT);
        }

        setup &= ~G703_START_SLOT;
        if (ssb->start_slot) {
            setup |= ((ssb->start_slot << 6) & G703_START_SLOT);
        }

        if (ssb->timeslot16_enabled)
            setup |= G703_TS16;
        else
            setup &= ~G703_TS16;

        if (ssb->clocksource_enabled)
            setup |= G703_CLOCK_SRC;
        else
            setup &= ~G703_CLOCK_SRC;

        /* 
         * For local loopback, do both local and remote loop. 
         * Remote loop being the looping of the incoming packets 
         * back to the remote side.
         */
        if (idb->loopback)
            setup |= G703_REMOTE_LOOP;
        else
            setup &= ~G703_REMOTE_LOOP;

        if (idb->far_end_looped) {
            /*
             * The Metrodata G.703 applique has a nasty bug where
             * it can clock itself into an unusable state if the
             * 'TX loop up' command is issued for the 2nd time.
             * Check the software field 'g703_loop_line' instead
             * of the FELR status bit. Since there is a window of
             * about 6 seconds from the time 'line loop' command
             * is issued until the FELR status bit is set.
             */
            if (!ds->g703_loop_line) {
                /*
                 * Okay to enable the line loop.
                 */
                setup |= G703_LOOP_UP;
                ds->g703_loop_line = TRUE;
            }
        } else {
	    setup |= G703_LOOP_DOWN;
	    ds->g703_loop_line = FALSE;
        }

        /*
         * Finally, set up the G.703 applique.
         * Toggle the bits that need to be toggled.
         */
        hd64570_setup_g703(idb, setup);

        if (setup & G703_LOOP_UP)
            setup &= ~G703_LOOP_UP;
        else if (setup & G703_LOOP_DOWN)
            setup &= ~G703_LOOP_DOWN;

        hd64570_setup_g703(idb, setup);
    }
}


/*
 * init_hd64570_idb()
 * 
 * Probes a port on the 4T nim and if present setups an idb for it.
 */
static boolean init_hd64570_idb (uint slot, uint unit, uint subunit)
{
    hwidbtype *idb;
    hd64570_instance_t *ds;
    u_short *mdm_reg;
    qsim_regs_t *regaddr;
    u_long regaddr_offset;
    

    regaddr_offset = (u_long)(GET_SLOT_BASE(slot));

    if (subunit >= 2)
        regaddr_offset += QSIM_PORT23_OFFSET;

    regaddr = (qsim_regs_t *)regaddr_offset;

    if (HD64570_EVEN(subunit))
        mdm_reg = (u_short *)&regaddr->ch_A_mdm;
    else
        mdm_reg = (u_short *)&regaddr->ch_B_mdm;
    
    /*
     * Check to see if this port actually exists by making 
     * sure a 5/1 Port Adapter is installed for it.
     */
    *mdm_reg &= ~PA_CONTROL_BITS;
    if (*mdm_reg & QSIM_MDM_SENSE0) {
        /*
         * No port here.
         */
        return (FALSE);
    }

    /*
     * Set up storage, add IDB to interface queue:
     */
    idb = idb_create();
    if (!idb) {
	crashdump(0);
    }
    
    idb->slot = slot;		/* save our NIM slot number */
    idb->unit = unit;		/* unit number */
    idb->subunit = subunit;	/* subunit number */

    hd64570_init_idb_common(idb);

    idb->serial_flags |= SERIAL_FLAGS_CAN_DIS_RX_SYNC;

    /*
     * Set up address pointers
     */
    ds = INSTANCE;
    ds->regaddr = regaddr;
    ds->hd64570_reg = (hd64570_reg_t *)(regaddr_offset + QSIM_SCA_REG_OFFSET);

    if (HD64570_EVEN(subunit)) {
        ds->msci_reg = &ds->hd64570_reg->msci_ch0;
        ds->dmac_rx_reg = &ds->hd64570_reg->msci_ch0_rx_dma;
        ds->dmac_tx_reg = &ds->hd64570_reg->msci_ch0_tx_dma;
        ds->mdm_reg = (u_short *)&ds->regaddr->ch_A_mdm;
        ds->int_reg = (u_short *)&ds->regaddr->ch_A_int;
    } else {
        ds->msci_reg = &ds->hd64570_reg->msci_ch1;
        ds->dmac_rx_reg = &ds->hd64570_reg->msci_ch1_rx_dma;
        ds->dmac_tx_reg = &ds->hd64570_reg->msci_ch1_tx_dma;
        ds->mdm_reg = (u_short *)&ds->regaddr->ch_B_mdm;
        ds->int_reg = (u_short *)&ds->regaddr->ch_B_int;
    }

    /*
     * Read the Port ID.
     */
    hd64570_read_port_id(idb);

#ifdef DEBUG
    buginf("HD64570(%d): init_hd64570_idb: cable ID = %d\n", 
            idb->unit, ds->port_id);
#endif

    /*
     * Initialize G.703 applique specific routines.
     */
    if (is_g703(idb)) {
	ds->appl_periodic = hd64570_g703_periodic;
    } 

     /*
      * Save our NIM type code, and NIM version:
      */
    idb->nim_type = nim_get_type(slot);
    idb->nim_version = nim_get_version(slot);

    if (idb->nim_version < 16) {
          hd64570_clock = hd64570_clock1;
      } else {
          hd64570_clock = hd64570_clock2;
     }

    /*
     * Initialize the interface:
     */
    hd64570_init(idb);

    return (TRUE);
}


/*
 * hd64570_analyze_interface()
 *
 * Initialize the HD64570 NIM for the given slot
 */
static void hd64570_analyze_interface (int slot)
{
    int     subunit = 0;
    ushort  type;

    hd64570_check_version(slot);

    type = nim_get_type(slot);

    if (type == XX_SERIAL_QUAD) {
	for (subunit = 0; subunit < QSIM_MAX_PORTS; subunit++) {
	    if (init_hd64570_idb(slot, nserialnets, subunit)) {
		nserialnets++;
	    }
	}
    }
}


/*
 * HD64570 subsystem initialisation entry point
 */
void hd64570_subsys_init (subsystype *subsys)
{
    /*
     * Register the NIM IDs for the various HD6450 NIMs
     */
    reg_add_analyze_interface(XX_SERIAL_QUAD,
			      hd64570_analyze_interface,
			      "hd64570_analyze_interface");

    reg_add_print_copyright(hd64570_g703_print_version,
			    "hd64570_g703_print_version");

    reg_add_media_half_or_full_duplex_cmd(IDBTYPE_HD64570, 
                                          serial_half_duplex_command,
                                          "serial_half_duplex_command");
}
