/* $Id: if_serial_quicc.c,v 3.8.20.8 1996/08/28 13:10:23 thille Exp $ 
 * $Source: /release/112/cvs/Xsys/src-36/if_serial_quicc.c,v $
 *------------------------------------------------------------------
 * if_serial_quicc.c  68360 QUICC chip serial driver for low end software
 *
 * March 1995, Dave Coli and John Ng
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: if_serial_quicc.c,v $
 * Revision 3.8.20.8  1996/08/28  13:10:23  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.8.20.7  1996/08/13  02:22:22  gstovall
 * CSCdi39484:  Giant packets output on ethernet interface
 * Branch: California_branch
 * Make sure we drop giants, even if they are just a wee bit too big.
 *
 * Revision 3.8.20.6  1996/07/20  03:20:13  fred
 * CSCdi63607:  add access list to traffic shaping per customer request
 *              requires change of parameters on function called by
 *              fastsend routines in drivers
 * Branch: California_branch
 *
 * Revision 3.8.20.5  1996/06/12  19:56:35  jng
 * CSCdi60260:  Async catwoman commit
 * Branch: California_branch
 * Async Catwoman commit into California
 *
 * Revision 3.8.20.4  1996/05/21  10:02:49  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.8.20.3  1996/04/27  06:47:40  syiu
 * Branch: California_branch
 * Commit Dial1_branch
 *
 * Revision 3.6.8.3  1996/04/26  00:52:33  syiu
 * Branch: Dial1_branch
 * Dial1_branch to IosSec_postmerge_960419
 *
 * Revision 3.6.8.2  1996/04/17  04:11:09  syiu
 * Branch: Dial1_branch
 * Sync Dial1_branch to V111_1_3
 *
 * Revision 3.6.8.1  1996/02/23  20:58:27  lbustini
 * Branch: Dial1_branch
 * Move dialer fields out of the idb.
 *
 * Revision 3.8.20.2  1996/04/17  13:47:25  fred
 * CSCdi54910:  commit rsvp, traffic shaping, and RED
 * Branch: California_branch
 *
 * Revision 3.8.20.1  1996/03/18  21:56:31  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.6.12.3  1996/03/13  02:00:05  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.6.12.2  1996/02/20  17:48:46  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.8  1996/01/29  07:29:53  hampton
 * Remove unnecessary includes of parser definition files.  [CSCdi47717]
 *
 * Revision 3.7  1996/01/25  23:12:17  gstovall
 * CSCdi47390:  down-when-looped command not working (regression)
 * Remember to reset the itemlist before tossing it up to the next level.
 *
 * Revision 3.6.12.1  1996/01/24  22:49:06  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.6  1995/12/17  18:37:24  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.5  1995/12/06  18:32:20  gstovall
 * CSCdi44769:  Interface idb queues are not getting populated correctly
 * Make sure the hwidbs are in the same order on the media hwidbQs as on
 * the hwidbQ.
 *
 * Revision 3.4  1995/11/22  20:09:10  lbustini
 * CSCdi44360:  DTR dialing broken on cisco 1005
 * Typo causes line transition to up on reset.
 * Also complete CSCdi39576 for catwoman.
 *
 * Revision 3.3  1995/11/17  18:39:10  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:29:14  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:19:05  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:26:18  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/06/07  22:51:50  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#include "master.h"
#include "interface_private.h"
#include "packet.h"
#include "config.h"
#include "logger.h"
#include "../src-36/msg_quicc.c"	/* Not a typo, see logger.h */
#include "../src-36/msg_serial_quicc.c"	/* Not a typo, see logger.h */
#include "../if/msg_datalink.c"		/* Not a typo, see logger.h */

#include "sys_registry.h"
#include "fastswitch_registry.h"
#include "media_registry.h"
#include "subsys.h"

#include "../os/pool.h"

#include "../les/if_les.h"

#include "../src-36/if_serial_quicc.h"
#include "../src-36/quicc_serial_common.h"
#include "../src-36-c1000/cpai.h"

#include "../ip/ip.h"
#include "../wan/ppp.h"
#include "../wan/frame_relay.h"
#include "../wan/smds.h"
#include "../wan/atm_dxi.h"
#include "../wan/serial_private.h"
#include "../wan/dialer_registry.h"

#include "../src-36-c1000/init_c1005.h"
#include "../if/serial_inline.h" /* needed for serial_RX_interrupt_inline */
                                 /* common RX int handler                 */

#include "parser.h"
#include "../parser/parser_actions.h"
#include "../parser/macros.h"

#include "../les/if_les_serial.h"
#include "../util/itemlist.h"
#include "../if/traffic_shape.h"

static void quicc_serial_scc_init(hwidbtype *, boolean);


/*
 * Parse chains for interface-specific commands not included by
 * wan/serial_chain.c
 */
#define ALTERNATE       NONE
#include "../if/cfg_int_transmitter-delay.h"
LINK_POINT(quicc_serial_interface_commands, ALTERNATE);
#undef  ALTERNATE


/*
 * QUICC serial subsystem header
 */
#define QUICC_SERIAL_MAJVERSION   1
#define QUICC_SERIAL_MINVERSION   0
#define QUICC_SERIAL_EDITVERSION  1

/*
 * =================================================================
 * Name: quicc_serial_modem_interrupt_handler
 *
 * Description:
 *	This routine is use to handle modem signal changes, it also
 *	determines if flapping has occurred.
 *
 * Parameters:
 *	hwidbtype 	- pointer to hwidb for this interface
 *
 * Returns:
 *	void - None.
 * =================================================================
 */
static void quicc_serial_modem_interrupt_handler (hwidbtype *idb)
{
    ushort newstate = IDBS_DOWN;	/* by default */
    serial_instance_t *ds;
    quicc_dpr_t *dpr;
    
    
    QASSERT(idb);
    ds = (serial_instance_t *)INSTANCE;
    
    QASSERT(ds);
    dpr = (quicc_dpr_t *)ds->dprbase;
    
    
    /* If someone admin us down then we should have stop the SCC and
     * disable all modem interrupt for this interface so we should
     * not ever get here.  Should this be ASSERT?
     */
    if (idb->state == IDBS_ADMINDOWN) {
	if (serial_debug)
	    buginf("Error: Should not received interrupt on a admin down interfacen");
	return;
    }
    
    /* By default "newstate == IDBS_DOWN". */
    
    /* Need to change this code if DCD is active low */
    if (cable_present()) {
	if (dcd_on()) {
	    newstate = IDBS_UP;		/* DCD signal exists */
	    if (serial_debug)
		buginf("\nQUICC_SERIAL(%d) : DCD is up.",idb->unit);
	}
	else {
	    if (serial_debug)
    		buginf("\nQUICC_SERIAL(%d) : DCD is down.",idb->unit);
	}

    	    
    }
    else {
	/* Disable all modem signals.  We will be checking cables
	 * again in the periodics.
	 */
	if (serial_debug)
	    buginf("\nQUICC_SERIAL(%d): Cable is unplugged.", idb->unit);
	
	/* newstate == IDBS_DOWN by default so don't need to set */
	quicc_modem_interrupt_mask(idb, DISABLE, QUICC_CIMR_DCD_MASK);
    }
    
    
    /*
     * If we have a state changed and is not spoofing then we should
     * do a change state and increment the number of transitions.
     */
    if ((idb->state != newstate) && !reg_invoke_dialer_spoofing_int(idb)) {
	idb->counters.transitions++;
	net_cstate(idb, newstate);
    }
    
    /*
     * 
     */
    if (reg_invoke_dialer_spoofing_int(idb) && (newstate == IDBS_UP)) {
	idb->counters.transitions++;
	net_cstate(idb, newstate);
    }
    
    
    
    /* Determine if the number of times we receive modem interrupts */
    
    /* Need to clear out flap count if left over from previous
     * flapping condition before incrementing flap_count.  
     */
    if ((!ds->flap_enabled) && (!TIMER_RUNNING_AND_SLEEPING(ds->flap_time))) {
	TIMER_START(ds->flap_time,ONESEC);
	ds->flap_count = 0;
    }
    
    if (++ds->flap_count > ds->max_flap_rate) {
	quicc_modem_interrupt_mask(idb, DISABLE, QUICC_CIMR_DCD_MASK);
	ds->flap_enabled = TRUE;
    }
}

/*
 * =================================================================  
 *	Name:		quicc_serial_stop
 *
 *	Description:	This routine will stop the specific port tied to
 *			this hwidbtype.
 *
 *      CALLER:		ds->hw_stop
 *
 *	PREQ:		None
 *
 *	Returns:		None
 * =================================================================
 */
static void quicc_serial_stop (hwidbtype *idb)
{
    leveltype SR;
    quicc_instance_t *ds;
    
    QASSERT(idb);

    ds = INSTANCE;

    SR = raise_interrupt_level(NETS_DISABLE);
    quicc_modem_interrupt_mask(idb, DISABLE, QUICC_CIMR_DCD_MASK);
    rts_assert(FALSE);
    dtr_assert(FALSE);
    reset_interrupt_level(SR);
    
    /*
     * Bring the serial scc to a screeeeching halt.
     * Both Rx and Tx must stop, interrupts disabled, etc
     */
    quicc_scc_stop(ds->dprbase, ds->qinfo, RX_DISABLED, TX_DISABLED);
    
    net_cstate(idb, IDBS_DOWN);
}

/* 
 * ================================================================= 
 *  Name:	quicc_serial_start
 *
 *  Description:This routine is called to start the RX/TX for this interface.
 *		If something keep us from starting we will stop the interface
 *		and keep the interface state down. 
 *
 *  CALLER:	Calller of this routine is through ds->hw_start,
 *		idb->hw_reset
 *
 *  PREQ:	The state of the interfaces must be off.
 *
 *  Returns	:	The state of the interfaces (RX/TX) will be started.
 * =================================================================
 */
static void quicc_serial_start (hwidbtype *idb)
{
    quicc_instance_t *ds;
    quicc_dpr_t *dpr;
        

    QASSERT(idb);
    
    if (!serial_pulse_DTR(idb)) {
	return;
    }

    /*
     * If a DTR dialer is configured on this interface we may not want
     * to raise DTR, so return here if this is the case.
     */
    if (is_ddr(idb)) {
	if (reg_invoke_dialer_keep_dtr_low(idb)) {
	    return;
	}
    }

    ds = INSTANCE;
    
    /* Check to make sure  the RX and TX has not been started. */
    QASSERT(quicc_scc_status(ds->dprbase, ds->port_no, FALSE,
			     FALSE));
    /* DCD or DSR signals; we do want to turn on interrupt on
     * these signals so that when they come back we can turn ourselves
     * back on.
     */
    quicc_modem_interrupt_mask(idb, ENABLE, QUICC_CIMR_DCD_MASK);
    
    dpr = (quicc_dpr_t *)ds->dprbase;
    
    dtr_assert(TRUE);
    rts_assert(TRUE);
        
    /*
     * Kick the CP with an INIT_RX_TX_PARAMS command.
     * Enable serial SCC interrupts.
     * Set ENT and ENR in GSMR to enable transmit and receive.
     */
    if (quicc_scc_start(ds->dprbase, ds->qinfo, 
                        idb_is_throttled(idb) ? 0 : RX_ENABLED,
                        TX_ENABLED)) {
	net_cstate(idb, (dcd_on() ? IDBS_UP : IDBS_DOWN));
	return;
    }
    
    /* Something happen, we can't start the SCC, we need to put it
     * back in stop mode, should we return a error code since this routine
     * should have put us in the start mode.
     */
    quicc_serial_stop(idb);
    errmsg(&msgsym(INITFAIL, QUICC_SERIAL), idb->unit, 0, ds->scce);
}
/*
 * =================================================================   
 *	Name:		quicc_serial_reset
 *
 *	Description:	This routine will clear out any packets that have
 *			not been process.  Resets the holdq for the packets.
 *			If the interface have not been brought down, bring
 *			it down.  Once the above has been done, bounce the
 *			interface back up.  This functions should probably
 *			be made inline since there is an extra level of
 *			indirection.
 *
 *	CALLER:		idb->reset
 *
 *	PREQ:		None
 *
 *	Returns:		None
 * =================================================================
 */
static void quicc_serial_reset (hwidbtype *idb, boolean maxdgram_changed)
{

    QASSERT(idb);

    holdq_reset(idb);

    if (idb->state != IDBS_ADMINDOWN) {
	quicc_serial_stop(idb);
        quicc_serial_scc_init(idb, maxdgram_changed);
	quicc_serial_start(idb);
	idb->counters.resets++;
    }
}


/*
 * =================================================================
 *	Name:		quicc_serial_reset_wrapper
 *
 *	Description:	This routine is only a wrapper to satisfy the
 *			parameter interface requirement for 'reset'.  The
 *			actual work will be done by quicc_serial_reset().
 *
 *	CALLER:		idb->reset
 *
 *	PREQ:		None
 *
 *	Returns:		None
 * =================================================================
 */
static void quicc_serial_reset_wrapper (hwidbtype *idb)
{

    /* This call will tell quicc_serial_reset that
     * maxdgram changed has not taken place.
     */
    quicc_serial_reset(idb,FALSE);

}



static void quicc_serial_throttle (hwidbtype *idb)
{
    /* Really should use void * instead of serial_instance_t
     * so that it can be general purpose.
     */
    quicc_instance_t *ds;
    
    QASSERT(idb);
    
    ds = INSTANCE;

    quicc_scc_disable_rx(ds->dprbase, ds->qinfo);
    idb_mark_throttled(idb);
}


/*
 * =================================================================
 *	Name:		quicc_serial_shutdown
 *
 *	Description:	This routine will stop the specific port tied to
 *			this hwidbtype.  This calls quicc_serial_stop().
 *
 *      CALLER:		idb->shutdown
 *
 *	PREQ:		None
 *
 *	Returns:	None
 * =================================================================
 */
static void quicc_serial_shutdown (hwidbtype *idb, int newstate)
{
    QASSERT(idb);
     
    
    quicc_serial_stop(idb);

    /* Although quicc_serial_stop would have change the state to
     * down, but we really want ADMIN_DOWN.
     */
    net_cstate(idb, newstate);
}



/*
 * Enable serial interface, most likely shut down for input congestion.
 * Please note; this function must be called with NETS_DISABLED, to
 * work correctly.  This is use to enable rx after it was disable because
 * off throttle.
 */
/* 
 * =================================================================
 *  Name:	quicc_serial_enable_rx
 *
 *  Description:This routine is called to start the RX for this interface.
 *		This routine should actually be un-throttle since this
 *		is expected to be call to take the interface out of throttle
 *		mode.
 *
 *  CALLER:	
 *		
 *
 *  PREQ:	The state of the interfaces must be off.
 *
 *  Returns:	The state of the interfaces (RX) will be started.
 * =================================================================
 */
static void quicc_serial_enable_rx (hwidbtype *idb)
{
    quicc_instance_t *ds;


    QASSERT(idb);
    ds = INSTANCE;

    quicc_scc_enable_rx(ds->dprbase, ds->qinfo);
    idb_mark_unthrottled(idb);
    ds->enabled++;
}


/* 
 * =================================================================
 *  Name:	quicc_serial_periodic
 *
 *  Description:	
 *
 *  CALLER:	Called once every KEEPALIVE_DEFAULT (10) seconds from the
 *              net_periodic process.
 *		
 *
 *  PREQ:	
 *
 *  Returns:	None
 * =================================================================
 */
static void quicc_serial_periodic (hwidbtype *idb)
{
    serial_instance_t *ds;
    leveltype SR;
    
    QASSERT(idb);
    
    ds = INSTANCE;
    
    /*
     * Check to see if we're getting pinned by modem
     * control interrupts
     */
    if (ds->flap_enabled) {
	/*
         * We've got a runaway interface. Choo Choo!!
         */
	if (!ds->flap_backoff)
	    errmsg(&msgsym(LINEFLAP, QUICC_SERIAL), idb->unit);
	
	SR = raise_interrupt_level(NETS_DISABLE);	
	if (++ds->flap_backoff > SERIAL_FLAP_BACKOFF) {

	    /*
	     * We're finished with the probation period
	     */
	    ds->flap_count = 0;
	    TIMER_STOP(ds->flap_time);
	    ds->flap_backoff = 0;
	    ds->flap_enabled = FALSE;
		
	    /* Add protection in case quicc_serial_modem_interrupt_mask
	     * gets call from interrupt thread
	     * (eg. quicc_serial_modem_interrupt_handler)
	     */
	
	    quicc_modem_interrupt_mask(idb, ENABLE,QUICC_CIMR_DCD_MASK);

	}
	reset_interrupt_level(SR);
    }
}


/*
 * Check the given output ring entry for transmit errors.
 * Take the correct action for each error:
 */

/*
 * =================================================================
 *	Name:		quicc_serial_check_tx_errors
 *
 *	Description:	This routine will check the quicc SCC transmit
 *			buffer descriptor ring.  Upon determining the error
 *			the proper message will be display and statics
 *			counter	will be incremented.
 *
 *	CALLER:		ds->TX_error from quicc_scc_tx_intr()
 *
 *	PREQ:		None
 *
 *	Returns:		None
 * =================================================================
 */
static void quicc_serial_check_tx_errors (hwidbtype *idb,
					  ushort status)
{
    serial_instance_t           *ds;

    ds = (serial_instance_t *)INSTANCE;

    if (status & QUICC_SERIAL_TX_ERR) {
        if (status & QUICC_BDSTAT_TX_UNRRUN) {
            /*
             * Underrun error. FATAL. TXE set in SCCE.
             */
            idb->counters.output_underrun++;
	    errmsg(&msgsym(UNDERFLO, QUICC_SERIAL), idb->unit);
        }
        if (status & QUICC_BDSTAT_TX_CTSLOS) {
	    ds->output_ctslosts++;
	    errmsg(&msgsym(CTSLOST, QUICC_SERIAL), idb->unit);
        }
        ds->fatal_tx_err = TRUE;
        idb->counters.output_error++;
    } /* end if-error */
}


/*
 ******************* Interrupt routines ****************************
 */
/*
 * =================================================================
 *	Name:		quicc_serial_check_rx_errors
 *
 *	Description:	This routine will check the quicc SCC receive
 *			buffer descriptor ring.  Upon determining the error
 *			the proper message will be display and statics
 *			counter	will be incremented.
 *
 *	CALLER:		ds->RX_error from quicc_scc_rx_intr()
 *
 *	PREQ:		None
 *
 *	Returns:		None
 * =================================================================
 */
static void quicc_serial_check_rx_errors (hwidbtype *idb, ushort bd_status)
{

    QASSERT(idb);
    /*
     * We encountered an error while receiving this frame.
     */
    if (bd_status & QUICC_SERIAL_RX_ERR) {
        idb->counters.input_error++;

        if (bd_status & QUICC_BDSTAT_RX_FLV) {
            idb->counters.giants++;
        }
        if (bd_status & QUICC_BDSTAT_RX_NOFA) {
            idb->counters.input_frame++;
        }
        if (bd_status & QUICC_BDSTAT_RX_ABORT) {
            idb->counters.input_abort++;
        }
        if (bd_status & QUICC_BDSTAT_RX_CRCERR) {
            idb->counters.input_crc++;
        }
        if (bd_status & QUICC_BDSTAT_RX_OVRRUN) {
            idb->counters.input_overrun++;
        }
    }
}

/*
 * =================================================================
 * Name:  quicc_serial_tx_start
 *
 * Description:
 *	This routine will try to dequeue a packet and then send it
 *	on its way, if the packet exceed the valid packet size then
 *	it will discard the packet.  The routine quicc_scc_transmit
 *	will mark the buffer descriptor use by the QUICC to be as
 *	READY to prepare it to be service by the RISC chip which
 *	do the actual transmission.
 *
 * Parameters:
 *	hwidbtype 	- pointer to hwidb for this interface
 *
 * Returns:
 *	void - None.
 * =================================================================
 */
static void quicc_serial_tx_start (hwidbtype *idb)
{
    uint bytes;
    paktype *pak;
    quicc_instance_t *ds;
    leveltype SR;


    ds = INSTANCE;
    QUICC_WAN_TX_LED_ON(ds->dprbase);

    SR = raise_interrupt_level(NETS_DISABLE);
    
    /*
     * While there is another packet to output and room for it in the
     * TX queue, enqueue the packet:
     */
    while ((ds->tx_count < ds->tx_size) &&
	   (pak = (*idb->oqueue_dequeue)(idb))) {
	bytes = pak->datagramsize;
	
	if (bytes > idb->max_pak_size) {
	    errmsg(&msgsym(TOOBIG, LINK), idb->hw_namestring, bytes);
	    datagram_done(pak);
	    idb->counters.output_total_drops++;
	    continue;
	}
	
	/*
	 * send the packet on it's way
	 */
	quicc_scc_transmit((quicc_instance_t *)ds, pak,
			   QUICC_BDSTAT_TX_INT|QUICC_BDSTAT_TX_LAST|
			   QUICC_BDSTAT_TX_TRCRC);
	
	/*
	 * Do the accounting
	 */
	idb->counters.tx_cumbytes += bytes;
	TIMER_START(idb->xmittimer, TIMEOUT); /* Set timeout timer */
	idb->counters.outputs++;
	
	/*
	 * Count broadcasts
	 */
	if ((pak->enctype == ET_HDLC) &&
	    ((*(ushort *)pak->datagramstart) & HDLC_BCAST))
	    idb->counters.output_broad++;
    }
    reset_interrupt_level(SR);
}


/*
 * quicc_serial_fastsend
 * Transmit this packet immediately or drop it
 * Called from interrupt level.
 */

/*
 * =================================================================
 *	Name:		quicc_serial_fastsend
 *
 *	Description:	This routine handles the fast switching for the
 *		        interface.  This routine transmits the packet
 *			immediately or drop it.  
 *			
 *	CALLER:		Called from interrupt level.
 *
 *	PREQ:		None
 *
 *	Returns:		None
 * =================================================================
 */
static void quicc_serial_fastsend (hwidbtype *idb, paktype *pak)
{
    TXBD_T *tmd;
    quicc_instance_t *ds;
    uchar *start;
    ushort bytes;
    ulong acct_proto;
    paktype **txp;
    traffic_shape_t *shape;

    if (idb->state != IDBS_UP) {
        idb->counters.output_total_drops++;
        datagram_done(pak);
        return;
    }

    acct_proto = (ulong)pak->acct_proto;
    ds = INSTANCE;
    
    bytes = pak->datagramsize;

    /*
     * determine whether there is window available to send the message with
     * in the rate control bucket
     */
    shape = traffic_shape_required_inline(pak->if_output->traffic_shape, pak);
    if (shape) {
	traffic_shape_defer_sending(shape, pak);
	return;
    }

    /*
     * If no packets are queued for output on this interface,
     * try to immediately transmit the packet:
     */
    if (!idb->output_qcount) {

	/*
	 * Get pointer to ring entry and packet pointer at tx_head.
	 */ 
        txp = ds->head_txp;
	tmd = ds->tx_head;
	start = pak->datagramstart;
	
	/*
	 * Is there room for another packet in the output queue?
	 */
	if (ds->tx_count < ds->tx_size) {  

            /* Room in TX ring */

	    if ((tmd->status & QUICC_BDSTAT_TX_RDY) == 0) {
		
		/*
		 * Count broadcasts
		 */
		if ((pak->enctype == ET_HDLC) &&
		    ((*(ushort *)pak->datagramstart) & HDLC_BCAST))
		    idb->counters.output_broad++;
		
		/*
		 * Give the quicc ownership of the buffer now to reduce
		 * latency
		 */
                tmd->buf_ptr = start;
		tmd->length = bytes;
		tmd->status = QUICC_BDSTAT_TX_DEFAULT | 
		    (tmd->status & QUICC_BDSTAT_TX_WRAP);

		/*
		 * Force the quicc to poll the TX ring and see this packet:
		 */
                if (ds->tx_count == 0)
		    *ds->tx_todr = QUICC_TX_KICK;
		ds->tx_count++;
		
		/*
		 * Update the head pointer for the next packet we want to TX:
		 */
                *txp++ = pak;
		if (++ds->tx_head > ds->tx_max) { /* End of ring? */
		    ds->tx_head = ds->tx_ra;
                    txp = &ds->tx_p[0];
		}
                ds->head_txp = txp;

		/*
		 * Do packet accounting after starting the quicc:
		 */
		idb->counters.tx_cumbytes_fast[acct_proto] += bytes;
		idb->counters.outputs_fast[acct_proto]++;

		TIMER_START(idb->xmittimer, TIMEOUT); /* Set timeout timer */

	    } else {	 /* Uh oh. Problem. We don't own the buffer. */
		
		retbuffer(pak);
		idb->counters.output_total_drops++;
	    }

	    return;

	} else {			/* TX ring full */
	    
	    /*
	     * We have a full TX ring. Is the first packet in the queue done?
	     * If so, we can service the TX interrupt on this packet and use
	     * the ring entry for our new packet:
	     */
	    if ((tmd->status & QUICC_BDSTAT_TX_RDY) == 0) {
		/*
                 * This packet has been transmitted. Does the BD
		 * need cleaning up ?
		 */
		if (*txp) {

		    /*
                     * BD needs cleaning up.
		     * Check for TX  errors, then release packet buffer.
		     */
	 	    (*ds->TX_errors)(idb, tmd->status);

		    /*
		     * Do TX packet accounting and clean up:
		     */
		    datagram_done(*txp); /* Release output packet*/
		}

		/*
		 * Count broadcasts
		 */
		if ((pak->enctype == ET_HDLC) &&
		    ((*(ushort *)pak->datagramstart) & HDLC_BCAST))
		    idb->counters.output_broad++;

		/*
		 * Give the quicc ownership of the buffer.
		 */
                tmd->buf_ptr = start;
		tmd->length = bytes;
		tmd->status = QUICC_BDSTAT_TX_DEFAULT | 
		    (tmd->status & QUICC_BDSTAT_TX_WRAP);

		/*
		 * Update the head pointer for the next packet we want to TX:
		 */
                *txp++ = pak;
		if (++ds->tx_head > ds->tx_max) {
		    ds->tx_head = ds->tx_ra;
                    txp = &ds->tx_p[0];
		}
                ds->head_txp = txp;
		
		/*
		 * Have the next TX interrupt bypass this packet
		 * since we just took care of it:
		 */
                ds->tail_txp++;
		if (++ds->tx_tail > ds->tx_max) {
		    ds->tx_tail = ds->tx_ra;
                    ds->tail_txp = &ds->tx_p[0];
		}

		/*
		 * Do packet accounting after starting the quicc:
		 */

		/* Don't change ds->tx_count, it should stay the same */
		idb->counters.tx_cumbytes_fast[acct_proto] += bytes;
		idb->counters.outputs_fast[acct_proto]++;
		GET_TIMESTAMP(idb->lastoutput);
		TIMER_START(idb->xmittimer, TIMEOUT); /* set timeout timer */
		
	    } else {			/* BD still in use by QUICC */
		
		/*
		 * We have a full TX queue, so queue the packet in the output
		 * queue. Note that holq_enqueue requires the if_output and
		 * linktype fields to be initialized:
		 */
		holdq_enqueue_fast(idb, pak);
		quicc_serial_tx_start(idb);
	    }

	    return;
	}

    } else {				/* Output hold queue not empty */

	/*
	 * There are already packets on the output queue,
	 * so add this one to the end of the queue to minimize
	 * out of order situations:
	 */
	holdq_enqueue_fast(idb, pak);

	/*
         * If the software thinks the TX ring is full, go check with
         * the controller (and in particular, the ownership bits, and see
         * if we can clear some space before pulling frames off of
         * the holdq by faking a TX interrupt. The TX interrupt
         * routine will call quicc_serial_tx_start() for us.
         *
         * This causes a smoother saturation roll-off
         */
        if (ds->tx_count == ds->tx_size)
            quicc_scc_tx_intr(idb);
        else
	    quicc_serial_tx_start(idb);
    }
}

/*
 ******************* Display routines *****************************
 */
/*
 * Show quicc serial errors:
 */
/*
 * =================================================================
 *	Name:		quicc_serial_show_errors
 *
 *	Description:	Display the errors encounter so far by the interface
 *			
 *
 *	CALLER:		
 *
 *	PREQ:		None
 *
 *	Returns:		None
 * =================================================================
 */
static void quicc_serial_show_errors (hwidbtype *idb)
{
    serial_instance_t *ds = INSTANCE;

    printf("QUICC SCC specific errors:\n");

    printf("%d input aborts on receiving flag sequence\n",
	   idb->counters.input_abort);

    printf("%d throttles, %d enables\n", idb->counters.throttle_count,
	   ds->enabled);

    printf("%d overruns\n",idb->counters.input_overrun);

    printf("%d transmitter underruns\n", idb->counters.output_underrun);
    printf("%d transmitter CTS losts\n",ds->output_ctslosts);
    
}

/*
 * Show quicc serial status. Return true if we quit the display.
 */
/*
 * =================================================================
 *	Name:		quicc_serial_show_controller
 *
 *	Description:	Display all statisical information about this
 *			interface.
 *
 *	CALLER:		
 *
 *	PREQ:		None
 *
 *	Returns:		None
 * =================================================================
 */

static boolean quicc_serial_show_controller (hwidbtype *idb, parseinfo *csb)
{
    quicc_instance_t *ds = INSTANCE;

    QASSERT(ds);
    
    printf("\n\n%s unit %d, ", idb->typestring, idb->unit);
    
    /*
     *  Display scc general registers
     */
    quicc_scc_show_regs(idb,TRUE);
    
    
    c1000_show_cable_state(idb);
    
    /*
     * Display SCC generic stuff and
     * serial SCC specific stuff.
     */
    quicc_scc_show_hdlc_param(ds->parmbase);
    
    printf("\nbuffer size %d\n", idb->max_buffer_size);
    
    /*
     * Allocate memory for the RX and TX rings.
     * Disable network interrupts and take a snapshot of the 
     * RX and TX ring states.
     * Enable network interrupts.
     * Display the Rx and Tx rings.
     */
    if (!show_bdrings(idb, sizeof(RXBD_T))) {
	errmsg(&msgsym(NOMEMORY, QUICC), idb->unit, "copy of BD ring");
	return(TRUE);
    }
    
    /*
     * Display controller specific errors.
     */
    quicc_serial_show_errors(idb);
    
    return(FALSE);
}



/*
 * =================================================================
 * Name: quicc_serial_mode_reg_init
 *
 * Description:
 *	This is routine will enable the appropriate 
 *	offset of where the data can be found within a packet
 *	base on the encapsulation.
 *
 * Parameters:
 *	quicc_dpr_t		- pointer to dual port ram
 *	hwidbtype 		- pointer to hwidb for this interface
 *	ulong			- SCC port which we need to modify
 *
 * Returns:
 *	void - None.
 * =================================================================
 */
static void quicc_serial_mode_reg_init (quicc_dpr_t *dpr, hwidbtype *idb,
					ulong port_no)
{
    serialsb	*ssb;

    ssb = idb_get_hwsb_inline(idb, HWIDB_SB_SERIAL);

    /*
     * Change this later so that when you get the nrzi_enable
     * option then mask that into ds->gsmrl and ds->gsmrh.  And
     * during reset, they should be use to initialize the actual
     * dual port ram registers.  Upon startup, these two variables
     * need to get set with information set in the configuration
     * file.
     */
    if (ssb->nrzi_enable)
	quicc_scc_write_gsmr(dpr, port_no, 
                             QUICC_SCC_NRZI_RX | QUICC_SCC_NRZI_TX, 
                             QUICC_GSMR_NONE);
    
    if (ssb->invert_txc)
	quicc_scc_write_gsmr(dpr, port_no, QUICC_SCC_GSMRL_TCINV,
			     QUICC_GSMR_NONE);

    quicc_scc_write_psmr(dpr, port_no, QUICC_PSMR_NOF(idb->xmitdelay));
}


/*
 * =================================================================
 *	Name:		quicc_serial_scc_init
 *
 *	Description:	This routine initializes the scc's registers,
 *			ring buffer descriptor and hdlc parameter for this
 *			particular scc.  Please see below for more detail
 *			description.  
 *			
 *	CALLER:		quicc_serial_driver_setup()  (being evaluated)
 *		        by ds->hw_init within quicc_serial_reset()
 *
 *	PREQ:		None
 *
 *	Returns:		None
 * =================================================================
 */
static void quicc_serial_scc_init (hwidbtype *idb, boolean maxdgram_change)
{
    serial_instance_t *ds;
    quicc_dpr_t *dpr;
    ulong port_no;
    leveltype SR;

    QASSERT(idb);
    ds = (serial_instance_t *)INSTANCE;
    dpr = (quicc_dpr_t *)ds->dprbase;
    port_no = ds->port_no;

    SR = raise_interrupt_level(NETS_DISABLE);
    
    cpai_set_loop(idb->loopback);
    
    quicc_cpic_handler_install(idb,
                               ds->dprbase,
                               DCD_INTR_INDEX,
                               QUICC_CPIC_INTERRUPT_PC11,
                               QUICC_CIMR_DCD_MASK,
                               quicc_serial_modem_interrupt_handler,
                               quicc_serial_dcd_interrupt);

    /*
     * Initialize the quicc SCC
     * - program the GSMR (except ENT and ENR bits)
     * - program the PSMR
     * - program the DSR
     * - initialize the RX and TX BD rings
     * - initialize common parameter RAM
     * - initialize hdlc parameter RAM
     * - clear SCCE events
     * - initialize CICR, CIPR, CIMR
     */
    
    /*
     * Initialize GSMR, PSMR, DSR
     */
    quicc_scc_reg_init(dpr, port_no);


    ds->fatal_tx_err = FALSE;
    /*
     * Initialize GSMR with specific configuration values from the
     * command line such as NRZI, Invert transmit clock...
     */
    quicc_serial_mode_reg_init(dpr, idb, port_no);

    les_serial_init_offset(idb);

    /*
     * Make sure encap is long aligned
     */
    idb->start_offset &= ~0x03;
    
    /*
     * Determine if the memory pool needs adjustment or allocation.
     */
    memory_pool_adjustment(idb,maxdgram_change);
    
    /*
     * Clean up RX and TX BD rings
     */
    quicc_scc_bd_ring_init(idb, maxdgram_change);
    
    /*
     * Initialize SCC general parameter RAM
     * Initialize SCC serial parameter RAM, SCCE, CICR, CIPR
     */
    
    /*
     *  Let's set the clock so that SCC4 uses RX CLK5 and
     *  TX CLK7.  We may think about clearing these two register
     *	if our loopback routine is going to mess with the sicr
     *	register.
     */
    ds->dprbase->regs.si_sicr |=
	(QUICC_SICR_RXCS(QUICC_PORT_SCC4, QUICC_SCC_TO_CLK5) |
          QUICC_SICR_TXCS(QUICC_PORT_SCC4, QUICC_SCC_TO_CLK7));


    quicc_hdlc_param_init(dpr, port_no, idb->max_buffer_size);
    
    quicc_scc_write_gsmr(dpr, port_no, QUICC_GSMR_NONE, QUICC_HDLC_IDLE_FLAGS);
    reset_interrupt_level(SR);
}

/*
 * =================================================================
 *	Name:		quicc_serial_RX_interrupt
 *
 *	Description:	This routine is use to process received packets.
 *			It utualize the common code in the general serial
 *			driver.  Because this code is in the received
 *			data path, it will later be put inline.
 *
 *	CALLER:		By the ds->rx_handler within quicc_scc_rx_intr()
 *
 *	PREQ:		None
 *
 *	Returns:		None
 * =================================================================
 */
static void quicc_serial_RX_interrupt (hwidbtype *idb, paktype *pak)
{
    quicc_instance_t *ds = INSTANCE;

    QASSERT(idb);
    QASSERT(pak);

    if (!idb->hdlc_lineup) {
	idb->hdlc_lineup = TRUE;
	idb->reason = "Keepalive OK";
    }
    serial_RX_interrupt_inline(idb, pak);
    QUICC_WAN_RX_LED_ON(ds->dprbase);
}



/*
 * =================================================================
 *	Name:		quicc_serial_set_maxdgram
 *
 *	Description:	This routine will set the maxdgram for this
 *			interface and call the quicc_reset_wrapper
 *			to 'reset' and  re-initialize the SCC to
 *			set the MTU's.
 *
 *	CALLER:		Called by the max_mtu_command to change the 
 *                      maximum transmission unit (MTU) for an interface.
 *
 *	PREQ:		None
 *
 *	Returns:		None
 * =================================================================
 */
static boolean quicc_serial_set_maxdgram (hwidbtype *idb, int buffersize,
					  int maxdgram)
{
    if_maxdgram_adjust(idb, maxdgram);

    quicc_serial_reset(idb,TRUE);
    return(TRUE);
}

/*
 * =================================================================
 * Name: quicc_show_serial_state
 *
 * Description:
 *	This routine acts as a wrapper to satisfy the vector
 *	interface for show_serial_state.
 *
 * Parameters:
 *	hwidbtype 		- pointer to hwidb for this interface
 *	quicc_instance_t 	- common instance structure for LES 
 *
 * Returns:
 *	void - None.
 * =================================================================
 */
static void quicc_show_serial_state (hwidbtype *idb, char *leader)
{
    c1000_show_serial_state(idb);
}

static boolean quicc_hw_set_config (hwidbtype *hwidb, itemlist *ilist,
				    serialsb *ssb)
{
    itemdesc *item;
    boolean any_unhandled = FALSE;   /* assume we've handled all items */
    tinybool this_item_unhandled;

    while ((item = itemlist_getnext(ilist))) {
	if (item->handled)
	    continue;
	this_item_unhandled = FALSE;

	switch (item->item) {
	  case SERIAL_HW_INVERT_TXC:
	    ssb->invert_txc = item->u.l_value;
	    quicc_serial_reset(hwidb, FALSE);
	    break;

	  case SERIAL_HW_NRZI_ENABLE:
	    ssb->nrzi_enable = item->u.l_value;
	    quicc_serial_reset(hwidb, FALSE);
	    break;

	  default:
	    this_item_unhandled = TRUE;
	    any_unhandled = TRUE;
	    break;
	}

	if (!this_item_unhandled)
	    item->handled = TRUE;
    }

    return(!any_unhandled);
}

static boolean quicc_hwidb_state_config (hwidbtype *hwidb, ulong opcode,
					 itemlist *ilist)
{
    boolean ret;
    serialsb *ssb;

    if ((hwidb == NULL) && (ilist == NULL))
	return (FALSE);

    if (ilist->item_count == -1)
	ilist->item_count = itemlist_count(ilist);

    ssb = idb_use_hwsb_inline(hwidb, HWIDB_SB_SERIAL);
    if (ssb == NULL)
	return (FALSE);

    switch (opcode) {
      case IDB_CONTROL_SET_CONFIG:
	ret = quicc_hw_set_config(hwidb, ilist, ssb);
	break;

      case IDB_CONTROL_READ_CONFIG:
      case IDB_CONTROL_SET_STATE:
      case IDB_CONTROL_READ_STATE:

      default:
	ret = FALSE;
	break;
    }
    idb_release_hwsb_inline(hwidb, HWIDB_SB_SERIAL);

    if (!ret) {
	itemlist_reset(ilist);
	ret = (*ssb->hw_super_state_config)(hwidb, opcode, ilist);
    }
    return (ret);
}



/*
 * =================================================================
 *	Name:		quicc_serial_idb_init
 *
 *	Description:	Initialize the idb and by setting all of the
 *			call back function handlers.
 *
 *	CALLER:		
 *
 *	PREQ:		None
 *
 *	Returns:		None
 * =================================================================
 */
static void quicc_serial_idb_init (hwidbtype *idb)
{
    serialsb *ssb;

    QASSERT(idb);

    les_serial_init_idb_common(idb);

    idb->type = IDBTYPE_QUICC_SERIAL;
    idb->typestring = "QUICC Serial";
    idb->serial_flags |= SERIAL_FLAGS_CAN_ALT_FLAG;
    idb->serial_flags |= SERIAL_FLAGS_CAN_CLK_INV | SERIAL_FLAGS_CAN_NRZI;

    /*
     * Set up function calls
     */
    idb->soutput = quicc_serial_tx_start;
    idb->reset = quicc_serial_reset_wrapper;
    idb->shutdown = quicc_serial_shutdown;
    idb->enable = quicc_serial_enable_rx;
    idb->dte_interface = TRUE;
    idb->start_offset  = ENCAPBYTES - idb->encsize;

    idb->setup_bridging = quicc_serial_reset_wrapper;
    idb->device_periodic = quicc_serial_periodic;

    idb->txqlength = les_driver_txqlength;
    idb->show_controller = quicc_serial_show_controller;
    idb->show_serial_state = quicc_show_serial_state;
    idb->fastsend = quicc_serial_fastsend;
    /*
     * idb->set_maxdgram must be initialized after calling serial_idb_init()
     * so that hdlc_setup does not attempt to call quicc_serial_set_maxdgram()
     * while doing system init.
     */
    
    idb->set_maxdgram = quicc_serial_set_maxdgram;

    /*
     * Set up state config vector
     */
    ssb = idb_use_hwsb_inline(idb, HWIDB_SB_SERIAL);
    ssb->hw_super_state_config = idb->state_config;
    idb->state_config = quicc_hwidb_state_config;
    idb_release_hwsb_inline(idb, HWIDB_SB_SERIAL);
    
    serial_idb_enqueue(idb);
    idb_enqueue(idb);
}

/*
 * =================================================================
 *	Name:		quicc_serial_driver_setup
 *
 *	Description:	Set up driver data structure by the following:
 *
 * 			- Malloc driver data structure and initialize it.
 * 			- initialize port number for SCC
 * 			- get RX and TX BD ring base addresses
 * 			- initialize RX and TX pak pointer arrays
 * 			- init reg values, error counts, pointers, etc
 * 			- Set up the serial SCC in the quicc.
 * 			- configure the parallel i-o port pins
 *   			- PAPAR, PADIR, PAODR
 *   			- PCPAR, PCDIR, PCSO 
 *   			- program SICR
 * 			- initialize the registers
 *   			- GSMR (except ENT and ENR bits)
 *   			- PSMR, DSR, CICR, CIPR, CIMR
 *   			- clear SCCE events
 * 			- initialize common parameter RAM
 *   			- initialize hdlc parameter RAM
 *
 *	CALLER:		
 *
 *	PREQ:		None
 *
 *	Returns:		None
 * =================================================================
 */
static void quicc_serial_driver_setup (hwidbtype *idb, long membase, long port)
{
    serial_instance_t *ds;
    int rx_ring_size, tx_ring_size;
    
    parser_add_commands(PARSE_ADD_CFG_INT_AS_CMD,
                        &pname(quicc_serial_interface_commands), "quicc");
    
    
    /*
     * Allocate the driver data structure:
     */
    ds = malloc(sizeof(serial_instance_t));
    if (!ds) {
	errmsg(&msgsym(NOMEMORY, QUICC), idb->unit, "driver structure");
	crashdump(0);
    }

    ds->qinfo = malloc(sizeof(quicc_info_t));

    if (!ds->qinfo) {
	errmsg(&msgsym(NOMEMORY, QUICC), idb->unit, "driver structure");
	crashdump(0);

    }
	
    /*
     * QUICC specific idb init.
     */
    quicc_serial_idb_init(idb);
    INSTANCE =  ds;
    
    ds->qinfo->port_no = ds->port_no  = port;
    
    ds->dprbase          = (quicc_dpr_t *)membase;
    ds->parmbase         = (void *)&ds->dprbase->scc4.hdlc;
    
    select_ring_size(&rx_ring_size, &tx_ring_size, idb->max_buffer_size);
    
    ds->max_rx_ring_size = min(QUICC_RXBD_NUM_SCC4, rx_ring_size);
    ds->max_tx_ring_size = min(QUICC_TXBD_NUM_SCC4, tx_ring_size);
    
    ds->rx_bd_mask       = QUICC_SERIAL_RX_MASK;
    ds->rx_bd_good_mask  = QUICC_BDSTAT_RX_PKT;
    ds->tx_bd_mask       = QUICC_SERIAL_TX_MASK;
    ds->tx_bd_good_mask  = QUICC_BDSTAT_TX_PKT;


    /*
     * Add scce and sccm mask.  These mask will be use to enable
     * and disable interrupt as well as clear events within the
     * routines used to enable and disable ports.
     */
    ds->qinfo->rx_enable_sccm_mask   = QUICC_RX_HDLC_ENABLE_SCCM_MASK;
    ds->qinfo->tx_enable_sccm_mask   = QUICC_TX_HDLC_ENABLE_SCCM_MASK;
    
    ds->qinfo->rx_clear_scce_event   = QUICC_RX_HDLC_CLEAR_SCCE_EVENT;
    ds->qinfo->tx_clear_scce_event   = QUICC_TX_HDLC_CLEAR_SCCE_EVENT;

    ds->qinfo->rx_disable_sccm_mask  = QUICC_RX_HDLC_DISABLE_SCCM_MASK;
    ds->qinfo->tx_disable_sccm_mask  = QUICC_TX_HDLC_DISABLE_SCCM_MASK;
    

    ds->crc_length       = QUICC_HDLC_CRC_SIZE;
    
    ds->hw_stop          = quicc_serial_stop;
    ds->hw_start         = quicc_serial_start;
    ds->hw_init          = quicc_serial_scc_init;
    ds->hw_health        = NULL;
    ds->hw_mode          = NULL;

    ds->hw_throttle      = quicc_serial_throttle;
    
    ds->hw_enable        = NULL;
    ds->TX_errors        = quicc_serial_check_tx_errors;
    ds->RX_errors        = quicc_serial_check_rx_errors;
    ds->rx_handler       = quicc_serial_RX_interrupt;
    ds->tx_handler       = NULL;
    ds->misc_handler     = NULL;
    
    ds->fatal_tx_err	= FALSE;
    ds->max_flap_rate 	= SERIAL_FLAP_RATE;
    ds->flap_count	= 0;
    TIMER_STOP(ds->flap_time);
    ds->flap_backoff	= 0;
    ds->flap_enabled	= FALSE;

    /*
     * Set up driver stuff common to SCCs.
     */
    quicc_scc_driver_setup(idb);
    
    /*
     * Set up i-o port pins
     */
    quicc_set_serial_pins(ds->dprbase);
    
    
    /*
     * Install RX, TX, and error interrupt handlers
     */
    quicc_scc_handler_install(idb, ds->port_no);
    
    
    cpai_init();

    /* don't know if this should be set to TRUE yet?  but should
     * not matter since we are setting up driver initially.
     */
    quicc_serial_scc_init(idb,TRUE);
    
    quicc_serial_start(idb);
}

/*
 * =================================================================
 * Name: quicc_serial_set_transmit_delay
 *
 * Description:
 *	This routine is set the number of transmit flags 
 *
 * Parameters:
 *	hwidbtype 	- pointer to hwidb for this interface
 *	int		- How much to delay
 *
 * Returns:
 *	void - None.
 * =================================================================
 */
static void quicc_serial_set_transmit_delay (hwidbtype *idb, int delay_flag)
{
    serialsb *ssb = idb_use_hwsb_inline(idb, HWIDB_SB_SERIAL);
    /* delay_flag range check is done by parser */
    ssb->hdx_transmit_delay = idb->xmitdelay = delay_flag;
    idb_release_hwsb_inline(idb, HWIDB_SB_SERIAL);

    quicc_serial_reset(idb,FALSE);
}

/*
 * =================================================================
 *	Name:		quicc_serial_subsys_init
 *
 *	Description:	This routine will add/register functions to global
 *			function registry, this will allow the added function
 *			to be called later when the 'invoke' function
 *			is called with 'QUICC_DRIVER' as an argument.
 *			
 *	CALLER:		SUBSYS_HEADER
 *
 *	PREQ:		None
 *
 *	Returns:		None
 * =================================================================
 */
static void quicc_serial_subsys_init (subsystype *subsys)
{
    
    reg_add_serial_driver_setup(QUICC_DRIVER, 
                                quicc_serial_driver_setup,
                                "quicc_serial_driver_setup");
    
    reg_add_media_transmit_delay(IDBTYPE_QUICC_SERIAL,
				 quicc_serial_set_transmit_delay,
                                 "quicc_serial_set_transmit_delay");
    
    reg_add_media_set_rxoffset(IDBTYPE_QUICC_SERIAL,
			       quicc_serial_reset_wrapper,
			       "quicc_serial_reset_wrapper");

    reg_add_if_onesec(quicc_serial_onesec_periodic,
		      "quicc_serial_onesec_periodic");
}

/*
 * QUICC serial subsystem header
 */
#define QUICC_SERIAL_MAJVERSION   1
#define QUICC_SERIAL_MINVERSION   0
#define QUICC_SERIAL_EDITVERSION  1


SUBSYS_HEADER(quicc_serial, QUICC_SERIAL_MAJVERSION, 
              QUICC_SERIAL_MINVERSION, QUICC_SERIAL_EDITVERSION,
	      quicc_serial_subsys_init, SUBSYS_CLASS_DRIVER,
	      NULL, NULL);
