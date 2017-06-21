/* $Id: quicc_bri.c,v 3.2.58.1 1996/03/21 23:29:17 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/rommon/src-36-c1000/quicc_bri.c,v $
 *------------------------------------------------------------------
 * quicc_bri.c
 *
 * January 1995, Karuna Sabnis
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 * BRI diagnostic for Sapphire.
 *------------------------------------------------------------------
 * $Log: quicc_bri.c,v $
 * Revision 3.2.58.1  1996/03/21  23:29:17  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.2.2.1  1995/12/01  04:22:18  mbeesley
 * Branch: ELC_branch
 * Reorganize Rom monitor header files to make it easier to port
 * to new platforms.
 *
 * Revision 3.2  1995/11/17  18:44:21  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:07:33  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/11/08  21:24:07  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.2  1995/06/30  00:10:26  clev
 * CSCdi36495:  Fix diags for integrated nt1
 *
 * Split BRI test into Internal test and External loopback test
 *
 * Revision 2.1  1995/06/07  22:39:32  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "monitor.h"
#include "error.h"
#include "bitdef.h"
#include "signal_4k.h"
#include "quicc.h"
#include "quicc_bri.h"

extern long *evec;
extern long *scc2_int, *scc3_int, *scc4_int;
extern long *timer2, *pc1_int, *spi_int;

/* =================================================================*
 *                  Globals for this Module                         *
 * =================================================================*/

static uchar quicc_bri_test;   /* BRI "test on" flag */
static ushort interrupt_flag;  /* Interrupt flag */
static uchar quicc_scc2_int;   /* SCC2 interrupt */
static uchar quicc_scc3_int;   /* SCC3 interrupt */
static uchar quicc_scc4_int;   /* SCC4 interrupt */
static uchar bri_rx_error;     /* Error in receive */ 
static uchar bri_tx_error;     /* Error in transmit */
static uchar bri_busy_error;   /* Error - busy */


/* =================================================================*
 *                      Function Prototypes                         *
 * =================================================================*/


static int   init_scc2(quicc_dpr_t *dpr, int loopmode, int mode);
static int   init_scc3(quicc_dpr_t *dpr, int loopmode, int mode);
static int   init_scc4(quicc_dpr_t *dpr, int loopmode, int mode);
static void  init_flags(void);
static void  setup_pio_ports(quicc_dpr_t *dpr);
static void  setup_pio_ports_ext(quicc_dpr_t *dpr);
static void  setup_clk(quicc_dpr_t *dpr);
static void  cleanup_clk(quicc_dpr_t *dpr);
static void  quicc_siram_init (quicc_dpr_t *dpr);
static void  spi_op (quicc_dpr_t *dpr, uchar *cmd, uchar *reply);
static uchar wrt_sid (quicc_dpr_t *dpr, uchar cmd);
static uchar rd_sid(quicc_dpr_t *dpr);
static void  mask_quicc_int(quicc_dpr_t *dpr);
static void  setup_init_bd(int mode);
static int   setup_scc_param_ram(volatile quicc_hdlc_param_t *hdlc_ptr);
static void  quicc_set_spi_pins (volatile quicc_dpr_t *dpr);
static void  setup_spi_param(volatile quicc_dpr_t *dpr);
static int   test_sccs(quicc_dpr_t *dpr, int mode);
static int   quicc_spi_lpbk(quicc_dpr_t *dpr, int mode);
static int   quicc_sid_int_lpbk(quicc_dpr_t *dpr, int mode);
static int   quicc_sid_ext_lpbk(quicc_dpr_t *dpr, int mode);
static void  print_errors(void);
static int   quicc_bri_cleanup(void);
/*
 * =================================================================
 * Name: init_scc2()
 *
 * Function: initialize SCC2. ENT, ENR bits in GSMR are not enabled.
 *
 * =================================================================
 */
static int init_scc2(quicc_dpr_t *dpr, int loopmode, int mode)
{
    volatile quicc_hdlc_param_t *hdlc_ptr;
    quicc_internal_regs_t *qir_p; /* quicc internal reg */
    volatile quicc_bd_t *bd;  /* scc buffer descriptor */

    qir_p = &(dpr->regs);

    switch(loopmode){
 
    case SID:
	/* normal operation and HDLC mode */
	qir_p->scc[1].scc_gsmrl = QUICC_SCC_GSMRL_DIAG(QUICC_NORMAL_OP);

	/* connect SCC2 to multiplexed SI */
	qir_p->si_sicr |= QUICC_SICR_SCX(QUICC_PORT_SCC2);
	break;
    
    case SCCLP:
	qir_p->brg_brgc2 = QUICC_BRGC_RESET; 

	/* use internal baud rate generator (BRGC2) */
	qir_p->brg_brgc2 = QUICC_BRGC_EN_CLK | 
	    QUICC_BRGC_CLK_SRC(0) | QUICC_BRGC_CLK_DIV(10); 
	
	/* set up gsmr_ local loopback and HDLC mode */
	qir_p->scc[1].scc_gsmrh = 0x0;
	qir_p->scc[1].scc_gsmrl = QUICC_SCC_GSMRL_DIAG(QUICC_LOCAL_LOOPBACK);

	/* set clock select field for SCC2 */

	qir_p->si_sicr = QUICC_SICR_RXCS(QUICC_PORT_SCC2, QUICC_SCC_TO_BRG2)|
	    QUICC_SICR_TXCS(QUICC_PORT_SCC2, QUICC_SCC_TO_BRG2);
	break;
    } /* end switch */

    /* set number of flags = 0, 16 bit CRC, normal operation */
    qir_p->scc[1].scc_psmr = QUICC_BRI_PSMR_SETTING;

    /* Clear SCC event register by writing all ones */  
    qir_p->scc[1].scc_scce = QUICC_BRI_REG_CLEAR;
    
    /* setup HDLC specific parameter ram for SCC2 */
    hdlc_ptr = &(dpr->scc2.hdlc);
    setup_scc_param_ram(hdlc_ptr);

    /* get ptr to user_data_1 if DPRAM was at zero */
    bd = (quicc_bd_t *)(&(((quicc_dpr_t *)(0))->user_data_1));
    
    /* Set pointers to RX and tx buffer descriptors */
    hdlc_ptr->scc_param.rbase = (int)(&bd[ISDN_SCC2_RX_BD_IDX]);
    hdlc_ptr->scc_param.tbase = (int)(&bd[ISDN_SCC2_TX_BD_IDX]);

    /* program CR to execute INIT RX and TX PARAMS command for this
       channel. This loads rbase, tbase params in RBPTR & TBPTR */
    quicc_cr_cmd(QUICC_CHAN_SCC2, QUICC_CPC_INIT_RXTX);

    /*  allow the following interrupts:
     *  error on transmit, complete frame has been received, a frame has been 
     *  received and discarded due to lack of buffers, a buffer has
     *  been transmitted.
     */

    if (mode == INTR_MODE){
	qir_p->scc[1].scc_sccm = QUICC_SCCM_TXERR |
	    QUICC_SCCM_RXFRAME | QUICC_SCCM_BSY  | QUICC_SCCM_TXBUF |
		QUICC_SCCM_RXBUF ;
    }
}
/*
 * =================================================================
 * Name: init_scc3()
 *
 * Function: initialize SCC3. ENT, ENR bits in GSMR are not enabled.
 *
 * =================================================================
 */
static int init_scc3(quicc_dpr_t *dpr, int loopmode, int mode)
{
    volatile quicc_hdlc_param_t *hdlc_ptr;
    quicc_internal_regs_t *qir_p; /* quicc internal reg */
    volatile quicc_bd_t *bd;  /* scc buffer descriptor */
    
    qir_p = &(dpr->regs);
   
    switch(loopmode){
   
    case SID:
	/* set up gsmr_ normal operation and HDLC mode */
	qir_p->scc[2].scc_gsmrl = QUICC_SCC_GSMRL_DIAG(QUICC_NORMAL_OP);

	/* clear clock select field for SCC3, ClK6 for Rx, CLK5 for Tx */
	qir_p->si_sicr &= ~(QUICC_SICR_RXCS(QUICC_PORT_SCC3, QUICC_SCC_TO_CLK6)|
	    QUICC_SICR_TXCS(QUICC_PORT_SCC3, QUICC_SCC_TO_CLK5));
    
	/* set clock select field for SCC3, ClK6 for Rx, CLK5 for Tx */
	qir_p->si_sicr |= QUICC_SICR_RXCS(QUICC_PORT_SCC3, QUICC_SCC_TO_CLK6)|
	    QUICC_SICR_TXCS(QUICC_PORT_SCC3, QUICC_SCC_TO_CLK5);
	break;
   
    case SCCLP:
	/* set up gsmr_ local loopback and HDLC mode */
	qir_p->scc[2].scc_gsmrh = 0x0;

	qir_p->scc[2].scc_gsmrl = QUICC_SCC_GSMRL_DIAG(QUICC_LOCAL_LOOPBACK);

	/* clear old clock select fields */
	qir_p->si_sicr &= ~(QUICC_SICR_RXCS(QUICC_PORT_SCC3, QUICC_SCC_TO_BRG2)|
			    QUICC_SICR_TXCS(QUICC_PORT_SCC3, QUICC_SCC_TO_BRG2));
	/* set clock select field for SCC3 */
	qir_p->si_sicr |= QUICC_SICR_RXCS(QUICC_PORT_SCC3, QUICC_SCC_TO_BRG2)|
	    QUICC_SICR_TXCS(QUICC_PORT_SCC3, QUICC_SCC_TO_BRG2);
	break;
    } /* end switch */
    
    /* number of flags = 0, 16 bit CRC, normal operation */
    qir_p->scc[2].scc_psmr = QUICC_BRI_PSMR_SETTING;

    /* Clear events register by writing all ones */
    qir_p->scc[2].scc_scce = QUICC_BRI_REG_CLEAR;

    /* setup HDLC specific parameter ram for SCC3 */
    hdlc_ptr = &(dpr->scc3.hdlc);
    setup_scc_param_ram(hdlc_ptr);

    /* get ptr to user_data_1 if DPRAM was at zero */
    bd = (quicc_bd_t *)(&(((quicc_dpr_t *)(0))->user_data_1));
    
    /* Set pointers to RX and TX buffer descriptors */
    hdlc_ptr->scc_param.rbase = (int)(&bd[ISDN_SCC3_RX_BD_IDX]);
    hdlc_ptr->scc_param.tbase = (int)(&bd[ISDN_SCC3_TX_BD_IDX]);

    /* program CR to execute INIT RX and TX PARAMS command for this
       channel */
    quicc_cr_cmd(QUICC_CHAN_SCC3, QUICC_CPC_INIT_RXTX);

    /*  allow the following interrupts:
     *  error on transmit, complete frame has been received, a frame has  
     *  been received and discarded due to lack of buffers, a buffer has
     *  been transmitted.
     */
    if (mode == INTR_MODE) {
	qir_p->scc[2].scc_sccm = QUICC_SCCM_TXERR | QUICC_SCCM_TXERR |
	    QUICC_SCCM_RXFRAME | QUICC_SCCM_TXBUF | QUICC_SCCM_BSY |
		QUICC_SCCM_RXBUF ;
    }
}
/*
 * =================================================================
 * Name: init_scc4()
 *
 * Function: initialize SCC4. ENT, ENR bits in GSMR are not enabled.
 *
 * =================================================================
 */
static int init_scc4(quicc_dpr_t *dpr, int loopmode, int mode)
{
    volatile quicc_hdlc_param_t *hdlc_ptr;
    quicc_internal_regs_t *qir_p; /* quicc internal reg */
    volatile quicc_bd_t *bd;  /* scc buffer descriptor */

    qir_p = &(dpr->regs);

    switch(loopmode){

    case SID:
	/* set up gsmr_ normal operation and HDLC mode */
	qir_p->scc[3].scc_gsmrl = QUICC_SCC_GSMRL_DIAG(QUICC_NORMAL_OP);

	/* connect SCC4 to multiplexed SI */
	qir_p->si_sicr |= QUICC_SICR_SCX(QUICC_PORT_SCC4);

	break;
   
    case SCCLP:
	qir_p->brg_brgc2 = QUICC_BRGC_RESET; 

	/* use internal baud rate generator (BRGC2) */
	qir_p->brg_brgc2 = QUICC_BRGC_EN_CLK | 
	    QUICC_BRGC_CLK_SRC(0)| QUICC_BRGC_CLK_DIV(10); 

	/* set up gsmr_ local loopback and HDLC mode */
	qir_p->scc[3].scc_gsmrh = 0x0;
	qir_p->scc[3].scc_gsmrl = QUICC_SCC_GSMRL_DIAG(QUICC_LOCAL_LOOPBACK);

	/* clear old clock select fields */
	qir_p->si_sicr &= ~(QUICC_SICR_RXCS(QUICC_PORT_SCC4, QUICC_SCC_TO_BRG2)|
			    QUICC_SICR_TXCS(QUICC_PORT_SCC4, QUICC_SCC_TO_BRG2));
	/* set clock select field for SCC4 */
	qir_p->si_sicr |= QUICC_SICR_RXCS(QUICC_PORT_SCC4, QUICC_SCC_TO_BRG2)|
	    QUICC_SICR_TXCS(QUICC_PORT_SCC4, QUICC_SCC_TO_BRG2);

	break;
     } /* end switch */

    /* number of flags = 0, 16 bit CRC, normal operation */
    qir_p->scc[3].scc_psmr = QUICC_BRI_PSMR_SETTING; 
			
    /* Clear events register by writing all ones */
    qir_p->scc[3].scc_scce = QUICC_BRI_REG_CLEAR;

    /* setup HDLC specific parameter ram for SCC4 */
    hdlc_ptr = &(dpr->scc4.hdlc);
    setup_scc_param_ram(hdlc_ptr);

    /* get ptr to user_data_1 if DPRAM was at zero */
    bd = (quicc_bd_t *)(&(((quicc_dpr_t *)(0))->user_data_1));
    
    /* Set pointers to RX and TX buffer descriptors */
    hdlc_ptr->scc_param.rbase = (int)(&bd[ISDN_SCC4_RX_BD_IDX]);
    hdlc_ptr->scc_param.tbase = (int)(&bd[ISDN_SCC4_TX_BD_IDX]);

    /* program CR to execute INIT RX and TX PARAMS command for this
       channel */
    quicc_cr_cmd(QUICC_CHAN_SCC4, QUICC_CPC_INIT_RXTX);
    /*  allow the following interrupts:
     *  error on transmit, complete frame has been received, a frame has been 
     *  received and discarded due to lack of buffers, a buffer has
     *  been transmitted.
     */
    if (mode == INTR_MODE) {
	qir_p->scc[3].scc_sccm = QUICC_SCCM_TXERR | QUICC_SCCM_TXERR |
	    QUICC_SCCM_RXFRAME  | QUICC_SCCM_TXBUF | QUICC_SCCM_BSY |
		QUICC_SCCM_RXBUF ;
    }
}
/*
 * =================================================================
 * Name: init_flags()
 *
 * Function:  init all flags
 *
 * =================================================================
 */
static void init_flags(void)
{
    quicc_scc2_int = 0;   /* SCC2 intr. */
    quicc_scc3_int = 0;   /* SCC3 intr. */ 	
    quicc_scc4_int = 0;   /* SCC4 intr. */ 
    interrupt_flag = 0;   /* flag for other interrupts */
    bri_rx_error = 0;     /* Rx error */ 
    bri_tx_error = 0;     /* Tx error */  
    bri_busy_error = 0;   /* busy error */
}
/*
 * =================================================================
 * Name: setup_pio_ports()
 *
 * Function: setup parallel I/O ports
 *
 * =================================================================
 */
void setup_pio_ports(quicc_dpr_t *dpr)
{
    volatile quicc_internal_regs_t *qir_p; /* quicc internal reg */
    int pcdat_stat = 0;

    qir_p = &(dpr->regs);
    
    /* port  A pins 4,5,12,13 are RxDat, Txdat, Txclk, Rxclk for channel D */
    /* port A pins 6,7,8 are Tx, Rx data and clock for channel B */

    qir_p->pio_papar |=  SAP_QU_PT_A_PAR_ID_RDAT | SAP_QU_PT_A_PAR_ID_TDAT | 
	SAP_QU_PT_A_PAR_ID_RCLK | SAP_QU_PT_A_PAR_ID_TCLK | SAP_QU_PT_A_PAR_IB_CLK |
	    SAP_QU_PT_A_PAR_IB_RDAT | SAP_QU_PT_A_PAR_IB_TDAT;

    /* configure port A pins: RxDat, Txdat for channel B, as output pins */
    /* others are input pins. */
 
    qir_p->pio_padir &= ~SAP_QU_PT_A_BRI_DIR_CLR;
    qir_p->pio_padir |=  SAP_QU_PT_A_PAR_IB_RDAT | SAP_QU_PT_A_PAR_IB_TDAT;
    qir_p->pio_paodr &= ~SAP_QU_PT_B_BRI_ODR_CLR;
    qir_p->pio_paodr |=  SAP_QU_PT_B_BRI_ODR_SET;

    /* configure port B pins: ISDN Chip Select, SPI serial data in,
    /* SPI serial data out, SPI clock  */

    qir_p->pip_pbpar |= SAP_QU_PT_B_PAR_SPI_SDI | SAP_QU_PT_B_PAR_SPI_SDO | SAP_QU_PT_B_PAR_SPI_CLK;
    qir_p->pip_pbdir |= SAP_QU_PT_B_PAR_SPI_SDI | SAP_QU_PT_B_PAR_SPI_SDO | SAP_QU_PT_B_PAR_SPI_CLK;
    qir_p->pip_pbodr &= ~(SAP_QU_PT_B_PAR_SPI_SDI | SAP_QU_PT_B_PAR_SPI_SDO | 
			  SAP_QU_PT_B_PAR_SPI_CLK);
   
    /* configure port C pins: ISDN Super Frame Sync* as O/P pin */
    qir_p->pio_pcpar |= SAP_QU_PT_C_PAR_I_FSA ;    /* pin 11 is input */    
    qir_p->pio_pcdir &= ~SAP_QU_PT_C_PAR_I_FSA;
    qir_p->pio_pcso  &= ~SAP_QU_PT_C_PAR_I_FSA;

    /* TP3420 A interrupt goes to PC1.*/
    qir_p->pio_pcdir &= ~SAP_QU_PT_C_PAR_SIMM_0;
    qir_p->pio_pcpar &= ~SAP_QU_PT_C_PAR_SIMM_0;
    qir_p->pio_pcso  &= ~SAP_QU_PT_C_PAR_SIMM_0;
  
    /* high to low change on PC1, generates an interrupt request */
    qir_p->pio_pcint = SAP_QU_PT_C_PAR_SIMM_0;
}
/*
 * ========================================================================
 * Name: setup_pio_ports_ext()
 *
 * Function: setup parallel I/O ports to do the external loopback test
 * Port A: pin 8 as an output, generates the clock
 * Port C: pin 10 as an general purpose I/O pin
 * History:
 * Port C: pin 11 as an general purpose I/O pin (rev 1)
 * Port C: pin 10 as an general purpose I/O pin (rev 2)
 * 
 * ========================================================================
 */
static void setup_pio_ports_ext(quicc_dpr_t *dpr)
{
    volatile quicc_internal_regs_t *qir_p; /* quicc internal reg */

    qir_p = &(dpr->regs);

    /* configure port A pin 8 as an output pin BRG01 for BCLK */
    qir_p->pio_padir |= SAP_QU_PT_A_PAR_IB_CLK; 

    /* configure port C pin 11 as an general purpose pin, output */
    qir_p->pio_pcpar &= ~SAP_QU_PT_C_PAR_I_FSA;    
    qir_p->pio_pcdir |= SAP_QU_PT_C_PAR_I_FSA;
    qir_p->pio_pcso &= ~SAP_QU_PT_C_PAR_I_FSA;
}
/*
 * =========================================================================
 * setup_clk()
 * Function: setup the clock generated from BGR01, and frame by using the
 * timer2 interrupt.
 *
 * =========================================================================
 */
static void setup_clk(quicc_dpr_t *dpr)
{
    volatile quicc_internal_regs_t *qir_p; /* quicc internal reg */

    qir_p = &(dpr->regs);

    /* setup the clock output, need the 256KHZ clock generated from BRG01 */
    qir_p->brg_brgc1 = QUICC_BRGC_RESET;
    qir_p->brg_brgc1 = QUICC_BRGC_EN_CLK|QUICC_BRGC_CLK_SRC(0)|QUICC_BRGC_CLK_DIV(98);

    /* setup timer2 to generate interrupt every 8KHZ or 125us reset
       all timers  */
    qir_p->tmr_tgcr = 0;   
  
    qir_p->tmr_tmr2 = QUICC_TMR_INT_EN|QUICC_TMR_RESET_MODE|QUICC_TMR_ICLK_INTCLK;
    
    /* init the count */
    qir_p->tmr_tcn2 = 0;    

    /* for 3125 decimal, to generate interrupt every 8KHz */
    qir_p->tmr_trr2 = 0xc35;
				
    /* clear the event register */
    qir_p->tmr_ter2 =  QUICC_BRI_REG_CLEAR;

    /* enable timer 2 interrupt */
    qir_p->cpic_cimr |= QUICC_CIMR_TIMER2;

    /* enable timer2 */ 
    qir_p->tmr_tgcr = QUICC_TGCR_TMR2_EN; 
}
/*
 * =========================================================================
 * cleanup_clk()
 * Function: clean up the clk after setup; baud rate generater and timer2 
 *
 * =========================================================================
 */
static void cleanup_clk(quicc_dpr_t *dpr)
{
    volatile quicc_internal_regs_t *qir_p; /* quicc internal reg */

    qir_p = &(dpr->regs);
    qir_p->brg_brgc1 = QUICC_BRGC_RESET;

    /* no more timer interrupt */
    qir_p->tmr_tmr2  = 0;    

    /* reset all timers */
    qir_p->tmr_tgcr  = 0;    
}
/*
 * =================================================================
 * Name: quicc_siram_init
 *
 * Description:
 * 	Set SIRAM format to use a dynamic map.  One part is set for
 *	64Kb/s and the other for 56Kb/s.  The transmit and receive
 *	maps are symetric. The values below are derived using the
 *	following format.  These maps are for Sapphire your map may
 *	vary.  This corresponds to the TP3420A DIF4 Format.  Note
 *	the D Channel bits are not needed in the sapphire design.
 *
 * 	The following map is for 64Kb/s:
 *
 *	SWTR	SSEL	CSEL	CNT	BYT	LST	Description
 *
 *	 0	0000	010	0000	1	0	B1 (8 bits)
 *	 0	0000	000	0001	0	0	2 Bits N/A
 *	 0	0000	100	0000	1	0	B2 (8 bits)
 *	 0	0000	000	0000    0	1	1 bit N/A
 *
 * 	The following map is for 56Kb/s:
 *
 *	SWTR	SSEL	CSEL	CNT	BYT	LST	Description
 *
 *	 0	0000	000	0000	0	0	1 Bit N/A
 *	 0	0000	010	0110	0	0	B1 (7bits)
 *	 0	0000	000	0010	0	0	3 Bits N/A
 *	 0	0000	100	0110	0	0	B2 (7bits)
 *	 0	0000	000	0000    0	1	1 bit N/A
 *
 * Parameters:
 *	quicc_dpr_t *dpr - pointer to dualport ram.
 *	ulong bandwidth - 64Kb/s or 56Kb/s value.
 *
 * Returns:
 *	void - none.
 * =================================================================
 */
void quicc_siram_init (quicc_dpr_t *dpr)
{
    volatile ushort *siram;
    ushort offset;
  
    siram = dpr->regs.si_siram;
    /*
     * Receive Routing Map for 64Kb/s.
     */
    offset = QUICC_SIRAM_MAP1_RX;
    siram[offset + 0] = (QUICC_SIRAM_CSEL(QUICC_PORT_SCC2) |
			 QUICC_SIRAM_CNT_BYTE(1));
    siram[offset + 1] = QUICC_SIRAM_CNT_BIT(2);
    siram[offset + 2] = (QUICC_SIRAM_CSEL(QUICC_PORT_SCC4) |
			 QUICC_SIRAM_CNT_BYTE(1));
    siram[offset + 3] = (QUICC_SIRAM_CNT_BIT(1) | QUICC_SIRAM_LST);

    /*
     * Transmit Routing Map for 64Kb/s.
     */
    offset = QUICC_SIRAM_MAP1_TX;
    siram[offset + 0] = QUICC_SIRAM_CNT_BIT(1);
    siram[offset + 1] = (QUICC_SIRAM_CSEL(QUICC_PORT_SCC2) |
			 QUICC_SIRAM_CNT_BYTE(1));
    siram[offset + 2] = QUICC_SIRAM_CNT_BIT(2);
    siram[offset + 3] = (QUICC_SIRAM_CSEL(QUICC_PORT_SCC4) |
			 QUICC_SIRAM_CNT_BYTE(1));
    siram[offset + 4] = (QUICC_SIRAM_CNT_BIT(1) | QUICC_SIRAM_LST);
 
    /*
     * Switch on strobe for debugging.
     */
    siram[offset + 1] |= QUICC_SIRAM_SSEL(QUICC_SIRAM_SSEL2);
    siram[offset + 2] |= QUICC_SIRAM_SSEL(QUICC_SIRAM_SSEL4);
    siram[offset + 3] |= QUICC_SIRAM_SSEL(QUICC_SIRAM_SSEL3);

    /*
     * Receive Routing Map for 56Kb/s.
     */
    offset = QUICC_SIRAM_MAP2_RX;
    siram[offset + 0] = QUICC_SIRAM_CNT_BIT(1);
    siram[offset + 1] = (QUICC_SIRAM_CSEL(QUICC_PORT_SCC2) |
			 QUICC_SIRAM_CNT_BIT(7));
    siram[offset + 2] = QUICC_SIRAM_CNT_BIT(3);
    siram[offset + 3] = (QUICC_SIRAM_CSEL(QUICC_PORT_SCC4) |
			 QUICC_SIRAM_CNT_BIT(7));
    siram[offset + 4] = (QUICC_SIRAM_CNT_BIT(1) | QUICC_SIRAM_LST);

    /*
     * Transmit Routing Map for 56Kb/s.
     */
    offset = QUICC_SIRAM_MAP2_TX;
    siram[offset + 0] = QUICC_SIRAM_CNT_BIT(2);
    siram[offset + 1] = (QUICC_SIRAM_CSEL(QUICC_PORT_SCC2) |
			 QUICC_SIRAM_CNT_BIT(7));
    siram[offset + 2] = QUICC_SIRAM_CNT_BIT(3);
    siram[offset + 3] = (QUICC_SIRAM_CSEL(QUICC_PORT_SCC4) |
			 QUICC_SIRAM_CNT_BIT(7));
    siram[offset + 4] = (QUICC_SIRAM_CNT_BIT(1) | QUICC_SIRAM_LST);

    /*
     * Tell the system that we are going to have two maps
     * for a single TDM.
     */
    dpr->regs.si_sigmr = QUICC_SIGMR_ONE_DYNAMIC;

    /*
     * Initalize the mode register.  Most bits in this register
     * are zero.  The console clock source should be set in the
     * prom.  Thus, mask console (SMC1) and set all other bits
     * to zero.
     */
    dpr->regs.si_simode |= (QUICC_SIMODE_CRTa |
			    QUICC_SIMODE_RFSDa(1));
    /*
     * Initalize the clock input for the D Channel.  The D Channel
     * is working in NMSI mode.  The receive clock pin is CLK6 and
     * the transmit clock pin is CLK 5 (both on port A).  Port A
     * will have been initalized by the ROM monitor.  Initalize the
     * B channels in SI mode.
     *
     * Initalize the clock routing.  Using the following bits:
     *
     *	SC4:	1
     *	SC3:	0  R3CS: 101b (CLK6) T3CS: 100b (CLK5)
     *  SC2:	1
     */
    dpr->regs.si_sicr |= (QUICC_SICR_SCX(QUICC_PORT_SCC2) |
			  QUICC_SICR_SCX(QUICC_PORT_SCC4) |
			  QUICC_SICR_RXCS(QUICC_PORT_SCC3, QUICC_SCC_TO_CLK6) |
			  QUICC_SICR_TXCS(QUICC_PORT_SCC3, QUICC_SCC_TO_CLK5));

    dpr->regs.si_sicmr = 0xC0;
    /*
     * Enable Time Division Multiplexer.
     */
    dpr->regs.si_sigmr |= QUICC_SIGMR_ENABLE_A;
}
/*
 * =================================================================
 * Name: spi_op()
 *
 * Function: Write commands to the TP3420 (SID) 
 *
 * =================================================================
 */
static void spi_op (quicc_dpr_t *dpr, uchar *cmd, uchar *reply)
{
    volatile quicc_internal_regs_t *qir_p;        /* quicc Internal reg */
    quicc_bd_t *spi_tx_bd, *spi_rx_bd;            /* SPI buffer descriptors */
    int count;

    qir_p = &(dpr->regs);

    /* Set up SPI buffer descriptors */
    spi_rx_bd = &((quicc_bd_t *)(((quicc_dpr_t *)ADRSPC_QUICC_DPRAM)->user_data_1))[ISDN_SPI_RX_BD_IDX];
    /* set wrap, empty bits */
    spi_rx_bd->status = QUICC_BDSTAT_RX_EMPTY | QUICC_BDSTAT_RX_WRAP;
    spi_rx_bd->length = 0;
    spi_rx_bd->buf_ptr = reply;

    spi_tx_bd = &((quicc_bd_t *)(((quicc_dpr_t *)ADRSPC_QUICC_DPRAM)->user_data_1))[ISDN_SPI_TX_BD_IDX];
  
    /* set wrap, last bit */
    spi_tx_bd->status = QUICC_BDSTAT_TX_WRAP | QUICC_BDSTAT_TX_LAST |
	QUICC_BDSTAT_TX_RDY;
    spi_tx_bd->length = 1;
    spi_tx_bd->buf_ptr = cmd;
  
    /* start transmit */
    qir_p->spi_spcom = QUICC_SPCOM_START;
/*    sleep(10);
*/
    count = 0x10000;
 
    /* check empty bit in RxBd to see if reply is in RXBD */
    while (count) {
	if ((spi_rx_bd->status & QUICC_BDSTAT_RX_EMPTY) == 0)
	    break;
	count--;
    }
    if (count == 0)
	cterr('w',0,"SPI timeout:waiting to receive reply");
}
/*
 * =================================================================
 * Name: wrt_sid()
 *
 * Description:
 *	Send a command to the TP3420 via the SPI interface.  This
 *	routine will set the appropriate chip select.
 *
 * Parameters:
 *	quicc_dpr_t *dpr - pointer to dualport ram.
 *	uchar cmd - command to send.
 *
 * =================================================================
 */
static uchar wrt_sid (quicc_dpr_t *dpr, uchar cmd)
{
    uchar command;
    uchar status;

    command = cmd;

    TP3420_ASSERT(dpr);
    spi_op(dpr, &command, &status);
    TP3420_NEGATE(dpr);
    return(status);
}
/*
 * =================================================================
 * Name: rd_sid()
 *
 * Function: Read the status register from the SID
 *
 * =================================================================
 */
static uchar rd_sid(quicc_dpr_t *dpr)
{
    wrt_sid(dpr,TP3420_CRF_NOP);
}
/*
 * =================================================================
 * Name: mask_quicc_int()
 *
 * Function: mask all interrupts from quicc
 *
 * =================================================================
 */
static void mask_quicc_int(quicc_dpr_t *dpr)
{
    volatile quicc_internal_regs_t *qir_p;      
    
    qir_p = &(dpr->regs);
  
  /* mask off all INRQ after the test */
    qir_p->cpic_cimr = 0;    
 
   /* clear Interrupt Service Register */ 
    qir_p->cpic_cisr = QUICC_BRI_REG_CLEAR; 
 
   /* clear Interrupt Pending Register */
    qir_p->cpic_cipr = QUICC_BRI_REG_CLEAR;
}
/*
 * =================================================================
 * Name: setup_init_bd()
 *
 * Function: setup and initialize buffer descriptors for all 3 channels. 
 *
 * =================================================================
 */
static void setup_init_bd(int mode)
{
    quicc_bd_t *tmd;                    /* TX buffer descr. */
    quicc_bd_t *rmd;                    /* RX buffer descr. */
    int i, chan_cnt;                    /* Channel count */
    volatile uchar * buf_start;         /* start of buffer pointer */
    uchar val;
    uchar *buf_addr;
    
    for (chan_cnt=0; chan_cnt <MAX_CHAN; chan_cnt++) {
	switch (chan_cnt) {

	case D_CHAN:
	    rmd = &((quicc_bd_t *)(((quicc_dpr_t *)ADRSPC_QUICC_DPRAM)->user_data_1))[ISDN_SCC3_RX_BD_IDX];
	    tmd = &((quicc_bd_t *)(((quicc_dpr_t *)ADRSPC_QUICC_DPRAM)->user_data_1))[ISDN_SCC3_TX_BD_IDX];
            rmd->buf_ptr = (uchar *) ISDN_RX_BD1_MEM;            
            tmd->buf_ptr = (uchar *) ISDN_TX_BD1_MEM;

            /* fill Rx buffer with 0 */
	    hkeepflags &= ~ H_INCFILL;
            buf_start = rmd->buf_ptr;
            filbyte(buf_start, (int)BUF_SIZE, 0);

            /* fill Tx buffer with a pattern */
	    val = 0x55;
	    hkeepflags &= ~H_INCFILL;
	    buf_start = tmd->buf_ptr;
            filbyte(buf_start, (int)BUF_SIZE, val);
            break;
	
	case B1_CHAN:
    	    rmd = &((quicc_bd_t *)(((quicc_dpr_t *)ADRSPC_QUICC_DPRAM)->user_data_1))[ISDN_SCC2_RX_BD_IDX];
            tmd = &((quicc_bd_t *)(((quicc_dpr_t *)ADRSPC_QUICC_DPRAM)->user_data_1))[ISDN_SCC2_TX_BD_IDX];
            rmd->buf_ptr = (uchar *) ISDN_RX_BD2_MEM;            
            tmd->buf_ptr = (uchar *) ISDN_TX_BD2_MEM;

            /* fill Rx buffer with value = 0 */
	    hkeepflags &= ~H_INCFILL;
            buf_start = rmd->buf_ptr;
            filbyte(buf_start, (int)BUF_SIZE, 0);

            /* fill Tx buffer with a pattern */
	    val = 0xAA;
	    hkeepflags &= ~H_INCFILL;
            buf_start = tmd->buf_ptr;
            filbyte(buf_start, (int)BUF_SIZE, val);
	    break;

	case B2_CHAN:
    	    rmd = &((quicc_bd_t *)(((quicc_dpr_t *)ADRSPC_QUICC_DPRAM)->user_data_1))[ISDN_SCC4_RX_BD_IDX];
            tmd = &((quicc_bd_t *)(((quicc_dpr_t *)ADRSPC_QUICC_DPRAM)->user_data_1))[ISDN_SCC4_TX_BD_IDX];
            rmd->buf_ptr = (uchar *) ISDN_RX_BD3_MEM;            
            tmd->buf_ptr = (uchar *) ISDN_TX_BD3_MEM;

            /* fill Rx buffer with 0 */
	    hkeepflags &= ~ H_INCFILL;
            buf_start = rmd->buf_ptr;
            filbyte(buf_start, (int)BUF_SIZE, 0);

            /* fill Tx buffer with a pattern */
	    val = 0x1;
	    hkeepflags |= H_INCFILL;
            buf_start = tmd->buf_ptr;
            filbyte(buf_start, (int)BUF_SIZE, val);
	    break;
	
	case SPI_CHAN:
    	    rmd = &((quicc_bd_t *)(((quicc_dpr_t *)ADRSPC_QUICC_DPRAM)->user_data_1))[ISDN_SPI_RX_BD_IDX];
            tmd = &((quicc_bd_t *)(((quicc_dpr_t *)ADRSPC_QUICC_DPRAM)->user_data_1))[ISDN_SPI_TX_BD_IDX];

            rmd->buf_ptr = (uchar *) ISDN_RX_BD4_MEM;            
            tmd->buf_ptr = (uchar *) ISDN_TX_BD4_MEM;

            /* fill Rx buffer with 0 */
	    hkeepflags &= ~ H_INCFILL;
            buf_start = rmd->buf_ptr;
            filbyte(buf_start, (int)BUF_SIZE, 0);

            /* fill Tx buffer with a pattern */
	    val = 0x1;
	    hkeepflags |= H_INCFILL;
            buf_start = tmd->buf_ptr;
            filbyte(buf_start, (int)BUF_SIZE, val);
	    break;
	}  /* end switch */

	/* set this flag back to initial value to avoid trouble later */
	hkeepflags &= ~ H_INCFILL;

	/* Set up the RX bd wrap, intr and empty bit. */
	if (mode == INTR_MODE)
	    rmd->status =  QUICC_BDSTAT_RX_EMPTY |
		QUICC_BDSTAT_RX_WRAP | QUICC_BDSTAT_RX_INT;
	else if (mode == POLL_MODE)
	    rmd->status =  QUICC_BDSTAT_RX_EMPTY | QUICC_BDSTAT_RX_WRAP;

        rmd->length = 0;

	/* Set up the TX bd_ not ready, intr, last in frame, tx CRC */
	if (mode == INTR_MODE)
	    tmd->status =  QUICC_BDSTAT_TX_WRAP | QUICC_BDSTAT_TX_INT |
		QUICC_BDSTAT_TX_LAST | QUICC_BDSTAT_TX_TRCRC; 
	else if (mode == POLL_MODE)
	    tmd->status =  QUICC_BDSTAT_TX_WRAP | QUICC_BDSTAT_TX_LAST |
		QUICC_BDSTAT_TX_TRCRC; 

        tmd->length = BUF_SIZE; 
    }  /* end for loop chan_cnt */
}
/*
 * =================================================================
 * Name: setup_scc_param_ram()
 *
 * Function: set up SCC parameters for HDLC   
 *
 * =================================================================
 */
static int setup_scc_param_ram(volatile quicc_hdlc_param_t *hdlc_ptr)
{
    volatile quicc_scc_param_t *scc_param_ptr;

    hdlc_ptr->c_mask = BRI_PRAM_C_MASK;   /* 16 bit CRC */
    hdlc_ptr->c_pres = BRI_PRAM_C_PRES;   /* 16 bit CRC preset */
    hdlc_ptr->disfc = 0;                  /* Discarded Frame Counter */
    hdlc_ptr->crcec = 0;                  /* CRC error counter */
    hdlc_ptr->abtsc = 0;                  /* Abort Sequence Counter */
    hdlc_ptr->nmarc = 0;                  /* Nonmatching Address Rx Counter */
    hdlc_ptr->retrc = 0;                  /* Frame Retransmission Counter */
    hdlc_ptr->mflr = MFLR;                /* max frame lenth reg */
    hdlc_ptr->max_cnt = 0x0;              /* Maximum Length Counter */
    hdlc_ptr->rfthr = 1;                  /* Received Frames Threshold_ 
				             interrupt after every frame */
    hdlc_ptr->rfcnt = 0;                  /* Received Frames count */
    hdlc_ptr->hmask = 0x0000;             /* User Defined Address Mask to allow
                                             all addresses to be recognized*/
    hdlc_ptr->haddr1 = 0;                 /* User Defined Frame Address 1 */
    hdlc_ptr->haddr2 = 0;                 /* User Defined Frame Address 2 */
    hdlc_ptr->haddr3 = 0;                 /* User Defined Frame Address 3 */
    hdlc_ptr->haddr4 = 0;                 /* User Defined Frame Address 4 */

    scc_param_ptr = &(hdlc_ptr->scc_param);

    /* Motorola format, DMA space */
    scc_param_ptr->rfcr =  QUICC_SCCFC_MOT |
                           QUICC_FCR_FUNC_CODE(QUICC_FC_DMA_SPACE);
    
    scc_param_ptr->tfcr =  QUICC_SCCFC_MOT |
                           QUICC_FCR_FUNC_CODE(QUICC_FC_DMA_SPACE);

    scc_param_ptr->mrblr = MRBLR;   /* max Rx buffer length */
}
/*
 * =================================================================
 * Name:  quicc_set_spi_pins
 *
 * Description:
 *	Set/clear all parallel port pins required to support SPI.
 *
 * Parameters:
 *	quicc_dpr_t * - pointer to dual port ram.
 *
 * Returns:
 *	void - None.
 * =================================================================
 */
void quicc_set_spi_pins (volatile quicc_dpr_t *dpr)
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
    
    dpr->regs.pip_pbpar |=  SAP_QU_PT_B_PAR_SPI_SDI | SAP_QU_PT_B_PAR_SPI_SDO
	| SAP_QU_PT_B_PAR_SPI_CLK;
    dpr->regs.pip_pbdir |=  SAP_QU_PT_B_PAR_SPI_SDI | SAP_QU_PT_B_PAR_SPI_SDO
	| SAP_QU_PT_B_PAR_SPI_CLK;
    dpr->regs.pip_pbodr &= ~(SAP_QU_PT_B_PAR_SPI_SDI | SAP_QU_PT_B_PAR_SPI_SDO
			     | SAP_QU_PT_B_PAR_SPI_CLK);
}
/*
 * =================================================================
 * Name: setup_spi_param()
 *
 * Function: set up SPI parameter
 *
 * =================================================================
 */
static void setup_spi_param(volatile quicc_dpr_t *dpr)
{
    volatile quicc_spi_param_t *spi_param_ptr;
    volatile quicc_bd_t *rmd, *tmd, *bd;   

    quicc_set_spi_pins (dpr);

    /* Initialize SPI mode register */
    dpr->regs.spi_spmode = QUICC_SPMR_DIV16 | QUICC_SPMR_REVDAT |
	QUICC_SPMR_MASTER | QUICC_SPMR_LEN(QUICC_SPI_8BITS);     

    spi_param_ptr = &(dpr->spi);

    /* get ptr to user_data_1 if DPRAM was at zero */
    bd = (quicc_bd_t *)(&(((quicc_dpr_t *)(0))->user_data_1));

    /* Set pointers to RX and tx buffer descriptors */
    spi_param_ptr->rbase = (int)(&bd[ISDN_SPI_RX_BD_IDX]);
    spi_param_ptr->tbase = (int)(&bd[ISDN_SPI_TX_BD_IDX]);

    /* big-endian, dma space (Motorola format, FC3 = 1) */
    spi_param_ptr->rfcr = QUICC_SPIFC_MOT;
    spi_param_ptr->tfcr = QUICC_SPIFC_MOT;

    /* max Rx buffer length is 0X4 */
    spi_param_ptr->mrblr = QUICC_SPI_MTU;

    /* program CR to execute INIT RX and TX PARAMS command for this
       channel. This loads rbase, tbase params in RBPTR & TBPTR */
    quicc_cr_cmd(QUICC_CHAN_SPI, QUICC_CPC_INIT_RXTX);
        
    /* clear SPI event reg */					
    dpr->regs.spi_spie = 0xFF;

    /* enable SPI */
    dpr->regs.spi_spmode |= QUICC_SPMR_ENASPI;
}

/*
 * =================================================================
 * Name: spi_isr() 
 *
 * Function: SPI interrupt service routine
 *
 * =================================================================
 */
void spi_isr(void)
{
    quicc_bd_t *tmd;                  /* TX buffer descr. */
    quicc_bd_t *rmd;                  /* RX buffer descr. */
    ulong ipr_reg;                    /* Contents of quicc IPR register */
    volatile ulong *ipr_reg_ptr;      /* Pointer to quicc IPR reg. */
    volatile ulong *isr_reg_ptr;      /* Pointer to quicc ISR reg. */
    uchar spi_event;                  /* Contents of SPI event reg. */
    volatile ushort *scce_ptr;        /* Pointer to quicc SCCE reg. */
    quicc_dpr_t *dpr;                 /* Ptr quicc parameter ram base */
    int i;

    /* Initialize various pointers */

    dpr = (quicc_dpr_t *) ADRSPC_QUICC_DPRAM;
    ipr_reg = dpr->regs.cpic_cipr;             /* Interrupt pending register */
    ipr_reg_ptr = &(dpr->regs.cpic_cipr);      /* Interrupt pending reg. pointer.*/
    isr_reg_ptr = &(dpr->regs.cpic_cisr);      /* Interrupt service reg. pointer.*/

    /* Check for SPI interrupt */
    if (ipr_reg &  QUICC_CIPR_SPI) {
	spi_event = dpr->regs.spi_spie;

	switch (spi_event){
	case QUICC_SPIE_MMERR:                /* Multi-Master Error */
	    interrupt_flag = SPI_M_MASTER;
	    break;
	case QUICC_SPIE_TXERR:                /* Tx Error */
	    interrupt_flag = SPI_TX_ERR;
	    break;
	case QUICC_SPIE_BSY:                  /* Busy Condition */
	    interrupt_flag = SPI_BSY;
	    break;
	default:
	    interrupt_flag = SPURIOUS_INTR;
	    break;
	}     
	/* Clear the Interrupt Pins */
	*ipr_reg_ptr =  QUICC_CISR_SPI;
	*isr_reg_ptr =  QUICC_CISR_SPI;
	dpr->regs.spi_spie = 0xff;
	return;
    }
}
/*
 * =================================================================
 * Name: sid_diag_int() 
 *
 * Function: SID interrupt handler 
 *
 * =================================================================
 */
void sid_diag_int(void)
{
    quicc_bd_t *tmd;                  /* TX buffer descr. */
    quicc_bd_t *rmd;                  /* RX buffer descr. */
    ulong ipr_reg;                    /* Contents of quicc IPR register */
    volatile ulong *ipr_reg_ptr;      /* Pointer to quicc IPR reg. */
    volatile ulong *isr_reg_ptr;      /* Pointer to quicc ISR reg. */
    uchar sid_status;                 /* Contents of SID status reg. */
    volatile ushort *scce_ptr;        /* Pointer to quicc SCCE reg. */
    quicc_dpr_t *dpr;                 /* Pointer to quicc parameter RAM base */
    int i;
    uchar reply;

    /* Initialize various pointers */
    dpr = (quicc_dpr_t *) ADRSPC_QUICC_DPRAM;
    ipr_reg = dpr->regs.cpic_cipr;             /* Interrupt pending register */
    ipr_reg_ptr = &(dpr->regs.cpic_cipr);      /* Interrupt pending reg. pointer.*/
    isr_reg_ptr = &(dpr->regs.cpic_cisr);      /* Interrupt service reg. pointer.*/

    /* SID interrupt is connected to PC1. Read the status register in
       TP3420 and take appropriate action. */
    sid_status = rd_sid(dpr);

    /* clear interrupt bits */
    *ipr_reg_ptr =  QUICC_CIPR_PC1;
    *isr_reg_ptr =  QUICC_CISR_PC1;

    switch (sid_status) {
    case TP3420_SRF_LSD:   /* Line Signal Detect, send PUP
			      to start Activation procedure. */
	wrt_sid(dpr, TP3420_CRF_PUP);
	break;
    case TP3420_SRF_AP:   /* Activation Pending, send AR 
			     to continue Activation */
   	wrt_sid(dpr, TP3420_CRF_AR);
	break;
    case TP3420_SRF_CON:   /* D-chan collision */
	interrupt_flag = SID_D_CHAN_COLLISION;
	break;
    case TP3420_SRF_AI:   /* Activation Indication, ready to transmit */
	interrupt_flag = SID_ACTIVATION_INDICATION;
	break;
    case TP3420_SRF_EI:   /* Loss of Frame Alignment */
	interrupt_flag = SID_LOSS_FR_ALIGN;
	break;
    default:
	interrupt_flag = SPURIOUS_INTR;
	break;
    }
    return;
}
/*
 * =================================================================
 * Name: scc_int() 
 *
 * Function: SCC interrupt handler 
 *
 * =================================================================
 */
void scc_int(void)
{
    quicc_bd_t *tmd1, *tmd2, *tmd3;   /* TX buffer descr. */
    quicc_bd_t *rmd1, *rmd2, *rmd3;   /* RX buffer descr. */
    ulong ipr_reg;                    /* Contents of quicc IPR register */
    volatile ulong *ipr_reg_ptr;      /* Pointer to quicc IPR reg. */
    volatile ulong *isr_reg_ptr;      /* Pointer to quicc ISR reg. */
    volatile ushort *scce_ptr;        /* Pointer to quicc SCCE reg. */
    uint scce_val;                    /* value of SCC event reg */
    ulong int_clear;                  /* Mask to clear the interrupt */
    quicc_dpr_t *dpr;                 /* Ptr quicc parameter ram base */
    int i;

    /* Initialize various pointers */
    dpr = (quicc_dpr_t *) ADRSPC_QUICC_DPRAM;

    rmd1 = &((quicc_bd_t *)(((quicc_dpr_t *)ADRSPC_QUICC_DPRAM)->user_data_1))[ISDN_SCC2_RX_BD_IDX];

    rmd2 = &((quicc_bd_t *)(((quicc_dpr_t *)ADRSPC_QUICC_DPRAM)->user_data_1))[ISDN_SCC3_RX_BD_IDX];

    rmd3 = &((quicc_bd_t *)(((quicc_dpr_t *)ADRSPC_QUICC_DPRAM)->user_data_1))[ISDN_SCC4_RX_BD_IDX];
 
    tmd1 = &((quicc_bd_t *)(((quicc_dpr_t *)ADRSPC_QUICC_DPRAM)->user_data_1))[ISDN_SCC2_TX_BD_IDX];
  
    tmd2 = &((quicc_bd_t *)(((quicc_dpr_t *)ADRSPC_QUICC_DPRAM)->user_data_1))[ISDN_SCC3_TX_BD_IDX];
  
    tmd3 = &((quicc_bd_t *)(((quicc_dpr_t *)ADRSPC_QUICC_DPRAM)->user_data_1))[ISDN_SCC4_TX_BD_IDX];
 
    ipr_reg = dpr->regs.cpic_cipr;             /* Interrupt pending register */

    /* SCC2, SCC3, SCC4 interrupt */
    
    if (ipr_reg & QUICC_CIPR_SCC2) {
	scce_ptr = &(dpr->regs.scc[1].scc_scce);
	scce_val = dpr->regs.scc[1].scc_scce;
	dpr->regs.scc[1].scc_scce = scce_val;
	int_clear =  QUICC_CISR_SCC2;
	quicc_scc2_int = 1;
 
	/*  find out the cause of interrupt  */
	if (scce_val & QUICC_SCCE_RXFRAME) {	   
	    bri_rx_intr(rmd1);
	} else
	    bri_misc_intr(scce_ptr, rmd1, tmd1);
    } 
    if (ipr_reg & QUICC_CIPR_SCC3) {
	scce_ptr = &(dpr->regs.scc[2].scc_scce);
	scce_val = dpr->regs.scc[2].scc_scce;
	dpr->regs.scc[2].scc_scce = scce_val;
	int_clear = QUICC_CISR_SCC3;
	quicc_scc3_int = 1;

	/*  find out the cause of interrupt  */
	if (scce_val & QUICC_SCCE_RXFRAME) {	   
	    bri_rx_intr(rmd2);
	} else
	    bri_misc_intr(scce_ptr, rmd2, tmd2);
    }
    if (ipr_reg & QUICC_CIPR_SCC4) {
	scce_ptr = &(dpr->regs.scc[3].scc_scce);
	scce_val = dpr->regs.scc[3].scc_scce;
	dpr->regs.scc[3].scc_scce = scce_val;
	int_clear = QUICC_CISR_SCC4;
 	quicc_scc4_int = 1;

	/*  find out the cause of interrupt  */
	if (scce_val & QUICC_SCCE_RXFRAME) {	   
	    bri_rx_intr(rmd3);
	} else
	    bri_misc_intr(scce_ptr, rmd3, tmd3);
    }
    /* Clear the interrupt bits  */

    dpr->regs.cpic_cisr = int_clear;
    sleep(100);
    reset_io();
    return;
}
/*
 * =================================================================
 * Name: bri_rx_intr()
 *
 * Function: 
 *
 * =================================================================
 */
int bri_rx_intr(quicc_bd_t *rmd)
{
    /* if data reception is aborted due to error or buffer full */
    if ((rmd->status & QUICC_BDSTAT_RX_EMPTY) == 0) { 
	
	/* Check for RX errors */
	if ((rmd->status & 0x3F) != 0) {
	    bri_rx_error = 1; /* set flag for receive error */
	    
	    if (rmd->status & QUICC_BDSTAT_RX_FLV) 
		interrupt_flag = RX_OV_FL; 
	    
	    if (rmd->status & QUICC_BDSTAT_RX_NOFA)
		interrupt_flag =  NON_OCT_ALGN;

	    if (rmd->status & QUICC_BDSTAT_RX_ABORT)
		interrupt_flag = RX_ABORT;

	    if (rmd->status & QUICC_BDSTAT_RX_CRCERR) 
		interrupt_flag = CRC_ERR;

	    if (rmd->status & QUICC_BDSTAT_RX_OVRRUN) 
		interrupt_flag = OVR_ERR;

	    if (rmd->status & QUICC_BDSTAT_RX_CDLOST) 
		interrupt_flag =  CARR_LOST;
	    else 	
		interrupt_flag = SPURIOUS_INTR;

	    return(1);
	}
    }    
}
/*
 * =================================================================
 * Name: bri_tx_intr()
 *
 * Function: 
 *
 * =================================================================
 */
int bri_tx_intr(quicc_bd_t *tmd)
{
    tmd = &((quicc_bd_t *)(((quicc_dpr_t *)ADRSPC_QUICC_DPRAM)->user_data_1))[ISDN_SCC3_TX_BD_IDX];
}
/*
 * =================================================================
 * Name: bri_misc_intr()
 *
 * Function: 
 *
 * =================================================================
 */
int bri_misc_intr(volatile ushort *scce_ptr, quicc_bd_t *rmd, quicc_bd_t *tmd)
{
    /* Check for Transmit Error Interrupt */
    if (*scce_ptr & QUICC_SCCE_TXERR) {
	*scce_ptr = QUICC_SCCE_TXERR;         /* clear the bit */
	bri_tx_error = 1;                     /* set flag_ Tx error */
    }
    /*  Check for Busy Condition Interrupt */
    if (*scce_ptr & QUICC_SCCE_BSY) {
	*scce_ptr = QUICC_SCCE_BSY;           /* clear the bit */ 
	bri_busy_error = 1;                   /* set flag for busy error */
    }
}
/*
 * ================================================================================
 * quicc_timer2_int()
 * Function: This timer interrupt happens every 8Khz to generate the SYNC signal
 * at pin 11 of port C
 * History:  Pin 11 generates sync pulse  from 360 (rev 1)
 *           Pin 10 generates sync pulse out from 360 to SID and is tied to pin 11 
 *           to provide sync (rev 2) to 360.
 * =================================================================================
 */
int quicc_timer2_int(void)
{
    quicc_dpr_t *dpr;                         
    volatile ulong *ipr_reg_ptr;              /* Pointer to quicc IPR reg. */
    volatile ulong *isr_reg_ptr;              /* Pointer to quicc ISR reg. */
    volatile quicc_internal_regs_t *qir_p;    /* QUICC Internal reg */
    ulong ipr_reg,i;                          /* Contents of QUICC IPR register */
    ushort ter;

    dpr = (quicc_dpr_t *)ADRSPC_QUICC_DPRAM;
    qir_p = &(dpr->regs);
    ipr_reg = dpr->regs.cpic_cipr;            /* Interrupt pending register */
    ter = qir_p->tmr_ter2;

    /* reset so that the timer will negate the interrupt */
    qir_p->tmr_ter2 = ter; 

    /* set PortC pin 11 to high */ 
    QUICC_SET_PC11(dpr);                    

    /* produce a pulse */
    for(i=0; i<8; i++);   
                  
    /* set PortC pin 11 to low */
    QUICC_CLR_PC11(dpr);                    

    /* clear the timer2 interrupt */
    dpr->regs.cpic_cipr = QUICC_CISR_TIMER2; 
    dpr->regs.cpic_cisr = QUICC_CISR_TIMER2;

    /* Interrupt pending register */
    ipr_reg = dpr->regs.cpic_cipr;         
}
/*
 * =================================================================
 * Name: test_sccs()
 *
 * Function: Test each SCC independently, in loopback mode.
 * 
 * =================================================================
 */
static int test_sccs(quicc_dpr_t *dpr, int mode)
{
    volatile quicc_scc_param_t *quicc_param_ram;  /* quicc param ram */
    volatile quicc_hdlc_param_t *quicc_prot_spec; /* quicc protocol spec. ram */
    volatile quicc_internal_regs_t *qir_p;        /* quicc internal reg */  
    quicc_bd_t *tmd, *rmd;                        /* tx, rx buffer descr. */
    volatile quicc_hdlc_param_t *hdlc_ptr;

    long  buff, *address, data, data1, *address_rx;
    int i, y, count;
    
    qir_p = &(dpr->regs);
  
    rmd = &((quicc_bd_t *)(((quicc_dpr_t *)ADRSPC_QUICC_DPRAM)->user_data_1))[ISDN_SCC2_RX_BD_IDX];
    tmd = &((quicc_bd_t *)(((quicc_dpr_t *)ADRSPC_QUICC_DPRAM)->user_data_1))[ISDN_SCC2_TX_BD_IDX];

    /* mask interrupt from quicc */
    mask_quicc_int(dpr); 

    /* Setup and Init the Rx and Tx buffer descriptors */
    setup_init_bd(mode); 

    /* init SCC2, don't set ENT, ENR bits in GSMR */
    init_scc2(dpr, SCCLP, mode);

    init_flags();

    /* enable all interrupts */
    if (mode == INTR_MODE){
	chgint(0);
	qir_p->cpic_cimr = QUICC_CIMR_SCC2 ;      /* unmask SCC2 inrq  */
    }
    /* SSC2 loopback */
    prpass(testpass, "SCC2 loopback");

    /* enable TX and Rx bits for SCC2 */
    qir_p->scc[1].scc_gsmrl |=  QUICC_SCC_GSMRL_ENT | QUICC_SCC_GSMRL_ENR;

    /* set the ready bit in transmit bd */
    tmd->status |=  QUICC_BDSTAT_TX_RDY; 
    
    /* check if ready bit is set */
    if (!(pstat((ulong *)&tmd->status, 2, 1, QUICC_BDSTAT_TX_RDY, QUICC_BDSTAT_TX_RDY,
		1000, 0)))
	cterr('w',0,"SCC2:Transmit BD is not ready");

    /* check Empty bit in RxBD to see if packet is received  */
    if (!(pstat((ulong *)&rmd->status, 2, 0, QUICC_BDSTAT_RX_EMPTY,
		QUICC_BDSTAT_RX_EMPTY, 1000, 0)))
	cterr('w',0,"SCC2: Receive BD did not receive the packet");

    if (mode == INTR_MODE){
	if(bri_rx_error || bri_tx_error || bri_busy_error){
	    if(bri_rx_error)     cterr('f',0,"bri: error in receive");
	    if(bri_tx_error)     cterr('f',0,"bri: error in transmit");
	    if(bri_busy_error)   cterr('f',0,"a packet was received and discarded due to lack of buffers");
	    cterr('f',0,"error in SCC2 internal loopback.");
	    return(1);
	}
    }
    /* successful transmit and receive, now check the data */
    if(cmpbyte(tmd->buf_ptr, rmd->buf_ptr, TX_LENGTH)){
	cterr('f',0,"SCC2: error in comparing transmit and receive data.");
	return(1);
    }
    /* check interrupt flag to make sure if interrupt has happened */

    if (mode == INTR_MODE){
	if (quicc_scc2_int != 1){
	    cterr('f',0,"error in SCC2 internal loopback.\n"
		  "SCC2  does not generate interrupt");
	    return(1);
	}
    }
    /* SCC3 loopback */
    prpass(testpass, "SCC3 loopback");
    
    rmd = &((quicc_bd_t *)(((quicc_dpr_t *)ADRSPC_QUICC_DPRAM)->user_data_1))[ISDN_SCC3_RX_BD_IDX];
    tmd = &((quicc_bd_t *)(((quicc_dpr_t *)ADRSPC_QUICC_DPRAM)->user_data_1))[ISDN_SCC3_TX_BD_IDX];
    mask_quicc_int(dpr);   /* mask interrupt from quicc */
  
    /* init SCC3, don't set ENT, ENR bits in GSMR */
    init_scc3(dpr, SCCLP, mode);
   
    init_flags();

    if (mode == INTR_MODE)
	qir_p->cpic_cimr = QUICC_CIMR_SCC3 ;      /* unmask SCC3 inrq  */ 

    /* enable TX and Rx bits for SCC3 */
    qir_p->scc[2].scc_gsmrl |= QUICC_SCC_GSMRL_ENT | QUICC_SCC_GSMRL_ENR;

    /* set the ready bit in transmit bd */
    tmd->status |=  QUICC_BDSTAT_TX_RDY; 

    /* check if ready bit is set */
    if (!(pstat((ulong *)&tmd->status, 2, 1, QUICC_BDSTAT_TX_RDY, QUICC_BDSTAT_TX_RDY,
		1000, 0)))
	cterr('w',0,"SCC3:Transmit BD is not ready");
  
    /* check Empty bit in RxBD to see if packet is received  */
    if (!(pstat((ulong *)&rmd->status, 2, 0,
		QUICC_BDSTAT_RX_EMPTY, QUICC_BDSTAT_RX_EMPTY, 1000, 0)))
	cterr('w',0,"SCC3: Receive BD did not receive the packet");
 
    if (mode == INTR_MODE){
	if(bri_rx_error || bri_tx_error || bri_busy_error){
	    if(bri_rx_error)     cterr('f',0,"bri: error in receive");
	    if(bri_tx_error)     cterr('f',0,"bri: error in transmit");
	    if(bri_busy_error)   cterr('f',0,"a packet was received and discarded due to lack of buffers");
	    cterr('f',0,"error in SCC3 internal loopback.");
	    return(1);
	}
    }
    /* successful transmit and receive, now check the data */
    if(cmpbyte(tmd->buf_ptr, rmd->buf_ptr, TX_LENGTH)){
        cterr('f',0,"SCC3: error in comparing transmit and receive data.");
        return(1);
    }
    /* check interrupt flag to make sure if interrupt has happened */
    if (mode == INTR_MODE){
	if (quicc_scc3_int != 1){
	    cterr('f',0,"error in SCC3 internal loopback.\n"
		  "SCC3 does not generate interrupt");
	    return(1);
	}
    }
    /* SCC4 loopback */
    prpass(testpass, "SCC4 loopback");
    
    rmd = &((quicc_bd_t *)(((quicc_dpr_t *)ADRSPC_QUICC_DPRAM)->user_data_1))[ISDN_SCC4_RX_BD_IDX];
    tmd = &((quicc_bd_t *)(((quicc_dpr_t *)ADRSPC_QUICC_DPRAM)->user_data_1))[ISDN_SCC4_TX_BD_IDX];

    mask_quicc_int(dpr);   /* mask interrupt from quicc */
    
    /* init SCC4, don't set ENT& ENR bits in GSMR */
    init_scc4(dpr, SCCLP, mode);
    init_flags();

    if (mode == INTR_MODE)
	qir_p->cpic_cimr = QUICC_CIMR_SCC4 ;      /* unmask SCC4 inrq  */

    /* enable TX and Rx bits for SCC4 */
    qir_p->scc[3].scc_gsmrl |= QUICC_SCC_GSMRL_ENT | QUICC_SCC_GSMRL_ENR;

    /* set the ready bit */
    tmd->status |=  QUICC_BDSTAT_TX_RDY; 
 
    /* check if ready bit is set */
    if (!(pstat((ulong *)&tmd->status, 2, 1, QUICC_BDSTAT_TX_RDY, QUICC_BDSTAT_TX_RDY,
		1000, 0)))
	cterr('w',0,"SCC4:Transmit BD is not ready");

    /* check Empty bit in RxBD to see if packet is received  */  
    if (!(pstat((ulong *)&rmd->status, 2, 0,
		QUICC_BDSTAT_RX_EMPTY, QUICC_BDSTAT_RX_EMPTY, 1000, 0)))
	cterr('w',0,"SCC4: Receive BD did not receive the packet");

    if (mode == INTR_MODE){
	if(bri_rx_error || bri_tx_error || bri_busy_error){
	    if(bri_rx_error)     cterr('f',0,"bri: error in receive");
	    if(bri_tx_error)     cterr('f',0,"bri: error in transmit");
	    if(bri_busy_error)   cterr('f',0,"a packet was received and discarded due to lack of buffers");
	    cterr('f',0,"error in SCC4 internal loopback.");
	    return(1);
	}
    }
    /* successful transmit and receive, now check the data */
    if(cmpbyte(tmd->buf_ptr, rmd->buf_ptr, TX_LENGTH)){
        cterr('f',0,"SCC4: error in comparing data of transmit and receive.");
        return(1);
    }
    /* check interrupt flag to make sure if interrupt has happened */
    if (mode == INTR_MODE){
	if (quicc_scc4_int != 1){
	    cterr('f',0,"error in SCC4 internal loopback.\n"
		  "SCC4 does not generate interrupt");
	    return(1);
	}
    }
    if (mode == INTR_MODE){
	chgint(4);
    }
    return(0);
}
/*
 * =================================================================
 * Name: quicc_spi_lpbk()
 *
 * Function: SPI Internal loopback test
 *
 * =================================================================
 */
static int quicc_spi_lpbk(quicc_dpr_t *dpr, int mode)
{
    volatile quicc_internal_regs_t *qir_p;         /* quicc internal reg */
    volatile quicc_bd_t *rmd, *tmd;                /* spi buffer descriptors */
    int i, count;
    uchar spi_event;                  /* Contents of SPI event reg. */


    count = 0x10000;

    /* Set up the register pointers */
    qir_p = &(dpr->regs);
    spi_event = dpr->regs.spi_spie;

    prpass(testpass, "SPI loopback");

    /* setup BDs */
    setup_init_bd(mode);

    /* setup SPI params */
    setup_spi_param(dpr);

    /* set max Rx buffer length */
    dpr->spi.mrblr = MRBLR;

    tmd = &((quicc_bd_t *)(((quicc_dpr_t *)ADRSPC_QUICC_DPRAM)->user_data_1))[ISDN_SPI_TX_BD_IDX];

    rmd = &((quicc_bd_t *)(((quicc_dpr_t *)ADRSPC_QUICC_DPRAM)->user_data_1))[ISDN_SPI_RX_BD_IDX];
    rmd->buf_ptr = (uchar *) ISDN_RX_BD4_MEM;            
    tmd->buf_ptr = (uchar *) ISDN_TX_BD4_MEM;
	   
    /* mask interrupt from quicc */
    mask_quicc_int(dpr);
    
    if (mode == INTR_MODE){
	chgint(0);
	/* Enable SPI interrupt */
	qir_p->cpic_cimr = QUICC_CIMR_SPI;
    }
    /* setup loopback mode for SPI */
    qir_p->spi_spmode |= QUICC_SPMR_LOOP;
    
    /* set the ready bit in transmit bd */
    tmd->status |=  QUICC_BDSTAT_TX_RDY; 
  
    /* check if ready bit is set */
    if (!(pstat((ulong *)&tmd->status, 2, 1, QUICC_BDSTAT_TX_RDY, QUICC_BDSTAT_TX_RDY,
		1000, 0)))
	cterr('w',0,"SPI:Transmit BD is not ready");

    /* start transmit */
    qir_p->spi_spcom =  QUICC_SPCOM_START;

    /* check Empty bit in RxBD to see if packet is received  */
    if (!(pstat((ulong *)&rmd->status, 2, 0,
		QUICC_BDSTAT_RX_EMPTY, QUICC_BDSTAT_RX_EMPTY, 1000, 0)))
	cterr('w',0,"SPI did not receive the packet");
    
    /* Now check data in Rx buffer descriptor */
    if(cmpbyte(tmd->buf_ptr, rmd->buf_ptr, TX_LENGTH)){
	cterr('f',0,"SPI: error in comparing data of transmit and receive.");
	return(1);
    }
    if (mode == INTR_MODE){
	chgint(4);
    }
    return(0);
}
/*
 * =================================================================
 * Name: quicc_sid_int_lpbk()
 *
 * Function: Internal loopback test for B1 and B2 channels
 *
 * =================================================================
 */
static int quicc_sid_int_lpbk(quicc_dpr_t *dpr, int mode)
{
    volatile quicc_scc_param_t *quicc_param_ram;   /* quicc param ram */
    volatile quicc_hdlc_param_t *quicc_prot_spec;  /* quicc protocol spec. ram */
    volatile quicc_internal_regs_t *qir_p;         /* quicc internal reg */
    quicc_bd_t *rmd1, *rmd2, *tmd1, *tmd2, *rmd3, *tmd3; /* rx, tx buffer descr. */
    volatile quicc_hdlc_param_t *hdlc_ptr;

    prpass(testpass, "TP3420 Internal loopback");
    qir_p = &(dpr->regs);
 
    /* mask interrupt from quicc */
    mask_quicc_int(dpr);

    /* Setup and Init the Rx and Tx buffer descriptors */
    setup_init_bd(mode);     

    /* setup parallel I/O ports */
    setup_pio_ports(dpr); 

    /* setup SPI params */
    setup_spi_param(dpr);

    /* initialize SCCs */    
    init_scc2(dpr, SID, mode);
    init_scc3(dpr, SID, mode);
    init_scc4(dpr, SID, mode);

    init_flags();
    
    /* initialize SIRAM, program SICR, SIMODE, SIGMR, enable Time
       Division Multiplexer. Enabling TDM will start data transfer */ 

    quicc_siram_init(dpr);
    if (mode == INTR_MODE){
	
	/* allow all interrupts */
	chgint(0);

	/* unmask SCC2, SC3, SCC4, PC1 inrq  */
	qir_p->cpic_cimr |= QUICC_CIMR_SCC2 |  QUICC_CIMR_SCC3 |
	    QUICC_CIMR_SCC4 | QUICC_CIMR_PC1;     
    }
    /* now set TP3420 in loopback mode */
    wrt_sid(dpr, TP3420_CRF_TEM);      /* TE Master mode */
    wrt_sid(dpr, TP3420_CRF_EID);      /* EOM Interrupt Disabled */
    wrt_sid(dpr, TP3420_CRF_DIF4);     /* DSI Format 4 */
    wrt_sid(dpr, TP3420_PSS_PINDEF);   /* Redefine pin signals */
    wrt_sid(dpr, TP3420_CRF_DCKE);     /* D channel clock enable */
    wrt_sid(dpr, TP3420_CRF_DACCE);    /* D channel access enable */
    wrt_sid(dpr, TP3420_BCHAN_BCLK4);  /* B channel clk = 2.56 MHz */
    wrt_sid(dpr, TP3420_CRF_B1E);      /* B1 channel enable */
    wrt_sid(dpr, TP3420_CRF_B2E);      /* B2 channel enable */
    wrt_sid(dpr, TP3420_CRF_LBL1);     /* B1 channel loopback */
    wrt_sid(dpr, TP3420_CRF_LBL2);     /* B2 channel loopback */
    wrt_sid(dpr, TP3420_CRF_LBS);      /* Loopback towards digital interface */
    sleep(10);
    wrt_sid(dpr, TP3420_CRF_PUP);      /* Power up */

    rmd1 = &((quicc_bd_t *)(((quicc_dpr_t *)ADRSPC_QUICC_DPRAM)->user_data_1))[ISDN_SCC2_RX_BD_IDX];
    tmd1 = &((quicc_bd_t *)(((quicc_dpr_t *)ADRSPC_QUICC_DPRAM)->user_data_1))[ISDN_SCC2_TX_BD_IDX];
    rmd2 = &((quicc_bd_t *)(((quicc_dpr_t *)ADRSPC_QUICC_DPRAM)->user_data_1))[ISDN_SCC4_RX_BD_IDX];
    tmd2 = &((quicc_bd_t *)(((quicc_dpr_t *)ADRSPC_QUICC_DPRAM)->user_data_1))[ISDN_SCC4_TX_BD_IDX];
    rmd3 = &((quicc_bd_t *)(((quicc_dpr_t *)ADRSPC_QUICC_DPRAM)->user_data_1))[ISDN_SCC3_RX_BD_IDX];
    tmd3 = &((quicc_bd_t *)(((quicc_dpr_t *)ADRSPC_QUICC_DPRAM)->user_data_1))[ISDN_SCC3_TX_BD_IDX];
 
    /* enable Tx and Rx for B1, B2 and D channels _ SCC2, SCC3, SCC4 */
    qir_p->scc[1].scc_gsmrl |= QUICC_SCC_GSMRL_ENT | QUICC_SCC_GSMRL_ENR;
   
    qir_p->scc[3].scc_gsmrl |= QUICC_SCC_GSMRL_ENT | QUICC_SCC_GSMRL_ENR;

    /* set up gsmrl for SCC3 in local loopback and HDLC mode. This is
       because the data can't be looped back in TP3420 from D channel,
       as is done for B1 and B2 channels. For D channel the data is
       looped back from within the SCC3. This test will use the
       External clock produced by TP3420. The differance between this 
       test and the SCC3 loopback test is that former uses External 
       clock whereas the latter uses the clock BRGC2 in the QUICC
       chip. This test verifies that the D channel TXCLK and RXCLK work */

    qir_p->scc[2].scc_gsmrl = QUICC_SCC_GSMRL_DIAG(QUICC_LOCAL_LOOPBACK);
    qir_p->scc[2].scc_gsmrl |= QUICC_SCC_GSMRL_ENT | QUICC_SCC_GSMRL_ENR;
    sleep(10);

    /* set the ready bit for SCC2, SCC3, SCC4 buffer descriptors */
    tmd1->status |=  QUICC_BDSTAT_TX_RDY; 
    tmd2->status |=  QUICC_BDSTAT_TX_RDY; 
    tmd3->status |=  QUICC_BDSTAT_TX_RDY; 

    /* check if ready bit is set */
    if (!(pstat((ulong *)&tmd1->status, 2, 1, QUICC_BDSTAT_TX_RDY, QUICC_BDSTAT_TX_RDY,
		2000, 0)))
	cterr('w',0,"SCC2:Transmit BD is not ready");

    /* check if ready bit is set */
    if (!(pstat((ulong *)&tmd2->status, 2, 1, QUICC_BDSTAT_TX_RDY,QUICC_BDSTAT_TX_RDY,
		2000, 0)))
	cterr('w',0,"SCC4:Transmit BD is not ready");
    
    /* check if ready bit is set */
    if (!(pstat((ulong *)&tmd3->status, 2, 1, QUICC_BDSTAT_TX_RDY,QUICC_BDSTAT_TX_RDY,
		2000, 0)))
	cterr('w',0,"SCC3:Transmit BD is not ready");
  
    /* check Empty bit in RxBD to see if packet is received  */
    if (!(pstat((ulong *)&rmd1->status, 2, 0,
		QUICC_BDSTAT_RX_EMPTY, QUICC_BDSTAT_RX_EMPTY,
		2000, 0)))
	cterr('w',0,"SCC2: Receive BD did not receive the packet");

    /* check Empty bit in RxBD to see if packet is received  */
    if (!(pstat((ulong *)&rmd2->status, 2, 0, QUICC_BDSTAT_RX_EMPTY,  
		QUICC_BDSTAT_RX_EMPTY, 2000, 0)))
	cterr('w',0,"SCC4: Receive BD did not receive the packet");

    /* check Empty bit in RxBD to see if packet is received  */
    if (!(pstat((ulong *)&rmd3->status, 2, 0, QUICC_BDSTAT_RX_EMPTY,  
		QUICC_BDSTAT_RX_EMPTY, 2000, 0)))
	cterr('w',0,"SCC3: Receive BD did not receive the packet");

    /* check if there is an error in internal loopback */
    if (mode == INTR_MODE){
	if(bri_rx_error || bri_tx_error || bri_busy_error){
	    if(bri_rx_error)   cterr('f',0,"bri: error in receive");
	    if(bri_tx_error)   cterr('f',0,"bri: error in transmit");
	    if(bri_busy_error) cterr('f',0,"a received packet was discarded due to lack of buffers");
	    cterr('f',0,"there is an error in SID internal loopback.");
	    return(1);
	}
    }
    /* successful of transmit and receive, now check the data */
    if(cmpbyte(tmd1->buf_ptr, rmd1->buf_ptr,TX_LENGTH)){
	cterr('f',0,"SCC2 Error in comparing data of transmit and receive.");
	return(1);
    }
    if(cmpbyte(tmd2->buf_ptr, rmd2->buf_ptr, TX_LENGTH)){
	cterr('f',0,"SCC4 Error in comparing data of transmit and receive.");
	return(1);
    }
    if(cmpbyte(tmd3->buf_ptr, rmd3->buf_ptr, 200)){
	cterr('f',0,"SCC3 Error in comparing data of transmit and receive.");
	return(1);
    }
    if (mode == INTR_MODE){
	chgint(4);
    }
    return(0);
}
/*
 * =================================================================
 * Name: quicc_si_int_lpbk()
 *
 * Function: Internal loopback test for B1 and B2 channels at 
 *           Serial Interface in QUICC
 *
 * =================================================================
 */
static int quicc_si_int_lpbk(quicc_dpr_t *dpr, int mode)
{
    volatile quicc_scc_param_t *quicc_param_ram;   /* quicc param ram */
    volatile quicc_hdlc_param_t *quicc_prot_spec;  /* quicc protocol spec. ram */
    volatile quicc_internal_regs_t *qir_p;         /* quicc internal reg */
    quicc_bd_t *rmd1, *rmd2, *tmd1, *tmd2;         /* rx, tx buffer descr. */
    volatile quicc_hdlc_param_t *hdlc_ptr;

    prpass(testpass, "SI loopback");
    qir_p = &(dpr->regs);
  
    /* mask interrupt from quicc */
    mask_quicc_int(dpr);

    /* Setup and Init the Rx and Tx buffer descriptors */
    setup_init_bd(mode);     

    /* setup parallel I/O ports */
    setup_pio_ports(dpr); 

    /* setup SPI params */
    setup_spi_param(dpr);

    /* initialize SCCs */    
    init_scc2(dpr, SID, mode);
    init_scc3(dpr, SID, mode);
    init_scc4(dpr, SID, mode);

    init_flags();
    
    /* initialize SIRAM, program SICR, SIMODE, SIGMR, enable Time
       Division Multiplexer. Enabling TDM will start data transfer */ 

    quicc_siram_init(dpr);
    if (mode == INTR_MODE){

	/* allow all interrupts */
	chgint(0);

	/* unmask SCC2, SC3, SCC4, PC1 inrq  */
	qir_p->cpic_cimr |= QUICC_CIMR_SCC2 |  QUICC_CIMR_SCC3 |
	    QUICC_CIMR_SCC4 | QUICC_CIMR_PC1;     
    }
    /* now set TP3420 in loopback mode */
    wrt_sid(dpr, TP3420_CRF_TEM);      /* TE Master mode */
    wrt_sid(dpr, TP3420_CRF_EID);      /* EOM Interrupt Disabled */
    wrt_sid(dpr, TP3420_CRF_DIF4);     /* DSI Format 4 */
    wrt_sid(dpr, TP3420_PSS_PINDEF);   /* Redefine pin signals */
    wrt_sid(dpr, TP3420_CRF_DCKE);     /* D channel clock enable */
    wrt_sid(dpr, TP3420_CRF_DACCE);    /* D channel access enable */
    wrt_sid(dpr, TP3420_BCHAN_BCLK4);  /* B channel clk = 2.56 MHz */
    wrt_sid(dpr, TP3420_CRF_B1E);      /* B1 channel enable */
    wrt_sid(dpr, TP3420_CRF_B2E);      /* B2 channel enable */
    sleep(1);
    wrt_sid(dpr, TP3420_CRF_PUP);      /* Power up */

    rmd1 = &((quicc_bd_t *)(((quicc_dpr_t *)ADRSPC_QUICC_DPRAM)->user_data_1))[ISDN_SCC2_RX_BD_IDX];
    tmd1 = &((quicc_bd_t *)(((quicc_dpr_t *)ADRSPC_QUICC_DPRAM)->user_data_1))[ISDN_SCC2_TX_BD_IDX];
    rmd2 = &((quicc_bd_t *)(((quicc_dpr_t *)ADRSPC_QUICC_DPRAM)->user_data_1))[ISDN_SCC4_RX_BD_IDX];
    tmd2 = &((quicc_bd_t *)(((quicc_dpr_t *)ADRSPC_QUICC_DPRAM)->user_data_1))[ISDN_SCC4_TX_BD_IDX];

    /* set loopback mode for TDM in simode */
    qir_p->si_simode |= 0x00000800;

    /* enable Tx and Rx for B1 and B2 channels _ SCC2 and SCC4 */
    qir_p->scc[1].scc_gsmrl |= QUICC_SCC_GSMRL_ENT | QUICC_SCC_GSMRL_ENR;
    qir_p->scc[3].scc_gsmrl |= QUICC_SCC_GSMRL_ENT | QUICC_SCC_GSMRL_ENR;
    sleep(10);
   
    /* set the ready bit for SCC2 and SCC4 buffer descriptors */
    tmd1->status |=  QUICC_BDSTAT_TX_RDY; 
    tmd2->status |=  QUICC_BDSTAT_TX_RDY; 

    /* check if ready bit is set */
    if (!(pstat((ulong *)&tmd1->status, 2, 1, QUICC_BDSTAT_TX_RDY, QUICC_BDSTAT_TX_RDY,
		1000, 0)))
	cterr('w',0,"SCC2:Transmit BD is not ready");
    
    /* check if ready bit is set */
    if (!(pstat((ulong *)&tmd2->status, 2, 1, QUICC_BDSTAT_TX_RDY,QUICC_BDSTAT_TX_RDY,
		1000, 0)))
	cterr('w',0,"SCC4:Transmit BD is not ready");
   
    /* check Empty bit in RxBD to see if packet is received  */
    if (!(pstat((ulong *)&rmd1->status, 2, 0, QUICC_BDSTAT_RX_EMPTY,
		QUICC_BDSTAT_RX_EMPTY, 1000, 0)))
	cterr('w',0,"SCC2: Receive BD did not receive the packet");

    /* check Empty bit in RxBD to see if packet is received  */
    if (!(pstat((ulong *)&rmd2->status, 2, 0, QUICC_BDSTAT_RX_EMPTY,  
		QUICC_BDSTAT_RX_EMPTY, 1000, 0)))
	cterr('w',0,"SCC4: Receive BD did not receive the packet");
    
    /* check if there is an error in internal loopback */
    if (mode == INTR_MODE){
	if(bri_rx_error || bri_tx_error || bri_busy_error){
	    if(bri_rx_error)   cterr('f',0,"bri: error in receive");
	    if(bri_tx_error)   cterr('f',0,"bri: error in transmit");
	    if(bri_busy_error) cterr('f',0,"a received packet was discarded due to lack of buffers");
	    cterr('f',0,"there is an error in SID internal loopback.");
	    return(1);
	}
    }
    /* successful of transmit and receive, now check the data */
    if(cmpbyte(tmd1->buf_ptr, rmd1->buf_ptr, TX_LENGTH)){
	cterr('f',0,"SCC2 Error in comparing data of transmit and receive.");
	return(1);
    }
    if(cmpbyte(tmd2->buf_ptr, rmd2->buf_ptr, TX_LENGTH)){
	cterr('f',0,"SCC4 Error in comparing data of transmit and receive.");
	return(1);
    }
    if (mode == INTR_MODE){
	chgint(4);
    }
    return(0);
}
/*
 * =================================================================
 * Name: quicc_sid_ext_lpbk()
 *
 * Function: sid external loopback test
 *
 * =================================================================
 */
static int quicc_sid_ext_lpbk(quicc_dpr_t *dpr, int mode)
{
    volatile quicc_scc_param_t *quicc_param_ram;  /* quicc param ram */
    volatile quicc_hdlc_param_t *quicc_prot_spec; /* quicc protocol spec. ram */
    volatile quicc_internal_regs_t *qir_p;        /* quicc internal reg */
    quicc_bd_t *tmd1, *tmd2, *tmd3;               /* tx buffer descr. */
    quicc_bd_t *rmd1, *rmd2, *rmd3;               /* rx buffer descr. */
    int  i, y, loopmode;             
    ushort temp;
    uchar reply;

    qir_p = &(dpr->regs);
    
    prpass(testpass, "TP3420 External loopback");
    /* mask interrupt from quicc */
    mask_quicc_int(dpr);

    /* Setup and Init the Rx and Tx buffer descriptors */
    setup_init_bd(mode);

    /* setup parallel I/O ports */
    setup_pio_ports(dpr);

    /* set up PIO ports to perform the external loopback */
    setup_pio_ports_ext(dpr); 

    /* setup SPI params */
    setup_spi_param(dpr);

    wrt_sid(dpr, TP3420_CRF_TES);      /* NT mode, Fixed sampling */

    /* at this point, no clock, no frame is generated_ set it up */
    setup_clk(dpr);
    sleep(20);

    /* initialize SCCs */
    init_scc2(dpr, SID, mode);
    init_scc3(dpr, SID, mode);
    init_scc4(dpr, SID, mode);
    
    /* clear interrupt flags */
    init_flags();

    /* initialize SIRAM, program SICR, SIMODE, SIGMR, enable Time
       Division Multiplexer. Enabling TDM will start data transfer */ 

    quicc_siram_init(dpr);
    if (mode == INTR_MODE){
	/* allow all interrupts */
	chgint(0);

	/* unmask SCC2, SC3, SCC4, PC1 inrq  */
	qir_p->cpic_cimr |= QUICC_CIMR_SCC2 |  QUICC_CIMR_SCC3 |
	    QUICC_CIMR_SCC4 | QUICC_CIMR_PC1;   
    }

    /* now set sid in normal mode */
    wrt_sid(dpr, TP3420_CRF_PUP);      /* Power up */
    wrt_sid(dpr, TP3420_CRF_NTF);      /* NT mode, Fixed sampling */
    wrt_sid(dpr, TP3420_CRF_EID);      /* EOM Interrupt Disabled */
    wrt_sid(dpr, TP3420_CRF_DIF4);     /* DSI Format 4 */
    wrt_sid(dpr, TP3420_PSS_PINDEF);   /* Redefine pin signals */
    wrt_sid(dpr, TP3420_CRF_DCKE);     /* D channel clock enable */
    wrt_sid(dpr, TP3420_CRF_DACCE);    /* D channel access enable */
    wrt_sid(dpr, TP3420_CRF_CAL);      /* clear all loopbacks */
    wrt_sid(dpr, TP3420_CRF_B1E);      /* B1 channel enable */
    wrt_sid(dpr, TP3420_CRF_B2E);      /* B2 channel enable */
    wrt_sid(dpr, TP3420_CRF_PUP);      /* Power up */
    wrt_sid(dpr, TP3420_CRF_AR);       /* Activation Request */
    sleep(10);
    wrt_sid(dpr, TP3420_CRF_AR);       /* Activation Request */

    rmd1 = &((quicc_bd_t *)(((quicc_dpr_t *)ADRSPC_QUICC_DPRAM)->user_data_1))[ISDN_SCC2_RX_BD_IDX];
    tmd1 = &((quicc_bd_t *)(((quicc_dpr_t *)ADRSPC_QUICC_DPRAM)->user_data_1))[ISDN_SCC2_TX_BD_IDX];
    rmd2 = &((quicc_bd_t *)(((quicc_dpr_t *)ADRSPC_QUICC_DPRAM)->user_data_1))[ISDN_SCC4_RX_BD_IDX];
    tmd2 = &((quicc_bd_t *)(((quicc_dpr_t *)ADRSPC_QUICC_DPRAM)->user_data_1))[ISDN_SCC4_TX_BD_IDX];
    rmd3 = &((quicc_bd_t *)(((quicc_dpr_t *)ADRSPC_QUICC_DPRAM)->user_data_1))[ISDN_SCC3_RX_BD_IDX];
    tmd3 = &((quicc_bd_t *)(((quicc_dpr_t *)ADRSPC_QUICC_DPRAM)->user_data_1))[ISDN_SCC3_TX_BD_IDX];
	
    sleep(10);

    if (interrupt_flag != SID_ACTIVATION_INDICATION){
	cterr('f',0,"error in External loopback:\n"
	      "Either loopback connector not present or SID does not generate interrupt");
	return(1);
    }
    else if (interrupt_flag == SID_ACTIVATION_INDICATION){
	wrt_sid(dpr, TP3420_CRF_DR);       /* Deactivation request */
	wrt_sid(dpr, TP3420_CRF_PDN);      /* Power down */
	/* clear sid_activation_indication flag */ 
	interrupt_flag |= ~SID_ACTIVATION_INDICATION;	
	return(0);
    } 
}
/*
 * =================================================================
 * Name: print_errors():
 *
 * =================================================================
 */
static void print_errors(void)
{
    switch (interrupt_flag){
    case SPI_M_MASTER: 	
	cterr('w',0,"\nSPI: Multi-Master Error");
	break;
    case  SPI_TX_ERR:
	cterr('f',0,"\nSPI: Tx Error");
	break;
    case SPI_BSY:
	cterr('f',0,"\nSPI: Busy Condition\n");
	break;
    case SID_D_CHAN_COLLISION:
	cterr('f',0,"\nSID:D-chan collision\n");
	break;
    case SID_LOSS_FR_ALIGN:
	cterr('f',0,"\nSID:Loss of Frame Alignment\n");  
	break;
    case RX_OV_FL:
	cterr('f',0,"\nRX Frame Length Violation");
	break;
    case NON_OCT_ALGN:
	cterr('f',0,"\n RX Nonoctet Aligned Frame\n");	
	break;
    case RX_ABORT:
	cterr('f',0,"\n RX Abort Sequence\n");
	break;
    case CRC_ERR:
	cterr('f',0,"\n RX CRC Error\n");
	break;
    case OVR_ERR:
	cterr('f',0,"\n RX Overrun Error\n");
	break;
    case  CARR_LOST:
	cterr('f',0,"\n RX Carrier Detect Lost\n");
	break;
    case SPURIOUS_INTR:
	cterr('f',0,"\nSpurious Interrupt\n");
	break;
    }
}
/*
 * =================================================================
 * Name: quicc_bri_cleanup():
 *
 * =================================================================
 */
static int quicc_bri_cleanup(void)
{
    quicc_dpr_t *dpr;

    dpr = (quicc_dpr_t *)ADRSPC_QUICC_DPRAM;
    quicc_bri_test = BRI_TEST_OFF;      /* reset the BRI "test on" flag */
    if (interrupt_flag == SID_ACTIVATION_INDICATION){ 
	wrt_sid(dpr, TP3420_CRF_DR);    /* Deactivation request */
	wrt_sid(dpr, TP3420_CRF_PDN);   /* Power down */
    }
    cleanup_clk(dpr);                   /* clean up clk generate from timer2 */
    setup_pio_ports(dpr);
    reset_io();
    alarm (0);                          /* kill the alarm anyway */
    setvbr(&evec);                      /* restore rom monitor interrupt vector table */
    return ((int)SIG_DFL);              /* do the default action too */
}
/*
 * =================================================================
 * Name: bri_test():
 *
 * Function: Test BRI port functionality
 *
 * =================================================================
 */
int bri_test(int one)
{
    ushort control;        /* ADRSPC control bits */
    int chan_cnt;          /* Channel count */
    quicc_dpr_t *dpr;   /* quicc system ram */
    volatile quicc_internal_regs_t *qir_p; /* quicc internal reg */
    int i, phase, mode, rtn_val;
    SIG_PF savsig;

    testname("BRI Internal");
    savsig = signal(SIGINT, (SIG_PF)quicc_bri_cleanup);
    init_flags();  
    rtn_val = 0;

    dpr = (quicc_dpr_t *)ADRSPC_QUICC_DPRAM;
    qir_p = &(dpr->regs);

    /* copy rom monitor int. vector table into memory */
    cp_intvec_toram(ADRSPC_RAM);

    /* install interrupt handler */
    put_int_handler(dpr, QUICC_CPIC_INTERRUPT_SCC2, &scc2_int);
    put_int_handler(dpr, QUICC_CPIC_INTERRUPT_SCC3, &scc3_int);
    put_int_handler(dpr, QUICC_CPIC_INTERRUPT_SCC4, &scc4_int);
    put_int_handler(dpr, QUICC_CPIC_INTERRUPT_TIMER2, &timer2);
    put_int_handler(dpr, QUICC_CPIC_INTERRUPT_PC1, &pc1_int);
    put_int_handler(dpr, QUICC_CPIC_INTERRUPT_SPI, &spi_int);

    /* mask all interrupts from quicc */
    mask_quicc_int(dpr);
  
    quicc_bri_test = BRI_TEST_ON;      /* turn on the BRI test */
    phase = 1;

    /* Do the following tests in Polled and Interrupt mode */
    for (mode = POLL_MODE; mode < MAX_MODE; mode++){

	if(test_sccs(dpr, mode))               /* test SCCs in loopback mode */
	    rtn_val++;

	if(quicc_spi_lpbk(dpr, mode))          /* test the SPI loop back */
	    rtn_val++;

	if(quicc_si_int_lpbk(dpr, mode))       /* test SI internal loopback */
	    rtn_val++;

	if(quicc_sid_int_lpbk(dpr, mode))      /* test SID internal loopback */
	    rtn_val++;

    }
    print_errors();
    quicc_bri_cleanup ();
    quicc_bri_test = BRI_TEST_OFF;             /* BRI test is off */
    prcomplete(testpass, errcount, 0);
    signal(SIGINT, savsig);
    return(rtn_val);
}
/*
 * =================================================================
 * Name: bri_ext_test():
 *
 * Function: Test BRI external loopback functionality
 *
 * =================================================================
 */
int bri_ext_test(int one)
{
    ushort control;        /* ADRSPC control bits */
    int chan_cnt;          /* Channel count */
    quicc_dpr_t *dpr;   /* quicc system ram */
    volatile quicc_internal_regs_t *qir_p; /* quicc internal reg */
    int i, phase, mode, rtn_val;
    SIG_PF savsig;

    testname("BRI External loopback");
    savsig = signal(SIGINT, (SIG_PF)quicc_bri_cleanup);
    init_flags();  
    rtn_val = 0;

    dpr = (quicc_dpr_t *)ADRSPC_QUICC_DPRAM;
    qir_p = &(dpr->regs);

    /* copy rom monitor int. vector table into memory */
    cp_intvec_toram(ADRSPC_RAM);

    /* install interrupt handler */
    put_int_handler(dpr, QUICC_CPIC_INTERRUPT_SCC2, &scc2_int);
    put_int_handler(dpr, QUICC_CPIC_INTERRUPT_SCC3, &scc3_int);
    put_int_handler(dpr, QUICC_CPIC_INTERRUPT_SCC4, &scc4_int);
    put_int_handler(dpr, QUICC_CPIC_INTERRUPT_TIMER2, &timer2);
    put_int_handler(dpr, QUICC_CPIC_INTERRUPT_PC1, &pc1_int);
    put_int_handler(dpr, QUICC_CPIC_INTERRUPT_SPI, &spi_int);

    /* mask all interrupts from quicc */
    mask_quicc_int(dpr);

    /* BRI test is in progress */  
    quicc_bri_test = BRI_TEST_ON;    
    phase = 1;

    /* test SID external loopback in interrupt mode */
    mode = INTR_MODE;

    if(quicc_sid_ext_lpbk(dpr, mode))        
	rtn_val++;
    
    print_errors();
    quicc_bri_cleanup ();
    prcomplete(testpass, errcount, 0);
    signal(SIGINT, savsig);
    return(rtn_val);
}

/* end of module */
