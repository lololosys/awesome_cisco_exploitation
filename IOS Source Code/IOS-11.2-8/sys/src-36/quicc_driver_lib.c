/* $Id: quicc_driver_lib.c,v 3.3.60.3 1996/08/28 13:10:31 thille Exp $
 * $Source: /release/112/cvs/Xsys/src-36/quicc_driver_lib.c,v $
 *------------------------------------------------------------------
 * quicc_driver_lib.c --  QUICC library interface to the cisco code.
 *
 * November 1994, Andrew G. Harvey
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: quicc_driver_lib.c,v $
 * Revision 3.3.60.3  1996/08/28  13:10:31  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.3.60.2  1996/06/12  19:56:44  jng
 * CSCdi60260:  Async catwoman commit
 * Branch: California_branch
 * Async Catwoman commit into California
 *
 * Revision 3.3.60.1  1996/03/18  21:56:38  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.24.2  1996/03/13  02:00:08  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.3.24.1  1996/01/24  22:49:13  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.3  1995/11/17  18:39:25  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:29:19  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:19:17  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.9  1995/11/08  21:26:25  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.8  1995/08/24  23:10:09  henniger
 * CSCdi38744:  BRI Fair-queueing on c1000 slows performance & breaks
 * compression
 * Increased buf-desc tx ringsize to '2' for fair-q-ing.
 *
 * Revision 2.7  1995/08/09  19:32:48  sgannu
 * CSCdi38309:  catwoman msg causes modular build failure
 *
 * Revision 2.6  1995/08/08  19:47:54  fred
 * CSCdi38414:  congestion of compressed interfaces reduces speed.
 * when compressing data AND using WFQ/PQ/CQ, use a ring size of two, not
 * one.
 *
 * Revision 2.5  1995/08/07  05:41:22  richl
 * CSCdi38315:  makefile rework
 *
 * Revision 2.4  1995/07/25  21:48:59  abaindur
 * CSCdi37628:  LOSTCARR msgs too frequent when c1000 ethernet cable
 * unplugged
 * Rate limit message. Also fix handling of line protocol state during
 * carrier loss, and counting of deferred transmits. Remove a bogus
 * check for idb->status in ethernet_enable.
 *
 * Revision 2.3  1995/06/23  16:39:56  fred
 * CSCdi36295:  Commit Queue MIB to 11.0
 *
 * Revision 2.2  1995/06/20  07:18:10  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.1  1995/06/07  22:51:59  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "interface_private.h"
#include "packet.h"
#include "logger.h"
#include "../src-36/msg_quicc.c"	/* Not a typo, see logger.h */
#include "../if/msg_datalink.c"		/* Not a typo, see logger.h */

#include "../os/pool.h"
#include "../les/if_les.h"
#include "../les/les_driver.h"
#include "../les/les_driver_inline.h"
#include "../if/network.h"

#include "../src-36/quicc_driver_lib.h"

/*
 * =================================================================
 * Name: quicc_scc_bd_tx_ring_init
 *
 * Description:
 *	Initalize the transmit buffer descriptor ring buffers.
 *
 * Parameters:
 *	hwidbtype * - hwidb for this interface.
 *	boolean  - TRUE mtu changed.
 *
 * Returns:
 *	void
 *
 * Note: Unlike sync case the async driver will initialize the
 *       input and output independently.
 *
 * =================================================================
 */
void
quicc_scc_bd_tx_ring_init (hwidbtype *idb)
{
    quicc_instance_t *ds;
    quicc_bd_t *bd;
    quicc_bd_t *last;
    paktype **pak;

    ds = INSTANCE;
    
    /*
     * Initalize the transmit buffers.
     * (1000 performance hindered if queue < 2.)
     */
    if (hwidb_use_short_tx_q_inline(idb))
        ds->tx_size = make_ring_size(min(ds->max_tx_ring_size,2));
    else
	ds->tx_size = make_ring_size(ds->max_tx_ring_size);

    idb->mci_txcount = ds->tx_size;

    ds->tx_head = ds->tx_tail = ds->tx_ra;
    ds->head_txp = ds->tail_txp = ds->tx_p;
    ds->tx_count = 0;

    last = ds->tx_head + (ds->tx_size - 1);
    ds->tx_max = last;
    pak = ds->head_txp;
    for (bd = ds->tx_head; bd <= last; bd++, pak++) {

	QASSERT(bd != NULL);
	QASSERT(pak != NULL);
	/*
	 * Free packet if nessesary.
	 */
	if (*pak != NULL) {
	    datagram_done(*pak);
	    *pak = NULL;
	    bd->buf_ptr = NULL;
	}

	QASSERT(bd->buf_ptr == NULL);
	bd->length = 0;

	/*
	 * Set the WRAP bit.
	 */
	if (bd == last)
	    bd->status = QUICC_BDSTAT_TX_WRAP;
	else
	    bd->status = 0;
    }

    /*
     * Initalize any transmit buffers left over.
     */
    last = ds->tx_head + ds->max_tx_ring_size;
    for ( ; bd < last; bd++, pak++) {
	/*
	 * Free packet if nessesary.
	 */
	if (*pak != NULL) {
	    datagram_done(*pak);
	    *pak = NULL;
	    bd->buf_ptr = NULL;
	}

	QASSERT(bd->buf_ptr == NULL);

	bd->length = 0;
	bd->status = 0;
    }
}

/*
 * =================================================================
 * Name: quicc_scc_bd_rx_ring_init
 *
 * Description:
 *	Initalize the receive buffer descriptor ring buffers.
 *
 * Parameters:
 *	hwidbtype * - hwidb for this interface.
 *	boolean  - TRUE mtu changed.
 *
 * Returns:
 *	void
 *
 * Note: Unlike sync case the async driver will initialize the
 *       input and output independently.
 *
 * =================================================================
 */
boolean 
quicc_scc_bd_rx_ring_init (hwidbtype *idb, boolean maxdgram_change)
{
    quicc_instance_t *ds;
    quicc_bd_t *bd;
    quicc_bd_t *last;
    paktype **pak;

    ds = INSTANCE;

    
    /*
     * Initalize receive buffer stuff.
     */
    ds->rx_size = make_ring_size(ds->max_rx_ring_size);
    ds->rx_head = ds->rx_ra;
    ds->head_rxp = ds->rx_p;
    last = ds->rx_head + (ds->rx_size - 1);
    ds->rx_max = last;
    pak = ds->head_rxp;

    /*
     * Initialize the receive buffer descriptors.
     */
    for (bd = ds->rx_head; bd <= last; bd++, pak++) {
	if (*pak != NULL) {
            if (maxdgram_change) {
                discard_buffer(*pak);
		*pak = NULL;
		bd->buf_ptr = NULL;
	    } else {
		bd->buf_ptr = (*pak)->datagramstart;
	    }
	}

	if (*pak == NULL) {
            *pak = les_driver_getbuf(idb);
            if (*pak == NULL) {
                return (FALSE);
            }
	    bd->buf_ptr = (*pak)->datagramstart;
        }

	bd->length = 0;
	bd->status = (QUICC_BDSTAT_RX_EMPTY |
                      QUICC_BDSTAT_RX_INT);
	if (bd == last)
	    bd->status |= QUICC_BDSTAT_RX_WRAP;
    }

    /*
     * Initialize any receive buffers left over.
     */
    last = ds->rx_head + ds->max_rx_ring_size;
    for (; bd < last; bd++, pak++) {
	bd->length = 0;
	bd->status = 0;
	if (*pak != NULL) {
	    discard_buffer(*pak);
	    *pak = NULL;
	    bd->buf_ptr = NULL;
	}
    }
    return(TRUE);

}

/*
 * =================================================================
 * Name: quicc_scc_bd_ring_init
 *
 * Description:
 *	Initalize the buffer descriptor ring buffers.
 *
 * Parameters:
 *	hwidbtype * - hwidb for this interface.
 *	boolean  - TRUE mtu changed.
 *
 * Returns:
 *	boolean TRUE - if the initalization worked.
 *	boolean FALSE - if we could not get a packet.
 * =================================================================
 */
boolean 
quicc_scc_bd_ring_init (hwidbtype *idb, boolean maxdgram_change)
{
    quicc_scc_bd_tx_ring_init(idb);
    return(quicc_scc_bd_rx_ring_init(idb, maxdgram_change));
}

/*
 * =================================================================
 * Name: quicc_tx_slots_available
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
 *	Number of available buffer descriptor
 *
 * =================================================================
 */
inline uint quicc_tx_slots_available (quicc_instance_t *ds)
{
    QASSERT(ds->tx_size > ds->tx_count);
    
    return(ds->tx_size - ds->tx_count);
}


/*
 * =================================================================
 * Name: quicc_scc_transmit
 *
 * Description:
 *	Transmit a packet on an SCC port.  The user must set the
 *	appropriate buffer descriptor flags.
 *
 * Parameters:
 *	quicc_instance_t * - pointer to driver instance.
 *	paktype * - packet to transmit.
 *	ushort  - protocol buffer descriptor flags.
 *
 * Returns:
 *	void - none.
 * =================================================================
 */
void quicc_scc_transmit (quicc_instance_t *ds, paktype *pak, ushort flags)
{
    quicc_bd_t *bd;

    bd = ds->tx_head;
    QASSERT((bd->status & QUICC_BDSTAT_TX_RDY) == 0);
    
    *ds->head_txp = pak;
        
    bd->length = pak->datagramsize;
    bd->buf_ptr = pak->datagramstart;
    bd->status |= (flags | QUICC_BDSTAT_TX_RDY);

    ds->tx_count++;
    
    if (bd == ds->tx_max) {
	ds->tx_head = ds->tx_ra;
	ds->head_txp = ds->tx_p;
    } else {
	ds->tx_head++;
	ds->head_txp++;
    }
}

/*
 * =================================================================
 * Name: quicc_scc_transmit_buffer
 *
 * Description:
 *	This routine is use mainly for Async.  Transmit a packet
 *	on an SCC port.  
 *	
 *
 * Parameters:
 *	quicc_instance_t * - pointer to driver instance.
 *	paktype * - packet to transmit.
 *	ushort  - protocol buffer descriptor flags.
 *
 * Returns:
 *	void - none.
 * =================================================================
 */
void
quicc_scc_transmit_buffer (quicc_instance_t *ds, paktype *pak,
			   uchar *datastart, int length, ushort flags)
{
    quicc_bd_t *bd;

    bd = ds->tx_head;
    QASSERT((bd->status & QUICC_BDSTAT_TX_RDY) == 0);
    
    *ds->head_txp = pak;
        
    bd->length = length;
    bd->buf_ptr = datastart;
    bd->status |= (flags | QUICC_BDSTAT_TX_RDY);

    ds->tx_count++;
    
    if (bd == ds->tx_max) {
	ds->tx_head = ds->tx_ra;
	ds->head_txp = ds->tx_p;
    } else {
	ds->tx_head++;
	ds->head_txp++;
    }
}


/*
 * SCC Interrupt Array.
 */
quicc_interrupt_t quicc_intr_array[QUICC_NUMBER_SCC];

/*
 * =================================================================
 * Name: quicc_scc_handler_install
 *
 * Description:
 *	Install the interrupt routines in the global interrupt
 *	array.
 *
 * Parameters:
 *	hwidbtype * - hardware idb.
 *	uhsort - SCC port number.
 *
 * Returns:
 * =================================================================
 */
void quicc_scc_handler_install (hwidbtype *idb, ushort port)
{
    quicc_interrupt_t *intr;
    quicc_dpr_t *dpr;
    
#if QUICC_PORT_SCC1 == 0
    QASSERT(port <= QUICC_PORT_SCC4);
#else
    QASSERT((port >= QUICC_PORT_SCC1) && (port <= QUICC_PORT_SCC4));
#endif
    
    dpr = (quicc_dpr_t *)getmbar();
    
    intr = &quicc_intr_array[port];

    QASSERT(intr->idb == NULL);

    intr->idb = idb;
    intr->event = &dpr->regs.scc[port].scc_scce;
    intr->cisr = &dpr->regs.cpic_cisr;
    intr->mask = quicc_scc_intr_mask[port];
}


#define QUICC_SCC_DEFAULT_REGISTERS(x)				\
	high = QUICC_SCC ## x ##_DEFAULT_GSMRH;			\
	low = QUICC_SCC ## x ## _DEFAULT_GSMRL;			\
	psmr = QUICC_SCC ## x ## _DEFAULT_PSMR;			\
	dsr = QUICC_SCC ## x ## _DEFAULT_DSR;			\
	rx_mask = QUICC_SCC ## x ## _DEFAULT_SCCM_RX;		\
	tx_mask = QUICC_SCC ## x ## _DEFAULT_SCCM_TX;		\
	misc_mask = QUICC_SCC ## x ## _DEFAULT_SCCM_MISC;

/*
 * =================================================================
 * Name: quicc_scc_reg_init
 *
 * Description:
 *
 * Parameters:
 *	quicc_dpr_t * - pointer to dualport ram.
 *	ushort - SCC port number.
 *
 * Returns:
 *	void - None.
 * =================================================================
 */
void quicc_scc_reg_init (quicc_dpr_t *dpr, ushort port)
{
    ulong low, high;
    ushort psmr;
    ushort dsr;
    ushort mask;
    ushort tx_mask, rx_mask, misc_mask;

#if QUICC_PORT_SCC1 == 0
    QASSERT(port <= QUICC_PORT_SCC4);
#else
    QASSERT((port >= QUICC_PORT_SCC1) && (port <= QUICC_PORT_SCC4));
#endif

    switch (port) {
    default:		/* This should never happen */
    case QUICC_PORT_SCC1:
	QUICC_SCC_DEFAULT_REGISTERS(1);
	break;
    case QUICC_PORT_SCC2:
	QUICC_SCC_DEFAULT_REGISTERS(2);
	break;
    case QUICC_PORT_SCC3:
	QUICC_SCC_DEFAULT_REGISTERS(3);
	break;
    case QUICC_PORT_SCC4:
	QUICC_SCC_DEFAULT_REGISTERS(4);
	break;
    }

    dpr->regs.scc[port].scc_dsr = dsr;
    dpr->regs.scc[port].scc_gsmrl = low;
    dpr->regs.scc[port].scc_gsmrh = high;
    dpr->regs.scc[port].scc_psmr = psmr;


    mask = (tx_mask | rx_mask | misc_mask);
    
    /*
     * Clear all SCC events.
     */
    dpr->regs.scc[port].scc_scce = 0xFFFF;
    dpr->regs.scc[port].scc_sccm = mask;
}


/*
 * =================================================================
 * Name: quicc_scc_show_regs
 *
 * Description:
 *	Display general SCC registers
 *
 * Parameters:
 *	hwidbtype *idb - idb pointer for interface
 *      boolean first_time - flag for controlling some portion of display
 *
 * Returns:
 *	void - None.
 * =================================================================
 */
void quicc_scc_show_regs (hwidbtype *idb, boolean first_time)
{
    quicc_instance_t *ds = (quicc_instance_t *)INSTANCE;

    /*
     * Display scc general parms 
     */
    printf("idb at %#x, driver data structure at %#x\n", idb, ds);
    printf("SCC Registers:\n");
    printf("General [GSMR]=%#x:0x%08x, Protocol-specific [PSMR]=%#x\n",
           *ds->gsmr_hi, *ds->gsmr_lo, *ds->psmr);
    printf("Events [SCCE]=0x%04x, Mask [SCCM]=0x%04x, "
	   "Status [SCCS]=0x%04x\n",
           *ds->scce, *ds->sccm, *ds->sccs);
    printf("Transmit on Demand [TODR]=%#x, Data Sync [DSR]=%#x\n",
	   ds->regbase->scc[ds->port_no].scc_todr,
	   ds->regbase->scc[ds->port_no].scc_dsr);

    if (first_time) { /* Display once only */
        printf("Interrupt Registers:\n");
	printf("Config [CICR]=0x%08x, Pending [CIPR]=0x%08x\n"
	       "Mask   [CIMR]=0x%08x, In-srv  [CISR]=0x%08x\n",
	       *ds->cicr, *ds->cipr, *ds->cimr, *ds->cisr);
	printf("Command register [CR]=%#x\n", *ds->cr);
	printf("Port A [PADIR]=0x%04x, [PAPAR]=0x%04x\n",
	       ds->regbase->pio_padir, ds->regbase->pio_papar);
	printf("       [PAODR]=0x%04x, [PADAT]=0x%04x\n",
	       ds->regbase->pio_paodr, ds->regbase->pio_padat);
	printf("Port B [PBDIR]=0x%06x, [PBPAR]=0x%06x\n",
	       ds->regbase->pip_pbdir, ds->regbase->pip_pbpar);
	printf("       [PBODR]=0x%06x, [PBDAT]=0x%06x\n",
	       ds->regbase->pip_pbodr, ds->regbase->pip_pbdat);
	printf("Port C [PCDIR]=0x%04x, [PCPAR]=0x%04x\n",
	       ds->regbase->pio_pcdir, ds->regbase->pio_pcpar);
	printf("       [PCSO]=0x%04x,  [PCDAT]=0x%04x, [PCINT]=0x%04x\n",
	       ds->regbase->pio_pcso, ds->regbase->pio_pcdat,
	       ds->regbase->pio_pcint);
    }
}
	       
/*
 * =================================================================
 * Name: quicc_scc_show_scc_param
 *
 * Description:
 *	Display general SCC parameters
 *
 * Parameters:
 *	quicc_scc_param_t * - pointer to scc parameter ram.
 *
 * Returns:
 *	void - None.
 * =================================================================
 */
static void quicc_scc_show_scc_param (quicc_scc_param_t *scc)
{
    printf ("\nSCC GENERAL PARAMETER RAM (at %#x)\n", scc);
    printf ("Rx BD Base [RBASE]=%#x, Fn Code [RFCR]=%#x\n", 
	    scc->rbase, scc->rfcr);
    printf ("Tx BD Base [TBASE]=%#x, Fn Code [TFCR]=%#x\n", 
	    scc->tbase, scc->tfcr);
    printf ("Max Rx Buff Len [MRBLR]=%d\n", scc->mrblr);
    printf ("Rx State [RSTATE]=%#x, BD Ptr [RBPTR]=%#x\n", 
	    scc->rstate, scc->rbptr);
    printf ("Tx State [TSTATE]=%#x, BD Ptr [TBPTR]=%#x\n\n", 
	    scc->tstate, scc->tbptr);
}


/*
 * =================================================================
 * Name: quicc_scc_show_ether_param
 *
 * Description:
 *	Display ethernet parameter ram values.
 *
 * Parameters:
 *	quicc_ether_param_t * - pointer to ethernet parameter ram.
 *
 * Returns:
 *	void - None.
 * =================================================================
 */
void quicc_scc_show_ether_param (quicc_ether_param_t *ether)
{
    quicc_scc_show_scc_param((quicc_scc_param_t *)ether);

    printf ("SCC ETHERNET PARAMETER RAM (at %#x)\n", &ether->c_pres);
    printf ("CRC Preset [C_PRES]=%#x, Mask [C_MASK]=%#x\n", 
	    ether->c_pres, ether->c_mask);
    printf ("Errors: CRC [CRCEC]=%d, "
	    "Alignment [ALEC]=%d, "
	    "Discards [DISFC]=%d\n", 
	    ether->crcec, ether->alec, ether->disfc);
    printf ("PAD Char [PADS]=%#x\n", ether->pads);
    printf ("Retry Limit [RET_LIM]=%d, Count [RET_CNT]=%d\n", 
	    ether->ret_lim, ether->ret_cnt);
    printf ("Frame Lengths: [MAXFLR]=%d, [MINFLR]=%d\n", 
	    ether->mflr, ether->minflr);
    printf ("Max DMA Lengths: [MAXD1]=%d, [MAXD2]=%d\n", 
	    ether->maxd1, ether->maxd2);
    printf ("Group Address Filter [GADDRn]=%04x:%04x:%04x:%04x\n", 
	    ether->gaddr1, ether->gaddr2,
	    ether->gaddr3, ether->gaddr4);
    printf ("Indiv Address Filter [IADDRn]=%04x:%04x:%04x:%04x\n",
	    ether->iaddr1, ether->iaddr2,
	    ether->iaddr3, ether->iaddr4);
    printf ("Physical Address [PADDR1]=%04x.%04x.%04x\n", 
	    ether->paddr_h, ether->paddr_m, ether->paddr_l);
    printf ("Last Address Set in Filter [TADDR]=%04x.%04x.%04x\n", 
	    ether->taddr_h, ether->taddr_m, ether->taddr_l);
    printf ("Persistence [P_Per]=%d, Backoff Cnt [BOFF_CNT]=%d\n", 
	    ether->p_per, ether->boff_cnt);
    printf("BD Pointers:\n");
    printf("First Rx [RFBD]=%#x, ", ether->rfbd_ptr);
    printf ("First Tx [TFBD]=%#x, Last Tx [TLBD]=%#x\n\n", 
	    ether->tfbd_ptr, ether->tlbd_ptr);
}


/*
 * =================================================================
 * Name: quicc_scc_show_hdlc_param
 *
 * Description:
 *	Display HDLC parameter ram values
 *
 * Parameters:
 *	quicc_hdlc_param_t * - pointer to hdlc pearmeter ram.
 *
 * Returns:
 *	void - None.
 * =================================================================
 */
void quicc_scc_show_hdlc_param (quicc_hdlc_param_t *hdlc)
{
    quicc_scc_show_scc_param((quicc_scc_param_t *)hdlc);

    printf ("SCC HDLC PARAMETER RAM (at %#x)\n", &hdlc->c_pres);
    printf ("CRC Preset [C_PRES]=%#x, Mask [C_MASK]=%#x\n", 
	    hdlc->c_pres, hdlc->c_mask);
    printf ("Errors: CRC [CRCEC]=%d, "
	    "Aborts [ABTSC]=%d, "
	    "Discards [DISFC]=%d\n", 
	    hdlc->crcec, hdlc->abtsc, hdlc->disfc);
    printf ("Nonmatch Addr Cntr [NMARC]=%d\n", hdlc->nmarc);
    printf ("Retry Count [RETRC]=%d\n", hdlc->retrc);
    printf ("Max Frame Length [MFLR]=%d\n", hdlc->mflr);
    printf ("Rx Int Threshold [RFTHR]=%d, Frame Cnt [RFCNT]=%d\n", 
	    hdlc->rfthr, hdlc->rfcnt);
    printf ("User-defined Address %04x/%04x/%04x/%04x\n", 
	    hdlc->haddr1, hdlc->haddr2,
	    hdlc->haddr3, hdlc->haddr4);
    printf ("User-defined Address Mask 0x%04x\n\n", hdlc->hmask);
}


/*
 * =================================================================
 * Name: quicc_scc_show_uart_param
 *
 * Description:
 *	Display UART parameter ram values
 *
 * Parameters:
 *	quicc_uart_param_t * - pointer to uart pearmeter ram.
 *
 * Returns:
 *	void - None.
 * =================================================================
 */
void quicc_scc_show_uart_param(quicc_uart_param_t *uart)
{
    int i;
    
    quicc_scc_show_scc_param((quicc_scc_param_t *)uart);
    
    printf ("SCC UART PARAMETER RAM (at %#x)\n", uart);
    printf ("Maximum idle characters %d\n", uart->max_idl);
    printf ("Break Character %x\n",uart->brkcr);
    
    printf ("Received Parity Error %d\n",uart->parec);
    printf ("Received Frame Error %d\n", uart->frmer);
    printf ("Received Noise Error %d\n", uart->nosec);
    printf ("Number of break conditions %d\n", uart->brkec);
    
    printf ("Break length %d\n", uart->brkln);
    printf ("uart1 %x\n", uart->uaddr1);
    printf ("uart2 %x\n", uart->uaddr2);
    printf ("Out of sequence %x\n", uart->toseq);

    /* Should use define for max count */
    for (i=0; i < 0x8 ; i++)
	printf ("cc[%d] = %x \n", i, uart->cc[i]);
    
    printf ("rccm = %x\n", uart->rccm);
    printf ("rccr = %x\n", uart->rccr);
    printf ("rlbc = %x\n", uart->rlbc);
}


/*
 * Set up the QUICC SCC RX ring entry to reuse the packet buffer:
 */
static inline void quicc_scc_rx_resetpak (RXBD_T *bd)
{
    /*
     * Re-Init fields in the buffer descriptor
     */
    bd->status = (bd->status & QUICC_BDSTAT_RX_WRAP) |
                  QUICC_BDSTAT_RX_DEFAULT;  /* quicc owns the buffer now */
    bd->length = 0;
}


/*
 * Handle quicc SCC RX interrupts.
 * This routine is in the interrupt path and is coded for speed.
 * Ha ha, just joking.
 */
void quicc_scc_rx_intr (hwidbtype *idb)
{
    quicc_instance_t *ds;
    RXBD_T *bd;
    paktype **pak_ptr;
    register ushort bd_status;
    paktype *pak, *replacement;
    ulong count;
    uchar packets = 1;

    ds = INSTANCE;
    
    /*
     * Receive all of the input frames that we own:
     */
    if (sched_run_interval && ds->hw_throttle && AWAKE(sched_run_time)) {
	/*
	 * the scheduler has not run recently enough,
	 * so, throttle the ethernet interface:
	 */
	(*ds->hw_throttle)(idb);
    } else

    for (bd = ds->rx_head;
         (((bd->status & QUICC_BDSTAT_RX_EMPTY) == 0) &&
	   (packets++ < MAX_SPIN));
	 bd = ds->rx_head) {
	    
        count = bd->length - ds->crc_length;
        bd_status = bd->status;
        pak_ptr = ds->head_rxp;
	pak = (paktype *)*ds->head_rxp; /* Get packet pointer */	

	/*
	 * Process only good packets which are not chained.
	 * If packets are chained, they are giants.
	 */
	set_if_input(pak, idb->firstsw);
	
	pak->datagramsize = count;
	if ((bd_status & ds->rx_bd_mask) == ds->rx_bd_good_mask) {
	    
	    /*
	     * Attempt to get a new packet buffer to replace
	     * this packet buffer on the RX ring:
	     */
	    replacement = les_driver_getbuf_inline(idb, TRUE);
	    if (replacement) {
		bd->buf_ptr = (uchar *)replacement->datagramstart; 
                bd->length = 0;
                bd->status = (bd_status & QUICC_BDSTAT_RX_WRAP) |
		    QUICC_BDSTAT_RX_DEFAULT;
                *pak_ptr = replacement;

		/* Need really for the UART case but we may decide
		 * to pass it into rx_handler later as an argument
		 * instead of setting ds->rx_bd_status
		 */
		ds->rx_bd_status = bd_status;

                if (ds->rx_handler)
                    (*ds->rx_handler)(idb, pak);
		else {
		    clear_if_input(pak);
		    quicc_scc_rx_resetpak(bd);
                    idb->counters.input_error++;
		    idb->counters.input_drops++;
		}
	    } else {
		
		/*
		 * We could not get a new buffer to replace this one on
		 * the receiver ring, so drop the frame instead of
		 * processing it.
		 */
		clear_if_input(pak);

                if (ds->hw_throttle) 
	            (*ds->hw_throttle)(idb);
		return;
	    }
  	} else {   /* error in receiving packet */

	    if (ds->RX_errors)
	        (*ds->RX_errors)(idb, bd_status);

	    clear_if_input(pak);
	    quicc_scc_rx_resetpak(bd);	/* Give packet back to the quicc */
 	}
	ds->head_rxp++;
	if (++ds->rx_head > ds->rx_max) {
	    ds->rx_head = ds->rx_ra;
	    ds->head_rxp = &ds->rx_p[0];
	}
    } /* End of 'for' loop */
}

/*
 * Handle Quicc SCC TX interrupts:
 */
void quicc_scc_tx_intr (hwidbtype *idb)
{
    TXBD_T *bd;
    quicc_instance_t *ds;
    paktype **pak_ptr;
    paktype *pak;

    /*
     * Process all transmitted packets:
     */
    ds = INSTANCE;

    for (bd = ds->tx_tail, pak_ptr = ds->tail_txp, pak = (paktype *)*pak_ptr;
	 pak && ((bd->status & QUICC_BDSTAT_TX_RDY) == 0);
	 bd = ds->tx_tail, pak_ptr = ds->tail_txp, pak = (paktype *)*pak_ptr) {

	/*
	 * Check for transmit errors:
	 */
        if (ds->TX_errors)
            (*ds->TX_errors)(idb, bd->status);
		
	/*
	 * Do packet TX accounting and clean up:
	 */
	datagram_done(pak); /* Release the output packet */
	*pak_ptr = 0;	/* Not pointing to a packet anymore */
	bd->buf_ptr = 0;
	bd->length = 0;
	bd->status &= QUICC_BDSTAT_TX_WRAP;

	ds->tx_count--;
	ds->tail_txp++;	/* Point to next packet */
	ds->tx_tail++;	/* Point to next ring entry */
	if (ds->tx_tail > ds->tx_max) {
	    ds->tx_tail = ds->tx_ra;
	    ds->tail_txp = &ds->tx_p[0];
	}
    }
    GET_TIMESTAMP(idb->lastoutput);

    /*
     * Attempt to recover from any fatal TX errors we noticed:
     */
    if (ds->fatal_tx_err) {
        ds->fatal_tx_err = FALSE;
        ds->tx_restarts++;

        if (ds->line_down_fn)
            (*ds->line_down_fn)(idb);

        /*
         * Give the RESTART_TRANSMIT command.
         */
        quicc_scc_enable_tx(ds->dprbase, ds->qinfo);
    }
    if (idb->idb_tty)
	(*ds->quicc_async_tx_intr_ptr)(idb);
    else
	(*idb->soutput)(idb); /* Possibly restart output */
}

/*
 * Handle quicc  Error interrupts:
 * Will be called only for the BSY error (error which
 * does not involve dorking with a BD).
 */
void quicc_scc_misc_intr (hwidbtype *idb, ushort status)
{
    quicc_instance_t *ds;

    /*
     * Process Error interrupts:
     */
    ds = INSTANCE;

    if (ds->misc_handler)
        (*ds->misc_handler)(idb, status);
    else {
        idb->counters.input_error++;
        idb->counters.input_resource++;
        ds->E_ints++;
        ds->buffer_err++;
    }
}

/*
 * Start or restart output.
 */
void quicc_tx_start_output (hwidbtype *idb)
{
    TXBD_T *tmd;
    quicc_instance_t *ds;
    paktype **pak_ptr;
    uchar *start;
    paktype *pak;
    leveltype SR;
    ushort bytes;
    
    ds = INSTANCE;
    SR = raise_interrupt_level(ETHER_DISABLE);
    
    /*
     * While there is another packet to output and room for it in the
     * TX queue, dequeue the packet:
     */
    while ((ds->tx_count < ds->tx_size) &&
	   (pak = (*idb->oqueue_dequeue)(idb))) {

	/*
	 * Is the packet too big to transmit?
	 */
	bytes = pak->datagramsize;
	if (bytes > ds->max_frame_len) {
	    idb->counters.output_total_drops++;
	    errmsg(&msgsym(TOOBIG, LINK), idb->hw_namestring, bytes);
	    datagram_done(pak);
	    continue;
	}
        /*
         * Too small ? Adjust size for output byte count.
         */
	if (bytes < ds->min_frame_len) {
	    bytes = ds->min_frame_len;
	    pak->datagramsize = bytes;
	}
	/*
	 * Get pointers for the TX ring entry at tx_head:
	 */
        pak_ptr = ds->head_txp;
	tmd = ds->tx_head;
	if ((tmd->status & QUICC_BDSTAT_TX_RDY) == 0) {

            *pak_ptr = pak;
	    start = pak->datagramstart;
	    if (*start & 1)
		idb->counters.output_broad++;

	    /*
	     * Set up this TX ring entry for this packet and force TX:
	     */
	    tmd->buf_ptr = start;
	    tmd->length = bytes;
	    tmd->status = QUICC_BDSTAT_TX_DEFAULT | 
                          (tmd->status & QUICC_BDSTAT_TX_WRAP);

	    /*
	     * Force the quicc to poll the TX ring and see this packet:
             * (if Tx ring is empty).
	     */
            if (ds->tx_count == 0)
	        *ds->tx_todr = QUICC_TX_KICK;
	    ds->tx_count++;

            ds->head_txp++;
	    if (++ds->tx_head > ds->tx_max) {
		ds->tx_head = ds->tx_ra;
                ds->head_txp = ds->tx_p;
	    }
	    /*
	     * Do packet accounting after starting the quicc:
	     */
	    idb->counters.tx_cumbytes += bytes;
	    TIMER_START(idb->xmittimer, TIMEOUT); /* set timeout timer */
	    idb->counters.outputs++;

	} else {

	    /*
	     * Error - we don't own the ring entry, try again later:
	     */
	    pak->if_output = idb->firstsw;
	    errmsg(&msgsym(OWNERR, QUICC), idb->unit, pak);
	    pak_requeue_head(idb, pak);
	    break;
	}
    }  /* End while */

    reset_interrupt_level(SR);
}

/*
 * Show a receive or transmit ring entry:
 * Use RXBD_T since both the Tx and Rx BDs are identical.
 */
static void quicc_scc_show_bd (paktype *pak, void *bd_ptr, int n)
{
    RXBD_T *bd = bd_ptr;

    printf("%02d pak=0x%07x buf=0x%07x status=%04x pak_size=%d\n",
           n, pak, bd->buf_ptr, bd->status, bd->length);
}

/*
 * Setup driver stuff common to SCCs
 */
void quicc_scc_driver_setup (hwidbtype *idb)
{
    quicc_instance_t *ds;
    ulong port;
    int i;

    ds = INSTANCE;
    port = ds->port_no;

    ds->regbase  = &ds->dprbase->regs;
    ds->tx_todr  = (ushort *)&ds->regbase->scc[port].scc_todr;

    /*
     * Allocate a receive ring:
     */
    ds->rx_base = quicc_rxbd_alloc(port);
    if (ds->rx_base)
        ds->rx_ra = ds->rx_base;
    else {
        crashdump(0);
    }
    for (i = 0; i < MAX_RING; i++)
        ds->rx_p[i] = 0;

    /*
     * Allocate a transmit ring:
     */
    ds->tx_base = quicc_txbd_alloc(port);
    if (ds->tx_base)
        ds->tx_ra = ds->tx_base;
    else {
        crashdump(0);
    }
    for (i = 0; i < MAX_RING; i++)
        ds->tx_p[i] = 0;

    ds->mode       = 0;
    ds->gsmr_hi    = &ds->regbase->scc[port].scc_gsmrh;  /* High GSMR */
    ds->gsmr_lo    = &ds->regbase->scc[port].scc_gsmrl;  /* Low GSMR */ 
    ds->psmr       = &ds->regbase->scc[port].scc_psmr;   /* PSMR */
    ds->cipr       = &ds->regbase->cpic_cipr; 
    ds->cicr       = &ds->regbase->cpic_cicr; 
    ds->cimr       = &ds->regbase->cpic_cimr;  
    ds->cisr       = &ds->regbase->cpic_cisr;     
    ds->cr         = &ds->regbase->cp_cr;     
    ds->scce       = &ds->regbase->scc[port].scc_scce; 
    ds->sccm       = &ds->regbase->scc[port].scc_sccm; 
    ds->sccs       = &ds->regbase->scc[port].scc_sccs; 

    ds->show_rxbd  = quicc_scc_show_bd;
    ds->show_txbd  = quicc_scc_show_bd;

    /*
     * Initialize Throttle statistics 
     */
    ds->enabled = 0;

    /*
     * Initialize RX error statistics 
     */
    ds->overflow_err    = 0;
    ds->buffer_err      = 0;

    /*
     * Initialize TX error statistics 
     */
    ds->tx_restarts     = 0;
    ds->fatal_tx_err    = FALSE;

    /*
     * Initialize other counters
     */
    ds->E_ints        = 0;
}

