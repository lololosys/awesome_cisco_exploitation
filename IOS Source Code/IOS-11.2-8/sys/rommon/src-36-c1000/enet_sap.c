/* $Id: enet_sap.c,v 3.3.28.1 1996/03/21 23:28:57 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/rommon/src-36-c1000/enet_sap.c,v $
 *------------------------------------------------------------------
 * enet_sap.c
 *
 * December 1994, Yvette Yuen
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Sapphire Ethernet diagnostic
 *------------------------------------------------------------------
 * $Log: enet_sap.c,v $
 * Revision 3.3.28.1  1996/03/21  23:28:57  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.2.2.2  1996/02/08  08:58:32  gstovall
 * Branch: ELC_branch
 * Sync to ELC_baseline_960205.
 *
 * Revision 3.2.2.1  1995/12/01  04:21:53  mbeesley
 * Branch: ELC_branch
 * Reorganize Rom monitor header files to make it easier to port
 * to new platforms.
 *
 * Revision 3.3  1996/01/22  09:06:43  mdb
 * CSCdi47263:  change definition of NULL to ((void *) 0) from 0
 *
 * Revision 3.2  1995/11/17  18:43:55  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:07:16  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:24:01  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/06/07  22:39:08  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "monitor.h"
#include "error.h"
#include "quicc.h"
#include "enet_sap.h"

#define NULL ((void *) 0)
#define CBUFSIZ        400

extern long *scc1_int, *evec;

static volatile char	enet_rx_buf[ENET_BUF_SIZE];
static volatile char	enet_tx_buf[ENET_BUF_SIZE];
static volatile short  event; /* ethernet interrupt event */
static volatile int    org_int_lvl; /*orginal interrupt level */

void lan_interrupt(void) 
{
    quicc_dpr_t           *dpr_ptr = (quicc_dpr_t *)ADRSPC_QUICC_DPRAM;   
    quicc_internal_regs_t *qr_ptr  = &dpr_ptr->regs;
    short l_event;

    l_event = qr_ptr->scc[QSCC_LAN].scc_scce;
    qr_ptr->scc[QSCC_LAN].scc_scce = l_event;   /* clr only this event */
    event |= l_event;
    qr_ptr->cpic_cisr |= QUICC_CISR_SCC1;       /* clr intr in-service */
}
	
int init_enet_scc(void) 
{
    quicc_bd_t            *bd_ptr;
    quicc_dpr_t           *dpr_ptr = (quicc_dpr_t *)ADRSPC_QUICC_DPRAM;   
    quicc_internal_regs_t *qr_ptr  = &dpr_ptr->regs;
    quicc_ether_param_t   *pram_ptr= &dpr_ptr->scc1.ether;
    int *enet_vect_ptr;

    /*
     * PORT A
     *
     * bit PAR DIR ODR DAT name   signal
     *-------------------------------------
     * 15   0   1   1   0  N.C.   N.C.
     * 14   0   0   X   X  PA14   TCON_PR_L(test connector)
     * 13   1   0   X   X  CLK6   ID_RXCLK (ISDN D chan)
     * 12   1   0   X   X  CLK5   ID_TXCLK (ISDN D chan)
     *
     * 11   1   0   X   X  CLK4   ETH_TCLK (Enet Tx clk)
     * 10   0   1   1   1  N.C.   N.C.
     *  9   1   0   X   X  CLK2   ETH_RCLK (Enet Rx clk)
     *  8   1   0   X   X  CLK1   ISDN_BCLK(B chan clk)
     *
     *  7   1   1   X   X  RXDA   ISDN_BR  (ISDN Rx data)
     *  6   1   1   X   X  TXDA   ISDN_BX  (B chan Tx data)
     *  5   1   0   X   X  TXD3   ID_TXD   (D chan Tx data
     *  4   1   0   X   X  RXD3   ISDN_BR  (D chan Rx data)
     *
     *  3   0   1   X   X  PA3   ~COLL_BB  (enet full duplex)
     *  2   0   0   X   X  PA2    CONS_DTR (console DTR)
     *  1   1   0   X   X  TXD1   ETH_TXD  (Enet Tx data)
     *  0   1   0   X   X  RXD1   ETH_RXD  (Enet Rx data)
     *
     * PAR : 0 - general purpose
     *       1 - dedicated peripherall function
     * DIR : 0 - input 
     *       1 - output
     * ODR : 0 - actively driven as an output
     *       1 - open-drain driver
     */
	
    qr_ptr->pio_papar |=   SAP_QU_PT_A_PAR_ET_TCLK | SAP_QU_PT_A_PAR_ET_RCLK | 
	                   SAP_QU_PT_A_PAR_ET_TDAT | SAP_QU_PT_A_PAR_ET_RDAT ;
    qr_ptr->pio_papar &= ~(SAP_QU_PT_A_PAR_ET_FDX) ;
    qr_ptr->pio_padir &= ~(SAP_QU_PT_A_PAR_ET_TCLK | SAP_QU_PT_A_PAR_ET_RCLK | 
	                   SAP_QU_PT_A_PAR_ET_TDAT | SAP_QU_PT_A_PAR_ET_RDAT) ;
    qr_ptr->pio_padir |=   SAP_QU_PT_A_PAR_ET_FDX;
    qr_ptr->pio_paodr &= ~(SAP_QU_PT_A_PAR_ET_TCLK | SAP_QU_PT_A_PAR_ET_RCLK | 
	                   SAP_QU_PT_A_PAR_ET_TDAT | SAP_QU_PT_A_PAR_ET_RDAT |
			   SAP_QU_PT_A_PAR_ET_FDX) ; /* actively driven*/

    /*
     * PORT B
     *
     * bit PAR DIR ODR DAT name     signal
     *---------------------------------------
     * 17   0   1   0   X  PB17     STA_LED_L (front status)
     * 16   0   1   0   X  PB16     DBG_LED4  (rear panel)
     *
     * 15   0   1   0   X  PB15     DBG_LED3
     * 14   0   1   0   X  PB14     DBG_LED2
     * 13   0   1   0   X  PB13     DBG_LED1
     * 12   0   0   X   X  PB12     TPPLR     (10BT polarity)
     *
     * 11   0   1   0      PB11     WRX_LED_L (WAN Tx led)
     * 10   0   1   0      PB10     WTX_LED_L (WAN RX led)
     *  9   0   1   0      PB9      APORT     (10BT Auto)
     *  8   0   1   0      PB8      LOOP      (Enet loopback)
     *
     *  7   1   0   X   X  SMRXD1   CONS_RXD  (console Rx)
     *  6   1   0   X   X  SMTXD1   CONS_TXD  (console Tx)
     *  5   0   0   X   X  PB5      TPJABB    (Enet Jabber)
     *  4   0   1   0   X  PB4      I_SPISEL  (ISDN chip sel)
     *
     *  3   1   1   X      SPIMISO  SPIMISO   (serial data in)
     *  2   1   1   X      SPIMOSI  SPIMOSI   (serial data out)
     *  1   1   1   X      SPICLK   SPICLK    (SPI clk out)
     *  0   0   1   0   X ~SPISEL   N_SPISEL  (NVRAM chip sel)
     *
     * PAR : 0 - general purpose
     *       1 - dedicated peripherall function
     * DIR : 0 - input 
     *       1 - output
     * ODR : 0 - actively driven as an output
     *       1 - open-drain driver
     */
    
    qr_ptr->pip_pbpar &= ~(SAP_QU_PT_B_PAR_10B_POL | SAP_QU_PT_B_PAR_10B_AUT| 
			   SAP_QU_PT_B_PAR_ET_LOOP | SAP_QU_PT_B_PAR_ET_JABR);
    qr_ptr->pip_pbdir |=   SAP_QU_PT_B_PAR_10B_AUT | SAP_QU_PT_B_PAR_ET_LOOP;  
    qr_ptr->pip_pbdir &= ~(SAP_QU_PT_B_PAR_10B_POL | SAP_QU_PT_B_PAR_ET_JABR);
    qr_ptr->pip_pbodr &= ~(SAP_QU_PT_B_PAR_10B_POL | SAP_QU_PT_B_PAR_10B_AUT| 
			   SAP_QU_PT_B_PAR_ET_LOOP | SAP_QU_PT_B_PAR_ET_JABR);
    qr_ptr->pip_pbdat |=   SAP_QU_PT_B_PAR_10B_AUT ; 
    qr_ptr->pip_pbdat &=  ~SAP_QU_PT_B_PAR_ET_LOOP ; 

    /*
     * PORT C
     *
     * bit PAR DIR SO  DAT name      signal
     *----------------------------------------
     * 11   1   X   X   X  L1RSYNCA  ISDN_FSA (super frame sync)
     * 10   0   1   X   X  PC10      N.C.
     *  9   0   1   X   X  PC9       TPSQEL   (Enet input to NTH)
     *  8   0   1   X   X  PC8       TPAPCE   (Enet input to DSQE)
     * 
     *  7   0   0   0   X  PC7       CONS_CD  (carrier detect)
     *  6   0   0   0   X  PC6       SIMM_TYP3(SIMM type bits)
     *  5   0   0   1   X  RENA      RENA     (Enet Rx Enable)
     *  4   0   0   1   X  CLSN      CLSN     (Enet Collision) 
     *
     *  3   0   0   0   X  PC3       SIMM_TYP2
     *  2   0   0   0   X  PC2       SIMM_TYP1
     *  1   0   0   0   X  PC1	     SIMM_TYP0
     *  0   1   0   0   X  TENA	     TENA     (Enet Tx En)
     */
    

    qr_ptr->pio_pcpar |=   SAP_QU_PT_C_PAR_ET_TENA ;
    qr_ptr->pio_pcpar &= ~(SAP_QU_PT_C_PAR_TPSQEL  | SAP_QU_PT_C_PAR_TPAPCE | 
			   SAP_QU_PT_C_PAR_ET_RENA | SAP_QU_PT_C_PAR_ET_CLSN);
    qr_ptr->pio_pcdir |=   SAP_QU_PT_C_PAR_TPSQEL  | SAP_QU_PT_C_PAR_TPAPCE;
    qr_ptr->pio_pcdir &= ~(SAP_QU_PT_C_PAR_ET_RENA | SAP_QU_PT_C_PAR_ET_CLSN | 
			   SAP_QU_PT_C_PAR_ET_TENA);
    qr_ptr->pio_pcso  |=   SAP_QU_PT_C_PAR_ET_RENA | SAP_QU_PT_C_PAR_ET_CLSN;
    qr_ptr->pio_pcso  &= ~(SAP_QU_PT_C_PAR_TPSQEL  | SAP_QU_PT_C_PAR_TPAPCE | 
			   SAP_QU_PT_C_PAR_ET_TENA);
    qr_ptr->pio_pcdat |=   SAP_QU_PT_C_PAR_TPSQEL  | SAP_QU_PT_C_PAR_TPAPCE ;

    pram_ptr->scc_param.rbase = LAN_RX_BD_BASE;
    pram_ptr->scc_param.tbase = LAN_TX_BD_BASE;
	
    qr_ptr->cp_cr = QUICC_CPCR_CHNUM(QSCC_LAN) | QUICC_CPCR_SEMA;
    {   /*## should replace by pstate later ##*/
	int  count = 500;
	while ( qr_ptr->cp_cr & QUICC_CPCR_SEMA) count--;
	if ((qr_ptr->cp_cr & QUICC_CPCR_SEMA) == QUICC_CPCR_SEMA) {
	    cterr('f',0,"Command Semaphore Flag never got cleared in CPM CR register");
	    cleanup();
	    return(1);
	}
    }
	
    pram_ptr->scc_param.rfcr    = QUICC_FCR_MOT | 
	                          QUICC_FCR_FUNC_CODE(QUICC_FC_DMA_SPACE);
    pram_ptr->scc_param.tfcr    = QUICC_FCR_MOT | 
                                  QUICC_FCR_FUNC_CODE(QUICC_FC_DMA_SPACE);
    pram_ptr->scc_param.mrblr   = ENET_RX_BUF_SIZE;
    pram_ptr->c_pres  = ENET_PRAM_C_PRES;   /* 32-bit CCITT CRC */
    pram_ptr->c_mask  = ENET_PRAM_C_MASK;		
    pram_ptr->crcec   = 0;                  /* clear CRC error ctr */ 
    pram_ptr->alec    = 0;                  /* clear alignment error ctr */
    pram_ptr->disfc   = 0;                  /* clear discard frame ctr */
    pram_ptr->pads    = ENET_PRAM_PADS;
    pram_ptr->ret_lim = ENET_PRAM_RET_LIM;
    pram_ptr->mflr    = ENET_PRAM_MFLR;
    pram_ptr->minflr  = MIN_PAK_SIZE;
    pram_ptr->maxd1   = ENET_PRAM_MAXD1;
    pram_ptr->maxd2   = ENET_PRAM_MAXD2;
    pram_ptr->dma_cnt = 0;
    pram_ptr->gaddr1  = 0;
    pram_ptr->gaddr2  = 0;
    pram_ptr->gaddr3  = 0;
    pram_ptr->gaddr4  = 0;
    pram_ptr->paddr_h = ENET_PRAM_PADDR_H;
    pram_ptr->paddr_m = ENET_PRAM_PADDR_M;
    pram_ptr->paddr_l = ENET_PRAM_PADDR_L;
    pram_ptr->p_per   = ENET_PRAM_P_PER;
    pram_ptr->iaddr1  = 0;
    pram_ptr->iaddr2  = 0;
    pram_ptr->iaddr3  = 0;
    pram_ptr->iaddr4  = 0;
    pram_ptr->taddr_h = 0;
    pram_ptr->taddr_m = 0;
    pram_ptr->taddr_l = 0;

    bd_ptr         = (quicc_bd_t *) LAN_RX_BD;
    bd_ptr->status = 0;
    bd_ptr->length = 0;
    bd_ptr->buf_ptr= (volatile unsigned char *) (((int)enet_rx_buf >> 4) << 4);
   
    bd_ptr         = (quicc_bd_t *) LAN_TX_BD;
    bd_ptr->status = 0;
    bd_ptr->length = 0;
    bd_ptr->buf_ptr= (volatile unsigned char *) (((int)enet_tx_buf >> 4 ) << 4);
    qr_ptr->scc[QSCC_LAN].scc_scce = QUICC_ENET_SCCE_CLEAR;  /* clr events */
    qr_ptr->scc[QSCC_LAN].scc_sccm = QUICC_SCCM_TXERR | QUICC_SCCM_RXFRAME | 
			             QUICC_SCCM_TXBUF | QUICC_SCCM_RXBUF;
    qr_ptr->cpic_cimr |= QUICC_CISR_SCC1;
    qr_ptr->scc[QSCC_LAN].scc_gsmrh = 0; /* enable normal op of all modes */
    qr_ptr->scc[QSCC_LAN].scc_gsmrl =
	    QUICC_SCC_GSMRL_TPL(QUICC_TX_PREAM_48BITS) |  /* 48 bit preamble */
	    QUICC_SCC_GSMRL_TPP(QUICC_TX_PREAM_PAT_10S) | /* repeating 10's */
	    QUICC_SCC_GSMRL_TDCR(QUICC_TX_DIV_CLK_1X) |   /* 1x clock */
	    QUICC_SCC_GSMRL_DIAG(QUICC_NORMAL_OP) |       /* normal mode */
	    QUICC_SCC_GSMRL_MODE(QUICC_SCC_ETHER_MODE);   /* ethernet mode */
    qr_ptr->scc[QSCC_LAN].scc_psmr = 
	    QUICC_PSMR_RSH |                        /* receive short frames */
	    QUICC_PSMR_CRC(QUICC_32bit_CCITT_CRC) | /* 32-bit ccitt-crc */
	    QUICC_PSMR_PRO |	                    /* promiscuous mode */
	    QUICC_PSMR_LPB |	                    /* loopback operation */
	    QUICC_PSMR_NIB(QUICC_IGNORE_22BITS) |   /* search for SFD */
	    QUICC_PSMR_FDE;                         /* enable full duplex */
    qr_ptr->scc[QSCC_LAN].scc_dsr = ENET_DSR_DEFAULT;
}

static
void cleanup(void)
{
    quicc_dpr_t *dpr_ptr = (quicc_dpr_t *)ADRSPC_QUICC_DPRAM;   
    quicc_internal_regs_t *qr_ptr = &dpr_ptr->regs;

    qr_ptr->si_sicr |= QUICC_SICR_RXCS( QSCC_LAN, QUICC_SCC_TO_CLK2 ) |
		       QUICC_SICR_TXCS( QSCC_LAN, QUICC_SCC_TO_CLK4 ) ;
    qr_ptr->scc[QSCC_LAN].scc_gsmrl &= ~QUICC_SCC_GSMRL_DIAG_MASK; 
    qr_ptr->scc[QSCC_LAN].scc_gsmrl |=  QUICC_SCC_GSMRL_DIAG(QUICC_NORMAL_OP) ;

    qr_ptr->pip_pbdat &= ~SAP_QU_PT_B_PAR_ET_LOOP ; /* enable enet txver loopback mode */
    qr_ptr->pio_padat |=  SAP_QU_PT_A_PAR_ET_FDX;   /* disable full duplex */
    qr_ptr->pio_pcdat |=  SAP_QU_PT_C_PAR_TPSQEL  | SAP_QU_PT_C_PAR_TPAPCE ;

    qr_ptr->scc[QSCC_LAN].scc_scce = QUICC_ENET_SCCE_CLEAR; /* clr events */
    qr_ptr->cpic_cimr &= ~QUICC_CISR_SCC1;          /* mask SCC1 interrupts */

    chgint(org_int_lvl); /* restore interrupt interrupt level*/
    setvbr(&evec);       /* restore monitor interupt vector table */

}

static
quicc_bd_t * get_tx_bd(void)
{
    int i;
    quicc_bd_t *bd_ptr=(quicc_bd_t *) (LAN_TX_BD); 
    int count=1000;
    
    for(i=0 ; bd_ptr[i].status & QUICC_BDSTAT_TX_RDY; ) {
	count--;
	if ( bd_ptr[i].status & QUICC_BDSTAT_TX_WRAP ) /* final bd in tbl */
	    i = 0;
	else 
	    i++;
	if (count == 0){
	    cterr('f',0,"Timeout waiting for free transmit buffer descriptor.");
	    return(NULL);
	}
    }
    return (&bd_ptr[i]);
}	

int setup_enet_loopback(int optype) 
{
    quicc_dpr_t *dpr_ptr = (quicc_dpr_t *)ADRSPC_QUICC_DPRAM;   
    quicc_internal_regs_t *qr_ptr = &dpr_ptr->regs;

    /* disable Tx and Rx */
    qr_ptr->scc[QSCC_LAN].scc_gsmrl &= ~QUICC_SCC_GSMRL_ENR;
    qr_ptr->scc[QSCC_LAN].scc_gsmrl &= ~QUICC_SCC_GSMRL_ENT;

    /* clear old clock select fields */
    qr_ptr->si_sicr  &= ~(QUICC_SICR_RXCS( QSCC_LAN, QUICC_SCC_CS_MASK ) |
			  QUICC_SICR_TXCS( QSCC_LAN, QUICC_SCC_CS_MASK ) ) ;
    qr_ptr->brg_brgc2 =  QUICC_BRGC_RESET;    /* reset brgc */
    qr_ptr->scc[QSCC_LAN].scc_gsmrl &= ~QUICC_SCC_GSMRL_DIAG_MASK;

    if ( optype != LAN_SCC_LOOPBACK ) {
	qr_ptr->si_sicr |= QUICC_SICR_RXCS( QSCC_LAN, QUICC_SCC_TO_CLK2 ) |
			   QUICC_SICR_TXCS( QSCC_LAN, QUICC_SCC_TO_CLK4 ) ;
 	qr_ptr->scc[QSCC_LAN].scc_gsmrl |=  QUICC_SCC_GSMRL_DIAG(QUICC_NORMAL_OP) ;
    }  

    switch (optype) {
    case LAN_SCC_LOOPBACK:
	qr_ptr->brg_brgc2 = QUICC_BRGC_EN_CLK |
			    QUICC_BRGC_CLK_SRC(0) | /*from BRGCLK(internal)*/
			    QUICC_BRGC_CLK_DIV(10);             /* SYS : DIV(10) */
	qr_ptr->si_sicr  |= QUICC_SICR_RXCS( QSCC_LAN, QUICC_SCC_TO_BRG2 ) |
			    QUICC_SICR_TXCS( QSCC_LAN, QUICC_SCC_TO_BRG2 ) ;
	qr_ptr->scc[QSCC_LAN].scc_gsmrl |=               
			QUICC_SCC_GSMRL_DIAG(QUICC_LOCAL_LOOPBACK) ; /* local loopback */
	break;
    case LAN_TXVR_LOOPBACK:
	qr_ptr->pip_pbdat |= SAP_QU_PT_B_PAR_ET_LOOP ; /* enable enet txver loopback mode */
	qr_ptr->pio_padat |= SAP_QU_PT_A_PAR_ET_FDX;   /* disable full duplex */
	qr_ptr->pio_pcdat |= SAP_QU_PT_C_PAR_TPAPCE ;  /* disable Signal Quality Error */ 
	break;
    case LAN_EXT_LOOPBACK:
	qr_ptr->pip_pbdat &= ~SAP_QU_PT_B_PAR_ET_LOOP ; /* disable enet txver loopback mode */    
	qr_ptr->pio_padat &= ~SAP_QU_PT_A_PAR_ET_FDX;   /* enable ful duplex */
	qr_ptr->pio_pcdat &= ~SAP_QU_PT_C_PAR_TPAPCE ;  /* enable Signal Quality Error */ 
	break;
    default: 
	cterr('f',0,"Unidentify loopback operation mode");
	cleanup();
	return(1);
    }
    qr_ptr->cp_cr = QUICC_CPCR_CHNUM(QSCC_LAN) | 
	            QUICC_CPCR_OPCODE(QUICC_CPC_INIT_RXTX) | QUICC_CPCR_SEMA;
    {   /*## should replace by pstate later ##*/
	int  count = 500;
	while ( qr_ptr->cp_cr & QUICC_CPCR_SEMA) count--;
	if ((qr_ptr->cp_cr & QUICC_CPCR_SEMA) == QUICC_CPCR_SEMA) {
	    cterr('f',0,"Command Semaphore Flag never got cleared in CPM CR register");
	    cleanup();
	    return(1);
	}
    }
    qr_ptr->scc[QSCC_LAN].scc_gsmrl |= QUICC_SCC_GSMRL_ENR;
    qr_ptr->scc[QSCC_LAN].scc_gsmrl |= QUICC_SCC_GSMRL_ENT;
}

static 
unsigned short rand_num(unsigned long num)
{
    num = (num*125) % 2796203;
    return((unsigned short)num);
}

/*
 * filldata : fill the packet data field with unique values
 */
static
unsigned long filldata(unsigned short data_size, unsigned short * data_ptr, 
		       unsigned long seed)
{
    int count,i;
    unsigned char *ptr;

    count = data_size & 0xfffe; /* even bytes */
    /* fill data area one word at a time */
    for(i = 0 ; i < (count>>1) ; i++) 
	seed = data_ptr[i] = rand_num(seed);
    /* fill the last byte if the data size is odd count */
    if(data_size & 0x01) {
	ptr  = (unsigned char *) &data_ptr[i];
	*ptr = (unsigned char) rand_num(seed);
    }
    return(seed);
}

static
int tx_status(unsigned short *status)
{
    int count=500;
    unsigned short check_bits; 
    char *errbufptr,errbuf[CBUFSIZ];

    while (*status & QUICC_BDSTAT_TX_RDY) { /* buf ready for tx */
	if (count == 0) {
	    cterr('f',0,"Timeout waiting for buffer to transmit");
	    return(1);
	}
	count --;
    }
    check_bits = QUICC_BDSTAT_TX_LAST |
	         QUICC_BDSTAT_TX_RDY |
		 QUICC_BDSTAT_TX_UNRRUN |
		 QUICC_BDSTAT_TX_CSLOS;
    if (*status & check_bits) {
	errbufptr  = errbuf;
	errbufptr += sprintf(errbufptr,"Error on TX : ");
	if (*status & QUICC_BDSTAT_TX_LAST)
	    errbufptr += sprintf(errbufptr,"Late Collision,");
	if (*status & QUICC_BDSTAT_TX_RDY)
	    errbufptr += sprintf(errbufptr,"Retransmission Limit,");
	if (*status & QUICC_BDSTAT_TX_UNRRUN)
	    errbufptr += sprintf(errbufptr,"Underrun,");
	if (*status & QUICC_BDSTAT_TX_CSLOS)
	    errbufptr += sprintf(errbufptr,"Carrier Sense Lost");
	cterr('f',0,"%s",&errbuf);
	return(1);
    }
    else {
	return(0); /* error free tx */
    }
}

static
void set_heading(unsigned short *data_ptr, unsigned short *dest_addr, 
		 unsigned short *src_addr, unsigned short proto_type)
{ 
    int i;
    for(i=0 ; i<THREE_WORDS ; i++) {
	data_ptr[i] = dest_addr[i];
	data_ptr[THREE_WORDS+i] = src_addr[i];
    }
    data_ptr[THREE_WORDS +i] = proto_type;
}

static
void set_test_mode (int mode)
{
    quicc_dpr_t *dpr_ptr = (quicc_dpr_t *)ADRSPC_QUICC_DPRAM;   
    quicc_internal_regs_t *qr_ptr = &dpr_ptr->regs;

    qr_ptr->scc[QSCC_LAN].scc_scce = QUICC_ENET_SCCE_CLEAR;
    qr_ptr->scc[QSCC_LAN].scc_sccm = QUICC_SCCM_TXERR | QUICC_SCCM_RXFRAME | 
			             QUICC_SCCM_TXBUF | QUICC_SCCM_RXBUF;
    if (mode == INTR_MODE)
	qr_ptr->cpic_cimr |=  QUICC_CISR_SCC1;
    else
	qr_ptr->cpic_cimr &= ~QUICC_CISR_SCC1;
}
	
static
void setup_tx_bd(quicc_bd_t * bd_ptr, int mode)
{
    int count = 10000;

#ifdef DEBUG
    printf("\nbd_ptr->status 0x%x",bd_ptr->status);
#endif /*DEBUG*/
    while (count) {
	if (!(bd_ptr->status & QUICC_BDSTAT_TX_RDY))
	    break;
	count--;
    }
    if (count == 0)
	cterr('w',0,"time out waiting for tx buffer");

    if (mode == INTR_MODE)
	bd_ptr->status = QUICC_BDSTAT_TX_PAD |
                         QUICC_BDSTAT_TX_WRAP |
        	         QUICC_BDSTAT_TX_INT|
		         QUICC_BDSTAT_TX_LAST |
	                 QUICC_BDSTAT_TX_TRCRC;
    else
	bd_ptr->status = QUICC_BDSTAT_TX_PAD |
                         QUICC_BDSTAT_TX_WRAP |
		         QUICC_BDSTAT_TX_LAST |
	                 QUICC_BDSTAT_TX_TRCRC;
    bd_ptr->length = 0;
}
	
static 
void setup_rx_bd(quicc_bd_t * bd_ptr, int mode)
{
    bd_ptr->length = 0;
    if (mode == INTR_MODE)
	bd_ptr->status = QUICC_BDSTAT_RX_EMPTY |
	                 QUICC_BDSTAT_RX_WRAP |
                         QUICC_BDSTAT_RX_INT;
    else
 	bd_ptr->status = QUICC_BDSTAT_RX_EMPTY |
	                 QUICC_BDSTAT_RX_WRAP;
}

char lostpakmsg[] = "Never receive any frame";
char spacemsg1[]  = "                       ";
char txerrmsg[]   = "Got transmit errors";
char rxerrmsg[]   = "Got receive errors";
char badpakmsg[]  = "Data errors";
char warnmsg[]    = "*** LOOPBACK CONNECTOR IS REQUIRED FOR THIS TEST ***";

static
void print_event(char* bufptr)
{
    char *errbufptr;

    errbufptr  = bufptr;
    errbufptr += sprintf(errbufptr,"Interrupt test event status : ");
    errbufptr += sprintf(errbufptr,"%s. ", lostpakmsg);
    if (event & QUICC_SCCE_RXBUF)
	errbufptr += sprintf(errbufptr,"Received incomplete frame. ");
    if (event & QUICC_SCCE_TXERR) 
	errbufptr += sprintf(errbufptr,"Got transmit error. ");
    if (event & QUICC_SCCE_TXBUF)
	errbufptr += sprintf(errbufptr,"Transmission completed.");
}

static
unsigned int check_intr_event(void)
{
    unsigned long count1 = 1000;
    unsigned long count2 = 1000000;
    int gotit = 0;
    int err = 0;

    /*### replace by pstat later when available ###*/
    while(count1) { 
	while (count2) {
	    if (event & QUICC_SCCE_RXFRAME) {
		gotit = 1;
		break;
	    }
	    count2--;
	}
	if(gotit) break;
	count1--;
    }

    if(!(event & QUICC_SCCE_RXFRAME))
	err |= INTR_RX_FAIL;
    if(event & (QUICC_SCCE_BSY | QUICC_SCCE_RXBUF))
	err |= INTR_RX_ERR;
    if(event & QUICC_SCCE_TXERR)
	err |= INTR_TX_ERR;
    if(!(event & QUICC_SCCE_TXBUF))
	err |= INTR_TX_FAIL;
#ifndef TRYWITHOUT
    {
	quicc_dpr_t   *dpr_ptr = (quicc_dpr_t *)ADRSPC_QUICC_DPRAM;   
	quicc_internal_regs_t *qr_ptr = &dpr_ptr->regs;
	quicc_bd_t 	  *tx_bd_ptr = (quicc_bd_t *)LAN_TX_BD;
	quicc_bd_t 	  *rx_bd_ptr = (quicc_bd_t *)LAN_RX_BD;

	if(err) {
	    printf("\n");
	    printf("SCCE = 0x%x\n",event);
	    printf("SCCM = 0x%x\n",qr_ptr->scc[QSCC_LAN].scc_sccm);
	    printf("CIPR = 0x%x\n",qr_ptr->cpic_cipr);
	    printf("CIMR = 0x%x\n",qr_ptr->cpic_cimr);
	    printf("CISR = 0x%x\n",qr_ptr->cpic_cisr);
	    printf("Tx buffer status = 0x%x\n", tx_bd_ptr->status);
	    printf("Rx buffer status = 0x%x\n", rx_bd_ptr->status);
            longjmp(monjmpptr,1);
	}
    }
#endif
    return(err);
}

static
unsigned int check_rx_status(quicc_bd_t *rx_bd_ptr) 
{ 
    unsigned long count1 = 1000;
    unsigned long count2 = 1000000;
    int gotit = 0;
    int err   = 0;

    while(count1) { 
	while (count2) {
	    if (!(rx_bd_ptr->status & QUICC_BDSTAT_RX_EMPTY)) {
		gotit = 1;
		break;
	    }
	    count2--;
	}
	if(gotit) break;
	count1--;
    }

    if (rx_bd_ptr->status & QUICC_BDSTAT_RX_EMPTY)
	err |= POLL_RX_FAIL;
    if (rx_bd_ptr->status & RX_BD_ERR_MASK)
	err |= POLL_RX_ERR;
    return(err);
}

static
unsigned int check_tx_status(quicc_bd_t *tx_bd_ptr)
{
    unsigned long count1 = 1000;
    unsigned long count2 = 1000000;
    int gotit = 0;
    int err   = 0;

    /*### replace by pstate later when available ###*/
    while(count1) { 
	while (count2) {
	    if (!(tx_bd_ptr->status & QUICC_BDSTAT_TX_RDY)) {
		gotit = 1;
		break;
	    }
	    count2--;
	}
	if(gotit) break;
	count1--;
    }
    if(tx_bd_ptr->status & QUICC_BDSTAT_TX_RDY)
	err |= POLL_TX_FAIL;
    if(tx_bd_ptr->status & TX_BD_ERR_MASK)
	err |= POLL_TX_ERR;
    return(err);
}

static 
char* gen_err_msg(char *errbufptr, unsigned int error,
		  quicc_bd_t *tx_bd_ptr, quicc_bd_t *rx_bd_ptr) 
{
    if (error & (POLL_TX_FAIL | INTR_TX_FAIL)) {
	errbufptr += sprintf(errbufptr,
		     "\nNever Transmitted : Tx buffer status = 0x%x", tx_bd_ptr->status);
	if (error & INTR_TX_FAIL)
	    errbufptr += sprintf(errbufptr,
		     "\n                  : event reg (SCCE)  = 0x%x", event);
    }
    if (error & (POLL_TX_ERR | INTR_TX_ERR)) {
	errbufptr += sprintf(errbufptr,
		     "\nTx errors : Tx buffer status = 0x%x", tx_bd_ptr->status);
	if (error & INTR_TX_ERR)
	    errbufptr += sprintf(errbufptr,
		     "\n          : event reg (SCCE)  = 0x%x", event);
    }
    if (error & (POLL_RX_FAIL | INTR_RX_FAIL)) {
	errbufptr += sprintf(errbufptr,
		     "\nNever received : Rx buffer status = 0x%x", rx_bd_ptr->status );
	if (error & INTR_RX_FAIL)
	    errbufptr += sprintf(errbufptr,
		     "\n          : event reg (SCCE)  = 0x%x", event);
    }
    if (error & (POLL_RX_ERR | INTR_RX_ERR)) {
	errbufptr += sprintf(errbufptr,
		     "\nRx errors : Rx buffer status = 0x%x", rx_bd_ptr->status);
	if (error & INTR_RX_ERR)
	    errbufptr += sprintf(errbufptr,
		     "\n          : event reg (SCCE)  = 0x%x", event);
    }
    if (error & DATA_MISMATCH) {
	errbufptr += sprintf(errbufptr,
                     "\nReceive and transmit data mismatch.");
    }
    return(errbufptr);
}

int lan_loopback_test(int loopback_mode)
{

#define MAX_PAK       4
    unsigned long  seed = 1001;
    int		   i,mode,fail=0;
    unsigned int   pak_num;
    unsigned int   error;       /* error flags */
    unsigned short bogus_addr[3] = {0x0005, 0xdead, 0xbe00};
    unsigned short own_addr[3]   = {ENET_PRAM_PADDR_H, ENET_PRAM_PADDR_M,
				    ENET_PRAM_PADDR_L};
    unsigned short pak_size[MAX_PAK] = {SCC1_FIFO_SIZE*2, SCC1_FIFO_SIZE,
					    SCC1_FIFO_SIZE-7 , MIN_PAK_SIZE};  
                                        /* FIFO size = 32 */
    quicc_bd_t 	  *tx_bd_ptr = (quicc_bd_t *)LAN_TX_BD;
    quicc_bd_t 	  *rx_bd_ptr = (quicc_bd_t *)LAN_RX_BD;
    quicc_dpr_t   *dpr_ptr = (quicc_dpr_t *)ADRSPC_QUICC_DPRAM;   
    quicc_internal_regs_t *qr_ptr = &dpr_ptr->regs;
    volatile long *tx_buf_ptr,*rx_buf_ptr ;
    char          *warnmsg_ptr,*errbufptr,errbuf[CBUFSIZ];

    cp_intvec_toram(ADRSPC_RAM); /* copy rom monitor int. vector table into memory */
    /* install ethernet interrupt handler */
    put_int_handler(dpr_ptr, QUICC_CPIC_INTERRUPT_SCC1, &scc1_int);

    switch (loopback_mode) {
    case LAN_SCC_LOOPBACK:
	testname("Ethernet internal (SCC) loopback");
	break;
    case LAN_TXVR_LOOPBACK:
	testname("Ethernet transceiver (LXT907) loopback");
	break;
    case LAN_EXT_LOOPBACK:
	testname("Ethernet external loopback");
	break;
    default:
	cterr('f',0,"Unidentify loopback operation mode");
	cleanup();
	return(1);
    }

    org_int_lvl = chgint(4);
    init_enet_scc();
    setup_enet_loopback(loopback_mode);
    chgint(0);
    tx_buf_ptr = (volatile long *)tx_bd_ptr->buf_ptr;
    rx_buf_ptr = (volatile long *)rx_bd_ptr->buf_ptr;
    for ( mode = 0 ; mode < MAX_MODE ; mode ++ ) { /* 0:poll 1:interrupt */
	set_test_mode(mode);
 	for ( pak_num = 0; pak_num <MAX_PAK  ; pak_num++) {
	    setup_tx_bd(tx_bd_ptr,mode);
	    setup_rx_bd(rx_bd_ptr,mode);
	    for ( i = 0 ; i < SCC1_FIFO_SIZE; i++ ) {
		*tx_buf_ptr++ = 0x0; /* clear tx buffer before use */
		*rx_buf_ptr++ = 0x0; /* clear rx buffer before use */
	    }
	    error = 0;
	    event = 0;
	    seed = filldata(pak_size[pak_num], (unsigned short *)tx_bd_ptr->buf_ptr, 
			    seed);
	    set_heading((unsigned short *)tx_bd_ptr->buf_ptr, bogus_addr, own_addr, 
			PROTOCOL_TYPE);
	    tx_bd_ptr->length  = pak_size[pak_num];
	    if (mode == INTR_MODE) {
		qr_ptr->scc[QSCC_LAN].scc_scce = QUICC_ENET_SCCE_CLEAR;
	    }
	    tx_bd_ptr->status |= QUICC_BDSTAT_TX_RDY; /* bye packet */
	    error |= check_tx_status(tx_bd_ptr);
	    error |= check_rx_status(rx_bd_ptr);
	    if (mode == INTR_MODE) {
		error |= check_intr_event();
	    }
#ifdef YY
	    error |= check_tx_status(tx_bd_ptr);
	    error |= check_rx_status(rx_bd_ptr);
#endif /*YY*/
	    if (!error) {
		if(bcmp(rx_bd_ptr->buf_ptr,tx_bd_ptr->buf_ptr,tx_bd_ptr->length))
		    error |= DATA_MISMATCH; 
	    }
	    if (error) {
		errbufptr = errbuf;
		gen_err_msg(errbufptr, error, tx_bd_ptr, rx_bd_ptr);
		if (loopback_mode == LAN_EXT_LOOPBACK)
		    warnmsg_ptr = warnmsg;
		else 
		    warnmsg_ptr = NULL;
		cterr('f',0,
		      "Ethernet %s test : frame length 0x%x frame num %d %s %s %s",
		      mode ? "interrupt" : "poll", pak_size[pak_num], (pak_num+1),
		      warnmsg_ptr ? warnmsg : " ", warnmsg_ptr ? "\n" : " ", &errbuf);
		fail++;
	    }
	}
    }

    prcomplete(testpass, errcount, 0);
    cleanup();
    return fail ? 1 : 0;
}

/* end of module */
