/* $Id: buffer_if.h,v 1.1.4.1 1996/05/09 14:54:30 rbadri Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/alc_cominc/buffer_if.h,v $
 *-----------------------------------------------------------------
 * buffer_if.h -- Buffer pools on the ACP and the TX and RX SARs
 *
 * Ratinder P. S. Ahuja
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: buffer_if.h,v $
 * Revision 1.1.4.1  1996/05/09  14:54:30  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.1  1996/04/27  06:13:55  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:38:38  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.1  1996/03/22  09:09:57  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.1  1996/03/13  07:40:55  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1  1996/03/11  07:39:26  rlowe
 * Placeholders for California.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 *------------------------------------------------------------------
 * buffer_if.h -- Buffer pools
 *			on the ACP and the TX and RX SARs
 *
 *------------------------------------------------------------------
 *
*/

/*
**	FILE:  buffer_if.h
**
**	CONTENTS:
**
**		TABSTOP=4
**
**		This file contains definitions of the various buffer pools,
**		buffer descriptors, transmit and receive descriptors, packet 
**		descriptors.
**
**		Buffer Pools:
**
**			synergy header pool
**			medium buffer (256 bytes) pool for acp traffic
**			large buffer (1500 bytes) pool for lane data traffic
**
**		Buffer desciptor:
**
**			Buffers are described using a buffer descriptor. Allows logical 
**			copying of buffers
**		
**
**		Secondry Buffer descriptors:
**
**			Packet data is resident in the shared memory, which is accessed 
**			by RxSar, TxSar, ACP and the BIGA.  To reduce contention, the 
**			data buffers in shared memory are described using descriptors
**			placed in secondry memory.
**
**
**	OWNER:
**		Ratinder P. S. Ahuja
**
*/

#ifndef _BUFFER_IF_H_
#define _BUFFER_IF_H_

#ifdef LANGUAGE_C
#ifndef PAK
#define PAK __attribute__ ((packed))
#endif /* PAK */
#endif /* LANGUAGE_C */

	/*
	**
	** INCLUDES
	**
	*/

#include	"basic_types.h"
#include	"param.h"
#include	"biga.h"
#include	"queue_if.h"

		/*
		**
		**		B U F F E R   P O O L S   A N D   Q U E U E S
		**
		*/


/*
**	S Y N E R G Y   H E A D E R
*/

#ifdef LANGUAGE_C

typedef struct synergy_hdr {

	union {

		struct {
			UINT32	type	:4 PAK;	/* identifies type and format of packet	*/
			UINT32	uinfo	:4 PAK;	/* reserved for future expansion		*/
			UINT32	control	:7 PAK;	/* controls processing of sy header		*/
			UINT32	bpdu	:1 PAK;	/* bpdu bit indicates BPDU				*/
			UINT32	vlan	:15 PAK;/* virtual lan / color identifier		*/
			UINT32	c_l		:1 PAK;	/* conditional learn bit				*/
		} control_bits;

		UINT32	control;

	} control_u;

	union {

		struct {
			UINT32	card_id	:3 PAK;	/* indicates which atm card in system	*/
			UINT32	resvd	:3 PAK;
			UINT32	vpc		:10 PAK;
			UINT32	status	:8 PAK;	/* status of recvd packet				*/
			UINT32	parity	:8 PAK;		
		} index_bits;

		UINT32	index;

	} index_u;

	UINT16	length;					/* number of data bytes including crc	*/
	UINT16	reserved;

} tSY_HDR, *tpSY_HDR;


#else	/* assembly */

        .data
        .struct 0

sy_typeu_8s:			.byte 0		/* tttt: 4 bit type field				*/
									/* uuuu: 4 bit user field				*/
sy_controlb_8s:			.byte 0		/* 7 bit control field 					*/
									/* 1 bit BPDU bit						*/
sy_vlanid_cl_s16:		.half 0		/* 15 bit vlan (color) id				*/
									/* 1 bit conditional learn bit			*/
sy_index_16s:			.half 0		/* 16 bit index field					*/

sy_status_8s:			.byte 0		/* 8 bit status							*/

sy_parity_8s:			.byte 0		/* 8 bit parity							*/

sy_length_16s:			.half 0		/* 16 bit length						*/

sy_reserved_16s:		.half 0		/* 16 bit reserved						*/

        .text           				/* reestablish the default 		*/

#endif /* LANGUAGE */


/*
**	B U F F E R   H E A D E R
**	Buffers have a header to indicate status, ownership
*/

/* Definitions for receive packet status */
#define RCV_PKT_STAT_OK_C       			0	/* Packet OK 			*/
#define RCV_PKT_STAT_ERROR_C        		1	/* Error in packet 		*/
#define RCV_PKT_STAT_TIMEOUT_ERROR_C    	2	/* Timeout Error 		*/
#define	RCV_PKT_STAT_BUF_OVFL_ERROR_C		3	/* Buffer Overflow		*/
#define	RCV_PKT_STAT_LEN_MISMATCH_ERROR_C	4	/* AAL5 len mismatch	*/
#define	RCV_PKT_STAT_BIGA_Q_OVFL_ERROR_C	5	/* BIGQ Q overflow		*/
#define	RCV_PKT_STAT_CRC_ERROR_C			6	/* AAL5 CRC Error		*/

#ifdef LANGUAGE_C

typedef struct buf_hdr {					/* buffer header structure		*/
	
	UINT8			bufh_poolid;			/* small, medium, large etc.	*/
	UINT8			bufh_usecount;			/* current number of owners		*/
	tVPC			bufh_vpc;
	UINT16			bufh_status;			/* Receive Status 				*/
	UINT16			bufh_length;			/* Received length				*/

} tBUFHDR, *tpBUFHDR;

#define mBUF_HDR_SZ 	sizeof(tBUFHDR)

#else /* assembly */

	
/* If buffer goes to ACP */
        .data
        .struct 0
bufh_poolid_8s:		.byte 0
bufh_usecount_8s:	.byte 0
bufh_vpc_16s:		.half 0
bufh_status_16s:	.half 0	/* Code assumes status and length is in same word */
bufh_length_16s:	.half 0
bufh_payload_s:

        .text           	/* reestablish the default 		*/

#endif /* language */


#ifdef LANGUAGE_C

typedef struct buffer {

	tBUFHDR			buf_hdr;
	UINT32			buf_data;			/* data begins here				*/

} tBUFFER, *tpBUFFER;

typedef struct acprxdatabuffer {
	tBUFHDR			buf_hdr;
	UINT8			buf_data[ACP_RX_BUFSZ];
} tAcpRxDataBuffer, *tpAcpRxDataBuffer;

typedef struct acptxdatabuffer {
	tBUFHDR			buf_hdr;
	UINT8			buf_data[ACP_TX_BUFSZ];
} tAcpTxDataBuffer, *tpAcpTxDataBuffer;

typedef struct rxbigadatabuffer {
	UINT8			Rsy_hdr[10]; /* Syn Hdr is 12 but 2 byte LECID is removed */
	UINT8			Rbuf_data[BIGA_TX_BUFSZ]; /* RXSAR transmits onto SynBus */
	UINT16          pad;		/* To make it 4 byte aligned */
} tRxBigaDataBuffer, *tpRxBigaDataBuffer;

typedef struct acpnmpipcbuffer {
	tBUFHDR			buf_hdr;
	UINT8			buf_data[ACP_NMP_IPC_BUFSZ];
} tAcpToNmpIpcBuffer, *tpAcpToNmpIpcBuffer;

#else

        .data
        .struct 0
buf_hdr:		.space 8
buf_data:		.word 0

        .text           				/* reestablish the default 		*/

        .data
        .struct 0
Rsy_hdr_s:		.space 10
Rbuf_data_s:	.word 0

        .text           				/* reestablish the default 		*/
#endif

	/*
	**					P R O T O T Y P E S
	**		operations on pools, buffers and descriptors
	*/

#ifdef LANGUAGE_C

/*
** Init all buffer pools
*/
#if	0
extern UINT32	bufmgr_init();
extern tpQDESC	pool_init(UINT16 poolid);
#endif

/*
**	Get and free small, medium, large buffers
*/
extern tpBUFFER	bufmgr_getbuf ( tpQDESC pPoolDesc);
extern void	bufmgr_freebuf ( tpQDESC pPoolDesc, tpBUFFER pBuf );

/*
**	Synergy header pool get and free
*/
extern 	tpSY_HDR	pool_getsyhdr(void);
extern 	tpSY_HDR	pool_freesyhdr(void);


/*
**	Operations on BIGA_TD
*/


/*
**	Operations on BIGA_RD
*/

/*
** Operations on synergy header
*/

#endif /* LANGUAGE_C */


#endif /* _BUFFER_IF_H_ */
