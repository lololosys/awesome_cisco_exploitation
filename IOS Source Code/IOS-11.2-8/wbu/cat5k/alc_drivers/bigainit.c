/* $Id: bigainit.c,v 1.1.4.1 1996/05/09 14:54:45 rbadri Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/alc_drivers/bigainit.c,v $
 *-----------------------------------------------------------------
 * bigainit.c
 *
 * December 1994, Ratinder P. S. Ahuja
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: bigainit.c,v $
 * Revision 1.1.4.1  1996/05/09  14:54:45  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.1  1996/04/27  06:14:13  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:38:56  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.1  1996/03/22  09:10:14  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.1  1996/03/13  07:41:11  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1  1996/03/11  07:39:41  rlowe
 * Placeholders for California.
 *
 *------------------------------------------------------------------
 * Legacy comments from Synergy repository:
 *
 * Revision 1.7  1995/10/04  01:30:50  kchristi
 * Fix CVS Endlog directives so that cvs-fix-merge script will work.
 *
 * Revision 1.6  1995/03/18  01:57:59  sakumar
 * Finished biag sar integration
 *
 * Revision 1.5  1995/02/06  19:06:48  rahuja
 * Changed OK and ERR to ACP_OK, ACP_ERR
 *
 * Revision 1.4  1995/01/14  00:05:11  rahuja
 * Finished biga testing
 *
 * Revision 1.3  1994/12/20  22:51:30  rahuja
 * support for sending
 *
 * Revision 1.2  1994/12/09  22:09:54  rahuja
 * *** empty log message ***
 *
 * Revision 1.1  1994/12/02  19:02:26  rahuja
 * Checked in after unit testing
 *
*
 *------------------------------------------------------------------
 * $Endlog$
 */


/*
**  FILE:  bigainit.c
**
**  CONTENTS:
**
**
**  OWNER:
**      Ratinder P. S. Ahuja
**
*/
 


/*
**	INCLUDES
*/

#ifndef LANGUAGE_C
#define LANGUAGE_C
#endif

#ifndef DEBUG
#define DEBUG
#endif

#include 	"basic_types.h"
#include	"param.h"
#include 	"biga.h"
#include	"spram.h"
#include	"shram.h"

/*
**	GLOBALS
*/

volatile UINT32 fd_mode;
UINT8 	*fd_rfpfirst; 				/*receive frame buffer, first rx buffer*/
UINT8 	*fd_rfplast; 				/*receive frame buffer, last rx buffer*/
tBIGARD	*fd_rbdfirst; 				/*receive buffer descriptor, first */
tBIGARD	*fd_rbdlast; 				/*receive buffer descriptor, last */
UINT8 	*fd_tfpfirst;				/*transmit frame buffer, first tx buffer*/
UINT8 	*fd_tfplast; 				/*transmit frame buffer, last tx buffer*/
tBIGATD	*fd_tbdfirst; 				/*transmit buffer descriptor, first*/
tBIGATD	*fd_tbdlast; 				/*transmit buffer descriptor, last*/
UINT32 	fd_softrhead; 				/*soft receive ring head pointer*/
UINT32 	fd_softrtail;				/*soft receive ring tail pointer*/
UINT32  fd_softthead;				/*soft transmit buffer descriptor, top*/
UINT32  fd_softttail; 				/*soft transmit buffer descriptor, tail*/
UINT32 	fd_txf;
UINT32 	fd_tx_octets;
UINT32 	fd_rx_octets;
UINT32	fd_notbds;


tBIGARD	*RBDstart, *RBDend;
tBIGATD	*TBDstart, *TBDend;

/*----------------------*/
/* burst size	  mask 	*/
/* ----------	  ---- 	*/
/* 16 words		0x3f  	*/
/*  8 words		0x1f  	*/
/*  4 words		0xf  	*/
/*  2 words		0x7  	*/
/*  1 word		 0x3  	*/
/*----------------------*/
INT32 bigaburst = 0x3f; 

tpBIGA_DEVICE	pBigaReg = (tpBIGA_DEVICE)(BIGA_REGBASE_ADDRESS);


INT32 rxfreecnt = 0;
UINT32 softthead, softrhead, softrtail, softttail;

/*
**	PROTOTYPES
*/

UINT32 biga_init();
void biga_reset();
INT32 biga_xmit();
UINT32 readNPreg(INT32 offset);



UINT32
getsofthead()
{
	return(softthead);
}

UINT32
getsofttail()
{
	return(softrtail);
}



/*
 * Initialize ringbuffer and dma gate array
 */
UINT32 
biga_bufinit()
{
INT32 		i;
UINT32 		rtail, p;
tpBIGARD	pBigaRd;
UINT8 		*rbuf, *last;
tpBIGATD 	pBigaTd;
UINT8 		*tbuf;
UINT8		*buf_start;
UINT8		*pchar;


	/*
	**	Setup receive side of dma controller
	*/


	/*remember first biga rx buffer descriptor*/
	pBigaRd = RBDstart = fd_rbdfirst = &acp_pShram->TxRBDBUF[0].bigard;   

	fd_rfpfirst = buf_start = acp_pShram->TxRBDBUF[0].bigard.bigard_syhdr;

#ifdef DEBUG
	printf("\n\rshram start is %x", acp_pShram);
	printf("\n\rfirst recv desc is %x", RBDstart);
	printf("\n\rbuf_start is %x", buf_start);
#endif

	/*
	**	Setup Descriptors 
	*/
	for (i = 0; i < NUM_BIGA_RDS; i++) {
		rxfreecnt++;
		pBigaRd->bigard_pData = acp_pShram->TxRBDBUF[i].bigard.bigard_syhdr;
		pBigaRd->bigard_usable_length = BIGA_RX_BUFSZ;
		pBigaRd->bigard_pNext = &acp_pShram->TxRBDBUF[i+1].bigard;

		pBigaRd = &acp_pShram->TxRBDBUF[i+1].bigard;	/* next dessriptor	*/

	} /*for*/

	pBigaRd = RBDend = &acp_pShram->TxRBDBUF[NUM_BIGA_RDS-1].bigard;


	/*
	**	The last descriptor needs to point back to the first.
	*/
	RBDend->bigard_pNext = fd_rbdfirst;	
	fd_rbdlast = RBDend;			/*remember last rx buffer descriptor*/
									/*remember last of buffer			*/
	fd_rfplast = (UINT8 *)&acp_pShram->TxRBDBUF[NUM_BIGA_RDS-1].bigard_data;   	


#ifdef DEBUG
	printf("\n\rRecv Descriptors End at %x", fd_rbdlast);
	printf("\n\rBuffers End at %x", fd_rfplast);
#endif

	softrhead = (UINT32)&(fd_rbdfirst->bigard_pNext);

	/*
	**	Store softrhead and softrtail in txspram info structure.  
	**	the softrhead and softrtail are converted from acp shared ram to 
	** 	tx sar address space
	**	Also write head and tail to BIGA
	*/

	acp_pTxSpram->txspram_info.txspram_softrhead = 
											(UINT32)MAP_SH_ACP_SAR(softrhead);
	acp_pTxSpram->txspram_info.txspram_bigastart = 
											(UINT32)MAP_SH_ACP_SAR(RBDstart);
	
	WRITE_BIGA32(RHEAD, softrhead);
	WRITE_BIGA32(RTMPH, softrhead);
#ifdef DEBUG
	printf("\n\rrecv descriptor softrhead = %x", softrhead);
#endif
	
	softrtail = (UINT32)&RBDend->bigard_pNext;
	acp_pTxSpram->txspram_info.txspram_softrtail = 
										(UINT32)MAP_SH_ACP_SAR(softrtail);
	acp_pTxSpram->txspram_info.txspram_bigaend = 
										(UINT32)MAP_SH_ACP_SAR(RBDend);

	WRITERTAIL(softrtail);
#ifdef DEBUG
	printf("\n\rrecv descriptor softrtail = %x", softrtail);
#endif


	/*
	**
	**	Setup transmit side of dma controller
	**
	*/

	TBDstart = fd_tbdfirst = pBigaTd = &acp_pShram->RxTBD[0];

	
	for (i = 0; i < NUM_BIGA_TDS; i++, pBigaTd++) {
		pBigaTd->bigatd_pNext = pBigaTd + 1;
		pBigaTd->control_u.bigatd_control = (CTL_QC_ASYNC1);

	}

	fd_tbdlast = TBDend = pBigaTd = &acp_pShram->RxTBD[NUM_BIGA_TDS-1];

	pBigaTd->bigatd_pNext = fd_tbdfirst;
#ifdef DEBUG
	printf("\n\rtransmit descriptors tbdfirst=%x ", fd_tbdfirst);
	printf("\n\rtransmit descriptors tbdlast=%x ", fd_tbdlast);
#endif

	softttail = (UINT32)&(TBDstart->bigatd_pNext);
	softthead = (UINT32)&(TBDstart->bigatd_pNext);
	WRITE_BIGA32( THEAD, (UINT32)(softthead));
	WRITE_BIGA32( TTMPH, (UINT32)(softthead))
	WRITETTAIL((UINT32)(softttail));
#ifdef DEBUG
	printf("\n\rtransmit softthead=%x ", softthead);
	printf("\n\rtransmit softttail=%x ", softttail);
#endif

	return ACP_OK;

}/*end biga_bufinit */


UINT32 
biga_init()
{
	INT32 i ;
	volatile UINT32 p;
	UINT32 freebuf;
	INT8 bitbucket;
	INT16	regval;

	printf("biga_init\n\r");

	fd_txf = 0;

	WRITE_BIGA16(NPCTL, (THHLC|RHHLC|RESET));
#ifdef DEBUG
	regval=READ_BIGA16(NPCTL);
	printf("\n\r biga_init:NPCTL=%x",regval);
#endif

	WRITE_BIGA32(BIGACFG, (CFG_S32 << 24) | (CFG_S32 << 16) |
							(CFG_S32 << 8) | CFG_S32);
#ifdef DEBUG
	regval=READ_BIGA(BIGACFG);
	printf("\n\r biga_init:BIGACFG=%x",regval);
#endif

	WRITE_BIGA16(SIST, 0xffff);
	WRITE_BIGA16(UPAD, 0xffff);
	WRITE_BIGA16(HICA, 0);
	WRITE_BIGA16(HICB, 0);
	WRITE_BIGA16(HICC, 0);


	WRITE_BIGA16(FLTR, 0xffff);

	WRITE_BIGA16(DCTRL, 0);
	WRITE_BIGA16(DCTRL2, DMA_ABTVLD);

#ifdef DEBUG
	regval=DMA_INIT;
	printf("\n\rinit DMA:%x", regval);
#endif
	WRITE_BIGA16(RLEN, BIGA_RX_BUFSZ);
#ifdef DEBUG
	regval=READ_BIGA16(RLEN);
	printf("\n\r biga_init:RLEN=%x",regval);
#endif

	INITDMA(pBigaReg);					/*disable and config dma controller	*/
	
	bitbucket = READ_BIGA8(DSTAT); 		/*clear dma status register			*/

#ifdef BIGA_TESTING
	/*
	**	Initialize biga buffers and descriptors for acp based testing,
	**	otherwise the initialization is already been done by the buffer
	**	manager.
	*/
	biga_bufinit();
#else

	/*
	**	Write the Transmitt head, tail and Recv head, tail registers
	**	These have been initialized by the buffer manager and should
	**	be in the sar address space.
	*/
#ifdef DEBUG
	printf("\n\r recv softrhead = %x", softrhead);
	printf("\n\r recv softrtail = %x", softrtail);
#endif
	WRITE_BIGA32(RHEAD, softrhead);		/* recv head		*/
	WRITE_BIGA32(RTMPH, softrhead);		/* recv temp head	*/
	WRITERTAIL(softrtail);				/* recv tail		*/


#ifdef DEBUG
    printf("\n\rtransmit softthead=%x ", softthead);
    printf("\n\rtransmit softttail=%x ", softttail);
#endif
    WRITE_BIGA32( THEAD, (UINT32)(softthead));
    WRITE_BIGA32( TTMPH, (UINT32)(softthead))
    WRITETTAIL((UINT32)(softttail));

#endif



#ifdef DEBUG
	printf("\n\r biga_bufinit done");
#endif
	
	READ_BIGA32(TTMPH, p);
#ifdef DEBUG
	printf("\n\r Transmit temphead, head and tail:");
	printf("\n\r TTMPH=%x", p);
#endif

	READ_BIGA32(THEAD, p);
#ifdef DEBUG
	printf("\n\r THEAD=%x\n\r", p);
#endif

#ifdef DEBUG
	printf("\n\r TTAIL=%x\n\r", softttail);
#endif

	READ_BIGA32(RTMPH, p);
#ifdef DEBUG
	printf("\n\r Receive temphead, head and tail:");
	printf("\n\r RTMPH=%x", p);
#endif

	READ_BIGA32(RHEAD, p);
#ifdef DEBUG
	printf("\n\r RHEAD=%x", p);
#endif

	READ_BIGA32(RTAIL, p);
#ifdef DEBUG
	printf("\n\r RTAIL=%x", p);
#endif

#ifdef DEBUG
	regval = (DMA_INIT|DMA_TXA|DMA_TXEN|DMA_RXA|DMA_RXEN);
	printf("\n\r start DMA:%x", regval);
#endif
	STARTDMA(pBigaReg);		  /* start dma controller*/
#ifdef DEBUG
	regval=READ_BIGA16(DCTRL);
	printf("\n\r biga_init DMA STARTED DCTRL=%x",regval);
#endif

	return ACP_OK;

}/*end biga_init */



void 
biga_reset()
{
/*Turn Interrupts OFF*/

	INITDMA(pBigaReg);		   /*initialize dma and disable*/

}


