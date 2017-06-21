/* $Id: console_36.c,v 3.2.58.1 1996/03/21 23:28:28 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/rommon/src-36/console_36.c,v $
 *------------------------------------------------------------------
 * console_36.c
 *
 * October 1994, Rob Clevenger
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Console I/O routines for the Sapphire.
 *------------------------------------------------------------------
 * $Log: console_36.c,v $
 * Revision 3.2.58.1  1996/03/21  23:28:28  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.2.2.1  1995/12/01  04:21:37  mbeesley
 * Branch: ELC_branch
 * Reorganize Rom monitor header files to make it easier to port
 * to new platforms.
 *
 * Revision 3.2  1995/11/17  18:43:32  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:07:02  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:23:56  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/06/07  22:38:49  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
** Console routines.
*/
#include "monitor.h"
#include "console_36.h"
#include "mon_defs.h"
#include "signal_4k.h"
#include "error.h"
#include "quicc.h"
#include "confreg.h"
#include "extern.h"

unsigned short uart_speeds[] = {
    BAUD9600,
    BAUD4800,
    BAUD1200,
    BAUD2400,
};


/*
** Raw console output routine.
*/
void
conout(char c)
{
    quicc_bd_t *bd;
    char txbuf;

    bd = &((quicc_bd_t *)(((quicc_dpr_t *)ADRSPC_QUICC_DPRAM)->user_data_1))[MON_CON_TX_BD_IDX];

    while ( bd->status & QUICC_BDSTAT_TX_RDY )
	;  /* wait for ready (negative logic?) */

    txbuf = c;
    bd->buf_ptr = &txbuf;
    bd->length = 1;
    bd->status = QUICC_BDSTAT_TX_RDY | QUICC_BDSTAT_TX_WRAP;
    
    /* wait for character to flush */
    while ( bd->status & QUICC_BDSTAT_TX_RDY )
	;  /* wait for ready (negative logic?) */
}

/*
** Basic string output routine
** Use when there is no main memory or in the event
** of catastrophie
*/
void
basic_strout(char *cp)
{
    while(*cp) conout(*cp++);
}

static char console_rxbuf;

/*
** Raw console input routine.
*/
int
conin(void)
{
    quicc_bd_t *bd;
    int cc;

    bd = &((quicc_bd_t *)(((quicc_dpr_t *)ADRSPC_QUICC_DPRAM)->user_data_1))[MON_CON_RX_BD_IDX];

    if(bd->status & QUICC_BDSTAT_RX_EMPTY)
    	return(-1);		/* empty (no char) */

    cc = console_rxbuf;
    bd->buf_ptr = &console_rxbuf;
    bd->length = 0;
    bd->status = QUICC_BDSTAT_RX_EMPTY | QUICC_BDSTAT_RX_WRAP ;
    return(cc);
}

/*
** Console status routine.
** Returns true (1) if there is a character to be had, false (0) otherwise.
*/
int
constat(void)
{
    volatile quicc_bd_t *bd;

    bd = &((quicc_bd_t *)(((quicc_dpr_t *)ADRSPC_QUICC_DPRAM)->user_data_1))[MON_CON_RX_BD_IDX];

    if(bd->status & QUICC_BDSTAT_RX_EMPTY)
    	return(0);  /* no character */

    return(1);	/* have char */
}

/*
** Return a character from the console device.
** This routine blocks if the input queue is empty.
** This routine throws away break chars.
*/
char
getchar(void)
{
    register char c;

    while((c = conin()) < 0 || c == 0); /* ignore breaks */
    return(c & 0177);	/* strip parity */
}

/*
** Put a character on the console device.
** Keep track of the char count on line (linepos) for the clrline()
** routine.
** Also supply the "more" (pagination) mechanism.
*/
#define MAXLINES 22
short linepos, linecnt;

void
putchar(char c)
{
    conout(c);  /* write to console */
    switch(c) {
    case '\r':
	linepos = 0;
	break;
    case '\b': break;  /* do not change linepos */
    case '\n':
	conout('\r'); /* expand newlines */
	linepos = 0;
	if(hkeepflags & H_MORE && ++linecnt >= MAXLINES) {
	    linecnt = 0;
	    puts("*** MORE ***  ");
	    c = getchar();
	    clrline("\0");
	    if(c == 'q' && monjmpptr)
		longjmp(monjmpptr, 3);  /* quiet termination */
	    if(c != ' ')
		linecnt = MAXLINES - 1;
	}
	break;
    default:
	linepos++;
	break;
    }
}

void
setmore(void)
{
    hkeepflags |= H_MORE;
    linecnt = 0;
}

/*
** Initialize SMC1 for UART mode
*/
static char console_txbuf;

void
init_con(int baud_index)
{
    quicc_dpr_t *dpram_p = (quicc_dpr_t *)ADRSPC_QUICC_DPRAM;
    quicc_internal_regs_t *qir_p = &dpram_p->regs;
    volatile quicc_smc_uart_param_t *param_p =
	(quicc_smc_uart_param_t *)&dpram_p->smc1;
    volatile quicc_bd_t *bd;

    /* Disable transmitter and receiver just in case */
    qir_p->smc_regs[CONSOLE_SMC_IDX].smc_smcmr = 0;
    /* turn off DSR while we do this */
    qir_p->pio_pcdat |= SAP_QU_PT_C_PAR_CON_DSR;   /* (neg logic) */

    /* set up baud rate generator */
    qir_p->brg_brgc3 = QUICC_BRGC_RESET;
    qir_p->brg_brgc3 = QUICC_BRGC_EN_CLK | QUICC_BRGC_CLK_SRC(0) | /* internal clock */
		QUICC_BRGC_CLK_DIV(uart_speeds[baud_index]);    /* baud rate */

    qir_p->si_simode = QUICC_SIMODE_SMC1_CS( QUICC_SIMODE_SMC_CS_BRG3 );

    qir_p->smc_regs[CONSOLE_SMC_IDX].smc_smcmr =  /* uart mode, 8 data, no parity, one stop */
	QUICC_SMCMR_CHAR_LEN9 | QUICC_SMCMR_UART;

    param_p->max_idl = 0;

    /* get ptr to user_data_1 as if DPRAM was at zero */
    bd = (quicc_bd_t *)(&(((quicc_dpr_t *)(0))->user_data_1));

    /* now calculate pointers to our buffer descriptors */
    param_p->rbase = (int)(&bd[MON_CON_RX_BD_IDX]);
    param_p->tbase = (int)(&bd[MON_CON_TX_BD_IDX]);
    param_p->rfcr = QUICC_SMCFC_MOT;	/* big-endian, dma space */
    param_p->tfcr = QUICC_SMCFC_MOT;	/* big-endian, dma space */
    param_p->mrblr = 1;			/* one char at a time	*/

    /* init buffer descriptors */
    bd = &((quicc_bd_t *)(dpram_p->user_data_1))[MON_CON_TX_BD_IDX];
    bd->buf_ptr = &console_txbuf;
    bd->length = 1;
    bd->status = QUICC_BDSTAT_TX_WRAP;

    bd = &((quicc_bd_t *)(dpram_p->user_data_1))[MON_CON_RX_BD_IDX];
    bd->buf_ptr = &console_rxbuf;
    bd->length = 0;
    bd->status = QUICC_BDSTAT_RX_EMPTY | QUICC_BDSTAT_RX_WRAP;

    quicc_cr_cmd(QUICC_CHAN_SMC1, QUICC_CPC_INIT_RXTX);

    qir_p->smc_regs[CONSOLE_SMC_IDX].smc_smcmr |=
	QUICC_SMCMR_TX_EN | QUICC_SMCMR_RX_EN;	/* enable transmit and
						   receive */
    /* console initialization is now complete, turn on DSR */
    /* this comment was put here to make enf happy */
    qir_p->pio_pcdat &= ~SAP_QU_PT_C_PAR_CON_DSR;   /* DSR on (neg logic) */
}

/*
** Handy routine for calling from assembler.
*/
void
re_init_con(void)
{
    int i;
    /*
    **  FIXME - make sure transmitter is empty
    */
    for(i=0; i<0x10000; i++) ;
    init_con(((NVRAM)->confreg & CONSOLE_MASK) >> CONSOLE_SHIFT);
}


/*
** When returning from a system image the monitor must take over the
** console port while remembering the state the system had set up so
** that it can be restored later if desired.
*/
static quicc_bd_t *rxbd;        /* Address of rx buffer descriptor start */
static quicc_bd_t *txbd;        /* Address of tx buffer descriptor start */
static quicc_bd_t saved_rxbd;   /* Saved receive buffer descriptor */
static quicc_bd_t saved_txbd;   /* Saved transmit buffer descriptor */
static int saved_mrblr;         /* Saved mrblr value */
static int saved_max_idl;       /* Saved max_idl value */
static char sys_console_state_valid;
static unsigned short saved_pitr;
static unsigned short saved_picr;
static unsigned char saved_avr;

void
save_sys_console_state (void)
{
    quicc_dpr_t *dpram_p = (quicc_dpr_t *)ADRSPC_QUICC_DPRAM;
    quicc_internal_regs_t *qir_p = &dpram_p->regs;
    volatile quicc_smc_uart_param_t *param_p =
	(quicc_smc_uart_param_t *)&dpram_p->smc1;
    int i;


    /* do this only if a system image has been launched */
    if(sys_state == MONITOR_RUNNING) return;

    /*
    ** Get the transmit and receive buffer descriptor base addresses
    ** from parameter ram and turn them into useful pointers.
    */
    rxbd = (quicc_bd_t *)((long)param_p->rbase + ADRSPC_QUICC_DPRAM);
    txbd = (quicc_bd_t *)((long)param_p->tbase + ADRSPC_QUICC_DPRAM);

    /*
    ** Remember the transmit and receive buffer descriptors
    */
    saved_rxbd = *rxbd;
    saved_txbd = *txbd;

    /*
    ** Remember the PIT stuff too
    */
    saved_pitr = qir_p->sim_pitr;
    saved_picr = qir_p->sim_picr;
    saved_avr = qir_p->sim_avr;

    /*
    ** Remember the maximum buffer length and idle timeout
    */
    saved_mrblr = param_p->mrblr;
    saved_max_idl = param_p->max_idl;

    sys_console_state_valid = 1;

    /*
    ** Make sure the timer is running.
    ** This step is important as it also disables the processing
    ** of multiple user interrupts (<break> keystrokes), allowing
    ** the monitor to pass the HammerBreak (tm) test.
    */
    timer_enable();

    /*
    ** Re-init console port for monitor use.
    */
    re_init_con();
}

void
restore_sys_console_state (void)
{
    quicc_dpr_t *dpram_p = (quicc_dpr_t *)ADRSPC_QUICC_DPRAM;
    quicc_internal_regs_t *qir_p = &dpram_p->regs;
    volatile quicc_smc_uart_param_t *param_p =
	(quicc_smc_uart_param_t *)&dpram_p->smc1;


    /* do this only if the saved state is valid */
    if(!sys_console_state_valid) return;

    /*
    ** No need to wait for I/O to complete here since the
    ** monitor console driver already does this.
    */

     /* Disable receiver */
    qir_p->smc_regs[CONSOLE_SMC_IDX].smc_smcmr &= (~QUICC_SMCMR_RX_EN);

    quicc_cr_cmd(QUICC_CHAN_SMC1, QUICC_CPC_INIT_RX);

    *txbd = saved_txbd;               /* Restore original TX buffer desc */
    *rxbd = saved_rxbd;               /* Restore original RX buffer desc */
    param_p->mrblr = saved_mrblr;     /* Restore saved maximum buffer length */
    param_p->max_idl = saved_max_idl; /* Restore saved maximum idle length */

    /* restore the bd pointers */
    param_p->rbase = ((int)rxbd - ADRSPC_QUICC_DPRAM);
    param_p->tbase = ((int)txbd - ADRSPC_QUICC_DPRAM);

    /* Enable transmitter and receiver */
    qir_p->smc_regs[CONSOLE_SMC_IDX].smc_smcmr |= 
	(QUICC_SMCMR_RX_EN | QUICC_SMCMR_TX_EN);

    /* restore the PIT stuff too */
    qir_p->sim_pitr = saved_pitr;
    qir_p->sim_picr = saved_picr;
    qir_p->sim_avr = saved_avr;

    sys_console_state_valid = 0;
}

char *auxstr = "\nSorry, there is no aux port on this platform\n";

void
initaux(void)
{
    puts(auxstr);
}

int
auxstat(void)
{
    puts(auxstr);
}

int
auxin(void)
{
    puts(auxstr);
}

void
auxout(int c)
{
    puts(auxstr);
}

/* end of module */
