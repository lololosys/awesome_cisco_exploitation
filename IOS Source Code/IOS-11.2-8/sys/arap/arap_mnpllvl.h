/* $Id: arap_mnpllvl.h,v 3.2 1995/11/17 08:38:50 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/arap/arap_mnpllvl.h,v $
 *------------------------------------------------------------------
 * Header File for llvl.c
 *
 * October 1993, Tim Kolar
 *
 * Copyright (c) 1993-1994 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: arap_mnpllvl.h,v $
 * Revision 3.2  1995/11/17  08:38:50  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  10:53:45  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:04:55  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define STRM_WNDW_SZ	8	/* window size in stream mode */
#define STRM_DATA_SZ	64	/* max data size in stream mode */
/* 62 bytes of data */
/* 2 bytes reserved */
#define STRM_FDAT_SZ	62

#define BLK_DATA_SZ 	260	/* max data size in block mode */

#define MAX_LPDU_SZ	40

#define LCL_SCLASS	2	/* local svc class */
/* async, full-duplex, BSC-like framing */

#define LR_TMR		10	/* value for LR retran timer */

#define LR_TRAN_CNT	10	/* number of times to transmit an LR */
#define LR_RETRAN_CNT	(LR_TRAN_CNT - 1)	/* times to REtransmit an LR */
#define LA_WAIT_CNT	10	/* number of times to wait for an LA */
#define RET_LIMIT	12	/* limit in retransmitting an LT */

/* LR parameters bit definition ((mnpcb->lcb)->lr_parm) */
#define LR_SRV_CLASS	1	/* lr received contains service class */
#define LR_WNDW_SZ	2	/* lr received contains window size */
#define LR_DATA_SZ	4	/* lr received contains max data size */
#define LR_SHORT_MODE	8	/* lr received contains short mode */
#define LR_COMPRESSION	16	/* lr received contains compression */

#define LR_ILEN 	20	/* length of an initiator's LR */
#define LR_ALEN 	10	/* basic length of an acceptor's LR - no
				 * parms 2, 3 & 4 yet */
#define LD_LEN		4
#define LA_LEN		7
#define LN_LEN		7
#define LNA_LEN 	4
#define LTH_LEN 	4

/* Link state variables */
#ifdef IDLE
#undef IDLE
#endif				/* IDLE */
#define IDLE			0
#define LR_RESP_WAIT		1
#define PARMS_NEGO		2
#define LR_WAIT 		3
#define CONNECT_REQ_WAIT	4
#define LA_WAIT 		5
#define LNK_CONNECTED		6
#define LNK_ERROR		7

#define PROTOCOL_ERR		1
#define PROT_LVL_MISMATCH	2
#define BAD_LR_PARMS		3
#define RETRAN_TMR_EXP		4
#define INACT_TMR_EXP		5

#define L_ACCEPTOR		mode

#define SETBIT1(bit)	mnp->lcb.status_1 |= bit	/* used to set bit */
#define SETBIT2(bit)	mnp->lcb.status_2 |= bit
#define SETBIT3(bit)	mnp->lcb.status_3 |= bit

#define CLRBIT1(bit)	mnp->lcb.status_1 &= ~(bit)	/* used to clear bit */
#define CLRBIT2(bit)	mnp->lcb.status_2 &= ~(bit)
#define CLRBIT3(bit)	mnp->lcb.status_3 &= ~(bit)

#define BIT1SET(bit)	(mnp->lcb.status_1 & bit)	/* used to test bit */
#define BIT2SET(bit)	(mnp->lcb.status_2 & bit)
#define BIT3SET(bit)	(mnp->lcb.status_3 & bit)

#define WAIT		1
#define NOWAIT		0
#define RF_INIT 	0
#define SF_INIT 	0
