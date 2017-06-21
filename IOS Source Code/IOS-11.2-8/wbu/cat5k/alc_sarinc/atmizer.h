/* $Id: atmizer.h,v 1.1.4.1 1996/05/09 15:00:04 rbadri Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/alc_sarinc/atmizer.h,v $
 *-----------------------------------------------------------------
 * atmizer.h --  
 *
 * ?
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: atmizer.h,v $
 * Revision 1.1.4.1  1996/05/09  15:00:04  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.1  1996/04/27  06:16:00  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:41:10  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.1  1996/03/22  09:12:09  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.1  1996/03/13  02:37:39  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1  1996/03/11  07:44:16  rlowe
 * Placeholders for California.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef _ATMIZER_
#define _ATMIZER_

/*
 * definitions for LSI Logic's ATMizer
 */

/* registers */
#define VCR_BASE    0x00000000	/* VCR ram */
#define M_CREDIT    0xfff04000	/* channel group credit register 	*/
#define M_CONFIG    0xfff04100	/* configuration register 		*/
#define M_STALL	    0xfff04200	/* stall register 			*/
#define M_PRPC      0xfff04300	/* peak rate pacing counter 		*/
#define M_RXBUFPTR  0xfff04400	/* current rx cell address 		*/
#define M_TXFIFO    0xfff04500	/* transmit address fifo 		*/
#define M_RXACK	    0xfff0460c	/* receive cell indicator 		*/
#define M_GPRR      0xfff04700	/* global pacing rate register 		*/
#define M_SCR	    0xfff04a00	/* system control register 		*/
#define M_AHI	    0xfff04b00	/* assert host interrupt 		*/
#define M_CRC32	    0xfff04c00	/* CRC32 partial result 		*/
#define M_ASUBR     0xfff04d00	/* address substitution register	*/
#define M_DMACR	    0x00400000	/* DMA control register 		*/

/* PRPC */
#define PRPC_RRSHFT 2
#define PRPC_RRMASK (7<<2)
#define PRPC_I  (1<<6)

/* CGCR */
#define CGCR_BBMASK (7<<2)
#define CGCR_BBSHFT 2

/* M_SCR system control register */
#define SCR_TAFMASK 7		/* tx addr fifo */
#define SCR_TAFSHFT 0
#define SCR_AWD	    (1<<4)	/* automatic watch-dog time-out */
#define SCR_RWD	    (1<<5)	/* regular watch-dog time-out */
#define SCR_BOCMASK (3<<6)
#define SCR_BOCSHFT 6
#define SCR_CHMASK  (0x3f<<8)
#define SCR_CHSHFT  8
#define SCR_SAFE    (1<<14)
#define SCR_CRCERR  (1<<15)
#define SCR_BSSHFT  16
#define SCR_BSMASK  (7<<16)	/* buffer size */
#define SCR_BS4     (0<<16)
#define SCR_BS8     (1<<16)
#define SCR_BS16    (2<<16)
#define SCR_BS32   (3<<16)	/* rx buffer size 32 cells */
#define SCR_CBSMASK (3<<19)	/* cache block size */
#define SCR_CBS2    (0<<19)
#define SCR_CBS4    (1<<19)
#define SCR_HHMASK  (7<<21)
#define SCR_HHSHFT  21
#define SCR_HH0	    (1<<21)
#define SCR_HH1	    (1<<22)
#define SCR_HH2	    (1<<23)
#define SCR_BM	    (1<<24)
#define SCR_TI	    (1<<26)	/* tx init */
#define SCR_RI      (1<<27)	/* receive init */
#define SCR_TOMASK  (3<<28)	/* tx offset */
#define SCR_TOSHFT  28
#define SCR_ROMASK  (3<<30)	/* rx offset */
#define SCR_ROSHFT  30
#define SCR_TO52   (3<<28)	/* tx cell size 52 bytes */
#define SCR_RO52   (3<<30)	/* rx cell size 52 bytes */
#define SCR_TXHEC  (1<<21)	/* generate HEC on tx */
#define SCR_RXHEC  (1<<22)	/* expect HEC on receive */
#define SCR_CHKHEC (1<<23)	/* check HEC on receive */
#define SCR_RXINIT (1<<27)	/* enable receiver */


/* address modifiers when writing to M_TAF */
#define A_CRC10	    (1<<2)	/* generate CRC10 value 		p39 */

/* address modifiers when writing to M_DMA 			  p28 & p52 */
#define DMA_LOMASK  (3<<30)	/* local offset register (VCR) */
#define DMA_LOSHFT  30	
#define DMA_BCMASK  (0x3f<<24)	/* transfer length counter */
#define DMA_BCSHFT  24
#define DMA_RD	    (1<<14)	/* read (versus write) */
#define DMA_WR	    0		/* rite (default) */
#define DMA_G	    (1<<13)	/* ghost write */
#define DMA_BCSHFT  24
#define DMA_LAMASK  (0x3ff<<2)	/* local address counter (VCR) */
#define DMA_LASHFT  2

/* RT stuff								p28 */
#define RT_MARMASK  (0xff<<24)	/* memory address register */
#define RT_MARSHFT  24
#define RT_MACMASK  (0x3fffff<<2) /* memory address counter */
#define RT_MACSHFT  2
#define RT_MORMASK  (3<<0)	/* memory (byte) offset register */
#define RT_MORSHFT  0

/* SP_AD 								p45 */
#define SPAD_BEMASK (0xf<<28)	/* byte enables */
#define SPAD_AT	    (1<<26)	/* access type */
#define SPAD_BF     (1<<25)	/* block fetch */
#define SPAD_ATOM   (1<<24)	/* atomic */

#endif /* _ATM_ */
