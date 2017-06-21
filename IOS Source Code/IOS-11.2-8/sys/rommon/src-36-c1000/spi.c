/* $Id: spi.c,v 3.2.58.1 1996/03/21 23:29:24 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/rommon/src-36-c1000/spi.c,v $
 *------------------------------------------------------------------
 * spi.c
 *
 * January 1995, Rob Clevenger
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 * MC68360 SPI support routines for Sapphire
 *------------------------------------------------------------------
 * $Log: spi.c,v $
 * Revision 3.2.58.1  1996/03/21  23:29:24  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.2.2.1  1995/12/01  04:22:25  mbeesley
 * Branch: ELC_branch
 * Reorganize Rom monitor header files to make it easier to port
 * to new platforms.
 *
 * Revision 3.2  1995/11/17  18:44:33  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:07:40  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:24:12  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/06/07  22:39:38  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/*
 * This code is ported from the FLEX project.
 * Original author:
 * Aug 1994, Kevin Rowett
 * It has been "specialized" for Sapphire
 */

#include "monitor.h" 
#include "quicc.h"

static unsigned char spi_rxbuf, spi_txbuf;

/*
 *	SPI_OPEN
 *
 */
void
spi_open (void)
{
    quicc_dpr_t *dpr_ptr = (quicc_dpr_t *)ADRSPC_QUICC_DPRAM;   
    quicc_internal_regs_t *qr_ptr  = &dpr_ptr->regs;
    quicc_spi_param_t *pram_ptr= &dpr_ptr->spi;
    volatile quicc_bd_t *bd;

    qr_ptr->pip_pbdat |= SAP_QU_PT_B_PAR_ISDN_CS;
    qr_ptr->spi_spmode = (QUICC_SPMR_DIV16 | QUICC_SPMR_REVDAT |
			  QUICC_SPMR_MASTER | QUICC_SPMR_LEN(7));

    /* get ptr to user_data_1 if DPRAM was at zero */
    bd = (quicc_bd_t *)(&(((quicc_dpr_t *)(0))->user_data_1));
    /* now calculate pointers to our buffer descriptors */
    pram_ptr->rbase = (int)(&bd[MON_SPI_RX_BD_IDX]);
    pram_ptr->tbase = (int)(&bd[MON_SPI_TX_BD_IDX]);
    pram_ptr->rfcr = QUICC_SMCFC_MOT;	/* big-endian, dma space */
    pram_ptr->tfcr = QUICC_SMCFC_MOT;	/* big-endian, dma space */
    pram_ptr->mrblr = 1;			/* one char at a time	*/

    /*
     *	init buffer descriptors
     */

    /*  init RX */
    bd = &((quicc_bd_t *)(dpr_ptr->user_data_1))[MON_SPI_RX_BD_IDX];
    bd->buf_ptr = &spi_rxbuf;
    bd->length = 0;
    bd->status = QUICC_BDSTAT_RX_EMPTY | QUICC_BDSTAT_RX_WRAP;

    /*  init TX */
    bd = &((quicc_bd_t *)(dpr_ptr->user_data_1))[MON_SPI_TX_BD_IDX];
    bd->buf_ptr = &spi_txbuf;
    bd->length = 1;
    bd->status = QUICC_BDSTAT_TX_WRAP | QUICC_BDSTAT_TX_LAST;;

    quicc_cr_cmd ( QUICC_CHAN_SPI, QUICC_CPC_INIT_RXTX );
    
    qr_ptr->spi_spie = 0xff;			/* clear all ints */
    qr_ptr->spi_spim = 0;
    qr_ptr->spi_spmode |= QUICC_SPMR_ENASPI;		/* enable SPI */
}

void
spi_close (void)
{
    quicc_dpr_t *dpr_ptr = (quicc_dpr_t *)ADRSPC_QUICC_DPRAM;   
    quicc_internal_regs_t *qr_ptr  = &dpr_ptr->regs;

    /*  disable the TP3420A chip select line */
    qr_ptr->pip_pbdat |= SAP_QU_PT_B_PAR_ISDN_CS;

    quicc_cr_cmd ( QUICC_CHAN_SPI, QUICC_CPC_RX_CLOSE_BD );

    qr_ptr->spi_spie = 0xff;			/* clear all ints */
    qr_ptr->spi_spim = 0;
    qr_ptr->spi_spmode &= ~QUICC_SPMR_ENASPI;	/* disable SPI...saves power */
}

int 
spi_cmd (uchar cmd)
{
    quicc_dpr_t *dpr_ptr = (quicc_dpr_t *)ADRSPC_QUICC_DPRAM;   
    quicc_internal_regs_t *qr_ptr  = &dpr_ptr->regs;
    volatile quicc_bd_t *bd;
    uchar ret_val;
    int wd;

    /* load the TX Buffer here, and start the SPI xmitter        */
    bd = &((quicc_bd_t *)(dpr_ptr->user_data_1))[MON_SPI_TX_BD_IDX];

    /* idle, start ,enable/disable command  */

    *bd->buf_ptr =  cmd;
    bd->length =  1;
    bd->status |= QUICC_BDSTAT_TX_RDY;

    qr_ptr->pip_pbdat &= ~SAP_QU_PT_B_PAR_ISDN_CS;

    qr_ptr->spi_spcom = QUICC_SPCOM_START;  /* kick the xmitter */
	
    /*  now wait for the RX done event flag to be set            */
    bd = &((quicc_bd_t *)(dpr_ptr->user_data_1))[MON_SPI_RX_BD_IDX];
    for(wd = 0; bd->status & QUICC_BDSTAT_RX_EMPTY; wd++) {
	/*
	 * watch dog timeout?
	 */
	if (wd == 0x10000000)
	    return(-1);
    }
    qr_ptr->pip_pbdat |= SAP_QU_PT_B_PAR_ISDN_CS;

    /* empty the (unused) RX bd */
    ret_val =  *bd->buf_ptr;
    bd->status   |= QUICC_BDSTAT_RX_EMPTY;
    return((int)ret_val);
}

/* end of module */
