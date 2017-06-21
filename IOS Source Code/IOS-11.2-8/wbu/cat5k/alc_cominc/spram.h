/* $Id: spram.h,v 1.1.4.2 1996/06/18 16:44:50 sakumar Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/alc_cominc/spram.h,v $
 *-----------------------------------------------------------------
 * spram.h -- Secondry port RAM map.
 *
 * Ratinder P. S. Ahuja
 *
 * Copyright (c) 1986-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: spram.h,v $
 * Revision 1.1.4.2  1996/06/18  16:44:50  sakumar
 * CSCdi60668:  PVC (RFC1483) support needs to be added to Cat5K ATM Module
 * Branch: California_branch
 *
 * Revision 1.1.4.1  1996/05/09  14:54:39  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.1  1996/04/27  06:14:04  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:38:49  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.1  1996/03/22  09:10:07  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.3  1996/03/20  10:52:28  rlowe
 * Integrate with ios/wbu sub-tree and makefiles.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1.2.2  1996/03/14  00:29:08  rlowe
 * Eliminate debugging switch.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1.2.1  1996/03/13  07:41:05  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1  1996/03/11  07:39:35  rlowe
 * Placeholders for California.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 *------------------------------------------------------------------
 * spram.h -- Secondry port RAM map.
 *	
 *------------------------------------------------------------------
 *
*/

/*
**	FILE:  spram.h
**
**	CONTENTS:
**		This file contains the secondry port RAM map.  The TX and RX sars
**		have secondry port RAMs associated with them.  
**
**		For the RX sar, the SPRAM is used to hold:
**			Inbound VCI translation table (vcmap.h)
**			Buffer descripters  (buffer_if.h)
**
** 		For the TX sar, the SPRAM is used to hold:
**			CAM table			(cam_if.h)
**			Outbound VCI table	(vcmap.h)
**			Buffer desc			(buffer_if.h)
**
**	OWNER:
**		Ratinder P. S. Ahuja
**
*/

#ifndef _SPRAM_H_
#define _SPRAM_H_

#include	"basic_types.h"
#include	"memmap.h"
#include	"queue_if.h"
#include	"buffer_if.h"
#include	"ivcimap.h"
#include	"sar_if.h"
#include	"cam_if.h"
#include	"lec_if.h"
#include	"shram.h"

#define	ACP_RXSPRAM_VERSION	1

	/*
	**  RX SAR secondary ram information
	*/

#ifdef	LANGUAGE_C

typedef struct rxspram_info {

	UINT32	rxspram_version;
	UINT32	rxspram_size;

					/* Buffer queue descriptors */
	void	*rxspram_pAcpBufPool;		/* ACP data buffer queue			*/
	void	*rxspram_pBigaBufPool;		/* pointer to BIGA Buffer Queue Desc*/

					/* Other descriptors	*/
	void	*rxspram_pBigatd;			/* start of biga td list			*/
	void	*rxspram_pAcpCmdPool;		/* pool of command buffers			*/
	void	*rxspram_pAcpCmdq;			/* command queue description		*/
	void	*rxspram_pAcpRespq;			/* response queue description		*/
	void	*rxspram_pRxToAcpDataq;		/* data indication queue			*/
	void	*rxspram_pAcpToNnpIpcPool; 	/* ptr to BufferQ for ACP to NMP IPC*/
	void	*rxspram_pEntryTable;		/* table of ptrs to vcmap entries	*/
	void	*rxspram_pMapTable;			/* ivci map table					*/
    void    *rxspram_pStats;            /* statistics record                */
	UINT32	debug0;
	UINT32	debug1;
	UINT32	debug2;
	UINT32	debug3;
	UINT8	*rxspram_pFree;				/* begining of free area in rxspram	*/

} tRXSPRAM_INFO, *tpRXSPRAM_INFO;

extern tRXSPRAM_INFO acp_Rxspram_info;

#else /* LANGUAGE_ASSEMBLY */
 
        .data
        .struct 0
rxspram_version_s:			.word 0
rxspram_size_s:				.word 0
rxspram_pAcpBufPool_s:		.word 0			
rxspram_pBigaBufPool_s:		.word 0			
                    
rxspram_pBigatd_s:			.word 0			
rxspram_pAcpCmdPool_s:		.word 0
rxspram_pAcpCmdq_s:			.word 0			
rxspram_pAcpRespq_s:		.word 0			
rxspram_pRxToAcpDataq_s:	.word 0			
rxspram_pAcpToNnpIpcPool_s: .word 0 	/* ptr to BufferQ for ACP to NMP IPC*/
rxspram_pEntryTable_s:		.word 0		
rxspram_pMapTable_s:		.word 0	
rxspram_pStats_s:           .word 0     /* points to global rx stats        */
debug0						.word 0
debug1						.word 0
debug2						.word 0
debug3						.word 0
rxspram_pFree_s:			.word 0

        .text           				
#endif /* language */

	/*
	** RX SPRAM Layout
	**
	*/

#ifdef LANGUAGE_C
typedef struct rxspram_ {
	tRXSPRAM_INFO		rxspram_info;
	tRXSARGLSTAT        rxstat_gl;                  /* global stats         */
    tRXSARVCSTAT        rxstat_vc[MAX_DEFAULT_VPC]; /* per vc stats         */
	tQDESC				BigaBufPool;
	tpQELEM				pBigaBuf[NUM_RXSAR_BIGA_BUFS+1];
	tQDESC				AcpBufPool;
	tpQELEM				pAcpBuf[NUM_RXSAR_ACP_BUFS+1];
	tQDESC				RxToAcpDataq;
	tpQELEM				pRxToAcpData[NUM_RXSAR_ACP_BUFS+1];
	tQDESC				AcpCmdPool;
	tpQELEM				pAcpCmdBuf[NUM_RX_CMD_BUFS+1];
	tQDESC				AcpCmdq;
	tpQELEM				pAcpCmd[NUM_RX_CMD_BUFS+1];
	tQDESC				AcpRespq;
	tpQELEM				pAcpResp[NUM_RX_CMD_BUFS+1];
	tQDESC				AcpToNmpIpcPool;
	tpQELEM				pAcpToNmpIpcBuf[NUM_ACP_NMP_IPC_BUFS+1];
	
	tIMAPENTRY			MapTable[MAX_DEFAULT_VPC];
	tpIMAPENTRY			MapEntryPtrTable[MAX_DEFAULT_VPC];

	tSARCMD				cmd_buffer[NUM_RX_CMD_BUFS];
} tRXSPRAM, *tpRXSPRAM;

extern tpRXSPRAM rxsar_pRxSpram;
extern tpRXSPRAM acp_pRxSpram;

#endif /* LANGUAGE_C */


	/*
	**
	**  TX SAR secondary ram information
	**
	*/

#define	ACP_TXSPRAM_VERSION	1

#ifdef LANGUAGE_C

typedef struct txspram_info {

	UINT32	txspram_version;
	UINT32	txspram_size;
	UINT32	txspram_softrhead;			/* biga recv head				*/
	UINT32	txspram_softrtail;			/* biga recv tail				*/
	UINT32	txspram_bigastart;			/* first biga desc				*/
	UINT32	txspram_bigaend;			/* last biga desc				*/

					/* Buffer pool descriptors */
	void	*txspram_pBigard;			/* start of biga rd list		*/

					/* Queue descriptors: 	*/
					/* command pools and Q's*/
	void	*txspram_pRxSarToTxSarq;	/* queue from rx sar to tx sar	*/
	void	*txspram_pAcpCmdPool;		/* pool of command buffers		*/
	void	*txspram_pAcpCmdq;			/* command queue description	*/
	void	*txspram_pAcpRespq;			/* response queue description	*/

					/* event pools and Q's	*/
	void	*txspram_pTxToAcpEventPool;	/* Pool of event buffers		*/
	void	*txspram_pTxToAcpEventq;	/* Q for tx->acp events, such as*/
										/* Inband IPC etc.				*/
	void	*txspram_pAcpToTxEventRespq;/* Response Q for TX->ACP Events*/

					/* cam, bus and vpc tables	*/

	void	*txspram_pCamTable;			/* CAM table					*/
	void	*txspram_pLecTable;			/* BUS vpc table				*/
    void    *txspram_pStats;            /* pointer to global tx stats   */
	UINT32	txspram_debug0;
	UINT32	txspram_debug1;
	UINT32	txspram_debug2;
	UINT32	txspram_debug3;
	UINT8	*txspram_pFree;				/* begining of free area in rxspram	*/

} tTXSPRAM_INFO, *tpTXSPRAM_INFO;

extern tTXSPRAM_INFO acp_Txspram_info;


#else /* assembley */

	/* 
	**	tTXSPRAM_INFO structure 
	*/

        .data
        .struct 0
txspram_version_s:			.word 0
txspram_size_s:				.word 0
txspram_softrhead_s:		.word 0 	
txspram_softrtail_s:		.word 0
txspram_bigastart_s:		.word 0	
txspram_bigaend_s:			.word 0

txspram_pBigard_s:			.word 0		/* start of biga rd list            */
 
                    /* Queue descriptors    */
 
txspram_pRxSarToTxSarq_s:	.word 0		/* queue from rx sar to tx sar  */
txspram_pAcpCmdPool_s:		.word 0		/* pool of command buffers		*/
txspram_pAcpCmdq_s:			.word 0     /* command queue description    */
txspram_pAcpRespq_s:		.word 0     /* response queue description   */
txspram_pTxToAcpEventPool_s:.word 0		/* pool of event buffers		*/
txspram_pTxToAcpEventq_s:	.word 0		/* Q for Inband IPC etc.		*/
txspram_pAcpToTxEventRespq_s:.word 0	/* Response Q for TX->ACP Events*/
 
                    /* cam, bus and vpc tables  */
 
txspram_pCamTable_s:		.word 0     /* CAM table                    */
txspram_pLecTable_s:		.word 0     /* BUS vpc table                */
txspram_pStats_s:           .word 0     /* points to global tx stats    */
txspram_debug0				.word 0
txspram_debug1				.word 0
txspram_debug2				.word 0
txspram_debug3				.word 0
txspram_pFree_s:			.word 0

        .text           				/* reestablish the default 			*/ 

#endif /* language */

		/*
		**	TX SPRAM actual layout
		*/

#ifdef LANGUAGE_C

typedef struct txspram_ {

	tTXSPRAM_INFO		txspram_info;

			/* Statistics  */
	tTXSARGLSTAT		txstat_gl;					/* global stats			*/
	tTXSARVCSTAT		txstat_vc[MAX_DEFAULT_VPC];	/* per vc stats			*/

			/* Command pool */
	tQDESC				AcpCmdPool;				
	tpQELEM				pAcpCmdBuf[NUM_TX_CMD_BUFS+1];

			/* Command queue */
	tQDESC				AcpCmdq;
	tpQELEM				pAcpCmd[NUM_TX_CMD_BUFS+1];

			/* Response queue */
	tQDESC				AcpRespq;
	tpQELEM				pAcpResp[NUM_TX_CMD_BUFS+1];

			/* Event pool 	*/
	tQDESC				TxToAcpEventPool;
	tpQELEM				pTxToAcpEventPool[NUM_TX_ACP_EVENT_BUFS+1];

	tQDESC				TxToAcpEventq;
	tpQELEM				pTxToAcpEvent[NUM_TX_ACP_EVENT_BUFS+1];

	tQDESC				TxToAcpEventRespq;
	tpQELEM				pAcpToTxEventResp[NUM_TX_ACP_EVENT_BUFS+1];

	tSARCMD				event_buffer[NUM_TX_ACP_EVENT_BUFS];

	tSARCMD				cmd_buffer[NUM_TX_CMD_BUFS];

	tCAM_ENTRY			CamTable[CAM_TABLE_SIZE];
	tLEC_ENTRY			LecTable[MAX_COLOR];		/* has lecid and bus vc	*/
													/* for a color (vlanid)	*/
    tPVC_info			PVCTable[MAX_DEFAULT_VPC];  /* for VLAN->PVC mapping */

} tTXSPRAM, *tpTXSPRAM;

extern tpTXSPRAM txsar_pTxSpram;
extern tpTXSPRAM acp_pTxSpram;

#endif /* LANGUAGE_C */

/*
**
**	PROTOTYPES
**
*/

#ifdef LANGUAGE_C

extern	void	spram_init(void);
extern	void	spram_log(void);

#endif

#endif /* _SPRAM_H_ */

