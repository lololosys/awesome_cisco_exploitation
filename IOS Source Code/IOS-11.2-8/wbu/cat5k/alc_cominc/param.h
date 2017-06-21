/* $Id: param.h,v 1.1.4.2 1996/06/18 16:44:37 sakumar Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/alc_cominc/param.h,v $
 *-----------------------------------------------------------------
 * param.h -- Defines operational parameters
 *
 * Ratinder P. S. Ahuja
 *
 * Copyright (c) 1986-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: param.h,v $
 * Revision 1.1.4.2  1996/06/18  16:44:37  sakumar
 * CSCdi60668:  PVC (RFC1483) support needs to be added to Cat5K ATM Module
 * Branch: California_branch
 *
 * Revision 1.1.4.1  1996/05/09  14:54:36  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.1  1996/04/27  06:14:01  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:38:45  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.1  1996/03/22  09:10:04  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.1  1996/03/13  07:41:01  rlowe
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
 * param.h -- Defines operational parameters
 *
 *------------------------------------------------------------------
 *
 *  FILE:  param.h
 *
 *	CONTENTS:
 *
 *	TAB STOP = 4
 *	Defines operational parameters
 *
 *	OWNER:
 *
 *		Ratinder P. S. Ahuja
 *
*/

#ifndef _PARAM_H_
#define _PARAM_H_

/*
**	BIGA start
*/

#define	BIGA_REGBASE_ADDRESS	0xb8000000		/* for 32 bit accesses		*/
#define	BIGA_H_BASE  			0x88000000  	/* Short 16 accesses		*/

#define VCI_BITS_C			10
#define VPI_BITS_C			0
#define VCI_MASK_C			((1<<VCI_BITS_C) -1)

#define MAX_PORT			1

#define MAX_DEFAULT_VPC     (1 << (VPI_BITS_C+VCI_BITS_C))
#define MAX_DEFAULT_VPC_C   MAX_DEFAULT_VPC
#define MAX_COLOR			1024			
#define MAX_DEFAULT_SYHDRS  MAX_DEFAULT_VPC /* default pool size            */
#define NUM_RX_CMD_BUFS     16
#define NUM_RXSAR_ACP_BUFS  32          /* number of buffers in rx sar to   */
                                        /* acp data queue                   */
#define NUM_RXSAR_BIGA_BUFS 256			/* number of buffers for inbound data*/
#define NUM_BIGA_TDS        (2*NUM_RXSAR_BIGA_BUFS)  /* number of biga tds 	*/

#define NUM_ACP_TX_DATA_BUFS 32
#define	NUM_TX_CMD_BUFS		NUM_ACP_TX_DATA_BUFS

#define RX_BUFSZ        	1584		/* MUST BE A MULTIPLE OF 48 		*/
#define RX_BUFSZ_C			RX_BUFSZ

#define NUM_BIGA_RDS        256     	/* default number of biga rds       */
#define	HOLDQ_SIZE_C		4			/* number of buffers in holdq		*/
#define TXSAR_ACPQ_SIZE     NUM_BIGA_RDS
#define ACP_SAR_CMDQ_SIZE   NUM_CMD_BUFS
#define HQ_DEFAULT_SIZE     NUM_BIGA_RDS
#define RX_TXSARQ_SIZE      256
#define ACP_RX_BUFSZ        RX_BUFSZ	/* MUST BE A MULTIPLE OF 48 		*/
#define ACP_TX_BUFSZ        1584		/* MUST BE A MULTIPLE OF 48 		*/
#define BIGA_TX_BUFSZ       RX_BUFSZ	/* MUST BE A MULTIPLE OF 48 		*/
#define CMD_BUFSZ           64
#define CAM_TABLE_SIZE      1024

#define	MAX_ENENT_PACKET	1514
#define	SYHDR_SIZE			12
#define	SYHDR_SIZE_C		SYHDR_SIZE
#define	LEC_HDRSZ_C			2			/* lec header size = 2 bytes		*/

/* Defines for ACP->NMP IPC (via RXSAR and BIGA) */
#define NUM_ACP_NMP_IPC_BUFS	16
#define ACP_NMP_IPC_BUFSZ		RX_BUFSZ

/* Defines for NMP->ACP IPC (via BIGA and TXSAR) */
#define NUM_TX_ACP_EVENT_BUFS	16		/* Should be much less than 		*/
										/* NUM_BIGA_RDS						*/


/*
**	SAR defines
*/

/*--- I/O registers ---*/
#define IOBASE  	0xA0E80000
 
#define STSREG0 		IOBASE
#define LED1_OFFSET    0x40
#define LED2_OFFSET    0x44
#define LED3_OFFSET    0x48
#define LED4_OFFSET    0x4C
 

#endif /* _PARAM_H_ */
