/* $Id: sar_hw.h,v 1.1.4.1 1996/05/09 15:01:37 rbadri Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/src-3k-sar/sar_hw.h,v $
 *-----------------------------------------------------------------
 * sar_hw.h --  SAR hardware defines
 *
 * November 1994, rbau
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: sar_hw.h,v $
 * Revision 1.1.4.1  1996/05/09  15:01:37  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.1  1996/04/27  06:17:33  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:43:02  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.1  1996/03/22  09:13:44  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.1  1996/03/13  02:38:37  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1  1996/03/11  07:44:38  rlowe
 * Placeholders for California.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*********************************************************************
 * sar_hw.h
 *   SAR hardware defines
 *
 * Date: 11/07/94  rbau
 *********************************************************************
 */
#ifndef _SAR_HW_H_
#define _SAR_HW_H_

/*
 * definitions for LSI Logic's ATMizer
 */
#define RAM_START_HI            0xA0C0
#define RAM_END_HI              0xA0C1
#define FRAME_BUF_START_HI      0xA080    /* min. 1M byte */
#define FRAME_BUF_END_HI        0xA08F

#define IRAM_SIZE		0x1000    /* 4 k bytes */
#define SP_RAM_SIZE		0x20000   /* 128K bytes */
#define FRAME_BUF_SIZE		0x100000  /* 1M bytes */

#define HOST_INTR_REG_BASE  0xFFF04B00
#define BIU_IRAM_WR     0x00034800
                        /*
                        DS=0 => Disable D-Cache
                        IS1=1 => Enable I-Cache Set 1
                        IBLKSZ=0 => i-cache block fill size 2 words
                        INTP=0, NOPAD=0, BGNT=1 =>
                        INT[5:0]activeLow,WaitState,EnableBusGrant
                        LDSCH=1 => Enable load scheduling
                        NOSTR=1 => Enable instruction streaming.
                        */

/*--- I/O registers ---*/
#define IOBASE  0xA0E80000

#ifdef  LANGUAGE_C
#define SIOBASE ((volatile int *)0xA0F00000)
#endif  /* LANGUAGE_C */

#define STSREG0 IOBASE
#define LED1_OFFSET    0x40
#define LED2_OFFSET    0x44
#define LED3_OFFSET    0x48
#define LED4_OFFSET    0x4C


/*--- bit defines for Status Register 0 ---*/
#define CAM_MATCH_HW  0x01
#define CAM_FULL      0x02
#define CAM_DONE      0x04
#define UART_EXIST    0x08

#define STSREG3 (IOBASE + 0x60)


#endif /* _SAR_HW_H_ */
