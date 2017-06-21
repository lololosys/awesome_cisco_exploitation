/* $Id: shram.h,v 1.1.4.1 1996/05/09 14:54:38 rbadri Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/alc_cominc/shram.h,v $
 *-----------------------------------------------------------------
 * shram.h -- Shared RAM map
 *
 * Ratinder P. S. Ahuja
 *
 * Copyright (c) 1986-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: shram.h,v $
 * Revision 1.1.4.1  1996/05/09  14:54:38  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.1  1996/04/27  06:14:03  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:38:48  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.1  1996/03/22  09:10:06  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.2  1996/03/20  10:52:27  rlowe
 * Integrate with ios/wbu sub-tree and makefiles.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1.2.1  1996/03/13  07:41:04  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1  1996/03/11  07:39:34  rlowe
 * Placeholders for California.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 *------------------------------------------------------------------
 * shram.h -- Shared RAM map
 *
 *------------------------------------------------------------------
 *
*/

/*
**	FILE:  shram.h
**
**	CONTENTS:
**
**		TABSTOP=4
**
**		This file defines the carving of the shared memory between 
**		common area, buffer pool, queue headers, biga descriptors
**
**
**	OWNER:
**		Ratinder P. S. Ahuja
**
*/

#ifndef _SHRAM_H_
#define _SHRAM_H_

	/*
	**
	** INCLUDES
	**
	*/

#include	"basic_types.h"
#include	"memmap.h"
#include	"buffer_if.h"
#include	"queue_if.h"
#include	"sar_if.h"
#include	"ivcimap.h"


#define	ACP_SHRAM_VERSION	1


	/*
	**
	**	The share memory holds the buffers used in various queues 
	**	The buffers queues are described using the queue descriptor
	**	defined in queue_if.h
	*/


#ifdef LANGUAGE_C

typedef struct shram_info {
	
	UINT32	shram_version;				/* version number of software that 	*/
										/* manages shram					*/
	UINT32	shram_size;					/* size of shram					*/
	tpQDESC shram_pAcpToSarPool;		/* Pointer to ACP to SAR Buffer Pool*/
	tpQDESC shram_pRxToTxQ;				/* Pointer to RSAR to TSAR queue    */
	tpQDESC shram_pTxToRxQ;				/* Pointer to TSAR to RSAR queue    */
	UINT8*	shram_pSmallPacket;			/* Pointer to Small Packet (sent    */
										/* after every 20K bytes to prevent */
										/* SAGE lockup)						*/
					
} tSHRAM_INFO, *tpSHRAM_INFO;

extern tSHRAM_INFO acp_Shram_info;


#else /* assembly */

        .data
        .struct 0
shram_version_s:		.word 0			
shram_size_s:			.word 0				
shram_pAcpToSarPool_s:	.word 0			/* Pointer to ACP to SAR Buffer Pool*/
shram_pRxToTxQ_s:		.word 0			/* Pointer to RSAR to TSAR queue    */
shram_pTxToRxQ_s:		.word 0			/* Pointer to TSAR to RSAR queue    */
shram_pSmallPacket_s:	.word 0			/* Pointer to Small Packet 			*/

        .text          					/* reestablish the default 			*/ 
#endif /* language */

	/*
	** Shared (Frame Buffer) Memory Layout
	**
	*/

#ifdef LANGUAGE_C
typedef struct shram_ {
	/* Shared RAM Information Element	*/
	tSHRAM_INFO			shram_info;			

	/* BIGA Transmit (to Synergy Bus) Buffer Descriptors 	*/
	tBIGATD				RxTBD[NUM_BIGA_TDS];			

	/* BIGA Receive descriptors and buffers 				*/
	tBIGARDBUF			TxRBDBUF[NUM_BIGA_RDS];

	/* Buffers for Received Packets	that go to BIGA	*/
	tRxBigaDataBuffer	RxBigaDataBuffer[NUM_RXSAR_BIGA_BUFS]; 

	/* Buffers for Received Packets that go to ACP			*/
	tAcpRxDataBuffer	AcpRxDataBuffer[NUM_RXSAR_ACP_BUFS];	

	/* ACP to SAR Data Buffer Pool */
    tQDESC              AcpSARBufPool;
    tpQELEM             pDataBuf[NUM_ACP_TX_DATA_BUFS+1];

	/* Buffers for Data Packets that go from ACP to SARs */
	tAcpTxDataBuffer	AcpTxDataBuffer[NUM_ACP_TX_DATA_BUFS];	

	/* Buffers for IPC packets that go from ACP to NMP (via RXSAR & BIGA) */
	tAcpToNmpIpcBuffer	AcpToNmpIpcBuf[NUM_ACP_NMP_IPC_BUFS];

	/* RXSAR->TXSAR Queue for BUS output */
	tQDESC				RxToTxQ;
	tpQELEM             pRxToTx[NUM_RXSAR_BIGA_BUFS+1];

	/* TXSAR->RXSAR Queue for returning Buffers */
	tQDESC				TxToRxQ;
	tpQELEM             pTxToRx[NUM_RXSAR_BIGA_BUFS+1];

	/* Single packet sent periodically to Synergy bus to prevent SAGE lockup */
	UINT8				small_packet[72];

} tSHRAM, *tpSHRAM;

extern tpSHRAM sar_pShram;
extern tpSHRAM acp_pShram;

#endif /* LANGUAGE_C */


/*
**
**	PROTOTYPES
**
*/

#ifdef LANGUAGE_C
extern	INT32	shmem_init(void);
extern	INT32	rx_mem_init(void);
#endif

#endif /* _SHRAM_H_ */
