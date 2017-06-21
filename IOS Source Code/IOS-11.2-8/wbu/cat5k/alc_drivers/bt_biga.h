/* $Id: bt_biga.h,v 1.1.4.1 1996/05/09 14:54:49 rbadri Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/alc_drivers/bt_biga.h,v $
 *------------------------------------------------------------------
 * bt_biga.h -- Boot time BIGA header file for IPC.
 *
 * June 12, 1995, Sanjaya Kumar
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: bt_biga.h,v $
 * Revision 1.1.4.1  1996/05/09  14:54:49  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.1  1996/04/27  06:14:16  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:39:01  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.1  1996/03/22  09:10:19  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.1  1996/03/13  07:41:15  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1  1996/03/11  07:39:44  rlowe
 * Placeholders for California.
 *
 *------------------------------------------------------------------
 * Legacy comments from Synergy repository:
 *
 * Revision 1.1  1995/06/12  23:34:56  sakumar
 * Initial version of BIGA Boot time driver for IPC.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef _BT_BIGA_H_
#define _BT_BIGA_H_

/* 
 * Hardware dependent defines 
 */
#define BT_SYHDR_SIZE 	12					/* Synergy header size		*/
#define BT_SHARED_MEM_BASE (0xb0000000)		/* Start of shared memory	*/ 

/*
 * Other constant definitions
 */
#define BT_MAX_PORT 	1			/* Max number of ports on this card */
#define	BT_IPC_BUF_SZ	1584		/* Maximum size of IPC data			*/
#define BT_NUM_TX_BUFS  32			/* number of Tx Buffers				*/
#define BT_NUM_RX_BUFS	32			/* number of Rx Buffers				*/


/*
 * Transmit to BIGA struct definition
 */
typedef struct BT_biga_tx_buffer {
	UINT8	syhdr[BT_SYHDR_SIZE];
	UINT8	data[BT_IPC_BUF_SZ];
	struct BT_biga_tx_buffer *next;
} tBT_biga_tx_buffer, *tpBT_biga_tx_buffer;

/*
 * Frame buffer shared memory layout for boot
 */
typedef struct BT_shram {
	/* BIGA Transmit (to Synergy Bus) Buffer Descriptors    */
    tBIGATD             tx_BD[BT_NUM_TX_BUFS];

	/* Transmit Buffers */
	tBT_biga_tx_buffer	tx_buf[BT_NUM_TX_BUFS];

	/* BIGA Receive Buffer Descriptors and buffers			*/
	tBIGARDBUF          rx_BD_buf[NUM_BIGA_RDS];
} tBT_shram, *tpBT_shram;

/*
 * Function Prototypes
 */

int BT_biga_tx_pkt(UINT8 *pData, UINT32 txlen);
void BT_biga_reclaim_txd_buffers(void);
tpBT_biga_tx_buffer BT_get_biga_tx_buffer(void);
void BT_free_biga_tx_buffer( tpBT_biga_tx_buffer pBuf ); 
void BT_buffer_init(void);
int BT_process_biga_rx_pkt (UINT8 *pData, UINT32 datalen);




#endif /* _BT_BIGA_H_ */
