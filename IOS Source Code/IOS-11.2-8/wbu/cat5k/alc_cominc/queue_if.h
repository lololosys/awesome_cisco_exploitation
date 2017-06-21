/* $Id: queue_if.h,v 1.1.4.1 1996/05/09 14:54:36 rbadri Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/alc_cominc/queue_if.h,v $
 *-----------------------------------------------------------------
 * queue_if.h -- queues and queue elements.  These descriptions and
 *               operations are used by the ACP and the SARs.  This
 *               is a shared file.
 *
 * Ratinder P. S. Ahuja
 *
 * Copyright (c) 1986-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: queue_if.h,v $
 * Revision 1.1.4.1  1996/05/09  14:54:36  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.1  1996/04/27  06:14:01  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:38:46  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.1  1996/03/22  09:10:04  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.1  1996/03/13  07:41:02  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1  1996/03/11  07:39:32  rlowe
 * Placeholders for California.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 *------------------------------------------------------------------
 * queue_if.h -- queues and queue elements.  These descriptions and
 *		operations are used by the ACP and the SARs.  This is a shared
 * 		file.
 *
 *------------------------------------------------------------------
*/

/*
**	FILE:  queue_if.h
**
**	CONTENTS:
**	There are two kinds of queues in the system: Inboud traffic queues
**	and outbound traffic queues.
**
**  Queues for inbound (recv from atm network) traffic:
**
**		Recv SAR to ACP queue (rxSAR_to_ACP_q)
**		Recv SAR to BIGA transmit queue (BIGA_TX_q)
**		ACP inband traffic queue (ACP_inband_q)
**
**	Queues for outbound (from synergy to atm) traffic:
**
**		Holding queue to preserve packet order (holding_q)
**		ACP to transmit SAR queue (ACP_to_txSAR_q)
**		Recv SAR to transmit SAR queue (rxSAR_to_txSAR_q)
**
**	Transmit descriptors:
**
**			Data packets to be DMA'ed by the BIGA are described using 
**			Transmit Descriptors(BIGA_TD).  
**			
**	Receive descriptors:
**
**			Data packets received from the synergy bus are DMA'ed into 
**			buffers described using Receive Desriptors (BIGA_RD)
**
**	OWNER:
**		Ratinder P. S. Ahuja
**
*/

#ifndef _QUEUEIF_H_
#define _QUEUEIF_H_

#include	"basic_types.h"

		/*
		**	G E N E R I C   Q U E U E   D E S C R I P T O R
		*/
	

	/*
	** Various types of producer-consumer queues
	*/

#define	 ACP_RXSAR_DATA_BUFFER_POOL		1

#define	 ACP_RX_CMD_BUFFER_POOL			2
#define  ACP_RXSAR_CMD_Q				3
#define  RXSAR_ACP_RESP_Q				4
#define	 RXSAR_ACP_DATA_Q				5
#define	 BIGA_RXSAR_BUFFER_POOL			6

#define	 ACP_TX_CMD_BUFFER_POOL			8
#define  ACP_TXSAR_CMD_Q				9
#define  TXSAR_ACP_RESP_Q				10
#define	 ACP_TXSAR_BUFFER_POOL			11
#define	 TXSAR_HOLDING_Q				12
#define  TXSAR_ACP_DATA_Q				13
#define	 BIGA_TXSAR_BUFFER_POOL			14

#define	 ACP_NMP_IPC_POOL				15

#define  TXSAR_ACP_EVENT_Q				16
#define  ACP_TXSAR_EVENT_RESP_Q			17
#define  TXSAR_ACP_EVENT_POOL			18

#define	 RX_TO_TX_Q						19
#define	 TX_TO_RX_Q						20

	/*
	** A producer-consumer queue can be in one of the following types of
	** memory.
	*/
#define	Q_IN_SHARED_RAM		0
#define	Q_IN_RX_SP_RAM		1
#define	Q_IN_TX_SP_RAM		2
#define	Q_IN_ACP_MEM		3

	/*
	**
	**  Q U E U E   A N D    B U F F E R   P O O L   D E S C R I P T O R
	**
	*/


#ifdef LANGUAGE_C

typedef UINT8 * tpQELEM;

typedef struct qdesc {					/* queue descriptor structure	*/
										/* also used to describe buffer	*/
										/* pools						*/
	tpQELEM		*qdesc_pFirst;
	tpQELEM		*qdesc_pLast;
	tpQELEM		*qdesc_pProducer;		/* next element to be enqued	*/
	tpQELEM		*qdesc_pConsumer;		/* next element to be dequed	*/
	UINT32		qdesc_bufsize;			/* size of bufers in buffer pool*/
	UINT32		qdesc_qmem;				/* what memory the q is in 		*/
	UINT32		qdesc_qid;				/* queue identifier 			*/

} tQDESC, *tpQDESC;

#else /* assembly */

        .data
        .struct 0
qdesc_pFirst_s:		.word 0
qdesc_pLast_s:	.word 0
qdesc_pProducer_s:	.word 0        		/* next element to be enqued    */
qdesc_pConsumer_s:	.word 0        		/* next element to be dequed    */
qdesc_bufsize_s:	.word 0          	/* size of bufers in buffer pool*/
qdesc_qmem:			.word 0				/* what memory the q is in 		*/
qdesc_qid_s:		.word 0             /* queue identifier             */
qdesc_size_s:
        .text           				/* reestablish the default 		*/
#endif

 

	/*
	**
	**	I N B O U N D    T R A F F I C  Q U E U E S
	**
	*/

#ifdef LANGUAGE_C

/*
** 
**			    R X - S A R   T O   A C P    Q U E U E 
**
**	This is a single queue for enqueuing packet descriptors destined for
**	protocol entites on the ACP.
**
*/

extern	tpQDESC	pRxsarToAcp_q;


/*
** 
**			    B I G A   T X    Q U E U E 
**
**	This is a single queue for enqueuing transmit descriptors for the BIGA
**	Transmit implies transmission to the synergy bus.  The RX SAR enqueues
**  reassembled inbound MAC frames. The RX SAR also enqueues ACP inband 
**	traffic on behalf of the ACP.  This is done to reduce contention
**
**	NOTE:  This queue is implemented using biga tx descriptors.
*/



/*
** 
**			    A C P   I N B A N D   Q U E U E
**
**	This is a single queue for enqueuing inband management and IPC traffic
**	destined for tasks on the NMP. 
**
*/

extern	tpQDESC	pAcpInband_q;


/*
** 
**		   A C P   T X  &  R X   S A R   C O M M A N D   Q U E U E S
**
**	These command queues are in the secondary port ram for the tx and
**	rx sars
**
*/

extern	tpQDESC	pAcpRxSarCmd_q;					/* command queues			*/
extern	tpQDESC	pAcpTxSarCmd_q;
extern	tpQDESC	pAcpRxSarResp_q;				/* response queues			*/
extern	tpQDESC	pAcpTxSarResp_q;


	/*
	**
	**	O U T B O U N D    T R A F F I C  Q U E U E S
	**
	*/



/*
**
**          H O L D I N G   Q U E U E S
**
**  These queues are implemented as an array of tQDESC, indexed by a
**  an index generated for a destination MAC address, for which a flush 
**	is in progress.  By default there are 256 holding queues.  Each queue
**	holds packets destined for a MAC address.  A unique Q index needs to be
**	generated for a destination MAC address.
**
*/


extern tpQDESC	pHolding_q;				/* array of q_descriptors, indexed	*/
										/* by an index generated from a MAC	*/
										/* address							*/

extern UINT16	hq_size;				/* Number of descriptors in Hq		*/


/*
**
**          A C P   T O   T X S A R   Q U E U E 
**
**  This is a single queue for the ACP to enque transmit requests for
**	outbound data.
**
*/

extern	tpQDESC	pAcpToTxSar_q;


/*
**
**          R X S A R   T O   T X S A R   Q U E U E 
**
**  This is a single queue for the recv sar to enque transmit requests for
**	outbound data.
**
*/

extern	tpQDESC	pRxsarToTxsar_q;			


/*
**
**          B I G A   R X   Q U E U E 
**
**  This is a single queue for the BIGA to receive frames of the synergy bus.
**
**	NOTE: this queue is implemented using biga recv descriptors 
**
*/

extern tpQDESC	pBigaRecvPool;			


#endif	/* language_c */

	/*
	**			P R O T O T Y P E S
	**
	**	Operations defined on the queue / buffer descriptor
	**
	*/

#ifdef LANGUAGE_C

extern tpQDESC q_init(UINT16 qid);
extern void q_log(tpQDESC   pQ);
void qmgr_enq(tpQDESC pQdesc,  tpQELEM   pelem);
void * qmgr_deq(tpQDESC pQ);

#endif /* language */

#endif 	/* _QUEUEIF_H_ */
