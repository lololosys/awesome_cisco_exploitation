/* $Id: quicc_serial_common.c,v 3.1.6.3 1996/08/28 13:10:35 thille Exp $
 * $Source: /release/112/cvs/Xsys/src-36/quicc_serial_common.c,v $
 *------------------------------------------------------------------
 * quicc_serial_common.c  source for 68360 QUICC chip use by sync/async
 *
 * March 1996, John Ng
 *
 * Copyright (c) 1996-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: quicc_serial_common.c,v $
 * Revision 3.1.6.3  1996/08/28  13:10:35  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.1.6.2  1996/08/13  02:22:24  gstovall
 * CSCdi39484:  Giant packets output on ethernet interface
 * Branch: California_branch
 * Make sure we drop giants, even if they are just a wee bit too big.
 *
 * Revision 3.1.6.1  1996/06/12  19:56:53  jng
 * CSCdi60260:  Async catwoman commit
 * Branch: California_branch
 * Async Catwoman commit into California
 *
 * Revision 3.1  1996/03/20  01:27:09  jng
 * Initial Async Catwoman placeholder
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "interface_private.h"
#include "packet.h"
#include "logger.h"
#include "../src-36/msg_quicc.c"	/* Not a typo, see logger.h */
#include "../src-36-c1000/msg_c1000_ether.c" /* Not a typo, see logger.h */

#include "../os/pool.h"
#include "../src-36/quicc.h"
#include "../src-36/quicc_lib.h"

#include "../src-36/quicc_serial_common.h"
#include "../les/if_les.h"

#include "../src-36/if_serial_quicc.h"
#include "../src-36/if_async_quicc.h"
#include "../src-36-c1000/cpai.h"
#include "../src-36-c1000/init_c1005.h"


/*
 * =================================================================
 * Name: quicc_modem_interrupt_mask
 *
 * Description:
 *	This routine is use to enable and disable modem interrupt
 *	handler.
 *
 * Parameters:
 *	hwidbtype 	- pointer to hwidb for this interface
 *
 * Returns:
 *	void - None.
 * =================================================================
 */
void
quicc_modem_interrupt_mask (hwidbtype *idb, boolean enable, ulong mask)
{
    
    quicc_instance_t *ds;
    quicc_dpr_t *dpr;

    QASSERT(idb);
    ds = INSTANCE;
    dpr = (quicc_dpr_t *)ds->dprbase;

    if (enable)
	dpr->regs.cpic_cimr |= mask; 
    else
	dpr->regs.cpic_cimr &= ~mask; 
	
}

/*
 * =================================================================
 * Name:  memory_pool_adjustment
 *
 * Description:
 *	This is a general memory pool allocation routine
 *	which will initialize, discard or create memory pool necessary
 *	for this particular idb.  PLEASE NOTE this routine assumes
 *	that packets which needs to be discarded have done so already.
 *
 * Parameters:
 *	hwidbtype 	- pointer to hwidb for this interface
 *	boolean		- MTU change
 *
 * Returns:
 *	void - None.
 * =================================================================
 */
void
memory_pool_adjustment (hwidbtype *idb, boolean maxdgram_change)
{
    
    quicc_instance_t *ds;
    pooltype *old_pool;
    
    ds = INSTANCE;
    /*
     * If a private buffer pool for this interface exists AND there
     * has been an MTU change, discard the pool with the old-sized
     * buffers.
     */
    if (idb->pool && maxdgram_change) {
	old_pool = idb->pool;
	idb->pool = NULL;
	pool_destroy(old_pool);
    }
    
    /*
     * If the private buffer pool has not been allocated yet, or if
     * it was destroyed due to an MTU change above, allocate it now.
     * In either case the maxdgram_change should be set to TRUE.
     * Populate the pool, initialize a private buffer cache,
     * and populate the buffer cache with buffers from the newly
     * created private pool.  Also set up the fallback public buffer
     * pool.
     */
    if (!idb->pool) {

        if (idb->pool_group == POOL_GROUP_INVALID)
            idb->pool_group = pool_create_group();
	
        if (!idb->hw_namestring)
            idb_init_names(idb->firstsw, FALSE);
	
        idb->pool = pak_pool_create(idb->hw_namestring, idb->pool_group,
				    idb->max_buffer_size,
				    POOL_SANITY | POOL_CACHE,
				    NULL);
        if (!idb->pool) {
	    /* This needs to be change to take out the QUICC to make this
	     * code generic.
	     */
	    errmsg(&msgsym(NOMEMORY, QUICC), idb->unit,
		   "private buffer pool");
	    crashdump(0);
	}
	
        idb->pool->fallback = pak_pool_find_by_size(idb->max_buffer_size);
        pool_adjust(idb->pool, 0, PRIV_NUM_BUF(ds->max_rx_ring_size),
                    PRIV_NUM_BUF(ds->max_rx_ring_size), TRUE);
	
        pak_pool_create_cache(idb->pool, PRIV_NUM_CACHE(ds->max_rx_ring_size));
        pool_adjust_cache(idb->pool, PRIV_NUM_CACHE(ds->max_rx_ring_size));
    }
}

quicc_cpic_interrupt_t quicc_cpic_intr_array[MAX_CPIC_INTR_INDEX];

/*
 * =================================================================
 * Name: quicc_cpic_handler_install
 *
 * Description:
 *	This is a general routine initialization for the cpic
 *	interrupt handler.  The function of this routine saves
 *	off the idb, cisr, cimr_mask and the interrupt function handler
 *	which the assembly routine to take care of processing
 *	the interrupt.  PLEASE NOTE, this means interrupt processing
 *	is taken care of in two pass, the assembly routine will get
 *	call which in turns call the function save off in the
 *	quicc_cpic_intr_array[], the motivation for this is to call
 *	one common assembly routine which will handle the general
 *	clean up and the specific function will handle the interrupt
 *	specific processing.  The assembly routine is intended to be
 *	coded for speed.
 *
 * Parameters:
 *	hwidbtype 		- pointer to hwidb for this interface
 *	quicc_dpr_t		- dual port ram for the QUICC chip this
 *				  is independent of the SCC.
 *	ushort			- the index into the quicc_cpic_intr_array[]
 *	ushort			- the vector number specific to the
 *				  interrupt handler being install
 *	func			- specific interrupt function handler 
 *	asm_func		- the assembly routine which the QUICC
 *				  will call when cpic_vbits interrupt
 *				  occurs.
 *
 * Returns:
 *	void - None.
 * =================================================================
 */
void quicc_cpic_handler_install
         (hwidbtype    *idb,
          quicc_dpr_t  *dpr,
          ushort       index,
          ushort       cpic_vbits, 
          ulong        interrupt_reset_mask,
          void         (*func)(hwidbtype *),
          void         (*asm_func)(void))
{
   quicc_cpic_interrupt_t *intr_array;

    QASSERT(index < MAX_CPIC_INTR_INDEX);
    
    intr_array = &quicc_cpic_intr_array[index];

    intr_array->idb		= idb;
    intr_array->cisr 		= &dpr->regs.cpic_cisr;
    intr_array->cimr_mask	= interrupt_reset_mask;
    intr_array->intr_handler	= func;

   quicc_install_cpm_exception(dpr,asm_func,cpic_vbits);
}


/*
 * =================================================================
 * Name:  quicc_set_serial_pins
 *
 * Description:
 *	Set/clear all parallel port pins required to support serial
 *	interface.
 *
 * Parameters:
 *	quicc_dpr_t * - pointer to dual port ram.
 *
 * Returns:
 *	void - None.
 * =================================================================
 */
void quicc_set_serial_pins (quicc_dpr_t *dpr)
{
    /*
     *	Port A
     *
     *	PIN    PAPAR   PADIR   PAODR    FUNCTION
     *
     *	15	0	0	0	Mode bit 2 (chk cable in parallel)
     *	14	1	0	0	Transmit clock for DTE
     *	13	0	0	0	Mode bit 1 (chk cable in parallel)
     *	12	1	0	0	Receive clock 
     *	10	0	0	0	Mode bit 0 (chk cable in parallel)
     *	 8	0	0	0	
     *	 7	1	0	0	Transmit data pin
     *	 6	1	0	0	Receive data pin
     *	 5	0	0	0	Loop back mode indicator
     *	 4	0	0	0	SENSE1 bit use to chk cable serially
     */
    
    /* Turn off papar bits 15, 13, 10, 8, 5, 4 and then
     * turn on bits 14, 12, 7, 6
     */
    dpr->regs.pio_papar &= ~0xA530;
    dpr->regs.pio_papar |=  0x50C0;
    
    /*
     * Turn off bits 15-12, 10, and 8-4 for both padir and paodr
     */
    dpr->regs.pio_padir &= ~0xF5F0;
    dpr->regs.pio_paodr &= ~0xF5F0;
    
    
    /*
     * Port B
     *
     *	PIN    PBPAR   PBDIR   PBODR	FUNCTION
     *
     *	4	0	1	0	
     *	3	0	1	0
     *	2	0	1	0
     *	1	0	1	0
     *	0	0	1	0
     */
    dpr->regs.pip_pbpar &= ~0x001F;
    dpr->regs.pip_pbdir |=  0x001F;
    dpr->regs.pip_pbodr &= ~0x001F;
    
    /*
     * Port C
     *
     *	PIN    PCPAR   PCDIR	PCSO	NAME
     *
     *	11	0	0	1	DCD 
     *	10	0	0	1	CTS 
     *	 3	0	1	0	RTS 
     *	 2	0	1	0	DTR
     *	 1	0	0	0	DSR
     */
    dpr->regs.pio_pcpar &= ~0x0C0E;
    
    dpr->regs.pio_pcdir &= ~0x0C02;
    dpr->regs.pio_pcdir |=  0x000C;
    
    dpr->regs.pio_pcso  &= ~0x000E;
    dpr->regs.pio_pcso  |=  0x0C00;
    
    /*
     * Monitor all transition for PC11, PC10, PC3-PC1
     */
    dpr->regs.pio_pcint &=  ~0x0C0E;
}

/*
 * =================================================================
 * Name: quicc_set_async_serial_pins 
 *
 * Description:
 *	Set/clear all parallel port pins required to support serial
 *	interface.
 *
 * Parameters:
 *	quicc_dpr_t * - pointer to dual port ram.
 *
 * Returns:
 *	void - None.
 * =================================================================
 */
void quicc_set_async_serial_pins (quicc_dpr_t *dpr)
{
    /*
     *	Port A
     *
     *	PIN    PAPAR   PADIR   PAODR    FUNCTION
     *
     *	15	0	0	0	Mode bit 2 (chk cable in parallel)
     *	14	1	0	0	Transmit clock for DTE
     *	13	0	0	0	Mode bit 1 (chk cable in parallel)
     *	12	1	0	0	Receive clock 
     *	10	0	0	0	Mode bit 0 (chk cable in parallel)
     *	 8	0	0	0	
     *	 7	1	0	0	Transmit data pin
     *	 6	1	0	0	Receive data pin
     *	 5	0	0	0	Loop back mode indicator
     *	 4	0	0	0	SENSE1 bit use to chk cable serially
     */
    
    /* Turn off papar bits 15, 13, 10, 8, 5, 4 and then
     * turn on bits 14, 12, 7, 6
     */
    dpr->regs.pio_papar &= ~0xA530;
    dpr->regs.pio_papar |=  0x50C0;
    
    /*
     * Turn off bits 15-12, 10, and 8-4 for both padir and paodr
     */
    dpr->regs.pio_padir &= ~0xF5F0;
    dpr->regs.pio_paodr &= ~0xF5F0;
    
    
    /*
     * Port B
     *
     *	PIN    PBPAR   PBDIR   PBODR	FUNCTION
     *
     *	4	0	1	0	
     *	3	0	1	0
     *	2	0	1	0
     *	1	0	1	0
     *	0	0	1	0
     */
    dpr->regs.pip_pbpar &= ~0x001F;
    dpr->regs.pip_pbdir |=  0x001F;
    dpr->regs.pip_pbodr &= ~0x001F;
    
    /*
     * Port C
     *
     *	PIN    PCPAR   PCDIR	PCSO	NAME
     *
     *	11	0	0	0	DCD asserted internally always 
     *	10	0	0	0	CTS asserted internally always 
     *	 3	1	0	0	RTS 
     *	 2	0	1	0	DTR
     *	 1	0	0	0	DSR
     */
    dpr->regs.pio_pcpar &= ~0x0C06;
    dpr->regs.pio_pcpar |=  0x0008;
    
    dpr->regs.pio_pcdir &= ~0x0C0A;
    dpr->regs.pio_pcdir |=  0x0004;

    dpr->regs.pio_pcso  &= ~0x0C0E;
    
    /*
     * Monitor all transition for PC11, PC10, PC3-PC1
     */
    dpr->regs.pio_pcint &=  ~0x0C0E;
}


/* 
 * =================================================================
 *  Name:	quicc_serial_onesec_periodic
 *
 *  Description: This routine is needed to turn off any WAN LEDS
 *		 that may have been turned on as a result of getting
 *		 a serial interrupt.
 *
 *  CALLER:	Called once per second from the net_periodic process.
 *		
 *  Returns:	None
 * =================================================================
 */
void quicc_serial_onesec_periodic (hwidbtype *idb)
{
    quicc_instance_t *ds;

    QASSERT(idb);

    if ((idb->type != IDBTYPE_QUICC_SERIAL) && (idb->type != IDBTYPE_ASYNC))
	return;
	    
    ds = INSTANCE;

    QUICC_WAN_RX_LED_OFF(ds->dprbase);
    QUICC_WAN_TX_LED_OFF(ds->dprbase);
}
