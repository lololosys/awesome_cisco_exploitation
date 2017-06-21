/* $Id: quicc_serial.c,v 3.4.10.1 1996/03/21 23:29:21 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/rommon/src-36-c1000/quicc_serial.c,v $
 *------------------------------------------------------------------
 * quicc_serial.c -- 68360 QUICC serial diagnostic code
 *
 * August 22, 1995, John Ng
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: quicc_serial.c,v $
 * Revision 3.4.10.1  1996/03/21  23:29:21  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.2.2.3  1996/03/19  01:01:19  mbeesley
 * Branch: ELC_branch
 * Sync to V111_1_3
 *
 * Revision 3.2.2.2  1996/01/16  01:59:42  dcarroll
 * Branch: ELC_branch
 * Sync ELC_branch between ELC_baseline_120995 and ELC_baseline_960110.
 *
 * Revision 3.2.2.1  1995/12/01  04:22:21  mbeesley
 * Branch: ELC_branch
 * Reorganize Rom monitor header files to make it easier to port
 * to new platforms.
 *
 * Revision 3.4  1996/02/23  17:15:55  cshen
 * CSCdi48454:  Need to support on-board flash on c1005 derivative
 *
 * Revision 3.3  1996/01/05  19:19:40  ksabnis
 * CSCdi46424:  Serial port internal loopback test fails with OKI chip
 *
 *         Added support so that Serial port Internal Loopback test works
 *         on
 *         Catwoman platforms with OKI chip.
 *
 * Revision 3.2  1995/11/17  18:44:27  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:07:36  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:24:10  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/08/22  20:30:20  jng
 * Place holder file for Catwoman 11.0
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "monitor.h"
#include "error.h"
#include "bitdef.h"
#include "signal_4k.h"
#include "quicc.h"
#include "quicc_serial.h"
#include "nvmonvars.h"

#include "../../../boot/cpu.h"

extern long *evec;

/* =================================================================*
 *                        Globals in this file                      *
 * =================================================================*/

static uchar serial_quicc_test;   /* Serial "test on" flag */
static uchar rx_ov_fl;            /* Overflow */
static uchar non_oct_algn;        /* Octate not algn */
static uchar rx_abort;            /* Abort */
static uchar crc_err;             /* CRC error */
static uchar ovr_err;             /* Overrun */
static uchar car_loss;            /* Carrier lost */   

/* =================================================================*
 *                      Function Prototypes                         *
 * =================================================================*/


static int    init_scc(quicc_dpr_t *dpr, int loopmode, int *clk_ptr);
static void   set_pio_ports(quicc_dpr_t *dpr);
static void   mask_quicc_intr(quicc_dpr_t *dpr);
static void   set_init_bd(int *clk_ptr);
static int    set_scc_param_ram(volatile quicc_hdlc_param_t *hdlc_ptr);
static int    scc_lpbk(quicc_dpr_t *dpr, int *clk_ptr);
static int    quicc_cpai_int_lpbk(quicc_dpr_t *dpr, int *clk_ptr);
static int    quicc_cpai_ext_lpbk(quicc_dpr_t *dpr, int *clk_ptr);
static int    print_serial_errors(quicc_bd_t *rmd);
static int    quicc_serial_cleanup(void);
static int    set_loopback(quicc_dpr_t *dpr);
static int    clear_loopback(quicc_dpr_t *dpr);      

int clock_rate[MAXCLOCKS] =
         {   QUICC_BRGC_CLK_DIV(6), QUICC_BRGC_CLK_DIV(7),
	     QUICC_BRGC_CLK_DIV(8), QUICC_BRGC_CLK_DIV(54),
	     QUICC_BRGC_CLK_DIV(111),QUICC_BRGC_CLK_DIV(195),
	     QUICC_BRGC_CLK_DIV(223),QUICC_BRGC_CLK_DIV(325),
	     QUICC_BRGC_CLK_DIV(650),QUICC_BRGC_CLK_DIV(1300),
	     QUICC_BRGC_CLK_DIV(3500)};

int * clk_ptr;

/*
 * =================================================================
 * Name: init_scc()
 *
 * Function: initialize SCC4. ENT, ENR bits in GSMR are not enabled.
 *
 * =================================================================
 */
static int init_scc(quicc_dpr_t *dpr, int loopmode, int *clk_ptr)
{
    volatile quicc_hdlc_param_t *hdlc_ptr;
    quicc_internal_regs_t *qir_p;   /* quicc internal reg */
    volatile quicc_bd_t *bd;        /* scc buffer descriptor */
    
    qir_p = &(dpr->regs);

    switch(loopmode){
   
    case CPAI:
	qir_p->scc[3].scc_gsmrh = 0x0;

	/* set up gsmr_ normal operation and HDLC mode */
	qir_p->scc[3].scc_gsmrl = QUICC_SCC_GSMRL_DIAG(QUICC_NORMAL_OP);

	break;
   
    case SCCLP:

	qir_p->scc[3].scc_gsmrh = 0x0;

	/* set up gsmr_ local loopback and HDLC mode */
	qir_p->scc[3].scc_gsmrl = QUICC_SCC_GSMRL_DIAG(QUICC_LOCAL_LOOPBACK);

	break;
    } /* end switch */

    /* clear old clock select fields */
    qir_p->si_sicr &= ~(QUICC_SICR_RXCS(QUICC_PORT_SCC4, QUICC_SCC_TO_BRG4)|
			QUICC_SICR_TXCS(QUICC_PORT_SCC4, QUICC_SCC_TO_BRG4));
   
    /* set clock select field for SCC4 */
    qir_p->si_sicr |= QUICC_SICR_RXCS(QUICC_PORT_SCC4, QUICC_SCC_TO_BRG4)|
	QUICC_SICR_TXCS(QUICC_PORT_SCC4, QUICC_SCC_TO_BRG4);
   
    qir_p->brg_brgc4 = QUICC_BRGC_RESET;  

    /* use internal baud rate generator (BRGC4) */
    qir_p->brg_brgc4 = QUICC_BRGC_EN_CLK | 
	QUICC_BRGC_CLK_SRC(0) | (* clk_ptr);
    
    /* number of flags = 0, 16 bit CRC, normal operation */
    qir_p->scc[3].scc_psmr = QUICC_SERIAL_PSMR_SETTING;

    /* Clear events register by writing all ones */
    qir_p->scc[3].scc_scce = QUICC_SERIAL_REG_CLEAR;

    /* setup HDLC specific parameter RAM for SCC4 */
    hdlc_ptr = &(dpr->scc4.hdlc);
    set_scc_param_ram(hdlc_ptr);

    /* get ptr to user_data_1 if DPRAM was at zero */
    bd = (quicc_bd_t *)(&(((quicc_dpr_t *)(0))->user_data_1));
   
    /* Set pointers to RX and tx buffer descriptors */
    hdlc_ptr->scc_param.rbase = (int)(&bd[SERIAL_SCC4_RX_BD_IDX]);
    hdlc_ptr->scc_param.tbase = (int)(&bd[SERIAL_SCC4_TX_BD_IDX]);

    /* program CR to execute INIT RX and TX PARAMS command for this
       channel */
    quicc_cr_cmd(QUICC_CHAN_SCC4, QUICC_CPC_INIT_RXTX);
}
/*
 * =================================================================
 * Name: set_pio_ports()
 *
 * Function: setup parallel I/O ports
 *
 * =================================================================
 */
static void set_pio_ports(quicc_dpr_t *dpr)
{
    volatile quicc_internal_regs_t *qir_p; /* quicc internal reg */

    qir_p = &(dpr->regs);
    
    /* init port A PAR pins */

    qir_p->pio_papar =  CAT_QU_PT_A_PAR_INIT;

    /* set T_BTXC (PA14), T_BRXC (PA12), T_BTXD (PA7), T_BRXD (PA6) */

    qir_p->pio_papar |=  CAT_QU_PT_A_PAR_T_BTXC | CAT_QU_PT_A_PAR_T_BRXC | 
	CAT_QU_PT_A_PAR_T_BTXD | CAT_QU_PT_A_PAR_T_BRXD;

    /* init DIR, ODR pins for port A */
 
    qir_p->pio_padir = CAT_QU_PT_A_DIR_INIT | 0x4000;

    qir_p->pio_paodr = CAT_QU_PT_A_ODR_INIT;

    /* init PAR, DIR, ODR pins for port B */
  
    qir_p->pip_pbpar = CAT_QU_PT_B_PAR_INIT; 
    qir_p->pip_pbdir = CAT_QU_PT_B_DIR_INIT;

    /* set DIR bits 0, 1, 2, 3, 4. These are for CTRL0-4 signals */
  
    qir_p->pip_pbdir |=  CAT_QU_PT_B_DIR_T_CTRL0 |
	CAT_QU_PT_B_DIR_T_CTRL1 | CAT_QU_PT_B_DIR_T_CTRL2 | 
	    CAT_QU_PT_B_DIR_T_CTRL3 | CAT_QU_PT_B_DIR_T_CTRL4;

    qir_p->pip_pbodr = CAT_QU_PT_B_ODR_INIT;

    /* init PAR, DIR, ODR pins for port C */

    qir_p->pio_pcpar = CAT_QU_PT_C_PAR_INIT;
   
    qir_p->pio_pcpar |= CAT_QU_PT_C_PAR_T_RTS;

    qir_p->pio_pcdir = CAT_QU_PT_C_DIR_INIT;

    /* DIR is set for T_BDTR */
    qir_p->pio_pcdir |= CAT_QU_PT_C_DIR_T_BDTR;
 
    /* Vpp pin for HANA, so consequent flash test won't fail */
    if((NVRAM_SAV)->nvcookie.cookie.vendor == VENDOR_OKI)
      qir_p->pio_pcdir |= QU_PT_C_FIXED_VPP_CTRL_HANA;

    qir_p->pio_pcso = CAT_QU_PT_C_SO_INIT;

    /* set SO for T_CTS (PC10) and T_DCD (PC11) */ 
    qir_p->pio_pcso |= CAT_QU_PT_C_SO_T_CTS | CAT_QU_PT_C_SO_T_DCD;
}
/*
 * =================================================================
 * Name: set_loopback()
 *
 * Description: This routine will set 5in1 interface  in a loopback 
 *      by setting loop control bit in the Control Register to 1.      
 *
 * Parameters:
 *	quicc_dpr_t *dpr - pointer to dualport ram.
 *
 * =================================================================
 */
static int set_loopback(quicc_dpr_t *dpr)
{
    volatile quicc_internal_regs_t *qir_p; /* quicc internal reg */
    
    qir_p = &(dpr->regs);

    /* zero the CTRL0-4 bits */ 
    qir_p->pip_pbdat &= CTRL_BITS_MASK;
  
    /* set enable (CTRL3) and data (CTRL1) bits. CLK (CTRL2) is still zero */
    qir_p->pip_pbdat |= CAT_QU_PT_B_DAT_T_CTRL3 | CAT_QU_PT_B_DAT_T_CTRL1;
   
    /* assert clock (CTRL2), providing 0->1 edge to write data in the
       Control  Register */
    qir_p->pip_pbdat |= CAT_QU_PT_B_DAT_T_CTRL2;

    /* negate clock (CTRL2) */
    qir_p->pip_pbdat &= ~CAT_QU_PT_B_DAT_T_CTRL2;

    /* zero the CTRL0-4 bits again */ 
    qir_p->pip_pbdat &= CTRL_BITS_MASK; 
}
/*
 * =================================================================
 * Name: clear_loopback()
 *
 * Description: This routine will clear loopback in a  5in1 interface 
 *      by reseting loop control bit in the Control Register to 0.      
 *
 * Parameters:
 *	quicc_dpr_t *dpr - pointer to dualport ram.
 *
 * =================================================================
 */
static int clear_loopback(quicc_dpr_t *dpr)
{
    volatile quicc_internal_regs_t *qir_p; /* quicc internal reg */
    
    qir_p = &(dpr->regs);

    /* zero the CTRL0-4 bits */ 
    qir_p->pip_pbdat &= CTRL_BITS_MASK;

    /* set enable (CTRL3). Data (CTRL1) and CLK (CTRL2) are zero now */
    qir_p->pip_pbdat |=  CAT_QU_PT_B_DAT_T_CTRL3;

    /* set clock (CTRL2), providing 0->1 edge to write data in the
       Control  Register */
    qir_p->pip_pbdat |= CAT_QU_PT_B_DAT_T_CTRL2;

    /* zero the CTRL0-4 bits again */ 
    qir_p->pip_pbdat &= CTRL_BITS_MASK;
}
/*
 * =================================================================
 * Name: mask_quicc_intr()
 *
 * Function: mask all interrupts from quicc
 *
 * =================================================================
 */
static void mask_quicc_intr(quicc_dpr_t *dpr)
{
    volatile quicc_internal_regs_t *qir_p;        /* quicc Internal reg */
    
    qir_p = &(dpr->regs);
   
    /* mask off all INRQ after the test */
    qir_p->cpic_cimr = 0;    
  
    /* clear Interrupt Service Register */ 
    qir_p->cpic_cisr = QUICC_SERIAL_REG_CLEAR; 
   
    /* clear Interrupt Pending Register */
    qir_p->cpic_cipr = QUICC_SERIAL_REG_CLEAR;
}
/*
 * =================================================================
 * Name: set_init_bd()
 *
 * Function: setup and initialize buffer descriptors for all 3 channels. 
 *
 * =================================================================
 */
static void set_init_bd(int *clk_ptr)
{
    quicc_bd_t *tmd;              /* TX buffer descr. */
    quicc_bd_t *rmd;              /* RX buffer descr. */
    int i;
    volatile quicc_bd_t *bd;
    uchar val;
    volatile uchar * buf_start;   /* start of buffer pointer */
    uchar *buf_addr;
    
    rmd = &((quicc_bd_t *)(((quicc_dpr_t *)ADRSPC_QUICC_DPRAM)->user_data_1))[SERIAL_SCC4_RX_BD_IDX];
    tmd = &((quicc_bd_t *)(((quicc_dpr_t *)ADRSPC_QUICC_DPRAM)->user_data_1))[SERIAL_SCC4_TX_BD_IDX];
  
    rmd->buf_ptr = (uchar *) SERIAL_RX_BD_MEM;            
    tmd->buf_ptr = (uchar *) SERIAL_TX_BD_MEM;

    /* fill Rx buffer with zeroes */
    hkeepflags &= ~ H_INCFILL;
    buf_start = rmd->buf_ptr;
    filbyte(buf_start, (int)BUF_SIZE, 0);

    /* fill Tx buffer with an incrementing data pattern */
    val = 0x1;
    hkeepflags |= H_INCFILL;
    buf_start = tmd->buf_ptr;
    filbyte(buf_start, (int)BUF_SIZE, val);

    /* set this flag back to initial value to avoid trouble later */
    hkeepflags &= ~H_INCFILL;

    /* Set up the RX bd empty and wrap bits. */
    rmd->status =  QUICC_BDSTAT_RX_EMPTY | QUICC_BDSTAT_RX_WRAP;
    rmd->length = 0;
    
    /* Set up the TX bd_ set wrap, last in frame, TCRC bits */
    tmd->status =  QUICC_BDSTAT_TX_WRAP | QUICC_BDSTAT_TX_LAST |
	QUICC_BDSTAT_TX_TRCRC; 
    tmd->length = BUF_SIZE;
}
/*
 * =================================================================
 * Name: set_scc_param_ram()
 *
 * Function: set up SCC parameters for HDLC   
 *
 * =================================================================
 */
static int set_scc_param_ram(volatile quicc_hdlc_param_t *hdlc_ptr)
{
    volatile quicc_scc_param_t *scc_param_ptr;

    hdlc_ptr->c_mask = SERIAL_PRAM_C_MASK;   /* 16 bit CRC */
    hdlc_ptr->c_pres = SERIAL_PRAM_C_PRES;   /* 16 bit CRC preset */
    hdlc_ptr->disfc = 0;                  /* Discarded Frame Counter */
    hdlc_ptr->crcec = 0;                  /* CRC error counter */
    hdlc_ptr->abtsc = 0;                  /* Abort Sequence Counter */
    hdlc_ptr->nmarc = 0;                  /* Nonmatching Address Rx Counter */
    hdlc_ptr->retrc = 0;                  /* Frame Retransmission Counter */
    hdlc_ptr->mflr = MFLR;                /* max frame lenth reg */
    hdlc_ptr->max_cnt = 0x0;              /* Maximum Length Counter */
    hdlc_ptr->rfthr = 0;                  /* Received Frames
				             Threshold_interrupt after every frame */
    hdlc_ptr->rfcnt = 0;                  /* Received Frames count */

    hdlc_ptr->hmask = 0x0000;      /* User Defined Address Mask to
                                      allow all addresses to be recognized*/
    hdlc_ptr->haddr1 = 0;          /* User Defined Frame Address 1 */
    hdlc_ptr->haddr2 = 0;          /* User Defined Frame Address 2 */
    hdlc_ptr->haddr3 = 0;          /* User Defined Frame Address 3 */
    hdlc_ptr->haddr4 = 0;          /* User Defined Frame Address 4 */

    scc_param_ptr = &(hdlc_ptr->scc_param);

    /* Motorola format, DMA space */

    scc_param_ptr->rfcr =  QUICC_SCCFC_MOT | QUICC_FCR_FUNC_CODE(QUICC_FC_DMA_SPACE);
    
    scc_param_ptr->tfcr =  QUICC_SCCFC_MOT | QUICC_FCR_FUNC_CODE(QUICC_FC_DMA_SPACE);

    scc_param_ptr->mrblr = MRBLR;   /* max Rx buffer length */
}
/*
 * =================================================================
 * Name: scc_lpbk()
 *
 * Function: Test SCC4 in loopback mode.
 * 
 * =================================================================
 */
static int scc_lpbk(quicc_dpr_t *dpr, int *clk_ptr)
{
    volatile quicc_scc_param_t *quicc_param_ram;  /* quicc param ram */
    volatile quicc_hdlc_param_t *quicc_prot_spec; /* quicc protocol spec. ram */
    volatile quicc_internal_regs_t *qir_p;        /* quicc internal reg */  
    quicc_bd_t *tmd, *rmd;                        /* tx, rx buffer descr. */
    volatile quicc_hdlc_param_t *hdlc_ptr;

    long  buff, *address, data, data1, *address_rx;
    int i, y, count;
    
    qir_p = &(dpr->regs);

    /* SCC4 loopback */
    prpass(testpass, "SCC4 loopback");

    rmd = &((quicc_bd_t *)(((quicc_dpr_t *)ADRSPC_QUICC_DPRAM)->user_data_1))[SERIAL_SCC4_RX_BD_IDX];
   
    tmd = &((quicc_bd_t *)(((quicc_dpr_t *)ADRSPC_QUICC_DPRAM)->user_data_1))[SERIAL_SCC4_TX_BD_IDX];

    mask_quicc_intr(dpr);  /* mask interrupt from quicc */
    set_init_bd(clk_ptr);  /* Setup and Init the Rx and Tx buffer
			   descriptor */

    /* init SCC4, don't set ENT& ENR bits in GSMR */
    init_scc(dpr, SCCLP, clk_ptr);
   
    /* enable TX and Rx bits for SCC4 */
    qir_p->scc[3].scc_gsmrl |= QUICC_SCC_GSMRL_ENT | QUICC_SCC_GSMRL_ENR;

    /* set the ready bit */
    tmd->status |=  QUICC_BDSTAT_TX_RDY; 
 
    /* check if ready bit is set */
    if (!(pstat((ulong *)&tmd->status, 2, 1, QUICC_BDSTAT_TX_RDY, QUICC_BDSTAT_TX_RDY,
		1000, 0)))
	cterr('w',0,"SCC Internal loopback: Failure to transmit a packet");

    /* check Empty bit in RxBD to see if packet is received  */  
    if (!(pstat((ulong *)&rmd->status, 2, 0,
		QUICC_BDSTAT_RX_EMPTY, QUICC_BDSTAT_RX_EMPTY,
		10000, 0)))
	cterr('w',0,"SCC Internal loopback: Failure to receive a packet");
   
    if ((rmd->status & 0x3F) != 0) 
	print_serial_errors(rmd);
 
    /* successful transmit and receive, now check the data */
    if(cmpbyte(tmd->buf_ptr, rmd->buf_ptr, TX_LENGTH)){
        cterr('f',0,"SCC Internal loopback: Transmit and receive data mismatch.");
        return(1);
    }
    return(0);
}
/*
 * =================================================================
 * Name: quicc_cpai_int_lpbk()
 *
 * Function: Internal loopback test for CPAI.
 *
 * =================================================================
 */
static int quicc_cpai_int_lpbk(quicc_dpr_t *dpr, int *clk_ptr)
{
    volatile quicc_scc_param_t *quicc_param_ram;   /* quicc param ram */
    volatile quicc_hdlc_param_t *quicc_prot_spec;  /* quicc protocol spec. ram */
    volatile quicc_internal_regs_t *qir_p;         /* quicc internal reg */
    quicc_bd_t *rmd, *tmd;                         /* rx, tx buffer descr. */
    int count;                                     /* counter */
    volatile quicc_hdlc_param_t *hdlc_ptr;

    prpass(testpass, "CPAI Internal loopback");
    qir_p = &(dpr->regs);

    rmd = &((quicc_bd_t *)(((quicc_dpr_t *)ADRSPC_QUICC_DPRAM)->user_data_1))[SERIAL_SCC4_RX_BD_IDX];
    tmd = &((quicc_bd_t *)(((quicc_dpr_t *)ADRSPC_QUICC_DPRAM)->user_data_1))[SERIAL_SCC4_TX_BD_IDX];

    /* mask interrupt from quicc */
    mask_quicc_intr(dpr);

    /* Setup and Init the Rx and Tx buffer descriptors */
    set_init_bd(clk_ptr);     

    /* setup parallel I/O ports */
    set_pio_ports(dpr); 

    /* initialize SCC4, don't set ENT& ENR bits in GSMR */    
    init_scc(dpr, CPAI, clk_ptr);
     
    /* set CPAI in loopback mode */
    set_loopback(dpr);
    sleep(1000);

    /* enable Tx and Rx for Serial channel _ SCC4 */
    qir_p->scc[3].scc_gsmrl |= QUICC_SCC_GSMRL_ENT | QUICC_SCC_GSMRL_ENR;

    /* set the ready bit for SCC4 buffer descriptors */
     tmd->status |=  QUICC_BDSTAT_TX_RDY; 

    /* check if ready bit is set */
    if (!(pstat((ulong *)&tmd->status, 2, 1, QUICC_BDSTAT_TX_RDY, 
		QUICC_BDSTAT_TX_RDY, 1000, 0)))
	cterr('w',0,"CPAI Internal loopback: Failure to transmit a packet");

    /* check Empty bit in RxBD to see if packet is received  */
    if (!(pstat((ulong *)&rmd->status, 2, 0, QUICC_BDSTAT_RX_EMPTY,  
		QUICC_BDSTAT_RX_EMPTY, 1000, 0)))
	cterr('w',0,"CPAI Internal loopback: Failure to receive a packet");
  
    if ((rmd->status & 0x3F) != 0) 
	print_serial_errors(rmd); 
   
    /* successful transmit and receive, now check the data */
    if(cmpbyte(tmd->buf_ptr, rmd->buf_ptr, 500)){
	cterr('f',0,"CPAI Internal loopback: transmit and receive data mismatch.");
	return(1);
    }
    return(0);
}
/*
 * =================================================================
 * Name: quicc_cpai_ext_lpbk()
 *
 * Function: CPAI external loopback test
 *
 * =================================================================
 */
static int quicc_cpai_ext_lpbk(quicc_dpr_t *dpr, int *clk_ptr)
{
    volatile quicc_scc_param_t *quicc_param_ram;  /* quicc param ram */
    volatile quicc_hdlc_param_t *quicc_prot_spec; /* quicc protocol spec. ram */
    volatile quicc_internal_regs_t *qir_p;        /* quicc internal reg */
    quicc_bd_t *tmd;                              /* tx buffer descr. */
    quicc_bd_t *rmd;                              /* rx buffer descr. */
    int count, i, y, loopmode;                    /* counter */
    ushort temp;
    uchar reply;

    qir_p = &(dpr->regs);
    
    prpass(testpass, "CPAI External loopback");
  
    /* mask interrupt from quicc */
    mask_quicc_intr(dpr);

    /* Setup and Init the Rx and Tx buffer descriptors */
    set_init_bd(clk_ptr);

    /* setup parallel I/O ports */
    set_pio_ports(dpr);

    /* initialize SCCs */
    init_scc(dpr, CPAI, clk_ptr);

    rmd = &((quicc_bd_t *)(((quicc_dpr_t *)ADRSPC_QUICC_DPRAM)->user_data_1))[SERIAL_SCC4_RX_BD_IDX];
  
    tmd = &((quicc_bd_t *)(((quicc_dpr_t *)ADRSPC_QUICC_DPRAM)->user_data_1))[SERIAL_SCC4_TX_BD_IDX];

    /* clear  CPAI  loopback mode */
    clear_loopback(dpr);
  
    /* enable Tx and Rx for Serial channel _ SCC4 */
    qir_p->scc[3].scc_gsmrl |= QUICC_SCC_GSMRL_ENT | QUICC_SCC_GSMRL_ENR;

    /* set the ready bit for SCC4 buffer descriptors */
    tmd->status |=  QUICC_BDSTAT_TX_RDY; 

    /* check if ready bit is set */
    if (!(pstat((ulong *)&tmd->status, 2, 1, QUICC_BDSTAT_TX_RDY,QUICC_BDSTAT_TX_RDY,
		1000, 0)))
	cterr('w',0,"CPAI External loopback: Failure to transmit a packet");
  
    /* check Empty bit in RxBD to see if packet is received  */
    if (!(pstat((ulong *)&rmd->status, 2, 0, QUICC_BDSTAT_RX_EMPTY,  
		QUICC_BDSTAT_RX_EMPTY, 1000, 0)))
	cterr('w',0,"CPAI External loopback: Failure to receive a packet");	

    if ((rmd->status & 0x3F) != 0) 
	print_serial_errors(rmd);

    /* successful of transmit and receive, now check the data */
    if(cmpbyte(tmd->buf_ptr, rmd->buf_ptr, 500)){
	cterr('f',0,"CPAI External loopback: transmit and receive data mismatch."); 
	return(1);
    }
    return(0);
}
/*
 * =================================================================
 * Name: print_serial_errors():
 *
 * Function: Print errors in data reception
 *
 * =================================================================
 */
static int print_serial_errors(quicc_bd_t *rmd)
{
    if (rmd->status & QUICC_BDSTAT_RX_FLV) 
	cterr('f',0,"Frame Length Violation"); 
    if (rmd->status & QUICC_BDSTAT_RX_NOFA)
	cterr('f',0,"Received Nonoctet Aligned Frame\n");
    if (rmd->status & QUICC_BDSTAT_RX_ABORT)
	cterr('f',0,"Abort Sequence seen in received frame");
    if (rmd->status & QUICC_BDSTAT_RX_CRCERR)   
	printf("CRC Error in received frame\n");
    if (rmd->status & QUICC_BDSTAT_RX_OVRRUN) 
	cterr('f',0,"Receiver Overrun occured during frame reception\n");
    if (rmd->status & QUICC_BDSTAT_RX_CDLOST) 
	cterr('f',0,"Carrier Detect signal lost during frame reception\n");
}
/*
 * =================================================================
 * Name: quicc_serial_cleanup():
 *
 * =================================================================
 */
static int quicc_serial_cleanup(void)
{
    quicc_dpr_t *dpr;

    dpr = (quicc_dpr_t *)ADRSPC_QUICC_DPRAM;
    reset_io();
    alarm (0);                 /* kill the alarm anyway */
    return ((int)SIG_DFL);     /* do the default action too */
}
/*
 * =================================================================
 * Name: serial_test():
 *
 * Function: Test Serial port functionality. This test is carried out 
 *           at different baud rates ranging from 3600 to 2 M.
 *           Specifically, they are 3600, 9600, 19.2K, 38.4K, 56K, 
 *           64K, 112K, 230K, 1.56M, 1.78M, 2M. The test consists of
 *           SCC loopback and CPAI internal loopback phases. 
 *
 * =================================================================
 */
int serial_test(int one)
{
    ushort control;                        /* ADRSPC control bits */
    quicc_dpr_t *dpr;                      /* quicc system ram */
    volatile quicc_internal_regs_t *qir_p; /* quicc internal reg */
    int i, phase, rtn_val;
    SIG_PF savsig;

    clk_ptr = clock_rate;

    testname("serial port internal loopback");
    savsig = signal(SIGINT, (SIG_PF)quicc_serial_cleanup);
    rtn_val = 0;

    dpr = (quicc_dpr_t *)ADRSPC_QUICC_DPRAM;
    qir_p = &(dpr->regs);

    /* mask interrupt from quicc */
    mask_quicc_intr(dpr);

    phase = 1;

    for (clk_ptr = &clock_rate[0]; clk_ptr < &clock_rate[MAXCLOCKS];
	 clk_ptr++){

	if(scc_lpbk(dpr, clk_ptr))                 /* test SCCs in loopback mode */
	    rtn_val++; 

	if(quicc_cpai_int_lpbk(dpr, clk_ptr))      /* test CPAI internal loopback */
	    rtn_val++;

	sleep(100);
    
    quicc_serial_cleanup ();
    }
    clear_loopback(dpr);
    prcomplete(testpass, errcount, 0);
    signal(SIGINT, savsig);
    return(rtn_val);
}
/*
 * =================================================================
 * Name: serial_ext_test():
 *
 * Function: Test Serial port functionality. This test is carried out 
 *           at different baud rates ranging from 3600 to 2 M.
 *           Specifically, they are 3600, 9600, 19.2K, 38.4K, 56K, 
 *           64K, 112K, 230K, 1.56M, 1.78M, 2M. This is an External 
 *           loopback test for CPAI.
 *
 * =================================================================
 */
int serial_ext_test(int one)
{
    ushort control;                        /* ADRSPC control bits */
    quicc_dpr_t *dpr;                      /* quicc system ram */
    volatile quicc_internal_regs_t *qir_p; /* quicc internal reg */
    int i, phase, rtn_val;
    SIG_PF savsig;

    clk_ptr = clock_rate;

    testname("serial port external loopback");
    savsig = signal(SIGINT, (SIG_PF)quicc_serial_cleanup);
    rtn_val = 0;

    dpr = (quicc_dpr_t *)ADRSPC_QUICC_DPRAM;
    qir_p = &(dpr->regs);

    /* mask interrupt from quicc */
    mask_quicc_intr(dpr);

    phase = 1;

    for (clk_ptr = &clock_rate[0]; clk_ptr < &clock_rate[MAXCLOCKS];
	 clk_ptr++){

	if(quicc_cpai_ext_lpbk(dpr, clk_ptr))      /* test CPAI external
						      loopback */
	    rtn_val++;
	sleep(100);
    
    quicc_serial_cleanup ();
    }
    prcomplete(testpass, errcount, 0);
    signal(SIGINT, savsig);
    return(rtn_val);
}
