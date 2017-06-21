/* $Id: bt_biga.c,v 1.1.4.1 1996/05/09 14:54:48 rbadri Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/alc_drivers/bt_biga.c,v $
 *------------------------------------------------------------------
 * bt_biga.c -- Boot time BIGA functions for IPC.
 *
 * June 12, 1995, Sanjaya Kumar
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: bt_biga.c,v $
 * Revision 1.1.4.1  1996/05/09  14:54:48  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.2  1996/05/02  06:47:22  rlowe
 * Clean up compilation warnings for bootrom-specific code.
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.40.1  1996/04/27  06:14:15  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:38:59  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.1  1996/03/22  09:10:17  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.1  1996/03/13  07:41:14  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1  1996/03/11  07:39:44  rlowe
 * Placeholders for California.
 *
 *------------------------------------------------------------------
 * Legacy comments from Synergy repository:
 *
 * Revision 1.4  1995/07/27  10:20:26  rlowe
 * Changes to allow dumping of Rx packets (if compiled in) and poking
 * of the Synergy header used for Tx msgs (if a debug monitor is available).
 *
 * Revision 1.3  1995/06/23  20:46:18  rlowe
 * Use LCP driver card instance number in Synergy header.
 *
 * Revision 1.2  1995/06/13  04:39:49  rlowe
 * This version now downloads successfully. Also removed some debugging
 * printfs.
 *
 * Revision 1.1  1995/06/12  23:34:56  sakumar
 * Initial version of BIGA Boot time driver for IPC.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef LANGUAGE_C
#define     LANGUAGE_C
#endif

/*
 * Includes
 */
 
#include "basic_types.h"
#include "biga.h"
#include "bt_biga.h"
#include "bt_biga_if.h"
#include "buffer_if.h"


/*
 * GLOBALS
 */
UINT32 				BT_biga_soft_tx_head, BT_biga_soft_tx_tail;
UINT32 				BT_biga_soft_rx_head, BT_biga_soft_rx_tail;
tpBT_biga_tx_buffer BT_head_tx_buf_list = NULLP;
tSY_HDR				BT_TXSyHdr;
tpBT_shram			pBT_shram;		/* pointer to start of Shared Memory */
UINT8				BT_Mac_Addr[6]; /* MAC address to look for			 */
tpBIGA_DEVICE   	pBigaReg_BT = (tpBIGA_DEVICE)(BIGA_REGBASE_ADDRESS);


int
BT_nmp_send2(UINT16 portindex, UINT32 datalen, void *pData, void * pS_Hdr)
{
	tpBT_biga_tx_buffer pBuf;
	tpSY_HDR			pSynHdr ;

	pSynHdr = pS_Hdr ;
	/* Check portindex (in Synergy ATM Card, has to be 0) */
    if (portindex >= BT_MAX_PORT) {
#ifdef DEBUG
		buginf("\nBT_nmp_send: Xmit failed, code -2");
#endif /* DEBUG */
        return(-2);
    }
 
    /* Check length */
    if (datalen > BT_IPC_BUF_SZ) {
#ifdef DEBUG
		buginf("\nBT_nmp_send: Xmit failed, code -3");
#endif /* DEBUG */
        return(-3);
    }
 
    /* Get TX buffer in Frame Buffer Memory */
	if ((pBuf = BT_get_biga_tx_buffer()) == NULLP) {
#ifdef DEBUG
		buginf("\nBT_nmp_send: Xmit failed, code -1");
#endif /* DEBUG */
		return(-1);
	}

	/* Copy canned Synergy header into start of data buffer */
	bcopy(pSynHdr, &pBuf->syhdr, BT_SYHDR_SIZE);

	/* Copy data from buffer in ACP DRAM to Frame Memory buffer */
    bcopy(pData, &pBuf->data, datalen);

	/* Send the packet out */
	return(BT_biga_tx_pkt((UINT8 *)pBuf, datalen+BT_SYHDR_SIZE));
}

int
BT_nmp_send(UINT16 portindex, UINT32 datalen, void *pData)
{
	return (BT_nmp_send2 (portindex, datalen, pData, &BT_TXSyHdr)) ;
}

int
BT_biga_tx_pkt(UINT8 *pData, UINT32 txlen)
{

	tpBIGATD pBigaTd;

	/* 
	 * First thing, check to see if we need to free up any previously 
	 * transmitted packets.
	 */

	BT_biga_reclaim_txd_buffers();

	/* Now see if a BIGA transmit descriptor is available */
	pBigaTd = (tpBIGATD) *(tpBIGATD *)BT_biga_soft_tx_tail;

	if ( ((UINT32) &(pBigaTd->bigatd_pNext)) == BT_biga_soft_tx_head ) {
		/* Biga Transmit queue overflow */
#ifdef DEBUG
		buginf("\nBT_biga_tx_pkt: Xmit failed, code -1");
#endif /* DEBUG */
		return (-1);
	}

	/* Next, fill in BIGA transmit descriptor */
	pBigaTd->bigatd_pData = pData;
	pBigaTd->control_u.bigatd_control = ((CTL_QC_ASYNC1) | (txlen & 0xffff));

	/* Finally, write to BIGA Registers to transmit packet */
	BT_biga_soft_tx_tail = (UINT32) &(pBigaTd->bigatd_pNext);
	WRITETTAIL(BT_biga_soft_tx_tail);

	return(0);
}

void
BT_biga_reclaim_txd_buffers()
{
	UINT32 temp_biga_tx_head;
	tpBIGATD pBigaTd;

	/* On entry, exhaustively free up all buffers that BIGA has transmitted */

	temp_biga_tx_head = BT_biga_soft_tx_head;

	/* Read the new TX_HEAD */
	READ_BIGA32(THEAD, BT_biga_soft_tx_head);

	/* Free all the buffers up to the TX_HEAD */
	while (temp_biga_tx_head != BT_biga_soft_tx_head) {
		pBigaTd = (tpBIGATD) *(tpBIGATD *) temp_biga_tx_head;
		BT_free_biga_tx_buffer((tpBT_biga_tx_buffer)pBigaTd->bigatd_pData);
		temp_biga_tx_head = (UINT32) &(pBigaTd->bigatd_pNext);
	}
}

tpBT_biga_tx_buffer
BT_get_biga_tx_buffer()
{
	tpBT_biga_tx_buffer pBuf;

	if (BT_head_tx_buf_list == NULLP) {
		return (NULLP);
	}
	pBuf = BT_head_tx_buf_list;
	BT_head_tx_buf_list = pBuf->next;
	return (pBuf);
}

void
BT_free_biga_tx_buffer( tpBT_biga_tx_buffer pBuf )
{
	pBuf->next = BT_head_tx_buf_list;
	BT_head_tx_buf_list = pBuf;
}

void
BT_buffer_init()
{
	int 	i;
	UINT8	*pCh;

	/* 
	 * Setup Shared Memory Layout for Transmit 
	 */

	pBT_shram = (tpBT_shram) BT_SHARED_MEM_BASE;

	/* Populate buffer list */
	BT_head_tx_buf_list = NULLP;
	for(i=0; i<BT_NUM_TX_BUFS; i++) {
		BT_free_biga_tx_buffer(&pBT_shram->tx_buf[i]);
	}

	/* Init BIGA Transmit Descriptors */
	for (i = 0; i < ((BT_NUM_TX_BUFS)-1); i++) {
        pBT_shram->tx_BD[i].bigatd_pNext = &pBT_shram->tx_BD[i+1];
        pBT_shram->tx_BD[i].control_u.bigatd_control = (CTL_QC_ASYNC1);
    }
	/* Last descriptor points back to first */
    pBT_shram->tx_BD[((BT_NUM_TX_BUFS)-1)].control_u.bigatd_control =
                                                (CTL_QC_ASYNC1);
    pBT_shram->tx_BD[((BT_NUM_TX_BUFS)-1)].bigatd_pNext =
                                                &pBT_shram->tx_BD[0];

	/* Setup Soft Pointers */
	BT_biga_soft_tx_head = BT_biga_soft_tx_tail =
						   (UINT32) &pBT_shram->tx_BD[0].bigatd_pNext;

	/* Setup Synergy Header */
	BT_TXSyHdr.control_u.control = 0;
    BT_TXSyHdr.index_u.index = 0;
    BT_TXSyHdr.length = 0x03;
    BT_TXSyHdr.reserved = 0;
    BT_TXSyHdr.control_u.control_bits.type = 0; /* Ethernet */
    BT_TXSyHdr.control_u.control_bits.vlan = 0;
    BT_TXSyHdr.control_u.control_bits.c_l =  0;
	/* LCP driver discovered our card instance number during startup. */
    BT_TXSyHdr.index_u.index_bits.card_id = LCP_CardInstanceNbr () ;
    BT_TXSyHdr.index_u.index_bits.vpc = 0;

	/* Write to BIGA Registers */
	WRITE_BIGA32( THEAD, (UINT32)(BT_biga_soft_tx_head));
    WRITE_BIGA32( TTMPH, (UINT32)(BT_biga_soft_tx_head))
    WRITETTAIL((UINT32)(BT_biga_soft_tx_tail));


	
	/*
	 * Setup Shared Memory Layout for Receive
	 */
	
	/* Init BIGA Receive Descriptors */
	for (i = 0; i < (BT_NUM_RX_BUFS - 1); i++) {
        pBT_shram->rx_BD_buf[i].bigard.bigard_pNext = 
								&pBT_shram->rx_BD_buf[i+1].bigard;
        pBT_shram->rx_BD_buf[i].bigard.bigard_usable_length = BIGA_RX_BUFSZ;
		pBT_shram->rx_BD_buf[i].bigard.bigard_pData = 
								pBT_shram->rx_BD_buf[i].bigard.bigard_syhdr;
	}
	pBT_shram->rx_BD_buf[(BT_NUM_RX_BUFS - 1)].bigard.bigard_pNext = 
				&pBT_shram->rx_BD_buf[0].bigard;
	pBT_shram->rx_BD_buf[(BT_NUM_RX_BUFS - 1)].bigard.bigard_usable_length = 
				BIGA_RX_BUFSZ;
	pBT_shram->rx_BD_buf[(BT_NUM_RX_BUFS - 1)].bigard.bigard_pData = 
				pBT_shram->rx_BD_buf[(BT_NUM_RX_BUFS - 1)].bigard.bigard_syhdr;

	
	/* Setup Soft Pointers */
	BT_biga_soft_rx_head = (UINT32) 
			&(pBT_shram->rx_BD_buf[0].bigard.bigard_pNext);
	BT_biga_soft_rx_tail = (UINT32) 
			&(pBT_shram->rx_BD_buf[(BT_NUM_RX_BUFS - 1)].bigard.bigard_pNext);

	/* Write to BIGA Registers */
	WRITE_BIGA32(RHEAD, BT_biga_soft_rx_head);
    WRITE_BIGA32(RTMPH, BT_biga_soft_rx_head);
	WRITERTAIL(BT_biga_soft_rx_tail);

	
	/* Get our MAC address */
	if ((void *) LCP_ReadMacAddr (BT_Mac_Addr) == NULL) {
        buginf("\nBT_buffer_init: Could not get network-side MAC addr blk");
        return;
    }



#ifdef DEBUG
	buginf("\n Mac Address:"); 
	for (i = 0; i < 6; i++) {
		buginf(" %02x", BT_Mac_Addr[i]);
	}
	buginf("\n");
	
	buginf("\n Synergy Header:");
	pCh = (UINT8 *) &BT_TXSyHdr;
	for (i = 0; i < BT_SYHDR_SIZE; i++) {
		buginf(" %02x", pCh[i]);
	}
	buginf("\n");
#endif /* DEBUG */
}

void
BT_biga_init()
{
	UINT8 bitbucket;
#ifdef DEBUG
	buginf("\nBT_biga_init: Entered");
#endif /* DEBUG */

	WRITE_BIGA16(NPCTL, (THHLC | RHHLC | RESET));
	WRITE_BIGA32(BIGACFG, (CFG_S32 << 24) | (CFG_S32 << 16) |
                          (CFG_S32 << 8) | CFG_S32);
	WRITE_BIGA16(SIST, 0xffff);
    WRITE_BIGA16(UPAD, 0xffff);
    WRITE_BIGA16(HICA, 0);
    WRITE_BIGA16(HICB, 0);
    WRITE_BIGA16(HICC, 0);
	WRITE_BIGA16(FLTR, 0xffff);
    WRITE_BIGA16(DCTRL, 0);
    WRITE_BIGA16(DCTRL2, DMA_ABTVLD);
	WRITE_BIGA16(RLEN, BIGA_RX_BUFSZ);

    INITDMA(pBigaReg_BT);               /*disable and config dma controller */
 
    bitbucket = READ_BIGA8(DSTAT);      /*clear dma status register         */

	BT_buffer_init();

	STARTDMA(pBigaReg_BT);       			/* start dma controller				*/
}

/*
*******************************************************************************
	Dump any Rx packet.  For test/diag/debug use from Boot (or Kernel).
*******************************************************************************
*/
void BT_DumpBigaRx (tpBIGARD pBH, UINT8 * pDA, UINT8 * pSA)
{
	UINT8 *	pCh ;


	buginf ("\nRx\tDA:  %02x %02x %02x %02x %02x %02x\n",
			pDA [0], pDA [1], pDA [2], pDA [3], pDA [4], pDA [5]) ;

	buginf ("\tSA:  %02x %02x %02x %02x %02x %02x\n",
			pSA [0], pSA [1], pSA [2], pSA [3], pSA [4], pSA [5]) ;

	pCh = pBH->bigard_syhdr ;
	buginf (
	"\tSH:  %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x\n",
			pCh [0], pCh [1], pCh [2], pCh [3], pCh [4], pCh [5],
			pCh [6], pCh [7], pCh [8], pCh [9], pCh [10], pCh [11]) ;

	buginf ("\tBH:  st %02x rz %02x em %01x tr %01x fl %01x id %01x rz %01x ig %01x ix %03x\n",
			pBH->dest_index_u.color_bits.sage_status,
			pBH->dest_index_u.color_bits.reserved_1,
			pBH->dest_index_u.color_bits.empty,
			pBH->dest_index_u.color_bits.trap,
			pBH->dest_index_u.color_bits.flood,
			pBH->dest_index_u.color_bits.card_id,
			pBH->dest_index_u.color_bits.reserved_2,
			pBH->dest_index_u.color_bits.i_g_bit,
			pBH->dest_index_u.color_bits.color) ;

}	/* BT_DumpBigaRx */

/*
*******************************************************************************
*
*   FUNCTION:
*       BT_biga_rx_pkt
*
*   PARAMETERS:
*       UINT8   fOnce   -- set TRUE if at most ONE inbound packet
*                  can be handled.
*
*   RETURN:
*       TRUE if an inbound packet was handled, else FALSE.
*
*   ALGORITHMS
*
*******************************************************************************
*/
int
BT_biga_rx_pkt(UINT8 fOnce)
{
    UINT32 temp_biga_rx_head;
	int in_pkt_handled = FALSE;
    tpBIGARD pBigaRd;

    /* Read the new RX_HEAD */
    READ_BIGA32(RHEAD, temp_biga_rx_head);
   
	/* 
	 * If fOnce is TRUE, then process incoming packets till one matches
	 * our MAC address, otherwise exhaustively service all received pkts.
	 */
    while (BT_biga_soft_rx_head != temp_biga_rx_head) {
        pBigaRd = (tpBIGARD) * (tpBIGARD *) BT_biga_soft_rx_head;
#if	0		/* Enable for driver debugging. */
		BT_DumpBigaRx (pBigaRd, (UINT8 *) &pBigaRd->bigard_da[0],
						(UINT8 *) &pBigaRd->bigard_sa[0]) ;
#endif
        if (TRUE == 
			     BT_process_biga_rx_pkt((UINT8 *) &pBigaRd->bigard_da[0],
								((tpSY_HDR)pBigaRd->bigard_syhdr)->length) ) {
			in_pkt_handled = TRUE;
		}

		/* Update both the soft tail and the Receive TAIL register */
		BT_biga_soft_rx_tail = BT_biga_soft_rx_head;
		WRITERTAIL(BT_biga_soft_rx_tail);

		/* Increment Head to next RBD */
        BT_biga_soft_rx_head = (UINT32) &(pBigaRd->bigard_pNext);

		if (fOnce && in_pkt_handled) {
			return(TRUE);
		}
    }

	return(in_pkt_handled);
}

int
BT_process_biga_rx_pkt (UINT8 *pData, UINT32 datalen)
{
	int i;

	/* Does this packet's DA match our MAC address? Return FALSE if not */
	for (i = 0; i < 6; i++) {
		if (pData[i] != BT_Mac_Addr[i]) {
			return (FALSE);
		}
	}

	ipc_pkt_rx(pData, datalen); /* Upcall to Download IPC */
	
	return(TRUE);
}
