/* $Id: quicc_lib.c,v 3.2.62.3 1996/06/25 17:31:53 snyder Exp $
 * $Source: /release/112/cvs/Xsys/src-36/quicc_lib.c,v $
 *------------------------------------------------------------------
 * Library routines specific to the QUICC chip.
 *
 * November 1994, Andrew G. Harvey
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: quicc_lib.c,v $
 * Revision 3.2.62.3  1996/06/25  17:31:53  snyder
 * CSCdi61230:  move format string out of #define, save code space
 * Branch: California_branch
 *              Instead of 4 copies of this text, how about just 1, saves
 *              ~200 bytes
 *
 * Revision 3.2.62.2  1996/06/12  19:56:49  jng
 * CSCdi60260:  Async catwoman commit
 * Branch: California_branch
 * Async Catwoman commit into California
 *
 * Revision 3.2.62.1  1996/03/18  21:56:42  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.2  1996/03/09  05:11:18  hampton
 * Slim down the kernel of the router.
 * Branch: DeadKingOnAThrone_branch
 *
 * Revision 3.2.26.1  1996/01/24  22:49:18  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.2  1995/11/17  18:39:31  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:19:21  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:26:27  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/06/07  22:52:03  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../src-36/quicc.h"
#include "../src-36/quicc_lib.h"
#include "stacks.h"

static ushort quicc_rxbd_offset(ushort);
static ushort quicc_txbd_offset(ushort);
static void quicc_set_spi_pins(quicc_dpr_t *);

const char assertion_failed_str[] = "\nASSERTION FAILED: file \"%s\", line %d";

boolean cpic_interrupt_started;	/* TRUE when CPIC stack created */
/*
 * SCC CPIC interrupt masks.
 */
const ulong quicc_scc_intr_mask[] = {
    0x40000000,		/* SCC1 CISR Mask */
    0x20000000,		/* SCC2 CISR Mask */
    0x10000000,		/* SCC3 CISR Mask */
    0x08000000		/* SCC4 CISR Mask */
    };

/*
 * =================================================================
 * Name: quicc_install_exception
 *
 * Description:
 *      Installs an exception handler in the vector table.
 *
 * Parameters:
 *      void (*func)(void) - pointer to the exception handler.
 *      ushort number -  exception vector number.
 *
 * Returns:
 *      void - none.
 * =================================================================
 */

static void quicc_install_exception (void (*func)(void), ushort number)
{
    QASSERT(func != NULL);
    QASSERT((number > 0) && (number < 0x400));
    
    /*
     * Install an interrupt vector in the exception table.
     */
    (getvbr())[number] = func;
}

/*
 * =================================================================
 * Name: quicc_install_cpm_exception
 *
 * Description:
 *      Installs an exception handler in the vector table.
 *
 * Parameters:
 *      quicc_dpr_t * - pointer to dualport ram.
 *      void (*)(void) - pointer to the exception handler.
 *      ushort number - lower 5 bits of the exception vector number.
 *
 * Returns:
 *      void - none.
 * =================================================================
 */

void quicc_install_cpm_exception (quicc_dpr_t *dpr,
                                  void (*func)(void),
                                  ushort number)
{
    ushort disp;

    QASSERT(func != NULL);
    QASSERT((number > 0) && (number < 0x20));
    /*
     * Set up the interrupt stack if it has not yet been done
     */

    if (!cpic_interrupt_started) {

	dpr->regs.sdma_sdcr = 0x740;	/* See MC68030 errata */
	
	dpr->regs.cpic_cicr = (QUICC_CICR_IRL(LEVEL_CPIC) |
			       QUICC_CICR_HP(QUICC_CPIC_INTERRUPT_SMC1) |
			       CPIC_INTERRUPT_BASE);

	dpr->regs.cpic_cicr |= (QUICC_CICR_SCaP(QUICC_PORT_SCC3) |
				QUICC_CICR_SCbP(QUICC_PORT_SCC2) |
				QUICC_CICR_SCcP(QUICC_PORT_SCC4) |
				QUICC_CICR_SCdP(QUICC_PORT_SCC1));

	createlevel(LEVEL_CPIC, NULL, "CPIC interrupts");	

	cpic_interrupt_started = TRUE; /* Once only please */
    }

    /*
     * Calculate the vector offset
     */

    disp = (CPIC_INTERRUPT_BASE + number);

    quicc_install_exception(func, disp);
}


/*
 * =================================================================
 * Name: quicc_install_avec_exception
 *
 * Description:
 *
 * Parameters:
 *      void (*)(void) - pointer to the exception handler.
 *	uchar - auto-vector bit flag.
 * Returns:
 *	void - None.
 * =================================================================
 */
void quicc_install_avec_exception (void (*func)(void), uchar bit,
				   boolean enable)
{
    ushort number;
    
    QASSERT((bit >= QUICC_AVR0) && (bit <= QUICC_AVR7));

    switch (bit) {
    default:
	QASSERT(FALSE);
	return;
    case QUICC_AVR1:
	number = QUICC_AVEC1;
	break;
    case QUICC_AVR2:
	number = QUICC_AVEC2;
	break;
    case QUICC_AVR3:
	number = QUICC_AVEC3;
	break;
    case QUICC_AVR4:
	number = QUICC_AVEC4;
	break;
    case QUICC_AVR5:
	number = QUICC_AVEC5;
	break;
    case QUICC_AVR6:
	number = QUICC_AVEC6;
	break;
    case QUICC_AVR7:
	number = QUICC_AVEC7;
	break;
    }
	
    quicc_install_exception(func, number);

    if (enable == TRUE)
	quicc_avec_enable(bit);
    else
	quicc_avec_disable(bit);
}


/*
 * =================================================================
 * Name: quicc_avec_enable
 *
 * Description:
 *	Enable auto-vectoring via the SIM60 AVEC register.
 *
 * Parameters:
 *	uchar - which bit to enable.
 *
 * Returns:
 *	void - None.
 * =================================================================
 */
void quicc_avec_enable (uchar bit)
{
    quicc_dpr_t *dpr;

    QASSERT((bit >= QUICC_AVR0) && (bit <= QUICC_AVR7));

    /*
     * Turn on bit in SIM auto-vector register.
     */
    dpr = (quicc_dpr_t *)getmbar();
    
    dpr->regs.sim_avr |= bit;
}


/*
 * =================================================================
 * Name: quicc_avec_disable
 *
 * Description:
 *	Disable auto-vectoring via the SIM60 AVEC register.
 *
 * Parameters:
 *	uchar - autovector bit to clear
 *
 * Returns:
 *	void - None.
 * =================================================================
 */
void quicc_avec_disable (uchar bit)
{
    quicc_dpr_t *dpr;

    QASSERT((bit >= QUICC_AVR0) && (bit <= QUICC_AVR7));

    /*
     * Turn on bit in SIM auto-vector register.
     */
    dpr = (quicc_dpr_t *)getmbar();
    
    dpr->regs.sim_avr &= ~bit;
}
	

/*
 * =================================================================
 * Name: quicc_pit_init
 *
 * Description:
 *      This routine will kick off the periodic interrupt timer.
 *
 * Parameters:
 *      Pointer to dual port ram.
 *      Delta in ms.
 *      Routine to invoke.
 *
 * Returns:
 *      TRUE if the timer was started.  False if the timer is
 *      already running.
 * =================================================================
 */

boolean quicc_pit_init (quicc_dpr_t *dpr, ushort delta, void (*pit)(void), 
                        ushort vector_num)
{
    static boolean pit_running = FALSE;
    ushort count;

    QASSERT((vector_num >= QUICC_AVEC1) && (vector_num < VECTORCOUNT));

    if (pit_running == TRUE)
        return(FALSE);

    pit_running = TRUE;

    if (pit)
      quicc_install_exception(pit, vector_num);

    /*
     * The PIT timer is calebrated into mseconds by applying the
     * formular on Page 6-11 QUICC Users Manual.  The Values are:
     *
     *  EXTAL Frequency = 4MHz
     *  Prescale Value = 1 (No Prescale)
     *  Divider = 128
     *
     *  PIT Period = PITR Count Value / (4000000/512)
     *  PIT Count = PIT Period * (4000000/512)
     */
    count =  (delta * PIT_XTAL_FREQ) / (PIT_XTAL_DIVIDER * 1000);

    /*
     * Install the periodic function timer and start
     * it off.
     */
    dpr->regs.sim_picr = QUICC_PICR_PIV(vector_num) |
                         QUICC_PICR_PIRQL(QUICC_IPL_LEVEL7);
    dpr->regs.sim_pitr = QUICC_PITR_PITR(count) |
			 (~QUICC_PITR_PITR_M & dpr->regs.sim_pitr);

    return(TRUE);
}

/******************************************************************************/
/*                    QUICC Library Timer Routines                            */
/******************************************************************************/



/*
 * =================================================================
 * Name: quicc_start_timer
 *
 * Description:
 *      Starts one of the 4 QUICC hardware timers.
 *
 * Parameters:
 *      The timer to be started.
 *
 * Returns:
 *      void - none
 * =================================================================
 */

void quicc_start_timer (quicc_timer_t which_timer)
{
    quicc_dpr_t *dpr;

    dpr = (quicc_dpr_t *) getmbar();

    switch (which_timer) {
        case QUICC_TMR_1:
            dpr->regs.tmr_tgcr |= QUICC_TGCR_TMR1_EN;
            dpr->regs.cpic_cimr |= QUICC_CIMR_TIMER1;
            break;
        case QUICC_TMR_2:
            dpr->regs.tmr_tgcr |= QUICC_TGCR_TMR2_EN;
            dpr->regs.cpic_cimr |= QUICC_CIMR_TIMER2;
            break;
        case QUICC_TMR_3:
            dpr->regs.tmr_tgcr |= QUICC_TGCR_TMR3_EN;
            dpr->regs.cpic_cimr |= QUICC_CIMR_TIMER3;
            break;
        case QUICC_TMR_4:
            dpr->regs.tmr_tgcr |= QUICC_TGCR_TMR4_EN;
            dpr->regs.cpic_cimr |= QUICC_CIMR_TIMER4;
            break;
        default:
            /* Invalid timer */
            QASSERT(FALSE);
            break;
    }
}


/*
 * =================================================================
 * Name: quicc_stop_timer
 *
 * Description:
 *      Stops one of the 4 QUICC hardware timers.
 *
 * Parameters:
 *      The timer to be stopped.
 *
 * Returns:
 *      void - none
 * =================================================================
 */

void quicc_stop_timer (quicc_timer_t which_timer)
{
    quicc_dpr_t *dpr;

    dpr = (quicc_dpr_t *) getmbar();

    switch (which_timer) {
        case QUICC_TMR_1:
            dpr->regs.cpic_cimr &= ~QUICC_CIMR_TIMER1;
            dpr->regs.tmr_tgcr &= ~QUICC_TGCR_TMR1_EN;
            break;
        case QUICC_TMR_2:
            dpr->regs.cpic_cimr &= ~QUICC_CIMR_TIMER2;
            dpr->regs.tmr_tgcr &= ~QUICC_TGCR_TMR2_EN;
            break;
        case QUICC_TMR_3:
            dpr->regs.cpic_cimr &= ~QUICC_CIMR_TIMER3;
            dpr->regs.tmr_tgcr &= ~QUICC_TGCR_TMR3_EN;
            break;
        case QUICC_TMR_4:
            dpr->regs.cpic_cimr &= ~QUICC_CIMR_TIMER4;
            dpr->regs.tmr_tgcr &= ~QUICC_TGCR_TMR4_EN;
            break;
        default:
            /* Invalid timer */
            QASSERT(FALSE);
            break;
    }
}


/*
 * =================================================================
 * Name: quicc_init_timer
 *
 * Description:
 *      Initializes one of the 4 QUICC hardware timers.
 *
 * Parameters:
 *      The timer to be initialized.
 *      The pre-scale value to be set in the Timer Mode Register.
 *      The clock source to be set in the Timer Mode Register.
 *      The time-out value to be set in the Timer Reference Register.
 *
 * Returns:
 *      void - none
 * =================================================================
 */

void quicc_init_timer (quicc_timer_t which_timer,
                       ushort  prescale,
                       boolean  restart_mode,
                       ushort clock_src,
                       ushort  time)
{
    quicc_dpr_t *dpr;
    ushort tmr_mode_reg;  /* Timer mode register image */

    dpr = (quicc_dpr_t *) getmbar();
    quicc_stop_timer(which_timer); /* Reset timer in Timer Global Config Reg */

    /* Build timer mode register image */
    tmr_mode_reg = prescale | QUICC_TMR_INT_EN | clock_src;
    if (restart_mode)
        tmr_mode_reg |= QUICC_TMR_RESET_MODE;

    switch (which_timer) {
        case QUICC_TMR_1:
            dpr->regs.tmr_tmr1 = tmr_mode_reg; /* Set the mode reg */
            dpr->regs.tmr_tcn1 = 0;            /* Clear the count */
            dpr->regs.tmr_trr1 = time;         /* Set timeout reference value */
            /* Clear any stale events */
            dpr->regs.tmr_ter1 = QUICC_TER_CAP_EVENT | QUICC_TER_REF_EVENT;
            break;
        case QUICC_TMR_2:
            dpr->regs.tmr_tmr2 = tmr_mode_reg; /* Set the mode reg */
            dpr->regs.tmr_tcn2 = 0;            /* Clear the count */
            dpr->regs.tmr_trr2 = time;         /* Set timeout reference value */
            /* Clear any stale events */
            dpr->regs.tmr_ter2 = QUICC_TER_CAP_EVENT | QUICC_TER_REF_EVENT;
            break;
        case QUICC_TMR_3:
            dpr->regs.tmr_tmr3 = tmr_mode_reg; /* Set the mode reg */
            dpr->regs.tmr_tcn3 = 0;            /* Clear the count */
            dpr->regs.tmr_trr3 = time;         /* Set timeout reference value */
            /* Clear any stale events */
            dpr->regs.tmr_ter3 = QUICC_TER_CAP_EVENT | QUICC_TER_REF_EVENT;
            break;
        case QUICC_TMR_4:
            dpr->regs.tmr_tmr4 = tmr_mode_reg; /* Set the mode reg */
            dpr->regs.tmr_tcn4 = 0;            /* Clear the count */
            dpr->regs.tmr_trr4 = time;         /* Set timeout reference value */
            /* Clear any stale events */
            dpr->regs.tmr_ter4 = QUICC_TER_CAP_EVENT | QUICC_TER_REF_EVENT;
            break;
        default:
            /* Invalid timer */
            QASSERT(FALSE);
            break;
    }
}


/*
 * =================================================================
 * Name: qassert
 *
 * Description:
 *	Crash and burn.  This routine is called when an ASSERTION
 *	fails.
 *
 * Parameters:
 *	char * - source filename.
 *	ulong - line number of ASSERTION.
 *
 * Returns:
 * =================================================================
 */
void qassert (char *file, ulong line)
{
    printf ("ASSERTION FAILED: file \"%s\", line %d\n", file, line);
    flush();
    crashdump(10);
}


/*
 * =================================================================
 * Name: quicc_version_number
 *
 * Description:
 *	This routine returns the Microcode Revision Number
 *
 * Parameters:
 *	quicc_dpr_t * - pointer to the dualport ram.
 *
 * Returns:
 *	ushort - microcode revision number
 * =================================================================
 */
ushort quicc_version_number (quicc_dpr_t *dpr)
{
    QASSERT(dpr != NULL);
    
    return(dpr->misc.rev_num);
}


/*
 * =================================================================
 * Name: quicc_port_to_channel
 *
 * Description:
 *	A unique port number has been assigned to each device that may
 *	send commands to the risc via the quicc_send_risc_cmd.
 *	Unfortunatly, the channel numbers required by the risc do not
 *	map in a logical way.  This routine maps the port numbers used
 *	by the library into the channel numbers required by the risc
 *	processor.  The channel value returned is garrenteed to be
 *	valid.
 *
 * Parameters:
 *	ushort - port number.
 *
 * Returns:
 *	ushort - channel number.
 * =================================================================
 */
ushort quicc_port_to_channel (ushort port)
{
    ushort channel;

#if QUICC_PORT_SCC1 == 0
    QASSERT(port <= QUICC_PORT_TIMER);
#else
    QASSERT((port >= QUICC_PORT_SCC1) && (port <= QUICC_PORT_TIMER));
#endif
    
    switch (port) {
    default:		/* Should not get here */
    case QUICC_PORT_SCC1:
	channel = QUICC_CHAN_SCC1;
	break;
    case QUICC_PORT_SCC2:
	channel = QUICC_CHAN_SCC2;
	break;
    case QUICC_PORT_SCC3:
	channel = QUICC_CHAN_SCC3;
	break;
    case QUICC_PORT_SCC4:
	channel = QUICC_CHAN_SCC4;
	break;
    case QUICC_PORT_SMC1:
	channel = QUICC_CHAN_SMC1;
	break;
    case QUICC_PORT_SMC2:
	channel = QUICC_CHAN_SMC2;
	break;
    case QUICC_PORT_TIMER:
	channel = QUICC_CHAN_TIMER;
	break;
    case QUICC_PORT_SPI:
	channel = QUICC_CHAN_SPI;
	break;
    }
    return(channel);
}


/*
 * =================================================================
 * Name: quicc_send_risc_cmd
 *
 * Description:
 *	This routine is used to send a command to the risc processor.
 *
 * Parameters:
 *	quicc_dpr_t * - pointer to dualport ram.
 *	ushort - port number.
 *	ushort - command to issue.
 *
 * Returns:
 *	boolean - TRUE if the command was sent.
 *	boolean - FALSE if the risc was unavailable.
 * =================================================================
 */
boolean quicc_send_risc_cmd (quicc_dpr_t *dpr, ushort port,
				    ushort opcode)
{
    volatile ushort *command;
    ushort cmd;
    ushort channel;
    ushort count;

    channel = quicc_port_to_channel(port);

    cmd = QUICC_CPCR_CHNUM(channel) | QUICC_CPCR_OPCODE(opcode);

    command = &dpr->regs.cp_cr;
    
    /*
     * Check the command semaphore flag to see if the risc
     * is busy.  If so try again for a little while and then
     * give up.
     */
    for (count = 0; count < QUICC_CPC_SPIN_MAX; count++) {
	if (((*command) & QUICC_CPCR_SEMA) == 0) {
	    *command = (cmd | QUICC_CPCR_SEMA);
	    return(TRUE);
	}
    }
    return(FALSE);
}


/*
 * =================================================================
 * Name: quicc_scc_enable_rx
 *
 * Description:
 *	This routine will enable the scc receiver.
 *
 * Parameters:
 *	quicc_dpr_t * - pointer to dualport ram.
 *	ushort - port number to enable.
 *
 * Returns:
 *	void - None.
 * =================================================================
 */

void quicc_scc_enable_rx (quicc_dpr_t *dpr, quicc_info_t *qinfo)
{
    ushort port = qinfo->port_no;
    
#if QUICC_PORT_SCC1 == 0
    QASSERT(port <= QUICC_PORT_SCC4);
#else
    QASSERT((port >= QUICC_PORT_SCC1) && (port <= QUICC_PORT_SCC4));
#endif

    dpr->regs.scc[port].scc_sccm |= qinfo->rx_enable_sccm_mask;
    dpr->regs.scc[port].scc_gsmrl |= QUICC_SCC_GSMRL_ENR;
}


/*
 * =================================================================
 * Name: quicc_scc_disable_rx
 *
 * Description:
 *	This routine will disable the scc receiver.
 *
 * Parameters:
 *	quicc_dpr_t * - pointer to dualport ram.
 *	ushort - port number to disable.
 *
 * Returns:
 *	void - None.
 * =================================================================
 */

void quicc_scc_disable_rx (quicc_dpr_t *dpr, quicc_info_t *qinfo)
{
    ushort port = qinfo->port_no;
    
#if QUICC_PORT_SCC1 == 0
    QASSERT(port <= QUICC_PORT_SCC4);
#else
    QASSERT((port >= QUICC_PORT_SCC1) && (port <= QUICC_PORT_SCC4));
#endif

    dpr->regs.scc[port].scc_gsmrl &= ~QUICC_SCC_GSMRL_ENR;

    dpr->regs.scc[port].scc_sccm &= ~(qinfo->rx_disable_sccm_mask);
    /*
       SCC Event Register:
       A bit is cleared by writing a one;
       writing a zero does not affect a bit's value.
       operator "|=" will clear all scce bits.
     */
    dpr->regs.scc[port].scc_scce = qinfo->rx_clear_scce_event;

}



/*
 * =================================================================
 * Name: quicc_scc_enable_tx
 *
 * Description:
 *	This routine will enable the scc transmitter.
 *
 * Parameters:
 *	quicc_dpr_t * - pointer to dualport ram.
 *	ushort - port number to enable.
 *
 * Returns:
 *	void - None.
 * =================================================================
 */
void quicc_scc_enable_tx (quicc_dpr_t *dpr, quicc_info_t *qinfo)
{
    ushort port = qinfo->port_no;
    
#if QUICC_PORT_SCC1 == 0
    QASSERT(port <= QUICC_PORT_SCC4);
#else
    QASSERT((port >= QUICC_PORT_SCC1) && (port <= QUICC_PORT_SCC4));
#endif

    dpr->regs.scc[port].scc_sccm |= qinfo->tx_enable_sccm_mask;
    dpr->regs.scc[port].scc_gsmrl |= QUICC_SCC_GSMRL_ENT;

    quicc_send_risc_cmd(dpr, port, QUICC_CPC_TX_RESTART);

}



/*
 * =================================================================
 * Name: quicc_scc_disable_tx
 *
 * Description:
 *	This routine will disable the scc transmitter.
 *
 * Parameters:
 *	quicc_dpr_t * - pointer to dualport ram.
 *	ushort - port number to disable.
 *
 * Returns:
 *	void - None.
 * =================================================================
 */
void quicc_scc_disable_tx (quicc_dpr_t *dpr, quicc_info_t *qinfo)
{
    ushort port = qinfo->port_no;
    
#if QUICC_PORT_SCC1 == 0
    QASSERT(port <= QUICC_PORT_SCC4);
#else
    QASSERT((port >= QUICC_PORT_SCC1) && (port <= QUICC_PORT_SCC4));
#endif

    quicc_send_risc_cmd(dpr, port, QUICC_CPC_GR_TX_STOP);

    dpr->regs.scc[port].scc_gsmrl &= ~QUICC_SCC_GSMRL_ENT;

    dpr->regs.scc[port].scc_sccm &= ~(qinfo->tx_disable_sccm_mask);
    /*
       SCC Event Register:
       A bit is cleared by writing a one;
       writing a zero does not affect a bit's value.
       operator "|=" will clear all scce bits.
     */
    dpr->regs.scc[port].scc_scce = qinfo->tx_clear_scce_event;

}


/*
 * =================================================================
 * Name: quicc_scc_start
 *
 * Description:
 *	Enable both transmit and/or receive channels for this port.
 *
 * Parameters:
 *	quicc_dpr_t * - pointer to dual port ram.
 *	ushort - port to enable.
 *	boolean - enable receiver.
 *	boolean - enable transmitter.
 *
 * Returns:
 *	TRUE - if the the ports were enabled.
 * =================================================================
 */
boolean quicc_scc_start(quicc_dpr_t *dpr,
			quicc_info_t *qinfo,
			boolean rx_enable,
			boolean tx_enable)
{
    ushort port = qinfo->port_no;
    
#if QUICC_PORT_SCC1 == 0
    QASSERT(port <= QUICC_PORT_SCC4);
#else
    QASSERT((port >= QUICC_PORT_SCC1) && (port <= QUICC_PORT_SCC4));
#endif
    /*
     * If we have already been started then just return.
     */
    if (dpr->regs.cpic_cimr & quicc_scc_intr_mask[port])
	return(TRUE);

    quicc_send_risc_cmd(dpr, port, QUICC_CPC_INIT_RXTX);

    /*
     * Enable master interrupt.
     */
    dpr->regs.cpic_cimr |= quicc_scc_intr_mask[port];

    if (rx_enable) 
	quicc_scc_enable_rx(dpr, qinfo);

    if (tx_enable)
	quicc_scc_enable_tx(dpr, qinfo);
    
    return(TRUE);
}


/*
 * =================================================================
 * Name: quicc_scc_stop
 *
 * Description:
 *	Disable both transmit and receive channels for this port.
 *
 * Parameters:
 *	quicc_dpr_t * - pointer to dual port ram.
 *	ushort - port to disable.
 *
 * Returns:
 *	void - none.
 * =================================================================
 */
void quicc_scc_stop (quicc_dpr_t *dpr,
		     quicc_info_t *qinfo,
		     boolean rx_disable,
		     boolean tx_disable)
{
    ushort port = qinfo->port_no;
	
#if QUICC_PORT_SCC1 == 0
    QASSERT(port <= QUICC_PORT_SCC4);
#else
    QASSERT((port >= QUICC_PORT_SCC1) && (port <= QUICC_PORT_SCC4));
#endif

    /*
     * Disable master interrupt.
     */
    dpr->regs.cpic_cimr &= ~quicc_scc_intr_mask[port];

    if (rx_disable)
	quicc_scc_disable_rx(dpr, qinfo);

    if (tx_disable)
	quicc_scc_disable_tx(dpr, qinfo);
}

/*
 * The following state is kept to prevent multiple user
 * of the spi stepping on each other.
 */

static boolean spi_locked = FALSE;
static boolean spi_running = FALSE;

/*
 * =================================================================
 * Name: quicc_spi_init
 *
 * Description:
 *	This call initializes the QUICC spi interface.
 *
 * Parameters:
 *	void - none.
 *
 * Returns:
 *	void - none.
 * =================================================================
 */

void quicc_spi_init (quicc_dpr_t *dpr)
{
    quicc_spi_param_t *spi;

    QASSERT(spi_running == FALSE);

    /*
     * Silently return if already running.
     */
    if (spi_running == TRUE)
	return;

    /*
     * Initalize all the pins.
     */
    quicc_set_spi_pins(dpr);

    spi = &dpr->spi;

    /*
     * Initalize SPI mode register.
     */
    dpr->regs.spi_spmode = QUICC_SPMR_DIV16 | QUICC_SPMR_REVDAT |
	QUICC_SPMR_MASTER | QUICC_SPMR_LEN(QUICC_SPI_8BITS);
    /*
     * Initalize SPI parameter ram.
     */
    spi->rbase = QUICC_RXBD_OFFSET_SPI;
    spi->tbase = QUICC_TXBD_OFFSET_SPI;
    spi->rfcr = QUICC_SPIFC_MOT;
    spi->tfcr = QUICC_SPIFC_MOT;
    spi->mrblr = QUICC_SPI_MTU;

    quicc_send_risc_cmd(dpr, QUICC_PORT_SPI, QUICC_CPC_INIT_RXTX);

    /*
     * Clear interrupt event register.
     */
    dpr->regs.spi_spie = 0xFF;

    /*
     * Enable the SPI.
     */
    dpr->regs.spi_spmode |= QUICC_SPMR_ENASPI;

    spi_running = TRUE;
}



/*
 * =================================================================
 * Name: quicc_spi_acquire
 *
 * Description:
 *	Simple mutex to garentees only one device is using the spi
 *	at an particular time.
 *
 * Parameters:
 *	void - none.
 *
 * Returns:
 *	void - none.
 * =================================================================
 */
void quicc_spi_acquire (void)
{
    QASSERT(spi_running == TRUE);
    QASSERT(spi_locked == FALSE);
    spi_locked = TRUE;
}


/*
 * =================================================================
 * Name: quicc_spi_release
 *
 * Description:
 *
 * Parameters:
 *	void - none.
 *
 * Returns:
 *	void - none
 * =================================================================
 */
void quicc_spi_release (void)
{
    QASSERT(spi_running == TRUE);
    QASSERT(spi_locked == TRUE);
    spi_locked = FALSE;
}    


/*
 * =================================================================
 * Name: quicc_spi_op
 *
 * Description:
 *	This call is used for all data transfer on the SPI interface.
 *	The user supplies 2 buffers of the same length.  One of the
 * 	buffers is for the transmit command and the other for the
 *	received responce.  The receive buffer will be filled at the
 *	same time the transmit buffer is being clocked out.  If the
 *	user expects to receive the data from the external device
 *	when this call returns (synchronous) then padding must be
 *	added to the transmit buffer to keep the clock running.
 *	See below: 
 *
 * 	For NVRAM the user must pad the buffer.
 *
 * 	|--------|--------|--------|--------|
 * 	| BYTE 1 | BYTE 2 | PAD 1  | PAD 2  |	Transmit Buffer
 * 	|--------|--------|--------|--------|
 *
 * 	|--------|--------|--------|--------|
 * 	| PAD  1 | PAD  2 | BYTE 1 | BYTE 2 |	Receive Buffer
 * 	|--------|--------|--------|--------|
 *
 * 	For TP3420 The user must make 2 calls because the last status
 *	is returned.
 *
 * 	|--------|
 * 	| CMD    |	Transmit Buffer
 * 	|--------|
 *
 * 	|--------|
 * 	| STATUS |	Receive Buffer.
 * 	|--------|
 *
 * 	|--------|
 * 	| NOP    |	Transmit Buffer
 * 	|--------|
 *
 * 	|--------|
 * 	| STATUS |	Receive Buffer (This is the STATUS for the CMD
 * 	|--------|	issued earlier.
 *
 *
 * Parameters:
 *	quicc_dpr_t * - pointer to dualport ram.
 *	uchar * - the command to execute.
 *	uchar * - where the reply is stored.
 *	ulong - length of both command and reply buffers.
 *
 * Returns:
 *	boolean - TRUE if the command was executed.
 *	boolean - FALSE if the command timed out.
 * =================================================================
 */
boolean quicc_spi_op (quicc_dpr_t *dpr, uchar *cmd, uchar *reply, ulong len)
{
    quicc_bd_t *tbd;
    quicc_bd_t *rbd;
    ushort count;

    QASSERT(spi_running == TRUE);
    QASSERT(spi_locked == TRUE);

    if (spi_running == FALSE)
	return(FALSE);
    /*
     * Fill the buffer descriptor info.
     */

    rbd = (quicc_bd_t *)((uchar *)dpr + dpr->spi.rbase);
    rbd->length = 0;
    rbd->buf_ptr = reply;
    rbd->status = (QUICC_BDSTAT_RX_EMPTY | QUICC_BDSTAT_RX_WRAP);

    tbd = (quicc_bd_t *)((uchar *)dpr + dpr->spi.tbase);
    tbd->length = len;
    tbd->buf_ptr = cmd;
    tbd->status = (QUICC_BDSTAT_TX_RDY | QUICC_BDSTAT_TX_LAST |
		    QUICC_BDSTAT_TX_WRAP);

    /*
     * Kick things off
     */
    dpr->regs.spi_spcom = QUICC_SPCOM_START;

    /*
     * Now we need to sit around and wait for the risc
     * to give back the buffer.  When this is done we
     * know the data is in the reply buffer.
     */
    for (count = 0; count < QUICC_SPI_SPIN_MAX; count++) {
	if ((rbd->status & QUICC_BDSTAT_RX_EMPTY) == 0)
	    return(TRUE);
    }
    return(FALSE);
}


/*
 * =================================================================
 * Name: quicc_scc_set_listen_addr
 *
 * Description:
 *
 * Parameters:
 *	quicc_dpr_t * - pointer to dualport ram.
 *	void * - pointer to 48bit mac address.
 *
 * Returns:
 *	void - none.
 * =================================================================
 */
void quicc_scc_set_listen_addr (quicc_dpr_t *dpr, void *mac)
{
    quicc_ether_param_t *ether;
    ushort *addr = (ushort *)mac;

    /*
     * This routine assumes that the ethernet is using SCC1.
     */
    ether = &dpr->scc1.ether;

    ether->taddr_l = ((*addr >> 8) & 0xff) |
                     ((*addr << 8) & 0xff00);
    addr++;
    ether->taddr_m = ((*addr >> 8) & 0xff) |
                     ((*addr << 8) & 0xff00);
    addr++;
    ether->taddr_h = ((*addr >> 8) & 0xff) |
                     ((*addr << 8) & 0xff00);

    quicc_send_risc_cmd (dpr, QUICC_PORT_SCC1, QUICC_CPC_SET_GROUP);
}


/*
 * =================================================================
 * Name: quicc_set_my_mac_addr
 *
 * Description:
 *	Set the primary mac address for this interface.
 *
 * Parameters:
 *	quicc_dpr_t * - pointer to dualport ram.
 *	void * - pointer to 48bit mac address.
 *
 * Returns:
 *	void - none.
 * =================================================================
 */
static void quicc_set_my_mac_addr (quicc_dpr_t *dpr, void *mac)
{
    quicc_ether_param_t *ether;
    ushort *addr = (ushort *)mac;

    /*
     * This routine assumes that the ethernet is using SCC1.
     * PADDR should contain the MAC address in network byte
     * order. This order is exactly the opposite of the order
     * in which we store and display MAC addresses.
     */
    ether = &dpr->scc1.ether;
    ether->paddr_l = ((*addr >> 8) & 0xff) |
                     ((*addr << 8) & 0xff00);
    addr++;
    ether->paddr_m = ((*addr >> 8) & 0xff) |
                     ((*addr << 8) & 0xff00);
    addr++;
    ether->paddr_h = ((*addr >> 8) & 0xff) |
                     ((*addr << 8) & 0xff00);
}


/*
 * =================================================================
 * Name: quicc_scc_param_init
 *
 * Description:
 *
 * Parameters:
 *	quicc_scc_param_t * - pointer to scc parameter ram.
 *	ushort - port number to initalize.
 *	ushort - maximum transfer unit.
 *
 * Returns:
 *	void - none.
 * =================================================================
 */
static void quicc_scc_param_init (quicc_scc_param_t *scc, ushort port,
				  ushort mtu)
{
    scc->rbase = quicc_rxbd_offset(port);
    scc->tbase = quicc_txbd_offset(port);
    scc->rfcr = QUICC_SCCFC_MOT;
    scc->tfcr = QUICC_SCCFC_MOT;
    
    /*
     * Note - The mrblr must divisible by 4.
     */
    scc->mrblr = (mtu + 3) & ~3;
}

/*
 * =================================================================
 * Name: quicc_uart_param_init
 *
 * Description:
 *	Initalize the parameter ram for UART.
 *
 * Parameters:
 *	quicc_dpr_t * - pointer to dualport ram.
 *	ushort - port number to initalize.
 *	ushort - maximum transfer unit.
 *
 * Returns:
 *	void - none.
 * =================================================================
 */
void quicc_uart_param_init (quicc_dpr_t *dpr, ushort port, ushort mtu)
{
    quicc_uart_param_t *async;
    int index;

    QASSERT(quicc_scc_status(dpr, port, FALSE, FALSE) == TRUE);

    switch (port) {
    default:		/* Should not get here */
    case QUICC_PORT_SCC1:
	async = &dpr->scc1.uart;
	break;
    case QUICC_PORT_SCC2:
	async = &dpr->scc2.uart;
	break;
    case QUICC_PORT_SCC3:
	async = &dpr->scc3.uart;
	break;
    case QUICC_PORT_SCC4:
	async = &dpr->scc4.uart;
	break;
    }

    quicc_scc_param_init(&async->scc_param, port, mtu);

    /* should not trap idle event */
    async->max_idl = 5;  /* WILD GUESS */


    async->parec = 0;
    async->frmer = 0;
    async->nosec = 0;
    async->brkec = 0;

    /* Assuming no multi-drop */
    async->uaddr1 = 0;
    async->uaddr2 = 0;
    
    async->toseq = 0;
    
    /* Write CHARACTER 1-8 (characters causing receive buffer
     * descriptor to be closed.)
     *
     * NOTE:  IF we add any control characters, we need to set a bit
     * in the scc mask to get the corresponding interrupt.
     */
    for (index = 0; index < 8; index ++) {
        async->cc[index] = QUICC_UART_CHARX_INVALID;
    }
    
    async->rccm = QUICC_UART_NO_CTRL_CHAR_MASK;
}


/*
 * =================================================================
 * Name: quicc_hdlc_param_init
 *
 * Description:
 *	Initalize the parameter ram for HDLC.
 *
 * Parameters:
 *	quicc_dpr_t * - pointer to dualport ram.
 *	ushort - port number to initalize.
 *	ushort - maximum transfer unit.
 *
 * Returns:
 *	void - none.
 * =================================================================
 */
void quicc_hdlc_param_init (quicc_dpr_t *dpr, ushort port, ushort mtu)
{
    quicc_hdlc_param_t *hdlc;

    QASSERT(quicc_scc_status(dpr, port, FALSE, FALSE) == TRUE);

    switch (port) {
    default:		/* Should not get here */
    case QUICC_PORT_SCC1:
	hdlc = &dpr->scc1.hdlc;
	break;
    case QUICC_PORT_SCC2:
	hdlc = &dpr->scc2.hdlc;
	break;
    case QUICC_PORT_SCC3:
	hdlc = &dpr->scc3.hdlc;
	break;
    case QUICC_PORT_SCC4:
	hdlc = &dpr->scc4.hdlc;
	break;
    }

    quicc_scc_param_init(&hdlc->scc_param, port, mtu);
	
    /*
     * Initalize HDLC specific parameters.
     */
    hdlc->c_pres = 0xFFFF;
    hdlc->c_mask = 0xF0B8;
    hdlc->disfc = 0;
    hdlc->crcec = 0;
    hdlc->abtsc = 0;
    hdlc->nmarc = 0;
    hdlc->retrc = 0;
    hdlc->mflr = mtu;
    hdlc->rfthr = 0;
    hdlc->hmask = 0;
    hdlc->haddr1 = 0;
    hdlc->haddr2 = 0;
    hdlc->haddr3 = 0;
    hdlc->haddr4 = 0;
}


/*
 * =================================================================
 * Name: quicc_scc_ethernet_param_init
 *
 * Description:
 *	Initalize the ethernet parameter ram.  Note that the MTU is
 * 	assumed to always be as specified in the IEEE 802.3 standard
 *	document.
 *
 * Parameters:
 *	quicc_dpr_t * - pointer to dualport ram.
 *	ushort - port to configure for ethernet.
 *	void * - pointer to 48bit mac address.
 *
 * Returns:
 *	void - none.
 * =================================================================
 */
void quicc_scc_ethernet_param_init (quicc_dpr_t *dpr, ushort port, void *mac)
{
    quicc_ether_param_t *ether;

    QASSERT(quicc_scc_status(dpr, port, FALSE, FALSE) == TRUE);

    ether = &dpr->scc1.ether;
    
    quicc_scc_param_init(&ether->scc_param, port, 1518);

    /*
     * Initalize ETHER specific parameters.
     */
    ether->c_pres = 0xFFFFFFFF;
    ether->c_mask = 0xDEBB20E3;
    ether->crcec = 0;
    ether->alec = 0;
    ether->disfc = 0;
    ether->pads = 0;
    ether->ret_lim = 15;
    ether->mflr = 1518;
    ether->minflr = 64;
    ether->maxd1 = 1518;
    ether->maxd2 = 1518;
    ether->dma_cnt = 0;
    ether->gaddr1 = 0;
    ether->gaddr2 = 0;
    ether->gaddr3 = 0;
    ether->gaddr4 = 0;
    ether->p_per = 0;
    ether->iaddr1 = 0;
    ether->iaddr2 = 0;
    ether->iaddr3 = 0;
    ether->iaddr4 = 0;
    ether->taddr_h = 0;
    ether->taddr_m = 0;
    ether->taddr_l = 0;

    quicc_set_my_mac_addr(dpr, mac);
}


/*
 * =================================================================
 * Name: quicc_tdma_running
 *
 * Description:
 *	Is the time division multiplexer running.
 *
 * Parameters:
 *	quicc_dpr_t * - pointer to dualport ram.
 *
 * Returns:
 *	boolean - TRUE its running.
 *	boolean - FALSE it stopped.
 * =================================================================
 */
boolean quicc_tdma_running (quicc_dpr_t *dpr)
{
    if (dpr->regs.si_sigmr & QUICC_SIGMR_ENABLE_A)
	return(TRUE);
    return(FALSE);
}    


/*
 * =================================================================
 * Name: quicc_tdma_enable
 *
 * Description:
 *	Enable the time division multiplexer.
 *
 * Parameters:
 *	quicc_dpr_t * - pointer to dualport ram.
 *
 * Returns:
 *	void - none.
 * =================================================================
 */
void quicc_tdma_enable (quicc_dpr_t *dpr)
{
    dpr->regs.si_sigmr |= QUICC_SIGMR_ENABLE_A;
}


/*
 * =================================================================
 * Name: quicc_tdma_disable
 *
 * Description:
 *	Disable the time division multiplexer.
 *
 * Parameters:
 *	quicc_dpr_t * - pointer to dualport ram.
 *
 * Returns:
 *	void - none.
 * =================================================================
 */
void quicc_tdma_disable (quicc_dpr_t *dpr)
{
    dpr->regs.si_sigmr &= ~QUICC_SIGMR_ENABLE_A;
}


/*
 * =================================================================
 * Name: quicc_scc_status
 *
 * Description:
 *	Check the receiver and transmitter are enabled.
 *
 * Parameters:
 *	quicc_dpr_t * - pointer to dualport ram.
 *	ushort - port to check.
 *	boolean - TRUE - receiver should be enabled 
 *	boolean - TRUE - transmitter should be enabled 
 *
 * Returns:
 *	boolean - TRUE if both transmitter and receiver are enabled.
 *	boolean - FALSE if at least one is disabled.
 * =================================================================
 */
boolean quicc_scc_status (quicc_dpr_t *dpr,
			  ushort port,
			  boolean rx_enabled,
			  boolean tx_enabled)
{
    ulong gsmr;
    boolean enabled;
    
    gsmr = dpr->regs.scc[port].scc_gsmrl;

    enabled = (gsmr & QUICC_SCC_GSMRL_ENT) ? TRUE : FALSE;
    if (enabled != tx_enabled)
	    return(FALSE);
    
    enabled = (gsmr & QUICC_SCC_GSMRL_ENR) ? TRUE : FALSE;
    if (enabled != rx_enabled)
	    return(FALSE);

    return(TRUE);
}


/*
 * =================================================================
 * Name: quicc_rxbd_offset
 *
 * Description:
 *	Return the offset for the receive buffer descriptor
 *	ring.
 *
 * Parameters:
 *	ushort - port of interest.
 *
 * Returns:
 *	ushort - offset relative to dualport ram.
 * =================================================================
 */
static ushort quicc_rxbd_offset (ushort port)
{
    ushort offset;

#if QUICC_PORT_SCC1 == 0
    QASSERT(port <= QUICC_PORT_SCC4);
#else
    QASSERT((port >= QUICC_PORT_SCC1) && (port <= QUICC_PORT_SCC4));
#endif
    
    switch (port) {
    default:		/* Should not get here */
    case QUICC_PORT_SCC1:
	offset = QUICC_RXBD_OFFSET_SCC1;
	break;
    case QUICC_PORT_SCC2:
	offset = QUICC_RXBD_OFFSET_SCC2;
	break;
    case QUICC_PORT_SCC3:
	offset = QUICC_RXBD_OFFSET_SCC3;
	break;
    case QUICC_PORT_SCC4:
	offset = QUICC_RXBD_OFFSET_SCC4;
	break;
    }
    return(offset);
}


/*
 * =================================================================
 * Name: quicc_txbd_offset
 *
 * Description:
 *	Return the offset for the transmit buffer descriptor
 *	ring.
 *
 * Parameters:
 *	ushort - port of interest.
 *
 * Returns:
 *	ushort - offset relative to dualport ram
 * =================================================================
 */
static ushort quicc_txbd_offset (ushort port)
{
    ushort offset;
    
#if QUICC_PORT_SCC1 == 0
    QASSERT(port <= QUICC_PORT_SCC4);
#else
    QASSERT((port >= QUICC_PORT_SCC1) && (port <= QUICC_PORT_SCC4));
#endif
    
    switch (port) {
    default:		/* Should not get here */
    case QUICC_PORT_SCC1:
	offset = QUICC_TXBD_OFFSET_SCC1;
	break;
    case QUICC_PORT_SCC2:
	offset = QUICC_TXBD_OFFSET_SCC2;
	break;
    case QUICC_PORT_SCC3:
	offset = QUICC_TXBD_OFFSET_SCC3;
	break;
    case QUICC_PORT_SCC4:
	offset = QUICC_TXBD_OFFSET_SCC4;
	break;
    }
    return(offset);
}


/*
 * =================================================================
 * Name: quicc_rxbd_alloc
 *
 * Description:
 *	Return the base for the receive buffer descriptor
 *	ring.
 *
 * Parameters:
 *	ushort - port of interest.
 *
 * Returns:
 *	void * - base address.
 * =================================================================
 */
void *quicc_rxbd_alloc (ushort port)
{
    ulong *base;

    base = (ulong *)((int)getmbar() + quicc_rxbd_offset(port));
    
    return((void *)base);
}


/*
 * =================================================================
 * Name: quicc_txbd_alloc
 *
 * Description:
 *	Return the base for the transmit buffer descriptor
 *	ring.
 *
 * Parameters:
 *	ushort - port of interest.
 *
 * Returns:
 *	void * - base address.
 * =================================================================
 */
void *quicc_txbd_alloc (ushort port)
{
    ulong *base;

    base = (ulong *)((int)getmbar() + quicc_txbd_offset(port));
    
    return((void *)base);
}


/*
 * =================================================================
 * Name: quicc_scc_set_signal
 *
 * Description:
 *	This routine is to set either the CD or CTS
 *	Port C pin so that the pin will be a general
 *	purpose pin.  Be setting this pin if the modem
 *	signal is CTS it means we will honor flow control.
 *
 * Parameters:
 *	quicc_dpr_t * - pointer to dualport ram.
 *	ushort	      - modem signal to set
 *
 * Returns:
 *	void - None.
 * =================================================================
 */
void quicc_scc_set_signal (quicc_dpr_t *dpr, ushort modem_sig)
{
    dpr->regs.pio_pcso |= modem_sig;
}

/*
 * =================================================================
 * Name: quicc_scc_clear_signal
 *
 * Description:
 *	This routine is to set either the CD or CTS
 *	Port C pin so that the pin will be a general
 *	purpose pin.  By clearing the pin we are
 *	ignoring flow control if the pin is CTS.  In
 *	either case, CD or CTS will always be asserted.
 *
 * Parameters:
 *	quicc_dpr_t * - pointer to dualport ram.
 *	ushort	      - modem signal to clear
 *
 * Returns:
 *	void - None.
 * =================================================================
 */
void quicc_scc_clear_signal (quicc_dpr_t *dpr, ushort modem_sig)
{
    dpr->regs.pio_pcso &= ~modem_sig;
}


/*
 * =================================================================
 * Name: quicc_scc_write_psmr
 *
 * Description:
 *
 * Parameters:
 *	quicc_dpr_t * - pointer to dualport ram.
 *	ushort - SCC port
 *
 * Returns:
 *	void - None.
 * =================================================================
 */
void quicc_scc_write_psmr (quicc_dpr_t *dpr, ushort port, ushort psmr)
{
    dpr->regs.scc[port].scc_psmr |= psmr;
}


/*
 * =================================================================
 * Name: quicc_scc_clear_psmr
 *
 * Description:
 *      Use to clear the various bits in the PSMR
 *      
 * Parameters:
 *      void - none.
 * Returns:
 *      void - none.
 * =================================================================
 */
void quicc_scc_clear_psmr(quicc_dpr_t *dpr, ushort port, ushort psmr_bit)
{
    dpr->regs.scc[port].scc_psmr &= ~(psmr_bit);
}

/*
 * =================================================================
 * Name: quicc_scc_tset_psmr
 *
 * Description:
 *      Use to test if a bit is set in the PSMR
 *      
 * Parameters:
 *      void - none.
 * Returns:
 *      void - none.
 * =================================================================
 */
boolean quicc_scc_tset_psmr(quicc_dpr_t *dpr, ushort port, ushort psmr_bit)
{
    return(dpr->regs.scc[port].scc_psmr & psmr_bit);
}


/*
 * =================================================================
 * Name: quicc_scc_write_gsmr
 *
 * Description:
 *
 * Parameters:
 *	quicc_dpr_t * - pointer to dualpoint ram.
 *	ushort - SCC port number.
 *	ulong - lower 32 bit of GSMR.
 *	ulong - higher 32 bit of GSMR.
 *
 * Returns:
 *	void - None.
 * =================================================================
 */
void quicc_scc_write_gsmr (quicc_dpr_t *dpr, ushort port, ulong low, ulong high)
{
    if (low)
	dpr->regs.scc[port].scc_gsmrl |= low;
    if (high)
	dpr->regs.scc[port].scc_gsmrh |= high;
}


/*
 * =================================================================
 * Name: quicc_scc_clear_gsmr
 *
 * Description:
 *
 * Parameters:
 *	quicc_dpr_t * - pointer to dualpoint ram.
 *	ushort - SCC port number.
 *	ulong - lower 32 bit of GSMR.
 *	ulong - higher 32 bit of GSMR.
 *
 * Returns:
 *	void - None.
 * =================================================================
 */
void quicc_scc_clear_gsmr (quicc_dpr_t *dpr, ushort port, ulong low, ulong high)
{
    if (low)
	dpr->regs.scc[port].scc_gsmrl &= ~low;
    if (high)
	dpr->regs.scc[port].scc_gsmrh &= ~high;
}


/*
 * =================================================================
 * Name:  quicc_set_ethernet_pins
 *
 * Description:
 *	Set/Clear all parallel port pins required for ethernet.
 *
 * Parameters:
 *	quicc_dpr_t * - pointer to dual port ram.
 *
 * Returns:
 *	void - None.
 * =================================================================
 */
void quicc_set_ethernet_pins (quicc_dpr_t *dpr)
{
    /*
     *	Port A
     *	PIN    PAPAR   PADIR   PAODR    PADAT	FUNCTION
     *
     *	11	 1	 0	 0	  -	Ethernet Transmit Clock.
     *	 9	 1	 0	 0	  -	Ethernet Receive Clock.
     *   3	 0	 1	 0	  1	Ethernet Full Dplx Control.
     *	 1	 1	 0	 0	  -	Ethernet Transmit Data.
     *	 0	 1	 0	 0	  -	Ethernet Receive Data.
     */
    dpr->regs.pio_padir &= ~0x0A03;
    dpr->regs.pio_padir |=  0x0008;

    dpr->regs.pio_papar &= ~0x0008;
    dpr->regs.pio_papar |=  0x0A03;

    dpr->regs.pio_paodr &= ~0x0A0B;

    dpr->regs.pio_padat |=  0x0008;
    /*
     * Port B
     *	PIN    PBPAR   PBDIR   PBODR	PBDAT 	FUNCTION
     *
     *	12	 0       0       0	  -	10BaseT Polarity Signal.
     *	 9	 0       1       0	  0	10BaseT Auto Port.
     *	 8	 0       1       0	  1	Ethernet Loopback.
     *	 5	 0       0       0	  -	Ethernet Jabber Signal
     */

    dpr->regs.pip_pbpar &= ~0x1320;
    dpr->regs.pip_pbdir &= ~0x1020;
    dpr->regs.pip_pbdir |=  0x0300;
    dpr->regs.pip_pbodr &= ~0x1320;

    dpr->regs.pip_pbdat &= ~0x0100;
    dpr->regs.pip_pbdat |=  0x0200;

    /*
     * Port C
     *	PIN	PCPAR	PCDIR	PCSO	PCDAT	FUNCTION
     *
     *	 9	 0	 1	 0	  -	Input to NTH.
     *	 8	 0	 1	 0	  - 	Input to DSQE.
     *	 5	 0	 0	 1	  1	Ethernet Receive Enable.
     *	 4	 0	 0	 1	  1	Ethernet Collision.
     *	 0	 1	 0	 0	  -	Ethernet Transmit Enable.
     */
    
    dpr->regs.pio_pcpar &= ~0x330;
    dpr->regs.pio_pcpar |=  0x001;
    dpr->regs.pio_pcdir &= ~0x031;
    dpr->regs.pio_pcdir |=  0x300;
    dpr->regs.pio_pcso  &= ~0x301;
    dpr->regs.pio_pcso  |=  0x030;
    dpr->regs.pio_pcdat |=  0x300;

    /*
     * Initalize the clock routing.  Using the following bits:
     *
     *	SCC1:	0  R1CS: 101b (CLK2) T1CS: 111b (CLK4)
     */
    dpr->regs.si_sicr |= 0x2F;
}


/*
 * =================================================================
 * Name:  quicc_set_spi_pins
 *
 * Description:
 *	Set/clear all parallel port pins required to support spi.
 *
 * Parameters:
 *	quicc_dpr_t * - pointer to dual port ram.
 *
 * Returns:
 *	void - None.
 * =================================================================
 */
static void quicc_set_spi_pins (quicc_dpr_t *dpr)
{
    /*
     *	PORT B
     *
     *	PIN    PBPAR   PBDIR   PBODR	FUNCTION
     *
     *	 3	 1       1       0	SPI Receive Data.
     *	 2	 1       1       0	SPI Transmit Data.
     *	 1	 1       1       0	SPI Clock Out.
     */
    
    dpr->regs.pip_pbpar |=  0xE;
    dpr->regs.pip_pbdir |=  0xE;
    dpr->regs.pip_pbodr &= ~0xE;
}


/*
 * =================================================================
 * Name:  quicc_set_bri_pins
 *
 * Description:
 *	Set/clear all parallel port pins required to support ISDN
 *	Basic Rate Interface.
 *
 * Parameters:
 *	quicc_dpr_t * - pointer to dual port ram.
 *
 * Returns:
 *	void - None.
 * =================================================================
 */
void quicc_set_bri_pins (quicc_dpr_t *dpr)
{
    /*
     *	Port A
     *
     *	PIN    PAPAR   PADIR   PAODR    FUNCTION
     *
     *	13	 1	 0	 0	ISDN D-Channel Receive Clock.
     *	12	 1	 0	 0	ISDN D-Channel Transmit Clock.
     *	 8	 1	 0	 0	ISDN B-Channel Clock.
     *	 7	 1	 1	 0	ISDN B-Channel Receive Data.
     *	 6	 1	 1	 1	ISDN B-Channel Transmit Data.
     *	 5	 1	 0	 0	ISDN D-Channel Transmit Data.
     *	 4	 1	 0	 0	ISDN D-Channel Receive Data.
     */
    
    dpr->regs.pio_papar |=  0x31F0;
    dpr->regs.pio_padir &= ~0x3130;
    dpr->regs.pio_padir |=  0x00C0;
    dpr->regs.pio_paodr &= ~0x31B0;
    dpr->regs.pio_paodr |= 0x0040;
    
    /*
     * Port B
     *
     *	PIN    PBPAR   PBDIR   PBODR	FUNCTION
     *
     *	11	 0	 1	 0	ISDN B-Channel Receive LED.
     *	10	 0	 1	 0	ISDN B-Channel Transmit LED.
     *	 4	 0       1       0	ISDN Chip Select (for SPI).
     */
    
    dpr->regs.pip_pbdir |=  0xC10;
    dpr->regs.pip_pbpar &= ~0xC10;
    dpr->regs.pip_pbodr &= ~0xC10;
    
    /*
     * Port B
     *
     *	PIN    PBPAR   PBDIR   PBODR	FUNCTION
     *
     *	15	 1	 0	 0	Strobe 4 (L1ST4)
     *	14	 1	 0	 0	Strobe 3 (List3)
     *	13	 1	 0	 0	Strobe 2 (L1ST2)
     */
    
    dpr->regs.pip_pbdir &= ~0xE000;
    dpr->regs.pip_pbpar |= 0xE000;
    dpr->regs.pip_pbodr &= ~0xE000;
    
    
    /*
     * Port C
     *
     *	PIN    PCPAR   PCDIR	PCSO	NAME
     *
     *	11	 1	 0	 0	ISDN Super Frame Sync (TDMA).
     *	 1       0       0       0      tp3420 Interrupt Signal
     */
    
    dpr->regs.pio_pcpar |=  0x800;
    dpr->regs.pio_pcdir &= ~0x800;
    dpr->regs.pio_pcso  &= ~0x800;

    /*
     * allow high to low change on pc1 to gen interrupt
     */
    dpr->regs.pio_pcint |=  0x02;
}


/*
 * =================================================================
 * Name: quicc_write_protect_nvram
 *
 * Description:
 *      Write protects configuration memory NVRAM by setting the
 *      Write-Protect bit in Base Register 7.
 *
 * Parameters:
 *      void - none.
 * Returns:
 *      void - none.
 * =================================================================
 */

void quicc_write_protect_nvram (void)
{
    quicc_dpr_t *dpr = (quicc_dpr_t *) getmbar();

    dpr->regs.memc_br7 |= QUICC_BR_WRITE_PROTECT;
}


/*
 * =================================================================
 * Name: quicc_write_enable_nvram
 *
 * Description:
 *      Write enables configuration memory NVRAM by clearing the
 *      Write-Protect bit in Base Register 7.
 *
 * Parameters:
 *      void - none.
 * Returns:
 *      void - none.
 * =================================================================
 */
void quicc_write_enable_nvram (void)
{
    quicc_dpr_t *dpr = (quicc_dpr_t *) getmbar();

    dpr->regs.memc_br7 &= ~QUICC_BR_WRITE_PROTECT;
}
/*
 * =================================================================
 * Name: quicc_set_cimr
 *
 * Description:
 *      sets appropriate bit in the cimr reg
 *      
 *
 * Parameters:
 *      void - CIMR bit
 * Returns:
 *      void - none.
 * =================================================================
 */
void quicc_set_cimr (ulong cimr_bit)
{
    quicc_dpr_t *dpr = (quicc_dpr_t *) getmbar();

    dpr->regs.cpic_cimr |= cimr_bit;
}
