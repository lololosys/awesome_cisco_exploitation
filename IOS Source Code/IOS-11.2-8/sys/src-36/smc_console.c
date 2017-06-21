/* $Id: smc_console.c,v 3.2.62.1 1996/03/18 21:56:47 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/src-36/smc_console.c,v $
 *------------------------------------------------------------------
 * smc_console - Console driver for Motorola SMC (Serial management controller)
 *  
 * November 27, 1994, Kevin Paul Herbert
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: smc_console.c,v $
 * Revision 3.2.62.1  1996/03/18  21:56:47  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.1  1996/01/24  22:49:23  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.2  1995/11/17  18:39:38  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:19:25  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:52:07  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "ttysrv.h"
#include "quicc.h"
#include "quicc_lib.h"
#include "../os/async.h"

#define DPR ((quicc_dpr_t *)ADRSPC_QUICC_DPRAM)
#define SMC1PARAM (DPR->smc1.uart)

#define REGB ((quicc_internal_regs_t *)ADRSPC_QUICC_REGB)
#define SMC1 (REGB->smc_regs[CONSOLE_SMC_IDX])

#define CONSOLE_DMASIZE (32)
#define POLLED_OVERFLOW_SIZE (CONSOLE_DMASIZE * 2)

extern void smc_int(void);	/* Assembly interrupt wrapper */

vectortype console_vector;
static int console_current_dma;	/* Size of DMA in progress, if from ttybuf */

/*
 * The following cells are used early in system startup (by gdb) so
 * they must be set up as initialized data or they will get zeroed during
 * BSS clearing.
 */

quicc_bd_t *rxbd = NULL;	/* Address of rx buffer descriptor start */
quicc_bd_t *txbd = NULL;	/* Address of tx buffer descriptor start */
static boolean console_initialized = FALSE; /* True when console initialized */
quicc_bd_t saved_rxbd = {};	/* Saved receive buffer descriptor */
quicc_bd_t saved_txbd = {};	/* Saved transmit buffer descriptor */
int saved_mrblr = 0;		/* Saved mrblr value */
int saved_max_idl = 0;		/* Saved max_idl value */

static int polled_overflow_empty = 0;
static int polled_overflow_fill = 0;

static char console_tx_dmabuf[CONSOLE_DMASIZE] = {};
static char console_rx_dmabuf[CONSOLE_DMASIZE] = {};

static char overflow_input[POLLED_OVERFLOW_SIZE] = {};

/*
 * smc_transmit_service - Set up a transmit
 *
 * Note: this routine must be called with tty interrupts disabled
 */

static void smc_transmit_service (tt_soc *tty)
{
    int len;

    /*
     * Handle transmit data
     */

    while (!(txbd->status & QUICC_BDSTAT_TX_RDY)) {

	/*
	 * Update the output buffer pointer based on the number of bytes
	 * that we just DMAed from the output buffer. If this was a special
	 * DMA operation, the size will be zero, which will keep us from
	 * changing the output pointer.
	 */

	tty->outhead += console_current_dma; /* Adjust for characters output */
	if (tty->outhead >= tty->outend)
	  tty->outhead -= tty->outsize;	/* Handle buffer wrap */

	tty->outcount -= console_current_dma; /* Adjust remaining characters */
	console_current_dma = 0;

	/*
	 * If the line is set to send flow control, do so now
	 */

	if (tty->statbits & SENDXOFF) {
	    if (tty->capabilities & SFLOWOUT) {
		console_tx_dmabuf[0] = tty->tty_stopchar;
		txbd->buf_ptr = console_tx_dmabuf; /* Set DMA Pointer */
		txbd->length = 1; /* Set the length */
		txbd->status = QUICC_BDSTAT_TX_RDY|
		  	       QUICC_BDSTAT_TX_WRAP|
			       QUICC_BDSTAT_TX_INT; /* Start the DMA */
		tty->statbits |= SENTXOFF;
	    }
	    tty->statbits &= ~SENDXOFF;
	    continue;
	}

	/*
	 * If this line has a special transmit service, get the characters
	 * from there.
	 */

	if (tty->txservice) {

	    len = (*(tty->txservice))(tty, console_tx_dmabuf, CONSOLE_DMASIZE);
	    if (len) {
		tty->totalout += len;
		txbd->buf_ptr = console_tx_dmabuf; /* Set DMA pointer */
		txbd->length = len; /* Set the length */
		txbd->status = QUICC_BDSTAT_TX_RDY|
		  	       QUICC_BDSTAT_TX_WRAP|
			       QUICC_BDSTAT_TX_INT; /* Start the DMA */
	    } else {
		(*(tty->txfinish))(tty);
	    }
	    continue;
	}

	/*
	 * By now, we are using the regular transmit service
	 */

	if (tty->outcount) {

	    register uchar *cp = tty->outhead + 1; /* Point to start of DMA */
	    if (cp == tty->outend) { /* Handle buffer wrapping */
		cp = tty->outstart;
	    }
	    len = tty->outcount; /* Get buffer length */
	    if ((cp + len) > tty->outend)
	      len = tty->outend - cp; /* Check for DMA wrapping */

	    tty->totalout += len; /* Account for characters output */
	    console_current_dma = len; /* Remember the size of this DMA */

	    txbd->buf_ptr = cp;	/* Set the DMA pointer */
	    txbd->length = len;	/* Set the length */
	    txbd->status = QUICC_BDSTAT_TX_RDY|
		  	   QUICC_BDSTAT_TX_WRAP|
			   QUICC_BDSTAT_TX_INT; /* Start the DMA */
	    continue;
	} else {
	    (*(tty->txfinish))(tty);
	    break;
	}		
    }
}

/*
 * smc_console_interrupt
 */

void smc_console_interrupt (void)
{
    tt_soc *tty;
    uchar c;
    int i;
    int len;

    tty = MODEMS[0];		/* Get the console pointer */
    if (!tty)
      return;			/* Strange - not quite set up yet */

    /* Avoid race conditions by acknowledging interrupts first */

    SMC1.smc_smce = QUICC_SMCE_TXBUF | QUICC_SMCE_RXBUF; 
				/* Ack the interrupt */

    while (!(rxbd->status & QUICC_BDSTAT_RX_EMPTY)) {

	if (rxbd->status & QUICC_BDSTAT_RX_OVRRUN)
	  tty->overrun++;	/* Post overrun errors */

	if (rxbd->status & QUICC_BDSTAT_RX_NOFA)
	  tty->linenoise++;	/* Count framing errors as noise */

#ifdef NOTYET
	if (rxbd->status & QUICC_BDSTAT_RX_BREAK)
	  mon_simbrk();		/* Break into the ROM mitor */
#endif /* NOTYET */

	/*
	 * We have handled the status bits, now handle the data
	 */

	len = rxbd->length;	/* Get the length of this receive */

	if (len) {

	    tty->totalin += len;	/* Account for data read */

	    /* If there is a special receive service, pass data to it */


	    if (tty->rxservice) {
		(*(tty->rxservice))(tty, console_rx_dmabuf, len);
	    } else {

		/* No special receive service, handle input data */

		for (i = 0; i < len; i++) {
		    c = console_rx_dmabuf[i];
		    if (special_chars(tty, c) >= 0) {
		
			if (tty->intail >= tty->inend)
			  tty->intail = tty->instart; /* Handle wraps */
		
			/* If we have used half of the input buffer,
			 * send an XOFF to try to stop the input.
			 */
		    
			if (tty->incount > (tty->insize >> 1)) {
			    if (tty->intail == tty->inhead) {
				tty->overrun++;
				continue;
			    }
			    tty->statbits |= SENDXOFF;

			    /* Note: we don't need to force an interrupt here,
			     * since we check receive processing before
			     * transmits, and will always try to do a
			     * transmit if the BD is available.
			     */

			}
			/* so there is room  */
			*tty->intail++ = c;
			++tty->incount;
		    }
		}
	    }
	}

	rxbd->status = QUICC_BDSTAT_RX_EMPTY|QUICC_BDSTAT_RX_WRAP|
	  	       QUICC_BDSTAT_RX_INT;
    }

    smc_transmit_service(tty);

    REGB->cpic_cisr = 1 << QUICC_CPIC_INTERRUPT_SMC1;
}

/*
 * smc_tx_disable - Disable transmitter
 *
 * At present, we don't implement anything here. The interrupt structure
 * of this driver is such that it isn't necessary to turn off the transmitter
 * (or transmit interrupts). This device interrupts on done, not on ready,
 * so there is no problem with constant ready interrupts when there is no
 * data to output.
 */

static void smc_tx_disable (tt_soc *tty)
{
}

/*
 * smc_startoutput - Start up pending output
 *
 * This routine is used to start up pending output. Because it is not
 * straightforward to force the SMC to interrupt (it interrupts on done,
 * not ready) we will disable interrupts here and simulate a transmit
 * interrupt.
 */

static boolean smc_startoutput (tt_soc *tty)
{
    leveltype SR;

    if (tty->statbits & (HOLDING | WANTXON))
      return(TRUE);		/* Nothing to do if output stopped */

    SR = raise_interrupt_level(TTY_DISABLE);

    smc_transmit_service(tty);	/* Fake an output interrupt */

    reset_interrupt_level(SR);

    return(TRUE);
}

/*
 * smc_clearoutput - Clear pending output
 *
 * This routine is used to clear pending output. We reset any DMA in
 * progress and then do the standard serial buffer accounting.
 */

static void smc_clearoutput (tt_soc *tty)
{
    leveltype status;

    status = raise_interrupt_level(TTY_DISABLE); /* Avoid race conditions */

    while (REGB->cp_cr & QUICC_CPCR_SEMA)
      ;				/* Wait for command register to be ready */

    REGB->cp_cr = (QUICC_CPCR_CHNUM(QUICC_CHAN_SMC1) |
		  QUICC_CPCR_OPCODE(QUICC_CPC_TX_STOP));
				/* Stop the transmitter, aborting DMA */

    console_current_dma = 0;	/* Forget about any DMA in progress */

    serial_clearoutput(tty);	/* Execute the common code */

    while (REGB->cp_cr & QUICC_CPCR_SEMA)
      ;				/* Wait for command register to be ready */

    REGB->cp_cr = (QUICC_CPCR_CHNUM(QUICC_CHAN_SMC1) |
		  QUICC_CPCR_OPCODE(QUICC_CPC_TX_RESTART));
				/* Restart the transmitter */


    reset_interrupt_level(status); /* Enable TTY interrupts */
}


/*
 * console_init_device - Device-level console initialization
 *
 * This routine is responsible for setting up the transmit and receive
 * buffer descriptors and other mandatory initialization. This is done
 * here so that it can be done very early in system startup if needed
 * for gdb, before the console tty is created.
 */

static void console_init_device (void)
{
    if (console_initialized)
      return;			/* Already done */

    /*
     * Get the transmit and receive buffer descriptor base addresses
     */

    rxbd = (quicc_bd_t *)(SMC1PARAM.rbase + ADRSPC_QUICC_DPRAM);
    txbd = (quicc_bd_t *)(SMC1PARAM.tbase + ADRSPC_QUICC_DPRAM);

    /*
     * Remember the transmit and receive buffer descriptors
     */

    saved_rxbd = *rxbd;
    saved_txbd = *txbd;
    saved_mrblr = SMC1PARAM.mrblr;
    saved_max_idl = SMC1PARAM.max_idl;

    /*
     * Set up our receive ring buffer
     */

    SMC1.smc_smcmr &= (~QUICC_SMCMR_RX_EN); /* Disable receiver */

    while (REGB->cp_cr & QUICC_CPCR_SEMA)
      ;				/* Wait for command register to be ready */

    REGB->cp_cr = (QUICC_CPCR_CHNUM(QUICC_CHAN_SMC1) |
		  QUICC_CPCR_OPCODE(QUICC_CPC_INIT_RX));
				/* Init RX parameter */

    rxbd->buf_ptr = console_rx_dmabuf; /* Set DMA pointer */
    rxbd->status = QUICC_BDSTAT_RX_EMPTY | QUICC_BDSTAT_RX_WRAP |
      		   QUICC_BDSTAT_RX_INT;

    SMC1PARAM.mrblr = CONSOLE_DMASIZE; /* Set buffer size */
    SMC1PARAM.max_idl = 1;	/* Set idle timeout to one character time */

    SMC1.smc_smcmr |= (QUICC_SMCMR_RX_EN|QUICC_SMCMR_TX_EN);
				/* Enable transmitter and receiver */

    console_initialized = TRUE;	/* Mark as initialized */
}
 

/*
 * console_putc - Output a character, polled
 *
 * This routine is used by gdb and other low-level parts of the system to
 * output a character while interrupts are disabled. We wait for the
 * transmit buffer descriptor to be free, and then set up a single byte
 * dma of the requested character.
 */

void console_putc (ch)
     uchar ch;
{
    leveltype SR;

    SR = raise_interrupt_level(TTY_DISABLE); /* Disable TTY interrupts */

    /*
     * Ensure the console is set up properly, if this is early in
     * system startup.
     */
    
    if (!console_initialized)
      console_init_device();	/* Do device initialization */

    while (txbd->status & QUICC_BDSTAT_TX_RDY)
      ;				/* Loop for buffer available */

    if (ch == '\n') {
	console_tx_dmabuf[0] = '\r'; /* Preface with CR */
	console_tx_dmabuf[1] = '\n'; /* ... */
	txbd->length = 2;	/* Set the length */
    } else {
	console_tx_dmabuf[0] = ch;	/* Set the character */
	txbd->length = 1;	/* Set the length */
    }
    txbd->buf_ptr = console_tx_dmabuf; /* Set DMA pointer */
    txbd->status = QUICC_BDSTAT_TX_RDY|
		   QUICC_BDSTAT_TX_WRAP|
		   QUICC_BDSTAT_TX_INT; /* Start the DMA */

    reset_interrupt_level(SR);
}

/*
 * console_getc - Get a character, polled
 *
 * This routine is used to get a character, usually for gdb. We will
 * spin in a loop with interrupts disabled until a character is ready for
 * input. We also maintain an overflow buffer to deal with high-speed
 * input, and provide characters out of our overflow buffer if they
 * are available there.
 */

int console_getc (void)
{
    uchar ch;
    int i;
    int len;
    leveltype SR;

    SR = raise_interrupt_level(TTY_DISABLE); /* Disable TTY interrupts */

    if (!console_initialized)
      console_init_device();	/* Ensure console is working */

    /*
     * See if there is data in the overflow buffer to return
     */

    if (polled_overflow_empty != polled_overflow_fill) {
	ch = overflow_input[polled_overflow_empty++]; /* Get a byte */
    } else {

	/*
	 * Nothing in the overflow area now, so wait until we get something
	 * from the receive buffer descriptor.
	 */

	polled_overflow_empty = 0; /* Reset empty pointer */
	polled_overflow_fill = 0; /* Reset fill pointer */

	do {
	    while (rxbd->status & QUICC_BDSTAT_RX_EMPTY)
	      ;			/* Loop until we get something */
	   
	    len = rxbd->length;	/* Get the length of this receive */
	    ch = console_rx_dmabuf[0]; /* Remember the first byte */

	    /*
	     * If we got more than one byte, we save the extra in the
	     * overflow area.
	     */
	    
	    for (i = 1; i < len; i++) {
		if (polled_overflow_fill > POLLED_OVERFLOW_SIZE)
		  break;		/* Get out if buffer over full */
		overflow_input[polled_overflow_fill++] = console_rx_dmabuf[i];
		/* Store character in buffer */
	    }
	    
	    rxbd->status = QUICC_BDSTAT_RX_EMPTY | QUICC_BDSTAT_RX_WRAP |
	      QUICC_BDSTAT_RX_INT;	/* Reset the buffer status */
	} while (len == 0);
    }

    reset_interrupt_level(SR);	/* Restore interrupts */
    return(ch & 0x7f);		/* Return the character */
}

/*
 * console_init - Set up the SMC as system console
 */

tt_soc *console_init ()
{
    register vectortype *vector;
    register tt_soc *tty;
    leveltype SR;

    SR = raise_interrupt_level(TTY_DISABLE);

    vector = &console_vector;
    vector_init(vector);
    vector->vgetc = serial_getc;
    vector->vclearinput = serial_clearinput; 
    vector->vinputpending = serial_inputpending;
    vector->vputc = serial_putc;
    vector->vstartoutput = smc_startoutput;
    vector->vclearoutput = smc_clearoutput;
    vector->voutputpending = serial_outputpending;
    vector->voutputblock = serial_outputblock;
    vector->voutputxoffed = serial_outputxoffed;
    vector->vputpkt = generic_putpkt;
    vector->vservicehook = generic_servicehook;
    vector->vnoservicehook = generic_noservicehook;
    vector->vstopoutput = smc_tx_disable;

    tty = (tt_soc *) tty_init(0,READY,CTY_LINE,vector);
    if (!tty) {
	reset_interrupt_level(SR);
	return(NULL);
    }
    serial_setbuffers(tty, MAXINCHARS, MAXOUTCHARS);
    tty->txfinish = smc_tx_disable;
    /*
     * This may be a lie (the rom monitor has control), but initialize it
     * to the default anyway so that NV generation doesn't get confused.
     */
    tty->tty_stopbits = STOPBITS2;

    console_init_device();	/* Do device initialization */

    /*
     * Set up interrupt handling
     */

    SMC1.smc_smcm = QUICC_SMCM_TXBUF | QUICC_SMCM_RXBUF; /* Set mask */
    SMC1.smc_smce = 0xff;	/* Clear any residual interrupt sources */

    REGB->cpic_cimr = 1 << QUICC_CPIC_INTERRUPT_SMC1;

    quicc_install_cpm_exception(DPR, smc_int, QUICC_CPIC_INTERRUPT_SMC1);

    reset_interrupt_level(SR);
    return(tty);
}
