/* $Id: if_async_quicc.c,v 3.1.6.4 1996/08/28 13:10:15 thille Exp $
 * $Source: /release/112/cvs/Xsys/src-36/if_async_quicc.c,v $
 *------------------------------------------------------------------
 * if_async_quicc.c  68360 QUICC chip async serial driver for low end
 *
 * March 1996, John Ng
 *
 * Copyright (c) 1996-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: if_async_quicc.c,v $
 * Revision 3.1.6.4  1996/08/28  13:10:15  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.1.6.3  1996/08/13  02:22:13  gstovall
 * CSCdi39484:  Giant packets output on ethernet interface
 * Branch: California_branch
 * Make sure we drop giants, even if they are just a wee bit too big.
 *
 * Revision 3.1.6.2  1996/06/18  17:37:54  hampton
 * Various 11.2 compilation problems.  Fix modular cobra breakage.
 * [CSCdi60703]
 * Branch: California_branch
 *
 * Revision 3.1.6.1  1996/06/12  19:56:27  jng
 * CSCdi60260:  Async catwoman commit
 * Branch: California_branch
 * Async Catwoman commit into California
 *
 * Revision 3.1  1996/03/20  01:27:07  jng
 * Initial Async Catwoman placeholder
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "sys_registry.h"
#include "packet.h"
#include "interface_private.h"

#include "ttysrv.h"
#include "subsys.h"
#include "../os/pool.h"
#include "../os/async.h"

#include "../les/if_les.h"
#include "../les/les_driver_inline.h"
#include "logger.h"
#include "../src-36/msg_quicc.c"	/* Not a typo, see logger.h */
#include "../src-36/msg_serial_quicc.c"	/* Not a typo, see logger.h */
#include "../src-36-c1000/msg_c1000_ether.c" /* Not a typo, see logger.h */
#include "../ts/if_async.h"
#include "fastswitch_registry.h"
#include "../src-36/quicc.h"
#include "../src-36-c1000/init_c1005.h"
#include "../src-36/quicc_serial_common.h"
#include "../src-36/if_async_quicc.h"

#include "../src-36-c1000/cpai.h"
#include "../les/if_les_async.h"



vectortype quicc_async_vector;
static uint quiccAsyncBase;  /* tty line base for this device - usually 1 */

/*
 * =================================================================
 * Name: quicc_async_tty_init_txptrs
 *
 * Description:
 *	Fill in tty output pointers to use the given packet's data area.
 *	It is a prequisite that the packet pass in is previously allocated.
 *	Data is assumed to be placed starting at datagramstart, and
 *	pak->length is assumed to hold the amount of space available in the
 *	buffer for transmit data to be enqueued.
 *
 * Parameters:
 *	quicc_instance_t * - Generic quicc instance pointer
 *
 * Returns:
 *	void
 *
 * =================================================================
 */
inline static void
quicc_async_tty_init_txptrs (tt_soc *tty, paktype *pak)
{
    QASSERT(pak);

    tty->outhead = tty->outtail = tty->outstart = pak->datagramstart-1;
    tty->outend = tty->outstart + pak->length;
    tty->outsize = pak->length;
    tty->outcount = 0;
}

/*
 * =================================================================
 * Name: quicc_async_alloc_txbuf
 *
 * Description:
 *	This routine will return the number of available transmit
 *	buffer descriptor.  Available is defined to be transmitted
 *	and confirmed.
 *
 * Parameters:
 *	quicc_instance_t * - Generic quicc instance pointer
 *
 * Returns:
 *	void
 *
 * =================================================================
 */
inline static void
quicc_async_alloc_txbuf (tt_soc *tty)
{
    paktype *pak;
    hwidbtype *idb = tty->tty_idb;
    quicc_instance_t *ds;
    
    QASSERT(idb);

    ds = INSTANCE;

    pak = les_driver_getbuf_inline(idb, TRUE);
    pak->length = pak->pool->size;
    /* Assert now but later we may want to just return and give status */
    QASSERT(pak); 
    if (pak) {
	enqueue(&tty->txpakq, pak);
	quicc_async_tty_init_txptrs(tty, pak);
    }
}


/*
 * =================================================================
 * Name: quicc_send_txochar
 *
 * Description:
 *	This routine is used to send out of band character such as
 *	XON/XOFF.  The QUICC will poll the toseq register and send
 *	this character out as long as the UART is enable even if
 *	the UART is in freeze mode, during buffer transmission and
 *	when no buffer is available for transmission.
 *
 * Parameters:
 *	quicc_uart_param_t * - SCC UART specific parameter pointer
 *	uchar out_of_band_char - Character to be sent out.
 *
 * Returns:
 *	void
 *
 * =================================================================
 */
static void
quicc_send_txochar (hwidbtype *idb, uchar out_of_band_char)
{
    int count;
    leveltype SR;
    quicc_instance_t *ds = INSTANCE;
    quicc_uart_param_t *parmptr = ds->parmbase;
    quicc_dpr_t *dpr;    

    /*
     * Wait until CP clears the "READY" bit, in case it is in the 
     * process of sending an out-of-sequence character already.
     * This routine can potentially loop until QUICC_CPC_SPIN_MAX
     * if CTS is being honor and CTS is not asserted.
     */
    for (count = 0; count < QUICC_CPC_SPIN_MAX; count++) {
        if ((parmptr->toseq & QUICC_TOSEQ_READY) == 0)
            break;
    }

    /* Last command to send out of sequence character was not able
     * to be process.  This can happen if for some reason the transmitter
     * was not able to transmit data.  Example, the transmitter was
     * turn off when the send of out of sequence character command
     * was issue, or if hardware flow control is configure and CTS
     * is not available.  Let's disable the transmitter so that the
     * RISC won't try to transmit anything, then clear the "READY" bit
     * so the RISC processor won't try to send this old character
     * anymore.
     */
    if (count == QUICC_CPC_SPIN_MAX) {
	dpr = (quicc_dpr_t *)ds->dprbase;
	quicc_scc_disable_tx(dpr, ds->qinfo);
	parmptr->toseq &= ~QUICC_TOSEQ_READY;
	quicc_scc_enable_tx(dpr, ds->qinfo);
    }
    
    SR = raise_interrupt_level(NETS_DISABLE);

    parmptr->toseq = QUICC_TOSEQ_READY | out_of_band_char;
     
    reset_interrupt_level(SR);
}

/*
 * =================================================================
 * Name: quicc_async_tty_xon
 *
 * Description:
 *	This routine will send out a XON if software flow control
 *	is enabled.
 *
 * Parameters:
 *	tt_soc * - tty structure pointer
 *
 * Returns:
 *	TRUE - if software flow control is enable
 *	FALSE - if software flow control is NOT enable
 *
 * =================================================================
 */
static boolean quicc_async_tty_xon (tt_soc *tty)
{
    hwidbtype *idb = tty->tty_idb;
    
    QASSERT(idb);

    if (tty->capabilities & SFLOWOUT) {
	quicc_send_txochar(idb, tty->tty_startchar);
	/* Clear the fact that we sent a XOFF sometime ago,
	 * The XON we are sending should cancel that XOFF.
	 */
	tty->statbits &= ~(SENTXOFF);
        if (serial_debug)
            buginf("\nquicc_async_tty_xon: sending XON");
	return(TRUE);
    }
    return(FALSE);
}

/*
 * =================================================================
 * Name: quicc_async_tty_xoff
 *
 * Description:
 *	This routine will send out a XOFF if software flow control
 *	is enabled.
 *
 * Parameters:
 *	tt_soc * - tty structure pointer
 *
 * Returns:
 *	TRUE - if software flow control is enable and we have
 *	       not already sent out a XOFF.
 *	FALSE - if software flow control is NOT enable or 
 *		we have already sent a XOFF
 *
 * =================================================================
 */
static boolean quicc_async_tty_xoff (tt_soc *tty)
{
    hwidbtype *idb = tty->tty_idb;

    QASSERT(idb);

    if ((tty->capabilities & SFLOWOUT) && !(tty->statbits & SENTXOFF)) {
	quicc_send_txochar(idb, tty->tty_stopchar);
	/* Mark that we are sent a XOFF so that upper layer knows
	 * about it.  We will clear this when we send a XON.
	 */
	tty->statbits |= SENTXOFF;
        if (serial_debug)
            buginf("\nquicc_async_tty_xoff: sending XOFF");
	return(TRUE);
    }
    return(FALSE);
}


/*
 * =================================================================
 * Name: quicc_async_tty_intr
 *
 * Description:
 *	This routine is used to transmit data.  It actually toggle
 *	flags on the buffer descriptor to tell the QUICC that a packet
 *	is ready to be send out.  This routine can be call from both
 *	an interrupt stack via quicc driver library or from a process
 *	level routine with interrupt raised idb->soutput (startoutput).
 *	Unlike CD2430, the outpak is set when no more resources is
 *	available to accept more data, the tty->outpak is then set to
 *	the current packet being sent out.
 *
 * Parameters:
 *	hwidbtype *idb - hardware idb
 *
 * Returns:
 *	void
 *
 * =================================================================
 */
void
quicc_async_tx_intr (hwidbtype *idb)
{
    quicc_instance_t *ds = INSTANCE;
    tt_soc *tty = idb->idb_tty;
    paktype *pak;
    uint count;

    if (tty->outpak) {
#if DEBUG
	if (serial_debug)
	    buginf("\nquicc_async_tx_intr: outpak set, returning");
#endif
	return;
    }

    if (tty->txservice) {
	/*
	 * Txservice is handled here by pulling bytes from it and
	 * queueing to the normal TTY output buffer.  With all the
	 * bytes lined up in that buffer, we can start a simple DMA.
	 *
	 * Note that outtail is advanced before storing a byte.
	 */
	count = (*(tty->txservice))(tty, tty->outtail + 1,
				    tty->outend - tty->outtail);
	tty->outcount += count;
	tty->outtail += count;
    }
    
    /*
     * At this point, outhead+1..outtail describe a range of
     * memory which should be transmitted.  This memory has
     * either been filled in with putc()'s elsewhere, or filled
     * from our txservice right above; we don't care, it's just
     * data from here.
     */
    if (tty->outcount > 0) {

        QUICC_WAN_TX_LED_ON(ds->dprbase);

	/*
	 * Take packet from assembly position, clear its presence
	 * from the tt_soc
	 */
	pak = dequeue(&tty->txpakq);
        QASSERT(pak != NULL);
	tty->outstart = NULL;
	
	pak->datagramsize = tty->outcount;

	/* If we got here then the should be at least 1 slot
	 * available and if there is only 1 slot available then
	 * set outpak else leave outpak along since the next
	 * packet that wants to go out is guarantee to have a
	 * buffer descriptor.
	 */
	QASSERT(quicc_tx_slots_available != 0);
        if (quicc_tx_slots_available(ds) == QUICC_TX_LAST_BD) 
	    tty->outpak = pak;
	
	/*
	 * Check to make sure the datagramsize is not zero, if it
	 * is then we really shouldn't be sending the packet out.
	 */
	QASSERT(pak->datagramsize > 0);
	QASSERT(pak->datagramstart);
	
	quicc_scc_transmit(ds, pak, QUICC_BDSTAT_TX_INT);
    }
    
    /*
     * Since data is always reference by tty->outhead, tty->outail,
     * tty->outend, when a packet is sent out, we must allocate a
     * new packet and assign these tty->out* fields to point to this
     * newly allocate buffer.  When data is ready to be send out
     * it will be either store already in the tty->out* fields or it
     * will be pull in via tty->txservice routine.
     */
    if (tty->outstart == NULL) {
	quicc_async_alloc_txbuf(tty);
    }
}


/*
 * =================================================================
 * Name: quicc_async_startoutput
 *
 * Description:
 *	This routine is the output interface to the upper layer rather
 *	then calling quicc_async_tty_intr() directly.  When a user
 * 	wants to send data, they will interface the driver via
 *	idb->soutput or startoutput.  This routine will check to see
 *	if we are flow control before trying to sending data.  
 *
 * Parameters:
 *	tty_soc * - tty pointer
 *
 * Returns:
 *	TRUE - always (seems odd but that is what other async driver
 *	do.
 *
 * =================================================================
 */
static boolean
quicc_async_startoutput (tt_soc *tty)
{
    leveltype SR;
    quicc_instance_t *ds;
    quicc_dpr_t *dpr;
    hwidbtype *idb;
    

    idb = tty->tty_idb;
    
    ds = INSTANCE;

    dpr = (quicc_dpr_t *)ds->dprbase;
    
    /*
     * Don't do anything if other side has stopped us from sending
     */
    if (tty->statbits & (HOLDING | WANTXON)) {
	/* Should we really return TRUE? jng */
	return(TRUE);
    }
    
    SR = raise_interrupt_level(NETS_DISABLE);

    /*
     *  If someone has already released us then FRZ bit should
     *  be clear if it is not already done so.
     */
    if (quicc_scc_tset_psmr(dpr, ds->port_no, QUICC_PSMR_FRZ)) 
        quicc_scc_clear_psmr(dpr, ds->port_no, QUICC_PSMR_FRZ);

    /*
     * Start transmission if it's idle
     */
    quicc_async_tx_intr(tty->tty_idb);

    /*
     * Restore interrupts if not running in interrupt handler
     */
    reset_interrupt_level(SR);

    return(TRUE);
}

/*
 * =================================================================
 * Name: quicc_async_stopoutput
 *
 * Description:
 *	This routine is use to flow control our transmission if
 *	software flowcontrol is enable.
 *
 * Parameters:
 *	tty_soc * - tty pointer
 *
 * Returns:
 *	void
 *
 * =================================================================
 */
static void quicc_async_stopoutput (tt_soc *tty)
{
    hwidbtype *idb = tty->tty_idb;
    quicc_instance_t *ds = INSTANCE;
    quicc_dpr_t *dpr = (quicc_dpr_t *)ds->dprbase;
    
    if (tty->capabilities & SFLOWIN) {
	quicc_scc_write_psmr(dpr, ds->port_no, QUICC_PSMR_FRZ);
	tty->statbits |= WANTXON;
    }
}


/*
 * =================================================================
 * Name: quicc_async_putpkt
 *
 * Description:
 *	This routine is used to s
 *
 * Parameters:
 *	tty_soc * - tty pointer
 *	paktype * - pak
 *	uchar *	  - datastart
 *	int	  - length of datagram to go out
 *
 * Returns:
 *	Number of bytes to be outputed.  Return 0 if we been flow
 *	control.
 *
 * =================================================================
 */
static int
quicc_async_putpkt (tt_soc *tty, paktype *pak, uchar *datastart, int length)
{
    hwidbtype *idb = tty->tty_idb;
    quicc_instance_t *ds = INSTANCE;
    leveltype SR;
    
    /*
     * If padding needs to be added, do it as a byte stream, and stuff
     * into outhead/tail/etc.
     */
    if (tty->current.padding_ptr) {
	return(generic_putpkt(tty, pak, datastart, length));
    }

    /*
     * Access this TTY.  If we're not currently allowed to transmit,
     * tell them to wait.
     */
    if (tty->statbits & HOLDING) {
	return(0);
    }

    /*
     * If we notice a byte stream of output is pending, wait until
     * it has cleared.  Note tty_dallytimer() will not wait forever.
     */
    if (tty->outcount != 0) {
	tty_dallytimer(tty);
	return(0);
    }

    /*
     * If we're wrapping up, just lie and say it's all taken care of.
     */
    if (tty->statbits & (CARDROP|IDLE)) {
	return(length);
    }

    /*
     * Place a lock on the packet, and mark it as our current output
     * packet.  Start DMA.
     */
    SR = raise_interrupt_level(NETS_DISABLE);
    if (tty->outpak == NULL) {
	pak_lock(pak);

        QUICC_WAN_TX_LED_ON(ds->dprbase);

        if (quicc_tx_slots_available(ds) == QUICC_TX_LAST_BD)
	    tty->outpak = pak;
	
	quicc_scc_transmit_buffer(ds, pak, datastart, length, QUICC_BDSTAT_TX_INT);
    } else {
	/*
	 * Raced with a transmit interrupt, probably a TXMPTY on the
	 * FIFO emptying.
	 */
	length = 0;
    }
    reset_interrupt_level(SR);

    /*
     * If we in fact started the transmit, start a timer.  Return
     * result.
     */
    if (length) {
	tty_dallytimer(tty);
    }
    return(length);
}

/*
 * =================================================================
 * Name: quicc_async_show_errors
 *
 * Description:
 *	This routine is used to show async errors.
 *
 * Parameters:
 *	hwidbtype * - hardware IDB
 *
 * Returns:
 *	void
 *
 * =================================================================
 */
static void 
quicc_async_show_errors (hwidbtype *idb)
{
    quicc_async_instance_t *ds = INSTANCE;

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
 * =================================================================
 * Name: quicc_async_show_controllers
 *
 * Description:
 *	This routine is used to show QUICC specific information
 *
 * Parameters:
 *	hwidbtype * - hardware IDB
 *	parseinfo * - parse information pointer
 *
 * Returns:
 *	void
 *
 * =================================================================
 */
static boolean quicc_async_show_controller (hwidbtype *idb, parseinfo *csb)
{
    quicc_async_instance_t *ds = INSTANCE;
    
    QASSERT(ds);
    
    printf("\n\n%s unit %d, ", idb->typestring, idb->unit);
    
    
    /*
     *  Display scc general registers
     */
    quicc_scc_show_regs(idb,TRUE);
	

    /* Probably needs to rip out clocking information */
    c1000_show_cable_state(idb);
	
    /*
     * Display SCC generic stuff and
     * serial SCC specific stuff.
     */
    quicc_scc_show_uart_param(ds->parmbase);

#ifdef DEBUG
    printf("[PSMR] = %x\n",ds->dprbase->regs.scc[ds->port_no].scc_psmr);

    printf("tty->outpak=%x\n",idb->idb_tty->outpak);
    printf("BRG2=%x\n",ds->dprbase->regs.brg_brgc2);
    printf("BRG3=%x\n",ds->dprbase->regs.brg_brgc3);
    printf("BRG4=%x\n",ds->dprbase->regs.brg_brgc4);
    printf("[SICR] = %x\n",ds->dprbase->regs.si_sicr);
    printf("\nbuffer size %d\n", idb->max_buffer_size);
#endif
	
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
    quicc_async_show_errors(idb);
    
    return(FALSE);
}

/*
 * =================================================================
 * Name: quicc_async_portenable
 *
 * Description:
 *	This routine is use to enable the receiver and transmitter
 *
 * Parameters:
 *	tty_soc * - tty pointer
 *
 * Returns:
 *	void
 *
 * =================================================================
 */
static void quicc_async_portenable (tt_soc *tty)
{
    hwidbtype *idb = tty->tty_idb;
    quicc_instance_t *ds;
    quicc_dpr_t *dpr;
    
    QASSERT(idb);

    ds = INSTANCE;
    dpr = (quicc_dpr_t *)ds->dprbase;


    quicc_scc_enable_rx(dpr, ds->qinfo);
    quicc_scc_enable_tx(dpr, ds->qinfo);

    dtr_assert(TRUE);
}

/*
 * =================================================================
 * Name: quicc_async_portdisable
 *
 * Description:
 *	This routine is use to disable the receiver and transmitter
 *
 * Parameters:
 *	tty_soc * - tty pointer
 *
 * Returns:
 *	void
 *
 * =================================================================
 */
static void quicc_async_portdisable (tt_soc *tty)
{
    hwidbtype *idb = tty->tty_idb;
    quicc_instance_t *ds;
    quicc_dpr_t *dpr;
    
    QASSERT(idb);
    
    ds = INSTANCE;
    dpr = (quicc_dpr_t *)ds->dprbase;

    quicc_scc_disable_rx(dpr, ds->qinfo);
    quicc_scc_disable_tx(dpr, ds->qinfo);
    
}

/*
 * =================================================================
 *	Name:		quicc_show_async_state
 *
 *	Description:	This routine will display the serial state
 *
 *	CALLER:		
 *
 *	PREQ:		None
 *
 *	Returns:	Void
 * =================================================================
 */
void quicc_show_async_state (hwidbtype *idb, char *leader)
{
    c1000_show_serial_state(idb);
}

/*
 * =================================================================
 *	Name:		quicc_async_check_tx_errors
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
static void quicc_async_check_tx_errors (register hwidbtype *idb,  ushort status)
{
    quicc_async_instance_t           *ds;
    tt_soc *tty = idb->idb_tty;

    ds = (quicc_async_instance_t *)INSTANCE;
    
    /*
     * Since we just confirm the packet, that means one available
     * slot just got open so we should set outpak to NULL so that
     * the upper later knows that and allow data to be transmitted.
     * Please note that we don't need to free outpak because it is
     * stored in the ring packet structure and will be free by
     * quicc_scc_tx_intr().
     */
    tty->outpak = NULL;


    if (status & QUICC_ASYNC_TX_ERR_MASK) {
        if (status & QUICC_BDSTAT_TX_CTSLOS) {
	    ds->output_ctslosts++;
	    errmsg(&msgsym(CTSLOST, QUICC_ASYNC), idb->unit);
        }
        ds->fatal_tx_err = TRUE;
        idb->counters.output_error++;
    } /* end if-error */
}


/*
 * =================================================================
 *	Name:		quicc_async_check_rx_errors
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
static void quicc_async_check_rx_errors (hwidbtype *idb, ushort bd_status)
{
    tt_soc *tty = idb->idb_tty;
    
    QASSERT(idb);
    /*
     * We encountered an error while receiving this frame.
     */
    if (bd_status & QUICC_ASYNC_RX_ERR_MASK) {
        idb->counters.input_error++;
        if (bd_status & QUICC_BDSTAT_RX_FR) {
            idb->counters.input_frame++;
	    tty->linenoise++;
        }
        if (bd_status & QUICC_BDSTAT_RX_PR) {
            idb->counters.parity++;
        }
        if (bd_status & QUICC_BDSTAT_RX_CRCERR) {
            idb->counters.input_crc++;
        }
        if (bd_status & QUICC_BDSTAT_RX_OVRRUN) {
            idb->counters.input_overrun++;
	    tty->overrun++;
        }
    }
}

/*
 * =================================================================
 * Name: quicc_async_rx_tty_input
 *
 * Description:
 *	This routine is used process individual character.  It will
 * 	call the appropriate routine within special_chars() if the
 *	char is XON/XOFF or BREAK character.  This routine will also
 *	flow control if the static buffer (unlike CD2430 which
 *	allocates a ring of receive buffer buffer) is half full.
 *	Please notice that the character pass is from the receive
 *	buffer packet ring which we will free after copying all data
 *	to the "tty" structure.
 *
 * Parameters:
 *	hwidbtype * - hardware IDB
 *
 * Returns:
 *	void
 *
 * =================================================================
 */
inline static void 
quicc_async_rx_tty_input (tt_soc *tty, int chr)
{
    quicc_dpr_t *dpr;
    hwidbtype *idb = tty->tty_idb;
    quicc_instance_t *ds = INSTANCE;
    dpr = (quicc_dpr_t *)ds->dprbase;

    if (special_chars(tty, chr) >= 0) {
        if (tty->intail >= tty->inend)
            tty->intail = tty->instart; /* Handle wraps */
 
        /* If we have used half of the input buffer,
         * send an XOFF to try to stop the input.
         */
 
        if (tty->incount > (tty->insize >> 1)) {
            if (tty->intail == tty->inhead) {
                tty->overrun++;
                return;
            }
            if (tty->capabilities & SFLOWOUT)
                quicc_async_tty_xoff(tty);
 
            if (tty->capabilities & HFLOWOUT) {
		quicc_scc_clear_gsmr(dpr, ds->port_no, QUICC_GSMR_NONE, QUICC_SCC_GSMRH_RTSMD);
                tty->statbits |= STOPPEDINPUT;
            }
        }
        /* so there is room  */
	/* Currently when a 'BREAK' (0x100) character is passed in
	 * a '0x00' is passed up by other drivers such as CD2430
	 * we are doing this as well but only because of the side effect
	 * of the (uchar) cast.  This is sorta hacky.
	 */
        *tty->intail++ = (uchar) chr;
        ++tty->incount;
    }
}


/*
 * =================================================================
 * Name: quicc_async_misc_interrupt
 *
 * Description:
 *	This routine is called by the quicc_scc_misc_intr() when
 *	no other mask are matched (RX, TX and BSY but quicc_asm.S
 *	needs to be look at to determine this for each SCC since
 *	they can be different.
 *
 * Parameters:
 *	hwidbtype * - hardware IDB
 *
 * Returns:
 *	void
 *
 * =================================================================
 */
void quicc_async_misc_interrupt (hwidbtype *idb, ushort event)
{
    tt_soc *tty = idb->idb_tty;
    
    /*rxservice don't support BREAKS*/
    
    if (event & QUICC_SCCE_BRKS && !tty->rxservice && 
       !(event & QUICC_SCCE_RXBUF)) {
        /*
         * QUICC will close the buffer descriptor on receiving a
         * BREAK char, if there is any data in the buffer the
	 * quicc_async_RX_interrupt() will be called.  Otherwise,
         * if there is no data in the buffer, we will get a 
         * miscellaneous interrupt and service it here.
         */
	quicc_async_rx_tty_input(tty, BREAK_CHAR);
    }
}

/*
 * =================================================================
 * Name: quicc_async_throttle
 *
 * Description:
 *	This routine is called by the quicc_scc_rx_intr() when
 *	either the scheduler have not run for some time so we 
 *	should be nice and make sure we don't starve out other
 *	interfaces or process.  The other situation is if we can't
 *	find a replacement buffer for the packet that just came in.
 *	If we throttle and flow control is setup we should either
 *	sent a XOFF for software or deassert RTS if hardware flow
 *	control is use.  Please note we do not turn off the receiver
 *	since we could be running in character mode hence dropping
 *	character because the receiver is turn off is not desirable.
 *
 * Parameters:
 *	hwidbtype * - hardware IDB
 *
 * Returns:
 *	void
 *
 * =================================================================
 */
void quicc_async_throttle (hwidbtype *idb)
{
    tt_soc *tty = idb->idb_tty;
    quicc_instance_t *ds = INSTANCE;
    quicc_dpr_t *dpr = (quicc_dpr_t *)ds->dprbase;

    if ((tty->capabilities & SFLOWOUT) && !(tty->statbits & SENTXOFF)) {
        quicc_async_tty_xoff(tty);
    }

    /* XXX - Are these mutually exclusive? */
    if (tty->capabilities & HFLOWOUT) {
	quicc_scc_clear_gsmr(dpr, ds->port_no, QUICC_GSMR_NONE, QUICC_SCC_GSMRH_RTSMD);
        tty->statbits |= STOPPEDINPUT;
    }

    idb_mark_throttled(idb);
}

/*
 * =================================================================
 * Name: quicc_async_unthrottle
 *
 * Description:
 *	This routine is called by the net_background() code.  When
 *	the upper layer code notices that throttled has been marked.
 *	It will try to unthrottle the interface by calling
 *	"idb->enable".
 *
 * Parameters:
 *	hwidbtype * - hardware IDB
 *
 * Returns:
 *	void
 *
 * =================================================================
 */
void quicc_async_unthrottle (hwidbtype *idb)
{
    tt_soc *tty = idb->idb_tty;
    quicc_instance_t *ds = INSTANCE;
    quicc_dpr_t *dpr = (quicc_dpr_t *)ds->dprbase;

    if (tty->capabilities & HFLOWOUT) {
	quicc_scc_write_gsmr(dpr, ds->port_no, QUICC_GSMR_NONE, QUICC_SCC_GSMRH_RTSMD);
        tty->statbits &= ~STOPPEDINPUT;
    }
    if (tty->statbits & SENTXOFF)
        quicc_async_tty_xon(tty);

    idb_mark_unthrottled(idb);
    ds->enabled++;
}

/*
 * =================================================================
 * Name: quicc_async_RX_interrupt
 *
 * Description:
 *	This routine is use to service incoming data.  When data
 *	comes in the quicc gets an interrupt.  The assembly routine
 *	will determine what type of interrupt it is, once then calls
 *	the generic quicc_scc_rx_intr() if it is a RX interrupt.
 *	The generic quicc library routine, through the instance
 *	vector, this routine will get call.  If rxservice is defined
 *	then that means we are running in protocol mode.  Else
 *	process the individual packet.
 *
 * Parameters:
 *	hwidbtype *	- hardware IDB pointer
 *	paktype	*	- Packet received from the wire
 *
 * Returns:
 *	void
 *
 * =================================================================
 */
void quicc_async_RX_interrupt (hwidbtype *idb, paktype *pak)
{
    tt_soc *tty;
    ushort count;    
    quicc_instance_t *ds;
    uchar *data;

    tty = idb->idb_tty;
    ds = INSTANCE;
    count = pak->datagramsize;
    data =  pak->datagramstart;
    
    QUICC_WAN_RX_LED_ON(ds->dprbase);

    /* We should always receive characters, if we received only a
     * BREAK without data then we should not be in this code path,
     * but rather the quicc_async_misc_interrupt() handler.
     */
    QASSERT(count > 0);
    if (count > 0) {
        tty->totalin += count;	/* Account for data read */

        /* If there is a special receive service, pass data to it.
	 * We are running in protocol mode, either SLIP or PPP.
	 */
        if (tty->rxservice) {
            (*(tty->rxservice))(tty, data, count);
        } else {
            uchar c;
            int i;
            
            /* No special receive service, handle input data */
            for (i = 0; i < count; i++) {
                c = data[i];
                quicc_async_rx_tty_input(tty, c);
            }
	    /*
	     * QUICC will close the buffer descriptor on receiving a
	     * BREAK char, if there is any data in the buffer.  Otherwise,
	     * if there is no data in the buffer, we will get a
	     * miscellaneous interrupt.
	     */
	    if (ds->rx_bd_status & QUICC_BDSTAT_RX_BREAK) {
		quicc_async_rx_tty_input(tty, BREAK_CHAR);
	    }
        }
    }
    
    /*
     * Release the packet.
     */
    datagram_done(pak);
}


/*
 * =================================================================
 * Name: quicc_async_tty_init
 *
 * Description:
 *	This routine will initialize and create a tty and tie to
 *	it an IDB.  This routine is assume to be called only once.
 *
 * Parameters:
 *	hwidbtype *	- hardware IDB pointer
 *	paktype	*	- Packet received from the wire
 *
 * Returns:
 *	void
 *
 * =================================================================
 */
static void quicc_async_tty_init (int unit)
{
    tt_soc *tty;
    hwidbtype *idb;
    quicc_async_instance_t *ds;
    int rx_ring_size, tx_ring_size;
    

    tty = (tt_soc *)tty_init(unit, READY, TTY_LINE, &quicc_async_vector);
    tty->tty_databits = 8;
    tty->tty_parity = PARITY_NO;
    tty->tty_stopbits = STOPBITS2;
    tty->modem_type = MODEMTYPE_LOCAL; /* Default to local line */
    tty->modem_changes = (modem_status){0};
    
    /*
     * Initialize the packet memory which is dedicated statically
     * as DMA data buffers.  Flag that the TTY, initially, has no
     * input data.  insize is not really used (we use the pak size
     * instead) but tell the world the size our pool uses.
     */
    queue_init(&tty->txpakq, 0);


    /* Don't expect IDB to be set.  Later when we have the ability to
     * switch between SYNC and ASYNC then we might want to put check
     * into test that we don't create idb when tty->tty_idb is not set.
     */
    QASSERT(!tty->tty_idb);
    idb = init_async_idb(tty, TRUE);

    /*
     * Reset some fields in the async 'async' IDB so that
     * they come out right.
     */
    
    /*
     *  Need to create and initalize idb->type ot IDBTYPE_QUICC_ASYNC
     *  just like sync case.  Beware once we create this type we need to
     *  look for places that uses IDBTYPE_ASYNC and include IDBTYPE_QUIC_ASYNC.
     *  Reason we create this tag is because we need to do specific things for
     *  QUICC_ASYNC and this is the flag we will use.
     */
    
    idb->typestring = "QUICC Async";
    if (!idb->hw_namestring)
	idb_init_names(idb->firstsw,FALSE);

    idb->pool = NULL;
    idb->pool_group = POOL_GROUP_INVALID;
    idb->counters.resets = 0;
    idb->error_count = 0;
    idb->dte_interface = TRUE;
    idb->show_serial_state = quicc_show_async_state;
    idb->start_offset  = ENCAPBYTES - idb->encsize;
    
    idb->show_controller = quicc_async_show_controller;
    idb->enable = quicc_async_unthrottle;
  
    /*
     *  Need to create INSTANCE structure so we can point to
     *  the dual port ram.  And keep some protocol specific fields
     */
    /*
     * Allocate the driver data structure:
     */
    ds = malloc(sizeof(quicc_async_instance_t));
    if (!ds) {
	errmsg(&msgsym(NOMEMORY, QUICC), idb->unit, "driver structure");
	crashdump(0);

    }
    
    ds->qinfo = malloc(sizeof(quicc_info_t));
    
    if (!ds->qinfo) {
	errmsg(&msgsym(NOMEMORY, QUICC), idb->unit, "driver structure");
	crashdump(0);

    }


    INSTANCE =  ds;

    /* Hard code to use SCC4 for the time being.  Need to clean up
     * code later.
     */
    ds->qinfo->port_no = ds->port_no = 3;
    
    ds->dprbase          = (quicc_dpr_t *) getmbar();
    ds->parmbase         = (void *)&ds->dprbase->scc4.uart;
    
    quicc_scc_driver_setup(idb);
    ds->hw_throttle = quicc_async_throttle;
    

    select_ring_size(&rx_ring_size, &tx_ring_size, idb->max_buffer_size);
    
    ds->max_rx_ring_size = min(QUICC_RXBD_NUM_SCC4, rx_ring_size);
    ds->max_tx_ring_size = min(QUICC_TXBD_NUM_SCC4, tx_ring_size);
    
    /* Disable mask, this means if we use quicc_scc_rx_intr then
     * we would always assume packet came in Ok.  We need to then
     * determine if error occur in our receive routine.
     */
    ds->rx_bd_mask   	= QUICC_ASYNC_RX_ERR_MASK;
    ds->rx_bd_good_mask = 0;
    ds->tx_bd_mask   	= QUICC_ASYNC_TX_ERR_MASK;
    ds->tx_bd_good_mask = 0;

    /*
     * Setup which baud rate generator we will use.
     */
    ds->rxbrgc	= QUICC_SET_RX_BRGC(ds->dprbase);
    ds->txbrgc	= QUICC_SET_TX_BRGC(ds->dprbase);

	    
    /*
     * Add scce and sccm mask.  These mask will be use to enable
     * and disable interrupt as well as clear events within the
     * routines used to enable and disable ports. By default
     * for Async we will be configuring for UART mode.
     */
    ds->qinfo->rx_enable_sccm_mask   = QUICC_RX_UART_ENABLE_SCCM_MASK;
    ds->qinfo->tx_enable_sccm_mask   = QUICC_TX_UART_ENABLE_SCCM_MASK;
    
    ds->qinfo->rx_clear_scce_event   = QUICC_RX_UART_CLEAR_SCCE_EVENT;
    ds->qinfo->tx_clear_scce_event   = QUICC_TX_UART_CLEAR_SCCE_EVENT;

    ds->qinfo->rx_disable_sccm_mask  = QUICC_RX_UART_DISABLE_SCCM_MASK;
    ds->qinfo->tx_disable_sccm_mask  = QUICC_TX_UART_DISABLE_SCCM_MASK;
    
        
    /* The quicc library will invokes these vectors */
    ds->TX_errors        = quicc_async_check_tx_errors;
    ds->RX_errors        = quicc_async_check_rx_errors;
    ds->rx_handler       = quicc_async_RX_interrupt;
    ds->misc_handler	 = quicc_async_misc_interrupt;
    ds->quicc_async_tx_intr_ptr = quicc_async_tx_intr;
    
}

/*
 * =================================================================
 * Name: quicc_async_startinput
 *
 * Description:
 *	This routine will disable flow control of the remote end
 *	if flow control is enable and mark the interface as unthrottled.
 *
 * Parameters:
 *	tt_soc	*	- tty socket pointer
 *
 * Returns:
 *	void
 *
 * =================================================================
 */
static void quicc_async_startinput (tt_soc * tty)
{
    hwidbtype *idb = tty->tty_idb;
    quicc_instance_t *ds;
    leveltype SR;
    quicc_dpr_t *dpr;
    
    QASSERT(idb);
    
    ds = INSTANCE;
    dpr = (quicc_dpr_t *)ds->dprbase;

    SR = raise_interrupt_level(NETS_DISABLE);
    /* Make sure this routine will:
     * 1. Send XON
     * 2. Turn on interrupt for RX, Modem signals.  The 2430 programs something
     * called RET for partial fifo received because FIFO is 16 bytes but
     * we will be changing our FIFO size to 1. so we should not need
     * to do this.
     */

    if (tty->statbits & SENTXOFF)
        quicc_async_tty_xon(tty);

    /*  This routine according to 2430 only enable RX interrupt
     *  and does not want to enable the SCC channel.  Enabling the
     *  SCC channel is done in portenable
     */
    quicc_scc_write_gsmr(dpr, ds->port_no, QUICC_GSMR_NONE, QUICC_SCC_GSMRH_RTSMD);
    tty->statbits &= ~STOPPEDINPUT;
    
    idb_mark_unthrottled(idb);
        
    reset_interrupt_level(SR);
}

/*
 * =================================================================
 * Name: get_speed_divsor
 *
 * Description:
 *	This routine will return take the speed and return back
 *	the divsor to set the baud rate generator configuration
 *	register.
 *
 * Parameters:
 *	int	- baud rate speed
 *	int *	- pointer to divsor to be return.
 *
 * Returns:
 *	TRUE 	- valid speed
 *	FALSE 	- invalid speed
 *
 * =================================================================
 */
static boolean get_speed_divsor (int speed, int *divsor)
{
    if (speed > MAX_ASYNC_SPEED || speed < MIN_ASYNC_SPEED)
	return FALSE;

    switch(speed) {
    case 50:
	*divsor = QUICC_BRGC_CLK_DIV(QUICC_BRGC_CLK_DIV_50) | \
	    QUICC_BRGC_DIV_BY_16;
	break;
    case 75:
	*divsor = QUICC_BRGC_CLK_DIV(QUICC_BRGC_CLK_DIV_75) | \
	    QUICC_BRGC_DIV_BY_16;
	break;
    case 150:
	*divsor = QUICC_BRGC_CLK_DIV(QUICC_BRGC_CLK_DIV_150) | QUICC_BRGC_DIV_BY_16;
	break;
    case 300:
	*divsor = QUICC_BRGC_CLK_DIV(QUICC_BRGC_CLK_DIV_300) | QUICC_BRGC_DIV_BY_16;
	break;
    case 600:
	*divsor = QUICC_BRGC_CLK_DIV(QUICC_BRGC_CLK_DIV_600);
	break;
    case 1200:
	*divsor = QUICC_BRGC_CLK_DIV(QUICC_BRGC_CLK_DIV_1200);
	break;
    case 2400:
	*divsor = QUICC_BRGC_CLK_DIV(QUICC_BRGC_CLK_DIV_2400);
	break;
    case 4800:
	*divsor = QUICC_BRGC_CLK_DIV(QUICC_BRGC_CLK_DIV_4800);
	break;
    case 9600:
	*divsor = QUICC_BRGC_CLK_DIV(QUICC_BRGC_CLK_DIV_9600);
	break;
    case 19200:
	*divsor = QUICC_BRGC_CLK_DIV(QUICC_BRGC_CLK_DIV_19200);
	break;
    case 38400:
	*divsor = QUICC_BRGC_CLK_DIV(QUICC_BRGC_CLK_DIV_38400);
	break;
    case 57600:
	*divsor = QUICC_BRGC_CLK_DIV(QUICC_BRGC_CLK_DIV_57600);
	break;
    case 115200:
	*divsor = QUICC_BRGC_CLK_DIV(QUICC_BRGC_CLK_DIV_115200);
	break;
    default:
	return FALSE;
    }
            
    return TRUE;
}

/*
 * =================================================================
 * Name: quicc_setbrgc
 *
 * Description:
 *	set the baud rate generator configuration register
 *
 * Parameters:
 *	ulong *	- pointer to baud rate generator register
 *	uint 	- clock divsor
 *
 * Returns:
 *	void
 *
 * =================================================================
 */
void quicc_setbrgc (volatile ulong *brgc, uint divsor)
{
    *brgc |= QUICC_BRGC_RESET;
    *brgc |= divsor;
    *brgc |= QUICC_BRGC_EN_CLK;
    *brgc &= ~(QUICC_BRGC_RESET);
}

/*
 * =================================================================
 * Name: quicc_async_setspeed
 *
 * Description:
 *	The routine will take the rxspeed and txspeed as input
 *	and figure out the divsor to set the baud rate generator
 *	register via quicc_set_brgc().
 *
 * Parameters:
 *	tt_soc *	- tty socket pointer
 *	txspeed		- transmit baud rate speed
 *	rxspeed		- receive baud rate speed
 *
 * Returns:
 *	TRUE 	- valid baud rate speed
 *	FALSE	- invalid baud rate speed
 *
 * =================================================================
 */
static boolean quicc_async_setspeed (tt_soc *tty, int txspeed, int rxspeed)
{
    hwidbtype *idb = tty->tty_idb;
    uint rx_divsor, tx_divsor;
    leveltype SR;
    quicc_async_instance_t *ds;
        
    QASSERT(idb);

    ds = INSTANCE;
    
    if (!get_speed_divsor(rxspeed, &rx_divsor))
	return FALSE;

    if (!get_speed_divsor(txspeed, &tx_divsor))
	return FALSE;

    SR = raise_interrupt_level(NETS_DISABLE);

    quicc_setbrgc(ds->rxbrgc, rx_divsor);

    tty->tty_rxspeed = rxspeed;
    
    quicc_setbrgc(ds->txbrgc, tx_divsor);
    
    tty->tty_txspeed = txspeed;

    reset_interrupt_level(SR);
    
    return TRUE;
}

#ifdef NOTSUPPORTED
static void quicc_async_autosetspeed(tt_soc *tty)
{
    /* This should call the setspeed vector from in ttysrv.h */
    (void)setspeed(tty, AUTOSPEEDHIGH, AUTOSPEEDHIGH);
}
#endif

/*
 * =================================================================
 * Name: quicc_async_stopbits
 *
 * Description:
 *	This routine will set the stop bit value for the QUICC and
 *	the tty	structure.
 *
 * Parameters:
 *	tt_soc *	- tty socket pointer
 *	int		- stop bit value
 *
 * Returns:
 *	void
 *
 * =================================================================
 */
static void quicc_async_stopbits (tt_soc *tty, int stop_bits)
{
    hwidbtype *idb = tty->tty_idb;
    quicc_instance_t *ds;
    quicc_dpr_t *dpr;
    
    QASSERT(idb);

    ds = INSTANCE;
    
    dpr = (quicc_dpr_t *)ds->dprbase;

    /* Assuming stop_bits are checked to be 1 or 2 bits */
    quicc_scc_clear_psmr(dpr, ds->port_no, QUICC_UART_STOPBITS_MASK);
    
    quicc_scc_write_psmr(dpr, ds->port_no, QUICC_UART_STOPBITS(stop_bits));
    tty->tty_stopbits = stop_bits;
}

/*
 * =================================================================
 * Name: quicc_async_sendbreak
 *
 * Description:
 *	This routine will instruct the QUICC to send break
 *	characters.
 *
 * Parameters:
 *	tt_soc *	- tty socket pointer
 *
 * Returns:
 *	void
 *
 * =================================================================
 */
static void quicc_async_sendbreak (tt_soc *tty)
{ 
    hwidbtype *idb = tty->tty_idb;
    quicc_instance_t *ds;
    quicc_dpr_t *dpr;
    
    QASSERT(idb);

    ds = INSTANCE;
    
    dpr = (quicc_dpr_t *)ds->dprbase;

    /* QUICC in UART mode: to send break, issue STOP_TRANSMIT command.
     * The number of break character has have been set elsewhere in the
     * UART specific parameters, BRKCR.
     */
    quicc_send_risc_cmd(dpr, ds->port_no, QUICC_CPC_TX_STOP);

    /* now issue TX_RESTART so that the QUICC will resume transmission 
     * automatically by the QUICC after the BRK has been sent (yes, it
     * really works).
     */
    quicc_send_risc_cmd(dpr, ds->port_no, QUICC_CPC_TX_RESTART);
    
}

/*
 * =================================================================
 * Name: quicc_async_paritybits
 *
 * Description:
 *	This routine will set the parity bit value for both the
 *	transmit and receive of the QUICC and the tty structure.
 *
 * Parameters:
 *	tt_soc *	- tty socket pointer
 *	int		- parity type
 *
 * Returns:
 *	void
 *
 * =================================================================
 */
static void quicc_async_paritybits (tt_soc *tty, int parity_type)
{
    hwidbtype *idb = tty->tty_idb;
    quicc_instance_t *ds;
    int parity_val;
    quicc_dpr_t *dpr;
    
    QASSERT(idb);
    
    ds = INSTANCE;
    
    dpr = (quicc_dpr_t *)ds->dprbase;
    
    tty->tty_parity = parity_type;
    
    switch(parity_type) {
    case PARITY_NO:
	quicc_scc_clear_psmr(dpr, ds->port_no, QUICC_UART_ENABLE_PARITY);
	return;
    case PARITY_ODD:
	parity_val = QUICC_ODD_PARITY;
	break;
    case PARITY_EVEN:
	parity_val = QUICC_EVEN_PARITY;
	break;
    case PARITY_SPACE:
	parity_val = QUICC_SPACE_PARITY;
	break;
    case PARITY_MARK:
	parity_val = QUICC_MARK_PARITY;
	break;
    default:
	return;
    }
    quicc_scc_clear_psmr(dpr, ds->port_no, QUICC_UART_PARITY_MASK);
    quicc_scc_write_psmr(dpr, ds->port_no, QUICC_UART_TX_PARITY(parity_val));
    quicc_scc_write_psmr(dpr, ds->port_no, QUICC_UART_RX_PARITY(parity_val));
    tty->tty_parity = parity_val;
}


/*
 * =================================================================
 * Name: quicc_async_stopbits
 *
 * Description:
 *	This routine will set the data bit length value for the
 *	QUICC and the tty structure.
 *
 * Parameters:
 *	tt_soc *	- tty socket pointer
 *	int		- stop bit value
 *
 * Returns:
 *	void
 *
 * =================================================================
 */
static void quicc_async_databits (tt_soc *tty, int bits)
{
    hwidbtype *idb = tty->tty_idb;
    quicc_instance_t *ds;
    quicc_dpr_t *dpr;
    
    QASSERT(idb);
    
    ds = INSTANCE;
    
    dpr = (quicc_dpr_t *)ds->dprbase;
    
    if (bits < QUICC_UART_MIN_DATA_LEN || bits > QUICC_UART_MAX_DATA_LEN) {
	bits = QUICC_UART_MAX_DATA_LEN;
    }

    tty->tty_databits = bits;

    quicc_scc_clear_psmr(dpr, ds->port_no, QUICC_UART_CHARLEN_MASK);
    quicc_scc_write_psmr(dpr, ds->port_no, QUICC_UART_DATABIT_LEN(bits));
    
}

#ifdef NOTSUPPORTED
/*
 *  This routine is actually for setting the RX clock.
 */
static void quicc_async_autobaud (tt_soc *tty)
{
    hwidbtype *idb = tty->tty_idb;
    quicc_async_instance_t *ds;
    
    QASSERT(idb);

    ds = INSTANCE;
    
    /*
     *  Set the ATB bit, make sure the baud rate generator 4 is
     *  on since we are using BRG4 for SCC4 RX.  Data book requires
     *  that we receive 3 RX clocks before we can do anything.
     */
    
    /* If the BRG is enable and ATB is not on then
     * let's set it up for ATB, when the QUICC is finish
     * with determining the Baud rate then we should
     * get an interrupt.  Need to writer interrupt handler for
     * the ATB event.  The handler should call setspeed() to
     * let the system know at what RX baud rate we are running.
     */
    *ds->rxbrgc |= QUICC_BRGC_AUTO_BAUD;

}

#endif

/*
 * =================================================================
 * Name: quicc_async_clearinput
 *
 * Description:
 *	This routine will clear the receiver by resetting the buffer
 *	descriptor ring and static buffer use by the tty input
 *	pointers.
 *
 * Parameters:
 *	tt_soc * - tty socket pointer
 *
 * Returns:
 *	void
 *
 * =================================================================
 */
static void
quicc_async_clearinput (tt_soc *tty)
{
    leveltype SR;
    hwidbtype *idb = tty->tty_idb;
    quicc_instance_t *ds = INSTANCE;
    quicc_dpr_t *dpr = (quicc_dpr_t *)ds->dprbase;
    ulong port_no = ds->port_no;
    
    /*
     * Hold off input interrupts
     */
    SR = raise_interrupt_level(NETS_DISABLE);

    /*
     * Disable receiver, if it isn't already
     */
    quicc_scc_disable_rx(dpr, ds->qinfo);
    
    /* by providing TRUE, it will reset all rx buffers, we may
     * need to do this if change maxmtu will cause this routine
     * to get run
     */
    quicc_scc_bd_rx_ring_init(tty->tty_idb, FALSE);

    /*
     * Reenable receiver
     */

    quicc_send_risc_cmd(dpr, port_no, QUICC_CPC_INIT_RX);

    quicc_scc_enable_rx(dpr, ds->qinfo);

    /* We need to reset the tty->inhead and tty->incount and all
     * those other good stuff.
     */
    serial_clearinput(tty);
    reset_interrupt_level(SR);

}

/*
 * =================================================================
 * Name: quicc_async_clearoutput
 *
 * Description:
 *	This routine will clear the transmitter by resetting
 *	the buffer descriptor ring and any queue up packets from
 *	the transmit routine.
 * 	
 *
 * Parameters:
 *	tt_soc * - tty socket pointer
 *
 * Returns:
 *	void
 *
 * =================================================================
 */
static void
quicc_async_clearoutput (tt_soc *tty)
{
    leveltype SR;
    hwidbtype *idb = tty->tty_idb;
    quicc_instance_t *ds = INSTANCE;
    quicc_dpr_t *dpr = (quicc_dpr_t *)ds->dprbase;
    ulong port_no = ds->port_no;
    
    /*
     * Hold off input interrupts
     */
    SR = raise_interrupt_level(NETS_DISABLE);

    /* Disable receiver, if it isn't already */
    quicc_scc_disable_tx(dpr, ds->qinfo);
    
    tty->outpak = NULL;

    /* Free off any output buffers allocated */
    while (tty->txpakq.qhead) 
	datagram_done(dequeue(&tty->txpakq));

    quicc_scc_bd_tx_ring_init(tty->tty_idb);

    /*
     *  Allocate a buffer and set the tty->out* fields.
     */
    quicc_async_alloc_txbuf(tty);
    
    /*
     * Reenable transmitter
     */
    quicc_send_risc_cmd(dpr, port_no, QUICC_CPC_INIT_TX);
    quicc_scc_enable_tx(dpr, ds->qinfo);
    
    
    /*
     * Allow interrupts
     */
    reset_interrupt_level(SR);
}

/*
 * =================================================================
 * Name: quicc_async_dtrstate
 *
 * Description:
 *	This routine will set the data modem signal.
 *
 * Parameters:
 *	tt_soc *	- tty socket pointer
 *	boolean		- dtr signal
 *
 * Returns:
 *	void
 *
 * =================================================================
 */
static void quicc_async_dtrstate (tt_soc *tty, boolean sense)
{

    /* Please note that we are assert/deasserting DTR while
     * in the 2430, they assert/deassert RTS.  The reason
     * the 2430 uses RTS rather then DTR is because the
     * Cirrus chip will deassert DTR when their internal
     * FIFO gets full rather then deassert RTS the correct
     * way.  As a work around for this chip the cd2430 driver
     * wired DTR to RTS and have RTS to DTR.
     */
    dtr_assert(sense);
}

/*
 * =================================================================
 * Name: quicc_async_modemsignal
 *
 * Description:
 *	This routine will return the data modem signal.
 *
 * Parameters:
 *	tt_soc *	- tty socket pointer
 *	boolean		- dtr signal
 *
 * Returns:
 *	void
 *
 * =================================================================
 */
static modem_status quicc_async_modemsignal (tt_soc *tty)
{
    modem_status summary;

    summary.ring = dcd_on();
    summary.cts  = cts_on();
    summary.dtr  = dtr_on();
    summary.rts  = rts_on();

    return(summary);
}

/*
 * =================================================================
 * Name: quicc_async_modemchange
 *
 * Description:
 *	This routine will return the modem signal values and clear
 *	the tty modem signal structures.
 *
 * Parameters:
 *	tt_soc *	- tty socket pointer
 *	boolean		- dtr signal
 *
 * Returns:
 *	void
 *
 * =================================================================
 */
static modem_status quicc_async_modemchange (tt_soc *tty)
{
    modem_status summary;
    leveltype SR;

    SR = raise_interrupt_level(NETS_DISABLE);
    summary = tty->modem_changes;
    tty->modem_changes = (modem_status) {0};
    reset_interrupt_level(SR);

    return (summary);
}

/*
 * =================================================================
 * Name: quicc_async_setflow
 *
 * Description:
 *	This routine is use to set the QUICC to support hardware
 *	flow control.
 *
 * Parameters:
 *	tt_soc *	- tty socket pointer
 *
 * Returns:
 *	void
 *
 * =================================================================
 */
static void quicc_async_setflow (tt_soc *tty)
{
    hwidbtype *idb = tty->tty_idb;
    quicc_instance_t *ds = INSTANCE;
    quicc_dpr_t *dpr = (quicc_dpr_t *)ds->dprbase;

    if (tty->capabilities & HFLOWIN) {
        /*
         * Stop transmitting when CTS is negated, and continue
         * transmitting when CTS is asserted.
         */
	quicc_scc_write_psmr(dpr, ds->port_no, QUICC_PSMR_FLC);

	/* Make QUICC honor hardware flow control and stop
	 * transmitting when we loose CTS
	 */
	quicc_scc_set_signal(dpr, QUICC_UART_CTS_FLOW(ds->port_no));

    }
    else {
        quicc_scc_clear_psmr(dpr, ds->port_no, QUICC_PSMR_FLC);

	/* QUICC will no longer worry about CTS since it will
	 * always be internally asserted.
	 */
	quicc_scc_clear_signal(dpr, QUICC_UART_CTS_FLOW(ds->port_no));
    }
}


/*
 * =================================================================
 * Name: quicc_async_vector_init
 *
 * Description:
 *	Initalize all the call back vectors used by the commserver
 *	async code.
 *
 * Parameters:
 *	void
 *
 * Returns:
 *	void
 *
 * =================================================================
 */
static void quicc_async_vector_init (void)
{
    
    /*
     * Wire ourselves in to our interrupt vectors
     */
    vector_init(&quicc_async_vector);

    /*
     * Input processing
     */
    quicc_async_vector.vgetc = serial_getc;
    quicc_async_vector.vinputpending = serial_inputpending;
    quicc_async_vector.vstartinput = quicc_async_startinput;
    quicc_async_vector.vclearinput = quicc_async_clearinput;

    /*
     * Output processing
     */
    quicc_async_vector.vputc = serial_putc;
    quicc_async_vector.voutputpending = serial_outputpending;
    quicc_async_vector.voutputblock = serial_outputblock;
    quicc_async_vector.voutputxoffed = serial_outputxoffed;

    quicc_async_vector.vstartoutput = quicc_async_startoutput;
    quicc_async_vector.vclearoutput = quicc_async_clearoutput;

    quicc_async_vector.vputpkt = quicc_async_putpkt;
    quicc_async_vector.vstopoutput = quicc_async_stopoutput;


    /*
     * Special functions
     */
    quicc_async_vector.vsetspeed = quicc_async_setspeed;
    
#ifdef NOTSUPPORTED
    quicc_async_vector.vautosetspeed = quicc_async_autosetspeed;
    quicc_async_vector.vauto_baud = quicc_async_autobaud;
#endif
    
    quicc_async_vector.vservicehook = generic_servicehook;
    quicc_async_vector.vnoservicehook = generic_noservicehook;

    quicc_async_vector.vparitybits = quicc_async_paritybits;
    quicc_async_vector.vdatabits = quicc_async_databits;
    quicc_async_vector.vstopbits = quicc_async_stopbits;
    quicc_async_vector.vsendbreak = quicc_async_sendbreak;
        
    quicc_async_vector.vportenable = quicc_async_portenable;
    quicc_async_vector.vportdisable = quicc_async_portdisable;

    quicc_async_vector.vdtrstate = quicc_async_dtrstate;
    quicc_async_vector.vmodemsignal = quicc_async_modemsignal;
    quicc_async_vector.vmodemchange = quicc_async_modemchange;
    
    quicc_async_vector.vsetflow = quicc_async_setflow;
}


/*
 * =================================================================
 * Name: quicc_async_mode_reg_init
 *
 * Description:
 *	This routine will initalize parallel port pin registers,
 *	initalize SCC specific parameters, ring buffer and assert
 *	DTR.
 *
 * Parameters:
 *	tt_soc * - tty socket pointer
 *
 * Returns:
 *	void
 *
 * =================================================================
 */
static void
quicc_async_mode_reg_init (tt_soc *tty)
{
    hwidbtype *idb = tty->tty_idb;
    quicc_instance_t *ds = INSTANCE;
    quicc_dpr_t *dpr = (quicc_dpr_t *)ds->dprbase;
    ulong port_no = ds->port_no;
    quicc_uart_param_t *parmptr = ds->parmbase;

    
    /* Channel is assumed to be disabled before example starts, so
     * we'll make sure the channel is disabled.
     */
    quicc_async_portdisable(tty);
  
    /* Initalize uart specific parameter ram */
    quicc_uart_param_init(dpr, ds->port_no, 
			tty->tty_idb->max_buffer_size+tty->tty_idb->encsize);

    /*
     *  Let's set the clock so that SCC4 uses BRG4 for RX and BRG2 for TX.
     */

    /* First clear any existing clocking options
     */
    ds->dprbase->regs.si_sicr &=
	~(QUICC_SICR_RXCS(port_no, QUICC_SCC_CS_MASK) |
          QUICC_SICR_TXCS(port_no, QUICC_SCC_CS_MASK));

    /* Then set our desired clocking options
     */
    ds->dprbase->regs.si_sicr |=
	(QUICC_SICR_RXCS(port_no, QUICC_SCC_TO_BRG4) |
	 QUICC_SICR_TXCS(port_no, QUICC_SCC_TO_BRG2));

    /* Set BRKCR so a break character will be sent when a
     * STOP TRANSMIT command is issued.
     *
     * Arm break duration--we want 1/4 second of break, and assume
     * here that a character time is 10 bit times.
     *
     * Calculation:  Bits per second divided by bits per char =
     *   chars per second.  Divide result by four to get 1/4 second
     *   of break (but at least one).
     *
     */
    parmptr->brkcr = ((tty->tty_txspeed / 10) / 4) + 1;

    /*
     * Determine if the memory pool needs adjustment or allocation.
     */
    memory_pool_adjustment(tty->tty_idb, TRUE);
    
    /* set up buffering, please note that we don't use a static
     * buffer for output, hence the value 0.  
     */        
    serial_setbuffers(tty,MAXINCHARS, 0);

    quicc_scc_bd_ring_init(tty->tty_idb, TRUE);

    
    /*
     * In high 32 bytes:  Set small receive FIFO length;
     * In low 32 bytes:  Configure 16x sampling for transmit & receive.
     *    Set UART mode.
     */
    quicc_scc_write_gsmr(dpr, ds->port_no,
			 (QUICC_SCC_GSMRL_TDCR(QUICC_TX_DIV_CLK_16X) |
			  QUICC_SCC_GSMRL_RDCR(QUICC_RX_DIV_CLK_16X) |
			  QUICC_SCC_GSMRL_MODE(QUICC_SCC_UART_MODE)),
			 (QUICC_SCC_GSMRH_RFLEN|
			  QUICC_SCC_GSMRH_TFLEN|
			  QUICC_SCC_GSMRH_RTSMD));
    
			 
    if (tty->capabilities & HFLOWIN) 
	quicc_scc_write_psmr(dpr, ds->port_no, QUICC_PSMR_FLC);
        
    cpai_set_sync(FALSE);
    cpai_set_txc_invert(TRUE);


    /* Need to be careful on some of these functions since it may
     * want to start the RX and DX after it sets the value.
     */
    quicc_async_databits(tty, tty->tty_databits);
    quicc_async_paritybits(tty, tty->tty_parity);
    quicc_async_stopbits(tty, tty->tty_stopbits);
    
    quicc_async_setspeed(tty, ASYNC_DEFSPEED, ASYNC_DEFSPEED);
    
    quicc_scc_start(dpr, ds->qinfo, TRUE, TRUE);
    dtr_assert(TRUE);
}


/*
 * =================================================================
 * Name: quicc_async_modem_interrupt_handler
 *
 * Description:
 *	This routine is use to handle modem signal changes
 *
 * Parameters:
 *	hwidbtype 	- pointer to hwidb for this interface
 *
 * Returns:
 *	void - None.
 * =================================================================
 */
void quicc_async_modem_interrupt_handler (hwidbtype *idb)
{
    tt_soc *tty = idb->idb_tty;
    
    /* Need to change this code if DCD is active low */
    if (cable_present()) {
	/* Please note that we are using DCD as the ring
	 * indicator.  For the 2430, the driver uses DSR
	 * while in reality they use DCD.  The DCD
	 * signal is routed directly into the DSR for the
	 * Copan platform.
	 */
	if (dcd_on())
	    tty->modem_changes.ring = TRUE;
	if (cts_on())
	    tty->modem_changes.cts = TRUE;
	return;
    }

}

/*
 * =================================================================
 * Name: quicc_async_scc_init
 *
 * Description:
 *	This routine will initalize SCC specific parameters
 *
 * Parameters:
 *	subsystype * - pointer to subsystem
 *
 * Returns:
 *	void
 *
 * =================================================================
 */
static void
quicc_async_scc_init (hwidbtype *idb)
{
    quicc_async_instance_t *ds;
    quicc_dpr_t *dpr;
    ulong port_no;
    leveltype SR;
    
    QASSERT(idb);
    ds = INSTANCE;
    dpr = ds->dprbase;
    port_no = ds->port_no;
    
    SR = raise_interrupt_level(NETS_DISABLE);

    /* Setup parallel port pins, the values assign by this routine
     * is platform specific.
     */
    quicc_set_async_serial_pins(dpr);
    
    cpai_init();

    /* Setup modem interrupt handler for DCD signal so that
     * when interrupt gets enable it will have an interrupt
     * handler to service it
     */
    quicc_cpic_handler_install(idb,
                               ds->dprbase,
                               DCD_INTR_INDEX,
                               QUICC_CPIC_INTERRUPT_PC11,
                               QUICC_CIMR_DCD_MASK,
                               quicc_async_modem_interrupt_handler,
                               quicc_serial_dcd_interrupt);
    
    /*
     * Initialize the quicc SCC
     * - program the GSMR (except ENT and ENR bits)
     * - program the PSMR
     * - program the DSR
     * - initialize the RX and TX BD rings
     * - initialize common parameter RAM
     * - initialize uart parameter RAM
     * - clear SCCE events
     * - initialize CICR, CIPR, CIMR
     */
    
    /*
     * Initialize GSMR, PSMR, DSR
     */
    quicc_scc_reg_init(dpr, port_no);

    ds->fatal_tx_err = FALSE;
    
    quicc_async_mode_reg_init(idb->idb_tty);

    /* Enable modem interrupt for DCD signal */
    quicc_modem_interrupt_mask(idb, ENABLE, QUICC_CIMR_DCD_MASK);
    reset_interrupt_level(SR);

}

/*
 * =================================================================
 * Name: quicc_print_modemcontrol
 *
 * Description:
 *	This routine is use to display the modem signals.  Please
 *	note that we use DCD as the ring indicator unlike the CD2430.
 *	In reality, the CD2430 is also looking at DCD but they wire
 *	up DCD to DSR within the logic of the Cirrus chip.  Also
 *	notice that we output RTS for RTS and DTR for DTR unlike the
 *	CD2430 which swaps the two values.  The reason is because
 *	the Cirrus chip will deassert DTR rather then RTS when their
 *	FIFO gets full.
 *
 * Parameters:
 *	tt_soc * - tty socket pointer
 *
 * Returns:
 *	void
 *
 * =================================================================
 */
static void
quicc_print_modemcontrol (tt_soc *tty)
{
    printf("\nModem hardware state: %sCTS%s%sDSR%s %sDTR %sRTS",
	   cts_on() ? "":"no", tty->modem_changes.cts ? "* ":" ",
	   dcd_on() ? "":"no", tty->modem_changes.ring ? "* ":" ",
	   dtr_on() ? "":"no",
	   rts_on() ? "":"no");
}

/*
 * =================================================================
 * Name: quicc_async_init
 *
 * Description:
 *	This routine will initialize all tty lines and its
 *	corresponding IDB.
 *
 * Parameters:
 *	void
 *
 * Returns:
 *	void
 *
 * =================================================================
 */
static void quicc_async_init (void)
{
    quicc_instance_t *ds;
    hwidbtype *idb;
    tt_soc *tty;
    int tty_line;
    
    /* Initialize and setup all vectors.  This should be done only
     * during initialization.
     */ 
    quicc_async_vector_init();
    
    /* Initialize the scc channel.  This should be done only
     * during initialization.  Console is always 0.
     */
    for (tty_line=quiccAsyncBase; tty_line < nttylines+1; tty_line++) {
	quicc_async_tty_init(tty_line);
	tty = MODEMS[tty_line];
	idb = tty->tty_idb;
	ds = INSTANCE;

	/* Done in here because we should only need to do it once.
	 * this will install the interrupt handlers
	 */
	quicc_scc_handler_install(idb, ds->port_no);

	/* Setup QUICC chip specific parameters */
	quicc_async_scc_init(idb);
    }
    async_show_controller_parser_init();
}

/*
 * =================================================================
 * Name: quicc_async_subsys_init
 *
 * Description:
 *	This routine will setup registry calls
 *
 * Parameters:
 *	subsystype * - pointer to subsystem
 *
 * Returns:
 *	void
 *
 * =================================================================
 */
static void quicc_async_subsys_init (subsystype *subsys)
{
    quiccAsyncBase = freeLineBase;	/* probably 1 */
    nttylines += 1;			/* Add this instance of tty line */
    freeLineBase += 1;


    reg_add_tty_show(quicc_print_modemcontrol, "quicc_print_modemcontrol");

    /* quicc_async_init() is the function which will do core
     * initialization.
     */
    reg_add_line_init(quicc_async_init, "quicc_async_init");
    
    reg_add_tty_xon(quicc_async_tty_xon, "quicc_async_tty_xon");

    /* Add generic fast switch registry */
    reg_add_init_async_fs(init_async_fs, "init_async_fs");
    reg_add_setup_global2(async_setup_global, "async_setup_global");

    reg_add_if_onesec(quicc_serial_onesec_periodic,
                      "quicc_serial_onesec_periodic");

}

/*
 * QUICC async subsystem header
 */
#define QUICC_ASYNC_MAJVERSION   1
#define QUICC_ASYNC_MINVERSION   0
#define QUICC_ASYNC_EDITVERSION  1

SUBSYS_HEADER(quicc_async, QUICC_ASYNC_MAJVERSION, 
              QUICC_ASYNC_MINVERSION, QUICC_ASYNC_EDITVERSION,
	      quicc_async_subsys_init, SUBSYS_CLASS_DRIVER,
	      NULL, NULL);




