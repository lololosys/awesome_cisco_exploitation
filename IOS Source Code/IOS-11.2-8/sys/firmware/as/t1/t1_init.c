/* $Id: init.c,v 1.5 1996/05/12 22:57:03 jliu Exp $
 * $Source: /cscka/t1_neat/src/t1/init.c,v $
 *------------------------------------------------------------------
 * init.c  NEAT initialization routines
 *
 * October, 1995. Joe Liu
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: init.c,v $
 * Revision 1.5  1996/05/12 22:57:03  jliu
 *     o Added bus error and illegal intruction handling routine
 *
 * Revision 1.4  1996/04/08  18:14:57  jliu
 *     o Made some configuration changes to get tone data generation
 *       works
 *     o Implemented transmit underrun error interrupt
 *
 * Revision 1.3  1996/03/19  03:54:38  jliu
 *    o change the Tx clock source to internal by default
 *    o always use XBCKI for slip/transmit buffer clock which is derived
 *      from TDM bus 2.044 Mhz
 *    o when external network loopback is detected, need to change the transmit
 *      clock to line and change it back to internal when that loopback
 *      request goes away
 *    o need to initialize Rx PCM control registers
 *    o modify some internal registers setup for tone generation
 *
 * Revision 1.2  1996/02/22  22:27:44  jliu
 *     o update thge current T1 firmware for starting channelized T1 coding
 *
 * Revision 1.1.1.1  1996/02/10  03:22:52  towu
 * Create
 *
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include 	"t1_headers.h"


void init_quicc( void )
{
    QuiccInternalRegsType *qir_p = (QuiccInternalRegsType *)REGB_ADDR;


    /*
     * This register can be written only once after system reset
     * no software watchdog enable, enable the double bus fault monitor
     * function, select 256 system clocks timeout period for the bus
     * monitor.(6-35)
     */
    qir_p->sim_sypcr = QUICC_SYPCR_NWD_VAL;  /* no WD timer */

    /* 
     * Tell CP to reset RISC processor, registers and parameters for all 
     * the channels(SCCs, SMCs, SPI and PIP) then wait until FLG bit be
     * cleared by CP.(7-5)
     */
    qir_p->cp_cr = (QUICC_CPCR_RST | QUICC_CPCR_SEMA);
    while(qir_p->cp_cr & QUICC_CPCR_SEMA);

    /*
     * SDMA configuration register (7-59)
     * The SDMA channels ignore the FREEZE signal, SDMA channels has priority
     * over all interrupt handlers, bus arbitration priority level ID is 4.
     */
    qir_p->sdma_sdcr = QUICC_SDCR_VAL;

    /*
     * Reset status register (6-34)
     * Cleared all reset resource to the SIM60.
     */
    qir_p->sim_rsr = 0xff;

    /*
     * CPM interrupt control registers (7-377)
     * cpic_cicr - slect level 4 interrupt priority, 0x5 for vector base
     * cpic_cimr - masked all 28 CPM interrupts
     * cpic_cipr - cleared all CPM interrupts pending
     * cpic_cisr - cleared all CPM interrupt in-service 
     */
    qir_p->cpic_cicr = QUICC_CICR_VAL;
    qir_p->cpic_cimr = 0;
    qir_p->cpic_cipr = 0xffffffff;
    qir_p->cpic_cisr = 0xffffffff;

    /*
     * SI clock route register (7-86)
     * Reset the SCC clock source and routing pins
     */
    qir_p->si_sicr = 0x00;              /* reset clock routing pins */

    /* 
     * Breakpoint control register (6-44)
     * make sure the hardware break(watch)point register is clear
     */
    qir_p->sim_bkcr = 0;

    /*
     * Autovector register (6-34)
     * QUICC_AVEC2 for core CPU int, QUICC_AVEC3 for ALINT from Bt8360,
     * QUICC_AVEC5 for backplane reset, QUICC_AVEC6 for DLINT from Bt8360
     */
    qir_p->sim_avr = QUICC_AVEC2 | QUICC_AVEC3 | QUICC_AVEC5 | QUICC_AVEC6;

    /*
     * port E pin assignment register (6-48)
     * select IACK6,5,3,2,1 as output pins
     */
    qir_p->sim_pepar = QUICC_PEPAR_VAL;

    /*
     * port A, B & C (7-357)
     * port A pin 7 is for TDMa transmit output, pin 6 is for receive input,
     * pin 8 & 10 are for TDMa clock. pin 6 and 8 & 10 is programmed for
     * input in pio_padir register.
     */
    qir_p->pio_papar = TDM_TXDa | TDM_TCLKa | TDM_RXDa | TDM_RCLKa;
    qir_p->pio_padir = ~(TDM_TCLKa | TDM_RCLKa);

    /*
     * port c pin 11 & 10 are used for TDMa sync pulse and programmed as 
     * input in pio_pcdir register.
     */
    qir_p->pio_pcpar = TDM_SYNCa;
    qir_p->pio_pcdir = ~TDM_SYNCa;

    /*
     * program all port B pins as output pins
     */
    qir_p->pip_pbpar = 0x0;
    qir_p->pip_pbdir = 0x3ffff;

    NEAT_INT |= CLR_NEAT_INT; /* clear interrupt from core CPU */
}

