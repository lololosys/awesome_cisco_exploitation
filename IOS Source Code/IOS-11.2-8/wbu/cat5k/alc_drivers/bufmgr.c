/* $Id: bufmgr.c,v 1.1.4.2 1996/06/18 16:45:11 sakumar Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/alc_drivers/bufmgr.c,v $
 *------------------------------------------------------------------
 * bufmgr.c --  Buffer and queue manager 
 *
 * Ratinder P. S. Ahuja
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: bufmgr.c,v $
 * Revision 1.1.4.2  1996/06/18  16:45:11  sakumar
 * CSCdi60668:  PVC (RFC1483) support needs to be added to Cat5K ATM Module
 * Branch: California_branch
 *
 * Revision 1.1.4.1  1996/05/09  14:54:50  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.1  1996/04/27  06:14:18  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:39:03  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.1  1996/03/22  09:10:20  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.1  1996/03/13  07:41:17  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1  1996/03/11  07:39:46  rlowe
 * Placeholders for California.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 *------------------------------------------------------------------
 * bufmgr.c --  Buffer and queue manager 
 *
 *------------------------------------------------------------------
*/
 
/*
**  FILE:  bufmgr.c
**
**  CONTENTS:
**  
**	This file contains the initialization for the following buffer pools
**	in the system:
**
**  Buffer pools for inbound (recv from atm network) traffic:
**
**      Large reassembly buffer pool for lane traffic (large_reass_bufpool)
**      Medium reassembly buffer pool for acp bound traffic (medium_reass_bufpool)
**		ACP command buffer pool (sar_cmd_pool)
**
**  Queues and buffer pools for outbound (from synergy to atm) traffic:
**
**      BIGA receive pool (BIGA_RX_pool)
**      Holding queue to preserve packet order (holding_q)
**      ACP to transmit SAR queue (ACP_to_txSAR_q)
**      Recv SAR to transmit SAR queue (rxSAR_to_txSAR_q)
**
**  OWNER:
**      Ratinder P. S. Ahuja
**
*/

/*
**
**	INCLUDES
**
*/

#ifndef LANGUAGE_C
#define LANGUAGE_C
#endif

#include	"basic_types.h"
#include	"param.h"
#include	"biga.h"
#include	"queue_if.h"
#include	"buffer_if.h"
#include	"ivcimap.h"
#include	"ovcimap.h"
#include	"cam_if.h"
#include	"memmap.h"
#include	"shram.h"
#include	"spram.h"
#include	"lo_wdog.h"
#include	"acp_hw_defs.h"
#include	"acp_c_defs.h"


/*
**
**	GLOBALS
**
*/

tpSHRAM			sar_pShram;
tpSHRAM			acp_pShram;

tpRXSPRAM		rxsar_pRxSpram;
tpRXSPRAM		acp_pRxSpram;

tpTXSPRAM		txsar_pTxSpram;
tpTXSPRAM		acp_pTxSpram;

tSHRAM_INFO		acp_Shram_info;
tRXSPRAM_INFO	acp_Rxspram_info;
tTXSPRAM_INFO	acp_Txspram_info;


/*
**
**	PROTOTYPES
**
*/

UINT8 *qid2str(UINT32 qid);
void qmgr_enq(tpQDESC pQ,  tpQELEM	pElem);
void * qmgr_deq(tpQDESC pQ);
void qmgr_init_q(tpQDESC acp_q_ptr, tpQDESC sar_q_ptr, UINT32 num_elements, UINT32 buf_size, UINT32 qmem, UINT32 qid);

/*
*******************************************************************************
*
*	FUNCTION: 
*			shmem_init
*
*	PARAMETERS:
*			NONE
*
*	RETURN:
*			ACP_OK
*			ACP_ERR
*
*	ALGORITHMS
*
*******************************************************************************
*/

INT32
shmem_init()
{

INT32 i;
tpBIGARD    pBigaRd, pPrevBigaRd;

	/*
	**	Initialize shared memory offset
	*/

	PUNCH_WD;

	sar_pShram = (tpSHRAM)(SAR_SHRAM_START_C);
	acp_pShram = (tpSHRAM)(ACP_SHRAM_START_C);
#ifdef DEBUG
	printf("\nStart of Shared Frame Buffer Memory: 0x%x\n", acp_pShram);
#endif

	acp_Shram_info.shram_version = 
	acp_pShram->shram_info.shram_version = ACP_SHRAM_VERSION;

	acp_Shram_info.shram_size = 
	acp_pShram->shram_info.shram_size = SHRAM_SIZE;

	printf("\nBytes needed for shared memory structure: %d\n", sizeof(tSHRAM));
	printf("\nActual size of shared memory: %d\n", SHRAM_SIZE);
	if (sizeof(tSHRAM) > SHRAM_SIZE) {
		printf("\nshmem_init: MEMORY ALLOCATION ACP_ERROR\n");
		printf("\nBytes needed for shared memory structure: %d\n", sizeof(tSHRAM));
		printf("\nActual size of shared memory: %d\n", SHRAM_SIZE);
		return (ACP_ERR);
	}

	/*
	 * Initialize ACP to SAR Data Queue
	 */
    acp_Shram_info.shram_pAcpToSarPool = 
						(tpQDESC) MAP_SH_SAR_ACP((&sar_pShram->AcpSARBufPool));
    acp_pShram->shram_info.shram_pAcpToSarPool = &sar_pShram->AcpSARBufPool;
    qmgr_init_q(acp_Shram_info.shram_pAcpToSarPool,
    			acp_pShram->shram_info.shram_pAcpToSarPool,
                NUM_ACP_TX_DATA_BUFS, ACP_TX_BUFSZ, 
				Q_IN_SHARED_RAM, ACP_TXSAR_BUFFER_POOL);

	/*
	 * Populate ACP to SAR Data Pool with Data Buffers
	 */
	PUNCH_WD;
    for (i = 0; i < NUM_ACP_TX_DATA_BUFS; i++) {
        qmgr_enq(acp_Shram_info.shram_pAcpToSarPool,
                 (tpQELEM) &sar_pShram->AcpTxDataBuffer[i]);
    }

	/*
	**	
	**	BIGA Transmit descriptors
	**
	*/

	PUNCH_WD;
    for (i = 0; i < ((NUM_BIGA_TDS)-1); i++) {
        acp_pShram->RxTBD[i].bigatd_pNext = &sar_pShram->RxTBD[i+1];
        acp_pShram->RxTBD[i].control_u.bigatd_control = (CTL_QC_ASYNC1);
    }

		/*
		**	Last descriptor points back to first
		*/

	acp_pShram->RxTBD[((NUM_BIGA_TDS)-1)].control_u.bigatd_control = 
												(CTL_QC_ASYNC1);
    acp_pShram->RxTBD[((NUM_BIGA_TDS)-1)].bigatd_pNext = 
												&sar_pShram->RxTBD[0];

		/*
		**	softtail and softthead in sar address space
		*/

	TBDstart =  &sar_pShram->RxTBD[0];
	TBDend = &sar_pShram->RxTBD[NUM_BIGA_TDS-1];
	softttail = softthead = (UINT32)&TBDstart->bigatd_pNext;
	
	/*
	**
	**	BIGA Receive descriptors and buffers
	**		Create the forward and backward links
	**
	*/

    pBigaRd = &acp_pShram->TxRBDBUF[0].bigard;	/* first biga recv desc		*/
                                                /* all pointers in the desc */
                                                /* fall in sars memory space*/
    pPrevBigaRd = &sar_pShram->TxRBDBUF[(NUM_BIGA_RDS-1)].bigard;

	PUNCH_WD;
    for (i = 0; i < (NUM_BIGA_RDS-1); i++) {

        pBigaRd->bigard_pData = sar_pShram->TxRBDBUF[i].bigard.bigard_syhdr;
        pBigaRd->bigard_usable_length = BIGA_RX_BUFSZ;
        pBigaRd->bigard_pNext = &sar_pShram->TxRBDBUF[i+1].bigard;
        pBigaRd->bigard_pPrev = pPrevBigaRd;

 		pPrevBigaRd = &sar_pShram->TxRBDBUF[i].bigard;	/* this becomes prev*/
														/* in sar addr space*/
        pBigaRd = &acp_pShram->TxRBDBUF[i+1].bigard;    /* next dessriptor  */
 
    } /*for*/
 
 
    	/*
    	**  The last descriptor needs to point back to the first.
    	*/
 
    acp_pShram->TxRBDBUF[NUM_BIGA_RDS-1].bigard.bigard_pData = 
					sar_pShram->TxRBDBUF[NUM_BIGA_RDS-1].bigard.bigard_syhdr;

    acp_pShram->TxRBDBUF[NUM_BIGA_RDS-1].bigard.bigard_usable_length = 
					BIGA_RX_BUFSZ;

    acp_pShram->TxRBDBUF[NUM_BIGA_RDS-1].bigard.bigard_pNext =
					&sar_pShram->TxRBDBUF[0].bigard;	/* pnext -> first rd */

    acp_pShram->TxRBDBUF[NUM_BIGA_RDS-1].bigard.bigard_pPrev =
					pPrevBigaRd;			/* prev as computed in for loop */

		/*
		**	initialize softrhead and softrtail in SAR ADDRESS SPACE
		*/

	RBDstart = &sar_pShram->TxRBDBUF[0].bigard;
	RBDend = &sar_pShram->TxRBDBUF[NUM_BIGA_RDS-1].bigard;
	softrhead = (UINT32)&RBDstart->bigard_pNext;
	softrtail = (UINT32)&RBDend->bigard_pNext;


		/*
		** Initialize SAR to SAR queues
		*/
    acp_Shram_info.shram_pRxToTxQ =
                        (tpQDESC) MAP_SH_SAR_ACP((&sar_pShram->RxToTxQ));
    acp_pShram->shram_info.shram_pRxToTxQ = &sar_pShram->RxToTxQ;
    qmgr_init_q(acp_Shram_info.shram_pRxToTxQ,
                acp_pShram->shram_info.shram_pRxToTxQ,
                NUM_RXSAR_BIGA_BUFS, BIGA_RX_BUFSZ,
                Q_IN_SHARED_RAM, RX_TO_TX_Q);

    acp_Shram_info.shram_pTxToRxQ =
                        (tpQDESC) MAP_SH_SAR_ACP((&sar_pShram->TxToRxQ));
    acp_pShram->shram_info.shram_pTxToRxQ = &sar_pShram->TxToRxQ;
    qmgr_init_q(acp_Shram_info.shram_pTxToRxQ,
                acp_pShram->shram_info.shram_pTxToRxQ,
                NUM_RXSAR_BIGA_BUFS, BIGA_RX_BUFSZ,
                Q_IN_SHARED_RAM, TX_TO_RX_Q);


		/*
		**  initialize Small Packet (sent to Synergy bus every 20,000
		**  bytes to prevent SAGE lockup).
		*/
	
    acp_Shram_info.shram_pSmallPacket = 
					MAP_SH_SAR_ACP((&(sar_pShram->small_packet[0])));
    acp_pShram->shram_info.shram_pSmallPacket = &(sar_pShram->small_packet[0]);

	acp_Shram_info.shram_pSmallPacket[0] = 0x00;
	acp_Shram_info.shram_pSmallPacket[1] = 0x23; /* DontForwrd, DontLrn, BPDU */
	acp_Shram_info.shram_pSmallPacket[2] = 0x07; /* (1018) << 1 :: high byte  */
	acp_Shram_info.shram_pSmallPacket[3] = 0xf4; /* (1018) << 1 :: low byte   */
	acp_Shram_info.shram_pSmallPacket[4] = (LCP_CardInstanceNbr() << 5);
	acp_Shram_info.shram_pSmallPacket[5] = 0x00;
	acp_Shram_info.shram_pSmallPacket[6] = 0x00;
	acp_Shram_info.shram_pSmallPacket[7] = 0x00;
	acp_Shram_info.shram_pSmallPacket[8] = 0x00;
	acp_Shram_info.shram_pSmallPacket[9] = 0x03; /* Actual lngth put in by HW */
	acp_Shram_info.shram_pSmallPacket[10] = 0x00;
	acp_Shram_info.shram_pSmallPacket[11] = 0x00;
	acp_Shram_info.shram_pSmallPacket[12] = 0x01; /* DA */
	acp_Shram_info.shram_pSmallPacket[13] = 0x80; /* DA */
	acp_Shram_info.shram_pSmallPacket[14] = 0xc2; /* DA */
	acp_Shram_info.shram_pSmallPacket[15] = 0x00; /* DA */
	acp_Shram_info.shram_pSmallPacket[16] = 0x00; /* DA */
	acp_Shram_info.shram_pSmallPacket[17] = 0x00; /* DA */

	acp_Shram_info.shram_pSmallPacket[18] = 0x01; /* FAKE SA */
	acp_Shram_info.shram_pSmallPacket[19] = 0x02; /* FAKE SA */
	acp_Shram_info.shram_pSmallPacket[20] = 0x03; /* FAKE SA */
	acp_Shram_info.shram_pSmallPacket[21] = 0x04; /* FAKE SA */
	acp_Shram_info.shram_pSmallPacket[22] = 0x05; /* FAKE SA */
	acp_Shram_info.shram_pSmallPacket[23] = 0x06; /* FAKE SA */

	PUNCH_WD;
	return(ACP_OK);

}/* end shmem_init */



/*
*******************************************************************************
*
*	FUNCTION: 
*			rx_mem_init
*
*	PARAMETERS:
*			NONE
*
*	RETURN:
*			ACP_OK
*			ACP_ERR
*
*	ALGORITHMS
*
*
*******************************************************************************
*/

INT32
rx_mem_init()
{
INT32 		i, j;
UINT8		*pCHAR;

	PUNCH_WD;
	rxsar_pRxSpram = (tpRXSPRAM) RX_SAR_SPRAM_START_C;
	acp_pRxSpram   = (tpRXSPRAM) ACP_RXSAR_SPRAM_START_C;

#ifdef DEBUG
	printf("\nStart of RxSAR Secondary Port Memory: 0x%x\n", acp_pRxSpram);
#endif

	printf("\nBytes needed for Rx Secondary Port memory structure: %d\n", 
			sizeof(tRXSPRAM));
	printf("\nActual size of Rx Secondary Port memory: %d\n", 
			RX_SAR_SPRAM_SIZE_C);
	if (sizeof(tRXSPRAM) > RX_SAR_SPRAM_SIZE_C) {
		printf("\nrx_mem_init: MEMORY ALLOCATION ACP_ERROR\n");
		printf("\nBytes needed for Rx Secondary Port memory structure: %d\n", 
				sizeof(tRXSPRAM));
		printf("\nActual size of Rx Secondary Port memory: %d\n", RX_SAR_SPRAM_SIZE_C);
		return(ACP_ERR);
	}
	
	/* Initialize Rx Secondary Port Shared Memory Structure */
    acp_Rxspram_info.rxspram_version =
    		acp_pRxSpram->rxspram_info.rxspram_version = ACP_RXSPRAM_VERSION;
    acp_Rxspram_info.rxspram_size =
    		acp_pRxSpram->rxspram_info.rxspram_size = RX_SAR_SPRAM_SIZE_C;

#ifdef DEBUG
    acp_Rxspram_info.debug0 = acp_pRxSpram->rxspram_info.debug0 = 0;
    acp_Rxspram_info.debug1 = acp_pRxSpram->rxspram_info.debug1 = 0;
    acp_Rxspram_info.debug2 = acp_pRxSpram->rxspram_info.debug2 = 0;
    acp_Rxspram_info.debug3 = acp_pRxSpram->rxspram_info.debug3 = 0;
#endif /* DEBUG


	/*
	**  Clear out statistcs
	*/

	PUNCH_WD;
	acp_Rxspram_info.rxspram_pStats = MAP_SP_RX_ACP(&rxsar_pRxSpram->rxstat_gl);
	acp_pRxSpram->rxspram_info.rxspram_pStats = &rxsar_pRxSpram->rxstat_gl;
	memset(acp_Rxspram_info.rxspram_pStats, 0x00,
				(sizeof(tRXSARGLSTAT) + sizeof(tRXSARVCSTAT)*MAX_DEFAULT_VPC));

	/* 
	 * Initialize Queues 
	 */
	PUNCH_WD;
	acp_Rxspram_info.rxspram_pBigaBufPool = 
								MAP_SP_RX_ACP(&rxsar_pRxSpram->BigaBufPool);
	acp_pRxSpram->rxspram_info.rxspram_pBigaBufPool = 
								&rxsar_pRxSpram->BigaBufPool;
	qmgr_init_q(acp_Rxspram_info.rxspram_pBigaBufPool,
				acp_pRxSpram->rxspram_info.rxspram_pBigaBufPool, 
				NUM_RXSAR_BIGA_BUFS, BIGA_RX_BUFSZ, 
				Q_IN_RX_SP_RAM, BIGA_RXSAR_BUFFER_POOL);


	PUNCH_WD;
	acp_Rxspram_info.rxspram_pAcpBufPool = 
								MAP_SP_RX_ACP(&rxsar_pRxSpram->AcpBufPool);
	acp_pRxSpram->rxspram_info.rxspram_pAcpBufPool = 
								&rxsar_pRxSpram->AcpBufPool;
	qmgr_init_q(acp_Rxspram_info.rxspram_pAcpBufPool,
				acp_pRxSpram->rxspram_info.rxspram_pAcpBufPool, 
				NUM_RXSAR_ACP_BUFS, ACP_RX_BUFSZ, 
				Q_IN_RX_SP_RAM, ACP_RXSAR_DATA_BUFFER_POOL);

	PUNCH_WD;
	acp_Rxspram_info.rxspram_pRxToAcpDataq = 
								MAP_SP_RX_ACP(&rxsar_pRxSpram->RxToAcpDataq);
	acp_pRxSpram->rxspram_info.rxspram_pRxToAcpDataq = 
								&rxsar_pRxSpram->RxToAcpDataq;
	qmgr_init_q(acp_Rxspram_info.rxspram_pRxToAcpDataq,
				acp_pRxSpram->rxspram_info.rxspram_pRxToAcpDataq,
				NUM_RXSAR_ACP_BUFS, 
				ACP_RX_BUFSZ, 
				Q_IN_RX_SP_RAM, 
				RXSAR_ACP_DATA_Q);


	PUNCH_WD;
	acp_Rxspram_info.rxspram_pAcpCmdPool = 
								MAP_SP_RX_ACP(&rxsar_pRxSpram->AcpCmdPool); 
	acp_pRxSpram->rxspram_info.rxspram_pAcpCmdPool = 
								&rxsar_pRxSpram->AcpCmdPool; 
	qmgr_init_q(acp_Rxspram_info.rxspram_pAcpCmdPool,
				acp_pRxSpram->rxspram_info.rxspram_pAcpCmdPool, 
			    NUM_RX_CMD_BUFS, 
				CMD_BUFSZ, 
				Q_IN_RX_SP_RAM, 
				ACP_RX_CMD_BUFFER_POOL);

	PUNCH_WD;
	acp_Rxspram_info.rxspram_pAcpCmdq = 
								MAP_SP_RX_ACP(&rxsar_pRxSpram->AcpCmdq); 
	acp_pRxSpram->rxspram_info.rxspram_pAcpCmdq = 
								&rxsar_pRxSpram->AcpCmdq; 
	qmgr_init_q(acp_Rxspram_info.rxspram_pAcpCmdq,
				acp_pRxSpram->rxspram_info.rxspram_pAcpCmdq, 
			    NUM_RX_CMD_BUFS, 
				CMD_BUFSZ, 
				Q_IN_RX_SP_RAM, 
				ACP_RXSAR_CMD_Q);

	PUNCH_WD;
	acp_Rxspram_info.rxspram_pAcpRespq = 
								MAP_SP_RX_ACP(&rxsar_pRxSpram->AcpRespq);
	acp_pRxSpram->rxspram_info.rxspram_pAcpRespq = 
								&rxsar_pRxSpram->AcpRespq;
	qmgr_init_q(acp_Rxspram_info.rxspram_pAcpRespq,
				acp_pRxSpram->rxspram_info.rxspram_pAcpRespq, 
				NUM_RX_CMD_BUFS, 
				CMD_BUFSZ, 
				Q_IN_RX_SP_RAM, 
				RXSAR_ACP_RESP_Q);


	PUNCH_WD;
	acp_Rxspram_info.rxspram_pAcpToNnpIpcPool = 
							MAP_SP_RX_ACP(&rxsar_pRxSpram->AcpToNmpIpcPool);
	acp_pRxSpram->rxspram_info.rxspram_pAcpToNnpIpcPool =
							&rxsar_pRxSpram->AcpToNmpIpcPool;
	qmgr_init_q(acp_Rxspram_info.rxspram_pAcpToNnpIpcPool,
				acp_pRxSpram->rxspram_info.rxspram_pAcpToNnpIpcPool,
				NUM_ACP_NMP_IPC_BUFS, 
				ACP_NMP_IPC_BUFSZ, 
				Q_IN_RX_SP_RAM,
				ACP_NMP_IPC_POOL);
				

	/* 
	 * Populate "pool" queues 
	 */
	PUNCH_WD;
	for (i = 0; i < NUM_RXSAR_BIGA_BUFS; i++) {
		qmgr_enq(acp_Rxspram_info.rxspram_pBigaBufPool, 
				 (tpQELEM) &sar_pShram->RxBigaDataBuffer[i]);
	}

	PUNCH_WD;
	for (i = 0; i < NUM_RXSAR_ACP_BUFS; i++) {
		qmgr_enq(acp_Rxspram_info.rxspram_pAcpBufPool, 
				 (tpQELEM) &sar_pShram->AcpRxDataBuffer[i]);
	}

	PUNCH_WD;
	for (i = 0; i < NUM_RX_CMD_BUFS; i++) {
		qmgr_enq(acp_Rxspram_info.rxspram_pAcpCmdPool, 
				 (tpQELEM) &rxsar_pRxSpram->cmd_buffer[i]);
	}

	PUNCH_WD;
	for (i = 0; i < NUM_ACP_NMP_IPC_BUFS; i++) {
		qmgr_enq(acp_Rxspram_info.rxspram_pAcpToNnpIpcPool, 
				 (tpQELEM) &sar_pShram->AcpToNmpIpcBuf[i]);
	}


	/* 
	 * BIGA Transmit (to Synergy Bus) Buffer Descriptors 
	 */
	acp_pRxSpram->rxspram_info.rxspram_pBigatd = &sar_pShram->RxTBD[0];


	/* 
	 * Initialize VC Map Table Entries to zero
	 */

	PUNCH_WD;
	acp_Rxspram_info.rxspram_pMapTable = 
								MAP_SP_RX_ACP(&rxsar_pRxSpram->MapTable[0]);
	acp_pRxSpram->rxspram_info.rxspram_pMapTable = 
								&rxsar_pRxSpram->MapTable[0];
	
	for(i = 0; i < MAX_DEFAULT_VPC; i++) {
		pCHAR = (UINT8 *) MAP_SP_RX_ACP(&rxsar_pRxSpram->MapTable[i]);
		for (j = 0; j < sizeof(tIMAPENTRY); j++) {
			*pCHAR++ = 0;
		}
	}


	/* 
	 * Initialize Table of Pointers to VC Map Table Entries 
	 */
	PUNCH_WD;
	acp_Rxspram_info.rxspram_pEntryTable = 
							MAP_SP_RX_ACP(&rxsar_pRxSpram->MapEntryPtrTable[0]);
	acp_pRxSpram->rxspram_info.rxspram_pEntryTable = 
							&rxsar_pRxSpram->MapEntryPtrTable[0];

	for(i=0; i < MAX_DEFAULT_VPC; i++) {
		acp_pRxSpram->MapEntryPtrTable[i] = &rxsar_pRxSpram->MapTable[i];
	}


	PUNCH_WD;
	return(ACP_OK);

}/*end rx_mem_init */



/*
*******************************************************************************
*
*   FUNCTION:
*           tx_mem_init
*
*   PARAMETERS:
*           NONE
*
*   RETURN:
*           ACP_OK
*           ACP_ERR
*
*   ALGORITHMS
*
*
*******************************************************************************
*/

UINT32
tx_mem_init()
{

INT32 	i;
UINT8	*pCh = NULLP;

	PUNCH_WD;
	txsar_pTxSpram = (tpTXSPRAM) (TX_SAR_SPRAM_START_C);
	acp_pTxSpram = (tpTXSPRAM) (ACP_TXSAR_SPRAM_START_C);

#ifdef DEBUG
    printf("\nStart of TxSAR Secondary Port Memory: 0x%x\n", acp_pTxSpram);
#endif /* DEBUG */
 
	printf("\nBytes needed for Tx Secondary Port memory structure: %d\n",
			sizeof(tTXSPRAM));
	printf("\nActual size of Tx Secondary Port memory: %d\n", 
			TX_SAR_SPRAM_SIZE_C);
    if (sizeof(tTXSPRAM) > TX_SAR_SPRAM_SIZE_C) {
        printf("\ntx_mem_init: MEMORY ALLOCATION ACP_ERROR\n");
        printf("\nBytes needed for Tx Secondary Port memory structure: %d\n",
                sizeof(tTXSPRAM));
        printf("\nActual size of Tx Secondary Port memory: %d\n", TX_SAR_SPRAM_SIZE_C);        return(ACP_ERR);
    }

	acp_Txspram_info.txspram_debug0 = 
		acp_pTxSpram->txspram_info.txspram_debug0 = 0xdead;
	acp_Txspram_info.txspram_debug1 = 
		acp_pTxSpram->txspram_info.txspram_debug1 = 0xdead;
	acp_Txspram_info.txspram_debug2 = 
		acp_pTxSpram->txspram_info.txspram_debug2 = 0xdead;
	acp_Txspram_info.txspram_debug3 = 
		acp_pTxSpram->txspram_info.txspram_debug3 = 0xdead;

    /* Initialize Tx Secondary Port Shared Memory Structure */
	acp_Txspram_info.txspram_version =
	acp_pTxSpram->txspram_info.txspram_version = ACP_TXSPRAM_VERSION;

    acp_Txspram_info.txspram_size =
    acp_pTxSpram->txspram_info.txspram_size = TX_SAR_SPRAM_SIZE_C;
 
	/*
	**	Clear stats
	*/
	PUNCH_WD;
	acp_Txspram_info.txspram_pStats = MAP_SP_TX_ACP(&txsar_pTxSpram->txstat_gl);
	acp_pTxSpram->txspram_info.txspram_pStats = &txsar_pTxSpram->txstat_gl;
	memset(acp_Txspram_info.txspram_pStats, 0x00, 
				(sizeof(tTXSARGLSTAT) + sizeof(tTXSARVCSTAT)*MAX_DEFAULT_VPC));


    /*
    **  Store softrhead and softrtail in txspram info structure.
    */
 
    acp_pTxSpram->txspram_info.txspram_softrhead = softrhead;
    acp_pTxSpram->txspram_info.txspram_bigastart = (UINT32)RBDstart;
 
    acp_pTxSpram->txspram_info.txspram_softrtail = softrtail;
    acp_pTxSpram->txspram_info.txspram_bigaend = (UINT32)RBDend;


    /*
	**
    ** Initialize pools and queues
	**
    */

	/*	
	**	Command buffer pool, request and response queues 
	*/

	PUNCH_WD;
    acp_Txspram_info.txspram_pAcpCmdPool = 
									MAP_SP_TX_ACP(&txsar_pTxSpram->AcpCmdPool);
    acp_pTxSpram->txspram_info.txspram_pAcpCmdPool = 
									&txsar_pTxSpram->AcpCmdPool;
    qmgr_init_q(acp_Txspram_info.txspram_pAcpCmdPool,
				acp_pTxSpram->txspram_info.txspram_pAcpCmdPool,
                NUM_TX_CMD_BUFS, 
				CMD_BUFSZ, 
				Q_IN_TX_SP_RAM, 
				ACP_TX_CMD_BUFFER_POOL);
 
	PUNCH_WD;
    acp_Txspram_info.txspram_pAcpCmdq = 
									MAP_SP_TX_ACP(&txsar_pTxSpram->AcpCmdq);
    acp_pTxSpram->txspram_info.txspram_pAcpCmdq = 
									&txsar_pTxSpram->AcpCmdq;
    qmgr_init_q(acp_Txspram_info.txspram_pAcpCmdq,
				acp_pTxSpram->txspram_info.txspram_pAcpCmdq,
                NUM_TX_CMD_BUFS, 
				CMD_BUFSZ, 
				Q_IN_TX_SP_RAM, 
				ACP_TXSAR_CMD_Q);
    
	PUNCH_WD;
    acp_Txspram_info.txspram_pAcpRespq = 
									MAP_SP_TX_ACP(&txsar_pTxSpram->AcpRespq);
    acp_pTxSpram->txspram_info.txspram_pAcpRespq = 
									&txsar_pTxSpram->AcpRespq;
    qmgr_init_q(acp_Txspram_info.txspram_pAcpRespq,
				acp_pTxSpram->txspram_info.txspram_pAcpRespq,
                NUM_TX_CMD_BUFS, 
				CMD_BUFSZ, 
				Q_IN_TX_SP_RAM, 
				TXSAR_ACP_RESP_Q);

	/*	
	**	Event pool, event ind and resp q 	
	*/

	PUNCH_WD;
    acp_Txspram_info.txspram_pTxToAcpEventPool =
                            MAP_SP_TX_ACP(&txsar_pTxSpram->TxToAcpEventPool);
    acp_pTxSpram->txspram_info.txspram_pTxToAcpEventPool =
                             &txsar_pTxSpram->TxToAcpEventPool;
    qmgr_init_q(acp_Txspram_info.txspram_pTxToAcpEventPool,
                acp_pTxSpram->txspram_info.txspram_pTxToAcpEventPool,
                NUM_TX_ACP_EVENT_BUFS,
                CMD_BUFSZ,
                Q_IN_TX_SP_RAM,
                TXSAR_ACP_EVENT_POOL);
 

	PUNCH_WD;
    acp_Txspram_info.txspram_pTxToAcpEventq = 
								MAP_SP_TX_ACP(&txsar_pTxSpram->TxToAcpEventq);
    acp_pTxSpram->txspram_info.txspram_pTxToAcpEventq = 
								&txsar_pTxSpram->TxToAcpEventq;
    qmgr_init_q(acp_Txspram_info.txspram_pTxToAcpEventq,
				acp_pTxSpram->txspram_info.txspram_pTxToAcpEventq,
                NUM_TX_ACP_EVENT_BUFS, 
				CMD_BUFSZ, 
				Q_IN_TX_SP_RAM, 
				TXSAR_ACP_EVENT_Q);
	


	PUNCH_WD;
    acp_Txspram_info.txspram_pAcpToTxEventRespq = 
							MAP_SP_TX_ACP(&txsar_pTxSpram->TxToAcpEventRespq);
    acp_pTxSpram->txspram_info.txspram_pAcpToTxEventRespq = 
							&txsar_pTxSpram->TxToAcpEventRespq;
    qmgr_init_q(acp_Txspram_info.txspram_pAcpToTxEventRespq,
				acp_pTxSpram->txspram_info.txspram_pAcpToTxEventRespq,
                NUM_TX_ACP_EVENT_BUFS, 
				CMD_BUFSZ, 
				Q_IN_TX_SP_RAM, 
				ACP_TXSAR_EVENT_RESP_Q);
	
	/*
	 * Populate Command Pool, with elements from the TX SPRAM address space
	 */
	PUNCH_WD;
	for (i = 0; i < NUM_TX_CMD_BUFS; i++) {
		qmgr_enq(acp_Txspram_info.txspram_pAcpCmdPool, 
				 (tpQELEM) &txsar_pTxSpram->cmd_buffer[i]);
	}

	/*
	 * Populate Event Pool, with elements from the TX SPRAM address space
	 */
	PUNCH_WD;
	for (i = 0; i < NUM_TX_ACP_EVENT_BUFS; i++) {
		qmgr_enq(acp_Txspram_info.txspram_pTxToAcpEventPool, 
				 (tpQELEM) &txsar_pTxSpram->event_buffer[i]);
	}


    /*
    **
    **  BIGA Receive (from synergy bus) Buffer Descriptors
    **
    */
                                                /* in the SARs address space */
    acp_pTxSpram->txspram_info.txspram_pBigard = &sar_pShram->TxRBDBUF[0];


	/*
 	 * Initialize pointers to Cam Table and Bus Table
 	 */

	acp_Txspram_info.txspram_pCamTable = 
								MAP_SP_TX_ACP(&txsar_pTxSpram->CamTable[0]);
	acp_pTxSpram->txspram_info.txspram_pCamTable = 
								&txsar_pTxSpram->CamTable[0];
	/*
	**	Zero out the cam table
	*/
	PUNCH_WD;
	pCh = (UINT8 *)acp_Txspram_info.txspram_pCamTable;
	for(i=0; i < (sizeof(tCAM_ENTRY)*CAM_TABLE_SIZE); i++) {
		*pCh++ = 0x00;
	}


	acp_Txspram_info.txspram_pLecTable = 
							MAP_SP_TX_ACP(&txsar_pTxSpram->LecTable[0]);
	acp_pTxSpram->txspram_info.txspram_pLecTable = 
							&txsar_pTxSpram->LecTable[0];
	
	/*
	**	Zero out bus table
	*/
	PUNCH_WD;
	pCh = (UINT8 *)acp_Txspram_info.txspram_pLecTable;
	for(i=0; i < (sizeof(tLEC_ENTRY)*MAX_COLOR); i++) {
		*pCh++ = 0x00;
	}

    /*
    **  Zero out PVC table
    */
    PUNCH_WD;
    pCh = (UINT8 *)MAP_SP_TX_ACP(&txsar_pTxSpram->PVCTable[0]);
    for(i=0; i < (sizeof(tPVC_info)*MAX_DEFAULT_VPC); i++) {
        *pCh++ = 0x00;
    }


	PUNCH_WD;
	return ACP_OK;

}/*end tx_mem_init */



/*
*******************************************************************************
*
*   FUNCTION:
*           shmem_log
*
*   PARAMETERS:
*           NONE
*
*   RETURN:
*           NONE
*
*   ALGORITHMS
*			Dump the contents of the shared ram
*
*******************************************************************************
*/

void
shmem_log()
{

	printf("\n\r\t\t SHRAM MAP start 0x%x:", acp_pShram);
	printf("\n\rshram_version: %d", acp_pShram->shram_info.shram_version);
	printf("\n\rshram_size: %d", acp_pShram->shram_info.shram_size);
	printf("\n\rshram_pAcpToSarPool:");
	q_log(acp_pShram->shram_info.shram_pAcpToSarPool);

}/*end shmem_log */




/*
*******************************************************************************
*
*   FUNCTION:
*           spram_log
*
*   PARAMETERS:
*           NONE
*
*   RETURN:
*           NONE
*
*   ALGORITHMS
*
*
*******************************************************************************
*/

void
spram_log()
{
	
	/*
	**	Dump contents of RX spram
	*/
	PUNCH_WD;
	printf("\n\r\t\t RX SPRAM MAP start 0x%x:", acp_pRxSpram);
	printf("\n\rrxspram_version: %d", acp_pRxSpram->rxspram_info.rxspram_version);
    printf("\n\rrxspram_size: %d", acp_pRxSpram->rxspram_info.rxspram_size);
 
                    /* Buffer queue descriptors */
    printf("\n\rrxspram_pAcpBufPool: 0x%x", 
			acp_pRxSpram->rxspram_info.rxspram_pAcpBufPool);
    printf("\n\rrxspram_pBigaBufPool: 0x%x", 
			acp_pRxSpram->rxspram_info.rxspram_pBigaBufPool);
 
                    /* Other descriptors    */
    printf("\n\rrxspram_pBigatd: 0x%x", 
			acp_pRxSpram->rxspram_info.rxspram_pBigatd);
					
	
    printf("\n\rrxspram_pAcpCmdPool: 0x%x", 
			acp_pRxSpram->rxspram_info.rxspram_pAcpCmdPool);
    printf("\n\rrxspram_pAcpCmdq: 0x%x", 
			acp_pRxSpram->rxspram_info.rxspram_pAcpCmdq);
    printf("\n\rrxspram_pAcpRespq: 0x%x", 
			acp_pRxSpram->rxspram_info.rxspram_pAcpRespq);
    printf("\n\rrxspram_pRxToAcpDataq: 0x%x", 
			acp_pRxSpram->rxspram_info.rxspram_pRxToAcpDataq);
    printf("\n\rrxspram_pEntryTable: 0x%x", 
			acp_pRxSpram->rxspram_info.rxspram_pEntryTable);
    printf("\n\rrxspram_pMapTable: 0x%x", 
			acp_pRxSpram->rxspram_info.rxspram_pMapTable);

	/*
	**	Dump contents of TX spram
	*/
	PUNCH_WD;
	printf("\n\r\t\t TX SPRAM MAP start 0x%x:", acp_pTxSpram);
    printf("\n\rtxspram_version: %d", acp_pTxSpram->txspram_info.txspram_version);
    printf("\n\rtxspram_size: %d", acp_pTxSpram->txspram_info.txspram_size);
 
                    /* Buffer pool descriptors */
    printf("\n\rtxspram_pBigard: 0x%x", acp_pTxSpram->txspram_info.txspram_pBigard);
 
                    /* Queue descriptors    */
 
    printf("\n\rtxspram_pRxSarToTxSarq: 0x%x", 
											acp_pTxSpram->txspram_info.txspram_pRxSarToTxSarq);
    printf("\n\rtxspram_pAcpCmdPool: 0x%x", acp_pTxSpram->txspram_info.txspram_pAcpCmdPool);
    printf("\n\rtxspram_pAcpCmdq: 0x%x", acp_pTxSpram->txspram_info.txspram_pAcpCmdq);
    printf("\n\rtxspram_pAcpRespq: 0x%x", acp_pTxSpram->txspram_info.txspram_pAcpRespq);
 
                    /* cam, bus and vpc tables  */
 
    printf("\n\rtxspram_pCamTable: 0x%x", acp_pTxSpram->txspram_info.txspram_pCamTable);
    printf("\n\rtxspram_pLecTable: 0x%x", acp_pTxSpram->txspram_info.txspram_pLecTable);
    printf("\n\rtxspram_pFree: 0x%x", acp_pTxSpram->txspram_info.txspram_pFree);
	PUNCH_WD;


}/* end spram_log */



/*
*******************************************************************************
*
*   FUNCTION:
*           q_log
*
*   PARAMETERS:
*           NONE
*
*   RETURN:
*           NONE
*
*   ALGORITHMS
*
*
*******************************************************************************
*/

void
q_log(tpQDESC	pQ)
{

	printf("\n\r\t Q descriptor for %s at 0x%x ", qid2str(pQ->qdesc_qid));
	printf("\n\r\t Q descriptor address 0x%x ", pQ);
    printf("\n\r qdesc_pFirst: 0x%x", pQ->qdesc_pFirst);
    printf("\n\r qdesc_pLast: 0x%x", pQ->qdesc_pLast);
    printf("\n\r qdesc_pProducer: 0x%x", pQ->qdesc_pProducer);
    printf("\n\r qdesc_pConsumer: 0x%x", pQ->qdesc_pConsumer);
    printf("\n\r qdesc_bufsize: 0x%x", pQ->qdesc_bufsize);

}


/*
*******************************************************************************
*
*   FUNCTION:
*           qid2str
*
*   PARAMETERS:
*           NONE
*
*   RETURN:
*           UINT8 *
*
*   ALGORITHMS
*
*
*******************************************************************************
*/

UINT8 *
qid2str(UINT32 qid)
{
	switch(qid) {

	case ACP_RXSAR_DATA_BUFFER_POOL:	return "ACP_RXSAR_DATA_BUFFER_POOL";
	case ACP_RX_CMD_BUFFER_POOL:		return "ACP_RX_CMD_BUFFER_POOL";
	case ACP_RXSAR_CMD_Q:				return "ACP_RXSAR_CMD_Q";
	case RXSAR_ACP_RESP_Q:				return "RXSAR_ACP_RESP_Q";
	case RXSAR_ACP_DATA_Q:				return "RXSAR_ACP_DATA_Q";
	case BIGA_RXSAR_BUFFER_POOL:		return "BIGA_RXSAR_BUFFER_POOL";
	case ACP_TX_CMD_BUFFER_POOL:		return "ACP_TX_CMD_BUFFER_POOL";
	case ACP_TXSAR_CMD_Q:				return "ACP_TXSAR_CMD_Q";
	case TXSAR_ACP_RESP_Q:				return "TXSAR_ACP_RESP_Q";
	case ACP_TXSAR_BUFFER_POOL:			return "ACP_TXSAR_BUFFER_POOL";
	case TXSAR_HOLDING_Q:				return "TXSAR_HOLDING_Q";
	case TXSAR_ACP_DATA_Q:				return "TXSAR_ACP_DATA_Q";
	case BIGA_TXSAR_BUFFER_POOL:		return "BIGA_TXSAR_BUFFER_POOL";
	case RX_TO_TX_Q:					return "RX_TO_TX_Q";
	case TX_TO_RX_Q:					return "TX_TO_RX_Q";
	default:							return "UNKNOWN Q";

	}

}



/* 
 * Function to Initialize Producer-Consumer Queue 
 *
 *	PARAMETERS:
 *
 *			tpQDESC acp_q_ptr,			queue pointer in acp space
 *			tpQDESC sar_q_ptr, 			queue pointer in sar space
 *			UINT32 num_elements, 		number of elements in queue
 *			UINT32 buf_size, 			size of buffers in queue	
 *			UINT32 qmem, 				which memory the Q resides in
 *			UINT32 qid					
 *
 */
void
qmgr_init_q(tpQDESC acp_q_ptr, 		
			tpQDESC sar_q_ptr, 
			UINT32 num_elements, 
			UINT32 buf_size, 
			UINT32 qmem, 
			UINT32 qid)
{
	acp_q_ptr->qdesc_pFirst = 
					(tpQELEM *) ( (UINT32) sar_q_ptr + sizeof(tQDESC));
    acp_q_ptr->qdesc_pLast = 
					(tpQELEM *) ((UINT32) acp_q_ptr->qdesc_pFirst +
                                 ((sizeof(UINT8 **)) * (num_elements)));
    acp_q_ptr->qdesc_pConsumer = 
					acp_q_ptr->qdesc_pProducer = acp_q_ptr->qdesc_pFirst;
    acp_q_ptr->qdesc_bufsize = buf_size;
    acp_q_ptr->qdesc_qmem = qmem;
    acp_q_ptr->qdesc_qid = qid;
}/* end qmgr_init_q */


void
qmgr_enq(tpQDESC pQ, tpQELEM pElem)
{
	tpQELEM * acp_pProducer;

	switch(pQ->qdesc_qmem) { /* which kind of memory does the queue live in ? */
		case Q_IN_SHARED_RAM:
			acp_pProducer = (tpQELEM *) MAP_SH_SAR_ACP(pQ->qdesc_pProducer);
			break;
		case Q_IN_RX_SP_RAM:
			acp_pProducer = (tpQELEM *) MAP_SP_RX_ACP(pQ->qdesc_pProducer);
			break;
		case Q_IN_TX_SP_RAM:
			acp_pProducer = (tpQELEM *) MAP_SP_TX_ACP(pQ->qdesc_pProducer);
			break;
		case Q_IN_ACP_MEM:
			acp_pProducer = pQ->qdesc_pProducer;
			break;
	}

	*(acp_pProducer) = (pElem);
   	if(pQ->qdesc_pProducer == pQ->qdesc_pLast) {
#ifdef DEBUG
		if (pQ->qdesc_pFirst == pQ->qdesc_pConsumer) {
			printf("\n\r******* UNEXPECTED Q OVERFLOW!  ******************* 0x%x ", pQ->qdesc_qid);
		}
#endif /* DEBUG */
		pQ->qdesc_pProducer = pQ->qdesc_pFirst;  
	} else {
#ifdef DEBUG
		acp_pProducer = pQ->qdesc_pProducer;
		acp_pProducer++;
		if (acp_pProducer == pQ->qdesc_pConsumer) {
			printf("\n\r******* UNEXPECTED Q OVERFLOW!  ******************* 0x%x ", pQ->qdesc_qid);
		}
#endif /* DEBUG */
   		pQ->qdesc_pProducer++;          
	}

}/* end qmgr_enq */


void *
qmgr_deq(tpQDESC pQ) 
{
	tpQELEM	pElem;
	tpQELEM * acp_pConsumer;

	if(pQ->qdesc_pProducer == pQ->qdesc_pConsumer) {
		return(NULLP);                                    
    }

    switch(pQ->qdesc_qmem) { /* which kind of memory does the queue live in ? */ 
        case Q_IN_SHARED_RAM:
            acp_pConsumer = (tpQELEM *) MAP_SH_SAR_ACP(pQ->qdesc_pConsumer);
            break;    
        case Q_IN_RX_SP_RAM:
            acp_pConsumer = (tpQELEM *) MAP_SP_RX_ACP(pQ->qdesc_pConsumer);
            break;
        case Q_IN_TX_SP_RAM:
            acp_pConsumer = (tpQELEM *) MAP_SP_TX_ACP(pQ->qdesc_pConsumer);
            break;
        case Q_IN_ACP_MEM:
            acp_pConsumer = pQ->qdesc_pConsumer;
            break;       
    } 

	pElem = *(acp_pConsumer);                 

	if(pQ->qdesc_pConsumer == pQ->qdesc_pLast) {
		pQ->qdesc_pConsumer = pQ->qdesc_pFirst; 
	} else {
		pQ->qdesc_pConsumer++;                            
	}

	return(pElem);

}/* end qmgr_deq */



/*
*******************************************************************************
*
*   FUNCTION:
*           bufmgr_getbuf
*
*   PARAMETERS:
*           NONE
*
*   RETURN:
*           ACP_OK
*           ACP_ERR
*
*   ALGORITHMS
*
*******************************************************************************
*/
 
tpBUFFER
bufmgr_getbuf(tpQDESC pPoolDesc)
{
tpBUFFER bufp = NULLP;
 
    bufp = qmgr_deq(pPoolDesc);
    return bufp;
 
}/*end bufmgr_getbuf */
 
 
/*
*******************************************************************************
*
*   FUNCTION:
*           bufmgr_freebuf
*
*   PARAMETERS:
*           NONE
*
*   RETURN:
*           ACP_OK
*           ACP_ERR
*
*   ALGORITHMS
*
*
*******************************************************************************
*/
 
 
void
bufmgr_freebuf(tpQDESC pPoolDesc, tpBUFFER  pBuf)
{
 
    qmgr_enq(pPoolDesc, (tpQELEM)pBuf);
 
}/*end bufmgr_freebuf */
 


